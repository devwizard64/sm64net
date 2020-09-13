#                   SM64Net - An Internet framework for SM64
#                    Copyright (C) 2019, 2020  devwizard
#       This project is licensed under the GNU General Public License
#       version 2.  See LICENSE for more information.

import sys
import time

import sm64net

import server
import ui

def main(argc, argv):
    print(
        "SM64Net Server " + sm64net.VERSION + "\n" +
        "Copyright (C) 2019, 2020  devwizard\n" +
        "This project is licensed under the GNU General Public License " +
        "version 2.  See\n" +
        "LICENSE for more information."
    )
    server.init(argv[1] if argc >= 2 else "default")
    ui.init()
    try:
        while True:
            for i in range(32):
                server.update()
                time.sleep(1.0/30.0 / sm64net.NET_PLAYER_LEN)
            if not ui.update():
                break
    finally:
        server.destroy()
        ui.destroy()
    return 0

if __name__ == "__main__":
    sys.exit(main(len(sys.argv), sys.argv))
