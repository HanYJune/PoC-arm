CC = /home/hyj/android-ndk-r27c/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android33-clang
CFLAGS = -O3 -Wall -D_GNU_SOURCE -fPIE -pie

SOURCES=$(wildcard *.c)

EXECUTABLES=$(SOURCES:.c=)

all: $(EXECUTABLES)

%: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(EXECUTABLES)
