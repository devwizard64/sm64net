/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                   Copyright (C) 2019, 2020  devwizard                      *
 *      This project is licensed under the GNU General Public License         *
 *      version 2.  See LICENSE for more information.                         *
 ******************************************************************************/

#ifndef _MAIN_H_
#define _MAIN_H_

#include "types.h"
#include "sm64.h"

extern u8 g_net_gfx_heap_start[];
extern u8 g_net_motion_heap_start[];

extern Gfx *gfx_object_net_player_callback(
    s32, struct gfx_callback_t *, f32[4][4]
);

#endif /* _MAIN_H_ */
