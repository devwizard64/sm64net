/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                    Copyright (C) 2019 - 2022  devwizard                    *
 *        This project is licensed under the terms of the GNU General         *
 *        Public License version 2.  See LICENSE for more information.        *
 ******************************************************************************/

#include <sm64net.h>

.set noreorder

.word 0x4E464600
    NFF(net_main)
#ifdef CHARACTER
    NFF(net_shape)
    NFF(net_gfx)
#endif
    NFF(net_hook)
.word 0

_net_hookSegmentStart = 0x80248BE0
_net_hookSegmentRomStart:
    la      $4, main
    jalr    $4
    addiu   $ra, $ra, -0x0080
_net_hookSegmentRomEnd:
