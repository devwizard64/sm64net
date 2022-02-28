/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                    Copyright (C) 2019 - 2021  devwizard                    *
 *        This project is licensed under the terms of the GNU General         *
 *        Public License version 2.  See LICENSE for more information.        *
 ******************************************************************************/

#define /* 0x022C */    np_shape_index      tcp[0x0B].u8[0]

typedef struct net_chr
{
    const S_SCRIPT *script;
    s16 waist;
}
NET_CHR;

extern S_SCRIPT s_kirby[];
extern S_SCRIPT s_meta[];
extern S_SCRIPT s_hrdee[];

extern u8 net_arena_start[];
extern u8 net_arena_end[];

static const NET_CHR net_chr_table[] =
{
    {NULL,    189},
    {s_kirby, 150},
    {s_meta,  150},
    {s_hrdee, 150},
};

static SHAPE *net_shape_table[lenof(net_chr_table)];

static void character_np_init_self(NET_PL *np)
{
    np->np_shape_index = 0;
}

static void character_np_update(NET_PL *np, unused OBJECT *obj)
{
    np->np_waist = net_chr_table[np->np_shape_index].waist;
    np->np_shape = net_shape_table[np->np_shape_index];
}

static void character_np_update_self(NET_PL *np, PLAYER *pl)
{
    if (pl->cont->down & L_JPAD)
    {
        if (np->np_shape_index > 0)
        {
            np->np_shape_index--;
            np->np_tcp_id = NP_CMD_SYNC;
        }
    }
    if (pl->cont->down & R_JPAD)
    {
        if (np->np_shape_index < lenof(net_chr_table)-1)
        {
            np->np_shape_index++;
            np->np_tcp_id = NP_CMD_SYNC;
        }
    }
}

static void character_init(void)
{
    ARENA arena =
    {
        net_arena_end - net_arena_start,
        0,
        net_arena_start,
        net_arena_start,
    };
    uint i;
    net_shape_table[0] = shape_table[1];
    for (i = 1; i < lenof(net_chr_table); i++)
    {
        net_shape_table[i] = s_script_main(&arena, net_chr_table[i].script);
    }
}
