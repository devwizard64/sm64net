/******************************************************************************/
/*                  SM64Net - An Internet framework for SM64                  */
/*                    Copyright (C) 2019 - 2022  devwizard                    */
/*        This project is licensed under the terms of the GNU General         */
/*        Public License version 2.  See LICENSE for more information.        */
/******************************************************************************/

#include <sm64.h>

#include "cart.c"
#include "edx.c"

#define netR    0x6E657452
#define netW    0x6E657457

extern u64 stack_usb[];

static u32 usb_cmd[4];
static OSMesgQueue usb_mq;
static OSMesg usb_msg[1];

static void usb_main(UNUSED void *arg)
{
    __cart_acs_get();
    IO_WRITE(PI_BSD_DOM1_LAT_REG, 0);
    IO_WRITE(PI_BSD_DOM1_PWD_REG, 12);
    IO_WRITE(PI_BSD_DOM1_PGS_REG, 7);
    IO_WRITE(PI_BSD_DOM1_RLS_REG, 2);
    __edx_init();
    __cart_acs_rel();
    for (;;)
    {
        osRecvMesg(&usb_mq, NULL, OS_MESG_BLOCK);
        __cart_acs_get();
        IO_WRITE(PI_BSD_DOM1_LAT_REG, 40);
        while (__edx_usb_is_rd())
        {
            if (__edx_usb_rd_dram(usb_cmd, sizeof(usb_cmd))) break;
            switch (usb_cmd[0])
            {
                case netR:
                    __edx_usb_wr_dram((void *)usb_cmd[1], usb_cmd[2]);
                    break;
                case netW:
                    __edx_usb_rd_dram((void *)usb_cmd[1], usb_cmd[2]);
                    osInvalICache((void *)usb_cmd[1], usb_cmd[2]);
                    break;
            }
        }
        IO_WRITE(PI_BSD_DOM1_LAT_REG, 0);
        __cart_acs_rel();
    }
}

void usb_init(void)
{
    osCreateMesgQueue(&usb_mq, usb_msg, 1);
    osCreateThread(
        &thread_rmon, 2, usb_main, NULL,
        stack_entry+lenof(stack_entry), 50
    );
    osStartThread(&thread_rmon);
}

void usb_update(void)
{
    osSendMesg(&usb_mq, (OSMesg)0, OS_MESG_NOBLOCK);
}
