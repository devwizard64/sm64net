/******************************************************************************/
/*                  SM64Net - An Internet framework for SM64                  */
/*                    Copyright (C) 2019 - 2021  devwizard                    */
/*        This project is licensed under the terms of the GNU General         */
/*        Public License version 2.  See LICENSE for more information.        */
/******************************************************************************/

extern struct
{
    u8 page[NET_PAGE_LEN];
    char data[NET_PAGE_LEN][NET_PAGE_SIZE];
}
net_heap;

static void *net_alloc(size_t size)
{
    int i;
    int page = 0;
    int pages = (size+NET_PAGE_SIZE-1) / NET_PAGE_SIZE;
    if (pages >= 0x100) return NULL;
    for (page = 0; page < NET_PAGE_LEN; page++)
    {
        for (i = 0; i < pages; i++)
        {
            if (net_heap.page[page+i] > 0) goto cnt;
        }
        net_heap.page[page] = pages;
        for (i = 1; i < pages; i++) net_heap.page[page+i] = 1;
        return &net_heap.data[page];
    cnt:
        page += i;
    }
    return NULL;
}

static void net_free(void *ptr)
{
    int i;
    int page = ((char *)ptr-(char *)net_heap.data) / NET_PAGE_SIZE;
    int pages = net_heap.page[page];
    for (i = 0; i < pages; i++) net_heap.page[page+i] = 0;
}

static void net_mem_init(void)
{
    bzero(net_heap.page, sizeof(net_heap.page));
}
