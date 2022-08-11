/******************************************************************************/
/*                  SM64Net - An Internet framework for SM64                  */
/*                    Copyright (C) 2019 - 2021  devwizard                    */
/*        This project is licensed under the terms of the GNU General         */
/*        Public License version 2.  See LICENSE for more information.        */
/******************************************************************************/

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
    gsDPPipeSync(),
    gsDPSetCombineLERP(
        0, 0, 0, ENVIRONMENT, 0, 0, 0, TEXEL0,
        0, 0, 0, ENVIRONMENT, 0, 0, 0, TEXEL0
    ),
    gsDPSetTextureFilter(G_TF_POINT),
    gsSPTexture(0xFFFF, 0xFFFF, G_TX_NOLOD, G_TX_RENDERTILE, G_ON),
    gsSPClearGeometryMode(G_SHADE|G_SHADING_SMOOTH|G_CULL_BACK|G_LIGHTING),
    gsSPEndDisplayList(),
};

static const Gfx gfx_np_name_end[] =
{
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsDPSetTextureFilter(G_TF_BILERP),
    gsSPTexture(0xFFFF, 0xFFFF, G_TX_NOLOD, G_TX_RENDERTILE, G_OFF),
    gsSPSetGeometryMode(G_SHADE|G_SHADING_SMOOTH|G_CULL_BACK|G_LIGHTING),
    gsSPEndDisplayList(),
};

static void *np_draw(int code, SHAPE *shape, UNUSED void *data)
{
    if (code == S_CODE_DRAW)
    {
        static OBJECT *obj_hold_prev;
        static s16 rot_head_prev[2];
        static u8 cam_mode_prev;
        SHAPE_CALLBACK *sc = (SHAPE_CALLBACK *)shape;
        PL_SHAPE *pls      = &pl_shape_data[0];
        PL_SHAPE *pls_prev = &pl_shape_data[1];
        CAMERA *cam = (CAMERA *)shape_camera->s.arg;
        OBJECT *obj = shape_object == &shape_object_mirror ?
            obj_mario : (OBJECT *)shape_object;
        if (sc->arg == 0)
        {
            NET_PL *np = obj->o_np_np;
            if (obj != obj_mario)
            {
                *pls_prev      = *pls;
                obj_hold_prev  = mario->obj_hold;
                *rot_head_prev = *camera_rot_head;
                cam_mode_prev  = cam->mode;
                pls->state = PL_JUMP_6;
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
            if (np->np_shape_flag_l & NP_SHAPE_BODY)
            {
#ifdef CHARACTER
                shape_np_body.child = net_shape_table[np->np_shape_index];
#else
                shape_np_body.child = shape_table[1];
#endif
            }
            else
            {
                shape_np_body.child = NULL;
            }
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
                message_print(-(w/2), h, np->np_name);
                size = (char *)video_gfx - (char *)gfx_prev;
                video_gfx = gfx_prev;
                if ((gfx = gfx_alloc(sizeof(Gfx)*3 + size)) != NULL)
                {
                    Gfx *dst = gfx;
                    Gfx *src = gfx_prev;
                    gSPDisplayList(dst++, gfx_np_name_start);
                    gDPSetColor(dst++, G_SETENVCOLOR, np->np_colour);
                    do {*dst++ = *src++;} while ((size -= sizeof(Gfx)) > 0);
                    gSPBranchList(dst++, gfx_np_name_end);
                }
                return gfx;
            }
        }
        else
        {
            if (obj != obj_mario)
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
    S_TYPE_EMPTY, S_FLAG_ACTIVE,
    &shape_np, &shape_np, NULL, &shape_np_billboard.s.s,
};

SHAPE_BILLBOARD shape_np_billboard =
{
    {
        {
            S_TYPE_BILLBOARD, S_FLAG_ACTIVE,
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
            S_TYPE_SCALE, S_FLAG_ACTIVE,
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
        S_TYPE_CALLBACK, S_LAYER_TEX_EDGE << 8 | S_FLAG_ACTIVE,
        &shape_np_name.s, &shape_np_name.s, &shape_np_scale.s.s, NULL,
    },
    np_draw, 0,
};

SHAPE shape_np_body =
{
    S_TYPE_EMPTY, S_FLAG_ACTIVE,
    &shape_np_billboard.s.s, &shape_np_end.s, &shape_np, NULL,
};

SHAPE_CALLBACK shape_np_end =
{
    {
        S_TYPE_CALLBACK, S_LAYER_TEX_EDGE << 8 | S_FLAG_ACTIVE,
        &shape_np_body, &shape_np_billboard.s.s, &shape_np, NULL,
    },
    np_draw, 1,
};
