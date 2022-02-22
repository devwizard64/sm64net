/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                    Copyright (C) 2019 - 2022  devwizard                    *
 *        This project is licensed under the terms of the GNU General         *
 *        Public License version 2.  See LICENSE for more information.        *
 ******************************************************************************/

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <types.h>
#include <sm64net.h>

typedef struct config
{
    /* 0x0000 */    char proc[0x400+1];
    /* 0x0401 */    char addr[0x400+1];
    /* 0x0802 */    u16  port;
    /* 0x0804 */    u32  colour;
    /* 0x0808 */    u8   name[NP_NAME_LEN];
}   /* 0x0828 */
CONFIG;

extern CONFIG config;

extern uint config_read(void);
extern uint config_write(void);
extern uint config_write_nff(void);

#endif /* __CONFIG_H__ */
