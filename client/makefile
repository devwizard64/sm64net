#                   SM64Net - An Internet framework for SM64
#                     Copyright (C) 2019 - 2021  devwizard
#         This project is licensed under the terms of the GNU General
#         Public License version 2.  See LICENSE for more information.

VERSION := 1.1.1

CC      := cc
CCFLAG  := -I ../include -D _VERSION=\"$(VERSION)\" -D _NATIVE 
CCFLAG  += -Wall -Wextra -Wpedantic -O2

OBJ := \
	build/sm64net       \
	build/sm64net_usb   \
	build/sm64net.exe   \
	build/SM64NetLauncher.exe

CLIENT      := src/client/main.c src/client/mem.c src/client/net.c
CLIENT_USB  := src/client/main.c src/client/mem_usb.c src/client/net.c
LAUNCHER    := src/launcher/main.c src/launcher/window.c src/launcher/config.c
build/sm64net_usb: LIB := -l ftdi
build/sm64net.exe: CC  := i686-w64-mingw32-gcc -mconsole
build/sm64net.exe: LIB := -l ws2_32
build/SM64NetLauncher.exe: CC  := i686-w64-mingw32-gcc -mwindows
build/SM64NetLauncher.exe: LIB := -l comctl32

default: $(OBJ)
build/sm64net:      $(CLIENT)
build/sm64net_usb:  $(CLIENT_USB)
build/sm64net.exe:  $(CLIENT)
build/SM64NetLauncher.exe: $(LAUNCHER)

build/%: | build
	@$(CC) $(CCFLAG) -o $@ $^ $(LIB)

build:
	@mkdir -p $@

clean:
	@rm -rf build

.PHONY: default clean
