/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                    Copyright (C) 2019 - 2022  devwizard                    *
 *        This project is licensed under the terms of the GNU General         *
 *        Public License version 2.  See LICENSE for more information.        *
 ******************************************************************************/

#include <stdio.h>
#include <windows.h>

#include <types.h>
#include <sm64net.h>

#include "window.h"
#include "config.h"

typedef struct msg_char
{
    u8 m;
    const char *a;
}
MSG_CHAR;

static MSG_CHAR msg_char_table[] =
{
    {0x50, "[^]"},
    {0x51, "[v]"},
    {0x52, "[<]"},
    {0x53, "[>]"},
    {0x54, "[A]"},
    {0x55, "[B]"},
    {0x56, "[C]"},
    {0x57, "[Z]"},
    {0x58, "[R]"},
    {0xD1, "the"},
    {0xD2, "you"},
    {0xE4, "<->"},
    {0xF8, "..."},
    {0xF9, "[+]"},
    {0xFA, "[*]"},
    {0xFB, "[x]"},
    {0xFC, "[-]"},
    {0xFD, "[.]"},
    {0xD0, "  " },
    {0xE2, ")(" },
    {0x3E, "'"  },
    {0x3F, "."  },
    {0x6F, ","  },
    {0x9E, " "  },
    {0x9F, "-"  },
    {0xE1, "("  },
    {0xE3, ")"  },
    {0xE5, "&"  },
    {0xE6, ":"  },
    {0xF2, "!"  },
    {0xF3, "%"  },
    {0xF4, "?"  },
    {0xF5, "{"  },
    {0xF6, "}"  },
    {0xF7, "~"  },
    {0xFE, "\n" },
};

#define W(x) (u8)((x) >> 24), (u8)((x) >> 16), (u8)((x) >> 8), (u8)(x)
static const u8 nff_header[] =
{
    W(0x4E464600),
    W(0xFFFFFFFF), W(0x00000014), W(0x00000014 + sizeof(NET_PL)),
    W(0x00000000),
};
#undef W

CONFIG config;

static void msg_to_ascii(char *dst, const u8 *src, ssize_t size)
{
    ZeroMemory(dst, size);
    size--;
    while (*src != 0xFF && size > 0)
    {
        uint i;
        for (i = 0; i < lenof(msg_char_table); i++)
        {
            if (*src == msg_char_table[i].m)
            {
                const char *a = msg_char_table[i].a;
                ssize_t s = strlen(a);
                strncpy(dst, a, size);
                dst += s;
                size -= s;
                goto end;
            }
        }
        if      (                *src <= 0x09) *dst++ = '0' + *src-0x00;
        else if (*src >= 0x0A && *src <= 0x23) *dst++ = 'A' + *src-0x0A;
        else if (*src >= 0x24 && *src <= 0x3D) *dst++ = 'a' + *src-0x24;
        size--;
    end:
        src++;
    }
}

static void ascii_to_msg(u8 *dst, const char *src, ssize_t size)
{
    memset(dst, 0xFF, size);
    size--;
    while (*src != 0 && size > 0)
    {
        uint i;
        for (i = 0; i < lenof(msg_char_table); i++)
        {
            const char *a = msg_char_table[i].a;
            ssize_t s = strlen(a);
            if (size >= s && strncmp(src, a, s) == 0)
            {
                *dst++ = msg_char_table[i].m;
                src += s;
                goto end;
            }
        }
        if      (*src >= '0' && *src <= '9') *dst++ = 0x00 + *src-'0';
        else if (*src >= 'A' && *src <= 'Z') *dst++ = 0x0A + *src-'A';
        else if (*src >= 'a' && *src <= 'z') *dst++ = 0x24 + *src-'a';
        src++;
    end:
        size--;
    }
}

static void config_update_port(void)
{
    char buf[16];
    snprintf(buf, sizeof(buf), "%d", config.port);
    SetWindowText(wnd_table[W_E_PORT].hwnd, buf);
}

static void config_update_colour(void)
{
    char buf[16];
    snprintf(buf, sizeof(buf), "%06X", config.colour);
    SetWindowText(wnd_table[W_E_COLOUR].hwnd, buf);
    ShowWindow(wnd_table[W_S_COLOURPREV].hwnd, SW_HIDE);
    ShowWindow(wnd_table[W_S_COLOURPREV].hwnd, SW_SHOWNORMAL);
}

uint config_read(void)
{
    char buf[256];
    FILE *f;
    uint new;
    ZeroMemory(&config, sizeof(config));
    if ((f = fopen("config.bin", "rb")) != NULL)
    {
        fread(&config, 1, sizeof(config), f);
        fclose(f);
        new = false;
    }
    else
    {
        strncpy(config.proc, "Project64.exe", sizeof(config.proc));
        config.port = NET_PORT;
        memset(config.name, 0xFF, sizeof(config.name));
        config.colour = 0xFFFFFF;
        new = true;
    }
    SetWindowText(wnd_table[W_E_PROC].hwnd, config.proc);
    SetWindowText(wnd_table[W_E_ADDR].hwnd, config.addr);
    config_update_port();
    config.name[lenof(config.name)-1] = 0xFF;
    msg_to_ascii(buf, config.name, sizeof(buf));
    SetWindowText(wnd_table[W_E_NAME].hwnd, buf);
    config_update_colour();
    return new;
}

uint config_write(void)
{
    FILE *f;
    long int port;
    char edit_port[6];
    char edit_name[0xF9];
    GetWindowText(wnd_table[W_E_PROC].hwnd, config.proc, sizeof(config.proc));
    GetWindowText(wnd_table[W_E_ADDR].hwnd, config.addr, sizeof(config.addr));
    GetWindowText(wnd_table[W_E_PORT].hwnd, edit_port, sizeof(edit_port));
    GetWindowText(wnd_table[W_E_NAME].hwnd, edit_name, sizeof(edit_name));
    port = strtol(edit_port, NULL, 10);
    if (port > 0 && port <= 65535)
    {
        config.port = port;
    }
    else
    {
        if (strlen(edit_port) == 0) config.port = 0;
        config_update_port();
    }
    config_update_colour();
    ascii_to_msg(config.name, edit_name, sizeof(config.name));
    if ((f = fopen("config.bin", "wb")) != NULL)
    {
        fwrite(&config, 1, sizeof(config), f);
        fclose(f);
        return false;
    }
    return true;
}

uint config_write_nff(void)
{
    NET_PL np;
    FILE *f;
    memset(&np, 0x00, sizeof(np));
    memcpy(np.np_name, config.name, sizeof(config.name));
    np.np_colour_b[0] = config.colour >> 16;
    np.np_colour_b[1] = config.colour >>  8;
    np.np_colour_b[2] = config.colour >>  0;
    np.np_colour_b[3] = 0xFF;
    if ((f = fopen("config.nff", "wb")) != NULL)
    {
        fwrite(nff_header, 1, sizeof(nff_header), f);
        fwrite(&np, 1, sizeof(np), f);
        fclose(f);
        return false;
    }
    return true;
}
