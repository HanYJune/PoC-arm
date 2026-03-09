# Toolchain setup
CC = clang
AR = aarch64-none-linux-gnu-ar
LD = aarch64-none-linux-gnu-ld 

# /home/hyj/pixel/toolchains/arm-gnu-toolchain-13.2.Rel1-x86_64-aarch64-none-linux-gnu

SYSROOT = ${GCC_TOOLCHAIN}/aarch64-none-linux-gnu/libc
# Compilation and linking flags
CFLAGS = --target=aarch64-linux-gnu -march=armv9a+memtag+pauth -std=gnu99 -O2 -Wall -Wextra -I./ --sysroot=$(SYSROOT) -DTIMER=$(TIMER)
LDFLAGS = -lpthread -lm -fuse-ld=lld -static --gcc-toolchain=$(GCC_TOOLCHAIN) -L$(GCC_TOOLCHAIN)/aarch64-none-linux-gnu/libc/lib/aarch64-linux-gnu/

# Source files and objects
SHARED_OBJECTS = lib/mte.o lib/perf_counters.o lib/scheduler.o lib/timer.o

SRCS = poc.c backpac.c
OBJS = $(SRCS:.c=.o)

# All target
all: poc backpac

# Build rules

$(SHARED_OBJECTS): %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

poc: poc.o $(SHARED_OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) poc.o $(SHARED_OBJECTS) -o $@

backpac: backpac.o $(SHARED_OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) backpac.o $(SHARED_OBJECTS) -o $@

clean:
	rm -f poc backpac $(OBJS) $(SHARED_OBJECTS) *.o
