/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                   Copyright (C) 2019, 2020  devwizard                      *
 *      This project is licensed under the GNU General Public License         *
 *      version 2.  See LICENSE for more information.                         *
 ******************************************************************************/

#include "types.h"
#include "sm64.h"

#define REG_USB_CFG     (*(u32 *)0xBF800004)
#define REG_USB_DAT     ((u8 *)0xBF800400)

#define REG_SYS_CFG     (*(u32 *)0xBF808000)
#define REG_KEY         (*(u32 *)0xBF808004)

#define USB_LE_CFG      0x8000
#define USB_LE_CTR      0x4000

#define USB_CFG_ACT     0x0200
#define USB_CFG_RD      0x0400
#define USB_CFG_WR      0x0000

#define USB_STA_ACT     0x0200
#define USB_STA_RXF     0x0400
#define USB_STA_TXE     0x0800
#define USB_STA_PWR     0x1000
#define USB_STA_BSY     0x2000

#define USB_CMD_RD_NOP  (USB_LE_CFG | USB_LE_CTR | USB_CFG_RD)
#define USB_CMD_RD      (USB_LE_CFG | USB_LE_CTR | USB_CFG_RD | USB_CFG_ACT)
#define USB_CMD_WR_NOP  (USB_LE_CFG | USB_LE_CTR | USB_CFG_WR)
#define USB_CMD_WR      (USB_LE_CFG | USB_LE_CTR | USB_CFG_WR | USB_CFG_ACT)

static bool usb_ready(void)
{
    return (REG_USB_CFG & (USB_STA_PWR | USB_STA_RXF)) == USB_STA_PWR;
}

static u32 usb_sync(void)
{
    u32 cnt = 0x2000;
    do
    {
        if (--cnt == 0)
        {
            REG_USB_CFG = USB_CMD_RD_NOP;
            return true;
        }
    }
    while (REG_USB_CFG & USB_STA_ACT);
    return false;
}

static void usb_cpy(void *dst, void *src, size_t size)
{
    u32 *_dst = dst;
    u32 *_src = src;
    do
    {
        _dst[0x00] = _src[0x00];
        _dst += 0x01;
        _src += 0x01;
        size -= 0x04;
    }
    while (size != 0);
}

static u32 usb_read(u8 *addr, size_t size)
{
    u32 status;
    do
    {
        u32 siz = size < 0x200 ? size : 0x200;
        u32 blk = 0x200 - siz;
        REG_USB_CFG = USB_CMD_RD | blk;
        status = usb_sync();
        if (status)
        {
            break;
        }
        usb_cpy(addr, REG_USB_DAT + blk, siz);
        addr += siz;
        size -= siz;
    }
    while (size > 0);
    return status;
}

static u32 usb_write(u8 *addr, size_t size)
{
    u32 status;
    REG_USB_CFG = USB_CMD_WR_NOP;
    do
    {
        u32 siz = size < 0x200 ? size : 0x200;
        u32 blk = 0x200 - siz;
        usb_cpy(REG_USB_DAT + blk, addr, siz);
        REG_USB_CFG = USB_CMD_WR | blk;
        status = usb_sync();
        if (status)
        {
            break;
        }
        addr += siz;
        size -= siz;
    }
    while (size > 0);
    return status;
}

void usb_init(void)
{
    REG_KEY     = 0xAA55;
    REG_SYS_CFG = 0;
    REG_USB_CFG = USB_CMD_RD_NOP;
    while (usb_ready())
    {
        u8 buf[0x10];
        usb_read(buf, sizeof(buf));
    }
}

void usb_update(void)
{
    while (usb_ready())
    {
        union
        {
            struct
            {
                void  *addr;
                size_t size;
                u8     mode;
            };
            u32 data[0x10/sizeof(u32)];
        }
        cmd;
        if (!usb_read(cmd.data, sizeof(cmd.data)))
        {
            if (cmd.mode == 0)
            {
                usb_write(cmd.addr, cmd.size);
            }
            else
            {
                usb_read(cmd.addr, cmd.size);
                osWritebackDCache(cmd.addr, cmd.size);
                osInvalICache(cmd.addr, cmd.size);
            }
        }
    }
}
