/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                    Copyright (C) 2019 - 2021  devwizard                    *
 *        This project is licensed under the terms of the GNU General         *
 *        Public License version 2.  See LICENSE for more information.        *
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#ifdef WIN32

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#define poll WSAPoll

#define e_msg(c, msg)  \
    {if (c) {eprint(msg " (%d)\n", WSAGetLastError());}}

#define e_socket(e, msg)    e_msg((e) == SOCKET_ERROR, msg)
#define e_socketfd(e, msg)  e_msg((e) == INVALID_SOCKET, msg)

#else

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>

#define e_socket(e, msg)  \
    {if ((e) < 0) {eprint(msg " (%s)\n", strerror(errno));}}
#define e_socketfd  e_socket

#define SOCKET int

#endif

#include <types.h>
#include <sm64net.h>

#include "mem.h"

#define W(i) (b[(i)+0] << 24 | b[(i)+1] << 16 | b[(i)+2] << 8 | b[(i)+3])
#define np_u32(x) (x##_b[0] << 24 | x##_b[1] << 16 | x##_b[2] << 8 | x##_b[3])

static const char str_version[] = _VERSION;

static SOCKET net_tcp_socket;
static SOCKET net_udp_socket;
static struct sockaddr_in net_tcp_addr;
static struct sockaddr_in net_udp_addr;
static bool net_banned = true;
static u32  np_table;

static uint recvall(int sockfd, void *buf, size_t len)
{
    size_t i = 0;
    do
    {
        int size = recv(sockfd, (char *)buf + i, len, 0);
        e_socket(size, "recv() failed");
        if (size == 0)
        {
            puts(
                net_banned ? "You are banned from the server" : "Disconnected"
            );
            return true;
        }
        i   += size;
        len -= size;
    }
    while (len != 0);
    net_banned = false;
    return false;
}

static uint sendall(int sockfd, const void *buf, size_t len)
{
    size_t i = 0;
    do
    {
        int size = send(sockfd, (const char *)buf + i, len, 0);
        e_socket(size, "send() failed");
        i   += size;
        len -= size;
    }
    while (len != 0);
    return false;
}

static uint nff_write(const char *fn)
{
    FILE  *f;
    u8    *data;
    u8    *b;
    size_t size;
    f = fopen(fn, "rb");
    if (f == NULL)
    {
        eprint("could not read '%s'\n", fn);
    }
    fseek(f, 0, SEEK_END);
    size = ftell(f);
    fseek(f, 0, SEEK_SET);
    data = b = malloc(size);
    fread(data, 1, size, f);
    fclose(f);
    if (W(0x00) != 0x4E464600)
    {
        free(data);
        eprint("'%s' is not a valid NFF file\n", fn);
    }
    while (true)
    {
        uint addr  = W(0x04);
        uint start = W(0x08);
        uint end   = W(0x0C);
        b += 0x0C;
        if (addr == 0)
        {
            break;
        }
        if (addr == (u32)-1)
        {
            addr = np_table;
        }
        if (mem_write(addr, data+start, end-start))
        {
            free(data);
            return true;
        }
    }
    free(data);
    return false;
}

uint net_init(const char *addr, long int port, const char **argv, int argc)
{
    struct net_meta_t meta;
    struct addrinfo *result;
    int opt;
    int error;
#ifdef WIN32
    /* init WSA */
    WSADATA wsa_data;
    error = WSAStartup(WINSOCK_VERSION, &wsa_data);
    if (error != NO_ERROR)
    {
        eprint("WSAStartup() failed (%d)\n", error);
    }
#endif
    /* create tcp and udp sockets */
    net_tcp_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    e_socketfd(net_tcp_socket, "could not create TCP socket");
    net_udp_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    e_socketfd(net_udp_socket, "could not create UDP socket");
    /* make it so that if it crashes or something you can reconnect */
    opt = true;
    error = setsockopt(
        net_udp_socket, SOL_SOCKET, SO_REUSEADDR, (void *)&opt, sizeof(opt)
    );
    error = getaddrinfo(addr, NULL, NULL, &result);
    if (error != 0)
    {
        eprint("getaddrinfo() failed (%s)\n", gai_strerror(error));
    }
    e_socket(error, "setsockopt(SO_REUSEADDR) failed");
    net_tcp_addr.sin_family = net_udp_addr.sin_family = AF_INET;
    net_tcp_addr.sin_port   = net_udp_addr.sin_port   = htons(port);
    net_tcp_addr.sin_addr.s_addr =
        ((struct sockaddr_in *)result->ai_addr)->sin_addr.s_addr;
    net_udp_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    freeaddrinfo(result);
    error = connect(
        net_tcp_socket, (struct sockaddr *)&net_tcp_addr, sizeof(net_tcp_addr)
    );
    e_socket(error, "could not connect to server");
    error = bind(
        net_udp_socket, (struct sockaddr *)&net_udp_addr, sizeof(net_udp_addr)
    );
    e_socket(error, "bind() failed");
    net_udp_addr.sin_addr.s_addr = net_tcp_addr.sin_addr.s_addr;
    assert(recvall(net_tcp_socket, &meta, sizeof(meta)));
    meta.version[lenof(meta.version)-1] = 0;
    if (memcmp(meta.version, str_version, sizeof(str_version)) != 0)
    {
        eprint("server is version %s\n", meta.version);
    }
    assert(mem_write(NP_TABLE, meta.np_table_b, sizeof(meta.np_table_b)));
    np_table = np_u32(meta.np_table);
    while (argc > 0)
    {
        assert(nff_write(*argv));
        argv++;
        argc--;
    }
    puts("Connected");
    return false;
}

uint net_update(void)
{
    uint i;
    int error;
    struct np_t np;
    memset(np.sys, 0x00, sizeof(np.sys));
    assert(mem_read(
        np_table, &np, sizeof(np.udp)+sizeof(np.tcp)
    ));
    /* tcp write */
    if (np.np_tcp_id != 0)
    {
        assert(sendall(net_tcp_socket, np.tcp, sizeof(np.tcp)));
        np.np_tcp_id = 0;
        assert(mem_write(
            np_table + sizeof(np.udp), &np.np_tcp_id, sizeof(np.np_tcp_id)
        ));
    }
    /* udp write */
    error = sendto(
        net_udp_socket, (void *)np.udp, sizeof(np.udp), 0,
        (struct sockaddr *)&net_udp_addr, sizeof(net_udp_addr)
    );
    e_socket(error, "sendto() failed");
    for (i = 0; i < NP_LEN-1; i++)
    {
        /* check for nonblocking read */
        struct pollfd pollfds[2];
        pollfds[0].fd = net_tcp_socket;
        pollfds[0].events = POLLIN;
        pollfds[0].revents = 0;
        pollfds[1].fd = net_udp_socket;
        pollfds[1].events = POLLIN;
        pollfds[1].revents = 0;
        error = poll(pollfds, lenof(pollfds), 0);
        e_socket(error, "poll() failed");
        if ((pollfds[0].revents | pollfds[1].revents) == 0)
        {
            break;
        }
        /* tcp read */
        if (pollfds[0].revents & POLLIN)
        {
            u32 tcp_id;
            assert(recvall(net_tcp_socket, np.tcp, sizeof(np.tcp)));
            tcp_id = np_u32(np.np_tcp_id);
            switch (tcp_id)
            {
                /* ban */
                case (u32)-1:
                {
                    puts("You are banned from the server:");
                    puts((const char *)(&np.np_tcp_id + 1));
                    return true;
                    break;
                }
                /* write request */
                case (u32)-2:
                {
                    void *data;
                    u32 addr = np_u32(np.np_write_addr);
                    u32 size = np_u32(np.np_write_size);
                    if (addr < 0x80000000 || addr+size >= 0x80800000)
                    {
                        eprint("invalid write request\n");
                    }
                    data = malloc(size);
                    assert(recvall(net_tcp_socket, data, size));
                    assert(mem_write(addr, data, size));
                    free(data);
                    break;
                }
                /* peer write */
                default:
                {
                    assert(mem_write(
                        np_table + sizeof(np)*tcp_id + sizeof(np.udp),
                        np.tcp, sizeof(np.tcp)
                    ));
                    break;
                }
            }
        }
        /* udp read */
        if (pollfds[1].revents & POLLIN)
        {
            u32 udp_id;
            error = recvfrom(
                net_udp_socket, (void *)np.udp, sizeof(np.udp), 0, NULL, NULL
            );
            udp_id = np_u32(np.np_udp_id);
            e_socket(error, "recvfrom() failed");
            assert(mem_write(
                np_table + sizeof(np)*udp_id, np.udp, sizeof(np.udp)
            ));
        }
    }
#ifdef WIN32
    Sleep(33);
#else
    usleep(33333);
#endif
    return false;
}
