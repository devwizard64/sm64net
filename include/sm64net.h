/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                    Copyright (C) 2019 - 2021  devwizard                    *
 *        This project is licensed under the terms of the GNU General         *
 *        Public License version 2.  See LICENSE for more information.        *
 ******************************************************************************/

#ifndef _SM64NET_H_
#define _SM64NET_H_

#include <types.h>

#define NET_PORT                0x1100

#define NP_LEN          32
#define NP_NAME_LEN     32
#define NP_UDP_LEN      0x80
#define NP_TCP_LEN      0x40
#define NP_SYS_LEN      0x40
#define NP_SIZE         (4*NP_UDP_LEN + 4*NP_TCP_LEN + 4*NP_SYS_LEN)
#define NP_TIMEOUT      (30*5 + 15)

#define NP_GFX_BODY     0x0400
#define NP_GFX_NAME     0x0800

#define NM_PAGE_LEN     64
#define NM_PAGE_SIZE    0x800

#ifndef __ASSEMBLER__

/* used by the client program */
#define /* 0x0000 */    np_udp_id_b         udp[0x00].u8
        /* 0x0004 */
#define /* 0x0200 */    np_tcp_id_b         tcp[0x00].u8

/* used by the server to send a write request, safe for your own tcp data */
#define /* 0x0204 */    np_write_addr_b     tcp[0x01].u8
#define /* 0x0208 */    np_write_size_b     tcp[0x02].u8
        /* 0x020C */

/* used by the launcher */
#define /* 0x0204 */    np_colour_b         tcp[0x01].u8
        /* 0x0208 */    /* np_name */
        /* 0x0228 */

#define /* 0x0000 */    np_udp_id           udp[0x00].u32
#define /* 0x0004 */    np_pos              udp[0x01].f32
#define /* 0x0004 */    np_pos_x            udp[0x01].f32
#define /* 0x0008 */    np_pos_y            udp[0x02].f32
#define /* 0x000C */    np_pos_z            udp[0x03].f32
#define /* 0x0010 */    np_scale            udp[0x04].f32
#define /* 0x0010 */    np_scale_x          udp[0x04].f32
#define /* 0x0014 */    np_scale_y          udp[0x05].f32
#define /* 0x0018 */    np_scale_z          udp[0x06].f32
#define /* 0x001C */    np_rot              udp[0x07].s16[0]
#define /* 0x001C */    np_rot_x            udp[0x07].s16[0]
#define /* 0x001E */    np_rot_y            udp[0x07].s16[1]
#define /* 0x0020 */    np_rot_z            udp[0x08].s16[0]
#define /* 0x0022 */    np_rot_torso        udp[0x08].s16[1]
#define /* 0x0022 */    np_rot_torso_x      udp[0x08].s16[1]
#define /* 0x0024 */    np_rot_torso_y      udp[0x09].s16[0]
#define /* 0x0026 */    np_rot_torso_z      udp[0x09].s16[1]
#define /* 0x0028 */    np_rot_head         udp[0x0A].s16[0]
#define /* 0x0028 */    np_rot_head_x       udp[0x0A].s16[0]
#define /* 0x002A */    np_rot_head_y       udp[0x0A].s16[1]
#define /* 0x002C */    np_gfx_flag_h       udp[0x0B].u16[0]
#define /* 0x002E */    np_gfx_flag_l       udp[0x0B].u16[1]
#define /* 0x0030 */    np_motion_frame_amt udp[0x0C].s32
#define /* 0x0034 */    np_motion_frame_vel udp[0x0D].s32
#define /* 0x0038 */    np_motion_dst       udp[0x0E].u8[0]
#define /* 0x0039 */    np_timer            udp[0x0E].u8[1]
        /* 0x003A */
#define /* 0x0200 */    np_tcp_id           tcp[0x00].u32
#define /* 0x0204 */    np_colour           tcp[0x01].u32
#define /* 0x0208 */    np_name             tcp[0x02].u8[0]
#define /* 0x0228 */    np_stage            tcp[0x0A].u8[0]
#define /* 0x0229 */    np_world            tcp[0x0A].u8[1]
#define /* 0x022A */    np_motion_height    tcp[0x0A].s16[1]
        /* 0x022C */
#define /* 0x0300 */    np_gfx              sys[0x00].ptr
#define /* 0x0304 */    np_object           sys[0x01].ptr
#define /* 0x0308 */    np_motion           sys[0x02].ptr
#define /* 0x030C */    np_timer_prev       sys[0x03].u8[0]
#define /* 0x030D */    np_timer_delta      sys[0x03].u8[1]
#define /* 0x030E */    np_motion_src       sys[0x03].u8[2]
        /* 0x030F */

union np_mem_t
{
    s8      s8[4];
    u8      u8[4];
    s16     s16[2];
    u16     u16[2];
    s32     s32;
    u32     u32;
    f32     f32;
#ifdef _NATIVE
    u32     ptr;
#else
    void   *ptr;
#endif
};

struct np_t
{
    /* 0x0000 */    union np_mem_t udp[NP_UDP_LEN];
    /* 0x0200 */    union np_mem_t tcp[NP_TCP_LEN];
    /* 0x0300 */    union np_mem_t sys[NP_SYS_LEN];
};  /* 0x0400 */

#ifdef _NATIVE
#define np_table        0x80025C00
#else
extern struct np_t np_table[NP_LEN];
#endif

#else /* __ASSEMBLER__ */

#define NFF(name)   .word code_##name##_start, name##_start, name##_end

#endif /* __ASSEMBLER__ */

#endif /* _SM64NET_H_ */
