/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                   Copyright (C) 2019, 2020  devwizard                      *
 *      This project is licensed under the GNU General Public License         *
 *      version 2.  See LICENSE for more information.                         *
 ******************************************************************************/

#ifndef _SM64NET_H_
#define _SM64NET_H_

#include "types.h"

#define NET_PORT                0x1100

#define NET_PLAYER_LEN          32
#define NET_PLAYER_NAME_LEN     32
#define NET_PLAYER_UDP_SIZE     0x0200
#define NET_PLAYER_TCP_SIZE     0x0100
#define NET_PLAYER_SYS_SIZE     0x0100
#define NET_PLAYER_TIMEOUT      (30*5 + 15)

#define NET_PLAYER_GFX_BODY     0x01
#define NET_PLAYER_GFX_NAME     0x02

/* used by the client program */
#define /* 0x0000 */    np_udp_id_0         udp.u8[0x00]
#define /* 0x0001 */    np_udp_id_1         udp.u8[0x01]
#define /* 0x0002 */    np_udp_id_2         udp.u8[0x02]
#define /* 0x0003 */    np_udp_id_3         udp.u8[0x03]
#define /* 0x0004 */    np_colour_0         udp.u8[0x04]
#define /* 0x0005 */    np_colour_1         udp.u8[0x05]
#define /* 0x0006 */    np_colour_2         udp.u8[0x06]
#define /* 0x0007 */    np_colour_3         udp.u8[0x07]
        /* 0x0008 */    /* np_name */
        /* 0x0028 */
#define /* 0x0200 */    np_tcp_id_0         tcp.u8[0x00]
#define /* 0x0201 */    np_tcp_id_1         tcp.u8[0x01]
#define /* 0x0202 */    np_tcp_id_2         tcp.u8[0x02]
#define /* 0x0203 */    np_tcp_id_3         tcp.u8[0x03]
        /* 0x0204 */
/* used by the server to send a write request, safe for your own tcp data */
#define /* 0x0204 */    np_write_addr_0     tcp.u8[0x04]
#define /* 0x0205 */    np_write_addr_1     tcp.u8[0x05]
#define /* 0x0206 */    np_write_addr_2     tcp.u8[0x06]
#define /* 0x0207 */    np_write_addr_3     tcp.u8[0x07]
#define /* 0x0208 */    np_write_size_0     tcp.u8[0x08]
#define /* 0x0209 */    np_write_size_1     tcp.u8[0x09]
#define /* 0x020A */    np_write_size_2     tcp.u8[0x0A]
#define /* 0x020B */    np_write_size_3     tcp.u8[0x0B]
        /* 0x020C */

#define /* 0x0000 */    np_udp_id           udp.u32[0x00]
#define /* 0x0004 */    np_colour           udp.u32[0x01]
#define /* 0x0008 */    np_name             udp.u8[0x08]
#define /* 0x0028 */    np_pos              udp.f32[0x0A]
#define /* 0x0028 */    np_pos_x            udp.f32[0x0A]
#define /* 0x002C */    np_pos_y            udp.f32[0x0B]
#define /* 0x0030 */    np_pos_z            udp.f32[0x0C]
#define /* 0x0034 */    np_scale            udp.f32[0x0D]
#define /* 0x0034 */    np_scale_x          udp.f32[0x0D]
#define /* 0x0038 */    np_scale_y          udp.f32[0x0E]
#define /* 0x003C */    np_scale_z          udp.f32[0x0F]
#define /* 0x0040 */    np_rot              udp.s16[0x20]
#define /* 0x0040 */    np_rot_x            udp.s16[0x20]
#define /* 0x0042 */    np_rot_y            udp.s16[0x21]
#define /* 0x0044 */    np_rot_z            udp.s16[0x22]
#define /* 0x0046 */    np_rot_torso        udp.s16[0x23]
#define /* 0x0046 */    np_rot_torso_x      udp.s16[0x23]
#define /* 0x0048 */    np_rot_torso_y      udp.s16[0x24]
#define /* 0x004A */    np_rot_torso_z      udp.s16[0x25]
#define /* 0x004C */    np_rot_head         udp.s16[0x26]
#define /* 0x004C */    np_rot_head_x       udp.s16[0x26]
#define /* 0x004E */    np_rot_head_y       udp.s16[0x27]
#define /* 0x0050 */    np_gfx_flags        udp.u16[0x28]
#define /* 0x0052 */    np_gfx_alpha        udp.u8[0x52]
#define /* 0x0053 */    np_gfx_punch        udp.u8[0x53]
#define /* 0x0054 */    np_stage_index      udp.u8[0x54]
#define /* 0x0055 */    np_world_index      udp.u8[0x55]
#define /* 0x0056 */    np_timer            udp.u8[0x56]
#define /* 0x0057 */    np_motion_dst       udp.u8[0x57]
#define /* 0x0058 */    np_motion_frame_amt udp.s32[0x16]
#define /* 0x005C */    np_motion_frame_vel udp.s32[0x17]
        /* 0x0060 */
#define /* 0x0200 */    np_tcp_id           tcp.u32[0x00]
        /* 0x0204 */
#define /* 0x0300 */    np_object           sys.ptr[0x00]
#define /* 0x0304 */    np_motion           sys.ptr[0x01]
#define /* 0x0308 */    np_timer_prev       sys.u8[0x08]
#define /* 0x0309 */    np_timer_delta      sys.u8[0x09]
#define /* 0x030A */    np_motion_src       sys.u8[0x0A]
        /* 0x030B */

struct net_player_t
{
    /* 0x0000 */    union
                    {
                        s8      s8[NET_PLAYER_UDP_SIZE/sizeof(s8)];
                        u8      u8[NET_PLAYER_UDP_SIZE/sizeof(u8)];
                        s16     s16[NET_PLAYER_UDP_SIZE/sizeof(s16)];
                        u16     u16[NET_PLAYER_UDP_SIZE/sizeof(u16)];
                        s32     s32[NET_PLAYER_UDP_SIZE/sizeof(s32)];
                        u32     u32[NET_PLAYER_UDP_SIZE/sizeof(u32)];
                        s64     s64[NET_PLAYER_UDP_SIZE/sizeof(s64)];
                        u64     u64[NET_PLAYER_UDP_SIZE/sizeof(u64)];
                        f32     f32[NET_PLAYER_UDP_SIZE/sizeof(f32)];
                        f64     f64[NET_PLAYER_UDP_SIZE/sizeof(f64)];
                    }
                    udp;
    /* 0x0200 */    union
                    {
                        s8      s8[NET_PLAYER_TCP_SIZE/sizeof(s8)];
                        u8      u8[NET_PLAYER_TCP_SIZE/sizeof(u8)];
                        s16     s16[NET_PLAYER_TCP_SIZE/sizeof(s16)];
                        u16     u16[NET_PLAYER_TCP_SIZE/sizeof(u16)];
                        s32     s32[NET_PLAYER_TCP_SIZE/sizeof(s32)];
                        u32     u32[NET_PLAYER_TCP_SIZE/sizeof(u32)];
                        s64     s64[NET_PLAYER_TCP_SIZE/sizeof(s64)];
                        u64     u64[NET_PLAYER_TCP_SIZE/sizeof(u64)];
                        f32     f32[NET_PLAYER_TCP_SIZE/sizeof(f32)];
                        f64     f64[NET_PLAYER_TCP_SIZE/sizeof(f64)];
                    }
                    tcp;
    /* 0x0300 */    union
                    {
                        s8      s8[NET_PLAYER_SYS_SIZE/sizeof(s8)];
                        u8      u8[NET_PLAYER_SYS_SIZE/sizeof(u8)];
                        s16     s16[NET_PLAYER_SYS_SIZE/sizeof(s16)];
                        u16     u16[NET_PLAYER_SYS_SIZE/sizeof(u16)];
                        s32     s32[NET_PLAYER_SYS_SIZE/sizeof(s32)];
                        u32     u32[NET_PLAYER_SYS_SIZE/sizeof(u32)];
                        s64     s64[NET_PLAYER_SYS_SIZE/sizeof(s64)];
                        u64     u64[NET_PLAYER_SYS_SIZE/sizeof(u64)];
                        f32     f32[NET_PLAYER_SYS_SIZE/sizeof(f32)];
                        f64     f64[NET_PLAYER_SYS_SIZE/sizeof(f64)];
                    #ifdef _N64
                        void   *ptr[NET_PLAYER_SYS_SIZE/sizeof(void *)];
                    #else
                        u32     ptr[NET_PLAYER_SYS_SIZE/sizeof(u32)];
                    #endif
                    }
                    sys;
};  /* 0x0400 */

#ifdef _N64
extern struct net_player_t g_net_players[];
#else
#define sm64_g_net_players                                      0x80025C00
#endif

#endif /* _SM64NET_H_ */
