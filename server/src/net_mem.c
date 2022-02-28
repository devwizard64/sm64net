/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                    Copyright (C) 2019 - 2021  devwizard                    *
 *        This project is licensed under the terms of the GNU General         *
 *        Public License version 2.  See LICENSE for more information.        *
 ******************************************************************************/

extern struct
{
    u8 page[NET_PAGE_LEN];
    u8 data[NET_PAGE_LEN][NET_PAGE_SIZE];
}
net_heap;

static void *net_alloc(size_t size)
{
    uint pages;
    uint page;
    uint i;
    pages = (size+sizeof(net_heap.data[0])-1) / sizeof(net_heap.data[0]);
    if (pages >= 0x100) return NULL;
    for (page = 0; page < lenof(net_heap.page); page++)
    {
        for (i = 0; i < pages; i++)
        {
            if (net_heap.page[page+i] > 0) goto end;
        }
        net_heap.page[page] = pages;
        for (i = 1; i < pages; i++) net_heap.page[page+i] = 1;
        return &net_heap.data[page];
    end:;
    }
    return NULL;
}

static void net_free(void *ptr)
{
    uint pages;
    uint page;
    uint i;
    page = (typeof(&net_heap.data[0]))ptr - net_heap.data;
    pages = net_heap.page[page];
    for (i = 0; i < pages; i++) net_heap.page[page+i] = 0;
}

static void net_mem_init(void)
{
    bzero(net_heap.page, sizeof(net_heap.page));
}
