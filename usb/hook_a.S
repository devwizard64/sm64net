/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                    Copyright (C) 2019 - 2021  devwizard                    *
 *        This project is licensed under the terms of the GNU General         *
 *        Public License version 2.  See LICENSE for more information.        *
 ******************************************************************************/

.globl mem_init_usb
mem_init_usb:
    addiu   $sp, $sp, -0x0020
    sw      $ra, 0x001C($sp)
    la      $a0, code_usb_start
    la      $a1, usb_start
    la      $a2, usb_end
    jal     mem_dma
    lw      $ra, 0x001C($sp)
    addiu   $sp, $sp, 0x0020
    j       usb_init
