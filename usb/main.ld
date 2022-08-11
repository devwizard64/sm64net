/******************************************************************************/
/*                  SM64Net - An Internet framework for SM64                  */
/*                    Copyright (C) 2019 - 2022  devwizard                    */
/*        This project is licensed under the terms of the GNU General         */
/*        Public License version 2.  See LICENSE for more information.        */
/******************************************************************************/

#include <sm64.ld>

#define ORG(addr, name, s_text, s_data) \
    __dev = _mainSegmentRomStart + (addr)-_mainSegmentStart; \
    SECTION(addr, name, s_text, s_data,)
#define J(addr)     LONG(2 << 26 | (addr) >> 2 & 0x03FFFFFF);
#define JAL(addr)   LONG(3 << 26 | (addr) >> 2 & 0x03FFFFFF);

OUTPUT_ARCH(mips)
SECTIONS
{
    __dev = 0;
    SECTION(0, image, DATA(build/src/image.o),,)
    ORG(0x80246170, init, TEXT(build/src/init.o),)
    ORG(0x80278A0C, hook_init, J(mem_load_usb),)
    ORG(0x802466A4, hook_update, JAL(usb_update),)
    __dev = 0x007CC6C0;
    SECTION(0x8038EEF0, usb,
        TEXT(build/src/main.o)
        ,
        DATA(build/src/main.o)
        ,
        BSS(build/src/main.o)
    )
    ASSERT(. <= 0x8038F800, "error: USB exceeds memory size.")
    /DISCARD/ : {*(*)}
}
