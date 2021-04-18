/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                    Copyright (C) 2019 - 2021  devwizard                    *
 *        This project is licensed under the terms of the GNU General         *
 *        Public License version 2.  See LICENSE for more information.        *
 ******************************************************************************/

#include <sm64net.h>

.set noreorder

.word 0x4E464600
    NFF(net_main)
#ifdef _CHARACTER
    NFF(net_script)
    NFF(net_gfx)
#endif
    NFF(net_hook)
.word 0

code_net_hook_start = 0x80248BE0
net_hook_start:
    la      $4, main
    jalr    $4
    addiu   $ra, $ra, -0x0080
net_hook_end:
