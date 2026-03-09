#ifndef PMU_ANDROID_H
#define PMU_ANDROID_H

#include <fcntl.h>
#include <sched.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/resource.h>
#include <unistd.h>

static inline __attribute__((always_inline)) void serialized_flush(void *ptr)
{
    uintptr_t addr = (uintptr_t)ptr;
    asm volatile("dsb ish" ::: "memory");
    asm volatile("dc civac, %0" :: "r"(addr) : "memory");
    asm volatile("dsb ish" ::: "memory");
    asm volatile("isb" ::: "memory");
}

static inline __attribute__((always_inline)) uint64_t rdtsc(void)
{
    uint64_t cycles;
    asm volatile("dsb sy" ::: "memory");
    asm volatile("isb" ::: "memory");
    asm volatile("mrs %0, pmccntr_el0" : "=r"(cycles));
    asm volatile("isb" ::: "memory");
    return cycles;
}

static inline void pin_to_core_android(int core)
{
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(core, &set);
    if (sched_setaffinity(0, sizeof(set), &set) != 0)
    {
        perror("sched_setaffinity");
    }
    (void)setpriority(PRIO_PROCESS, 0, -20);
}

static inline void try_enable_pmu_for_this_process(void)
{
    int fd;
    char buf[64];

    fd = open("/proc/enable-pmu", O_RDONLY);
    if (fd >= 0)
    {
        (void)read(fd, buf, sizeof(buf));
        close(fd);
    }

    fd = open("/dev/pmuctl", O_WRONLY);
    if (fd >= 0)
    {
        (void)write(fd, "PMCCNTR=1", 9);
        close(fd);
    }
}

#endif
