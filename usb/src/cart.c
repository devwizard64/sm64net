extern void __osPiGetAccess(void);
extern void __osPiRelAccess(void);

#define __cart_acs_get()        __osPiGetAccess()
#define __cart_acs_rel()        __osPiRelAccess()

static u32 __cart_rd(u32 addr)
{
    while (IO_READ(PI_STATUS_REG) & (PI_STATUS_DMA_BUSY|PI_STATUS_IO_BUSY));
    return IO_READ(addr);
}

static void __cart_wr(u32 addr, u32 data)
{
    while (IO_READ(PI_STATUS_REG) & (PI_STATUS_DMA_BUSY|PI_STATUS_IO_BUSY));
    IO_WRITE(addr, data);
}
