/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                   Copyright (C) 2019, 2020  devwizard                      *
 *      This project is licensed under the GNU General Public License         *
 *      version 2.  See LICENSE for more information.                         *
 ******************************************************************************/

#ifndef _NET_H_
#define _NET_H_

#include "types.h"

extern bool net_init(const char *, long int, u32, u8 *);
extern bool net_update(void);

#endif /* _NET_H_ */
