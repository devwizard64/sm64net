/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                    Copyright (C) 2019 - 2021  devwizard                    *
 *        This project is licensed under the terms of the GNU General         *
 *        Public License version 2.  See LICENSE for more information.        *
 ******************************************************************************/

static uint msg_str_w(const u8 *str)
{
    uint w = 0;
    uint l = 0;
    while (*str != 0xFF)
    {
        if (*str == 0xFE)
        {
            if (w < l) w = l;
            l = 0;
        }
        l += message_kern[*str];
        str++;
    }
    if (w < l) w = l;
    return w;
}

static uint msg_str_h(const u8 *str)
{
    uint h = 0;
    while (*str != 0xFF)
    {
        if (*str == 0xFE) h += 16;
        str++;
    }
    return h;
}

extern SHAPE            shape_np;
extern SHAPE_BILLBOARD  shape_np_billboard;
extern SHAPE_SCALE      shape_np_scale;
extern SHAPE_CALLBACK   shape_np_name;
extern SHAPE            shape_np_body;
extern SHAPE_CALLBACK   shape_np_end;

static const Gfx gfx_np_name_start[] =
{
    {{0xE7000000, 0x00000000}},  /* G_RDPPIPESYNC       */
    {{0xFCFFFFFF, 0xFFFEF379}},  /* G_SETCOMBINE        */
    {{0xBA000C02, 0x00000000}},  /* G_SETOTHERMODE_H    */
    {{0xBB000001, 0xFFFFFFFF}},  /* G_TEXTURE           */
    {{0xB6000000, 0x00022204}},  /* G_CLEARGEOMETRYMODE */
    {{0xB8000000, 0x00000000}},  /* G_ENDDL             */
};

static const Gfx gfx_np_name_end[] =
{
    {{0xE7000000, 0x00000000}},  /* G_RDPPIPESYNC       */
    {{0xFCFFFFFF, 0xFFFE793C}},  /* G_SETCOMBINE        */
    {{0xBA000C02, 0x00002000}},  /* G_SETOTHERMODE_H    */
    {{0xBB000000, 0xFFFFFFFF}},  /* G_TEXTURE           */
    {{0xB7000000, 0x00022204}},  /* G_SETGEOMETRYMODE   */
    {{0xB8000000, 0x00000000}},  /* G_ENDDL             */
};

static void *np_draw(int code, SHAPE *shape, unused void *data)
{
    if (code == 1)
    {
        static OBJECT *obj_hold_prev;
        static s16 rot_head_prev[2];
        static u8 cam_mode_prev;
        SHAPE_CALLBACK *sc = (SHAPE_CALLBACK *)shape;
        PL_SHAPE *pls      = &pl_shape_table[0];
        PL_SHAPE *pls_prev = &pl_shape_table[1];
        CAMERA *cam = (CAMERA *)shape_camera->s.arg;
        OBJECT *obj = shape_object == &shape_object_mirror ?
            object_mario : (OBJECT *)shape_object;
        if (sc->arg == 0)
        {
            NET_PL *np = obj->o_np_np;
            if (obj != object_mario)
            {
                *pls_prev      = *pls;
                obj_hold_prev  = mario->obj_hold;
                *rot_head_prev = *camera_rot_head;
                cam_mode_prev  = cam->mode;
                pls->state = 0x20810446;
                pls->head  =  np->np_shape_flag_h >>  6 & 0x0003;
                pls->eye   = (np->np_shape_flag_h >>  3 & 0x0007) + 1;
                pls->glove =  np->np_shape_flag_h >>  0 & 0x0007;
                pls->wing  =  np->np_shape_flag_l >> 15 & 0x0001;
                pls->cap   =  np->np_shape_flag_l >>  0 & 0x03FF;
                pls->hold  =  np->np_shape_flag_l >> 13 & 0x0003;
                pls->punch =  np->np_shape_flag_h >>  8 & 0x00FF;
                pls->torso[0] = np->np_torso_x;
                pls->torso[1] = np->np_torso_y;
                pls->torso[2] = np->np_torso_z;
                camera_rot_head[0] = np->np_head_x;
                camera_rot_head[1] = np->np_head_y;
                mario->obj_hold = NULL;
                cam->mode = 6;
            }
            shape_np_body.child =
                (np->np_shape_flag_l & NP_SHAPE_BODY) ? np->np_shape : NULL;
            if (np->np_shape_flag_l & NP_SHAPE_NAME)
            {
                Gfx *gfx_prev;
                Gfx *gfx;
                uint size;
                uint w;
                uint h;
                (&np->np_name[0])[NP_NAME_LEN-1] = 0xFF;
                w = msg_str_w(np->np_name);
                h = msg_str_h(np->np_name);
                gfx_prev = video_gfx;
                message_print(-w >> 1, h, np->np_name);
                size = (u8 *)video_gfx - (u8 *)gfx_prev;
                video_gfx = gfx_prev;
                if ((gfx = gfx_alloc(sizeof(Gfx)*3 + size)) != NULL)
                {
                    Gfx *dst = gfx;
                    Gfx *src = gfx_prev;
                    *dst++ = (Gfx){{0x06000000, (uintptr_t)gfx_np_name_start}};
                    *dst++ = (Gfx){{0xFB000000, np->np_colour}};
                    do {*dst++ = *src++;} while ((size -= sizeof(Gfx)) > 0);
                    *dst++ = (Gfx){{0x06010000, (uintptr_t)gfx_np_name_end}};
                }
                return gfx;
            }
        }
        else
        {
            if (obj != object_mario)
            {
                *pls             = *pls_prev;
                mario->obj_hold  = obj_hold_prev;
                *camera_rot_head = *rot_head_prev;
                cam->mode        = cam_mode_prev;
            }
        }
    }
    return NULL;
}

SHAPE shape_np =
{
    S_TYPE_EMPTY, S_FLAG_ENABLED,
    &shape_np, &shape_np, NULL, &shape_np_billboard.s.s,
};

SHAPE_BILLBOARD shape_np_billboard =
{
    {
        {
            S_TYPE_BILLBOARD, S_FLAG_ENABLED,
            &shape_np_end.s, &shape_np_body, &shape_np, &shape_np_scale.s.s,
        },
        NULL,
    },
    {0, 160, 0},
};

SHAPE_SCALE shape_np_scale =
{
    {
        {
            S_TYPE_SCALE, S_FLAG_ENABLED,
            &shape_np_scale.s.s, &shape_np_scale.s.s,
            &shape_np_billboard.s.s, &shape_np_name.s,
        },
        NULL,
    },
    4,
};

SHAPE_CALLBACK shape_np_name =
{
    {
        S_TYPE_CALLBACK, S_LAYER_TEX_EDGE << 8 | S_FLAG_ENABLED,
        &shape_np_name.s, &shape_np_name.s, &shape_np_scale.s.s, NULL,
    },
    np_draw, 0,
};

SHAPE shape_np_body =
{
    S_TYPE_EMPTY, S_FLAG_ENABLED,
    &shape_np_billboard.s.s, &shape_np_end.s, &shape_np, NULL,
};

SHAPE_CALLBACK shape_np_end =
{
    {
        S_TYPE_CALLBACK, S_LAYER_TEX_EDGE << 8 | S_FLAG_ENABLED,
        &shape_np_body, &shape_np_billboard.s.s, &shape_np, NULL,
    },
    np_draw, 1,
};
