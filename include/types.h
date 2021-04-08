/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                    Copyright (C) 2019 - 2021  devwizard                    *
 *        This project is licensed under the terms of the GNU General         *
 *        Public License version 2.  See LICENSE for more information.        *
 ******************************************************************************/

#ifndef _TYPES_H_
#define _TYPES_H_

#define false   0
#define true    1

#ifndef __ASSEMBLER__

#include <stddef.h>
#include <stdint.h>

typedef unsigned int uint;

typedef  int8_t  s8;
typedef uint8_t  u8;
typedef  int16_t s16;
typedef uint16_t u16;
typedef  int32_t s32;
typedef uint32_t u32;
typedef  int64_t s64;
typedef uint64_t u64;
typedef float    f32;
typedef double   f64;

typedef u8 bool;

#define lenof(x)        (sizeof((x)) / sizeof((x)[0]))

#ifdef __GNUC__
#define unused          __attribute__((unused))
#else
#define unused
#endif

#endif /* __ASSEMBLER__ */

#endif /* _TYPES_H_ */
