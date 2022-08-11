#                   SM64Net - An Internet framework for SM64
#                     Copyright (C) 2019 - 2022  devwizard
#         This project is licensed under the terms of the GNU General
#         Public License version 2.  See LICENSE for more information.

import os
import struct

VERSION         = "2.2"
REVISION        = "2"
VERSION_STR     = "SM64Net Server " + VERSION+"."+REVISION

osException     = 0x80327650

NET_PORT        = 4352

NP_DATA         = 0x80025C00
NP_LEN          = 32
NP_NAME_LEN     = 32
NP_UDP_SIZE     = 512
NP_TCP_SIZE     = 256
NP_SYS_SIZE     = 256

NP_CMD_SYNC     = 1
NP_CMD_COL      = 2
NP_CMD_SAVE     = 3

NP_SAVE_FLAGSET = 0
NP_SAVE_FLAGCLR = 1
NP_SAVE_STARSET = 2
NP_SAVE_COINSET = 3
NP_SAVE_ERASE   = 4
NP_SAVE_COPY    = 5

np_name         = 0x008
np_col          = 0x02B
np_save         = 0x02C

save            = 0x80207700

import server

msg_char_table = (
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
    ("[+]", "[*]", "[x]", "[-]", "[.]", "\\n", "")
)

class SAVE_DATA:
    def __init__(self):
        self.time = [15*[3-i] for i in range(4)]
    def time_update(self, file_, course):
        if self.time[file_][course] != 0:
            for i in range(4):
                if self.time[i][course] < self.time[file_][course]:
                    self.time[i][course] += 1
                    self.time[i][course] &= 3
            self.time[file_][course] = 0
    def time_update_all(self, file_):
        for i in range(15): self.time_update(file_, i)
    def pack(self):
        return B"".join([
            struct.pack(">I", sum([x << (2*i) for i, x in enumerate(t)]))
            for t in self.time
        ]) + struct.pack(">12x")

class SAVE_FILE:
    def __init__(self):
        self.flag = 0
        self.star = 25*[0]
        self.coin = 15*[0]
    def copy(self):
        file_ = SAVE_FILE()
        file_.flag = self.flag
        file_.star = self.star[:]
        file_.coin = self.coin[:]
        return file_
    def pack(self, flag=0):
        return struct.pack(">8xI", self.flag|flag) + B"".join([
            struct.pack(">B", x) for x in self.star+self.coin
        ])

class SAVE:
    def __init__(self, path):
        self.path = path
        self.file = [SAVE_FILE() for i in range(4)]
        self.data = SAVE_DATA()
        if os.path.isfile(self.path):
            with open(self.path, "rb") as f: data = f.read()
            for i in range(4):
                flag, = struct.unpack(">I", data[0x70*i+8:0x70*i+12])
                self.file[i].flag = flag & 0xFFF0FFFF
                self.file[i].star = [
                    struct.unpack(">B", data[n:n+1])[0]
                    for n in range(0x70*i+12, 0x70*i+37)
                ]
                self.file[i].coin = [
                    struct.unpack(">B", data[n:n+1])[0]
                    for n in range(0x70*i+37, 0x70*i+52)
                ]
            n = 0x1C0
            self.data.time = [
                [x >> (2*i) & 3 for i in range(15)]
                for x in struct.unpack(">IIII", data[0x1C0:0x1C0+16])
            ]
    def checksum(self, data):
        return data + struct.pack(">H", sum([
            struct.unpack(">B", data[i:i+1])[0]
            for i in range(len(data))
        ]) & 0xFFFF)
    def pack(self):
        return B"".join([
            2*self.checksum(x.pack()+B"DA") for x in self.file
        ]) + 2*self.checksum(self.data.pack()+B"HI")
    def save(self):
        data = self.pack()
        with open(self.path, "wb") as f: f.write(data)

class NET_PL:
    def __init__(self, tcp, udp, addr):
        self.s_udp  = udp
        self.s_tcp  = tcp
        self.s_addr = addr
        self.udp    = None
        self.tcp    = None
        self.name   = None
        self.save_flag = 0

    def write_udp(self, data):
        self.s_udp.sendto(data, (self.s_addr, NET_PORT))
    def write_tcp(self, data):
        try: self.s_tcp.sendall(data)
        except: pass
    def write_mem(self, addr, data):
        header = struct.pack(">iII", -2, addr, len(data))
        self.write_tcp(header + (0x100-len(header))*B"\0" + data)

    def nff_read(self, fn):
        with open(fn, "rb") as f: data = f.read()
        if data[:4] != B"NFF\0":
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
        data = data[:np_col] + B"\0" + data[np_col+1:]
        self.tcp = data
        self.sync(True, True)
        start = np_name
        end   = np_name + 1*(NP_NAME_LEN-1)
        self.name = "".join([
            msg_char_table[struct.unpack(">B", self.tcp[c:c+1])[0]]
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
    def cmd_save(self, data):
        cmd, file_, index, value = struct.unpack(
            ">BBhI", data[np_save:np_save+8]
        )
        if file_ >= 4: return
        save_flag = server.save.file[file_].flag | self.save_flag
        if cmd == NP_SAVE_FLAGSET:
            save_flag |= value
        elif cmd == NP_SAVE_FLAGCLR:
            save_flag &= ~value
        elif cmd == NP_SAVE_STARSET:
            if index < -1 or index >= 25: return
            if index < 0:   save_flag |= value << 24
            else:           server.save.file[file_].star[index] |= value & 0xFF
        elif cmd == NP_SAVE_COINSET:
            if index < 0 or index >= 25: return
            server.save.file[file_].coin[index] = value & 0xFF
            server.save.data.time_update(file_, index)
        elif cmd == NP_SAVE_ERASE:
            server.save.data.time_update_all(file_)
            server.save.file[file_] = SAVE_FILE()
        elif cmd == NP_SAVE_COPY:
            if index < 0 or index >= 4: return
            server.save.data.time_update_all(file_)
            server.save.file[file_] = server.save.file[index].copy()
        if cmd <= NP_SAVE_STARSET:
            save_flag |= 0x000001
            server.save.file[file_].flag = save_flag & 0xFFF0FFFF
            self.save_flag = save_flag & 0x000F0000
        f = server.save.file[file_].pack(self.save_flag)[8:]
        t = server.save.data.pack()[:16]
        for np in server.np_table:
            if np != None:
                np.write_mem(save+0x70*file_+8, f)
                np.write_mem(save+0x1C0, t)
        server.save.save()
    cmd_table = {
        NP_CMD_SYNC:    cmd_sync,
        NP_CMD_COL:     cmd_col,
        NP_CMD_SAVE:    cmd_save,
    }

    def update_connect(self):
        self.write_tcp(struct.pack(">II55s1x",
            osException, NP_DATA, VERSION.encode()
        ))
        self.nff_write_file("dprint.nff")
        self.nff_write_file("dab.nff")
        self.nff_write_file("build/main.nff")
        self.write_mem(save, server.save.pack())
        self.sync(False, True)
    def update_tcp(self, data):
        cmd, = struct.unpack(">I", data[:4])
        if cmd in self.cmd_table: self.cmd_table[cmd](self, data)
        else: print("warning: bad cmd 0x%08X" % cmd)
    def update_udp(self, data):
        self.udp = data
        self.sync(True, False)
