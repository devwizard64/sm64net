#                   SM64Net - An Internet framework for SM64
#                     Copyright (C) 2019 - 2022  devwizard
#         This project is licensed under the terms of the GNU General
#         Public License version 2.  See LICENSE for more information.

import struct

VERSION         = "2.2"
REVISION        = "1"
VERSION_STR     = "SM64Net Server " + VERSION+"."+REVISION

osException     = 0x80327650

NET_PORT        = 4352

NP_DATA         = 0x80025C00
NP_LEN          = 32
NP_NAME_LEN     = 32
NP_UDP_SIZE     = 0x200
NP_TCP_SIZE     = 0x100
NP_SYS_SIZE     = 0x100

NP_CMD_SYNC     = 1
NP_CMD_COL      = 2

np_name         = 0x0008
np_col          = 0x002D

import server

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

class NET_PL:
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
        try: self.s_tcp.sendall(data)
        except: pass
    def write_mem(self, addr, data):
        header = struct.pack(">iII", -2, addr, len(data))
        self.write_tcp(header + (0x100-len(header))*B"\x00" + data)

    def nff_read(self, fn):
        with open(fn, "rb") as f: data = f.read()
        if data[:4] != B"NFF\x00":
            raise ValueError("'%s' is not a valid NFF file" % fn)
        nff = []
        i = 4
        while True:
            addr, start, end = struct.unpack(">III", data[i:i+12])
            i += 12
            if addr == 0: break
            nff.append((addr, data[start:end]))
        return nff
    def nff_write(self, nff):
        for addr, data in nff: self.write_mem(addr, data)
    def nff_write_file(self, fn):
        self.nff_write(self.nff_read(fn))

    def sync(self, write, tcp):
        s = server.np_table.index(self)
        for i, np in enumerate(server.np_table):
            if np != None and np != self:
                if write:   dst, src, n = np, self, s if s != 0 else i
                else:       dst, src, n = self, np, i if i != 0 else s
                if tcp:     w, d = dst.write_tcp, src.tcp
                else:       w, d = dst.write_udp, src.udp
                if d != None: w(struct.pack(">I", n) + d[4:])

    def cmd_sync(self, data):
        data = data[:np_col] + B"\x00" + data[np_col+1:]
        self.tcp = data
        self.sync(True, True)
        start = np_name
        end   = np_name + 1*(NP_NAME_LEN-1)
        self.name = "".join([
            menu_table[struct.unpack(">B", self.tcp[c:c+1])[0]]
            for c in range(start, end)
        ])
    def cmd_col(self, data):
        col, = struct.unpack(">B", data[np_col:np_col+1])
        s = server.np_table.index(self)
        for i, np in enumerate(server.np_table):
            if np != None and np != self:
                n = s if s != 0 else i
                x = i if i != 0 else s
                a = struct.pack(">I", n)
                b = struct.pack(">B", n if x == col else 0)
                np.write_tcp(a + data[4:np_col] + b + data[np_col+1:])
    cmd_table = {
        NP_CMD_SYNC:    cmd_sync,
        NP_CMD_COL:     cmd_col,
    }

    def update_connect(self):
        self.write_tcp(struct.pack(">II55s1x",
            osException, NP_DATA, VERSION.encode()
        ))
        self.nff_write_file("save.nff")
        self.nff_write_file("dab.nff")
        self.nff_write_file("dprint.nff")
        self.nff_write_file("build/main.nff")
        self.sync(False, True)
    def update_tcp(self, data):
        cmd, = struct.unpack(">I", data[:4])
        if cmd in self.cmd_table: self.cmd_table[cmd](self, data)
        else: print("warning: bad cmd 0x%08X" % cmd)
    def update_udp(self, data):
        self.udp = data
        self.sync(True, False)
