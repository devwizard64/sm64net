/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                    Copyright (C) 2019 - 2022  devwizard                    *
 *        This project is licensed under the terms of the GNU General         *
 *        Public License version 2.  See LICENSE for more information.        *
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <libftdi1/ftdi.h>

#include <types.h>

static void ftdi_recvall(
    struct ftdi_context *ftdi, unsigned char *buf, int size
)
{
    uint count = 60*5;
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

static void ftdi_sendall(
    struct ftdi_context *ftdi, const unsigned char *buf, int size
)
{
    int i = 0;
    do
    {
        int n = ftdi_write_data(ftdi, buf+i, size);
        if (n < 0) eprint("ftdi_write_data() failed (%d)\n", n);
        i    += n;
        size -= n;
    }
    while (size > 0);
}

static struct ftdi_context *ftdi;

void mem_read(PTR addr, void *dst, size_t size)
{
    unsigned char buf[16];
    buf[0x00] = 'n';
    buf[0x01] = 'e';
    buf[0x02] = 't';
    buf[0x03] = 'R';
    buf[0x04] = addr >> 24;
    buf[0x05] = addr >> 16;
    buf[0x06] = addr >>  8;
    buf[0x07] = addr >>  0;
    buf[0x08] = size >> 24;
    buf[0x09] = size >> 16;
    buf[0x0A] = size >>  8;
    buf[0x0B] = size >>  0;
    buf[0x0C] = 0;
    buf[0x0D] = 0;
    buf[0x0E] = 0;
    buf[0x0F] = 0;
    ftdi_sendall(ftdi, buf, sizeof(buf));
    ftdi_recvall(ftdi, dst, size);
}

void mem_write(PTR addr, const void *src, size_t size)
{
    unsigned char buf[16];
    buf[0x00] = 'n';
    buf[0x01] = 'e';
    buf[0x02] = 't';
    buf[0x03] = 'W';
    buf[0x04] = addr >> 24;
    buf[0x05] = addr >> 16;
    buf[0x06] = addr >>  8;
    buf[0x07] = addr >>  0;
    buf[0x08] = size >> 24;
    buf[0x09] = size >> 16;
    buf[0x0A] = size >>  8;
    buf[0x0B] = size >>  0;
    buf[0x0C] = 0;
    buf[0x0D] = 0;
    buf[0x0E] = 0;
    buf[0x0F] = 0;
    ftdi_sendall(ftdi, buf, sizeof(buf));
    ftdi_sendall(ftdi, src, size);
}

void mem_init(unused const char *proc)
{
    if ((ftdi = ftdi_new()) == NULL) eprint("ftdi_new() failed\n");
    if (ftdi_usb_open(ftdi, 0x0403, 0x6001) < 0)
    {
        ftdi_free(ftdi);
        eprint("EverDrive not plugged in\n");
    }
}
