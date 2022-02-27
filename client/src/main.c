/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                    Copyright (C) 2019 - 2022  devwizard                    *
 *        This project is licensed under the terms of the GNU General         *
 *        Public License version 2.  See LICENSE for more information.        *
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <types.h>

#include "net.h"

static void main_exit(void)
{
#ifdef WIN32
    getchar();
#endif
}

int main(int argc, char *argv[])
{
    const char *proc;
    const char *addr;
    long int port;
    atexit(main_exit);
    puts(
        "SM64Net Client " VERSION "." REVISION "\n"
        "Copyright (C) 2019 - 2022  devwizard\n"
        "This project is licensed under the terms of the GNU General Public "
        "License\n"
        "version 2.  See LICENSE for more information.\n"
    );
    if (argc < 4)
    {
        fprintf(stderr, "usage: %s <proc> <addr> <port> [nff ...]\n", argv[0]);
        return EXIT_FAILURE;
    }
    proc = argv[1];
    addr = argv[2];
    errno = 0;
    port = strtol(argv[3], NULL, 0);
    if (errno != 0 || port < 1 || port > 65535) eprint("invalid port\n");
    net_init(proc, addr, port, argc-4, argv+4);
    while (true) net_update();
    return EXIT_SUCCESS;
}
