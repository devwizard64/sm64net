/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                    Copyright (C) 2019 - 2021  devwizard                    *
 *        This project is licensed under the terms of the GNU General         *
 *        Public License version 2.  See LICENSE for more information.        *
 ******************************************************************************/

#ifndef _MEM_H_
#define _MEM_H_

#include <types.h>

extern uint mem_read(uint, void *, size_t);
extern uint mem_write(uint, const void *, size_t);
extern uint mem_init(const char *);

#endif /* _MEM_H_ */
