#                   SM64Net - An Internet framework for SM64
#                     Copyright (C) 2019 - 2022  devwizard
#         This project is licensed under the terms of the GNU General
#         Public License version 2.  See LICENSE for more information.

.PHONY: default
default: build/main.z64

include ../include/sm64net.mk

build/main.z64:
build/main.elf: build/src/image.o build/src/init.o build/src/main.o
build/main.elf: U64_LDFLAG := --no-check-sections
build/main.ld:
