/******************************************************************************/
/*                  SM64Net - An Internet framework for SM64                  */
/*                    Copyright (C) 2019 - 2022  devwizard                    */
/*        This project is licensed under the terms of the GNU General         */
/*        Public License version 2.  See LICENSE for more information.        */
/******************************************************************************/

#ifndef __SM64_LD__
#define __SM64_LD__

#define TEXT(x) x(.text*); . = ALIGN(16);
#define DATA(x) x(.data*); . = ALIGN(16); x(.rodata*); . = ALIGN(16);
#define BSS(x)  x(.bss*);  . = ALIGN(16);
#define SECTION(addr, name, s_text, s_data, s_bss)                      \
    _##name##SegmentRomStart  = __dev;                                  \
    _##name##SegmentStart     = ADDR(.name.text);                       \
    _##name##SegmentTextStart = ADDR(.name.text);                       \
    .name.text (addr)  : AT(__dev) {s_text}                             \
    _##name##SegmentTextEnd   = ADDR(.name.text) + SIZEOF(.name.text);  \
    __dev += SIZEOF(.name.text);                                        \
    _##name##SegmentDataStart = ADDR(.name.data);                       \
    .name.data         : AT(__dev) {s_data}                             \
    _##name##SegmentDataEnd   = ADDR(.name.data) + SIZEOF(.name.data);  \
    __dev += SIZEOF(.name.data);                                        \
    _##name##SegmentBssStart  = ADDR(.name.bss);                        \
    .name.bss (NOLOAD) : AT(__dev) {s_bss}                              \
    _##name##SegmentBssEnd    = ADDR(.name.bss)  + SIZEOF(.name.bss);   \
    _##name##SegmentEnd       = ADDR(.name.bss)  + SIZEOF(.name.bss);   \
    _##name##SegmentRomEnd    = __dev;

_mainSegmentRomStart        = 0x00001000;
_mainSegmentStart           = 0x80246000;

pl_collision_bound          = 0x8024D578;
pl_collision_attacked       = 0x8024D998;
s_player_alpha              = 0x802770A4;
s_player_select_eye         = 0x802771BC;
s_player_rot_torso          = 0x80277294;
s_player_rot_head           = 0x802773A4;
s_player_select_glove       = 0x802774F4;
s_player_scale              = 0x802775CC;
s_player_select_cap         = 0x802776D8;
s_player_select_head        = 0x80277740;
s_player_rot_wing           = 0x80277824;
s_player_hand               = 0x8027795C;
s_player_mirror             = 0x80277D6C;
mem_dma                     = 0x80278504;
gfx_alloc                   = 0x80278F2C;
save_file_star_range        = 0x8027A010;
obj_create                  = 0x8029ED20;
s_mario_pos_child           = 0x802B1BB0;
dprintf                     = 0x802D62D8;
dprint                      = 0x802D6554;
message_print               = 0x802D77DC;
s_script_main               = 0x8037E0B4;

mario                       = 0x8032D93C;
pl_shape_blink              = 0x8032DD50;
shape_table                 = 0x8032DDC4;
save_index                  = 0x8032DDF4;
stage_index                 = 0x8032DDF8;
shape_camera                = 0x8032DEFC;
shape_object                = 0x8032DF00;
draw_timer                  = 0x8032DF08;
message_kern                = 0x80331370;
thread_rmon                 = 0x8033A580;
video_gfx                   = 0x8033B06C;
shape_object_mirror         = 0x8033B350;
pl_shape_data               = 0x8033B3B0;
segment_table               = 0x8033B400;
course_index                = 0x8033BAC6;
scene_index                 = 0x8033BACA;
camera_rot_head             = 0x8033C536;
obj_mario                   = 0x80361158;
object                      = 0x80361160;

stack_entry                 = 0x80200200;

osCreateMesgQueue           = 0x803225A0;
osCreateThread              = 0x803226B0;
osRecvMesg                  = 0x80322800;
osSendMesg                  = 0x80322C20;
osStartThread               = 0x80322DF0;
osInvalDCache               = 0x803243B0;
bzero                       = 0x80324570;
osInvalICache               = 0x80324610;
memcpy                      = 0x803273F0;
__osPiGetAccess             = 0x80328440;
__osPiRelAccess             = 0x80328484;

#endif /* __SM64_LD__ */
