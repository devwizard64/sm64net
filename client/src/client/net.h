/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                    Copyright (C) 2019 - 2021  devwizard                    *
 *        This project is licensed under the terms of the GNU General         *
 *        Public License version 2.  See LICENSE for more information.        *
 ******************************************************************************/

#ifndef _NET_H_
#define _NET_H_

#include <types.h>

extern uint net_init(const char *, long int, const char **, int);
extern uint net_update(void);

#endif /* _NET_H_ */
