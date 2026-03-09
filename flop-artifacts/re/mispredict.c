#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include "pmu_android.h"
#include <time.h>
#include <unistd.h>

#define PAGE_SZ 16384
#define CACHE_LINE_SZ 64
#define NUM_CACHELINES PAGE_SZ / CACHE_LINE_SZ
#define CACHE_HIT_THRESHOLD 100
#define REPS 250
#define BUSY_WAIT_US 100 * 1000

#define READ(addr) (*(volatile uint32_t *)(addr))
#define FORCE_READ(addr, trash) (READ((uintptr_t)(addr) | (trash == 0xbaaaaad)))

// Note: must serialize with isb and dsb ish
inline __attribute__((always_inline)) void clflush(void *ptr)
{
    serialized_flush(ptr);
}

// For shuffling cacheline indices in an array as to randomize
// the memory access pattern.
void shuffle(volatile int *array, volatile int n)
{
    if (n > 1)
    {
        for (int i = 0; i < n - 1; i++)
        {
            volatile int j = i + rand() / (RAND_MAX / (n - i) + 1);
            volatile int temp = array[j];
            array[j] = array[i];
            array[i] = temp;
        }
    }
}

// Isolated critical section where LVP training happens
// into a function that never gets inlined.
__attribute__((noinline)) void critical_section(void *page, volatile int *indices, volatile unsigned char *channel_ptr, int iters)
{
    register uint64_t trash = 0;
    for (int i = 0; i < iters; i++)
    {
        volatile int idx = indices[i % NUM_CACHELINES];
        trash = FORCE_READ((volatile char *)page + idx * CACHE_LINE_SZ, trash);

        /* Uncomment this code to measure the speculation window with
        instructions between the predicted load and the covert channel transmission. */
        // register uint64_t one = 0x1;
        // #pragma GCC unroll 50
        // for (int i = 0; i < 50; ++i)
        // {
        //     asm volatile("mul %0, %1, %2" : "=r"(trash) : "r"(trash), "r"(one));
        // }

        // Transmit load value over covert channel.
        volatile unsigned char junk = channel_ptr[(uint8_t)trash * PAGE_SZ];
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s CORE\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Initialize RNG for random page access.
    srand(time(NULL));
    const int core_id = atoi(argv[1]);
    pin_to_core_android(core_id);
    try_enable_pmu_for_this_process();

    // Allocate one test page and memset it
    // such that load values become constant.
    void *page = mmap(NULL, PAGE_SZ, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (page == MAP_FAILED)
    {
        printf("Failed to allocate page\n");
        return EXIT_FAILURE;
    }
    else
    {
        memset(page, 0x41, PAGE_SZ);
    }

    // Make array to hold the cacheline indices, then
    // shuffle the order of accesses.
    volatile int indices[NUM_CACHELINES];
    for (int i = 0; i < NUM_CACHELINES; i++)
    {
        indices[i] = i;
    }
    shuffle(indices, NUM_CACHELINES);

    // Allocate the cache channel.
    void *channel_pages = mmap(NULL, 256 * PAGE_SZ, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    unsigned char *channel_ptr;

    if (channel_pages == MAP_FAILED)
    {
        printf("Failed to allocate cache channel pages\n");
        return EXIT_FAILURE;
    }
    else
    {
        memset(channel_pages, 0x99, 256 * PAGE_SZ);
        channel_ptr = (unsigned char *)channel_pages;
    }

    // First, run the critical section several times for training.
    critical_section(page, indices, channel_ptr, REPS);

    // Flush the cache channel array.
    for (int set = 0; set < 256; ++set)
    {
        clflush((void *)(channel_ptr + set * PAGE_SZ));
    }

    // Change the ground truth load value.
    memset(page, 0x77, PAGE_SZ);

    // Flush the page's cachelines.
    for (int i = 0; i < NUM_CACHELINES; i++)
    {
        clflush((void *)((volatile char *)page + i * CACHE_LINE_SZ));
    }

    // Serialize the flush operations.
    asm volatile("isb");
    asm volatile("dsb ish");

    /* Uncomment this section to test for state persistence
    with busy-waiting between training and activation. */
    // struct timespec start, end;
    // uint64_t elapsed_us;
    // clock_gettime(CLOCK_MONOTONIC, &start);
    // while (1)
    // {
    //     clock_gettime(CLOCK_MONOTONIC, &end);
    //     elapsed_us = (end.tv_sec - start.tv_sec) * 1000000LL + (end.tv_nsec - start.tv_nsec) / 1000LL;

    //     if (elapsed_us >= BUSY_WAIT_US)
    //     {
    //         break;
    //     }
    // }

    // Load the same PC again, activating the LAP.
    critical_section(page, indices, channel_ptr, 1);

    // Measure only selected probe indices.
    const int PROBE_A = 0x77;
    const int PROBE_B = 0x41;
    

    uint64_t start = rdtsc();
    volatile unsigned char trash_a = channel_ptr[PROBE_A * PAGE_SZ];
    uint64_t end = rdtsc();
    uint64_t timing_a = end - start;

    start = rdtsc();
    volatile unsigned char trash_b = channel_ptr[PROBE_B * PAGE_SZ];
    end = rdtsc();
    uint64_t timing_b = end - start;

    
        printf("~0x%02x: %llu\n", PROBE_A, (unsigned long long)timing_a);
    
    
        printf("~0x%02x: %llu\n", PROBE_B, (unsigned long long)timing_b);
    

    // Clean up allocations.
    if (munmap(page, PAGE_SZ) == -1)
    {
        printf("Failed to deallocate page\n");
        return EXIT_FAILURE;
    }
    if (munmap(channel_pages, 256 * PAGE_SZ) == -1)
    {
        printf("Failed to deallocate cache channel pages\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
