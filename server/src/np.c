/******************************************************************************/
/*                  SM64Net - An Internet framework for SM64                  */
/*                    Copyright (C) 2019 - 2021  devwizard                    */
/*        This project is licensed under the terms of the GNU General         */
/*        Public License version 2.  See LICENSE for more information.        */
/******************************************************************************/

#ifdef LOCATION
static const char *const str_stage_table[] =
{
    NULL,       /*  1                                   */
    NULL,       /*  2                                   */
    NULL,       /*  3                                   */
    "BBH",      /*  4  Big Boo's Haunt                  */
    "CCM",      /*  5  Cool, Cool Mountain              */
    "Lobby",    /*  6  Castle Inside                    */
    "HMC",      /*  7  Hazy Maze Cave                   */
    "SSL",      /*  8  Shifting Sand Land               */
    "BoB",      /*  9  Bob-omb Battlefield              */
    "SL",       /* 10  Snowman's Land                   */
    "WDW",      /* 11  Wet-Try Dorld                    */
    "JRB",      /* 12  Jolly Roger Bay                  */
    "THI Huge", /* 13  Tiny-Huge Island                 */
    "TTC",      /* 14  Tick Tock Cock                   */
    "RR",       /* 15  Rainbow Ride                     */
    "Outside",  /* 16  Castle Grounds                   */
    "BitDW",    /* 17  Bowser in the Dark World         */
    "VCutM",    /* 18  Vanish Cap under the Moat        */
    "BitFS",    /* 19  Bowser in the Fire Sea           */
    "SA",       /* 20  The Secret Aquarium              */
    "BitS",     /* 21  Bowser in the Sky                */
    "LLL",      /* 22  Lethal Lava Land                 */
    "DDD",      /* 23  Dire, Dire Docks                 */
    "WF",       /* 24  Whomp's Fortress                 */
    "The End",  /* 25  The End                          */
    "Crtyrd",   /* 26  Castle Courtyard                 */
    "PSS",      /* 27  The Princess's Secret Slide      */
    "CotMC",    /* 28  Cavern of the Metal Cap          */
    "TotWC",    /* 29  Tower of the Wing Cap            */
    "BitDW A",  /* 30  Bowser in the Dark World (Arena) */
    "WMotR",    /* 31  Wing Mario over the Rainbow      */
    NULL,       /* 32                                   */
    "BitFS A",  /* 33  Bowser in the Fire Sea (Arena)   */
    "BitS A",   /* 34  Bowser in the Sky (Arena)        */
    NULL,       /* 35                                   */
    "TTM",      /* 36  Tall, Tall Mountain              */
    NULL,       /* 37                                   */
    NULL,       /* 38                                   */
};

static s16 location_y = 0;
static u8  location_start = 1;
static u8  location_index = 1;
static u8  location_draw = 0;
#endif

static void interpolate_f(f32 *dst, const f32 *src, uint len, uint delay)
{
    float div = (delay > 0 ? delay : 1) * (3/2.0F);
    do
    {
        float d = *src - *dst;
        if (d > -8 && d < 8)    *dst = *src;
        else                    *dst += d / div;
        dst++;
        src++;
    }
    while (--len > 0);
}

static void interpolate_s(s16 *dst, const s16 *src, uint len, uint delay)
{
    int div = (delay > 0 ? delay : 1) * 3/2;
    do
    {
        s16 d = *src - *dst;
        if (d > -0x40 && d < 0x40)  *dst = *src;
        else                        *dst += d / div;
        dst++;
        src++;
    }
    while (--len > 0);
}

static void np_init_self(NET_PL *np)
{
    np->np_timer        = NP_TIMEOUT;
    np->np_shape_flag_l = NP_SHAPE_BODY | NP_SHAPE_NAME;
    np->np_obj          = NULL;
#ifdef CHARACTER
    np->np_shape_index  = 0;
#endif
    while (np->np_tcp_id != 0);
    np->np_tcp_id = NP_CMD_SYNC;
}

static void np_init_peer(NET_PL *np)
{
    np->np_timer        = 0;
#ifdef CHARACTER
    np->np_shape_index  = 0;
#endif
    np->np_obj          = NULL;
    np->np_anime        = NULL;
    np->np_anime_src    = -1;
}

static void np_free(NET_PL *np)
{
    if (np->np_obj != NULL)
    {
        ((OBJECT *)np->np_obj)->flag = 0;
        np->np_obj = NULL;
    }
    if (np->np_anime != NULL)
    {
        net_free(np->np_anime);
        np->np_anime = NULL;
        np->np_anime_src = 0xFF;
    }
}

static void np_update(NET_PL *np, OBJECT *obj)
{
    obj->o_np_np = np;
#ifdef CHARACTER
    obj->list.s.skeleton.waist =
        net_chr_table[np->np_shape_index].waist;
#endif
}

static void np_update_self(NET_PL *np, PLAYER *pl)
{
    int i;
    int eye;
    PL_SHAPE *pls = pl->shape;
    OBJECT   *obj = pl->obj;
    if (pls == NULL || obj == NULL) return;
    np->np_pos_x   = obj->list.s.pos[0];
    np->np_pos_y   = obj->list.s.pos[1];
    np->np_pos_z   = obj->list.s.pos[2];
    np->np_scale_x = obj->list.s.scale[0];
    np->np_scale_y = obj->list.s.scale[1];
    np->np_scale_z = obj->list.s.scale[2];
    np->np_rot_x   = obj->list.s.rot[0];
    np->np_rot_y   = obj->list.s.rot[1];
    np->np_rot_z   = obj->list.s.rot[2];
    np->np_torso_x = pls->torso[0];
    np->np_torso_y = pls->torso[1];
    np->np_torso_z = pls->torso[2];
    /*
    This fixes a bug where if we leave first person, our head is stuck rotated
    for everyone else's client.  Of course, we allow our head to be rotated
    for the star dance state, for obvious reasons.
    */
    /* first person / star dance */
    if (
        pl->state == PL_WAIT_39 ||
        (pl->state_prev == PL_WAIT_39 && pl->state == PL_DEMO_2)
    )
    {
        np->np_head_x = camera_rot_head[0];
        np->np_head_y = camera_rot_head[1];
    }
    else
    {
        np->np_head_x = 0;
        np->np_head_y = 0;
    }
    if (pls->eye == PL_EYE_BLINK)
    {
        i = draw_timer >> 1 & 0x1F;
        eye = i < (int)lenof(pl_shape_blink) ? pl_shape_blink[i] : 0;
    }
    else
    {
        eye = pls->eye - 1;
    }
    np->np_shape_flag_h  = pls->punch <<  8 & 0xFF00;
    np->np_shape_flag_h |= pls->head  <<  6 & 0x00C0;
    np->np_shape_flag_h |= eye        <<  3 & 0x0038;
    np->np_shape_flag_h |= pls->glove <<  0 & 0x0007;
    np->np_shape_flag_l &= NP_SHAPE_BODY | NP_SHAPE_NAME;
    np->np_shape_flag_l |= pls->wing  << 15 & 0x8000;
    np->np_shape_flag_l |= pls->hold  << 13 & 0x6000;
    np->np_shape_flag_l |= pls->cap   <<  0 & 0x03FF;
    np->np_anime_frame_amt = obj->list.s.skeleton.frame_amt;
    np->np_anime_frame_vel = obj->list.s.skeleton.frame_vel;
    np->np_anime_dst       = obj->list.s.skeleton.index;
    if (np->np_stage != stage_index || np->np_scene != scene_index)
    {
        while (np->np_tcp_id != 0);
        np->np_tcp_id = NP_CMD_SYNC;
    }
    np->np_stage = stage_index;
    np->np_scene = scene_index;
    np->np_obj   = obj;
    obj->list.s.shape = &shape_np;
#ifdef LOCATION
    location_draw ^= pl->cont->down & L_TRIG;
    if (location_draw)
    {
        if (pl->cont->down & D_JPAD)
        {
            if (location_start > 1) location_start--;
        }
        if (pl->cont->down & U_JPAD)
        {
            if (location_start < NP_LEN) location_start++;
        }
        location_y = 20 + 18*9;
        dprintf(320-8 - 12*9, location_y, "%2d online", location_index);
        location_index = 1;
    }
#endif
#ifdef CHARACTER
    if (pl->cont->down & L_JPAD)
    {
        if (np->np_shape_index > 0)
        {
            np->np_shape_index--;
            while (np->np_tcp_id != 0);
            np->np_tcp_id = NP_CMD_SYNC;
        }
    }
    if (pl->cont->down & R_JPAD)
    {
        if (np->np_shape_index < (int)lenof(net_chr_table)-1)
        {
            np->np_shape_index++;
            while (np->np_tcp_id != 0);
            np->np_tcp_id = NP_CMD_SYNC;
        }
    }
#endif
#ifdef COLLISION
    for (i = 1; i < NP_LEN; i++)
    {
        NET_PL *n = &np_data[i];
        if (n->np_col != 0)
        {
            n->np_col = 0;
            if (!(pl->state & 0x00020000))
            {
                OBJECT *obj = np_data[i].np_obj;
                if (obj != NULL)
                {
                    obj->o_col_arg = 0;
                    pl_collision_attacked(pl, obj);
                    obj->o_col_arg = 2;
                }
            }
        }
    }
#endif
    np_update(np, obj);
#ifdef CHARACTER
    pl->waist = obj->list.s.skeleton.waist;
#endif
}

static void np_update_peer(NET_PL *np)
{
    OBJECT *obj;
    /*
    If player is not spawned, that means we currently are not in a stage.  If we
    are not in a stage, our obj ptr is stale, and should be cleared.
    */
    if (obj_mario == NULL)
    {
        np->np_obj = NULL;
        return;
    }
#ifdef LOCATION
    if (location_draw && np->np_timer > 0)
    {
        if (location_index >= location_start && location_y >= 20)
        {
            const char *str = str_stage_table[np->np_stage-1];
            switch (np->np_stage << 4 | np->np_scene)
            {
                case  5 << 4 | 2: str = "CCM Sld";  break;
                case  8 << 4 | 2: str = "Pyramid";  break;
                case  6 << 4 | 2: str = "Upstairs"; break;
                case  6 << 4 | 3: str = "Basement"; break;
                case 12 << 4 | 2: str = "JRB Ship"; break;
                case 13 << 4 | 2: str = "THI Tiny"; break;
                case 13 << 4 | 3: str = "Wiggler";  break;
                case 22 << 4 | 2: str = "Volcano";  break;
                case 36 << 4 | 2:
                case 36 << 4 | 3:
                case 36 << 4 | 4: str = "TTM Sld";  break;
            }
            if (str == NULL) str = "???";
            dprintf(8, location_y, "%2d", location_index);
            dprint(8 + 12*3, location_y, str);
            location_y -= 18;
        }
        location_index++;
    }
#endif
    if (
        np->np_stage != stage_index ||
        np->np_scene != scene_index ||
        np->np_timer == 0
    )
    {
        np_free(np);
        return;
    }
    if (np->np_anime_dst != np->np_anime_src)
    {
        FILE_TABLE *ft = mario->file_anime->table;
        if (np->np_anime != NULL)
        {
            net_free(np->np_anime);
            np->np_anime = NULL;
        }
        np->np_anime_src = 0xFF;
        if (np->np_anime_dst < ft->len)
        {
            ANIME *anime;
            if ((anime = net_alloc(ft->table[np->np_anime_dst].size)) != NULL)
            {
                const char *start = ft->src + ft->table[np->np_anime_dst].start;
                const char *end   = start + ft->table[np->np_anime_dst].size;
                mem_dma((char *)anime, start, end);
                anime->val = (s16 *)((char *)anime + (uintptr_t)anime->val);
                anime->tbl = (u16 *)((char *)anime + (uintptr_t)anime->tbl);
                np->np_anime = anime;
                np->np_anime_src = np->np_anime_dst;
            }
        }
    }
    if ((obj = np->np_obj) == NULL)
    {
        obj = np->np_obj = obj_create(
            obj_mario, 0, 2, (void *)K0_TO_PHYS(o_np)
        );
        obj->list.s.rot[0] = np->np_rot_x;
        obj->list.s.rot[1] = np->np_rot_y;
        obj->list.s.rot[2] = np->np_rot_z;
        obj->list.s.pos[0] = np->np_pos_x;
        obj->list.s.pos[1] = np->np_pos_y;
        obj->list.s.pos[2] = np->np_pos_z;
    }
    interpolate_s(obj->list.s.rot, &np->np_rot, 3, np->np_timer_delta);
    interpolate_f(obj->list.s.pos, &np->np_pos, 3, np->np_timer_delta);
    obj->list.s.scale[0] = np->np_scale_x;
    obj->list.s.scale[1] = np->np_scale_y;
    obj->list.s.scale[2] = np->np_scale_z;
    obj->list.s.skeleton.index = np->np_anime_src;
#ifndef CHARACTER
    obj->list.s.skeleton.waist = mario->waist;
#endif
    obj->list.s.skeleton.anime = np->np_anime;
    if (np->np_timer == NP_TIMEOUT)
    {
        int delta;
        obj->list.s.skeleton.frame     = np->np_anime_frame_amt >> 16;
        obj->list.s.skeleton.frame_amt = np->np_anime_frame_amt;
        obj->list.s.skeleton.frame_vel = np->np_anime_frame_vel;
        delta = np->np_timer - np->np_timer_prev;
        np->np_timer_delta = delta < NP_DELTA_MAX ? delta : NP_DELTA_MAX;
    }
    else
    {
        np->np_timer_prev = np->np_timer;
    }
    if (np->np_timer < 16)
    {
        np->np_shape_flag_l &= 0xFC00;
        np->np_shape_flag_l |= PL_CAP_VANISH << 8;
        np->np_shape_flag_l |= 16*np->np_timer - 8;
        obj->list.s.scale[0] *= 0.50F + (1.0F/32)*np->np_timer;
        obj->list.s.scale[2] *= 0.50F + (1.0F/32)*np->np_timer;
        obj->list.s.scale[1] *= 0.75F + 4.0F/np->np_timer;
    }
    np->np_timer--;
    np_update(np, obj);
}
