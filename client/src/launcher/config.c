/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                   Copyright (C) 2019, 2020  devwizard                      *
 *      This project is licensed under the GNU General Public License         *
 *      version 2.  See LICENSE for more information.                         *
 ******************************************************************************/

#include <stdio.h>
#include <windows.h>

#include "types.h"
#include "sm64net.h"

#include "window.h"
#include "config.h"
#include "menu.h"

struct config_t g_config;

static void config_update_port(void)
{
    char buf[0x10];
    snprintf(buf, sizeof(buf), "%d", g_config.port);
    SetWindowText(g_w_table[W_E_PORT].hwnd, buf);
}

static void config_update_colour(void)
{
    char buf[0x10];
    snprintf(buf, sizeof(buf), "%06X", g_config.colour);
    SetWindowText(g_w_table[W_E_COLOUR].hwnd, buf);
    ShowWindow(g_w_table[W_S_COLOURPREV].hwnd, SW_HIDE);
    ShowWindow(g_w_table[W_S_COLOURPREV].hwnd, SW_SHOWNORMAL);
}

int config_read(void)
{
    char buf[0x100];
    FILE *f;
    bool new;
    ZeroMemory(&g_config, sizeof(g_config));
    f = fopen("config.bin", "rb");
    if (f == NULL)
    {
        strncpy(g_config.proc, "Project64.exe", sizeof(g_config.proc));
        g_config.port = NET_PORT;
        memset(g_config.name, 0xFF, sizeof(g_config.name));
        g_config.colour = 0xFFFFFF;
        new = true;
    }
    else
    {
        fread(&g_config, 1, sizeof(g_config), f);
        fclose(f);
        new = false;
    }
    SetWindowText(g_w_table[W_E_PROC].hwnd, g_config.proc);
    SetWindowText(g_w_table[W_E_ADDR].hwnd, g_config.addr);
    config_update_port();
    g_config.name[lenof(g_config.name)-1] = 0xFF;
    menu_a_m(buf, g_config.name, sizeof(buf));
    SetWindowText(g_w_table[W_E_NAME].hwnd, buf);
    config_update_colour();
    return new;
}

int config_write(void)
{
    FILE *f;
    long int port;
    char edit_port[6];
    char edit_name[0xF9];
    GetWindowText(
        g_w_table[W_E_PROC].hwnd, g_config.proc, sizeof(g_config.proc)
    );
    GetWindowText(
        g_w_table[W_E_ADDR].hwnd, g_config.addr, sizeof(g_config.addr)
    );
    GetWindowText(g_w_table[W_E_PORT].hwnd, edit_port, sizeof(edit_port));
    GetWindowText(g_w_table[W_E_NAME].hwnd, edit_name, sizeof(edit_name));
    port = strtol(edit_port, NULL, 10);
    if (port > 0 && port < 0x10000)
    {
        g_config.port = port;
    }
    else
    {
        if (strlen(edit_port) == 0)
        {
            g_config.port = 0;
        }
        config_update_port();
    }
    config_update_colour();
    menu_m_a(g_config.name, edit_name, sizeof(g_config.name));
    f = fopen("config.bin", "wb");
    if (f == NULL)
    {
        return true;
    }
    fwrite(&g_config, 1, sizeof(g_config), f);
    fclose(f);
    return false;
}
