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

struct Node
{
    unsigned char *data;
    struct Node *next;
};

// Each node takes 64 bytes (=L2 cacheline size) or 256 bytes
// such that 1) they can be flushed independently and 2)
// the node addresses do not stride - only the pointers to data.
struct Node *createNode(unsigned char *data)
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
struct Node *insertAtEnd(struct Node **head, unsigned char *data)
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
    if (argc != 7)
    {
        printf("Usage: %s CORE LL_SIZE STRIDE MULs flushLastNode flushPredAddr\n", argv[0]);
        return EXIT_FAILURE;
    }

    srand((unsigned int)time(NULL));
    const int core_id = atoi(argv[1]);

    // Linked-list length and stride are now runtime parameters.
    const int LL_SIZE = atoi(argv[2]);
    const int STRIDE = atoi(argv[3]);
    const int PAGE_SZ = 4096;
    const int L2_LINE_SZ = 64;
    const int MULS = atoi(argv[4]);

    // Furthermore, we vary two parameters: whether the last linked
    // list node is cached or flushed, and whether the LAP's predicted
    // address is cached or flushed.
    const int FLUSH_LAST_NODE = atoi(argv[5]);
    const int FLUSH_PRED_ADDR = atoi(argv[6]);

    // How many cycles (incl. barrier instructions)
    // should we count as a cache hit?
    const int CACHE_HIT_THRESHOLD = 150;

    // This is for sizing the buffer where the striding
    // memory accesses load from.
    const int NUM_PAGES_BUF = 1 + (LL_SIZE * abs(STRIDE)) / PAGE_SZ;

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
        // Account for negative striding
        unsigned char *ptr;
        if (STRIDE >= 0)
        {
            ptr = (unsigned char *)buffer + i * STRIDE;
        }
        else
        {
            unsigned char *buffer_end = (unsigned char *)buffer + NUM_PAGES_BUF * PAGE_SZ - 8;
            ptr = buffer_end + i * STRIDE;
        }

        if (i == LL_SIZE - 1)
        {
            // For the last node: we will change the value to 0x77, so that
            // transient accesses to this address will result in a different
            // value being transmitted over Flush+Reload.
            *ptr = 0x77;

            // Then, decrement the pointer (for positive stride)
            // or increment it (negative stride), making
            // the new pointer disobey the stride and
            // point to a randomized dummy value instead.
            ptr = ptr - 5 * STRIDE;
            lastNode = insertAtEnd(&ll, ptr);
        }
        else
        {
            // All architectural accesses from the linked list traversal should
            // lead to 0xff being sent over Flush+Reload.
            *ptr = 0xff;
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
        // Flush only probe indices we care about.
        serialized_flush((void *)(channel_ptr + SECRET_IDX * PAGE_SZ));
        serialized_flush((void *)(channel_ptr + ARCH_IDX * PAGE_SZ));

        // Traverse the linked list, chasing the pointers.
        // Even with -O3, need the first two vars to be declared
        // with the register keyword to prevent extra loads/stores to
        // stack. Trash MUST be declared volatile, or else the compiler
        // optimizes out this entire loop.
        for (int i = 0; i < TRIALS; ++i)
        {
            // Change the cache state of the last node and predicted address.
            if (FLUSH_LAST_NODE)
            {
                serialized_flush((void *)lastNode);
            }
            else
            {
                *(volatile char *)lastNode;
            }

            if (FLUSH_PRED_ADDR)
            {
                serialized_flush((void *)(lastNode->data + 5 * STRIDE));
            }
            else
            {
                *(volatile char *)(lastNode->data + 5 * STRIDE);
            }

            struct Node *head = ll;
            register int junk = 0;
            while (head != NULL)
            {
                register unsigned char *int_ptr = (unsigned char *)(head->data);
                register unsigned char lap_load = *int_ptr;

                // Experiment for speculation window: between the striding load
                // and F+R transmit, add junk MUL instructions (3 cycles each) to
                // measure the window length.
                if (lap_load != 0xff)
                {
#pragma GCC unroll 300
                    for (int i = 0; i < MULS; ++i)
                    {
                        asm volatile("mul %w0, %w1, %w2" : "=r"(junk) : "r"(junk), "r"(junk));
                    }
                }
                volatile unsigned char trash = channel_ptr[lap_load * PAGE_SZ];
                head = head->next;
            }
        }

        // Measure Flush+Reload, checking for both the
        // speculative and architectural loads.
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
    printf("Linked list length: %d\n", LL_SIZE);
    printf("Stride between loads: %d\n", STRIDE);
    printf("MULs in speculation window: %d\n", MULS);
    printf("%d/%d transient, %d/%d architectural\n\n", successes, MEASUREMENTS, sanity_check, MEASUREMENTS);

    // Clean up allocations and linked list.
    if (munmap(buffer, NUM_PAGES_BUF * PAGE_SZ) == -1)
    {
        printf("Failed to deallocate buffer pages\n");
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
