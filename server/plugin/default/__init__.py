#                   SM64Net - An Internet framework for SM64
#                     Copyright (C) 2019 - 2021  devwizard
#         This project is licensed under the terms of the GNU General
#         Public License version 2.  See LICENSE for more information.

import struct

import server

NP_CMD_TOUCH    = 2

np_touch        = 0x002D

def init():
    server.update_tcp_table[NP_CMD_TOUCH] = np_update_touch

def destroy():
    return

def update():
    return

def np_update(self):
    return

def np_update_connect(self):
    self.nff_write_file("plugin/print_font.nff")
    self.nff_write_file("plugin/default/build/main.nff")

def np_update_disconnect(self):
    return

def np_update_udp(self):
    return

def np_update_tcp(self):
    self.tcp = self.tcp[:np_touch] + B"\x00" + self.tcp[np_touch+1:]

def np_update_touch(self, data):
    self.tcp = data
    touch, = struct.unpack(">B", self.tcp[np_touch:np_touch+1])
    s = server.np_table.index(self)
    for i, np in enumerate(server.np_table):
        if np != None and np != self:
            n = s if s != 0 else i
            x = i if i != 0 else s
            a = struct.pack(">I", n)
            b = struct.pack(">B", n if x == touch else 0)
            np.write_tcp(a + self.tcp[4:np_touch] + b + self.tcp[np_touch+1:])
