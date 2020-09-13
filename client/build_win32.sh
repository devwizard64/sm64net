#!/bin/bash

#                   SM64Net - An Internet framework for SM64
#                    Copyright (C) 2019, 2020  devwizard
#       This project is licensed under the GNU General Public License
#       version 2.  See LICENSE for more information.

set -e
source build_init.sh
i686-w64-mingw32-gcc -mconsole \
    -O2 -Wall -Wextra -Wpedantic -I ../include -D _VERSION=\"$VERSION\" \
    -o build/sm64net.exe \
    src/client/main.c \
    src/client/mem.c \
    src/client/net.c \
    -l ws2_32
i686-w64-mingw32-gcc -mwindows \
    -O2 -Wall -Wextra -Wpedantic -I ../include -D _VERSION=\"$VERSION\" \
    -o build/SM64NetLauncher.exe \
    src/launcher/main.c \
    src/launcher/window.c \
    src/launcher/config.c \
    src/launcher/menu.c \
    -l comctl32
