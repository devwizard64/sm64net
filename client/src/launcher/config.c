/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                    Copyright (C) 2019 - 2021  devwizard                    *
 *        This project is licensed under the terms of the GNU General         *
 *        Public License version 2.  See LICENSE for more information.        *
 ******************************************************************************/

#include <stdio.h>
#include <windows.h>

#include <types.h>
#include <sm64net.h>

#include "window.h"
#include "config.h"

struct menu_char_t
{
    u8 m;
    const char *a;
};

static struct menu_char_t menu_char_table[] =
{
    {0xFD, "[nostar]"},
    {0xF9, "[coin]"  },
    {0xFA, "[star]"  },
    {0x50, "[^]"     },
    {0x51, "[v]"     },
    {0x52, "[<]"     },
    {0x53, "[>]"     },
    {0x54, "[A]"     },
    {0x55, "[B]"     },
    {0x56, "[C]"     },
    {0x57, "[Z]"     },
    {0x58, "[R]"     },
    {0xD1, "the"     },
    {0xD2, "you"     },
    {0xE4, "<->"     },
    {0xF8, "..."     },
    {0xFB, "[x]"     },
    {0xFC, "[-]"     },
    {0xD0, "  "      },
    {0xE2, ")("      },
    {0x3E, "'"       },
    {0x3F, "."       },
    {0x6F, ","       },
    {0x9E, " "       },
    {0x9F, "-"       },
    {0xE1, "("       },
    {0xE3, ")"       },
    {0xE5, "&"       },
    {0xE6, ":"       },
    {0xF2, "!"       },
    {0xF3, "%"       },
    {0xF4, "?"       },
    {0xF5, "{"       },
    {0xF6, "}"       },
    {0xF7, "~"       },
    {0xFE, "\n"      },
};

#define W(x) (u8)((x) >> 24), (u8)((x) >> 16), (u8)((x) >> 8), (u8)(x)
static const u8 nff_header[] =
{
    W(0x4E464600),
    W(np_table),
    W(0x00000014),
    W(0x00000014 + sizeof(struct np_t)),
    W(0x00000000),
};
#undef W

struct config_t config;

static void menu_a_m(char *dst, const u8 *src, ssize_t size)
{
    ZeroMemory(dst, size);
    size--;
    while (*src != 0xFF && size > 0)
    {
        uint i;
        for (i = 0; i < lenof(menu_char_table); i++)
        {
            if (*src == menu_char_table[i].m)
            {
                const char *a = menu_char_table[i].a;
                ssize_t s = strlen(a);
                strncpy(dst, a, size);
                dst += s;
                size -= s;
                goto end;
            }
        }
        if (*src <= 0x09)
        {
            *dst++ = *src + ('0' - 0x00);
        }
        else if (*src >= 0x0A && *src <= 0x23)
        {
            *dst++ = *src + ('A' - 0x0A);
        }
        else if (*src >= 0x24 && *src <= 0x3D)
        {
            *dst++ = *src + ('a' - 0x24);
        }
        size--;
    end:
        src++;
    }
}

static void menu_m_a(u8 *dst, const char *src, ssize_t size)
{
    memset(dst, 0xFF, size);
    size--;
    while (*src != 0x00 && size > 0)
    {
        uint i;
        for (i = 0; i < lenof(menu_char_table); i++)
        {
            const char *a = menu_char_table[i].a;
            ssize_t s = strlen(a);
            if (size >= s && strncmp(src, a, s) == 0)
            {
                *dst++ = menu_char_table[i].m;
                src += s;
                goto end;
            }
        }
        if (*src >= '0' && *src <= '9')
        {
            *dst++ = *src - ('0' - 0x00);
        }
        else if (*src >= 'A' && *src <= 'Z')
        {
            *dst++ = *src - ('A' - 0x0A);
        }
        else if (*src >= 'a' && *src <= 'z')
        {
            *dst++ = *src - ('a' - 0x24);
        }
        src++;
    end:
        size--;
    }
}

static void config_update_port(void)
{
    char buf[0x10];
    snprintf(buf, sizeof(buf), "%d", config.port);
    SetWindowText(wnd_table[W_E_PORT].hwnd, buf);
}

static void config_update_colour(void)
{
    char buf[0x10];
    snprintf(buf, sizeof(buf), "%06X", config.colour);
    SetWindowText(wnd_table[W_E_COLOUR].hwnd, buf);
    ShowWindow(wnd_table[W_S_COLOURPREV].hwnd, SW_HIDE);
    ShowWindow(wnd_table[W_S_COLOURPREV].hwnd, SW_SHOWNORMAL);
}

uint config_read(void)
{
    char buf[0x100];
    FILE *f;
    uint new;
    ZeroMemory(&config, sizeof(config));
    f = fopen("config.bin", "rb");
    if (f == NULL)
    {
        strncpy(config.proc, "Project64.exe", sizeof(config.proc));
        config.port = NET_PORT;
        memset(config.name, 0xFF, sizeof(config.name));
        config.colour = 0xFFFFFF;
        new = true;
    }
    else
    {
        fread(&config, 1, sizeof(config), f);
        fclose(f);
        new = false;
    }
    SetWindowText(wnd_table[W_E_PROC].hwnd, config.proc);
    SetWindowText(wnd_table[W_E_ADDR].hwnd, config.addr);
    config_update_port();
    config.name[lenof(config.name)-1] = 0xFF;
    menu_a_m(buf, config.name, sizeof(buf));
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
    GetWindowText(
        wnd_table[W_E_PROC].hwnd, config.proc, sizeof(config.proc)
    );
    GetWindowText(
        wnd_table[W_E_ADDR].hwnd, config.addr, sizeof(config.addr)
    );
    GetWindowText(wnd_table[W_E_PORT].hwnd, edit_port, sizeof(edit_port));
    GetWindowText(wnd_table[W_E_NAME].hwnd, edit_name, sizeof(edit_name));
    port = strtol(edit_port, NULL, 10);
    if (port > 0 && port < 0x10000)
    {
        config.port = port;
    }
    else
    {
        if (strlen(edit_port) == 0)
        {
            config.port = 0;
        }
        config_update_port();
    }
    config_update_colour();
    menu_m_a(config.name, edit_name, sizeof(config.name));
    f = fopen("config.bin", "wb");
    if (f == NULL)
    {
        return true;
    }
    fwrite(&config, 1, sizeof(config), f);
    fclose(f);
    return false;
}

uint config_write_nff(void)
{
    struct np_t np;
    FILE *f;
    memset(&np, 0x00, sizeof(np));
    memcpy(&np.np_name, config.name, sizeof(config.name));
    np.np_colour_b[0] = config.colour >> 16;
    np.np_colour_b[1] = config.colour >>  8;
    np.np_colour_b[2] = config.colour >>  0;
    np.np_colour_b[3] = 0xFF;
    f = fopen("config.nff", "wb");
    if (f == NULL)
    {
        return true;
    }
    fwrite(nff_header, 1, sizeof(nff_header), f);
    fwrite(&np, 1, sizeof(np), f);
    fclose(f);
    return false;
}
