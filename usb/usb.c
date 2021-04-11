/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                    Copyright (C) 2019 - 2021  devwizard                    *
 *        This project is licensed under the terms of the GNU General         *
 *        Public License version 2.  See LICENSE for more information.        *
 ******************************************************************************/

#include <types.h>
#include <sm64.h>

#define REG_USB_CFG     0x1F800004
#define REG_USB_DAT     0x1F800400

#define REG_SYS_CFG     0x1F808000
#define REG_KEY         0x1F808004

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

#define usb_ready()     \
    ((usb_reg_r(REG_USB_CFG) & (USB_STA_PWR | USB_STA_RXF)) == USB_STA_PWR)

static OSIoMesg    iomesg_usb;
static OSMesgQueue mq_usb;
static OSMesg      msg_usb[1];

static void usb_dma(u32 addr, void *data, size_t size, uint mode)
{
    while (osPiStartDma(
        &iomesg_usb, OS_MESG_PRI_NORMAL, mode, addr, data, size, &mq_usb
    ) < 0);
    osRecvMesg(&mq_usb, NULL, OS_MESG_BLOCK);
}

static u32 usb_reg_r(u32 addr)
{
    union {u32 w; u64 d;} reg;
    osInvalDCache(&reg.w, sizeof(reg.w));
    usb_dma(addr, &reg.w, sizeof(reg.w), OS_READ);
    return reg.w;
}

static void usb_reg_w(u32 addr, u32 val)
{
    union {u32 w; u64 d;} reg = {val};
    osWritebackDCache(&reg.w, sizeof(reg.w));
    usb_dma(addr, &reg.w, sizeof(reg.w), OS_WRITE);
}

static void usb_sync(void)
{
    while (usb_reg_r(REG_USB_CFG) & USB_STA_ACT);
}

static void usb_read(void *dst, size_t size)
{
    u8 *addr = dst;
    do
    {
        uint siz = size < 0x200 ? size : 0x200;
        uint blk = 0x200 - siz;
        usb_reg_w(REG_USB_CFG, USB_CMD_RD | blk);
        usb_sync();
        usb_dma(REG_USB_DAT + blk, addr, siz, OS_READ);
        addr += siz;
        size -= siz;
    }
    while (size > 0);
}

static void usb_write(const void *src, size_t size)
{
    u8 *addr = (void *)src;
    do
    {
        uint siz = size < 0x200 ? size : 0x200;
        uint blk = 0x200 - siz;
        usb_reg_w(REG_USB_CFG, USB_CMD_WR_NOP);
        usb_dma(REG_USB_DAT + blk, addr, siz, OS_WRITE);
        usb_reg_w(REG_USB_CFG, USB_CMD_WR | blk);
        usb_sync();
        addr += siz;
        size -= siz;
    }
    while (size > 0);
}

void usb_init(void)
{
    osCreateMesgQueue(&mq_usb, msg_usb, lenof(msg_usb));
    usb_reg_w(REG_KEY, 0xAA55);
    usb_reg_w(REG_SYS_CFG, 0);
    usb_reg_w(REG_USB_CFG, USB_CMD_RD_NOP);
    while (usb_ready())
    {
        u64 data[0x10/sizeof(u64)];
        usb_read(data, sizeof(data));
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
            u64 data[0x10/sizeof(u64)];
        }
        cmd;
        usb_read(cmd.data, sizeof(cmd.data));
        if (cmd.mode == 0)
        {
            usb_write(cmd.addr, cmd.size);
        }
        else
        {
            osInvalICache(cmd.addr, cmd.size);
            usb_read(cmd.addr, cmd.size);
        }
    }
}
