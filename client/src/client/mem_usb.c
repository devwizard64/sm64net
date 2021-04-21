/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                    Copyright (C) 2019 - 2021  devwizard                    *
 *        This project is licensed under the terms of the GNU General         *
 *        Public License version 2.  See LICENSE for more information.        *
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <libftdi1/ftdi.h>

#include <types.h>

static struct ftdi_context *usb_ftdi;

uint mem_read(uint addr, void *dst, size_t size)
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
        eprint("rsig (w) failed\n");
    }
    if (ftdi_read_data(usb_ftdi, dst, size) < 0)
    {
        eprint("rprc (r) failed\n");
    }
    return false;
}

uint mem_write(uint addr, const void *src, size_t size)
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
        eprint("wsig (w) failed\n");
    }
    if (ftdi_write_data(usb_ftdi, src, size) < 0)
    {
        eprint("wprc (w) failed\n");
    }
    return false;
}

uint mem_init(unused const char *proc)
{
    usb_ftdi = ftdi_new();
    if (usb_ftdi == NULL)
    {
        eprint("ftdi_new() failed\n");
    }
    if (ftdi_usb_open(usb_ftdi, 0x0403, 0x6001) < 0)
    {
        ftdi_free(usb_ftdi);
        eprint("EverDrive not plugged in\n");
    }
    usb_ftdi->usb_read_timeout  = 500;
    usb_ftdi->usb_write_timeout = 500;
    return false;
}
