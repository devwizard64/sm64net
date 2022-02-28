/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                    Copyright (C) 2019 - 2021  devwizard                    *
 *        This project is licensed under the terms of the GNU General         *
 *        Public License version 2.  See LICENSE for more information.        *
 ******************************************************************************/

static void net_init(void)
{
    uint i;
    shape_table[2] = &shape_np;
    np_init_self(&np_table[0]);
    for (i = 1; i < NP_LEN; i++) np_init_peer(&np_table[i]);
#ifdef CHARACTER
    character_init();
#endif
    net_mem_init();
}

static void net_update(void)
{
    uint i;
    np_update_self(&np_table[0], mario);
    for (i = 1; i < NP_LEN; i++) np_update_peer(&np_table[i]);
}

void main(void)
{
    if (segment_table[0x04] != 0)
    {
        static u8 init = true;
        if (init)
        {
            init = false;
            net_init();
        }
        net_update();
    }
}
