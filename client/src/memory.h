/******************************************************************************/
/*                  SM64Net - An Internet framework for SM64                  */
/*                    Copyright (C) 2019 - 2022  devwizard                    */
/*        This project is licensed under the terms of the GNU General         */
/*        Public License version 2.  See LICENSE for more information.        */
/******************************************************************************/

#ifndef __MEMORY_H__
#define __MEMORY_H__

extern void mem_rd(PTR addr, void *dst, size_t size);
extern void mem_wr(PTR addr, const void *src, size_t size);
extern void mem_init(const char *proc, PTR addr);

#endif /* __MEMORY_H__ */
