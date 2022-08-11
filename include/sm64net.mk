#                   SM64Net - An Internet framework for SM64
#                     Copyright (C) 2019 - 2022  devwizard
#         This project is licensed under the terms of the GNU General
#         Public License version 2.  See LICENSE for more information.

VERSION     := 2.2
REVISION    := 2

CC      := gcc
FLAG    := -I../include -DVERSION=\"$(VERSION)\" -DREVISION=\"$(REVISION)\"
OPT     := -O2
WARN    := -Wall -Wextra -Wpedantic
CCFLAG   = $(FLAG) $(OPT) $(WARN)

W32_HOST    := x86_64-w64-mingw32
W32_CC      := $(W32_HOST)-gcc
W32_ARCH    :=
W32_CCFLAG   = $(W32_ARCH) $(FLAG) $(OPT) $(WARN)

U64_HOST    := mips-linux-gnu
U64_CC      := $(U64_HOST)-gcc
U64_LD      := $(U64_HOST)-ld
U64_CPP     := $(U64_HOST)-cpp
U64_OBJCOPY := $(U64_HOST)-objcopy
U64_ARCH    := -mabi=32 -march=vr4300 -mfix4300 -mno-abicalls -fno-PIC -G 0
U64_FLAG    := -I../include -D_ULTRA64
U64_OPT     := -Os
U64_WARN    := -Wall -Wextra -Wpedantic -Werror-implicit-function-declaration
U64_CCFLAG   = $(U64_ARCH) -mno-check-zero-division -ffreestanding
U64_CCFLAG  += -fno-common -fno-zero-initialized-in-bss -fno-toplevel-reorder
U64_CCFLAG  += $(U64_FLAG) $(U64_OPT) $(U64_WARN)
U64_ASFLAG   = $(U64_ARCH) $(U64_FLAG)
U64_LDFLAG   =
U64_CPPFLAG  = $(U64_FLAG) -Umips

.PHONY: clean
clean:
	rm -f -r bin build tools/mask

build/%.z64: build/%.elf tools/mask
	$(U64_OBJCOPY) -O binary $< $@
	tools/mask $@

build/%.nff: build/%.elf
	$(U64_OBJCOPY) -O binary $< $@

build/%.elf: build/%.ld
	$(U64_LD) $(U64_LDFLAG) -Map $(@:.elf=.map) -o $@ -T $^

build/%.ld: %.ld
	@mkdir -p $(dir $@)
	$(U64_CPP) $(U64_CPPFLAG) -P -o $@ $<

build/%.o: %.s
	@mkdir -p $(dir $@)
	$(U64_CC) $(U64_CCFLAG) -c -o $@ $<

build/%.o: %.sx
	@mkdir -p $(dir $@)
	$(U64_CC) $(U64_CCFLAG) -c -o $@ $<

build/%.o: %.c
	@mkdir -p $(dir $@)
	$(U64_CC) $(U64_CCFLAG) -c -o $@ $<

tools/mask:
tools/%: tools/%.c
	@mkdir -p $(dir $@)
	$(CC) $(OPT) $(WARN) -s -o $@ $<
