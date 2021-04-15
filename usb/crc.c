/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                    Copyright (C) 2019 - 2021  devwizard                    *
 *        This project is licensed under the terms of the GNU General         *
 *        Public License version 2.  See LICENSE for more information.        *
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

typedef uint8_t  u8;
typedef uint32_t u32;
typedef unsigned int uint;

int main(int argc, const char **argv)
{
    FILE *f;
    u8   *data;
    u32   c0;
    u32   c1;
    u32   c0a;
    u32   c0b;
    u32   c1a;
    u32   c1b;
    uint  size;
    uint  i;
    u8    buf[8];
    if (argc != 2)
    {
        fprintf(stderr, "usage: %s <image>\n", argv[0]);
        return EXIT_FAILURE;
    }
    f = fopen(argv[1], "r+b");
    if (f == NULL)
    {
        fprintf(stderr, "error: could not open '%s'\n", argv[1]);
        return EXIT_FAILURE;
    }
    fseek(f, 0x00001000, SEEK_END);
    data = malloc(0x00100000);
    size = ftell(f);
    if (size > 0x00100000)
    {
        size = 0x00100000;
    }
    else if (size < 0x00100000)
    {
        memset(data+size, 0x00, 0x00100000-size);
    }
    fseek(f, 0x00001000, SEEK_SET);
    fread(data, 1, size, f);
    c0 = c1 = c0a = c0b = c1a = c1b = 0xF8CA4DDC;
    for (i = 0; i < 0x00100000; i += 4)
    {
        u32 x;
        u32 o;
        u32 s;
        u32 r;
        x = data[i+0] << 24 | data[i+1] << 16 | data[i+2] << 8 | data[i+3];
        o = c0 + x;
        if (o < c0)
        {
            c0a++;
        }
        s    = x & 0x1F;
        r    = x << s | x >> (0x20-s);
        c0   = o;
        c0b ^= x;
        c1  += r;
        c1a ^= c1a < x ? c0^x : r;
        c1b += c1^x;
    }
    c0 ^= c0a^c0b;
    c1 ^= c1a^c1b;
    buf[0] = c0 >> 24;
    buf[1] = c0 >> 16;
    buf[2] = c0 >>  8;
    buf[3] = c0 >>  0;
    buf[4] = c1 >> 24;
    buf[5] = c1 >> 16;
    buf[6] = c1 >>  8;
    buf[7] = c1 >>  0;
    free(data);
    fseek(f, 0x00000010, SEEK_SET);
    fwrite(buf, 1, sizeof(buf), f);
    fclose(f);
    return EXIT_SUCCESS;
}
