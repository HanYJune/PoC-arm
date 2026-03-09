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

// This time, each node has a double-pointer.
struct Node
{
    unsigned char **data;
    struct Node *next;
};

// Each node takes 64 bytes (=L2 cacheline size) or 256 bytes
// such that 1) they can be flushed independently and 2)
// the node addresses do not stride - only the pointers to data.
struct Node *createNode(unsigned char **data)
{
    const int L2_LINE_SZ = 64;
    struct Node *newNode = (struct Node *)malloc(L2_LINE_SZ * getRandomNumber(1, 2));
    if (newNode == NULL)
    {
        printf("Memory allocation for Node struct failed\n");
        exit(EXIT_FAILURE);
    }
    newNode->data = data;
    newNode->next = NULL;
    return newNode;
}

// insertAtEnd returns a pointer to the newly
// created node, such that we can flush the last node
// when measuring for speculation.
struct Node *insertAtEnd(struct Node **head, unsigned char **data)
{
    struct Node *newNode = createNode(data);
    if (*head == NULL)
    {
        *head = newNode;
    }
    else
    {
        struct Node *current = *head;
        while (current->next != NULL)
        {
            current = current->next;
        }
        current->next = newNode;
    }
    return newNode;
}

void freeList(struct Node *head)
{
    while (head != NULL)
    {
        struct Node *temp = head;
        head = head->next;
        free(temp);
    }
}

// Invocation is commented out, but this is for debugging purposes.
void printList(struct Node *head)
{
    printf("Head Node\n");
    while (head != NULL)
    {
        printf("(Addr: %p, Data: %p) ->\n", head, head->data);
        head = head->next;
    }
    printf("Tail Node\n");
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s CORE\n", argv[0]);
        return EXIT_FAILURE;
    }

    srand((unsigned int)time(NULL));
    const int core_id = atoi(argv[1]);

    // Constants for memory allocations. Linked list length
    // and stride are fixed to optimal parameters of
    // 1000 training loads, 32 bytes apart.
    const int LL_SIZE = 1000;
    const int STRIDE = 32;
    const int PAGE_SZ = 4096;
    const int L2_LINE_SZ = 64;

    // How many cycles (incl. barrier instructions)
    // should we count as a cache hit?
    const int CACHE_HIT_THRESHOLD = 150;

    // This is for sizing the buffer where the striding
    // memory accesses load from.
    const int NUM_PAGES_BUF = 1 + (LL_SIZE * STRIDE) / PAGE_SZ;

    // Make the dummy and secret pages 10*16KiB wide, so that the
    // LAP definitely can't reach it with the 255B stride limit.
    // Also, for more certainty in our signal, access different page offsets.
    const int NUM_PAGES_OTH = 10;
    const int DUMMY_OFFSET = 0x3210;
    const int SECRET_OFFSET = 0x1234;

    // How many linked list traversals should be performed
    // between each Flush+Reload measurement?
    const int TRIALS = 20;

    // When measuring activation rate, how many runs
    // should be performed?
    const int MEASUREMENTS = 1000;

    pin_to_core_android(core_id);
    try_enable_pmu_for_this_process();

    // Allocate buffer pages.
    void *buffer = mmap(NULL, NUM_PAGES_BUF * PAGE_SZ, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (buffer == MAP_FAILED)
    {
        printf("Failed to allocate buffer pages\n");
        return EXIT_FAILURE;
    }
    else
    {
        memset(buffer, 0x41, NUM_PAGES_BUF * PAGE_SZ);
        printf("Allocating %d buffer pages starting at %p\n", NUM_PAGES_BUF, buffer);
    }

    // Allocate dummy pages. Mispredicted pointer points to secret page,
    // while all other pointers (deref'd architecturally) will point
    // to the dummy pages. Using different page offsets and cache sets to
    // increase certainty in results.
    void *dummy_pages = mmap(NULL, NUM_PAGES_OTH * PAGE_SZ, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    unsigned char *dummy_ptr;

    if (dummy_pages == MAP_FAILED)
    {
        printf("Failed to allocate dummy pages\n");
        return EXIT_FAILURE;
    }
    else
    {
        memset(dummy_pages, 0xff, NUM_PAGES_OTH * PAGE_SZ);
        dummy_ptr = (unsigned char *)dummy_pages + DUMMY_OFFSET;
        printf("Dummy pointer at %p\n", dummy_ptr);
    }

    // Allocate secret pages.
    void *secret_pages = mmap(NULL, NUM_PAGES_OTH * PAGE_SZ, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    unsigned char *secret_ptr;

    if (secret_pages == MAP_FAILED)
    {
        printf("Failed to allocate secret pages\n");
        return EXIT_FAILURE;
    }
    else
    {
        memset(secret_pages, 0x77, NUM_PAGES_OTH * PAGE_SZ);
        secret_ptr = (unsigned char *)secret_pages + SECRET_OFFSET;
        printf("Secret pointer at %p\n", secret_ptr);
    }

    // Allocate the reload buffer for Flush+Reload.
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

    // Initialize the linked list, but keep a pointer to the last node,
    // since we will flush it to force speculation.
    struct Node *ll = NULL;
    struct Node *lastNode = NULL;

    for (int i = 0; i < LL_SIZE; ++i)
    {
        unsigned char *temp = (unsigned char *)buffer + i * STRIDE;
        unsigned char **ptr = (unsigned char **)temp;

        if (i == LL_SIZE - 1)
        {
            // First, write secret pointer to predicted
            // striding load address.
            *ptr = secret_ptr;

            // Then, decrement the pointer, making
            // the new pointer disobey the stride and
            // point to a randomized dummy address instead.
            ptr = (unsigned char **)(temp - 5 * STRIDE);
            lastNode = insertAtEnd(&ll, ptr);
        }
        else
        {
            // All architectural accesses from the linked list traversal should
            // lead to 0xff (dummy data) being sent over Flush+Reload.
            *ptr = dummy_ptr + getRandomNumber(0, 32);
            insertAtEnd(&ll, ptr);
        }
    }

    // Uncomment for debugging!
    // printList(ll);

    // We'll traverse the linked list, and see how many times
    // we receive 0x77 (which is NOT supposed to be accessed!)
    // as well as 0xff (which is accessed architecturally).
    int successes = 0;
    int sanity_check = 0;
    const int SECRET_IDX = 0x77;
    const int ARCH_IDX = 0xff;

    for (int i = 0; i < MEASUREMENTS; ++i)
    {
        // Page in the secret pointer.
        *(volatile unsigned char *)secret_ptr;

        // Flush only probe indices we care about.
        serialized_flush((void *)(channel_ptr + SECRET_IDX * PAGE_SZ));
        serialized_flush((void *)(channel_ptr + ARCH_IDX * PAGE_SZ));

        // Traverse the linked list, chasing the pointers.
        // Even with -O3, need the first three vars to be declared
        // with the register keyword to prevent extra loads/stores to
        // stack. Trash MUST be declared volatile, or else the compiler
        // optimizes out this entire loop.
        for (int i = 0; i < TRIALS; ++i)
        {
            // Flush the last node to force LAP to speculate.
            serialized_flush((void *)lastNode);

            // Cache the predicted load address.
            *(volatile char *)(lastNode->data + 5 * STRIDE);

            struct Node *head = ll;
            while (head != NULL)
            {
                register unsigned char **int_ptr = (unsigned char **)(head->data);
                register unsigned char *lap_load = *int_ptr;
                register unsigned char secret = *lap_load;
                volatile unsigned char trash = channel_ptr[secret * PAGE_SZ];
                head = head->next;
            }
        }

        // Measure Flush+Reload for the two probe indices.
        uint64_t start = rdtsc();
        volatile unsigned char trash_secret = channel_ptr[SECRET_IDX * PAGE_SZ];
        uint64_t end = rdtsc();
        uint64_t timing_secret = end - start;

        start = rdtsc();
        volatile unsigned char trash_arch = channel_ptr[ARCH_IDX * PAGE_SZ];
        end = rdtsc();
        uint64_t timing_arch = end - start;

        if (timing_secret < CACHE_HIT_THRESHOLD)
        {
            ++successes;
        }
        if (timing_arch < CACHE_HIT_THRESHOLD)
        {
            ++sanity_check;
        }

        // Uncomment to read all measurements.
        // for (int j = 0; j < 256; ++j)
        // {
        //     if (timings[j] < CACHE_HIT_THRESHOLD)
        //     {
        //         printf("0x%02x: %llu\n", j, timings[j]);
        //     }
        // }
    }

    printf("%d/%d secret value received, %d/%d dummy value received\n\n", successes, MEASUREMENTS, sanity_check, MEASUREMENTS);

    // Clean up allocations and linked list.
    if (munmap(buffer, NUM_PAGES_BUF * PAGE_SZ) == -1)
    {
        printf("Failed to deallocate buffer pages\n");
        return EXIT_FAILURE;
    }
    if (munmap(dummy_pages, NUM_PAGES_OTH * PAGE_SZ) == -1)
    {
        printf("Failed to deallocate dummy pages\n");
        return EXIT_FAILURE;
    }
    if (munmap(secret_pages, NUM_PAGES_OTH * PAGE_SZ) == -1)
    {
        printf("Failed to deallocate secret pages\n");
        return EXIT_FAILURE;
    }
    if (munmap(channel_pages, 256 * PAGE_SZ) == -1)
    {
        printf("Failed to deallocate cache channel pages\n");
        return EXIT_FAILURE;
    }
    freeList(ll);

    return EXIT_SUCCESS;
}
