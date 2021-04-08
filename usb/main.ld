/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                    Copyright (C) 2019 - 2021  devwizard                    *
 *        This project is licensed under the terms of the GNU General         *
 *        Public License version 2.  See LICENSE for more information.        *
 ******************************************************************************/

#include <sm64.ld>
#include <sm64net.ld>

#define ORG(sym, name, section) \
    __dev = main_start + (sym)-code_main_start; SECTION(sym, name, section)

OUTPUT_ARCH(mips)
SECTIONS
{
    __dev = 0;
    SECTION(0, image, DATA(build/UNSME0.o))
    ORG(0x80246050, hook_a, TEXT(build/hook_a.o))
    ORG(0x80278A0C, hook_b, TEXT(build/hook_b.o))
    ORG(0x80248B90, hook_c, TEXT(build/hook_c.o))
    __dev = 0x007CC6C0;
    CODE(0x8038EEF0, usb, TEXT(build/usb.o) DATA(build/usb.o), BSS(build/usb.o))
    /DISCARD/ :
    {
        *(*);
    }
}
