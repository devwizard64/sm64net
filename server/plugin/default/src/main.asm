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

.definelabel _CHARACTER, 0

.create "build/main.nff", 0

NET_MOTION_PAGE_LEN     equ 64
NET_MOTION_PAGE_SIZE    equ 0x800

.definelabel net_motion_heap,   \
    0x8005C000 - (1+NET_MOTION_PAGE_SIZE)*NET_MOTION_PAGE_LEN

.definelabel net_main_start,    \
    net_player_table + NET_PLAYER_SIZE*NET_PLAYER_LEN
.ifdef _CHARACTER
.definelabel net_gfx_start,     0x80400000
.definelabel net_gfx_heap_end,  0x80800000
.endif
.definelabel net_hook_start,    app_main + 0xF0

.dw 0x4E464600
.dw net_main_start,   seg_net_main_start,   seg_net_main_end
.ifdef _CHARACTER
.dw net_gfx_start,    seg_net_gfx_start,    seg_net_gfx_end
.endif
.dw net_hook_start,   seg_net_hook_start,   seg_net_hook_end
.dw 0x00000000

.headersize 0
seg_net_main_start:
.base net_main_start
    .importobj "build/src/main.o"
    .if . > net_motion_heap
        .error "net_main too large (0x" + tohex(. - net_motion_heap) + ")"
    .endif
.headersize 0
seg_net_main_end:

.ifdef _CHARACTER
.headersize 0
seg_net_gfx_start:
.base net_gfx_start
    .importobj "src/gfx_metaknight.o"
    .importobj "src/script_metaknight.o"
    net_gfx_heap_start:
.headersize 0
seg_net_gfx_end:
.endif

.headersize 0
seg_net_hook_start:
.base net_hook_start
    jal     main
    nop
    b       app_main + 0x80
    nop
.headersize 0
seg_net_hook_end:

.close
