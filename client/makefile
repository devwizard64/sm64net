#                   SM64Net - An Internet framework for SM64
#                     Copyright (C) 2019 - 2022  devwizard
#         This project is licensed under the terms of the GNU General
#         Public License version 2.  See LICENSE for more information.

.PHONY: default
default: bin/sm64net bin/sm64net.exe bin/sm64net_usb

include ../include/sm64net.mk

W32_ARCH    := -mconsole -D_WIN32_WINNT=0x0600

bin/sm64net: src/main.c src/memory.c
	@mkdir -p $(dir $@)
	$(CC) $(CCFLAG) -s -o $@ $^

bin/sm64net.exe: src/main.c src/memory.c
	@mkdir -p $(dir $@)
	$(W32_CC) $(W32_CCFLAG) -s -o $@ $^ -lws2_32

bin/sm64net_usb: src/main.c src/memory_usb.c
	@mkdir -p $(dir $@)
	$(CC) $(CCFLAG) -s -o $@ $^ -lftdi
