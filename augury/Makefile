API_LEVEL ?= 33
ANDROID_NDK_HOME ?= $(HOME)/android-ndk-r27c
ANDROID_TC := $(ANDROID_NDK_HOME)/toolchains/llvm/prebuilt/linux-x86_64
CC := $(ANDROID_TC)/bin/aarch64-linux-android$(API_LEVEL)-clang
CFLAGS := -O0 -D_GNU_SOURCE -fPIE
LDFLAGS := -pie
UTIL_OBJ := util/linux-util.o
LIBS :=
_OBJ ?= augury imp

PAC ?= 0
ifneq ($(PAC),0)
	CFLAGS += -arch arm64e
	LIBS += -arch arm64e
endif
CFLAGS += -DPMU

OUTDIR=bin
OBJDIR=obj
N_OBJ = $(_OBJ:%=$(OUTDIR)/%)
D_OBJ = $(_OBJ:%=$(OUTDIR)/%-debug)

NA_OBJ = $(_OBJ:%=$(OUTDIR)/%.S)
DA_OBJ = $(_OBJ:%=$(OUTDIR)/%-debug.S)

all: debug native

ifneq ($(findstring clang,$(notdir $(CC))),)
$(OBJDIR)/slh-poc.o: CFLAGS += -mspeculative-load-hardening
$(OBJDIR)/slh-poc-debug.o: CFLAGS += -mspeculative-load-hardening
endif

$(OBJDIR)/%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

$(OBJDIR)/%-debug.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

$(OUTDIR)/%: $(OBJDIR)/%.o $(UTIL_OBJ)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

$(OUTDIR)/%-debug: $(OBJDIR)/%-debug.o $(UTIL_OBJ)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

$(OUTDIR)/%.S: $(OUTDIR)/%
	$(CC) -S -c $(CFLAGS) $(notdir $(patsubst %.S,%.c,$(patsubst %-debug.S,%.S,$@))) -o $@

# $(OUTDIR)/%-debug.S: $(OUTDIR)/%-debug
# 	$(ASM_CMD) $(@:-debug=nope) > $@
# 	# $(ASM_CMD) $(notdir $(@:-debug.S=.c)) > $@

.PHONY: clean

debug: CFLAGS += -DNATIVE -DDEBUGG
debug: |$(OBJDIR)
debug: |$(OUTDIR) $(D_OBJ) $(DA_OBJ)

native: CFLAGS += -DNATIVE
native: |$(OBJDIR)
native: |$(OUTDIR) $(N_OBJ) $(NA_OBJ)

asm-native: CFLAGS += -DNATIVE
asm-native: |$(NA_OBJ)

asm-debug: CFLAGS += -DNATIVE -DDEBUGG
asm-debug: |$(DA_OBJ)

asm: asm-native asm-debug

$(OUTDIR):
	mkdir -p $(OUTDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf bin obj
	rm -rf util/*.o
