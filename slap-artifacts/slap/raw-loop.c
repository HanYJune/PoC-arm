#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include "pmu_android.h"

int getRandomNumber(int min, int max)
{
    return min + rand() % (max - min + 1);
}

// Used for the RA+CV experiment.
inline __attribute__((always_inline)) int min(int a, int b)
{
    return (a < b) ? a : b;
}

// Compare function for qsort
int compare(const void *a, const void *b)
{
    return (*(uint64_t *)a > *(uint64_t *)b) - (*(uint64_t *)a < *(uint64_t *)b);
}

double median(uint64_t arr[], int n)
{
    qsort(arr, n, sizeof(uint64_t), compare);
    if (n % 2 == 0)
    {
        return (double)(arr[n / 2 - 1] + arr[n / 2]) / 2.0;
    }
    else
    {
        return (double)arr[n / 2];
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s CORE ITERS\n", argv[0]);
        return EXIT_FAILURE;
    }
    const int core_id = atoi(argv[1]);
    const int ITERS = atoi(argv[2]);
    const int PAGE_SZ = 4096;

    pin_to_core_android(core_id);
    try_enable_pmu_for_this_process();
    const int REPS = 100;

    printf("Iters = %d\n", ITERS);
    srand((unsigned int)time(NULL));

    // We'll make each address stride by 8 * 4B per int = 32B, and
    // allocate a sufficiently big buffer for LAP training.
    const int STRIDE = 8;
    const int NUM_PAGES_BUF = 1 + (ITERS * STRIDE * sizeof(int)) / PAGE_SZ;

    void *buffer = mmap(NULL, NUM_PAGES_BUF * PAGE_SZ, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (buffer == MAP_FAILED)
    {
        printf("Failed to allocate buffer pages\n");
        return EXIT_FAILURE;
    }
    else
    {
        memset(buffer, 0x0, NUM_PAGES_BUF * PAGE_SZ);
    }

    // Define the max value we can write such that if it is used
    // as an index, the result won't be out of bounds.
    const int WRITE_MAX = NUM_PAGES_BUF * PAGE_SZ / sizeof(int);

    // To force serialization with RAW dependencies
    volatile register int junk;

    // Variables for measurement
    int *ptr = (int *)buffer;
    uint64_t timings[REPS];
    uint64_t t0, t1;

    // Test for random addresses + random values
    for (int i = 0; i < WRITE_MAX; ++i)
    {
        ptr[i] = getRandomNumber(0, WRITE_MAX);
    }

    // Dry run to cache addresses, disproving that speedup
    // is from prefetching
    junk = 0;
    for (int i = 0; i < ITERS; ++i)
    {
        junk = ptr[junk];
    }

    for (int i = 0; i < REPS; ++i)
    {
        junk = 0;
        t0 = rdtsc();
        for (int i = 0; i < ITERS; ++i)
        {
            junk = ptr[junk];
        }
        t1 = rdtsc();
        timings[i] = t1 - t0;
    }
    printf("Loop Random Addr + Random Value: %.2f\n", median(timings, REPS));

    // Unroll the above to test for PC-tagging
    junk = 0;
    for (int i = 0; i < ITERS; ++i)
    {
        junk = ptr[junk];
    }

    for (int i = 0; i < REPS; ++i)
    {
        junk = 0;
        t0 = rdtsc();
#pragma GCC unroll 10
        for (int i = 0; i < ITERS; ++i)
        {
            junk = ptr[junk];
        }
        t1 = rdtsc();
        timings[i] = t1 - t0;
    }
    printf("Unrolled Random Addr + Random Value: %.2f\n", median(timings, REPS));

    // Test for striding addresses + striding values
    int index = 0;
    while (index + STRIDE < WRITE_MAX)
    {
        ptr[index] = index + STRIDE;
        index += STRIDE;
    }

    junk = 0;
    for (int i = 0; i < ITERS; ++i)
    {
        junk = ptr[junk];
    }

    for (int i = 0; i < REPS; ++i)
    {
        junk = 0;
        t0 = rdtsc();
        for (int i = 0; i < ITERS; ++i)
        {
            junk = ptr[junk];
        }
        t1 = rdtsc();
        timings[i] = t1 - t0;
    }
    printf("Loop Striding Addr + Striding Value: %.2f\n", median(timings, REPS));

    // Again, unroll the above to test PC-tagging
    junk = 0;
    for (int i = 0; i < ITERS; ++i)
    {
        junk = ptr[junk];
    }

    for (int i = 0; i < REPS; ++i)
    {
        junk = 0;
        t0 = rdtsc();
#pragma GCC unroll 10
        for (int i = 0; i < ITERS; ++i)
        {
            junk = ptr[junk];
        }
        t1 = rdtsc();
        timings[i] = t1 - t0;
    }
    printf("Unrolled Striding Addr + Striding Value: %.2f\n", median(timings, REPS));

    /*
        Test for load address prediction: make trash the minimum
        of stride and load value such that load value is random,
        but load address strides.
    */
    for (int i = 0; i < WRITE_MAX; ++i)
    {
        ptr[i] = getRandomNumber(STRIDE, WRITE_MAX);
    }

    junk = 0;
    for (int i = 0; i < ITERS; ++i)
    {
        junk += min(ptr[junk], STRIDE);
    }

    for (int i = 0; i < REPS; ++i)
    {
        junk = 0;
        t0 = rdtsc();
        for (int i = 0; i < ITERS; ++i)
        {
            junk += min(ptr[junk], STRIDE);
        }
        t1 = rdtsc();
        timings[i] = t1 - t0;
    }
    printf("Loop Striding Addr + Random Value: %.2f\n\n", median(timings, REPS));

    // Clean up allocations.
    if (munmap(buffer, NUM_PAGES_BUF * PAGE_SZ) == -1)
    {
        printf("Failed to deallocate buffer pages\n");
        return EXIT_FAILURE;
    }

    return junk;
}
