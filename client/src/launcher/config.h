/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                   Copyright (C) 2019, 2020  devwizard                      *
 *      This project is licensed under the GNU General Public License         *
 *      version 2.  See LICENSE for more information.                         *
 ******************************************************************************/

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "types.h"

struct config_t
{
    /* 0x0000 */    char proc[493];
    /* 0x01ED */    char addr[493];
    /* 0x03D8 */    u16  port;
    /* 0x03DC */    u32  colour;
    /* 0x03E0 */    u8   name[32];
};  /* 0x0400 */

extern struct config_t g_config;

extern int config_read(void);
extern int config_write(void);

#endif /* _CONFIG_H_ */
