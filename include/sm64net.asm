;                   SM64Net - An Internet framework for SM64
;                    Copyright (C) 2019, 2020  devwizard
;       This project is licensed under the GNU General Public License
;       version 2.  See LICENSE for more information.

NET_PLAYER_LEN          equ 32
NET_PLAYER_NAME_LEN     equ 32
NET_PLAYER_UDP_SIZE     equ 0x0200
NET_PLAYER_TCP_SIZE     equ 0x0100
NET_PLAYER_SYS_SIZE     equ 0x0100
NET_PLAYER_TIMEOUT      equ (30*5 + 15)

NET_PLAYER_GFX_BODY     equ 0x01
NET_PLAYER_GFX_NAME     equ 0x02

np_udp_id               equ 0x0000
np_colour               equ 0x0004
np_name                 equ 0x0008
np_pos                  equ 0x0028
np_pos_x                equ 0x0028
np_pos_y                equ 0x002C
np_pos_z                equ 0x0030
np_scale                equ 0x0034
np_scale_x              equ 0x0034
np_scale_y              equ 0x0038
np_scale_z              equ 0x003C
np_rot                  equ 0x0040
np_rot_x                equ 0x0040
np_rot_y                equ 0x0042
np_rot_z                equ 0x0044
np_rot_torso            equ 0x0046
np_rot_torso_y          equ 0x0046
np_rot_torso_x          equ 0x0048
np_rot_torso_z          equ 0x004A
np_rot_head             equ 0x004C
np_rot_head_x           equ 0x004C
np_rot_head_y           equ 0x004E
np_gfx_flags            equ 0x0050
np_gfx_alpha            equ 0x0052
np_gfx_punch            equ 0x0053
np_stage_index          equ 0x0054
np_world_index          equ 0x0055
np_timer                equ 0x0056
np_motion_dst           equ 0x0057
np_motion_frame_amt     equ 0x0058
np_motion_frame_vel     equ 0x005C

np_tcp_id               equ 0x0200

np_object               equ 0x0300
np_motion               equ 0x0304
np_timer_prev           equ 0x0308
np_timer_delta          equ 0x0309
np_motion_src           equ 0x030A

.definelabel g_net_players,                                     0x80025C00
