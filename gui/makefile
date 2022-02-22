#                   SM64Net - An Internet framework for SM64
#                     Copyright (C) 2019 - 2022  devwizard
#         This project is licensed under the terms of the GNU General
#         Public License version 2.  See LICENSE for more information.

build/SM64NetGUI.exe: src/main.c src/window.c src/config.c
build/SM64NetGUI.exe: CC  := i686-w64-mingw32-gcc -mwindows
build/SM64NetGUI.exe: LIB := -lcomctl32 -lcomdlg32 -lgdi32

include ../include/native.mk
