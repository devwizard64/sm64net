#                   SM64Net - An Internet framework for SM64
#                     Copyright (C) 2019 - 2022  devwizard
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
        "Copyright (C) 2019 - 2022  devwizard\n"
        "This project is licensed under the terms of the GNU General Public "
        "License\n"
        "version 2.  See LICENSE for more information."
    )
    server.init()
    ui.init()
    try:
        while ui.update():
            server.update()
            time.sleep(1.0/60)
    finally:
        ui.exit()
        server.exit()
    return 0

if __name__ == "__main__":
    sys.exit(main())
