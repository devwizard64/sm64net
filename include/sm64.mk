#                   SM64Net - An Internet framework for SM64
#                     Copyright (C) 2019 - 2022  devwizard
#         This project is licensed under the terms of the GNU General
#         Public License version 2.  See LICENSE for more information.

CROSS   := mips-linux-gnu-
CC      := $(CROSS)gcc
LD      := $(CROSS)ld
CPP     := $(CROSS)cpp
OBJCOPY := $(CROSS)objcopy
ARCH    := -mabi=32 -march=vr4300 -mfix4300 -mno-abicalls -fno-PIC -G 0
WFLAG   := -Wall -Wextra -Wpedantic -Werror-implicit-function-declaration
CCFLAG  := $(ARCH) -mno-check-zero-division -ffreestanding
CCFLAG  += -fno-common -fno-zero-initialized-in-bss -fno-toplevel-reorder
CCFLAG  += $(FLAG) -Os $(WFLAG)
LDFLAG  :=
CPPFLAG := $(FLAG) -Umips

.PHONY: clean
clean:
	rm -rf build

build/%.z64: build/%.elf build/crc | build
	$(OBJCOPY) -O binary $< $@
	build/crc $@

build/%.nff: build/%.elf | build
	$(OBJCOPY) -O binary $< $@

build/%.elf: build/%.ld | build
	$(LD) $(LDFLAG) -Map $(@:.elf=.map) -o $@ -T $^

build/%.ld: %.ld | build
	$(CPP) $(CPPFLAG) -P -o $@ $<

build/%.ld.o: | build
	$(LD) -r -o $@ $^

build/%.o: %.c | build
	$(CC) $(CCFLAG) -c -o $@ $<

build/%.o: %.S | build
	$(CC) $(CCFLAG) -c -o $@ $<

build/crc: crc.c | build
	cc -Wall -Wextra -Wpedantic -O2 -s -o $@ $<

build:
	mkdir -p $@
