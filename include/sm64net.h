/******************************************************************************/
/*                  SM64Net - An Internet framework for SM64                  */
/*                    Copyright (C) 2019 - 2022  devwizard                    */
/*        This project is licensed under the terms of the GNU General         */
/*        Public License version 2.  See LICENSE for more information.        */
/******************************************************************************/

#ifndef __SM64NET_H__
#define __SM64NET_H__

#define NET_PORT        4352

#define NP_LEN          32
#define NP_NAME_LEN     32
#define NP_UDP_LEN      128
#define NP_TCP_LEN      64
#define NP_SYS_LEN      64
#define NP_SIZE         (4*NP_UDP_LEN + 4*NP_TCP_LEN + 4*NP_SYS_LEN)

#define NP_TIMEOUT      (30*5 + 15)
#define NP_DELTA_MAX    10

#define NP_CMD_SYNC     1

#define NP_SHAPE_BODY   0x0400
#define NP_SHAPE_NAME   0x0800

#define NET_PAGE_LEN    64
#define NET_PAGE_SIZE   2048

#ifndef __ASSEMBLER__

#define /* 0x094 */ o_np_np             mem[3].ptr

#define /* 0x000 */ np_udp_id           udp[0].u32
#define /* 0x000 */ np_udp_id_b         udp[0].u8
#define /* 0x004 */ np_pos              udp[1].f32
#define /* 0x004 */ np_pos_x            udp[1].f32
#define /* 0x008 */ np_pos_y            udp[2].f32
#define /* 0x00C */ np_pos_z            udp[3].f32
#define /* 0x010 */ np_scale            udp[4].f32
#define /* 0x010 */ np_scale_x          udp[4].f32
#define /* 0x014 */ np_scale_y          udp[5].f32
#define /* 0x018 */ np_scale_z          udp[6].f32
#define /* 0x01C */ np_rot              udp[7].s16[0]
#define /* 0x01C */ np_rot_x            udp[7].s16[0]
#define /* 0x01E */ np_rot_y            udp[7].s16[1]
#define /* 0x020 */ np_rot_z            udp[8].s16[0]
#define /* 0x022 */ np_torso            udp[8].s16[1]
#define /* 0x022 */ np_torso_x          udp[8].s16[1]
#define /* 0x024 */ np_torso_y          udp[9].s16[0]
#define /* 0x026 */ np_torso_z          udp[9].s16[1]
#define /* 0x028 */ np_head             udp[10].s16[0]
#define /* 0x028 */ np_head_x           udp[10].s16[0]
#define /* 0x02A */ np_head_y           udp[10].s16[1]
#define /* 0x02C */ np_shape_flag_h     udp[11].u16[0]
#define /* 0x02E */ np_shape_flag_l     udp[11].u16[1]
#define /* 0x030 */ np_anime_frame_amt  udp[12].s32
#define /* 0x034 */ np_anime_frame_vel  udp[13].s32
#define /* 0x038 */ np_anime_dst        udp[14].u8[0]
#define /* 0x039 */ np_timer            udp[14].u8[1]
        /* 0x03A */
#define /* 0x200 */ np_tcp_id           tcp[0].vu32
#define /* 0x200 */ np_tcp_id_b         tcp[0].u8
#define /* 0x204 */ np_write_addr_b     tcp[1].u8
#define /* 0x208 */ np_write_size_b     tcp[2].u8
#define /* 0x204 */ np_colour           tcp[1].u32
#define /* 0x204 */ np_colour_b         tcp[1].u8
#define /* 0x208 */ np_name             tcp[2].u8
#define /* 0x228 */ np_stage            tcp[10].u8[0]
#define /* 0x229 */ np_scene            tcp[10].u8[1]
        /* 0x22A */
#define /* 0x300 */ np_obj              sys[0].ptr
#define /* 0x304 */ np_anime            sys[1].ptr
#define /* 0x308 */ np_timer_prev       sys[2].u8[0]
#define /* 0x309 */ np_timer_delta      sys[2].u8[1]
#define /* 0x30A */ np_anime_src        sys[2].u8[2]
        /* 0x30B */

typedef union np_mem
{
    s8      s8[4];
    u8      u8[4];
    s16     s16[2];
    u16     u16[2];
    s32     s32;
    u32     u32;
    f32     f32;
    volatile s32 vs32;
    volatile u32 vu32;
#ifdef _ULTRA64
    void   *ptr;
#else
    PTR     ptr;
#endif
}
NP_MEM;

typedef struct net_pl
{
    /* 0x000 */ NP_MEM udp[NP_UDP_LEN];
    /* 0x200 */ NP_MEM tcp[NP_TCP_LEN];
    /* 0x300 */ NP_MEM sys[NP_SYS_LEN];
}   /* 0x400 */
NET_PL;

#ifndef _ULTRA64
typedef struct net_meta
{
    /* 0x00 */  u8      mem_addr_b[4];
    /* 0x04 */  u8      np_data_b[4];
    /* 0x08 */  char    version[64-8];
}   /* 0x40 */
NET_META;
#endif

#ifdef _ULTRA64
extern NET_PL np_data[NP_LEN];
#endif

#else /* __ASSEMBLER__ */

#define NFF(name)                       \
    .word _##name##SegmentStart;        \
    .word _##name##SegmentRomStart;     \
    .word _##name##SegmentRomEnd

#endif /* __ASSEMBLER__ */

#endif /* __SM64NET_H__ */
