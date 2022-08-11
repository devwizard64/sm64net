/******************************************************************************/
/*                  SM64Net - An Internet framework for SM64                  */
/*                    Copyright (C) 2019 - 2022  devwizard                    */
/*        This project is licensed under the terms of the GNU General         */
/*        Public License version 2.  See LICENSE for more information.        */
/******************************************************************************/

#include <sm64.ld>
#define __ASSEMBLER__
#include <sm64net.h>

#define NP_DATA         0x80025C00
#define NET_HEAP_END    0x8005C000
#define NET_GFX_START   0x80400000
#define NET_GFX_END     0x80800000

#define NET_MAIN_START  (NP_DATA + NP_SIZE*NP_LEN)
#define NET_HEAP_START  (NET_HEAP_END - (1+NET_PAGE_SIZE)*NET_PAGE_LEN)

np_data = NP_DATA;

OUTPUT_ARCH(mips)
SECTIONS
{
    __dev = 0;
    SECTION(0, nff, TEXT(build/src/nff.o),,)
    SECTION(NET_MAIN_START, net_main,
        TEXT(build/src/main.o)
        ,
        DATA(build/src/main.o)
        ,
        BSS(build/src/main.o)
    )
    ASSERT(. <= NET_HEAP_START, "error: NET_MAIN exceeds memory size.")
    net_heap = NET_HEAP_START;
#ifdef CHARACTER
    SECTION(, net_shape,
        TEXT(build/shape.o)
        ,
        DATA(build/shape.o)
        ,
        BSS(build/shape.o)
    )
    ASSERT(. <= NET_HEAP_END, "error: NET_SHAPE exceeds memory size.")
    SECTION(NET_GFX_START, net_gfx,
        TEXT(build/gfx.o)
        ,
        DATA(build/gfx.o)
        ,
        BSS(build/gfx.o)
    )
    ASSERT(. <= NET_GFX_END, "error: NET_GFX exceeds memory size.")
    net_arena_start  = .;
    net_arena_end    = NET_GFX_END;
#endif
    /DISCARD/ : {*(*)}
}
