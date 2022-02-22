/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                    Copyright (C) 2019 - 2022  devwizard                    *
 *        This project is licensed under the terms of the GNU General         *
 *        Public License version 2.  See LICENSE for more information.        *
 ******************************************************************************/

#ifndef __TYPES_H__
#define __TYPES_H__

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

typedef u32 PTR;

#define lenof(x)        (sizeof((x)) / sizeof((x)[0]))

#ifdef __GNUC__
#define unused          __attribute__((unused))
#else
#define unused
#endif

#define eexit()     exit(EXIT_FAILURE)
#define assert(c)   {if (c) eexit();}
#define wprint(...) {fprintf(stderr, "warning: " __VA_ARGS__);}
#define eprint(...) {fprintf(stderr, "error: " __VA_ARGS__); eexit();}
#ifdef __DEBUG__
#define wdebug wprint
#define edebug eprint
#else
#define wdebug(...)
#define edebug(...)
#endif

#ifdef WIN32
#define usleep(x)   Sleep((x)/1000)
#endif

#endif /* __ASSEMBLER__ */

#endif /* __TYPES_H__ */
