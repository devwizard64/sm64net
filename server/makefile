#                   SM64Net - An Internet framework for SM64
#                     Copyright (C) 2019 - 2022  devwizard
#         This project is licensed under the terms of the GNU General
#         Public License version 2.  See LICENSE for more information.

build/main.nff:
build/main.elf:     build/nff.o build/main.o build/gfx.ld.o build/shape.ld.o
build/main.ld:
build/gfx.ld.o:     kirby_gfx.o   meta_gfx.o   hrdee_gfx.o
build/shape.ld.o:   kirby_shape.o meta_shape.o hrdee_shape.o

FLAG    := -I../include -DLOCATION -DCHARACTER -DCOLLISION
include ../include/sm64.mk
