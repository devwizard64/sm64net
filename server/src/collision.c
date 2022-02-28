/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                    Copyright (C) 2019 - 2021  devwizard                    *
 *        This project is licensed under the terms of the GNU General         *
 *        Public License version 2.  See LICENSE for more information.        *
 ******************************************************************************/

#define NP_CMD_COL      2

#define /* 0x022D */    np_col          tcp[0x0B].u8[1]

static void collision_np_update_self(unused NET_PL *np, PLAYER *pl)
{
    uint i;
    for (i = 1; i < NP_LEN; i++)
    {
        NET_PL *n = &np_table[i];
        if (n->np_col != 0)
        {
            n->np_col = 0;
            if (!(pl->state & 0x00020000))
            {
                OBJECT *obj = np_table[i].np_obj;
                if (obj != NULL)
                {
                    obj->o_colarg = 0;
                    pl_collision_attacked(pl, obj);
                    obj->o_colarg = 2;
                }
            }
        }
    }
}
