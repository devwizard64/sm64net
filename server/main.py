#                   SM64Net - An Internet framework for SM64
#                     Copyright (C) 2019 - 2021  devwizard
#         This project is licensed under the terms of the GNU General
#         Public License version 2.  See LICENSE for more information.

import sys
import time

import sm64net

import server
import ui

def main(argv):
    print(
        "SM64Net Server " + sm64net.VERSION + "\n"
        "Copyright (C) 2019 - 2021  devwizard\n"
        "This project is licensed under the terms of the GNU General Public "
        "License\n"
        "version 2.  See LICENSE for more information."
    )
    server.init(argv[1] if len(argv) > 1 else "default")
    ui.init()
    try:
        while True:
            for i in range(32):
                server.update()
                time.sleep(1.0/30.0 / sm64net.NP_LEN)
            if not ui.update():
                break
    finally:
        server.destroy()
        ui.destroy()
    return 0

if __name__ == "__main__":
    sys.exit(main(sys.argv))
