#                   SM64Net - An Internet framework for SM64
#                     Copyright (C) 2019 - 2022  devwizard
#         This project is licensed under the terms of the GNU General
#         Public License version 2.  See LICENSE for more information.

VERSION     := 2.2
REVISION    := 0

CC      := cc
CCFLAG  := -I../include -DVERSION=\"$(VERSION)\" -DREVISION=\"$(REVISION)\"
CCFLAG  += -D__NATIVE__ -Wall -Wextra -Wpedantic -O2

.PHONY: clean
clean:
	rm -f -r build

build/%: | build
	$(CC) $(CCFLAG) -s -o $@ $^ $(LIB)

build/%.exe: | build
	$(CC) $(CCFLAG) -s -o $@ $^ $(LIB)

build:
	mkdir -p $@
