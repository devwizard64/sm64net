/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                   Copyright (C) 2019, 2020  devwizard                      *
 *      This project is licensed under the GNU General Public License         *
 *      version 2.  See LICENSE for more information.                         *
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "types.h"

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

void menu_h_m(char *dst, const u8 *src, size_t size)
{
    do
    {
        sprintf(dst, "%02X", *src);
        dst  += 2;
        src  += 1;
        size -= 1;
    }
    while (size != 0 && *src != 0xFF);
}

void menu_a_m(char *dst, const u8 *src, ssize_t size)
{
    ZeroMemory(dst, size);
    size--;
    while (*src != 0xFF && size > 0)
    {
        unsigned int i;
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

void menu_m_a(u8 *dst, const char *src, ssize_t size)
{
    memset(dst, 0xFF, size);
    size--;
    while (*src != 0x00 && size > 0)
    {
        unsigned int i;
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
