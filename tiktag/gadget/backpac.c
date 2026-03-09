#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

#include "config.h"
#include "lib/aarch64.h"
#include "lib/scheduler.h"
#include "lib/timer.h"

#define PACDZA(x) __asm__ __volatile__("pacdza %0\n" : "+r"(x) :: "memory")
#define AUTDZA(x) __asm__ __volatile__("autdza %0\n" : "+r"(x) :: "memory")
#define XPACD(x) __asm__ __volatile__("xpacd %0\n" : "+r"(x) :: "memory")


#define CODE_INSTRUCTIONS ((size_t)0x400)


__attribute__((aligned(64)))
uint32_t code[CODE_INSTRUCTIONS];
uint32_t* code_ptr;

__attribute__((noinline))
void code_start(uint32_t nop_instruction){
    mprotect(code, sizeof(code), PROT_READ | PROT_WRITE);
    for(size_t i = 0; i < CODE_INSTRUCTIONS; i++){
        code[i] = nop_instruction;
    }
    code_ptr = code;
}

__attribute__((noinline))
void code_emit(uint32_t instruction){
    *code_ptr = instruction;
    code_ptr += 1;
}

__attribute__((noinline))
void code_emit_bl(void* target) {
    uintptr_t pc = (uintptr_t)code_ptr + 4;
    int64_t delta = ((int64_t)target - (int64_t)pc) >> 2;
    if (delta < -(1LL << 25) || delta >= (1LL << 25)) {
        fprintf(stderr, "code_emit_bl: target out of range\n");
        exit(1);
    }
    uint32_t imm26 = (uint32_t)(delta & 0x03ffffffu);
    code_emit(0x94000000u | imm26);
}

__attribute__((noinline))
void code_skip(size_t count) {
    code_ptr += count;
}

__attribute__((noinline))
void code_finish() {
  mprotect(code, sizeof(code), PROT_READ|PROT_EXEC);
  flush_instruction_cache(code, sizeof(code));
}

typedef void (*function)(void*, void*, void*, void*,uint64_t);
const function code_function = (void(*)(void*,void*,void*, void*, uint64_t))(code);



const uint32_t cbnz_x0_c    = 0xb5000060;
const uint32_t ldr_x0_x0    = 0xf9400000;
const uint32_t ldr_x0_x1    = 0xf9400020;
const uint32_t ldr_x4_x3    = 0xf9400064;
const uint32_t ldr_x1_x1    = 0xf9400021;
const uint32_t ldr_x2_x2    = 0xf9400042;
const uint32_t ldr_x2_x1    = 0xf9400022;
const uint32_t str_x2_x1    = 0xf9000022;
const uint32_t mov_x9_x0    = 0xaa000009;
const uint32_t mov_x10_x1   = 0xaa01002a;
const uint32_t mov_x11_x2   = 0xaa02004b;
const uint32_t mov_x12_x3   = 0xaa03006c;
const uint32_t mov_x13_x4   = 0xaa04008d;
const uint32_t mov_x15_x30  = 0xaa1e03cf;
const uint32_t mov_x0_x9    = 0xaa090120;
const uint32_t mov_x1_x10   = 0xaa0a0141;
const uint32_t mov_x2_x11   = 0xaa0b0162;
const uint32_t mov_x3_x12   = 0xaa0c0183;
const uint32_t mov_x4_x13   = 0xaa0d01a4;
const uint32_t mov_x30_x15  = 0xaa0f01fe;
const uint32_t orr_x1_x2_x1 = 0xaa010041;
const uint32_t ret          = 0xd65f03c0;
const uint32_t bkpt         = 0xd4200000;
const uint32_t aut_x1        = 0xdac13be1;
const uint32_t orr_x0_x0_x0 = 0xaa000000;

__attribute__((noinline))
void generate_code1(size_t nop_count){
    code_start(orr_x1_x2_x1);
    
   
    code_emit(ldr_x0_x0); // cond
    code_emit(cbnz_x0_c); // if(!cond)
    code_emit(ret);
    code_emit(bkpt);
    
    code_emit(aut_x1); // aut
    
    code_emit(ldr_x0_x1); // load
    
    
    code_skip(nop_count);
    
    code_emit(ldr_x2_x2);
    code_emit(ret);
    code_emit(bkpt);
    code_finish();
}

__attribute__((noinline))
void generate_code2(size_t nop_count){
    code_start(orr_x1_x2_x1);
    code_emit(ldr_x0_x0); // cond
    code_emit(cbnz_x0_c); // if(!cond)
    code_emit(ret);
    code_emit(bkpt);
    code_emit(aut_x1); // aut
    code_emit(str_x2_x1); // store ptr
    
    code_emit(ldr_x2_x1); // load ptr
    code_skip(nop_count);
    // for (size_t i = 0; i < nop_count; ++i) {
    //     code_emit(orr_x1_x2_x1); // nops that propagate data dependency on x1
    // }
    code_emit(ldr_x2_x2);
    code_emit(ret);
    code_emit(bkpt);
    code_finish();

}

__attribute__((noinline))
void generate_code3(size_t nop_count){
    code_start(orr_x1_x2_x1);

    code_emit(ldr_x0_x0); // cond
    code_emit(cbnz_x0_c); // if(!cond)
    code_emit(ret);
    code_emit(bkpt);
    code_emit(aut_x1); // aut
    code_emit(ldr_x0_x1); // load
    for (size_t i = 0; i < nop_count; ++i) {
        code_emit(orr_x0_x0_x0); // load from [x1] without clobbering x1
    }
    code_emit(ldr_x2_x2);
    code_emit(ret);
    code_emit(bkpt);
    code_finish();
}

__attribute__((noinline))
void* map_and_zero(size_t size, bool paced){
    int prot = PROT_WRITE | PROT_READ;
    uint64_t* ptr = (uint64_t*)mmap(NULL,size, prot, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
    memset(ptr,0,size);
    
    if(paced){
        PACDZA(ptr);
    }
    
    return ptr;
} 


uint64_t prng_state = 0;
uint32_t prng_counter = 0;

__attribute__((noinline))
uint32_t prng() {
  prng_state ^= prng_state >> 12;
  prng_state ^= prng_state << 25;
  prng_state ^= prng_state >> 27;
  prng_counter += 362437;
  // xorshift may have some patterns in the low bits, we don't need many bits of
  // randomness here anyway, so take the high bits only.
  return (prng_state + prng_counter) >> 32;
}

__attribute__((aligned(0x1000)))
void *buffer;

void run_tests(int cpu, size_t iterations, size_t start_count, size_t end_count){
    
    uint64_t* cond_ptr = (uint64_t*)(char*)map_and_zero(0x20000,false);
    uint64_t* channel_ptr = (uint64_t*)(char*)map_and_zero(0x20000,false);
    
    uint64_t* right_pac_ptr = (uint64_t*)(char*)map_and_zero(0x20000,true);

 
    for(size_t i = 0; i < iterations; i++){
        uint32_t random = prng();
        bool pass = random >> 31;
        //bool pass = 0;
        size_t nop_count = start_count + (((random << 1) >> 1) % (end_count - start_count));

        generate_code1(nop_count);
        
        uint64_t latency = 0;

        for(uint64_t j = 0; j < BRANCH_PREDICTOR_ITERATIONS; j++){
            uint64_t is_warmup = ((j + 1) ^ BRANCH_PREDICTOR_ITERATIONS) != 0;
            
            *cond_ptr = is_warmup ? 1 : 0;
            
            uintptr_t ptr_mask = 0xfffffffffffffffful
                     ^ (!pass * !is_warmup * 0xffff000000000000ul);
            uint64_t* test_ptr = (uint64_t*)((uintptr_t)right_pac_ptr & ptr_mask);
           
            

            // if (!is_warmup) {
            //     *cond_ptr = 1;
            //     printf("cond=%lu pass=%d nop=%zu test=%016lx right=%016lx\n",
            //            *cond_ptr, pass, nop_count,
            //            (uint64_t)test_ptr, (uint64_t)right_pac_ptr);  
            // }
            
            local_memory_barrier();
            flush_data_cache(cond_ptr);
            flush_data_cache(channel_ptr);
            local_memory_barrier();
            instruction_barrier();
            
            code_function(cond_ptr,test_ptr,channel_ptr,buffer,10);

            latency = read_latency(channel_ptr);
        }
        printf("%i,%i,%zu,%zu\n", cpu, pass, nop_count, latency);
    }
}

int main(int argc, char** argv) {
  if (argc != 6) {
    fprintf(stderr, "usage: speculation_window cpu_id seed iterations start_count end_count\n");
    exit(-1);
  }

  int cpu = atoi(argv[1]);
  size_t seed = atoi(argv[2]);
  size_t iterations = atoi(argv[3]);
  size_t start_count = atoi(argv[4]);
  size_t end_count = atoi(argv[5]);

  prng_state = seed;

  set_max_priority();
  start_timer();

  
  cpu_pin_to(cpu);
  buffer = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
  memset(buffer,0,4096);

  run_tests(cpu, iterations, start_count, end_count);
}
