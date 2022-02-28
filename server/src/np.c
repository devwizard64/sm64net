/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                    Copyright (C) 2019 - 2021  devwizard                    *
 *        This project is licensed under the terms of the GNU General         *
 *        Public License version 2.  See LICENSE for more information.        *
 ******************************************************************************/

static void interpolate_f(f32 *dst, const f32 *src, uint len, uint delay)
{
    f32 div = (delay > 0 ? delay : 1) * (3/2.0F);
    do
    {
        f32 d = *src - *dst;
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
    np->np_shape        = shape_table[1];
    np->np_obj          = NULL;
#ifdef CHARACTER
    character_np_init_self(np);
#endif
    np->np_tcp_id = NP_CMD_SYNC;
}

static void np_init_peer(NET_PL *np)
{
    np->np_timer     = 0;
    np->np_shape     = shape_table[1];
    np->np_obj       = NULL;
    np->np_anime     = NULL;
    np->np_anime_src = -1;
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
    character_np_update(np, obj);
#endif
}

static void np_update_self(NET_PL *np, PLAYER *pl)
{
    PL_SHAPE *pls = pl->shape;
    OBJECT   *obj = pl->obj;
    int eye;
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
        pl->state == 0x0C000227 ||
        (pl->state_prev == 0x0C000227 && pl->state == 0x00001302)
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
        uint i = shape_timer >> 1 & 0x1F;
        eye = i < lenof(pl_shape_blink) ? pl_shape_blink[i] : 0;
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
        np->np_tcp_id = NP_CMD_SYNC;
    }
    np->np_stage = stage_index;
    np->np_scene = scene_index;
    np->np_obj   = obj;
    pl->waist = np->np_waist;
    obj->list.s.list = &shape_np;
    obj->list.s.skeleton.waist = np->np_waist;
#ifdef LOCATION
    location_np_update_self(np, pl);
#endif
#ifdef CHARACTER
    character_np_update_self(np, pl);
#endif
#ifdef COLLISION
    collision_np_update_self(np, pl);
#endif
    np_update(np, obj);
}

static void np_update_peer(NET_PL *np)
{
    OBJECT *obj;
    /*
    If player is not spawned, that means we currently are not in a stage.  If we
    are not in a stage, our obj ptr is stale, and should be cleared.
    */
    if (object_mario == NULL)
    {
        np->np_obj = NULL;
        return;
    }
#ifdef LOCATION
    location_np_update_peer(np);
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
                u8 *start = ft->src + ft->table[np->np_anime_dst].start;
                u8 *end   = start + ft->table[np->np_anime_dst].size;
                mem_dma(anime, start, end);
                anime->val = (s16 *)((u8 *)anime + (uintptr_t)anime->val);
                anime->tbl = (u16 *)((u8 *)anime + (uintptr_t)anime->tbl);
                np->np_anime = anime;
                np->np_anime_src = np->np_anime_dst;
            }
        }
    }
    if ((obj = np->np_obj) == NULL)
    {
        obj = np->np_obj = obj_create(
            object_mario, 0, 2, (void *)K0_TO_PHYS(o_np)
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
    obj->list.s.skeleton.waist = np->np_waist;
    obj->list.s.skeleton.anime = np->np_anime;
    if (np->np_timer == NP_TIMEOUT)
    {
        uint delta;
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
