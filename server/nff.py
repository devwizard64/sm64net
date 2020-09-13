#                   SM64Net - An Internet framework for SM64
#                    Copyright (C) 2019, 2020  devwizard
#       This project is licensed under the GNU General Public License
#       version 2.  See LICENSE for more information.

import struct

import server

def byteswap(data):
    return B"".join([data[i:i+4][::-1] for i in range(0, len(data), 4)])

def read(fn):
    with open(fn, "rb") as f:
        data = f.read()
    if data[:0x04] == B"\x00FFN" or data[:0x04] == B"\x01FFN":
        data = byteswap(data)
    return data

def write(sock, addr, data):
    header = struct.pack(">III", 0, addr, len(data))
    sock.sendall(header + (0x100-len(header))*B"\x00" + data)

def write_data(sock, data):
    i = 4
    while struct.unpack(">I", data[i:i+0x04])[0] != 0:
        addr, start, end = struct.unpack(">III", data[i:i+0x0C])
        i += 0x0C
        write(sock, addr, data[start:end])

def write_file(sock, fn):
    write_data(sock, read(fn))

def write_file_all(fn):
    data = read(fn)
    for sock in server.sockets:
        if sock != None:
            write_data(sock, data)
