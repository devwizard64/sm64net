/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                    Copyright (C) 2019 - 2022  devwizard                    *
 *        This project is licensed under the terms of the GNU General         *
 *        Public License version 2.  See LICENSE for more information.        *
 ******************************************************************************/

#include <sm64.ld>
#include <sm64net.ld>

#define NET_MEM_START   0x80025C00  /* np_table */
#define NET_MEM_END     0x8005C000
#define NET_GFX_START   0x80400000
#define NET_GFX_END     0x80800000

net_heap = NET_MEM_END - (1+NET_PAGE_SIZE)*NET_PAGE_LEN;

OUTPUT_ARCH(mips)
SECTIONS
{
    __dev = 0;
    SECTION(0, nff, TEXT(build/nff.o), ,)
    SECTION(NET_MEM_START + NP_LEN*NP_SIZE, net_main,
        TEXT(build/main.o)
        ,
        DATA(build/main.o)
        ,
        BSS(build/main.o)
    )
#ifdef CHARACTER
    SECTION(net_heap, net_shape,
        TEXT(build/shape.ld.o)
        ,
        DATA(build/shape.ld.o)
        ,
    )
    SECTION(NET_GFX_START, net_gfx,
        TEXT(build/gfx.ld.o)
        ,
        DATA(build/gfx.ld.o)
        ,
        BSS(build/gfx.ld.o)
    )
    net_arena_start  = .;
    net_arena_end    = NET_GFX_END;
#endif
    /DISCARD/ : {*(*)}
}
