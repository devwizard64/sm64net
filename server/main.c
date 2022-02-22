/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                    Copyright (C) 2019 - 2021  devwizard                    *
 *        This project is licensed under the terms of the GNU General         *
 *        Public License version 2.  See LICENSE for more information.        *
 ******************************************************************************/

#include <types.h>
#include <sm64.h>
#include <sm64net.h>

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

static void np_main(void);
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

static void *np_draw(int mode, SHAPE *shape, void *data);

extern SHAPE            shape_np;
extern SHAPE_BILLBOARD  shape_np_billboard;
extern SHAPE_SCALE      shape_np_scale;
extern SHAPE_CALLBACK   shape_np_name;
extern SHAPE            shape_np_body;
extern SHAPE_CALLBACK   shape_np_end;

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
static u8  location_draw = false;

static void np_update_self_location(unused NET_PL *np, PLAYER *pl)
{
    if (pl->cont->down & L_TRIG) location_draw ^= true;
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
}

static void np_update_peer_location(NET_PL *np)
{
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
}
#else
#define np_update_self_location(np, pl)
#define np_update_peer_location(np)
#endif

#ifdef CHARACTER
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
    {NULL,      189},
    {s_kirby, 150},
    {s_meta,  150},
    {s_hrdee, 150},
};

static SHAPE *net_shape_table[lenof(net_chr_table)];

static void np_init_self_character(NET_PL *np)
{
    np->np_shape_index = 0;
}

static void np_update_character(NET_PL *np, unused OBJECT *obj)
{
    np->np_waist = net_chr_table[np->np_shape_index].waist;
    np->np_shape = net_shape_table[np->np_shape_index];
}

static void np_update_self_character(NET_PL *np, PLAYER *pl)
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

static void net_init_character(void)
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
#else
#define np_init_self_character(np)
#define np_update_character(np, obj)
#define np_update_self_character(np, pl)
#define net_init_character()
#endif

#ifdef COLLISION
#define NP_CMD_COL      2

#define /* 0x022D */    np_col          tcp[0x0B].u8[1]

static void np_update_self_collision(unused NET_PL *np, PLAYER *pl)
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
#else
#define np_update_self_collision(np, pl)
#endif

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

static void *np_draw(int mode, SHAPE *shape, unused void *data)
{
    if (mode == 1)
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
    if ((object->o_colflag & 0xFF) != 0)
    {
        np_table[0].np_col    = np-np_table;
        np_table[0].np_tcp_id = NP_CMD_COL;
    }
    object->o_colflag = 0;
#endif
}

extern struct
{
    u8 page[NET_PAGE_LEN];
    u8 data[NET_PAGE_LEN][NET_PAGE_SIZE];
}
net_heap;

static void *net_alloc(size_t size)
{
    uint pages;
    uint page;
    uint i;
    pages = (size+sizeof(net_heap.data[0])-1) / sizeof(net_heap.data[0]);
    if (pages >= 0x100) return NULL;
    for (page = 0; page < lenof(net_heap.page); page++)
    {
        for (i = 0; i < pages; i++)
        {
            if (net_heap.page[page+i] > 0) goto end;
        }
        net_heap.page[page] = pages;
        for (i = 1; i < pages; i++) net_heap.page[page+i] = 1;
        return &net_heap.data[page];
    end:;
    }
    return NULL;
}

static void net_free(void *ptr)
{
    uint pages;
    uint page;
    uint i;
    page = (typeof(&net_heap.data[0]))ptr - net_heap.data;
    pages = net_heap.page[page];
    for (i = 0; i < pages; i++) net_heap.page[page+i] = 0;
}

static void np_interpolate_f32(f32 *dst, const f32 *src, uint len, uint delay)
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

static void np_interpolate_s16(s16 *dst, const s16 *src, uint len, uint delay)
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
    np_init_self_character(np);
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

static void np_destroy(NET_PL *np)
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
    np_update_character(np, obj);
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
    np_update_self_location(np, pl);
    np_update_self_character(np, pl);
    np_update_self_collision(np, pl);
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
    np_update_peer_location(np);
    if (
        np->np_stage != stage_index ||
        np->np_scene != scene_index ||
        np->np_timer == 0
    )
    {
        np_destroy(np);
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
    np_interpolate_s16(obj->list.s.rot, &np->np_rot, 3, np->np_timer_delta);
    np_interpolate_f32(obj->list.s.pos, &np->np_pos, 3, np->np_timer_delta);
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

static void net_init(void)
{
    uint i;
    shape_table[2] = &shape_np;
    np_init_self(&np_table[0]);
    for (i = 1; i < NP_LEN; i++) np_init_peer(&np_table[i]);
    net_init_character();
    bzero(net_heap.page, sizeof(net_heap.page));
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
