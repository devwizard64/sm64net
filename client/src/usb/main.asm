;                   SM64Net - An Internet framework for SM64
;                    Copyright (C) 2019, 2020  devwizard
;       This project is licensed under the GNU General Public License
;       version 2.  See LICENSE for more information.

.n64
.include "../include/types.asm"
.include "../include/sm64.asm"
.include "../include/sm64net.asm"

.open "../donor/nsme.z64", "build/sm64net.z64", 0

.definelabel g_usb_start,                                       0x8038EEF0
.definelabel g_usb_end,                                         0x8038F800
.definelabel seg_usb_start,                                     0x007CC6C0

.orga seg_main_start
.base g_main_start
.org profiler_update
init:
    addiu   sp, sp, -0x20
    sw      ra, 0x1C(sp)
    la      a0, g_usb_start
    la      a1, seg_usb_start
    la.u    a2, seg_usb_end
    jal     mem_dma_read
    la.l    a2, seg_usb_end
    lw      ra, 0x1C(sp)
    j       usb_init
    addiu   sp, sp, 0x20
.org mem_init_main2
.skip 0x98
    j       init
.org game_main
.skip 0xA0
    jal     usb_update

.orga seg_usb_start
.base g_usb_start
.area g_usb_end - g_usb_start
.importobj "build/src/usb/usb.o"
.align 0x10
.endarea
.headersize 0
seg_usb_end:

.close
