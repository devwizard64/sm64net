#                   SM64Net - An Internet framework for SM64
#                     Copyright (C) 2019 - 2021  devwizard
#         This project is licensed under the terms of the GNU General
#         Public License version 2.  See LICENSE for more information.

import os
import struct
import select
import socket
import importlib

import sm64net

plugin   = None
s_tcp    = None
s_udp    = None
np_table = None

def recvall(self, length):
    msg = b""
    while length:
        try:
            buf = self.recv(length)
        except:
            return None
        if len(buf) == 0:
            return None
        msg += buf
        length -= len(buf)
    return msg

def init(fn):
    global plugin
    global s_tcp
    global s_udp
    global np_table
    plugin = importlib.import_module("plugin." + fn)
    s_tcp = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s_tcp.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s_tcp.bind(("", sm64net.NET_PORT))
    s_tcp.listen(5)
    s_udp = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s_udp.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s_udp.bind(("", sm64net.NET_PORT))
    np_table = sm64net.NP_LEN*[None]
    plugin.init()

def destroy():
    plugin.destroy()
    s_tcp.shutdown(socket.SHUT_RDWR)
    s_tcp.close()
    s_udp.close()

def update_connect(sock, addr):
    with open("bans.txt", "r") as f:
        data = f.read()
    for line in data.split("\n"):
        a, sep, msg = line.partition(" ")
        if a == addr:
            if msg != "":
                data = struct.pack(">i", -1) + msg.encode()
                data = data[:sm64net.NP_TCP_SIZE]
                data += B"\x00" * (sm64net.NP_TCP_SIZE-len(data))
                sock.sendall(data)
            sock.close()
            break
    else:
        for i, np in enumerate(np_table):
            if np == None or np.s_addr == addr:
                np = sm64net.np_t(sock, s_udp, addr)
                np_table[i] = np
                np.update_connect()
                plugin.np_update_connect(np)
                break
        else:
            sock.close()

def update_udp(self, data):
    self.udp = data
    plugin.np_update_udp(self)
    self.update_udp()

def update_tcp_sync(self, data):
    self.tcp = data
    plugin.np_update_tcp(self)
    self.update_tcp()

update_tcp_table = {sm64net.NP_CMD_SYNC: update_tcp_sync}

def update_tcp(self, data):
    cmd, = struct.unpack(">I", data[:4])
    if cmd in update_tcp_table:
        update_tcp_table[cmd](self, data)
    else:
        print("warning: bad tcp cmd 0x%08X" % cmd)

def update():
    sockets = [np.s_tcp for np in np_table if np != None] + [s_tcp, s_udp]
    rd, wr, er = select.select(sockets, [], [], 0)
    for s in rd:
        if s == s_tcp:
            sock, addr = s.accept()
            update_connect(sock, addr[0])
        elif s == s_udp:
            data, addr = s.recvfrom(sm64net.NP_UDP_SIZE)
            addr = addr[0]
            for np in np_table:
                if np != None and np.s_addr == addr:
                    update_udp(np, data)
                    break
        else:
            data = recvall(s, sm64net.NP_TCP_SIZE)
            i = sockets.index(s)
            np = np_table[i]
            if np != None:
                if data != None:
                    update_tcp(np, data)
                else:
                    plugin.np_update_disconnect(np)
                    np_table[i] = None
    for np in np_table:
        plugin.np_update(np)
    plugin.update()
