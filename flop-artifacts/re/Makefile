# Compiler and compiler flags
CC = /home/hyj/android-ndk-r27c/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android33-clang
CFLAGS = -O3 -Wall -D_GNU_SOURCE -fPIE
LDFLAGS = -pie

# Source files
SRC = $(wildcard *.c)
OBJ = $(patsubst %.c, %.o, $(SRC))

# Targets
all: load-loop-timing mispredict oob-read rogue-function aliasing cross-process

load-loop-timing: load-loop-timing.o
	$(CC) $^ -o $@ $(LDFLAGS)

mispredict: mispredict.o
	$(CC) $^ -o $@ $(LDFLAGS)

oob-read: oob-read.o
	$(CC) $^ -o $@ $(LDFLAGS)

rogue-function: rogue-function.o
	$(CC) $^ -o $@ $(LDFLAGS)

aliasing: aliasing.o
	$(CC) $^ -o $@ $(LDFLAGS)

cross-process: cross-process.o
	$(CC) $^ -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)
	rm -f load-loop-timing
	rm -f mispredict
	rm -f oob-read
	rm -f rogue-function
	rm -f aliasing
	rm -f cross-process

.PHONY: all clean
