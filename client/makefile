#                   SM64Net - An Internet framework for SM64
#                     Copyright (C) 2019 - 2021  devwizard
#         This project is licensed under the terms of the GNU General
#         Public License version 2.  See LICENSE for more information.

VERSION 	:= 2.1
REVISION	:= 0

CC      := cc
CCFLAG  := -I ../include -D _VERSION=\"$(VERSION)\" -D _REVISION=\"$(REVISION)\"
CCFLAG  += -D _NATIVE -Wall -Wextra -Wpedantic -O2

OBJ := \
	build/sm64net       \
	build/sm64net_usb   \
	build/sm64net.exe   \
	build/SM64NetLauncher.exe

default: $(OBJ)

SRC := src/client/main.c src/client/mem.c src/client/net.c
build/sm64net: $(SRC)
build/sm64net.exe: CC  := i686-w64-mingw32-gcc -mconsole -D _WIN32_WINNT=0x0600
build/sm64net.exe: LIB := -l ws2_32
build/sm64net.exe: $(SRC)

SRC := src/client/main.c src/client/mem_usb.c src/client/net.c
build/sm64net_usb: LIB := -l ftdi
build/sm64net_usb: $(SRC)

SRC := src/launcher/main.c src/launcher/window.c src/launcher/config.c
build/SM64NetLauncher.exe: CC  := i686-w64-mingw32-gcc -mwindows
build/SM64NetLauncher.exe: LIB := -l comctl32
build/SM64NetLauncher.exe: $(SRC)

build/%: | build
	@$(CC) $(CCFLAG) -o $@ $^ $(LIB)

build:
	@mkdir -p $@

clean:
	@rm -rf build

.PHONY: default clean
