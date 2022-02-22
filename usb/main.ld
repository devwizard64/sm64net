/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                    Copyright (C) 2019 - 2022  devwizard                    *
 *        This project is licensed under the terms of the GNU General         *
 *        Public License version 2.  See LICENSE for more information.        *
 ******************************************************************************/

#include <sm64.ld>
#include <sm64net.ld>

#define ORG(addr, name, s_text, s_data) \
    __dev = _mainSegmentRomStart + (addr)-_mainSegmentStart; \
    SECTION(addr, name, s_text, s_data,)
#define J(addr) \
    BYTE(0x02 << 2); \
    BYTE((addr) >> 18 & 0xFF);  \
    BYTE((addr) >> 10 & 0xFF);  \
    BYTE((addr) >>  2 & 0xFF);
#define JAL(addr) \
    BYTE(0x03 << 2); \
    BYTE((addr) >> 18 & 0xFF);  \
    BYTE((addr) >> 10 & 0xFF);  \
    BYTE((addr) >>  2 & 0xFF);

OUTPUT_ARCH(mips)
SECTIONS
{
    __dev = 0;
    SECTION(0, image, TEXT(build/UNSME0.o),,)
    ORG(0x80246050, init, TEXT(build/init.o),)
    ORG(0x80278A0C, hook_init, J(mem_load_usb),)
    ORG(0x802466A4, hook_update, JAL(usb_update),)
    ORG(0x80248B90, hook_draw, JAL(usb_draw),)
    __dev = 0x007CC6C0;
    SECTION(0x8038EEF0, usb,
        TEXT(build/main.o)
        ,
        DATA(build/main.o)
        ,
        BSS(build/main.o)
    )
    ASSERT(. <= 0x8038F800, "error: USB exceeds memory size.")
    /DISCARD/ : {*(*)}
}
