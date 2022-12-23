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
    buf = B""
    while length > 0:
        try: x = self.recv(length)
        except: return None
        if len(x) == 0: return None
        buf += x
        length -= len(x)
    return buf

def init():
    global s_tcp
    global s_udp
    global save
    global np_table
    s_tcp = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s_tcp.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s_tcp.bind(("", sm64net.NET_PORT))
    s_tcp.listen(sm64net.NP_LEN)
    s_udp = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s_udp.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s_udp.bind(("", sm64net.NET_PORT))
    save = sm64net.SAVE("save.bin")
    np_table = sm64net.NP_LEN*[None]

def exit():
    s_tcp.shutdown(socket.SHUT_RDWR)
    s_tcp.close()
    s_udp.close()

def update_connect(sock, addr):
    with open("ban.txt", "r") as f: data = f.read()
    for line in data.split("\n"):
        a, sep, msg = line.partition(" ")
        if a == addr:
            if msg != "":
                data = struct.pack(">i", -1) + msg.encode()
                data = data[:sm64net.NP_TCP_SIZE]
                data += B"\0" * (sm64net.NP_TCP_SIZE-len(data))
                sock.sendall(data)
            sock.close()
            break
    else:
        for i, np in enumerate(np_table):
            if np == None or np.s_addr == addr:
                np = np_table[i] = sm64net.NET_PL(sock, s_udp, addr)
                np.update_connect()
                break
        else:
            sock.close()

def update():
    rd, wr, er = select.select(
        [s_tcp, s_udp] + [np.s_tcp for np in np_table if np != None], [], [], 0
    )
    for s in rd:
        if s == s_tcp:
            sock, addr = s.accept()
            update_connect(sock, addr[0])
        elif s == s_udp:
            data, addr = s.recvfrom(sm64net.NP_UDP_SIZE)
            for np in np_table:
                if np != None and np.s_addr == addr[0]:
                    np.update_udp(data)
                    break
        else:
            data = recvall(s, sm64net.NP_TCP_SIZE)
            for i, np in enumerate(np_table):
                if np != None and np.s_tcp == s:
                    if data != None: np.update_tcp(data)
                    else: np_table[i] = None
                    break
