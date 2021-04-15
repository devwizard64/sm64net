/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                    Copyright (C) 2019 - 2021  devwizard                    *
 *        This project is licensed under the terms of the GNU General         *
 *        Public License version 2.  See LICENSE for more information.        *
 ******************************************************************************/

#ifndef _SM64_LD_
#define _SM64_LD_

#define TEXT(fn)    fn(.text*); . = ALIGN(0x10);
#define DATA(fn)    fn(.data*); . = ALIGN(0x10); fn(.rodata*); . = ALIGN(0x10);
#define BSS(fn)     fn(.bss*);  . = ALIGN(0x10);
#define SECTION(addr, name, section)        \
    name##_start = __dev;                   \
    .name (addr) : AT(__dev) SUBALIGN(0x10) \
    {section}                               \
    __dev += SIZEOF(.name);                 \
    name##_end = __dev;
#define SECTION_NOLOAD(name, section)       \
    .name.noload (NOLOAD) : SUBALIGN(0x10)  \
    {section}
#define CODE(addr, name, code, bss)                                     \
    code_##name##_start = ADDR(.name);                                  \
    code_##name##_end = ADDR(.name) + SIZEOF(.name);                    \
    bss_##name##_start = ADDR(.name.noload);                            \
    bss_##name##_end = ADDR(.name.noload) + SIZEOF(.name.noload);       \
    bss_##name##_size = SIZEOF(.name.noload);                           \
    SECTION(addr, name, code)                                           \
    SECTION_NOLOAD(name, bss)

main_start                  = 0x00001000;
code_main_start             = 0x80246000;
mem_dma                     = 0x80278504;
gfx_alloc                   = 0x80278F2C;
object_create               = 0x8029ED20;
print_int                   = 0x802D62D8;
print                       = 0x802D6554;
message_print               = 0x802D77DC;
osCreateMesgQueue           = 0x803225A0;
osRecvMesg                  = 0x80322800;
osInvalDCache               = 0x803243B0;
osPiStartDma                = 0x80324460;
bzero                       = 0x80324570;
osInvalICache               = 0x80324610;
osWritebackDCache           = 0x80325D20;
memcpy                      = 0x803273F0;
player_p1                   = 0x8032D93C;
player_blink                = 0x8032DD50;
world_gfx_table             = 0x8032DDC4;
world_stage                 = 0x8032DDF8;
gfx_camera                  = 0x8032DEFC;
gfx_object                  = 0x8032DF00;
gfx_frame                   = 0x8032DF08;
message_kern                = 0x80331370;
video_gfx                   = 0x8033B06C;
ft_motion_player            = 0x8033B080;
player_gfx_table            = 0x8033B3B0;
segment_table               = 0x8033B400;
world_index                 = 0x8033BACA;
camera_rot_head             = 0x8033C536;
object_p1                   = 0x80361158;
script_g_main               = 0x8037E0B4;
data_motion_player_start    = 0x004EC000;

#endif /* _SM64_LD_ */
