#!/bin/bash

#                   SM64Net - An Internet framework for SM64
#                    Copyright (C) 2019, 2020  devwizard
#       This project is licensed under the GNU General Public License
#       version 2.  See LICENSE for more information.

set -e
CC="i686-w64-mingw32-gcc -O2"
source build_init.sh
$CC -mconsole -o build/sm64net.exe \
    src/client/main.c \
    src/client/mem.c \
    src/client/net.c \
    -l ws2_32
$CC -mwindows -o build/SM64NetLauncher.exe \
    src/launcher/main.c \
    src/launcher/window.c \
    src/launcher/config.c \
    -l comctl32
