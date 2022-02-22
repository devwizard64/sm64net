#                   SM64Net - An Internet framework for SM64
#                     Copyright (C) 2019 - 2022  devwizard
#         This project is licensed under the terms of the GNU General
#         Public License version 2.  See LICENSE for more information.

import os
import struct
import select
import socket
import importlib

import sm64net

def recvall(self, length):
    msg = b""
    while length > 0:
        try: buf = self.recv(length)
        except: return None
        if len(buf) == 0: return None
        msg += buf
        length -= len(buf)
    return msg

def init():
    global s_tcp
    global s_udp
    global np_table
    s_tcp = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s_tcp.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s_tcp.bind(("", sm64net.NET_PORT))
    s_tcp.listen(5)
    s_udp = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s_udp.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s_udp.bind(("", sm64net.NET_PORT))
    np_table = sm64net.NP_LEN*[None]

def exit():
    s_tcp.shutdown(socket.SHUT_RDWR)
    s_tcp.close()
    s_udp.close()

def update_connect(sock, addr):
    with open("bans.txt", "r") as f: data = f.read()
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
                np = sm64net.NET_PL(sock, s_udp, addr)
                np_table[i] = np
                np.update_connect()
                break
        else:
            sock.close()

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
                    np.update_udp(data)
                    break
        else:
            data = recvall(s, sm64net.NP_TCP_SIZE)
            i = sockets.index(s)
            np = np_table[i]
            if np != None:
                if data != None: np.update_tcp(data)
                else: np_table[i] = None
