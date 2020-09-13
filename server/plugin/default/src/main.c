/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                   Copyright (C) 2019, 2020  devwizard                      *
 *      This project is licensed under the GNU General Public License         *
 *      version 2.  See LICENSE for more information.                         *
 ******************************************************************************/

#include "types.h"
#include "sm64.h"
#include "sm64net.h"

#include "main.h"
#include "gfx_object_net_player.h"

#define NET_MOTION_PAGE_LEN     64
#define NET_MOTION_PAGE_SIZE    0x800

static const Gfx dl_object_net_player_name_start[] =
{
    {{0xE7000000, 0x00000000}},  /* G_RDPPIPESYNC       */
    {{0xB6000000, 0x00020000}},  /* G_CLEARGEOMETRYMODE */
    {{0xBA000C02, 0x00000000}},  /* G_SETOTHERMODE_H    */
    {{0xBB000001, 0xFFFFFFFF}},  /* G_TEXTURE           */
    {{0xFC12FE25, 0xFFFFF3F9}},  /* G_SETCOMBINE        */
    {{0xB8000000, 0x00000000}},  /* G_ENDDL             */
};

static const Gfx dl_object_net_player_name_end[] =
{
    {{0xE7000000, 0x00000000}},  /* G_RDPPIPESYNC       */
    {{0xB7000000, 0x00020000}},  /* G_SETGEOMETRYMODE   */
    {{0xBA000C02, 0x00002000}},  /* G_SETOTHERMODE_H    */
    {{0xBB000000, 0xFFFFFFFF}},  /* G_TEXTURE           */
    {{0xFCFFFFFF, 0xFFFE793C}},  /* G_SETCOMBINE        */
    {{0xB8000000, 0x00000000}},  /* G_ENDDL             */
};

static const uintptr_t obj_object_net_player[] =
{
    0x00000000,
    0x0A000000,
};

static struct heap_t net_gfx_heap =
{
    0x00006000,
    0x00000000,
    g_net_gfx_heap_start,
    g_net_gfx_heap_start,
};

static struct player_gfx_t net_player_gfx_backup;
static s16 net_rot_head_backup[2];
static u8 net_camera_mode_backup;

static u8 net_motion_page_alloc[NET_MOTION_PAGE_LEN] = {0};

static bool net_init = true;

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

Gfx *gfx_object_net_player_callback(
    s32 mode, struct gfx_callback_t *gfx, unused f32 mtxf[4][4]
)
{
    if (mode == 1)
    {
        struct camera_t *camera =
            (struct camera_t *)g_gfx_camera->gfx_callback.arg;
        if (gfx->arg == 0)
        {
            struct net_player_t *np =
                ((struct object_t *)g_gfx_object)->mem.ptr[0x1B];
            Gfx *dl;
            Gfx *dl_backup;
            ptrdiff_t size;
            u32 w;
            u32 h;
            net_player_gfx_backup.head   = g_player_gfx_table[0].head;
            net_player_gfx_backup.eyes   = g_player_gfx_table[0].eyes;
            net_player_gfx_backup.gloves = g_player_gfx_table[0].gloves;
            net_player_gfx_backup.wings  = g_player_gfx_table[0].wings;
            net_player_gfx_backup.cap    = g_player_gfx_table[0].cap;
            net_player_gfx_backup.hold   = g_player_gfx_table[0].hold;
            net_player_gfx_backup.punch  = g_player_gfx_table[0].punch;
            net_player_gfx_backup.rot_torso[0] =
                g_player_gfx_table[0].rot_torso[0];
            net_player_gfx_backup.rot_torso[1] =
                g_player_gfx_table[0].rot_torso[1];
            net_player_gfx_backup.rot_torso[2] =
                g_player_gfx_table[0].rot_torso[2];
            net_rot_head_backup[0] = g_camera_rot_head[0];
            net_rot_head_backup[1] = g_camera_rot_head[1];
            net_camera_mode_backup =
                ((struct camera_t *)g_gfx_camera->gfx_callback.arg)->mode;
            g_player_gfx_table[0].action = 0x20810446;
            g_player_gfx_table[0].head   = np->np_gfx_flags >> 14 & 0x0003;
            g_player_gfx_table[0].eyes   = np->np_gfx_flags >> 11 & 0x0007;
            g_player_gfx_table[0].eyes++;
            g_player_gfx_table[0].gloves = np->np_gfx_flags >>  8 & 0x0007;
            g_player_gfx_table[0].wings  = np->np_gfx_flags >>  7 & 0x0001;
            g_player_gfx_table[0].cap    =
                (np->np_gfx_flags << 3 & 0x0300) | np->np_gfx_alpha;
            g_player_gfx_table[0].hold   = np->np_gfx_flags >>  3 & 0x0003;
            g_player_gfx_table[0].punch  = np->np_gfx_punch;
            g_player_gfx_table[0].rot_torso[0] = np->np_rot_torso_x;
            g_player_gfx_table[0].rot_torso[1] = np->np_rot_torso_y;
            g_player_gfx_table[0].rot_torso[2] = np->np_rot_torso_z;
            g_camera_rot_head[0] = np->np_rot_head_x;
            g_camera_rot_head[1] = np->np_rot_head_y;
            camera->mode = 0x06;
            if (np->np_gfx_flags & NET_PLAYER_GFX_BODY)
            {
                gfx->gfx.parent->parent->next->flags |= 0x01;
            }
            else
            {
                gfx->gfx.parent->parent->next->flags &= ~0x01;
            }
            if (!(np->np_gfx_flags & NET_PLAYER_GFX_NAME))
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
                dst->w.w1 = (u32)dl_object_net_player_name_start;
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
                while (size != 0);
                dst->w.w0 = 0x06010000;
                dst->w.w1 = (u32)dl_object_net_player_name_end;
            }
            return dl;
        }
        else
        {
            g_player_gfx_table[0].head   = net_player_gfx_backup.head;
            g_player_gfx_table[0].eyes   = net_player_gfx_backup.eyes;
            g_player_gfx_table[0].gloves = net_player_gfx_backup.gloves;
            g_player_gfx_table[0].wings  = net_player_gfx_backup.wings;
            g_player_gfx_table[0].cap    = net_player_gfx_backup.cap;
            g_player_gfx_table[0].hold   = net_player_gfx_backup.hold;
            g_player_gfx_table[0].punch  = net_player_gfx_backup.punch;
            g_player_gfx_table[0].rot_torso[0] =
                net_player_gfx_backup.rot_torso[0];
            g_player_gfx_table[0].rot_torso[1] =
                net_player_gfx_backup.rot_torso[1];
            g_player_gfx_table[0].rot_torso[2] =
                net_player_gfx_backup.rot_torso[2];
            g_camera_rot_head[0] = net_rot_head_backup[0];
            g_camera_rot_head[1] = net_rot_head_backup[1];
            camera->mode = net_camera_mode_backup;
        }
    }
    return NULL;
}

static void *net_motion_alloc(size_t size)
{
    u32 pages;
    u32 page;
    u32 i;
    pages = (size+(NET_MOTION_PAGE_SIZE-1)) / NET_MOTION_PAGE_SIZE;
    if (pages >= 0x100)
    {
        return NULL;
    }
    for (page = 0; page < NET_MOTION_PAGE_LEN; page++)
    {
        for (i = 0; i < pages; i++)
        {
            if (net_motion_page_alloc[page+i] != 0)
            {
                goto end;
            }
        }
        net_motion_page_alloc[page] = pages;
        for (i = 1; i < pages; i++)
        {
            net_motion_page_alloc[page+i] = 1;
        }
        return g_net_motion_heap_start + NET_MOTION_PAGE_SIZE*page;
    end:;
    }
    return NULL;
}

static void net_motion_free(void *ptr)
{
    u32 pages;
    u32 page;
    u32 i;
    page = ((u8 *)ptr-g_net_motion_heap_start) / NET_MOTION_PAGE_SIZE;
    pages = net_motion_page_alloc[page];
    for (i = 0; i < pages; i++)
    {
        net_motion_page_alloc[page+i] = 0;
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
    while (--len != 0);
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
    while (--len != 0);
}

static void net_player_init_self(struct net_player_t *np)
{
    np->np_timer     = NET_PLAYER_TIMEOUT;
    np->np_gfx_flags = NET_PLAYER_GFX_BODY | NET_PLAYER_GFX_NAME;
}

static void net_player_init_peer(struct net_player_t *np)
{
    np->np_timer      = 0;
    np->np_object     = NULL;
    np->np_motion     = NULL;
    np->np_motion_src = 0xFF;
}

static void net_player_update_self(
    struct net_player_t *np,
    struct player_t *player
)
{
    struct player_gfx_t *gfx    = player->gfx;
    struct object_t     *object = player->object;
    u8 eyes;
    if (gfx == NULL || object == NULL)
    {
        return;
    }
    np->np_pos_x       = object->gfx_object.pos[0];
    np->np_pos_y       = object->gfx_object.pos[1];
    np->np_pos_z       = object->gfx_object.pos[2];
    np->np_scale_x     = object->gfx_object.scale[0];
    np->np_scale_y     = object->gfx_object.scale[1];
    np->np_scale_z     = object->gfx_object.scale[2];
    np->np_rot_x       = object->gfx_object.rot[0];
    np->np_rot_y       = object->gfx_object.rot[1];
    np->np_rot_z       = object->gfx_object.rot[2];
    np->np_rot_torso_x = gfx->rot_torso[0];
    np->np_rot_torso_y = gfx->rot_torso[1];
    np->np_rot_torso_z = gfx->rot_torso[2];
    /*
    This fixes a bug where if we leave first person, our head is stuck rotated
    for everyone else's client. Of course, we allow our head to be rotated
    for the star dance action, for obvious reasons.
    */
    /* first person / star dance */
    if
    (
        player->action == 0x0C000227 ||
        (player->action_prev == 0x0C000227 && player->action == 0x00001302)
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
    if (gfx->eyes == PLAYER_GFX_EYES_BLINK)
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
        eyes = gfx->eyes - 1;
    }
    np->np_gfx_flags &= 0x0007;
    np->np_gfx_flags |= gfx->head   << 14 & 0xC000;
    np->np_gfx_flags |= eyes        << 11 & 0x3800;
    np->np_gfx_flags |= gfx->gloves <<  8 & 0x0700;
    np->np_gfx_flags |= gfx->wings  <<  7 & 0x0080;
    np->np_gfx_flags |= gfx->cap    <<  5 & 0x0060;
    np->np_gfx_flags |= gfx->hold   <<  3 & 0x0018;
    np->np_gfx_alpha = gfx->cap & 0xFF;
    np->np_gfx_punch = gfx->punch;
    np->np_stage_index  = g_stage_index;
    np->np_world_index  = g_world_index;
    np->np_motion_dst       = object->gfx_object.motion.index;
    np->np_motion_frame_amt = object->gfx_object.motion.frame_amt;
    np->np_motion_frame_vel = object->gfx_object.motion.frame_vel;
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
    if
    (
        np->np_timer == 0 ||
        np->np_stage_index != g_stage_index ||
        np->np_world_index != g_world_index
    )
    {
        if (np->np_object != NULL)
        {
            ((struct object_t *)np->np_object)->flags = 0;
            np->np_object = NULL;
        }
        if (np->np_motion != NULL)
        {
            net_motion_free(np->np_motion);
            np->np_motion = NULL;
            np->np_motion_src = 0xFF;
        }
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
            g_object_p1, 0, 0x02, obj_object_net_player
        );
        object->gfx_object.rot[0] = np->np_rot_x;
        object->gfx_object.rot[1] = np->np_rot_y;
        object->gfx_object.rot[2] = np->np_rot_z;
        object->gfx_object.pos[0] = np->np_pos_x;
        object->gfx_object.pos[1] = np->np_pos_y;
        object->gfx_object.pos[2] = np->np_pos_z;
    }
    net_player_intp_s16(
        object->gfx_object.rot, &np->np_rot, 3, np->np_timer_delta
    );
    net_player_intp_f32(
        object->gfx_object.pos, &np->np_pos, 3, np->np_timer_delta
    );
    object->gfx_object.scale[0] = np->np_scale_x;
    object->gfx_object.scale[1] = np->np_scale_y;
    object->gfx_object.scale[2] = np->np_scale_z;
    object->gfx_object.motion.index  = np->np_motion_src;
    object->gfx_object.motion.height = 189;
    object->gfx_object.motion.motion = np->np_motion;
    if (np->np_timer == NET_PLAYER_TIMEOUT)
    {
        object->gfx_object.motion.frame     = np->np_motion_frame_amt >> 16;
        object->gfx_object.motion.frame_amt = np->np_motion_frame_amt;
        object->gfx_object.motion.frame_vel = np->np_motion_frame_vel;
        np->np_timer_delta = np->np_timer - np->np_timer_prev;
    }
    else
    {
        np->np_timer_prev = np->np_timer;
    }
    object->mem.ptr[0x1B] = np;
    if (np->np_timer < 16)
    {
        np->np_gfx_flags |= PLAYER_GFX_CAP_VANISH << 5;
        np->np_gfx_alpha = 16*np->np_timer - 8;
        object->gfx_object.scale[0] *= 0.50F + (1.0F/32.0F)*np->np_timer;
        object->gfx_object.scale[2] *= 0.50F + (1.0F/32.0F)*np->np_timer;
        object->gfx_object.scale[1] *= 0.75F + 4.0F/np->np_timer;
    }
    np->np_timer--;
}

void main(void)
{
    if (g_segment_table[0x04] != NULL)
    {
        u32 i;
        if (net_init)
        {
            net_init = false;
            g_gfx_object_table[0x02] = gfx_script_compile(
                &net_gfx_heap, gfx_object_net_player
            );
            net_player_init_self(&g_net_players[0]);
            for (i = 1; i < NET_PLAYER_LEN; i++)
            {
                net_player_init_peer(&g_net_players[i]);
            }
        }
        net_player_update_self(&g_net_players[0], &g_players[0]);
        for (i = 1; i < NET_PLAYER_LEN; i++)
        {
            net_player_update_peer(&g_net_players[i]);
        }
    }
}
