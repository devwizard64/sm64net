/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                    Copyright (C) 2019 - 2021  devwizard                    *
 *        This project is licensed under the terms of the GNU General         *
 *        Public License version 2.  See LICENSE for more information.        *
 ******************************************************************************/

#include <types.h>
#include <sm64.h>
#include <sm64net.h>

extern struct
{
    u8 page[NM_PAGE_LEN];
    u8 data[NM_PAGE_LEN][NM_PAGE_SIZE];
}
nm_heap;

extern struct g_t g_np;
extern struct g_billboard_t g_np_billboard;
extern struct g_scale_t g_np_scale;
extern struct gc_t g_np_name;
extern struct g_t g_np_gfx;
extern struct gc_t g_np_end;

static void *g_np_main(int mode, struct g_t *g, void *data);
static void o_np_main(void);

static const Gfx gfx_np_name_start[] =
{
    {{0xE7000000, 0x00000000}},  /* G_RDPPIPESYNC       */
    {{0xFC12FE25, 0xFFFFF3F9}},  /* G_SETCOMBINE        */
    {{0xBA000C02, 0x00000000}},  /* G_SETOTHERMODE_H    */
    {{0xBB000001, 0xFFFFFFFF}},  /* G_TEXTURE           */
    {{0xB6000000, 0x00020000}},  /* G_CLEARGEOMETRYMODE */
    {{0xB8000000, 0x00000000}},  /* G_ENDDL             */
};

static const Gfx gfx_np_name_end[] =
{
    {{0xE7000000, 0x00000000}},  /* G_RDPPIPESYNC       */
    {{0xFCFFFFFF, 0xFFFE793C}},  /* G_SETCOMBINE        */
    {{0xBA000C02, 0x00002000}},  /* G_SETOTHERMODE_H    */
    {{0xBB000000, 0xFFFFFFFF}},  /* G_TEXTURE           */
    {{0xB7000000, 0x00020000}},  /* G_SETGEOMETRYMODE   */
    {{0xB8000000, 0x00000000}},  /* G_ENDDL             */
};

static const uintptr_t o_np[] =
{
    0x00040000,
#ifdef _TOUCH
    0x23000000, 0x002500A0,
    0x2F000000, 0x00400000,
    0x10050000,
    0x103E0001,
    0x10420002,
#endif
    0x08000000,
        0x0C000000, (uintptr_t)o_np_main,
    0x09000000,
};

struct g_t g_np =
{
    G_T_GFX, G_F_ENABLED,
    &g_np, &g_np, NULL, &g_np_billboard.g.g,
};

struct g_billboard_t g_np_billboard =
{
    {
        {
            G_T_BILLBOARD, G_F_ENABLED,
            &g_np_end.g, &g_np_gfx, &g_np, &g_np_scale.g.g,
        },
        NULL,
    },
    {0, 160, 0},
};

struct g_scale_t g_np_scale =
{
    {
        {
            G_T_SCALE, G_F_ENABLED,
            &g_np_scale.g.g, &g_np_scale.g.g, &g_np_billboard.g.g, &g_np_name.g,
        },
        NULL,
    },
    4,
};

struct gc_t g_np_name =
{
    {
        G_T_CALLBACK, G_R_SPRITE << 8 | G_F_ENABLED,
        &g_np_name.g, &g_np_name.g, &g_np_scale.g.g, NULL,
    },
    g_np_main, 0,
};

struct g_t g_np_gfx =
{
    G_T_GFX, G_F_ENABLED,
    &g_np_billboard.g.g, &g_np_end.g, &g_np, NULL,
};

struct gc_t g_np_end =
{
    {
        G_T_CALLBACK, G_R_SPRITE << 8 | G_F_ENABLED,
        &g_np_gfx, &g_np_billboard.g.g, &g_np, NULL,
    },
    g_np_main, 1,
};

static s16  net_rot_head_backup[2];
static u8   net_cam_mode_backup;
static bool net_boot = true;

#ifdef _LOCATION
static const char *const str_stage_table[] =
{
    /* 0x00 */  NULL,           /*  0                                         */
    /* 0x01 */  NULL,           /*  1                                         */
    /* 0x02 */  NULL,           /*  2                                         */
    /* 0x03 */  NULL,           /*  3                                         */
    /* 0x04 */  "BBH",          /*  4  Big Boo's Haunt                        */
    /* 0x05 */  "CCM",          /*  5  Cool, Cool Mountain                    */
    /* 0x06 */  NULL,           /*  6  Castle Inside                          */
    /* 0x07 */  "HMC",          /*  7  Hazy Maze Cave                         */
    /* 0x08 */  "SSL",          /*  8  Shifting Sand Land                     */
    /* 0x09 */  "BoB",          /*  9  Bob-omb Battlefield                    */
    /* 0x0A */  "SL",           /* 10  Snowman's Land                         */
    /* 0x0B */  "WDW",          /* 11  Wet-Try Dorld                          */
    /* 0x0C */  "JRB",          /* 12  Jolly Roger Bay                        */
    /* 0x0D */  NULL,           /* 13  Tiny-Huge Island                       */
    /* 0x0E */  "TTC",          /* 14  Tick Tock Cock                         */
    /* 0x0F */  "RR",           /* 15  Rainbow Ride                           */
    /* 0x10 */  "Outside",      /* 16  Castle Outside                         */
    /* 0x11 */  "BitDW",        /* 17  Bowser in the Dark World               */
    /* 0x12 */  "VCutM",        /* 18  Vanish Cap under the Moat              */
    /* 0x13 */  "BitFS",        /* 19  Bowser in the Fire Sea                 */
    /* 0x14 */  "SA",           /* 20  The Secret Aquarium                    */
    /* 0x15 */  "BitS",         /* 21  Bowser in the Sky                      */
    /* 0x16 */  "LLL",          /* 22  Lethal Lava Land                       */
    /* 0x17 */  "DDD",          /* 23  Dire, Dire Docks                       */
    /* 0x18 */  "WF",           /* 24  Whomp's Fortress                       */
    /* 0x19 */  "The End",      /* 25  The End                                */
    /* 0x1A */  "Courtyard",    /* 26  Castle Courtyard                       */
    /* 0x1B */  "PSS",          /* 27  The Princess's Secret Slide            */
    /* 0x1C */  "CotMC",        /* 28  Cavern of the Metal Cap                */
    /* 0x1D */  "TotWC",        /* 29  Tower of the Wing Cap                  */
    /* 0x1E */  "BitDW Fight",  /* 30  Bowser in the Dark World (Fight)       */
    /* 0x1F */  "WMotR",        /* 31  Wing Mario over the Rainbow            */
    /* 0x20 */  NULL,           /* 32                                         */
    /* 0x21 */  "BitFS Fight",  /* 33  Bowser in the Fire Sea (Fight)         */
    /* 0x22 */  "BitS Fight",   /* 34  Bowser in the Sky (Fight)              */
    /* 0x23 */  NULL,           /* 35                                         */
    /* 0x24 */  "TTM",          /* 36  Tall, Tall Mountain                    */
    /* 0x25 */  NULL,           /* 37                                         */
    /* 0x26 */  NULL,           /* 38                                         */
};  /* 0x27 */                  /* 39                                         */

static s16  location_y = 0;
static u8   location_start = 1;
static u8   location_index = 1;
static bool location_draw = false;

static void np_update_self_location(unused struct np_t *np, struct player_t *pl)
{
    if (pl->controller->down & L_TRIG)
    {
        location_draw ^= true;
    }
    if (location_draw)
    {
        if (pl->controller->down & D_JPAD)
        {
            if (location_start > 1)
            {
                location_start--;
            }
        }
        if (pl->controller->down & U_JPAD)
        {
            if (location_start < NP_LEN)
            {
                location_start++;
            }
        }
        location_y = 20 + 18*9;
        print_int(320-8 - 12*9, location_y, "%2d online", location_index);
        location_index = 1;
    }
}

static void np_update_peer_location(struct np_t *np)
{
    if (location_draw && np->np_timer > 0)
    {
        if (location_index >= location_start && location_y >= 20)
        {
            const char *str = str_stage_table[np->np_stage];
            switch (np->np_stage << 4 | np->np_world)
            {
                case 0x052: str = "CCM Slide";  break;
                case 0x082: str = "Pyramid";    break;
                case 0x0C2: str = "JRB Ship";   break;
                case 0x061: str = "Lobby";      break;
                case 0x062: str = "Upstairs";   break;
                case 0x063: str = "Basement";   break;
                case 0x0D1: str = "THI Big";    break;
                case 0x0D2: str = "THI Small";  break;
                case 0x0D3: str = "Wiggler";    break;
                case 0x162: str = "Volcano";    break;
                case 0x242:
                case 0x243:
                case 0x244: str = "TTM Slide";  break;
            }
            if (str == NULL)
            {
                str = "???";
            }
            print_int(8, location_y, "%2d", location_index);
            print(8 + 12*3, location_y, str);
            location_y -= 18;
        }
        location_index++;
    }
}
#else
#define np_update_self_location(np, pl)
#define np_update_peer_location(np)
#endif

#ifdef _CHARACTER
#define /* 0x022C */    np_gfx_index        tcp[0x0B].u8[0]

struct character_t
{
    const uintptr_t *script;
    s16 height;
};

extern const uintptr_t gfx_metaknight[];
extern const uintptr_t gfx_bandanadee[];

extern u8 net_gfx_arena_start[];
extern u8 net_gfx_arena_end[];

static const struct character_t net_character_table[] =
{
    {NULL,           189},
    {gfx_metaknight, 100},
    {gfx_bandanadee, 100},
};

static struct g_t *net_gfx_table[lenof(net_character_table)];

static void np_init_self_character(struct np_t *np)
{
    np->np_gfx_index = 0;
}

static void np_update_character(struct np_t *np, unused struct object_t *obj)
{
    np->np_motion_height = net_character_table[np->np_gfx_index].height;
    np->np_gfx = net_gfx_table[np->np_gfx_index];
}

static void np_update_self_character(struct np_t *np, struct player_t *pl)
{
    if (pl->controller->down & L_JPAD)
    {
        if (np->np_gfx_index > 0)
        {
            np->np_gfx_index--;
            np->np_tcp_id = NP_CMD_SYNC;
        }
    }
    if (pl->controller->down & R_JPAD)
    {
        if (np->np_gfx_index < lenof(net_character_table)-1)
        {
            np->np_gfx_index++;
            np->np_tcp_id = NP_CMD_SYNC;
        }
    }
}

static void net_init_character(void)
{
    struct arena_t arena;
    uint i;
    arena.size  = net_gfx_arena_end - net_gfx_arena_start;
    arena.used  = 0;
    arena.start = net_gfx_arena_start;
    arena.free  = net_gfx_arena_start;
    net_gfx_table[0] = world_gfx_table[0x01];
    for (i = 1; i < lenof(net_character_table); i++)
    {
        net_gfx_table[i] = script_g_main(&arena, net_character_table[i].script);
    }
}
#else
#define np_init_self_character(np)
#define np_update_character(np)
#define np_update_self_character(np)
#define net_init_character()
#endif

static uint menu_str_w(const u8 *str)
{
    uint w = 0;
    uint l = 0;
    while (*str != 0xFF)
    {
        if (*str == 0xFE)
        {
            if (w < l)
            {
                w = l;
            }
            l = 0;
        }
        l += message_kern[*str];
        str++;
    }
    if (w < l)
    {
        w = l;
    }
    return w;
}

static uint menu_str_h(const u8 *str)
{
    uint h = 0;
    while (*str != 0xFF)
    {
        if (*str == 0xFE)
        {
            h += 16;
        }
        str++;
    }
    return h;
}

static void *g_np_main(int mode, struct g_t *g, unused void *data)
{
    if (mode == 1)
    {
        struct gc_t *gc = (struct gc_t *)g;
        struct player_gfx_t *pg        = &player_gfx_table[0];
        struct player_gfx_t *pg_backup = &player_gfx_table[1];
        struct camera_t *cam = (struct camera_t *)g_camera->g.arg;
        struct object_t *obj = g_object == &g_object_mirror ?
            object_p1 : (struct object_t *)g_object;
        if (gc->arg == 0)
        {
            struct np_t *np = obj->o_np_np;
            *pg_backup = *pg;
            *net_rot_head_backup = *camera_rot_head;
            net_cam_mode_backup = cam->mode;
            if (obj != object_p1)
            {
                pg->state = 0x20810446;
                pg->head  =  np->np_gfx_flag_h >>  6 & 0x0003;
                pg->eye   = (np->np_gfx_flag_h >>  3 & 0x0007) + 1;
                pg->glove =  np->np_gfx_flag_h >>  0 & 0x0007;
                pg->wing  =  np->np_gfx_flag_l >> 15 & 0x0001;
                pg->cap   =  np->np_gfx_flag_l >>  0 & 0x03FF;
                pg->hold  =  np->np_gfx_flag_l >> 13 & 0x0003;
                pg->punch =  np->np_gfx_flag_h >>  8 & 0x00FF;
                pg->rot_torso[0] = np->np_rot_torso_x;
                pg->rot_torso[1] = np->np_rot_torso_y;
                pg->rot_torso[2] = np->np_rot_torso_z;
                camera_rot_head[0] = np->np_rot_head_x;
                camera_rot_head[1] = np->np_rot_head_y;
                cam->mode = 0x06;
            }
            g_np_gfx.child =
                (np->np_gfx_flag_l & NP_GFX_BODY) ? np->np_gfx : NULL;
            if (np->np_gfx_flag_l & NP_GFX_NAME)
            {
                Gfx *gfx_backup;
                Gfx *gfx;
                uint size;
                uint w;
                uint h;
                gfx_backup = video_gfx;
                (&np->np_name[0])[NP_NAME_LEN-1] = 0xFF;
                w = menu_str_w(np->np_name);
                h = menu_str_h(np->np_name);
                message_print(-w >> 1, h, np->np_name);
                size = (u8 *)video_gfx - (u8 *)gfx_backup;
                gfx = gfx_alloc(sizeof(*gfx)*3 + size);
                video_gfx = gfx_backup;
                if (gfx != NULL)
                {
                    Gfx *dst = gfx;
                    Gfx *src = gfx_backup;
                    dst->w.w0 = 0x06000000;
                    dst->w.w1 = (uintptr_t)gfx_np_name_start;
                    dst++;
                    dst->w.w0 = 0xFB000000;
                    dst->w.w1 = np->np_colour;
                    dst++;
                    do
                    {
                        *dst++ = *src++;
                        size -= sizeof(*dst);
                    }
                    while (size > 0);
                    dst->w.w0 = 0x06010000;
                    dst->w.w1 = (uintptr_t)gfx_np_name_end;
                }
                return gfx;
            }
        }
        else
        {
            if (obj != object_p1)
            {
                *pg = *pg_backup;
                *camera_rot_head = *net_rot_head_backup;
                cam->mode = net_cam_mode_backup;
            }
        }
    }
    return NULL;
}

#ifdef _TOUCH
#define NP_CMD_TOUCH    2

#define /* 0x022D */    np_touch        tcp[0x0B].u8[1]

static void np_update_self_touch(unused struct np_t *np, struct player_t *pl)
{
    uint i;
    for (i = 1; i < NP_LEN; i++)
    {
        struct np_t *n = &np_table[i];
        if (n->np_touch)
        {
            n->np_touch = 0;
            if (!(pl->state & 0x00020000))
            {
                struct object_t *obj = np_table[i].np_object;
                obj->o_touch_arg = 0;
                player_touch_attacked(pl, obj);
                obj->o_touch_arg = 2;
            }
        }
    }
}
#else
#define np_update_self_touch(np, pl)
#endif

static void o_np_main(void)
{
    struct np_t *np = object->o_np_np;
#ifdef _TOUCH
    struct player_t *pl = player_p1;
#endif
    if (object != np->np_object)
    {
        object->flag = 0;
    }
#ifdef _TOUCH
    object->o_pos_x = object->g.pos[0];
    object->o_pos_y = object->g.pos[1];
    object->o_pos_z = object->g.pos[2];
    if (pl->pos[1] < object->o_pos_y+160 && object->o_pos_y < pl->pos[1]+160)
    {
        player_touch_bound(pl, object, -5);
    }
    if ((object->o_touch & 0xFF) != 0)
    {
        np_table[0].np_touch = np-np_table;
        np_table[0].np_tcp_id = NP_CMD_TOUCH;
    }
    object->o_touch = 0;
#endif
}

static void *nm_alloc(size_t size)
{
    uint pages;
    uint page;
    uint i;
    pages = (size+sizeof(nm_heap.data[0])-1) / sizeof(nm_heap.data[0]);
    if (pages >= 0x100)
    {
        return NULL;
    }
    for (page = 0; page < lenof(nm_heap.page); page++)
    {
        for (i = 0; i < pages; i++)
        {
            if (nm_heap.page[page+i] > 0)
            {
                goto end;
            }
        }
        nm_heap.page[page] = pages;
        for (i = 1; i < pages; i++)
        {
            nm_heap.page[page+i] = 1;
        }
        return &nm_heap.data[page];
    end:;
    }
    return NULL;
}

static void nm_free(void *ptr)
{
    uint pages;
    uint page;
    uint i;
    page = (typeof(&nm_heap.data[0]))ptr - nm_heap.data;
    pages = nm_heap.page[page];
    for (i = 0; i < pages; i++)
    {
        nm_heap.page[page+i] = 0;
    }
}

static void np_interpolate_f32(f32 *dst, const f32 *src, uint len, uint delay)
{
    f32 div = (delay > 0 ? delay : 1) * (3/2.0F);
    do
    {
        f32 d = *src - *dst;
        if (d > -8 && d < 8)
        {
            *dst = *src;
        }
        else
        {
            *dst += d / div;
        }
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
        int d = *src - *dst;
        if (d < -0x8000)
        {
            d += 0x10000;
        }
        else if (d >= 0x8000)
        {
            d -= 0x10000;
        }
        if (d > -0x0040 && d < 0x0040)
        {
            *dst = *src;
        }
        else
        {
            *dst += d / div;
        }
        dst++;
        src++;
    }
    while (--len > 0);
}

static void np_init_self(struct np_t *np)
{
    np->np_timer      = NP_TIMEOUT;
    np->np_gfx_flag_l = NP_GFX_BODY | NP_GFX_NAME;
    np->np_gfx        = world_gfx_table[1];
    np->np_object     = NULL;
    np_init_self_character(np);
    np->np_tcp_id = NP_CMD_SYNC;
}

static void np_init_peer(struct np_t *np)
{
    np->np_timer      = 0;
    np->np_gfx        = world_gfx_table[1];
    np->np_object     = NULL;
    np->np_motion     = NULL;
    np->np_motion_src = -1;
}

static void np_destroy(struct np_t *np)
{
    if (np->np_object != NULL)
    {
        ((struct object_t *)np->np_object)->flag = 0;
        np->np_object = NULL;
    }
    if (np->np_motion != NULL)
    {
        nm_free(np->np_motion);
        np->np_motion = NULL;
        np->np_motion_src = 0xFF;
    }
}

static void np_update(struct np_t *np, struct object_t *obj)
{
    obj->o_np_np = np;
    np_update_character(np, obj);
}

static void np_update_self(struct np_t *np, struct player_t *pl)
{
    struct player_gfx_t *pg  = pl->gfx;
    struct object_t     *obj = pl->object;
    int eye;
    if (pg == NULL || obj == NULL)
    {
        return;
    }
    np->np_pos_x       = obj->g.pos[0];
    np->np_pos_y       = obj->g.pos[1];
    np->np_pos_z       = obj->g.pos[2];
    np->np_scale_x     = obj->g.scale[0];
    np->np_scale_y     = obj->g.scale[1];
    np->np_scale_z     = obj->g.scale[2];
    np->np_rot_x       = obj->g.rot[0];
    np->np_rot_y       = obj->g.rot[1];
    np->np_rot_z       = obj->g.rot[2];
    np->np_rot_torso_x = pg->rot_torso[0];
    np->np_rot_torso_y = pg->rot_torso[1];
    np->np_rot_torso_z = pg->rot_torso[2];
    /*
    This fixes a bug where if we leave first person, our head is stuck rotated
    for everyone else's client.  Of course, we allow our head to be rotated
    for the star dance state, for obvious reasons.
    */
    /* first person / star dance */
    if
    (
        pl->state == 0x0C000227 ||
        (pl->state_prev == 0x0C000227 && pl->state == 0x00001302)
    )
    {
        np->np_rot_head_x = camera_rot_head[0];
        np->np_rot_head_y = camera_rot_head[1];
    }
    else
    {
        np->np_rot_head_x = 0;
        np->np_rot_head_y = 0;
    }
    if (pg->eye == PLAYER_EYE_BLINK)
    {
        uint i = g_timer >> 1 & 0x1F;
        eye = i < lenof(player_blink) ? player_blink[i] : 0;
    }
    else
    {
        eye = pg->eye - 1;
    }
    np->np_gfx_flag_h  = pg->punch  <<  8 & 0xFF00;
    np->np_gfx_flag_h |= pg->head   <<  6 & 0x00C0;
    np->np_gfx_flag_h |= eye        <<  3 & 0x0038;
    np->np_gfx_flag_h |= pg->glove  <<  0 & 0x0007;
    np->np_gfx_flag_l &= NP_GFX_BODY | NP_GFX_NAME;
    np->np_gfx_flag_l |= pg->wing   << 15 & 0x8000;
    np->np_gfx_flag_l |= pg->hold   << 13 & 0x6000;
    np->np_gfx_flag_l |= pg->cap    <<  0 & 0x03FF;
    np->np_motion_frame_amt = obj->g.motion.frame_amt;
    np->np_motion_frame_vel = obj->g.motion.frame_vel;
    np->np_motion_dst       = obj->g.motion.index;
    if (np->np_stage != world_stage || np->np_world != world_index)
    {
        np->np_tcp_id = NP_CMD_SYNC;
    }
    np->np_stage = world_stage;
    np->np_world = world_index;
    np->np_object = obj;
    pl->motion_height = np->np_motion_height;
    obj->g.list = &g_np;
    obj->g.motion.height = np->np_motion_height;
    np_update_self_location(np, pl);
    np_update_self_character(np, pl);
    np_update_self_touch(np, pl);
    np_update(np, obj);
}

static void np_update_peer(struct np_t *np)
{
    struct object_t *obj;
    /*
    If player is not spawned, that means we currently are not in a stage.  If we
    are not in a stage, our object ptr is stale, and should be cleared.
    */
    if (object_p1 == NULL)
    {
        np->np_object = NULL;
        return;
    }
    np_update_peer_location(np);
    if
    (
        np->np_stage != world_stage || np->np_world != world_index ||
        np->np_timer == 0
    )
    {
        np_destroy(np);
        return;
    }
    if (np->np_motion_dst != np->np_motion_src)
    {
        uintptr_t *ft;
        if (np->np_motion != NULL)
        {
            nm_free(np->np_motion);
            np->np_motion = NULL;
        }
        np->np_motion_src = 0xFF;
        ft = ft_motion_player;
        if (np->np_motion_dst < ft[0])
        {
            u8 *start;
            u8 *end;
            struct motion_t *motion;
            ft += 2*np->np_motion_dst;
            start  = data_motion_player_start + ft[2];
            end    = start + ft[3];
            motion = nm_alloc(ft[3]);
            if (motion != NULL)
            {
                mem_dma(motion, start, end);
                motion->val = (s16 *)((u8 *)motion + (uintptr_t)motion->val);
                motion->tbl = (u16 *)((u8 *)motion + (uintptr_t)motion->tbl);
                np->np_motion = motion;
                np->np_motion_src = np->np_motion_dst;
            }
        }
    }
    obj = np->np_object;
    if (obj == NULL)
    {
        obj = np->np_object = object_create(object_p1, 0, 2, o_np);
        obj->g.rot[0] = np->np_rot_x;
        obj->g.rot[1] = np->np_rot_y;
        obj->g.rot[2] = np->np_rot_z;
        obj->g.pos[0] = np->np_pos_x;
        obj->g.pos[1] = np->np_pos_y;
        obj->g.pos[2] = np->np_pos_z;
    }
    np_interpolate_s16(obj->g.rot, &np->np_rot, 3, np->np_timer_delta);
    np_interpolate_f32(obj->g.pos, &np->np_pos, 3, np->np_timer_delta);
    obj->g.scale[0] = np->np_scale_x;
    obj->g.scale[1] = np->np_scale_y;
    obj->g.scale[2] = np->np_scale_z;
    obj->g.motion.index  = np->np_motion_src;
    obj->g.motion.height = np->np_motion_height;
    obj->g.motion.motion = np->np_motion;
    if (np->np_timer == NP_TIMEOUT)
    {
        uint delta;
        obj->g.motion.frame     = np->np_motion_frame_amt >> 16;
        obj->g.motion.frame_amt = np->np_motion_frame_amt;
        obj->g.motion.frame_vel = np->np_motion_frame_vel;
        delta = np->np_timer - np->np_timer_prev;
        np->np_timer_delta = delta < NP_DELTA_MAX ? delta : NP_DELTA_MAX;
    }
    else
    {
        np->np_timer_prev = np->np_timer;
    }
    if (np->np_timer < 16)
    {
        np->np_gfx_flag_l  &= 0xFC00;
        np->np_gfx_flag_l  |= PLAYER_CAP_VANISH << 8;
        np->np_gfx_flag_l  |= 16*np->np_timer - 8;
        obj->g.scale[0] *= 0.50F + (1.0F/32)*np->np_timer;
        obj->g.scale[2] *= 0.50F + (1.0F/32)*np->np_timer;
        obj->g.scale[1] *= 0.75F + 4.0F/np->np_timer;
    }
    np->np_timer--;
    np_update(np, obj);
}

static void net_init(void)
{
    uint i;
    world_gfx_table[2] = &g_np;
    np_init_self(&np_table[0]);
    for (i = 1; i < NP_LEN; i++)
    {
        np_init_peer(&np_table[i]);
    }
    net_init_character();
    bzero(nm_heap.page, sizeof(nm_heap.page));
}

static void net_update(void)
{
    uint i;
    np_update_self(&np_table[0], player_p1);
    for (i = 1; i < NP_LEN; i++)
    {
        np_update_peer(&np_table[i]);
    }
}

void main(void)
{
    if (segment_table[0x04] != 0)
    {
        if (net_boot)
        {
            net_boot = false;
            net_init();
        }
        net_update();
    }
}
