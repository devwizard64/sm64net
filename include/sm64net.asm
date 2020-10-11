;                   SM64Net - An Internet framework for SM64
;                    Copyright (C) 2019, 2020  devwizard
;       This project is licensed under the GNU General Public License
;       version 2.  See LICENSE for more information.

NET_PLAYER_LEN          equ 32
NET_PLAYER_NAME_LEN     equ 32
NET_PLAYER_UDP_SIZE     equ 0x0200
NET_PLAYER_TCP_SIZE     equ 0x0100
NET_PLAYER_SYS_SIZE     equ 0x0100
NET_PLAYER_SIZE         equ \
    (NET_PLAYER_UDP_SIZE+NET_PLAYER_TCP_SIZE+NET_PLAYER_SYS_SIZE)

NET_PLAYER_TIMEOUT      equ (30*5 + 15)

NET_PLAYER_GFX_BODY     equ 0x0400
NET_PLAYER_GFX_NAME     equ 0x0800

np_udp_id               equ 0x0000
np_pos                  equ 0x0004
np_pos_x                equ 0x0004
np_pos_y                equ 0x0008
np_pos_z                equ 0x000C
np_scale                equ 0x0010
np_scale_x              equ 0x0010
np_scale_y              equ 0x0014
np_scale_z              equ 0x0018
np_rot                  equ 0x001C
np_rot_x                equ 0x001C
np_rot_y                equ 0x001E
np_rot_z                equ 0x0020
np_rot_torso            equ 0x0022
np_rot_torso_y          equ 0x0022
np_rot_torso_x          equ 0x0024
np_rot_torso_z          equ 0x0026
np_rot_head             equ 0x0028
np_rot_head_x           equ 0x0028
np_rot_head_y           equ 0x002A
np_gfx_flag_h           equ 0x002C
np_gfx_flag_l           equ 0x002E
np_motion_frame_amt     equ 0x0030
np_motion_frame_vel     equ 0x0034
np_motion_dst           equ 0x0038
np_timer                equ 0x0039
;                           0x003A

np_tcp_id               equ 0x0200
np_colour               equ 0x0204
np_name                 equ 0x0208
np_stage_index          equ 0x0228
np_world_index          equ 0x0229
;                           0x022A

np_gfx                  equ 0x0300
np_object               equ 0x0304
np_motion               equ 0x0308
np_timer_prev           equ 0x030C
np_timer_delta          equ 0x030D
np_motion_src           equ 0x030E
;                           0x030F

.definelabel net_player_table,                                  0x80025C00
