#                   SM64Net - An Internet framework for SM64
#                     Copyright (C) 2019 - 2022  devwizard
#         This project is licensed under the terms of the GNU General
#         Public License version 2.  See LICENSE for more information.

build/sm64net: src/main.c src/net.c src/mem.c

build/sm64net.exe: src/main.c src/net.c src/mem.c
build/sm64net.exe: CC  := i686-w64-mingw32-gcc -mconsole -D_WIN32_WINNT=0x0600
build/sm64net.exe: LIB := -lws2_32

build/sm64net_usb: src/main.c src/net.c src/mem_usb.c
build/sm64net_usb: LIB := -lftdi

include ../include/native.mk
