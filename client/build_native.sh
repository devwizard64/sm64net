#!/bin/bash

#                   SM64Net - An Internet framework for SM64
#                    Copyright (C) 2019, 2020  devwizard
#       This project is licensed under the GNU General Public License
#       version 2.  See LICENSE for more information.

set -e
CC="gcc -O2"
source build_init.sh
$CC -o build/sm64net \
    src/client/main.c \
    src/client/mem.c \
    src/client/net.c
# $CC -o build/sm64net_usb \
#     src/client/main.c \
#     src/client/mem_usb.c \
#     src/client/net.c \
#     -l ftdi
