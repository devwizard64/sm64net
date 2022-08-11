/******************************************************************************/
/*                  SM64Net - An Internet framework for SM64                  */
/*                    Copyright (C) 2019 - 2021  devwizard                    */
/*        This project is licensed under the terms of the GNU General         */
/*        Public License version 2.  See LICENSE for more information.        */
/******************************************************************************/

static void np_main(void)
{
    NET_PL *np = object->o_np_np;
    if (object != np->np_obj) object->flag = 0;
#ifdef COLLISION
    object->o_pos_x = object->list.s.pos[0];
    object->o_pos_y = object->list.s.pos[1];
    object->o_pos_z = object->list.s.pos[2];
    if (
        mario->pos[1] < object->o_pos_y+160 &&
        object->o_pos_y < mario->pos[1]+160
    ) pl_collision_bound(mario, object, -5);
    if ((object->o_col_flag & 0xFF) != 0)
    {
        while (np_data[0].np_tcp_id != 0);
        np_data[0].np_col    = np-np_data;
        np_data[0].np_tcp_id = NP_CMD_COL;
    }
    object->o_col_flag = 0;
#endif
}

static const O_SCRIPT o_np[] =
{
    0x00040000,
#ifdef COLLISION
    0x23000000, 0x002500A0,
    0x2F000000, 0x00400000,
    0x10050000,
    0x103E0001,
    0x10420002,
#endif
    0x08000000,
        0x0C000000, (uintptr_t)np_main,
    0x09000000,
};
