#                   SM64Net - An Internet framework for SM64
#                    Copyright (C) 2019, 2020  devwizard
#       This project is licensed under the GNU General Public License
#       version 2.  See LICENSE for more information.

import os
import struct
import select
import socket
import importlib

import sm64net

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

plugin     = None
tcp_server = None
udp_server = None
sockets    = sm64net.NET_PLAYER_LEN*[None]
addrs      = sm64net.NET_PLAYER_LEN*[None]
names      = sm64net.NET_PLAYER_LEN*[None]

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
    global tcp_server
    global udp_server
    plugin = importlib.import_module("plugin." + fn)
    tcp_server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    tcp_server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    tcp_server.bind(("", sm64net.NET_PORT))
    tcp_server.listen(5)
    udp_server = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    udp_server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    udp_server.bind(("", sm64net.NET_PORT))
    plugin.init()

def destroy():
    global tcp_server
    global udp_server
    plugin.destroy()
    tcp_server.shutdown(socket.SHUT_RDWR)
    tcp_server.close()
    udp_server.close()

def update_connect(s):
    global tcp_server
    global sockets
    global addrs
    sock, addr = s.accept()
    addr = addr[0]
    with open("bans.txt", "r") as f:
        data = f.read()
    for line in data.split("\n"):
        a, sep, msg = line.partition(" ")
        if a == addr:
            if msg != "":
                data = struct.pack(">I", 0xFFFFFFFF) + msg.encode("utf-8")
                data = data[:sm64net.NET_PLAYER_TCP_SIZE]
                data += B"\x00" * (sm64net.NET_PLAYER_TCP_SIZE-len(data))
                sock.sendall(data)
            sock.close()
            break
    else:
        for i in range(len(sockets)):
            if sockets[i] == None or addrs[i] == addr:
                sockets[i] = sock
                addrs[i] = addr
                plugin.update_connect(s, i, sock, addr)
                break
        else:
            sock.close()

def update_udp(s):
    global addrs
    global names
    data, a = s.recvfrom(sm64net.NET_PLAYER_UDP_SIZE)
    a = a[0]
    if a in addrs:
        i = addrs.index(a)
        if names[i] == None:
            start = sm64net.np_name
            end   = sm64net.np_name + 1*(sm64net.NET_PLAYER_NAME_LEN-1)
            names[i] = "".join([
                menu_table[struct.unpack(">B", data[c:c+1])[0]]
                for c in range(start, end)
            ])
        plugin.update_udp(s, i, data, a)
        for o, addr in enumerate(addrs):
            if addr != None and addr != a:
                header = struct.pack(">I", i if i != 0 else o)
                s.sendto(header + data[0x0004:], (addr, sm64net.NET_PORT))

def update_tcp(s):
    global sockets
    global names
    global addrs
    i = sockets.index(s)
    data = recvall(s, sm64net.NET_PLAYER_TCP_SIZE)
    if data != None:
        plugin.update_tcp(s, i, data)
        cmd = struct.unpack(">I", data[:0x0004])
        if cmd == 0x00000001:
            for o, sock in enumerate(sockets):
                if sock != None and sock != s:
                    header = struct.pack(">I", i if i != 0 else o)
                    sock.sendall(header + data[0x0004:])
        else:
            plugin.update_tcp_cmd(s, i, data, cmd)
    else:
        sockets[i] = None
        addrs[i] = None
        names[i] = None
        plugin.update_disconnect(s, i)

def update():
    global tcp_server
    global udp_server
    global sockets
    rd, wr, er = select.select(
        [tcp_server, udp_server] + [s for s in sockets if s != None],
        [], [], 0
    )
    for s in rd:
        if s == tcp_server:
            update_connect(s)
        elif s == udp_server:
            update_udp(s)
        else:
            update_tcp(s)
    plugin.update()
