#!/bin/bash

#                   SM64Net - An Internet framework for SM64
#                    Copyright (C) 2019, 2020  devwizard
#       This project is licensed under the GNU General Public License
#       version 2.  See LICENSE for more information.

set -e
source build_init.sh
gcc \
    -O2 -Wall -Wextra -Wpedantic -I ../include -D _VERSION=\"$VERSION\" \
    -o build/sm64net \
    src/client/main.c \
    src/client/mem.c \
    src/client/net.c
# gcc \
#     -O2 -Wall -Wextra -Wpedantic -I ../include -D _VERSION=\"$VERSION\" \
#     -o build/sm64net_usb \
#     src/client/main.c \
#     src/client/mem_usb.c \
#     src/client/net.c \
#     -l ftdi
