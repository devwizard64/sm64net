#!/bin/sh

#                   SM64Net - An Internet framework for SM64
#                    Copyright (C) 2019, 2020  devwizard
#       This project is licensed under the GNU General Public License
#       version 2.  See LICENSE for more information.

set -e
MKDIR()
{
    if [ ! -d $1 ]
    then
        mkdir $1
    fi
}
CC()
{
    mips-linux-gnu-gcc -march=mips3 -mabi=32 -mfix4300 -mno-abicalls \
        -mno-shared -G 0 -fno-stack-protector -fno-common -fno-PIC \
        -ffreestanding -fno-builtin -fwrapv \
        -Os -Wall -Wextra -Wpedantic -I ../../../include -D _N64 \
        -c -o build/$1.o $1.c
}
MKDIR build
MKDIR build/src
CC src/main
CC src/gfx_object_net_player
../../../tools/armips src/main.asm