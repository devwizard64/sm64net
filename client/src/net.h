/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                    Copyright (C) 2019 - 2022  devwizard                    *
 *        This project is licensed under the terms of the GNU General         *
 *        Public License version 2.  See LICENSE for more information.        *
 ******************************************************************************/

#ifndef __NET_H__
#define __NET_H__

#include <types.h>

extern void net_init(
    const char *proc, const char *addr, int port, int argc, char *argv[]
);
extern void net_update(void);

#endif /* __NET_H__ */
