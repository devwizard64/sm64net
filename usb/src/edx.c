#define EDX_BASE_REG            0x1F800000

#define EDX_USB_CFG_REG         0x0004
#define EDX_USB_DAT_REG         0x0400

#define EDX_SYS_CFG_REG         0x8000
#define EDX_KEY_REG             0x8004

#define EDX_CFG_SDRAM_ON        0x0000

#define EDX_USB_CFG             0x8000
#define EDX_USB_CTR             0x4000

#define EDX_USB_ACT             0x0200
#define EDX_USB_RD              0x0400
#define EDX_USB_WR              0x0000

#define EDX_USB_RXF             0x0400
#define EDX_USB_TXE             0x0800
#define EDX_USB_PWR             0x1000
#define EDX_USB_BSY             0x2000

#define EDX_USB_CMD_RD_NOP      (EDX_USB_CFG|EDX_USB_CTR|EDX_USB_RD)
#define EDX_USB_CMD_RD          (EDX_USB_CFG|EDX_USB_CTR|EDX_USB_RD|EDX_USB_ACT)
#define EDX_USB_CMD_WR_NOP      (EDX_USB_CFG|EDX_USB_CTR|EDX_USB_WR)
#define EDX_USB_CMD_WR          (EDX_USB_CFG|EDX_USB_CTR|EDX_USB_WR|EDX_USB_ACT)

#define EDX_KEY                 0xAA55

#define __edx_reg_rd(reg)       __cart_rd(EDX_BASE_REG|(reg))
#define __edx_reg_wr(reg, val)  __cart_wr(EDX_BASE_REG|(reg), val)

#define __edx_usb_is_rd()   \
    ((__edx_reg_rd(EDX_USB_CFG_REG) & (EDX_USB_PWR|EDX_USB_RXF)) == EDX_USB_PWR)
#define __edx_usb_is_wr()   \
    ((__edx_reg_rd(EDX_USB_CFG_REG) & (EDX_USB_PWR|EDX_USB_TXE)) == EDX_USB_PWR)

static int __edx_usb_sync(void)
{
    int n = 8192;
    do
    {
        if (--n == 0)
        {
            __edx_reg_wr(EDX_USB_CFG_REG, EDX_USB_CMD_RD_NOP);
            return -1;
        }
    }
    while (__edx_reg_rd(EDX_USB_CFG_REG) & EDX_USB_ACT);
    return 0;
}

static int __edx_init(void)
{
    __edx_reg_wr(EDX_KEY_REG, EDX_KEY);
    __edx_reg_wr(EDX_SYS_CFG_REG, EDX_CFG_SDRAM_ON);
    __edx_reg_wr(EDX_USB_CFG_REG, EDX_USB_CMD_RD_NOP);
    while (__edx_usb_is_rd())
    {
        __edx_reg_wr(EDX_USB_CFG_REG, EDX_USB_CMD_RD);
        if (__edx_usb_sync()) break;
    }
    return 0;
}

static int __edx_usb_rd_dram(void *dram, size_t size)
{
    char *addr = dram;
    int i;
    int n;
    while (size > 0)
    {
        u32 cart;
        n = MIN(512, size);
        __edx_reg_wr(EDX_USB_CFG_REG, EDX_USB_CMD_RD|(512-n));
        if (__edx_usb_sync()) return -1;
        cart = EDX_BASE_REG|EDX_USB_DAT_REG|(512-n);
        for (i = 0; i < n; i += 4) *(u32 *)(addr+i) = __cart_rd(cart+i);
        addr += n;
        size -= n;
    }
    return 0;
}

static int __edx_usb_wr_dram(const void *dram, size_t size)
{
    const char *addr = dram;
    int i;
    int n;
    __edx_reg_wr(EDX_USB_CFG_REG, EDX_USB_CMD_WR_NOP);
    while (size > 0)
    {
        u32 cart;
        n = MIN(512, size);
        cart = EDX_BASE_REG|EDX_USB_DAT_REG|(512-n);
        for (i = 0; i < n; i += 4) __cart_wr(cart+i, *(u32 *)(addr+i));
        __edx_reg_wr(EDX_USB_CFG_REG, EDX_USB_CMD_WR|(512-n));
        if (__edx_usb_sync()) return -1;
        addr += n;
        size -= n;
    }
    return 0;
}
