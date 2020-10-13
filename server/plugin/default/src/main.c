/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                   Copyright (C) 2019, 2020  devwizard                      *
 *      This project is licensed under the GNU General Public License         *
 *      version 2.  See LICENSE for more information.                         *
 ******************************************************************************/

#include "types.h"
#include "sm64.h"
#include "sm64net.h"

#define _LOCATION
#define _CHARACTER

#define NET_MOTION_PAGE_LEN     64
#define NET_MOTION_PAGE_SIZE    0x800

extern struct
{
    u8 page[NET_MOTION_PAGE_LEN];
    u8 data[NET_MOTION_PAGE_LEN][NET_MOTION_PAGE_SIZE];
}
net_motion_heap;

extern struct gfx_t gfx_net_player;
extern struct gfx_dl_billboard_t gfx_net_player_billboard;
extern struct gfx_dl_scale_t gfx_net_player_scale;
extern struct gfx_callback_t gfx_net_player_name;
extern struct gfx_t gfx_net_player_gfx;
extern struct gfx_callback_t gfx_net_player_end;

static Gfx *gfx_net_player_callback(
    s32 mode, struct gfx_callback_t *gfx, unused f32 mtxf[4][4]
);

static const Gfx gfx_net_player_name_start[] =
{
    {{0xE7000000, 0x00000000}},  /* G_RDPPIPESYNC       */
    {{0xB6000000, 0x00020000}},  /* G_CLEARGEOMETRYMODE */
    {{0xBA000C02, 0x00000000}},  /* G_SETOTHERMODE_H    */
    {{0xBB000001, 0xFFFFFFFF}},  /* G_TEXTURE           */
    {{0xFC12FE25, 0xFFFFF3F9}},  /* G_SETCOMBINE        */
    {{0xB8000000, 0x00000000}},  /* G_ENDDL             */
};

static const Gfx gfx_net_player_name_end[] =
{
    {{0xE7000000, 0x00000000}},  /* G_RDPPIPESYNC       */
    {{0xB7000000, 0x00020000}},  /* G_SETGEOMETRYMODE   */
    {{0xBA000C02, 0x00002000}},  /* G_SETOTHERMODE_H    */
    {{0xBB000000, 0xFFFFFFFF}},  /* G_TEXTURE           */
    {{0xFCFFFFFF, 0xFFFE793C}},  /* G_SETCOMBINE        */
    {{0xB8000000, 0x00000000}},  /* G_ENDDL             */
};

static const uintptr_t obj_net_player[] =
{
    0x00000000,
    0x0A000000,
};

struct gfx_t gfx_net_player =
{
    GFX_TYPE_GFX,
    GFX_FLAG_ENABLED,
    &gfx_net_player,
    &gfx_net_player,
    NULL,
    &gfx_net_player_billboard.gfx.gfx,
};

struct gfx_dl_billboard_t gfx_net_player_billboard =
{
    {
        {
            GFX_TYPE_DL_BILLBOARD,
            GFX_FLAG_ENABLED,
            &gfx_net_player_end.gfx,
            &gfx_net_player_gfx,
            &gfx_net_player,
            &gfx_net_player_scale.gfx.gfx,
        },
        NULL,
    },
    {     0,    160,      0},
};

struct gfx_dl_scale_t gfx_net_player_scale =
{
    {
        {
            GFX_TYPE_DL_SCALE,
            GFX_FLAG_ENABLED,
            &gfx_net_player_scale.gfx.gfx,
            &gfx_net_player_scale.gfx.gfx,
            &gfx_net_player_billboard.gfx.gfx,
            &gfx_net_player_name.gfx,
        },
        NULL,
    },
    4.0F,
};

struct gfx_callback_t gfx_net_player_name =
{
    {
        GFX_TYPE_CALLBACK,
        GFX_RM_SPRITE << 8 | GFX_FLAG_ENABLED,
        &gfx_net_player_name.gfx,
        &gfx_net_player_name.gfx,
        &gfx_net_player_scale.gfx.gfx,
        NULL,
    },
    gfx_net_player_callback,
    0x00000000,
};

struct gfx_t gfx_net_player_gfx =
{
    GFX_TYPE_GFX,
    GFX_FLAG_ENABLED,
    &gfx_net_player_billboard.gfx.gfx,
    &gfx_net_player_end.gfx,
    &gfx_net_player,
    NULL,
};

struct gfx_callback_t gfx_net_player_end =
{
    {
        GFX_TYPE_CALLBACK,
        GFX_RM_SPRITE << 8 | GFX_FLAG_ENABLED,
        &gfx_net_player_gfx,
        &gfx_net_player_billboard.gfx.gfx,
        &gfx_net_player,
        NULL,
    },
    gfx_net_player_callback,
    0x00000001,
};

static s16  net_rot_head_backup[2];
static u8   net_camera_mode_backup;
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

static void net_player_update_self_location(
    unused struct net_player_t *np,
    struct player_t *player
)
{
    if (player->pad->down & L_TRIG)
    {
        location_draw ^= true;
    }
    if (location_draw)
    {
        if (player->pad->down & D_JPAD)
        {
            if (location_start > 1)
            {
                location_start--;
            }
        }
        if (player->pad->down & U_JPAD)
        {
            if (location_start < lenof(net_player_table))
            {
                location_start++;
            }
        }
        location_y = 20 + 18*9;
        hud_print_int(320-8 - 12*9, location_y, "%2d online", location_index);
        location_index = 1;
    }
}

static void net_player_update_peer_location(struct net_player_t *np)
{
    if (location_draw && np->np_timer > 0)
    {
        if (location_index >= location_start && location_y >= 20)
        {
            const char *str = str_stage_table[np->np_stage_index];
            switch (np->np_stage_index << 4 | np->np_world_index)
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
            hud_print_int(8, location_y, "%2d", location_index);
            hud_print(8 + 12*3, location_y, str);
            location_y -= 18;
        }
        location_index++;
    }
}
#else
#define net_player_update_self_location(np, player)
#define net_player_update_peer_location(np)
#endif

#ifdef _CHARACTER
#define /* 0x022C */    np_gfx_index        tcp.u8[0x2C]

struct character_t
{
    const uintptr_t *script;
    s16 height;
};

extern const uintptr_t gfx_metaknight[];
extern const uintptr_t gfx_bandanadee[];
extern u8 net_gfx_heap_start[];
extern u8 net_gfx_heap_end[];

static const struct character_t net_character_table[] =
{
    {NULL,           189},
    {gfx_metaknight, 100},
    {gfx_bandanadee, 100},
};

static struct gfx_t *net_gfx_table[lenof(net_character_table)];

static void net_player_init_self_character(struct net_player_t *np)
{
    np->np_gfx_index = 0;
}

static void net_player_update_character(
    struct net_player_t *np,
    unused struct object_t *object
)
{
    np->np_motion_height = net_character_table[np->np_gfx_index].height;
    np->np_gfx = net_gfx_table[np->np_gfx_index];
}

static void net_player_update_self_character(
    unused struct net_player_t *np,
    struct player_t *player
)
{
    if (player->pad->down & L_JPAD)
    {
        if (np->np_gfx_index > 0)
        {
            np->np_gfx_index--;
            np->np_tcp_id = 0x00000001;
        }
    }
    if (player->pad->down & R_JPAD)
    {
        if (np->np_gfx_index < lenof(net_character_table)-1)
        {
            np->np_gfx_index++;
            np->np_tcp_id = 0x00000001;
        }
    }
}

static void net_init_character(void)
{
    struct heap_t heap;
    u32 i;
    heap.size  = net_gfx_heap_end - net_gfx_heap_start;
    heap.used  = 0;
    heap.start = net_gfx_heap_start;
    heap.free  = net_gfx_heap_start;
    net_gfx_table[0] = g_gfx_object_table[0x01];
    for (i = 1; i < lenof(net_character_table); i++)
    {
        net_gfx_table[i] = gfx_script_compile(
            &heap, net_character_table[i].script
        );
    }
}
#else
#define net_player_init_self_character(np)
#define net_player_update_character(np)
#define net_player_update_self_character(np)
#define net_init_character()
#endif

static u32 menu_str_w(const u8 *str)
{
    u32 w = 0;
    u32 l = 0;
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
        l += g_menu_kern[*str];
        str++;
    }
    if (w < l)
    {
        w = l;
    }
    return w;
}

static u32 menu_str_h(const u8 *str)
{
    u32 h = 0;
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

static Gfx *gfx_net_player_callback(
    s32 mode, struct gfx_callback_t *gfx, unused f32 mtxf[4][4]
)
{
    if (mode == 1)
    {
        struct player_gfx_t *player_gfx        = &g_player_gfx_table[0];
        struct player_gfx_t *player_gfx_backup = &g_player_gfx_table[1];
        struct camera_t *camera = (struct camera_t *)g_gfx_camera->gfx.arg;
        struct object_t *object = (struct object_t *)g_gfx_object;
        if (gfx->arg == 0)
        {
            struct net_player_t *np = object->mem.ptr[0x49];
            Gfx *dl;
            Gfx *dl_backup;
            ptrdiff_t size;
            u32 w;
            u32 h;
            *player_gfx_backup = *player_gfx;
            *net_rot_head_backup = *g_camera_rot_head;
            net_camera_mode_backup = camera->mode;
            if (object != g_object_p1)
            {
                player_gfx->state  = 0x20810446;
                player_gfx->head   =  np->np_gfx_flag_h >>  6 & 0x0003;
                player_gfx->eyes   = (np->np_gfx_flag_h >>  3 & 0x0007) + 1;
                player_gfx->gloves =  np->np_gfx_flag_h >>  0 & 0x0007;
                player_gfx->wings  =  np->np_gfx_flag_l >> 15 & 0x0001;
                player_gfx->cap    =  np->np_gfx_flag_l >>  0 & 0x03FF;
                player_gfx->hold   =  np->np_gfx_flag_l >> 13 & 0x0003;
                player_gfx->punch  =  np->np_gfx_flag_h >>  8 & 0x00FF;
                player_gfx->rot_torso[0] = np->np_rot_torso_x;
                player_gfx->rot_torso[1] = np->np_rot_torso_y;
                player_gfx->rot_torso[2] = np->np_rot_torso_z;
                g_camera_rot_head[0] = np->np_rot_head_x;
                g_camera_rot_head[1] = np->np_rot_head_y;
                camera->mode = 0x06;
            }
            gfx_net_player_gfx.child =
                (np->np_gfx_flag_l & NET_PLAYER_GFX_BODY) ? np->np_gfx : NULL;
            if (!(np->np_gfx_flag_l & NET_PLAYER_GFX_NAME))
            {
                return NULL;
            }
            dl_backup = g_dl;
            (&np->np_name)[NET_PLAYER_NAME_LEN-1] = 0xFF;
            w = menu_str_w(&np->np_name);
            h = menu_str_h(&np->np_name);
            menu_print(-w >> 1, h, &np->np_name);
            size = (u8 *)g_dl - (u8 *)dl_backup;
            dl = mem_alloc_gfx(sizeof(*dl)*3 + size);
            g_dl = dl_backup;
            if (dl != NULL)
            {
                Gfx *dst = dl;
                Gfx *src = dl_backup;
                dst->w.w0 = 0x06000000;
                dst->w.w1 = (u32)gfx_net_player_name_start;
                dst++;
                dst->w.w0 = 0xFB000000;
                dst->w.w1 = np->np_colour;
                dst++;
                do
                {
                    dst->w.w0 = src->w.w0;
                    dst->w.w1 = src->w.w1;
                    dst++;
                    src++;
                    size -= sizeof(*dst);
                }
                while (size > 0);
                dst->w.w0 = 0x06010000;
                dst->w.w1 = (u32)gfx_net_player_name_end;
            }
            return dl;
        }
        else
        {
            if (object != g_object_p1)
            {
                *player_gfx = *player_gfx_backup;
                *g_camera_rot_head = *net_rot_head_backup;
                camera->mode = net_camera_mode_backup;
            }
        }
    }
    return NULL;
}

static void *net_motion_alloc(size_t size)
{
    u32 pages;
    u32 page;
    u32 i;
    pages =
        (size + sizeof(net_motion_heap.data[0])-1) /
        sizeof(net_motion_heap.data[0]);
    if (pages >= 0x100)
    {
        return NULL;
    }
    for (page = 0; page < lenof(net_motion_heap.page); page++)
    {
        for (i = 0; i < pages; i++)
        {
            if (net_motion_heap.page[page+i] > 0)
            {
                goto end;
            }
        }
        net_motion_heap.page[page] = pages;
        for (i = 1; i < pages; i++)
        {
            net_motion_heap.page[page+i] = 1;
        }
        return &net_motion_heap.data[page];
    end:;
    }
    return NULL;
}

static void net_motion_free(void *ptr)
{
    u32 pages;
    u32 page;
    u32 i;
    page = (typeof(&net_motion_heap.data[0]))ptr - net_motion_heap.data;
    pages = net_motion_heap.page[page];
    for (i = 0; i < pages; i++)
    {
        net_motion_heap.page[page+i] = 0;
    }
}

static void net_player_intp_f32(f32 *dst, const f32 *src, u32 len, u32 delay)
{
    f32 div = (delay > 0 ? delay : 1) * (3.0F/2.0F);
    do
    {
        f32 d = *src - *dst;
        if (d > -8.0F && d < 8.0F)
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

static void net_player_intp_s16(s16 *dst, const s16 *src, u32 len, u32 delay)
{
    s32 div = (delay > 0 ? delay : 1) * 3/2;
    do
    {
        s32 d = *src - *dst;
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

static void net_player_init_self(struct net_player_t *np)
{
    np->np_timer      = NET_PLAYER_TIMEOUT;
    np->np_gfx_flag_l = NET_PLAYER_GFX_BODY | NET_PLAYER_GFX_NAME;
    np->np_gfx        = g_gfx_object_table[0x01];
    np->np_object     = NULL;
    net_player_init_self_character(np);
    np->np_tcp_id     = 0x00000001;
}

static void net_player_init_peer(struct net_player_t *np)
{
    np->np_timer      = 0;
    np->np_gfx        = g_gfx_object_table[0x01];
    np->np_object     = NULL;
    np->np_motion     = NULL;
    np->np_motion_src = 0xFF;
}

static void net_player_destroy(struct net_player_t *np)
{
    if (np->np_object != NULL)
    {
        ((struct object_t *)np->np_object)->flag = 0;
        np->np_object = NULL;
    }
    if (np->np_motion != NULL)
    {
        net_motion_free(np->np_motion);
        np->np_motion = NULL;
        np->np_motion_src = 0xFF;
    }
}

static void net_player_update(struct net_player_t *np, struct object_t *object)
{
    object->mem.ptr[0x49] = np;
    net_player_update_character(np, object);
}

static void net_player_update_self(
    struct net_player_t *np,
    struct player_t *player
)
{
    struct player_gfx_t *player_gfx = player->gfx;
    struct object_t     *object     = player->object;
    s32 eyes;
    if (player_gfx == NULL || object == NULL)
    {
        return;
    }
    np->np_pos_x       = object->gfx.pos[0];
    np->np_pos_y       = object->gfx.pos[1];
    np->np_pos_z       = object->gfx.pos[2];
    np->np_scale_x     = object->gfx.scale[0];
    np->np_scale_y     = object->gfx.scale[1];
    np->np_scale_z     = object->gfx.scale[2];
    np->np_rot_x       = object->gfx.rot[0];
    np->np_rot_y       = object->gfx.rot[1];
    np->np_rot_z       = object->gfx.rot[2];
    np->np_rot_torso_x = player_gfx->rot_torso[0];
    np->np_rot_torso_y = player_gfx->rot_torso[1];
    np->np_rot_torso_z = player_gfx->rot_torso[2];
    /*
    This fixes a bug where if we leave first person, our head is stuck rotated
    for everyone else's client. Of course, we allow our head to be rotated
    for the star dance state, for obvious reasons.
    */
    /* first person / star dance */
    if
    (
        player->state == 0x0C000227 ||
        (player->state_prev == 0x0C000227 && player->state == 0x00001302)
    )
    {
        np->np_rot_head_x = g_camera_rot_head[0];
        np->np_rot_head_y = g_camera_rot_head[1];
    }
    else
    {
        np->np_rot_head_x = 0;
        np->np_rot_head_y = 0;
    }
    if (player_gfx->eyes == PLAYER_GFX_EYES_BLINK)
    {
        u32 index = (g_motion_timer >> 1) & 0x1F;
        if (index < 7)
        {
            eyes = g_player_blink_table[index];
        }
        else
        {
            eyes = 0;
        }
    }
    else
    {
        eyes = player_gfx->eyes - 1;
    }
    np->np_gfx_flag_h  = player_gfx->punch  <<  8 & 0xFF00;
    np->np_gfx_flag_h |= player_gfx->head   <<  6 & 0x00C0;
    np->np_gfx_flag_h |= eyes               <<  3 & 0x0038;
    np->np_gfx_flag_h |= player_gfx->gloves <<  0 & 0x0007;
    np->np_gfx_flag_l &= NET_PLAYER_GFX_BODY | NET_PLAYER_GFX_NAME;
    np->np_gfx_flag_l |= player_gfx->wings  << 15 & 0x8000;
    np->np_gfx_flag_l |= player_gfx->hold   << 13 & 0x6000;
    np->np_gfx_flag_l |= player_gfx->cap    <<  0 & 0x03FF;
    np->np_motion_frame_amt = object->gfx.motion.frame_amt;
    np->np_motion_frame_vel = object->gfx.motion.frame_vel;
    np->np_motion_dst       = object->gfx.motion.index;
    if
    (
        np->np_stage_index != g_stage_index ||
        np->np_world_index != g_world_index
    )
    {
        np->np_tcp_id = 0x00000001;
    }
    np->np_stage_index = g_stage_index;
    np->np_world_index = g_world_index;
    np->np_object = object;
    player->motion_height = np->np_motion_height;
    object->gfx.list = &gfx_net_player;
    object->gfx.motion.height = np->np_motion_height;
    net_player_update_self_location(np, player);
    net_player_update_self_character(np, player);
    net_player_update(np, object);
}

static void net_player_update_peer(struct net_player_t *np)
{
    struct object_t *object;
    /*
    If player is not spawned, that means we currently are not in a stage. If we
    are not in a stage, our object ptr is stale, and should be cleared.
    */
    if (g_object_p1 == NULL)
    {
        np->np_object = NULL;
        return;
    }
    net_player_update_peer_location(np);
    if
    (
        np->np_stage_index != g_stage_index ||
        np->np_world_index != g_world_index ||
        np->np_timer == 0
    )
    {
        net_player_destroy(np);
        return;
    }
    if (np->np_motion_dst != np->np_motion_src)
    {
        uintptr_t *table;
        if (np->np_motion != NULL)
        {
            net_motion_free(np->np_motion);
            np->np_motion = NULL;
        }
        np->np_motion_src = 0xFF;
        table = g_player_motion_table;
        if (np->np_motion_dst < table[0])
        {
            u8 *start;
            u8 *end;
            struct motion_t *motion;
            table += 2*np->np_motion_dst;
            start  = seg_player_motion_start + table[2];
            end    = start + table[3];
            motion = net_motion_alloc(table[3]);
            if (motion != NULL)
            {
                mem_dma_read(motion, start, end);
                motion->val = (s16 *)((u8 *)motion + (uintptr_t)motion->val);
                motion->tbl = (u16 *)((u8 *)motion + (uintptr_t)motion->tbl);
                np->np_motion = motion;
                np->np_motion_src = np->np_motion_dst;
            }
        }
    }
    object = np->np_object;
    if (object == NULL)
    {
        object = np->np_object = object_spawn(
            g_object_p1, 0, 0x02, obj_net_player
        );
        object->gfx.rot[0] = np->np_rot_x;
        object->gfx.rot[1] = np->np_rot_y;
        object->gfx.rot[2] = np->np_rot_z;
        object->gfx.pos[0] = np->np_pos_x;
        object->gfx.pos[1] = np->np_pos_y;
        object->gfx.pos[2] = np->np_pos_z;
    }
    net_player_intp_s16(object->gfx.rot, &np->np_rot, 3, np->np_timer_delta);
    net_player_intp_f32(object->gfx.pos, &np->np_pos, 3, np->np_timer_delta);
    object->gfx.scale[0] = np->np_scale_x;
    object->gfx.scale[1] = np->np_scale_y;
    object->gfx.scale[2] = np->np_scale_z;
    object->gfx.motion.index  = np->np_motion_src;
    object->gfx.motion.height = np->np_motion_height;
    object->gfx.motion.motion = np->np_motion;
    if (np->np_timer == NET_PLAYER_TIMEOUT)
    {
        object->gfx.motion.frame     = np->np_motion_frame_amt >> 16;
        object->gfx.motion.frame_amt = np->np_motion_frame_amt;
        object->gfx.motion.frame_vel = np->np_motion_frame_vel;
        np->np_timer_delta = np->np_timer - np->np_timer_prev;
    }
    else
    {
        np->np_timer_prev = np->np_timer;
    }
    if (np->np_timer < 16)
    {
        np->np_gfx_flag_l &= 0xFC00;
        np->np_gfx_flag_l |= PLAYER_GFX_CAP_VANISH << 8;
        np->np_gfx_flag_l |= 16*np->np_timer - 8;
        object->gfx.scale[0] *= 0.50F + (1.0F/32.0F)*np->np_timer;
        object->gfx.scale[2] *= 0.50F + (1.0F/32.0F)*np->np_timer;
        object->gfx.scale[1] *= 0.75F + 4.0F/np->np_timer;
    }
    np->np_timer--;
    net_player_update(np, object);
}

static void net_init(void)
{
    u32 i;
    bzero(net_motion_heap.page, sizeof(net_motion_heap.page));
    g_gfx_object_table[0x02] = &gfx_net_player;
    net_player_init_self(&net_player_table[0]);
    for (i = 1; i < lenof(net_player_table); i++)
    {
        net_player_init_peer(&net_player_table[i]);
    }
    net_init_character();
}

static void net_update(void)
{
    u32 i;
    net_player_update_self(&net_player_table[0], g_player_p1);
    for (i = 1; i < lenof(net_player_table); i++)
    {
        net_player_update_peer(&net_player_table[i]);
    }
}

void main(void)
{
    if (g_mem_segment_table[0x04] != NULL)
    {
        if (net_boot)
        {
            net_boot = false;
            net_init();
        }
        net_update();
    }
}
