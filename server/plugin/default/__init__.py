#                   SM64Net - An Internet framework for SM64
#                    Copyright (C) 2019, 2020  devwizard
#       This project is licensed under the GNU General Public License
#       version 2.  See LICENSE for more information.

import nff

def init():
    return

def destroy():
    return

def update_connect(s, i, sock, addr):
    nff.write_file(sock, "plugin/default/build/main.nff")

def update_udp(s, i, data, a):
    return

def update_tcp(s, i, data):
    return

def update_tcp_cmd(s, i, data, cmd):
    return

def update_disconnect(s, i):
    return

def update():
    return
