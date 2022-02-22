/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                    Copyright (C) 2019 - 2022  devwizard                    *
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

#define check(c, msg)  \
    {if (c) eprint(msg " (%d)\n", WSAGetLastError());}
#define check_sockfd(e, msg)    check((e) == INVALID_SOCKET, msg)
#define check_socket(e, msg)    check((e) == SOCKET_ERROR, msg)

#else

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>

#define check(c, msg)   \
    {if (c) eprint(msg " (%s)\n", strerror(errno));}
#define check_sockfd(e, msg)    check((e) < 0, msg)
#define check_socket(e, msg)    check((e) < 0, msg)

typedef int SOCKET;

#endif

#include <types.h>
#include <sm64net.h>

#include "mem.h"

#define W(i) (b[(i)+0] << 24 | b[(i)+1] << 16 | b[(i)+2] << 8 | b[(i)+3])
#define np_u32(x) (x##_b[0] << 24 | x##_b[1] << 16 | x##_b[2] << 8 | x##_b[3])

static const char str_version[] = VERSION;

static SOCKET s_tcp;
static SOCKET s_udp;
static struct sockaddr_in tcp_addr;
static struct sockaddr_in udp_addr;
static PTR np_table;

static void recvall(int sockfd, void *buf, size_t len)
{
    static u8 banned = true;
    size_t i = 0;
    do
    {
        int n = recv(sockfd, (char *)buf+i, len, 0);
        check_socket(n, "recv() failed");
        if (n == 0)
        {
            puts(banned ? "You are banned from the server." : "Disconnected.");
            exit(EXIT_SUCCESS);
        }
        i   += n;
        len -= n;
    }
    while (len > 0);
    banned = false;
}

static void sendall(int sockfd, const void *buf, size_t len)
{
    size_t i = 0;
    do
    {
        int n = send(sockfd, (const char *)buf+i, len, 0);
        check_socket(n, "send() failed");
        i   += n;
        len -= n;
    }
    while (len > 0);
}

static void nff_write(const char *path)
{
    FILE *f;
    u8 *data;
    u8 *b;
    size_t size;
    if ((f = fopen(path, "rb")) == NULL) eprint("could not read '%s'\n", path);
    fseek(f, 0, SEEK_END);
    size = ftell(f);
    fseek(f, 0, SEEK_SET);
    data = b = malloc(size);
    fread(data, 1, size, f);
    fclose(f);
    if (W(0x00) != 0x4E464600) eprint("'%s' is not a valid NFF file\n", path);
    while (true)
    {
        PTR addr  = W(0x04);
        PTR start = W(0x08);
        PTR end   = W(0x0C);
        b += 0x0C;
        if (addr == 0) break;
        if (addr == (u32)-1) addr = np_table;
        mem_write(addr, data+start, end-start);
    }
    free(data);
}

void net_init(
    const char *proc, const char *addr, int port,
    const char **argv, int argc
)
{
    NET_META meta;
    struct addrinfo *result;
    int opt;
    int error;
#ifdef WIN32
    /* init WSA */
    WSADATA wsa_data;
    if ((error = WSAStartup(WINSOCK_VERSION, &wsa_data)) != NO_ERROR)
    {
        eprint("WSAStartup() failed (%d)\n", error);
    }
#endif
    /* create tcp and udp sockets */
    s_tcp = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    check_sockfd(s_tcp, "could not create TCP socket");
    s_udp = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    check_sockfd(s_udp, "could not create UDP socket");
    /* make it so that if it crashes or something you can reconnect */
    opt = true;
    check_socket(setsockopt(
        s_udp, SOL_SOCKET, SO_REUSEADDR, (void *)&opt, sizeof(opt)
    ), "setsockopt(SO_REUSEADDR) failed");
    if ((error = getaddrinfo(addr, NULL, NULL, &result)))
    {
        eprint("getaddrinfo() failed (%s)\n", gai_strerror(error));
    }
    tcp_addr.sin_family = udp_addr.sin_family = AF_INET;
    tcp_addr.sin_port   = udp_addr.sin_port   = htons(port);
    tcp_addr.sin_addr.s_addr =
        ((struct sockaddr_in *)result->ai_addr)->sin_addr.s_addr;
    udp_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    freeaddrinfo(result);
    check_socket(connect(
        s_tcp, (struct sockaddr *)&tcp_addr, sizeof(tcp_addr)
    ), "could not connect to server");
    check_socket(bind(
        s_udp, (struct sockaddr *)&udp_addr, sizeof(udp_addr)
    ), "bind() failed");
    udp_addr.sin_addr.s_addr = tcp_addr.sin_addr.s_addr;
    recvall(s_tcp, &meta, sizeof(meta));
    meta.version[lenof(meta.version)-1] = 0;
    if (memcmp(meta.version, str_version, sizeof(str_version)) != 0)
    {
        eprint("server is version %s\n", meta.version);
    }
    mem_init(proc, np_u32(meta.mem_addr));
    mem_write(NP_TABLE, meta.np_table_b, sizeof(meta.np_table_b));
    np_table = np_u32(meta.np_table);
    while (argc-- > 0) nff_write(*argv++);
    puts("Connected.");
}

void net_update(void)
{
    uint i;
    NET_PL np;
    memset(np.sys, 0x00, sizeof(np.sys));
    mem_read(np_table, &np, sizeof(np.udp)+sizeof(np.tcp));
    /* tcp write */
    if (np.np_tcp_id != 0)
    {
        sendall(s_tcp, np.tcp, sizeof(np.tcp));
        np.np_tcp_id = 0;
        mem_write(np_table+sizeof(np.udp), &np.np_tcp_id, sizeof(np.np_tcp_id));
    }
    /* udp write */
    check_socket(sendto(
        s_udp, (void *)np.udp, sizeof(np.udp), 0,
        (struct sockaddr *)&udp_addr, sizeof(udp_addr)
    ), "sendto() failed");
    for (i = 0; i < NP_LEN-1; i++)
    {
        /* check for nonblocking read */
        struct pollfd pollfds[2];
        pollfds[0].fd = s_tcp;
        pollfds[0].events = POLLIN;
        pollfds[0].revents = 0;
        pollfds[1].fd = s_udp;
        pollfds[1].events = POLLIN;
        pollfds[1].revents = 0;
        check_socket(poll(pollfds, lenof(pollfds), 0), "poll() failed");
        if ((pollfds[0].revents | pollfds[1].revents) == 0) break;
        /* tcp read */
        if (pollfds[0].revents & POLLIN)
        {
            u32 tcp_id;
            recvall(s_tcp, np.tcp, sizeof(np.tcp));
            tcp_id = np_u32(np.np_tcp_id);
            switch (tcp_id)
            {
                /* ban */
                case (u32)-1:
                {
                    puts("You are banned from the server:");
                    puts((const char *)(&np.np_tcp_id + 1));
                    exit(EXIT_SUCCESS);
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
                    recvall(s_tcp, data, size);
                    mem_write(addr, data, size);
                    free(data);
                    break;
                }
                /* peer write */
                default:
                {
                    mem_write(
                        np_table+sizeof(np)*tcp_id+sizeof(np.udp),
                        np.tcp, sizeof(np.tcp)
                    );
                    break;
                }
            }
        }
        /* udp read */
        if (pollfds[1].revents & POLLIN)
        {
            u32 udp_id;
            check_socket(recvfrom(
                s_udp, (void *)np.udp, sizeof(np.udp), 0, NULL, NULL
            ), "recvfrom() failed");
            udp_id = np_u32(np.np_udp_id);
            mem_write(np_table+sizeof(np)*udp_id, np.udp, sizeof(np.udp));
        }
    }
    usleep(33333);
}
