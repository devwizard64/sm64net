/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                   Copyright (C) 2019, 2020  devwizard                      *
 *      This project is licensed under the GNU General Public License         *
 *      version 2.  See LICENSE for more information.                         *
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "types.h"
#include "sm64net.h"

#include "assert.h"
#include "mem.h"
#include "net.h"

int main(int argc, const char **argv)
{
    const char *proc;
    const char *addr;
    const char *str;
    long int port;
    u32 colour;
    u32 i;
    u8  name[NET_PLAYER_NAME_LEN];
    puts(
        "SM64Net Client " _VERSION "\n"
        "Copyright (C) 2019, 2020  devwizard\n"
        "This project is licensed under the GNU General Public License version "
        "2.  See\n"
        "LICENSE for more information."
    );
    if (argc != 6)
    {
        fprintf(
            stderr, "usage: %s <proc> <addr> <port> <colour> <name>\n",
            argv[0]
        );
        return EXIT_FAILURE;
    }
    proc = argv[1];
    addr = argv[2];
    port = strtol(argv[3], NULL, 0);
    assert_main("invalid port");
    colour = strtol(argv[4], NULL, 16);
    assert_main("invalid colour");
    memset(name, 0xFF, sizeof(name));
    str = argv[5];
    for (i = 0; *str != 0x00 && i < NET_PLAYER_NAME_LEN-1; i++)
    {
        char n[3];
        n[0] = *str++;
        n[1] = *str++;
        n[2] = 0x00;
        name[i] = strtol(n, NULL, 16);
        assert_main("invalid name");
    }
    if (mem_init(proc) || net_init(addr, port, colour, name))
    {
    #ifdef WIN32
        getchar();
    #endif
        return EXIT_FAILURE;
    }
    while (!net_update());
    return EXIT_SUCCESS;
}
