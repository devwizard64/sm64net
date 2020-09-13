/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                   Copyright (C) 2019, 2020  devwizard                      *
 *      This project is licensed under the GNU General Public License         *
 *      version 2.  See LICENSE for more information.                         *
 ******************************************************************************/

#ifndef _MEM_H_
#define _MEM_H_

#include "types.h"

extern bool mem_read(u32, void *, size_t);
extern bool mem_write(u32, const void *, size_t);
extern bool mem_init(const char *);

#endif /* _MEM_H_ */
