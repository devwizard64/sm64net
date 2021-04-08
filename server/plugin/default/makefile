#                   SM64Net - An Internet framework for SM64
#                     Copyright (C) 2019 - 2021  devwizard
#         This project is licensed under the terms of the GNU General
#         Public License version 2.  See LICENSE for more information.

build/main.nff:
build/main.elf:     build/nff.o build/main.o build/hook.o
build/main.elf:     build/gfx.ld.o build/script.ld.o
build/gfx.ld.o:     gfx_metaknight.o gfx_bandanadee.o
build/script.ld.o:  script_metaknight.o script_bandanadee.o

FLAG    := -I ../../../include -D _LOCATION -D _CHARACTER
include ../../../include/sm64.mak
