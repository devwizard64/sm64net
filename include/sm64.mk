#                   SM64Net - An Internet framework for SM64
#                     Copyright (C) 2019 - 2021  devwizard
#         This project is licensed under the terms of the GNU General
#         Public License version 2.  See LICENSE for more information.

CC      := mips-linux-gnu-gcc
LD      := mips-linux-gnu-ld
CPP     := mips-linux-gnu-cpp
OBJCOPY := mips-linux-gnu-objcopy
CC      += -march=vr4300 -mfix4300 -mfp32 -mno-abicalls -fno-PIC
CC      += -ffreestanding -fno-zero-initialized-in-bss -G 0
CC      += -Wall -Wextra -Wpedantic -Os $(FLAG)
LD      += --accept-unknown-input-arch --no-check-sections
CPP     += -U mips $(FLAG)

build/%.z64: build/%.elf build/crc
	@$(OBJCOPY) -O binary $< $@
	@build/crc $@

build/%.nff: build/%.elf
	@$(OBJCOPY) -O binary $< $@

build/%.elf: build/%.ld
	@$(LD) -Map $(@:.elf=.map) -o $@ -T $^

build/%.ld: %.ld | build
	@$(CPP) -P -o $@ $<

build/%.ld.o:
	@$(LD) -r -o $@ $^

build/%.o: %.c | build
	@$(CC) -c -o $@ $<

build/%.o: %.S | build
	@$(CC) -c -o $@ $<

build/%.o: %.z64 | build
	@$(OBJCOPY) -O elf32-tradbigmips -I binary $< $@

build/crc: crc.c
	@cc -O2 -o $@ $<

build:
	@mkdir -p $@

clean:
	@rm -rf build

.PHONY: clean
