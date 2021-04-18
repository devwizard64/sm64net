/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                    Copyright (C) 2019 - 2021  devwizard                    *
 *        This project is licensed under the terms of the GNU General         *
 *        Public License version 2.  See LICENSE for more information.        *
 ******************************************************************************/

#include <sm64.ld>
#include <sm64net.ld>

OUTPUT_ARCH(mips)
SECTIONS
{
    __dev = 0;
    SECTION(0, nff, TEXT(build/nff.o))
    nm_heap = 0x8005C000 - (1+NM_PAGE_SIZE)*NM_PAGE_LEN;
    CODE(0x8002DC00, net_main,
        TEXT(build/main.o)
        DATA(build/main.o),
        BSS(build/main.o)
    )
#ifdef _CHARACTER
    CODE(nm_heap, net_script, DATA(build/script.ld.o),)
    CODE(0x80400000, net_gfx, DATA(build/gfx.ld.o),)
    net_gfx_arena_start  = .;
    net_gfx_arena_end    = 0x80800000;
#endif
    /DISCARD/ :
    {
        *(*);
    }
}
