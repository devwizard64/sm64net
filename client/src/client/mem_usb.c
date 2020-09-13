/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                   Copyright (C) 2019, 2020  devwizard                      *
 *      This project is licensed under the GNU General Public License         *
 *      version 2.  See LICENSE for more information.                         *
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <libftdi1/ftdi.h>

#include "types.h"

#include "assert.h"

static struct ftdi_context *usb_ftdi;

bool mem_read(u32 addr, void *dst, size_t size)
{
    u8 buf[0x10];
    memset(buf, 0x00, sizeof(buf));
    buf[0x00] = addr >> 24;
    buf[0x01] = addr >> 16;
    buf[0x02] = addr >>  8;
    buf[0x03] = addr >>  0;
    buf[0x04] = size >> 24;
    buf[0x05] = size >> 16;
    buf[0x06] = size >>  8;
    buf[0x07] = size >>  0;
    buf[0x08] = 0;
    if (ftdi_write_data(usb_ftdi, buf, sizeof(buf)) < 0)
    {
        fprintf(stderr, "error: rsig (w) failed\n");
        return true;
    }
    if (ftdi_read_data(usb_ftdi, dst, size) < 0)
    {
        fprintf(stderr, "error: rprc (r) failed\n");
        return true;
    }
    return false;
}

bool mem_write(u32 addr, const void *src, size_t size)
{
    u8 buf[0x10];
    memset(buf, 0x00, sizeof(buf));
    buf[0x00] = addr >> 24;
    buf[0x01] = addr >> 16;
    buf[0x02] = addr >>  8;
    buf[0x03] = addr >>  0;
    buf[0x04] = size >> 24;
    buf[0x05] = size >> 16;
    buf[0x06] = size >>  8;
    buf[0x07] = size >>  0;
    buf[0x08] = 1;
    if (ftdi_write_data(usb_ftdi, buf, sizeof(buf)) < 0)
    {
        fprintf(stderr, "error: wsig (w) failed\n");
        return true;
    }
    if (ftdi_write_data(usb_ftdi, src, size) < 0)
    {
        fprintf(stderr, "error: wprc (w) failed\n");
        return true;
    }
    return false;
}

bool mem_init(unused const char *proc)
{
    usb_ftdi = ftdi_new();
    if (usb_ftdi == NULL)
    {
        fprintf(stderr, "error: ftdi_new() failed\n");
        return true;
    }
    if (ftdi_usb_open(usb_ftdi, 0x0403, 0x6001) < 0)
    {
        fprintf(stderr, "error: X7 not plugged in\n");
        ftdi_free(usb_ftdi);
        return true;
    }
    usb_ftdi->usb_read_timeout  = 500;
    usb_ftdi->usb_write_timeout = 500;
    return false;
}
