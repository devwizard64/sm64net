/******************************************************************************/
/*                  SM64Net - An Internet framework for SM64                  */
/*                    Copyright (C) 2019 - 2022  devwizard                    */
/*        This project is licensed under the terms of the GNU General         */
/*        Public License version 2.  See LICENSE for more information.        */
/******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <libftdi1/ftdi.h>

#include <types.h>

#define netR    0x6E657452
#define netW    0x6E657457

static int cmd(
    struct ftdi_context *ftdi,
    unsigned int code,
    unsigned int addr,
    unsigned int size,
    unsigned int arg
)
{
    unsigned char buf[16];
    buf[ 0] = code >> 24;
    buf[ 1] = code >> 16;
    buf[ 2] = code >>  8;
    buf[ 3] = code >>  0;
    buf[ 4] = addr >> 24;
    buf[ 5] = addr >> 16;
    buf[ 6] = addr >>  8;
    buf[ 7] = addr >>  0;
    buf[ 8] = size >> 24;
    buf[ 9] = size >> 16;
    buf[10] = size >>  8;
    buf[11] = size >>  0;
    buf[12] = arg  >> 24;
    buf[13] = arg  >> 16;
    buf[14] = arg  >>  8;
    buf[15] = arg  >>  0;
    return ftdi_write_data(ftdi, buf, sizeof(buf));
}

static void ftdi_recvall(
    struct ftdi_context *ftdi, unsigned char *buf, int size
)
{
    int count = 60*5;
    int i = 0;
    do
    {
        int n = ftdi_read_data(ftdi, buf+i, size);
        if (n < 0) eprint("ftdi_read_data() failed (%d)\n", n);
        if (n == 0)
        {
            if (--count == 0) eprint("USB connection timeout\n");
            usleep(16666);
        }
        i    += n;
        size -= n;
    }
    while (size > 0);
}

static struct ftdi_context *ftdi;

void mem_rd(PTR addr, void *dst, size_t size)
{
    cmd(ftdi, netR, addr, size, 0);
    ftdi_recvall(ftdi, dst, size);
}

void mem_wr(PTR addr, const void *src, size_t size)
{
    cmd(ftdi, netW, addr, size, 0);
    ftdi_write_data(ftdi, src, size);
}

void mem_init(UNUSED const char *proc)
{
    if ((ftdi = ftdi_new()) == NULL) eprint("ftdi_new() failed\n");
    if (ftdi_usb_open(ftdi, 0x0403, 0x6001) < 0)
    {
        ftdi_free(ftdi);
        eprint("EverDrive not plugged in\n");
    }
}
