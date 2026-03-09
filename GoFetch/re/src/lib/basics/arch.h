#ifndef __ARCH_H__
#define __ARCH_H__

#define KB 1024ULL
#define MB (1024*KB)

/*
 * Apple M1/M2 defaults are preserved for macOS.
 * Android defaults target ARM64 phones (Pixel-class) and can be overridden
 * via compiler defines (e.g. -DGOFETCH_L2_NWAYS=16).
 */
#ifndef GOFETCH_PAGE_SIZE
#if defined(__ANDROID__)
#define GOFETCH_PAGE_SIZE       (16 * KB)
#else
#define GOFETCH_PAGE_SIZE       (16 * KB)
#endif
#endif

#ifndef GOFETCH_L1_NWAYS
#define GOFETCH_L1_NWAYS        8
#endif

#ifndef GOFETCH_L1_LINE_SIZE
#define GOFETCH_L1_LINE_SIZE    64
#endif

#ifndef GOFETCH_L1_SIZE
#if defined(__ANDROID__)
#define GOFETCH_L1_SIZE         (64 * KB)
#else
#define GOFETCH_L1_SIZE         (128 * KB)
#endif
#endif

#ifndef GOFETCH_L2_NWAYS
#if defined(__ANDROID__)
#define GOFETCH_L2_NWAYS        16
#else
#define GOFETCH_L2_NWAYS        12
#endif
#endif

#ifndef GOFETCH_L2_SIZE
#if defined(__ANDROID__)
#define GOFETCH_L2_SIZE         (2 * MB)
#else
#define GOFETCH_L2_SIZE         (12 * MB)
#endif
#endif

#ifndef GOFETCH_L2_LINE_SIZE
#if defined(__ANDROID__)
#define GOFETCH_L2_LINE_SIZE    64
#else
#define GOFETCH_L2_LINE_SIZE    128
#endif
#endif

#ifdef PAGE_SIZE
#undef PAGE_SIZE
#endif
#define PAGE_SIZE               GOFETCH_PAGE_SIZE
#define L1_NWAYS                GOFETCH_L1_NWAYS
#define L1_LINE_SIZE            GOFETCH_L1_LINE_SIZE
#define L1_SIZE                 GOFETCH_L1_SIZE
#define L2_NWAYS                GOFETCH_L2_NWAYS
#define L2_SIZE                 GOFETCH_L2_SIZE
#define L2_LINE_SIZE            GOFETCH_L2_LINE_SIZE

#define MASK(x)     ((1 << x) - 1)
#define MSB_MASK    0x8000000000000000ULL

#define HPO_nbits   11
#define RPO_nbits   7
#if L2_LINE_SIZE == 128
#define CLO_nbits   7
#else
#define CLO_nbits   6
#endif

#define HPO(vaddr)  ( (vaddr >> (CLO_nbits + RPO_nbits)) & MASK(HPO_nbits) )
#define RPO(vaddr)  ( (vaddr >> CLO_nbits) & MASK(CLO_nbits) )
#define CLO(vaddr)  ( (vaddr) & MASK(CLO_nbits) )


#if defined(__ANDROID__)
#define L1_HIT_MAX_LATENCY  100
#define L2_MISS_MIN_LATENCY 220
#elif defined(__linux__)
#define L1_HIT_MAX_LATENCY  65
#define L2_MISS_MIN_LATENCY 200
#else
#define L1_HIT_MAX_LATENCY  224
#define L2_MISS_MIN_LATENCY 300
#endif

#endif
