/******************************************************************************/
/*                  SM64Net - An Internet framework for SM64                  */
/*                    Copyright (C) 2019 - 2021  devwizard                    */
/*        This project is licensed under the terms of the GNU General         */
/*        Public License version 2.  See LICENSE for more information.        */
/******************************************************************************/

#include <types.h>
#include <sm64.h>
#include <sm64net.h>

#define NP_CMD_COL      2
#define NP_CMD_SAVE     3

#define /* 0x22A */ np_shape_index      tcp[10].u8[2]
#define /* 0x22B */ np_col              tcp[10].u8[3]
#define /* 0x22C */ np_save_cmd         tcp[11].u8[0]
#define /* 0x22D */ np_save_file        tcp[11].u8[1]
#define /* 0x22E */ np_save_index       tcp[11].s16[1]
#define /* 0x230 */ np_save_value       tcp[12].u32
        /* 0x234 */

#ifdef CHARACTER
typedef struct net_chr
{
    const S_SCRIPT *script;
    s16 waist;
}
NET_CHR;

extern S_SCRIPT s_kirby[];
extern S_SCRIPT s_meta[];
extern S_SCRIPT s_hrdee[];
static const NET_CHR net_chr_table[] =
{
    {NULL,    189},
    {s_kirby, 150},
    {s_meta,  150},
    {s_hrdee, 150},
};

static SHAPE *net_shape_table[lenof(net_chr_table)];
#endif

#include "net_mem.c"
#include "np_shape.c"
#include "np_object.c"
#include "np.c"
#include "net.c"
