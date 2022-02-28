#                   SM64Net - An Internet framework for SM64
#                     Copyright (C) 2019 - 2022  devwizard
#         This project is licensed under the terms of the GNU General
#         Public License version 2.  See LICENSE for more information.

build/main.nff:
build/main.elf:     build/nff.o build/main.o build/gfx.ld.o build/shape.ld.o
build/main.ld:
build/main.o:       $(wildcard src/*.c)
build/gfx.ld.o:     src/kirby_gfx.o   src/meta_gfx.o   src/hrdee_gfx.o
build/shape.ld.o:   src/kirby_shape.o src/meta_shape.o src/hrdee_shape.o

FLAG    := -I../include -DLOCATION -DCHARACTER -DCOLLISION
include ../include/sm64.mk
