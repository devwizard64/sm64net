;                   SM64Net - An Internet framework for SM64
;                    Copyright (C) 2019, 2020  devwizard
;       This project is licensed under the GNU General Public License
;       version 2.  See LICENSE for more information.

.definelabel g_main_start,                                      0x80246000
.definelabel profiler_update,                                   0x80246050
.definelabel app_main,                                          0x80248AF0
.definelabel mem_dma_read,                                      0x80278504
.definelabel mem_init_main2,                                    0x80278974
.definelabel mem_alloc_gfx,                                     0x80278F2C
.definelabel object_spawn,                                      0x8029ED20
.definelabel hud_print_int,                                     0x802D62D8
.definelabel hud_print,                                         0x802D6554
.definelabel menu_print,                                        0x802D77DC
.definelabel bzero,                                             0x80324570
.definelabel osInvalICache,                                     0x80324610
.definelabel osWritebackDCache,                                 0x80325D20
.definelabel memcpy,                                            0x803273F0
.definelabel g_player_p1,                                       0x8032D93C
.definelabel g_player_blink_table,                              0x8032DD50
.definelabel g_gfx_object_table,                                0x8032DDC4
.definelabel g_stage_index,                                     0x8032DDF8
.definelabel g_gfx_camera,                                      0x8032DEFC
.definelabel g_gfx_object,                                      0x8032DF00
.definelabel g_motion_timer,                                    0x8032DF08
.definelabel g_menu_kern,                                       0x80331370
.definelabel g_dl,                                              0x8033B06C
.definelabel g_player_motion_table,                             0x8033B080
.definelabel g_player_gfx_table,                                0x8033B3B0
.definelabel g_mem_segment_table,                               0x8033B400
.definelabel g_world_index,                                     0x8033BACA
.definelabel g_camera_rot_head,                                 0x8033C536
.definelabel g_object_p1,                                       0x80361158
.definelabel gfx_script_compile,                                0x8037E0B4

.definelabel seg_main_start,                                    0x00001000
.definelabel seg_player_motion_start,                           0x004EC000
