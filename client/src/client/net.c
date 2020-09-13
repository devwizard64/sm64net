/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                   Copyright (C) 2019, 2020  devwizard                      *
 *      This project is licensed under the GNU General Public License         *
 *      version 2.  See LICENSE for more information.                         *
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#ifdef WIN32
#include <winsock2.h>
#ifndef POLLIN
#define POLLIN  0x0300
#endif
#ifndef WSAPOLLFD
typedef struct pollfd
{
    SOCKET      fd;
    SHORT       events;
    SHORT       revents;
}
WSAPOLLFD, *PWSAPOLLFD, FAR *LPWSAPOLLFD;
#endif
#ifndef WSAPoll
WINSOCK_API_LINKAGE int WSAAPI WSAPoll(
    _Inout_ LPWSAPOLLFD fdArray,
    _In_ ULONG fds,
    _In_ INT timeout
);
#endif
#include <ws2tcpip.h>
#include <windows.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#endif

#include "types.h"
#include "sm64net.h"

#include "assert.h"
#include "mem.h"

#define np_u32(x) (x##_0 << 24 | x##_1 << 16 | x##_2 << 8 | x##_3 << 0)

static int net_tcp_socket;
static int net_udp_socket;
static struct sockaddr_in net_tcp_addr;
static struct sockaddr_in net_udp_addr;
static bool net_banned = true;

static bool recvall(int sockfd, void *buf, size_t len)
{
    size_t i = 0;
    do
    {
        int size = recv(sockfd, (char *)buf + i, len, 0);
        assert_msg(size < 0, "recv() failed");
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

static bool sendall(int sockfd, const void *buf, size_t len)
{
    size_t i = 0;
    do
    {
        int size = send(sockfd, (const char *)buf + i, len, 0);
        assert_msg(size < 0, "send() failed");
        i   += size;
        len -= size;
    }
    while (len != 0);
    return false;
}

bool net_init(const char *addr, long int port, u32 colour, u8 *name)
{
    int error;
#ifdef WIN32
    /* init WSA */
    {
        WSADATA wsa_data;
        error = WSAStartup(MAKEWORD(2, 2), &wsa_data);
        if (error)
        {
            const char *str;
            switch (error)
            {
                case WSAEFAULT:             str = "WSAEFAULT";          break;
                case WSAEINPROGRESS:        str = "WSAEINPROGRESS";     break;
                case WSAEPROCLIM:           str = "WSAEPROCLIM";        break;
                case WSASYSNOTREADY:        str = "WSASYSNOTREADY";     break;
                case WSAVERNOTSUPPORTED:    str = "WSAVERNOTSUPPORTED"; break;
                default:                    str = NULL;                 break;
            }
            if (str != NULL)
            {
                fprintf(stderr, "error: WSAStartup() failed (%s)\n", str);
            }
            else
            {
                fprintf(stderr, "error: WSAStartup() failed (%d)\n", error);
            }
            return true;
        }
    }
#endif
    /* create tcp and udp sockets */
    net_tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
    assert_msg(net_tcp_socket < 0, "could not create TCP socket");
    net_udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    assert_msg(net_udp_socket < 0, "could not create UDP socket");
    /* make it so that if it crashes or something you can reconnect */
    {
        int opt = true;
        error = setsockopt(
            net_udp_socket, SOL_SOCKET, SO_REUSEADDR, (void *)&opt, sizeof(opt)
        );
        assert_msg(error < 0, "setsockopt(SO_REUSEADDR) failed");
    }
    /* is this necessary? docs don't say anything about it but examples do */
    memset(&net_tcp_addr, 0x00, sizeof(net_tcp_addr));
    memset(&net_udp_addr, 0x00, sizeof(net_udp_addr));
    net_tcp_addr.sin_family = AF_INET;
    net_tcp_addr.sin_port = htons(port);
    net_udp_addr.sin_family = AF_INET;
    net_udp_addr.sin_port = htons(port);
    net_udp_addr.sin_addr.s_addr = INADDR_ANY;
    {
        struct addrinfo *result;
        error = getaddrinfo(addr, NULL, NULL, &result);
        if (error != 0)
        {
            fprintf(
                stderr, "error: getaddrinfo() failed (%s)\n",
                gai_strerror(error)
            );
            return true;
        }
        net_tcp_addr.sin_addr.s_addr =
            ((struct sockaddr_in *)result->ai_addr)->sin_addr.s_addr;
        freeaddrinfo(result);
    }
    error = connect(
        net_tcp_socket, (struct sockaddr *)&net_tcp_addr, sizeof(net_tcp_addr)
    );
    assert_msg(error < 0, "could not connect to server");
    error = bind(
        net_udp_socket, (struct sockaddr *)&net_udp_addr, sizeof(net_udp_addr)
    );
    assert_msg(error < 0, "bind() failed");
    net_udp_addr.sin_addr.s_addr = net_tcp_addr.sin_addr.s_addr;
    {
        struct net_player_t np;
        memset(&np, 0x00, sizeof(np));
        memcpy(&np.np_name, name, sizeof(np.np_name)*NET_PLAYER_NAME_LEN);
        np.np_colour_0 = colour >> 16;
        np.np_colour_1 = colour >>  8;
        np.np_colour_2 = colour >>  0;
        np.np_colour_3 = 0xFF;
        assert(mem_write(sm64_g_net_players, &np, sizeof(np)));
    }
    puts("Connected");
    return false;
}

bool net_update(void)
{
    int error;
    struct net_player_t np;
    struct pollfd pollfds[2];
    memset(&np.sys, 0x00, sizeof(np.sys));
    assert(mem_read(sm64_g_net_players, &np, sizeof(np.udp) + sizeof(np.tcp)));
    /* tcp write */
    if (np.np_tcp_id != 0)
    {
        assert(sendall(net_tcp_socket, &np.tcp, sizeof(np.tcp)));
        np.np_tcp_id = 0;
        assert(mem_write(
            sm64_g_net_players + sizeof(np.udp), &np.np_tcp_id,
            sizeof(np.np_tcp_id)
        ));
    }
    /* udp write */
    error = sendto(
        net_udp_socket, (void *)&np.udp, sizeof(np.udp), 0,
        (struct sockaddr *)&net_udp_addr, sizeof(net_udp_addr)
    );
    assert_msg(error < 0, "sendto() failed");
    /* check for nonblocking read */
    pollfds[0].fd = net_tcp_socket;
    pollfds[0].events = POLLIN;
    pollfds[0].revents = 0;
    pollfds[1].fd = net_udp_socket;
    pollfds[1].events = POLLIN;
    pollfds[1].revents = 0;
#ifdef WIN32
    error = WSAPoll(pollfds, 2, 0);
#else
    error = poll(pollfds, 2, 0);
#endif
    assert_msg(error < 0, "poll() failed");
    /* tcp read */
    if (pollfds[0].revents & POLLIN)
    {
        u32 tcp_id;
        assert(recvall(net_tcp_socket, &np.tcp, sizeof(np.tcp)));
        tcp_id = np_u32(np.np_tcp_id);
        /* write request */
        if (tcp_id == 0x00000000)
        {
            void *data;
            u32 addr = np_u32(np.np_write_addr);
            u32 size = np_u32(np.np_write_size);
            if (addr < 0x80000000 || addr+size >= 0x80800000)
            {
                fprintf(stderr, "error: invalid write request\n");
                return true;
            }
            data = malloc(size);
            assert(recvall(net_tcp_socket, data, size));
            assert(mem_write(addr, data, size));
            free(data);
        }
        /* peer write */
        else if (tcp_id < NET_PLAYER_LEN)
        {
            assert(mem_write(
                sm64_g_net_players + sizeof(np)*tcp_id + sizeof(np.udp),
                &np.tcp, sizeof(np.tcp)
            ));
        }
        else
        {
            puts("You are banned from the server:");
            puts((const char *)(&np.np_tcp_id + 1));
            return true;
        }
    }
    /* udp read */
    if (pollfds[1].revents & POLLIN)
    {
        u32 udp_id;
        error = recvfrom(
            net_udp_socket, (void *)&np.udp, sizeof(np.udp), 0, NULL, NULL
        );
        udp_id = np_u32(np.np_udp_id);
        assert_msg(error < 0, "recvfrom() failed");
        assert(mem_write(
            sm64_g_net_players + sizeof(np)*udp_id, &np.udp, sizeof(np.udp)
        ));
    }
#ifdef WIN32
    Sleep(33);
#else
    /* supposedly i am not supposed to do this, but this is convenient */
    usleep(33333);
#endif
    return false;
}
