/******************************************************************************/
/*                  SM64Net - An Internet framework for SM64                  */
/*                    Copyright (C) 2019 - 2022  devwizard                    */
/*        This project is licensed under the terms of the GNU General         */
/*        Public License version 2.  See LICENSE for more information.        */
/******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>

static void crc(char *buf, const unsigned char *data, size_t size)
{
    size_t i;
    uint32_t c0;
    uint32_t c1;
    uint64_t v0 = 0xF8CA4DDCF8CA4DDC;
    uint32_t v1 = 0xF8CA4DDC;
    uint32_t v2 = 0xF8CA4DDC;
    uint32_t v3 = 0xF8CA4DDC;
    uint32_t v4 = 0xF8CA4DDC;
    for (i = 0; i < size; i += 4)
    {
        uint32_t x =
            data[i+0] << 24 | data[i+1] << 16 | data[i+2] << 8 | data[i+3];
        uint32_t y = x << (x & 31) | x >> (-x & 31);
        v0 += x;
        v1 ^= x;
        v2 += y;
        v3 ^= v3 < x ? (uint32_t)v0^x : y;
        v4 += v2^x;
    }
    for (; i < 0x100000; i += 4) v4 += v2;
    c0 = (uint32_t)(v0 >> 32) ^ (uint32_t)v0 ^ v1;
    c1 = v2 ^ v3 ^ v4;
    buf[0] = c0 >> 24;
    buf[1] = c0 >> 16;
    buf[2] = c0 >>  8;
    buf[3] = c0 >>  0;
    buf[4] = c1 >> 24;
    buf[5] = c1 >> 16;
    buf[6] = c1 >>  8;
    buf[7] = c1 >>  0;
}

int main(int argc, char *argv[])
{
    FILE *f;
    unsigned char *data;
    size_t size;
    char buf[8];
    if (argc != 2)
    {
        fprintf(stderr, "usage: %s <image>\n", argv[0]);
        return 1;
    }
    if ((f = fopen(argv[1], "r+b")) == NULL)
    {
        fprintf(stderr, "error: could not open '%s'\n", argv[1]);
        return 1;
    }
    fseek(f, -0x1000, SEEK_END);
    size = ftell(f);
    if (size > 0x100000) size = 0x100000;
    data = malloc(size);
    fseek(f, 0x1000, SEEK_SET);
    fread(data, 1, size, f);
    crc(&buf[0x00], data, size);
    free(data);
    fseek(f, 0x10, SEEK_SET);
    fwrite(buf, 1, sizeof(buf), f);
    fclose(f);
    return 0;
}
