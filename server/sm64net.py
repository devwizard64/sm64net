#                   SM64Net - An Internet framework for SM64
#                     Copyright (C) 2019 - 2021  devwizard
#         This project is licensed under the terms of the GNU General
#         Public License version 2.  See LICENSE for more information.

import struct

import server

VERSION         = "1.1.1"

NET_PORT        = 0x1100

NP_LEN          = 32
NP_NAME_LEN     = 32
NP_UDP_SIZE     = 0x0200
NP_TCP_SIZE     = 0x0100
NP_SYS_SIZE     = 0x0100

np_name         = 0x0008

menu_table = (
    tuple("0123456789")                                 + \
    tuple("ABCDEFGHIJKLMNOPQRSTUVWXYZ")                 + \
    tuple("abcdefghijklmnopqrstuvwxyz")                 + \
    ("'", ".")                                          + \
    tuple(["[%02X]" % i for i in range(0x50 - 0x40)])   + \
    tuple(["[%c]" % c for c in "^v<>ABCZR"])            + \
    tuple(["[%02X]" % i for i in range(0x6F - 0x59)])   + \
    (",",)                                              + \
    tuple(["[%02X]" % i for i in range(0x9E - 0x70)])   + \
    (" ", "-")                                          + \
    tuple(["[%02X]" % i for i in range(0xD0 - 0xA0)])   + \
    ("  ", "the", "you")                                + \
    tuple(["[%02X]" % i for i in range(0xE0 - 0xD3)])   + \
    ("[%d]", "(", ")(", ")", "<->", "&", ":")           + \
    tuple(["[%02X]" % i for i in range(0xF2 - 0xE7)])   + \
    ("!", "%", "?", "{", "}", "~", "...")               + \
    ("[coin]", "[star]", "[x]", "[*]", "[star_outline]", "\\n", "")
)

class np_t:
    def __init__(self, tcp, udp, addr):
        self.s_udp  = udp
        self.s_tcp  = tcp
        self.s_addr = addr
        self.udp    = None
        self.tcp    = None
        self.name   = None

    def write_udp(self, data):
        self.s_udp.sendto(data, (self.s_addr, NET_PORT))
    def write_tcp(self, data):
        self.s_tcp.sendall(data)
    def write_mem(self, addr, data):
        header = struct.pack(">III", 0x00000000, addr, len(data))
        self.write_tcp(header + (0x100-len(header))*B"\x00" + data)

    def nff_read(self, fn):
        with open(fn, "rb") as f:
            data = f.read()
        if data[:0x04] != B"NFF\x00":
            raise ValueError("'%s' is not a valid NFF file" % fn)
        nff = []
        i = 0
        while True:
            addr, start, end = struct.unpack(">III", data[i+0x04:i+0x10])
            i += 0x0C
            if addr == 0x00000000:
                break
            nff.append((addr, data[start:end]))
        return nff
    def nff_write(self, nff):
        for addr, data in nff:
            self.write_mem(addr, data)
    def nff_write_file(self, fn):
        self.nff_write(self.nff_read(fn))

    def sync(self, write, tcp):
        s = server.np_table.index(self)
        for i, np in enumerate(server.np_table):
            if np != None and np != self:
                if write:
                    dst, src, n = np, self, s if s != 0 else i
                else:
                    dst, src, n = self, np, i if i != 0 else s
                if tcp:
                    w, d = dst.write_tcp, src.tcp
                else:
                    w, d = dst.write_udp, src.udp
                if d != None:
                    w(struct.pack(">I", n) + d[0x0004:])
    def sync_read_tcp(self):
        self.sync(False, True)
    def sync_write_udp(self, data):
        self.udp = data
        self.sync(True, False)
    def sync_write_tcp(self, data):
        self.tcp = data
        self.sync(True, True)

    def update_connect(self):
        self.sync_read_tcp()
    def update_tcp(self, data):
        self.sync_write_tcp(data)
        start = np_name
        end   = np_name + 1*(NP_NAME_LEN-1)
        self.name = "".join([
            menu_table[struct.unpack(">B", self.tcp[c:c+1])[0]]
            for c in range(start, end)
        ])
    def update_udp(self, data):
        self.sync_write_udp(data)
