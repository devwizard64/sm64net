/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                    Copyright (C) 2019 - 2021  devwizard                    *
 *        This project is licensed under the terms of the GNU General         *
 *        Public License version 2.  See LICENSE for more information.        *
 ******************************************************************************/

#include <stdio.h>
#include <windows.h>
#include <windowsx.h>

#include <commctrl.h>

#include <types.h>

#include "window.h"
#include "config.h"

#define wnd(w, class, name, style)                                             \
{                                                                              \
    NULL, class, name, WS_VISIBLE | WS_CHILD | (style),                        \
    w##_X, w##_Y, w##_W, w##_H                                                 \
}
struct wnd_t wnd_table[] =
{
    wnd(W_S_PROC, WC_STATIC, "Emulator Name:", 0),
    wnd(W_S_ADDR, WC_STATIC, "IP Address:", 0),
    wnd(W_S_PORT, WC_STATIC, "Port:", 0),
    wnd(W_S_NAME, WC_STATIC, "Username:", 0),
    wnd(W_S_COLOUR, WC_STATIC, "Colour:", 0),
    wnd(W_E_PROC, WC_EDIT, NULL, WS_BORDER | ES_AUTOHSCROLL),
    wnd(W_E_ADDR, WC_EDIT, NULL, WS_BORDER | ES_AUTOHSCROLL),
    wnd(W_E_PORT, WC_EDIT, NULL, WS_BORDER | ES_AUTOHSCROLL | ES_NUMBER),
    wnd(
        W_E_NAME, WC_EDIT, NULL,
        WS_BORDER | WS_HSCROLL | WS_VSCROLL |
        ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE
    ),
    wnd(W_B_PUNCTUATION, WC_BUTTON, "Punctuation List...", 0),
    wnd(W_B_MACRO, WC_BUTTON, "Macro Character List...", 0),
    wnd(W_E_COLOUR, WC_EDIT, NULL, WS_BORDER | ES_READONLY),
    wnd(W_S_COLOURPREV, WC_STATIC, NULL, WS_BORDER),
    wnd(W_B_COLOUR, WC_BUTTON, "...", 0),
    wnd(W_B_HELP, WC_BUTTON, "Help", 0),
    wnd(W_B_ABOUT, WC_BUTTON, "About SM64Net", 0),
    wnd(W_B_LAUNCH, WC_BUTTON, "Launch SM64Net", BS_DEFPUSHBUTTON),
};

static COLORREF window_palette[0x10] = {0};
static HBRUSH window_brush = NULL;
static bool window_update = false;

static void window_error(HWND hwnd, const char *msg)
{
    MessageBox(hwnd, msg, "Error", MB_ICONERROR | MB_OK);
}

static void window_config_write(HWND hwnd, uint nff)
{
    if (config_write() || (nff && config_write_nff()))
    {
        MessageBox(
            hwnd, "Could not load config file.", "Warning",
            MB_ICONWARNING | MB_OK
        );
    }
}

static void window_punctuation(HWND hwnd)
{
    MessageBox(
        hwnd,
        "'\t(Apostrophe)\n"
        ".\t(Period)\n"
        ",\t(Comma)\n"
        "-\t(Hyphen)\n"
        "(\t(Open parenthesis)\n"
        ")(\t(Double parentheses)\n"
        ")\t(Closing parenthesis)\n"
        "<->\t(Double arrow)\n"
        "&\t(Ampersand)\n"
        ":\t(Colon)\n"
        "!\t(Exclamation Mark)\n"
        "%\t(Percent Sign)\n"
        "?\t(Question Mark)\n"
        "{\t(Opening Quote)\n"
        "}\t(Closing Quote)\n"
        "~\t(Tilde)\n",
        "Punctuation List",
        MB_ICONINFORMATION | MB_OK
    );
}

static void window_macro(HWND hwnd)
{
    MessageBox(
        hwnd,
        "Macro character list:\n"
        "[^]\t(Up arrow)\n"
        "[v]\t(Down arrow)\n"
        "[<]\t(Left arrow)\n"
        "[>]\t(Right arrow)\n"
        "[A]\t(Bold A)\n"
        "[B]\t(Bold B)\n"
        "[C]\t(Bold C)\n"
        "[Z]\t(Bold Z)\n"
        "[R]\t(Bold R)\n"
        "[coin]\t(Coin symbol)\n"
        "[star]\t(Star symbol)\n"
        "[x]\t(Multiply)\n"
        "[-]\t(Bullet)\n"
        "[nostar]\t(No Star symbol)",
        "Macro Character List",
        MB_ICONINFORMATION | MB_OK
    );
}

static void window_colour(HWND hwnd)
{
    CHOOSECOLOR cc;
    ZeroMemory(&cc, sizeof(cc));
    cc.lStructSize = sizeof(cc);
    cc.hwndOwner = hwnd;
    cc.lpCustColors = (LPDWORD)window_palette;
    cc.rgbResult = RGB(
        config.colour >> 16 & 0xFF,
        config.colour >>  8 & 0xFF,
        config.colour >>  0 & 0xFF
    );
    cc.Flags = CC_FULLOPEN | CC_RGBINIT;
    if (ChooseColor(&cc))
    {
        config.colour =
            GetRValue(cc.rgbResult) << 16 |
            GetGValue(cc.rgbResult) <<  8 |
            GetBValue(cc.rgbResult) <<  0;
    }
    window_config_write(hwnd, true);
}

static void window_help(HWND hwnd)
{
    MessageBox(
        hwnd,
        "See README.md for more information on how to use SM64Net.",
        "Help", MB_ICONINFORMATION | MB_OK
    );
}

static void window_about(HWND hwnd)
{
    MessageBox(
        hwnd,
        "SM64Net " _VERSION "." _REVISION "\n"
        "Copyright (C) 2019 - 2021  devwizard\n"
        "\n"
        "This project is licensed under the terms of the GNU General Public "
        "License version 2.  See LICENSE for more information.\n"
        "\n"
        "See README.md for more information on how to use SM64Net.",
        "About SM64Net", MB_ICONINFORMATION | MB_OK
    );
}

static void window_launch(HWND hwnd)
{
    char args[0x8000];
    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    uint success;
    if (config.proc[0] == 0)
    {
        window_error(hwnd, "The 'Emulator Name' field cannot be blank.");
        return;
    }
    if (config.addr[0] == 0)
    {
        window_error(hwnd, "The 'IP Address' field cannot be blank.");
        return;
    }
    if (config.port == 0)
    {
        window_error(hwnd, "The value in the 'Port' field is invalid.");
        return;
    }
    if (config.name[0] == 0xFF)
    {
        window_error(hwnd, "The 'Username' field cannot be blank.");
        return;
    }
    snprintf(
        args, sizeof(args), "sm64net.exe %s %s %d config.nff",
        config.proc, config.addr, config.port
    );
    ZeroMemory(&pi, sizeof(pi));
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    success = CreateProcess(
        NULL, args, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi
    );
    if (!success)
    {
        window_error(hwnd, "SM64Net could not be launched.");
        return;
    }
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

static void window_init(HWND hwnd)
{
    RECT rect;
    NONCLIENTMETRICS ncm;
    HFONT hfont;
    uint i;
    GetWindowRect(hwnd, &rect);
    SetWindowPos(
        hwnd, HWND_TOP,
        (GetSystemMetrics(SM_CXSCREEN) + rect.left - rect.right ) / 2,
        (GetSystemMetrics(SM_CYSCREEN) + rect.top  - rect.bottom) / 2,
        0, 0, SWP_NOSIZE
    );
    ncm.cbSize = sizeof(ncm);
    SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0);
    hfont = CreateFontIndirect(&ncm.lfMessageFont);
    SendMessage(hwnd, WM_SETFONT, (WPARAM)hfont, MAKELPARAM(TRUE, 0));
    for (i = 0; i < lenof(wnd_table); i++)
    {
        struct wnd_t *w = &wnd_table[i];
        w->hwnd = CreateWindowEx(
            0, w->class, w->name, w->style,
            w->x, w->y, w->w, w->h,
            hwnd, NULL, NULL, NULL
        );
        SendMessage(
            w->hwnd, WM_SETFONT, (WPARAM)hfont, MAKELPARAM(TRUE, 0)
        );
    }
    if (config_read())
    {
        window_about(hwnd);
        window_config_write(hwnd, true);
    }
    window_update = false;
    SendMessage(
        wnd_table[W_E_PROC].hwnd, EM_SETLIMITTEXT, lenof(config.proc)-1, 0
    );
    SendMessage(
        wnd_table[W_E_ADDR].hwnd, EM_SETLIMITTEXT, lenof(config.addr)-1, 0
    );
    SendMessage(wnd_table[W_E_PORT].hwnd, EM_SETLIMITTEXT, 5, 0);
    SendMessage(wnd_table[W_E_NAME].hwnd, EM_SETLIMITTEXT, 0xF8, 0);
    window_update = true;
}

LRESULT CALLBACK window_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg)
    {
        case WM_CREATE:
        {
            window_init(hwnd);
            break;
        }
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            break;
        }
        case WM_COMMAND:
        {
            uint i;
            for (i = 0; i < lenof(wnd_table); i++)
            {
                if ((HWND)lparam == wnd_table[i].hwnd)
                {
                    break;
                }
            }
            switch (i)
            {
                case W_E_PROC:
                case W_E_ADDR:
                case W_E_PORT:
                case W_E_NAME:
                    if (HIWORD(wparam) == EN_CHANGE && window_update)
                    {
                        window_update = false;
                        window_config_write(hwnd, i == W_E_NAME);
                        window_update = true;
                    }
                    break;
                case W_B_PUNCTUATION:
                    window_punctuation(hwnd);
                    break;
                case W_B_MACRO:
                    window_macro(hwnd);
                    break;
                case W_B_COLOUR:
                    window_colour(hwnd);
                    break;
                case W_B_HELP:
                    window_help(hwnd);
                    break;
                case W_B_ABOUT:
                    window_about(hwnd);
                    break;
                case W_B_LAUNCH:
                    window_launch(hwnd);
                    break;
            }
            break;
        }
        case WM_CTLCOLORSTATIC:
        {
            if ((HWND)lparam == wnd_table[W_E_COLOUR].hwnd)
            {
                return (LRESULT)GetSysColorBrush(COLOR_WINDOW);
            }
            else if ((HWND)lparam == wnd_table[W_S_COLOURPREV].hwnd)
            {
                if (window_brush != NULL)
                {
                    DeleteObject(window_brush);
                }
                window_brush = CreateSolidBrush(RGB(
                    config.colour >> 16 & 0xFF,
                    config.colour >>  8 & 0xFF,
                    config.colour >>  0 & 0xFF
                ));
                return (LRESULT)window_brush;
            }
            break;
        }
    }
    return DefWindowProc(hwnd, msg, wparam, lparam);
}
