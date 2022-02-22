/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                    Copyright (C) 2019 - 2022  devwizard                    *
 *        This project is licensed under the terms of the GNU General         *
 *        Public License version 2.  See LICENSE for more information.        *
 ******************************************************************************/

#include <types.h>
#include <sm64.h>

#define PI_BSD_DOM1_LAT_REG     0x04600014
#define PI_BSD_DOM1_PWD_REG     0x04600018

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

#define REG(x)  (*(volatile u32 *)(0xA0000000 | (x)))

static u32 usb_cmd[4];
static OSMesgQueue usb_mq;
static OSMesg usb_msg[1];
static const char *volatile usb_status;

static void usb_dma(u32 cart, void *dram, size_t size, int direction)
{
    OSIoMesg mb;
    while (osPiStartDma(
        &mb, OS_MESG_PRI_HIGH, direction, cart, dram, size, &usb_mq
    ) < 0);
    osRecvMesg(&usb_mq, NULL, OS_MESG_BLOCK);
}

#define usb_can_rd()    \
    ((REG(REG_USB_CFG) & (USB_STA_PWR | USB_STA_RXF)) == USB_STA_PWR)
#define usb_can_wr()    \
    ((REG(REG_USB_CFG) & (USB_STA_PWR | USB_STA_TXE)) == USB_STA_PWR)

static int usb_busy(void)
{
    uint count = 1 << 21;
    while (REG(REG_USB_CFG) & USB_STA_ACT)
    {
        if (--count == 0)
        {
            REG(REG_USB_CFG) = USB_CMD_RD_NOP;
            return true;
        }
    }
    return false;
}

static int usb_rd(void *data, size_t size)
{
    u8 *addr = data;
    osInvalDCache(data, size);
    while (size > 0)
    {
        uint siz = size < 0x200 ? size : 0x200;
        uint blk = 0x200 - siz;
        int code;
        REG(REG_USB_CFG) = USB_CMD_RD | blk;
        if ((code = usb_busy())) return code;
        usb_dma(REG_USB_DAT+blk, addr, siz, OS_READ);
        addr += siz;
        size -= siz;
    }
    return false;
}

static int usb_wr(void *data, size_t size)
{
    u8 *addr = data;
    osWritebackDCache(data, size);
    REG(REG_USB_CFG) = USB_CMD_WR_NOP;
    while (size > 0)
    {
        uint siz = size < 0x200 ? size : 0x200;
        uint blk = 0x200 - siz;
        int code;
        usb_dma(REG_USB_DAT+blk, addr, siz, OS_WRITE);
        REG(REG_USB_CFG) = USB_CMD_WR | blk;
        if ((code = usb_busy())) return code;
        addr += siz;
        size -= siz;
    }
    return false;
}

void usb_init(void)
{
    REG(PI_BSD_DOM1_LAT_REG) = 4;
    REG(PI_BSD_DOM1_PWD_REG) = 12;
    REG(REG_KEY) = 0xAA55;
    REG(REG_SYS_CFG) = 0;
    REG(REG_USB_CFG) = USB_CMD_RD_NOP;
    while (usb_can_rd())
    {
        REG(REG_USB_CFG) = USB_CMD_RD;
        if (usb_busy()) break;
    }
    usb_status = NULL;
    osCreateMesgQueue(&usb_mq, usb_msg, lenof(usb_msg));
}

void usb_update(void)
{
    while (usb_can_rd())
    {
        if (usb_rd(usb_cmd, sizeof(usb_cmd)))
        {
            usb_status = "TOUTCMD";
            return;
        }
        switch (usb_cmd[0])
        {
            case 0x6E657452:
                if (usb_wr((void *)usb_cmd[1], usb_cmd[2]))
                {
                    usb_status = "TOUTWR";
                    return;
                }
                break;
            case 0x6E657457:
                osInvalICache((void *)usb_cmd[1], usb_cmd[2]);
                if (usb_rd((void *)usb_cmd[1], usb_cmd[2]))
                {
                    usb_status = "TOUTRD";
                    return;
                }
                break;
            default:
                usb_status = "ILLCMD";
                return;
                break;
        }
    }
}

void usb_draw(void)
{
    const char *str = usb_status;
    usb_status = NULL;
    if (str != NULL) dprint(20, 20, str);
}
