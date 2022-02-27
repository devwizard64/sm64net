/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                    Copyright (C) 2019 - 2022  devwizard                    *
 *        This project is licensed under the terms of the GNU General         *
 *        Public License version 2.  See LICENSE for more information.        *
 ******************************************************************************/

.globl mem_load_usb
mem_load_usb:
    addiu   $sp, $sp, -0x0018
    sw      $ra, 0x0014($sp)
    la      $a0, _usbSegmentStart
    li      $a1, 0x8038F800 - 0x8038EEF0
    jal     osInvalICache
    la      $a0, _usbSegmentStart
    li      $a1, 0x8038F800 - 0x8038EEF0
    jal     osInvalDCache
    la      $a0, _usbSegmentStart
    la      $a1, _usbSegmentRomStart
    la      $a2, _usbSegmentRomEnd
    jal     mem_dma
    lw      $ra, 0x0014($sp)
    addiu   $sp, $sp, 0x0018
    j       usb_init
