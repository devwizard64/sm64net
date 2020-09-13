;                   SM64Net - An Internet framework for SM64
;                    Copyright (C) 2019, 2020  devwizard
;       This project is licensed under the GNU General Public License
;       version 2.  See LICENSE for more information.

.n64
.sym off
.include "../../../include/types.asm"
.include "../../../include/sm64.asm"
.include "../../../include/sm64net.asm"
.sym on

.create "build/main.nff", 0

.definelabel g_net_gfx_heap_start,      g_net_players + 0x8000
.definelabel g_net_main_start,          g_net_gfx_heap_start + 0x6000
.definelabel g_net_motion_heap_start,   0x8005C000 - 0x800*64
.definelabel g_net_gfx_start,           g_net_motion_heap_start
.definelabel g_net_hook_start,          game_main + 0xF0

.dw 0x4E464600
.dw g_net_main_start, seg_net_main_start, seg_net_main_end
.dw g_net_gfx_start,  seg_net_gfx_start,  seg_net_gfx_end
.dw g_net_hook_start, seg_net_hook_start, seg_net_hook_end
.dw 0x00000000

.headersize 0
seg_net_main_start:
.base g_net_main_start
    .importobj "build/src/main.o"
    .if . > g_net_gfx_start
        .error "seg_net_main too large (0x" + tohex(. - g_net_gfx_start) + ")"
    .endif
.headersize 0
seg_net_main_end:

.headersize 0
seg_net_gfx_start:
.base g_net_gfx_start
    .importobj "build/src/gfx_object_net_player.o"
.headersize 0
seg_net_gfx_end:

.headersize 0
seg_net_hook_start:
.base g_net_hook_start
    jal     main
    nop
    b       game_main + 0x80
    nop
.headersize 0
seg_net_hook_end:

.close
