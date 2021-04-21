#                   SM64Net - An Internet framework for SM64
#                     Copyright (C) 2019 - 2021  devwizard
#         This project is licensed under the terms of the GNU General
#         Public License version 2.  See LICENSE for more information.

import sys
import time

import sm64net
import server
import ui

def main():
    print(
        sm64net.VERSION_STR + "\n"
        "Copyright (C) 2019 - 2021  devwizard\n"
        "This project is licensed under the terms of the GNU General Public "
        "License\n"
        "version 2.  See LICENSE for more information."
    )
    server.init()
    ui.init()
    try:
        while ui.update():
            for _ in range(sm64net.NP_LEN):
                server.update()
                time.sleep((1.0/60) / sm64net.NP_LEN)
    finally:
        server.destroy()
        ui.destroy()
    return 0

if __name__ == "__main__":
    sys.exit(main())
