#                   SM64Net - An Internet framework for SM64
#                     Copyright (C) 2019 - 2022  devwizard
#         This project is licensed under the terms of the GNU General
#         Public License version 2.  See LICENSE for more information.

.PHONY: default
default: bin/SM64NetGUI.exe

include ../include/sm64net.mk

W32_ARCH    := -mwindows

bin/SM64NetGUI.exe: src/main.c
	@mkdir -p $(dir $@)
	$(W32_CC) $(W32_CCFLAG) -s -o $@ $^ -lcomctl32 -lcomdlg32 -lgdi32
