/******************************************************************************/
/*                  SM64Net - An Internet framework for SM64                  */
/*                    Copyright (C) 2019 - 2021  devwizard                    */
/*        This project is licensed under the terms of the GNU General         */
/*        Public License version 2.  See LICENSE for more information.        */
/******************************************************************************/

#ifdef CHARACTER
extern char net_arena_start[];
extern char net_arena_end[];
static void character_init(void)
{
    int i;
    ARENA arena;
    arena.size = net_arena_end - net_arena_start;
    arena.used = 0;
    arena.free = arena.start = net_arena_start;
    net_shape_table[0] = shape_table[1];
    for (i = 1; i < (int)lenof(net_chr_table); i++)
    {
        net_shape_table[i] = s_script_main(&arena, net_chr_table[i].script);
    }
}
#endif

#ifdef SAVE
#define NP_SAVE_FLAGSET 0
#define NP_SAVE_FLAGCLR 1
#define NP_SAVE_STARSET 2
#define NP_SAVE_COINSET 3
#define NP_SAVE_ERASE   4
#define NP_SAVE_COPY    5

void net_save_flag_set(int file, uint flag)
{
    while (np_data[0].np_tcp_id != 0);
    np_data[0].np_save_cmd      = NP_SAVE_FLAGSET;
    np_data[0].np_save_file     = file;
    np_data[0].np_save_value    = flag;
    np_data[0].np_tcp_id        = NP_CMD_SAVE;
}

void net_save_flag_clr(int file, uint flag)
{
    while (np_data[0].np_tcp_id != 0);
    np_data[0].np_save_cmd      = NP_SAVE_FLAGCLR;
    np_data[0].np_save_file     = file;
    np_data[0].np_save_value    = flag;
    np_data[0].np_tcp_id        = NP_CMD_SAVE;
}

void net_save_star_set(int file, int course, uint flag)
{
    while (np_data[0].np_tcp_id != 0);
    np_data[0].np_save_cmd      = NP_SAVE_STARSET;
    np_data[0].np_save_file     = file;
    np_data[0].np_save_index    = course;
    np_data[0].np_save_value    = flag;
    np_data[0].np_tcp_id        = NP_CMD_SAVE;
}

void net_save_coin_set(int file, int course, int coin)
{
    while (np_data[0].np_tcp_id != 0);
    np_data[0].np_save_cmd      = NP_SAVE_COINSET;
    np_data[0].np_save_file     = file;
    np_data[0].np_save_index    = course;
    np_data[0].np_save_value    = coin;
    np_data[0].np_tcp_id        = NP_CMD_SAVE;
}

void net_save_erase(int file)
{
    while (np_data[0].np_tcp_id != 0);
    np_data[0].np_save_cmd      = NP_SAVE_ERASE;
    np_data[0].np_save_file     = file;
    np_data[0].np_tcp_id        = NP_CMD_SAVE;
}

void net_save_copy(int src, int dst)
{
    while (np_data[0].np_tcp_id != 0);
    np_data[0].np_save_cmd      = NP_SAVE_COPY;
    np_data[0].np_save_file     = dst;
    np_data[0].np_save_index    = src;
    np_data[0].np_tcp_id        = NP_CMD_SAVE;
}

void net_save_update(void)
{
    int i;
    int y = 19+18*3;
    for (i = 0; i < 4; i++)
    {
        int total = save_file_star_total(i);
        if (i == save_index-1)
        {
            mario->star = total;
        }
        else
        {
            static const char str[][2] = {"A", "B", "C", "D"};
            int x = total < 100;
            y -= 18;
            dprint(22, y, str[i]);
            if (x) dprint(22+16, y, "*");
            dprintf(22+16+14*x, y, "%d", total);
        }
    }
}
#endif

static void net_init(void)
{
    int i;
    shape_table[2] = &shape_np;
    np_init_self(&np_data[0]);
    for (i = 1; i < NP_LEN; i++) np_init_peer(&np_data[i]);
#ifdef CHARACTER
    character_init();
#endif
    net_mem_init();
}

static void net_update(void)
{
    int i;
    np_update_self(&np_data[0], mario);
    for (i = 1; i < NP_LEN; i++) np_update_peer(&np_data[i]);
#ifdef SAVE
    net_save_update();
#endif
}

void net_main(void)
{
    if (segment_table[0x04] != 0)
    {
        static u8 init = TRUE;
        if (init)
        {
            init = FALSE;
            net_init();
        }
        net_update();
    }
}
