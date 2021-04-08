/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                    Copyright (C) 2019 - 2021  devwizard                    *
 *        This project is licensed under the terms of the GNU General         *
 *        Public License version 2.  See LICENSE for more information.        *
 ******************************************************************************/

#include <sm64net.h>

.word 0x4E464600
    NFF(net_main)
#ifdef _CHARACTER
    NFF(net_gfx)
#endif
    NFF(net_hook)
.word 0
