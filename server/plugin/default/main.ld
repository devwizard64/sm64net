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
    CODE(np_table + NP_SIZE*NP_LEN, net_main,
        TEXT(build/main.o)
        DATA(build/main.o)
#ifdef _CHARACTER
        DATA(build/script.ld.o)
#endif
        ,
        BSS(build/main.o)
    )
#ifdef _CHARACTER
    CODE(0x80400000, net_gfx, DATA(build/gfx.ld.o),)
    net_gfx_heap_start  = .;
    net_gfx_heap_end    = 0x80800000;
#endif
    CODE(0x80248BE0, net_hook, TEXT(build/hook.o),)
    /DISCARD/ :
    {
        *(*);
    }
}
