/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                    Copyright (C) 2019 - 2022  devwizard                    *
 *        This project is licensed under the terms of the GNU General         *
 *        Public License version 2.  See LICENSE for more information.        *
 ******************************************************************************/

#ifndef __MEM_H__
#define __MEM_H__

#include <types.h>

extern void mem_read(PTR addr, void *dst, size_t size);
extern void mem_write(PTR addr, const void *src, size_t size);
extern void mem_init(const char *proc, PTR addr);

#endif /* __MEM_H__ */
