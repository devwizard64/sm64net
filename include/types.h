/******************************************************************************/
/*                  SM64Net - An Internet framework for SM64                  */
/*                    Copyright (C) 2019 - 2022  devwizard                    */
/*        This project is licensed under the terms of the GNU General         */
/*        Public License version 2.  See LICENSE for more information.        */
/******************************************************************************/

#ifndef __TYPES_H__
#define __TYPES_H__

#include <stddef.h>
#include <stdint.h>

#ifndef FALSE
#define FALSE   0
#endif
#ifndef TRUE
#define TRUE    1
#endif

typedef unsigned int uint;

typedef  int8_t  s8;
typedef uint8_t  u8;
typedef  int16_t s16;
typedef uint16_t u16;
typedef  int32_t s32;
typedef uint32_t u32;
typedef float    f32;

typedef u32 PTR;

#ifdef __GNUC__
#define UNUSED          __attribute__((unused))
#else
#define UNUSED
#endif
#define lenof(x)        (sizeof((x)) / sizeof((x)[0]))

#define eprint(...) {fprintf(stderr, "error: " __VA_ARGS__); exit(1);}

#endif /* __TYPES_H__ */
