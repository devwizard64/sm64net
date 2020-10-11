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

#include "assert.h"
#include "mem.h"
#include "net.h"

int main(int argc, const char **argv)
{
    const char *proc;
    const char *addr;
    long int port;
    puts(
        "SM64Net Client " _VERSION "\n"
        "Copyright (C) 2019, 2020  devwizard\n"
        "This project is licensed under the GNU General Public License version "
        "2.  See\n"
        "LICENSE for more information."
    );
    if (argc < 4)
    {
        fprintf(
            stderr, "usage: %s <proc> <addr> <port> [nff ...]\n",
            argv[0]
        );
        return EXIT_FAILURE;
    }
    proc = argv[1];
    addr = argv[2];
    port = strtol(argv[3], NULL, 0);
    if (errno != 0)
    {
        fprintf(stderr, "error: invalid port\n");
        return EXIT_FAILURE;
    }
    if (mem_init(proc) || net_init(addr, port, argv+4, argc-4))
    {
    #ifdef WIN32
        getchar();
    #endif
        return EXIT_FAILURE;
    }
    while (!net_update());
    return EXIT_SUCCESS;
}
