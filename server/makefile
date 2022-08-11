#                   SM64Net - An Internet framework for SM64
#                     Copyright (C) 2019 - 2022  devwizard
#         This project is licensed under the terms of the GNU General
#         Public License version 2.  See LICENSE for more information.

.PHONY: default
default: build/main.nff

include ../include/sm64net.mk

U64_FLAG    += -DLOCATION -DCHARACTER -DCOLLISION -DSAVE

build/main.nff:
build/main.elf: build/src/nff.o build/src/main.o build/gfx.o build/shape.o
build/main.ld:
build/src/main.o: $(wildcard src/*.c)

build/gfx.o: src/kirby_gfx.o src/meta_gfx.o src/hrdee_gfx.o
	@mkdir -p $(dir $@)
	$(U64_LD) -r -o $@ $^

build/shape.o: src/kirby_shape.o src/meta_shape.o src/hrdee_shape.o
	@mkdir -p $(dir $@)
	$(U64_LD) -r -o $@ $^
