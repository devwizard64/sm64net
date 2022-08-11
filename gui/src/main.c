/******************************************************************************/
/*                  SM64Net - An Internet framework for SM64                  */
/*                    Copyright (C) 2019 - 2022  devwizard                    */
/*        This project is licensed under the terms of the GNU General         */
/*        Public License version 2.  See LICENSE for more information.        */
/******************************************************************************/

#include <stdio.h>
#include <windows.h>
#include <windowsx.h>

#include <commctrl.h>

#include <types.h>
#include <sm64net.h>

enum
{
    W_S_PROC,
    W_S_ADDR,
    W_S_PORT,
    W_S_NAME,
    W_S_COLOUR,
    W_E_PROC,
    W_E_ADDR,
    W_E_PORT,
    W_E_NAME,
    W_B_PUNCTUATION,
    W_B_MACRO,
    W_E_COLOUR,
    W_S_COLOURPREV,
    W_B_COLOUR,
    W_B_HELP,
    W_B_ABOUT,
    W_B_LAUNCH,
    W_MAX,
};

typedef struct wnd
{
    HWND hWnd;
    LPCSTR lpClassName;
    LPCSTR lpWindowName;
    DWORD dwStyle;
    int X;
    int Y;
    int nWidth;
    int nHeight;
}
WND;

#define MARGIN 12
#define BUTTON_H 26
#define W_MAIN_W 480
#define W_MAIN_H 360

#define W_S_PROC_W 96
#define W_S_PROC_H 18
#define W_S_ADDR_W 96
#define W_S_ADDR_H 18
#define W_S_PORT_W 96
#define W_S_PORT_H 18
#define W_S_NAME_W 96
#define W_S_NAME_H (13*10 + 22)
#define W_S_COLOUR_W 80
#define W_S_COLOUR_H 18
#define W_E_COLOUR_H W_S_COLOUR_H
#define W_E_PROC_W (W_MAIN_W - MARGIN*5/2 - W_S_PROC_W)
#define W_E_PROC_H W_S_PROC_H
#define W_E_ADDR_W (W_MAIN_W - MARGIN*5/2 - W_S_ADDR_W)
#define W_E_ADDR_H W_S_ADDR_H
#define W_E_PORT_W (W_MAIN_W - MARGIN*5/2 - W_S_PORT_W)
#define W_E_PORT_H W_S_PORT_H
#define W_E_NAME_W (W_MAIN_W - MARGIN*5/2 - W_S_NAME_W)
#define W_E_NAME_H W_S_NAME_H
#define W_B_PUNCTUATION_W 140
#define W_B_PUNCTUATION_H BUTTON_H
#define W_B_MACRO_W 160
#define W_B_MACRO_H BUTTON_H
#define W_S_COLOURPREV_W BUTTON_H
#define W_S_COLOURPREV_H BUTTON_H
#define W_B_COLOUR_W (BUTTON_H + 4)
#define W_B_COLOUR_H BUTTON_H
#define W_E_COLOUR_W \
    (W_MAIN_W - MARGIN*9/2 - W_S_COLOUR_W - W_S_COLOURPREV_W - W_B_COLOUR_W)
#define W_B_HELP_W 50
#define W_B_HELP_H BUTTON_H
#define W_B_ABOUT_W 100
#define W_B_ABOUT_H BUTTON_H
#define W_B_LAUNCH_W (W_MAIN_W - MARGIN*9/2 - W_B_HELP_W - W_B_ABOUT_W)
#define W_B_LAUNCH_H BUTTON_H

#define W_S_PROC_X MARGIN
#define W_S_PROC_Y (MARGIN - 3)
#define W_S_ADDR_X W_S_PROC_X
#define W_S_ADDR_Y (W_S_PROC_Y + 24)
#define W_S_PORT_X W_S_PROC_X
#define W_S_PORT_Y (W_S_ADDR_Y + 24)
#define W_S_NAME_X W_S_PROC_X
#define W_S_NAME_Y (W_S_PORT_Y + 24)
#define W_E_PROC_X (W_S_PROC_X + W_S_PROC_W)
#define W_E_PROC_Y (W_S_PROC_Y - 3)
#define W_E_ADDR_X (W_S_ADDR_X + W_S_ADDR_W)
#define W_E_ADDR_Y (W_S_ADDR_Y - 3)
#define W_E_PORT_X (W_S_PORT_X + W_S_PORT_W)
#define W_E_PORT_Y (W_S_PORT_Y - 3)
#define W_E_NAME_X (W_S_NAME_X + W_S_NAME_W)
#define W_E_NAME_Y (W_S_NAME_Y - 3)
#define W_B_PUNCTUATION_X (W_MAIN_W*1/4 - W_B_PUNCTUATION_W/2 + MARGIN*3/2)
#define W_B_PUNCTUATION_Y (W_S_NAME_Y + W_S_NAME_H + 3)
#define W_B_MACRO_X (W_MAIN_W*3/4 - W_B_MACRO_W/2 - MARGIN*3/2)
#define W_B_MACRO_Y W_B_PUNCTUATION_Y
#define W_S_COLOUR_X MARGIN
#define W_S_COLOUR_Y (W_B_PUNCTUATION_Y + W_B_PUNCTUATION_H + 6 + 3 + 4)
#define W_E_COLOUR_X (W_S_COLOUR_X + W_S_COLOUR_W)
#define W_E_COLOUR_Y (W_S_COLOUR_Y - 3)
#define W_S_COLOURPREV_X (W_E_COLOUR_X + W_E_COLOUR_W + MARGIN)
#define W_S_COLOURPREV_Y (W_S_COLOUR_Y - 4 - 4)
#define W_B_COLOUR_X (W_S_COLOURPREV_X + W_S_COLOURPREV_W + MARGIN)
#define W_B_COLOUR_Y W_S_COLOURPREV_Y
#define W_B_HELP_X MARGIN
#define W_B_HELP_Y (W_B_COLOUR_Y + W_B_COLOUR_H + MARGIN/2)
#define W_B_ABOUT_X (W_B_HELP_X + W_B_HELP_W + MARGIN)
#define W_B_ABOUT_Y W_B_HELP_Y
#define W_B_LAUNCH_X (W_B_ABOUT_X + W_B_ABOUT_W + MARGIN)
#define W_B_LAUNCH_Y W_B_HELP_Y

static WND wnd_table[] =
{
#define WND(w, class, name, style)                  \
{                                                   \
    NULL, class, name, WS_VISIBLE|WS_CHILD|(style), \
    w##_X, w##_Y, w##_W, w##_H                      \
}
    WND(W_S_PROC, WC_STATIC, "Emulator Name:", 0),
    WND(W_S_ADDR, WC_STATIC, "IP Address:", 0),
    WND(W_S_PORT, WC_STATIC, "Port:", 0),
    WND(W_S_NAME, WC_STATIC, "Username:", 0),
    WND(W_S_COLOUR, WC_STATIC, "Colour:", 0),
    WND(W_E_PROC, WC_EDIT, NULL, WS_BORDER|ES_AUTOHSCROLL),
    WND(W_E_ADDR, WC_EDIT, NULL, WS_BORDER|ES_AUTOHSCROLL),
    WND(W_E_PORT, WC_EDIT, NULL, WS_BORDER|ES_AUTOHSCROLL|ES_NUMBER),
    WND(
        W_E_NAME, WC_EDIT, NULL, WS_BORDER|WS_HSCROLL|WS_VSCROLL |
        ES_AUTOHSCROLL|ES_AUTOVSCROLL|ES_MULTILINE
    ),
    WND(W_B_PUNCTUATION, WC_BUTTON, "Punctuation List...", 0),
    WND(W_B_MACRO, WC_BUTTON, "Macro Character List...", 0),
    WND(W_E_COLOUR, WC_EDIT, NULL, WS_BORDER|ES_READONLY),
    WND(W_S_COLOURPREV, WC_STATIC, NULL, WS_BORDER),
    WND(W_B_COLOUR, WC_BUTTON, "...", 0),
    WND(W_B_HELP, WC_BUTTON, "Help", 0),
    WND(W_B_ABOUT, WC_BUTTON, "About SM64Net", 0),
    WND(W_B_LAUNCH, WC_BUTTON, "Launch SM64Net", BS_DEFPUSHBUTTON),
#undef WND
};

typedef struct msg_char
{
    u8 m;
    const char *a;
}
MSG_CHAR;

static const MSG_CHAR msg_char_table[] =
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

static void msg_to_ascii(char *dst, const u8 *src, size_t size)
{
    memset(dst, 0, size);
    if (size-- <= 0) return;
    while (*src != 0xFF && size > 0)
    {
        int i;
        for (i = 0; i < (int)lenof(msg_char_table); i++)
        {
            if (*src == msg_char_table[i].m)
            {
                const char *a = msg_char_table[i].a;
                size_t n = strlen(a);
                strncpy(dst, a, size);
                dst += n;
                size -= n;
                goto end;
            }
        }
        if      (              *src <=  9) *dst++ = '0' + *src- 0;
        else if (*src >= 10 && *src <= 35) *dst++ = 'A' + *src-10;
        else if (*src >= 36 && *src <= 61) *dst++ = 'a' + *src-36;
        size--;
    end:
        src++;
    }
}

static void ascii_to_msg(u8 *dst, const char *src, size_t size)
{
    memset(dst, ~0, size);
    if (size-- <= 0) return;
    while (*src != 0 && size > 0)
    {
        int i;
        for (i = 0; i < (int)lenof(msg_char_table); i++)
        {
            const char *a = msg_char_table[i].a;
            size_t n = strlen(a);
            if (size >= n && !strncmp(src, a, n))
            {
                *dst++ = msg_char_table[i].m;
                src += n;
                goto end;
            }
        }
        if      (*src >= '0' && *src <= '9') *dst++ =  0 + *src-'0';
        else if (*src >= 'A' && *src <= 'Z') *dst++ = 10 + *src-'A';
        else if (*src >= 'a' && *src <= 'z') *dst++ = 36 + *src-'a';
        src++;
    end:
        size--;
    }
}

typedef struct config
{
    /* 0x000 */ char proc[1024+1];
    /* 0x401 */ char addr[1024+1];
    /* 0x802 */ u16  port;
    /* 0x804 */ u32  colour;
    /* 0x808 */ u8   name[NP_NAME_LEN];
}   /* 0x828 */
CONFIG;

static CONFIG config;

static int cfg_load(void)
{
    int flag = FALSE;
    FILE *f;
    char buf[128+1];
    memset(&config, 0, sizeof(CONFIG));
    if ((f = fopen("config.bin", "rb")) != NULL)
    {
        fread(&config, 1, sizeof(CONFIG), f);
        fclose(f);
    }
    else
    {
        strncpy(config.proc, "retroarch.exe", sizeof(config.proc));
        memset(config.name, ~0, sizeof(config.name));
        config.colour = 0xFFFFFF;
        flag = TRUE;
    }
    if (config.port == 0) config.port = NET_PORT;
    SetWindowText(wnd_table[W_E_PROC].hWnd, config.proc);
    SetWindowText(wnd_table[W_E_ADDR].hWnd, config.addr);
    snprintf(buf, sizeof(buf), "%d", config.port);
    SetWindowText(wnd_table[W_E_PORT].hWnd, buf);
    config.name[sizeof(config.name)-1] = 0xFF;
    msg_to_ascii(buf, config.name, sizeof(buf));
    SetWindowText(wnd_table[W_E_NAME].hWnd, buf);
    snprintf(buf, sizeof(buf), "%06X", config.colour);
    SetWindowText(wnd_table[W_E_COLOUR].hWnd, buf);
    ShowWindow(wnd_table[W_S_COLOURPREV].hWnd, SW_HIDE);
    ShowWindow(wnd_table[W_S_COLOURPREV].hWnd, SW_SHOWNORMAL);
    return flag;
}

static int cfg_save(void)
{
    FILE *f;
    long port;
    char buf[128+1];
    GetWindowText(wnd_table[W_E_PROC].hWnd, config.proc, sizeof(config.proc));
    GetWindowText(wnd_table[W_E_ADDR].hWnd, config.addr, sizeof(config.addr));
    GetWindowText(wnd_table[W_E_PORT].hWnd, buf, sizeof(buf));
    port = strtol(buf, NULL, 10);
    config.port = port > 0 && port <= 65535 ? port : 0;
    GetWindowText(wnd_table[W_E_NAME].hWnd, buf, sizeof(buf));
    ascii_to_msg(config.name, buf, sizeof(config.name));
    if ((f = fopen("config.bin", "wb")) != NULL)
    {
        fwrite(&config, 1, sizeof(CONFIG), f);
        fclose(f);
        return FALSE;
    }
    return TRUE;
}

static int cfg_nff(void)
{
    FILE *f;
    NET_PL np;
    memset(&np, 0, sizeof(NET_PL));
    memcpy(np.np_name, config.name, sizeof(config.name));
    np.np_colour_b[0] = config.colour >> 16;
    np.np_colour_b[1] = config.colour >>  8;
    np.np_colour_b[2] = config.colour >>  0;
    np.np_colour_b[3] = 0xFF;
    if ((f = fopen("config.nff", "wb")) != NULL)
    {
#define W(x) (char)((x) >> 24), (char)((x) >> 16), (char)((x) >> 8), (char)(x)
        static const char header[] =
        {
            W(0x4E464600),
            W(-1), W(0x14), W(0x14+sizeof(NET_PL)),
            W(0),
        };
#undef W
        fwrite(header, 1, sizeof(header), f);
        fwrite(&np, 1, sizeof(NET_PL), f);
        fclose(f);
        return FALSE;
    }
    return TRUE;
}

static void wnd_save(HWND hWnd, int nff)
{
    if (cfg_save() || (nff && cfg_nff())) MessageBox(
        hWnd, "Could not save config file.", "Warning",
        MB_ICONWARNING|MB_OK
    );
}

static void wnd_punctuation(HWND hWnd)
{
    MessageBox(
        hWnd,
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
        MB_ICONINFORMATION|MB_OK
    );
}

static void wnd_macro(HWND hWnd)
{
    MessageBox(
        hWnd,
        "[^]\t(Up arrow)\n"
        "[v]\t(Down arrow)\n"
        "[<]\t(Left arrow)\n"
        "[>]\t(Right arrow)\n"
        "[A]\t(Bold A)\n"
        "[B]\t(Bold B)\n"
        "[C]\t(Bold C)\n"
        "[Z]\t(Bold Z)\n"
        "[R]\t(Bold R)\n"
        "[+]\t(Coin)\n"
        "[*]\t(Star)\n"
        "[x]\t(Multiply)\n"
        "[-]\t(Bullet)\n"
        "[.]\t(Star outline)",
        "Macro Character List",
        MB_ICONINFORMATION|MB_OK
    );
}

static void wnd_colour(HWND hWnd)
{
    static COLORREF rgbPalette[16];
    CHOOSECOLOR cc;
    ZeroMemory(&cc, sizeof(CHOOSECOLOR));
    cc.lStructSize = sizeof(CHOOSECOLOR);
    cc.hwndOwner = hWnd;
    cc.lpCustColors = (LPDWORD)rgbPalette;
    cc.rgbResult = RGB(
        config.colour >> 16 & 0xFF,
        config.colour >>  8 & 0xFF,
        config.colour >>  0 & 0xFF
    );
    cc.Flags = CC_FULLOPEN|CC_RGBINIT;
    if (ChooseColor(&cc))
    {
        char buf[6+1];
        config.colour =
            GetRValue(cc.rgbResult) << 16 |
            GetGValue(cc.rgbResult) <<  8 |
            GetBValue(cc.rgbResult) <<  0;
        snprintf(buf, sizeof(buf), "%06X", config.colour);
        SetWindowText(wnd_table[W_E_COLOUR].hWnd, buf);
        ShowWindow(wnd_table[W_S_COLOURPREV].hWnd, SW_HIDE);
        ShowWindow(wnd_table[W_S_COLOURPREV].hWnd, SW_SHOWNORMAL);
    }
    wnd_save(hWnd, TRUE);
}

static void wnd_help(HWND hWnd)
{
    MessageBox(
        hWnd,
        "See README.md for more information on how to use SM64Net.",
        "Help", MB_ICONINFORMATION|MB_OK
    );
}

static void wnd_about(HWND hWnd)
{
    MessageBox(
        hWnd,
        "SM64Net " VERSION "." REVISION "\n"
        "Copyright (C) 2019 - 2022  devwizard\n"
        "\n"
        "This project is licensed under the terms of the GNU General Public "
        "License version 2.  See LICENSE for more information.\n"
        "\n"
        "See README.md for more information on how to use SM64Net.",
        "About SM64Net", MB_ICONINFORMATION|MB_OK
    );
}

static void wnd_launch(HWND hWnd)
{
    char buf[2048+32];
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    if (config.proc[0] == 0)
    {
        MessageBox(
            hWnd, "The 'Emulator Name' field cannot be blank.",
            "Error", MB_ICONERROR|MB_OK
        );
        return;
    }
    if (config.addr[0] == 0)
    {
        MessageBox(
            hWnd, "The 'IP Address' field cannot be blank.",
            "Error", MB_ICONERROR|MB_OK
        );
        return;
    }
    if (config.port == 0)
    {
        MessageBox(
            hWnd, "The value in the 'Port' field is invalid.",
            "Error", MB_ICONERROR|MB_OK
        );
        return;
    }
    if (config.name[0] == 0xFF)
    {
        MessageBox(
            hWnd, "The 'Username' field cannot be blank.",
            "Error", MB_ICONERROR|MB_OK
        );
        return;
    }
    snprintf(
        buf, sizeof(buf), "cmd.exe /k sm64net.exe \"%s\" \"%s\" %u config.nff",
        config.proc, config.addr, config.port
    );
    ZeroMemory(&si, sizeof(STARTUPINFO));
    ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
    si.cb = sizeof(STARTUPINFO);
    if (!CreateProcess(NULL, buf, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
    {
        snprintf(
            buf, sizeof(buf), "SM64Net could not be launched (%lu).",
            GetLastError()
        );
        MessageBox(hWnd, buf, "Error", MB_ICONERROR|MB_OK);
        return;
    }
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

static void wnd_init(HWND hWnd)
{
    int i;
    RECT rect;
    NONCLIENTMETRICS ncm;
    HFONT hFont;
    GetWindowRect(hWnd, &rect);
    SetWindowPos(
        hWnd, HWND_TOP,
        (GetSystemMetrics(SM_CXSCREEN) + rect.left-rect.right )/2,
        (GetSystemMetrics(SM_CYSCREEN) + rect.top -rect.bottom)/2,
        0, 0, SWP_NOSIZE
    );
    ncm.cbSize = sizeof(NONCLIENTMETRICS);
    SystemParametersInfo(
        SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0
    );
    hFont = CreateFontIndirect(&ncm.lfMessageFont);
    SendMessage(hWnd, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
    for (i = 0; i < W_MAX; i++)
    {
        WND *w = &wnd_table[i];
        w->hWnd = CreateWindowEx(
            0, w->lpClassName, w->lpWindowName, w->dwStyle,
            w->X, w->Y, w->nWidth, w->nHeight, hWnd, NULL, NULL, NULL
        );
        SendMessage(w->hWnd, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
    }
    if (cfg_load())
    {
        wnd_about(hWnd);
        wnd_save(hWnd, TRUE);
    }
    SendMessage(
        wnd_table[W_E_PROC].hWnd, EM_SETLIMITTEXT, sizeof(config.proc)-1, 0
    );
    SendMessage(
        wnd_table[W_E_ADDR].hWnd, EM_SETLIMITTEXT, sizeof(config.addr)-1, 0
    );
    SendMessage(wnd_table[W_E_PORT].hWnd, EM_SETLIMITTEXT, 5, 0);
    SendMessage(wnd_table[W_E_NAME].hWnd, EM_SETLIMITTEXT, 128, 0);
}

static LRESULT CALLBACK wnd_proc(
    HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam
)
{
    static char lock;
    switch (uMsg)
    {
        case WM_CREATE:
        {
            lock = TRUE;
            wnd_init(hWnd);
            lock = FALSE;
            break;
        }
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            break;
        }
        case WM_COMMAND:
        {
            int i;
            for (i = 0; i < W_MAX; i++)
            {
                if ((HWND)lParam == wnd_table[i].hWnd) break;
            }
            switch (i)
            {
                case W_E_PROC:
                case W_E_ADDR:
                case W_E_PORT:
                case W_E_NAME:
                    if (HIWORD(wParam) == EN_CHANGE && !lock)
                    {
                        lock = TRUE;
                        wnd_save(hWnd, i == W_E_NAME);
                        lock = FALSE;
                    }
                    break;
                case W_B_PUNCTUATION:   wnd_punctuation(hWnd);  break;
                case W_B_MACRO:         wnd_macro(hWnd);        break;
                case W_B_COLOUR:        wnd_colour(hWnd);       break;
                case W_B_HELP:          wnd_help(hWnd);         break;
                case W_B_ABOUT:         wnd_about(hWnd);        break;
                case W_B_LAUNCH:        wnd_launch(hWnd);       break;
            }
            break;
        }
        case WM_CTLCOLORSTATIC:
        {
            if ((HWND)lParam == wnd_table[W_E_COLOUR].hWnd)
            {
                return (LRESULT)GetSysColorBrush(COLOR_WINDOW);
            }
            else if ((HWND)lParam == wnd_table[W_S_COLOURPREV].hWnd)
            {
                static HBRUSH hBrush = NULL;
                if (hBrush != NULL) DeleteObject(hBrush);
                hBrush = CreateSolidBrush(RGB(
                    config.colour >> 16 & 0xFF,
                    config.colour >>  8 & 0xFF,
                    config.colour >>  0 & 0xFF
                ));
                return (LRESULT)hBrush;
            }
            break;
        }
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(
    HINSTANCE hInstance,
    UNUSED HINSTANCE hPrevInstance,
    UNUSED LPSTR lpCmdLine,
    int nShowCmd
)
{
    INITCOMMONCONTROLSEX icex;
    WNDCLASS wc;
    HWND hWnd;
    MSG msg;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC  = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&icex);
    wc.style          = CS_HREDRAW|CS_VREDRAW;
    wc.lpfnWndProc    = wnd_proc;
    wc.cbClsExtra     = 0;
    wc.cbWndExtra     = 0;
    wc.hInstance      = hInstance;
    wc.hIcon          = LoadIcon(hInstance, IDI_APPLICATION);
    wc.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground  = GetSysColorBrush(COLOR_3DFACE);
    wc.lpszMenuName   = NULL;
    wc.lpszClassName  = "SM64NETGUICLASS";
    RegisterClass(&wc);
    hWnd = CreateWindow(
        wc.lpszClassName, "SM64Net GUI",
        WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX,
        0, 0, W_MAIN_W, W_MAIN_H, NULL, NULL, hInstance, NULL
    );
    ShowWindow(hWnd, nShowCmd);
    UpdateWindow(hWnd);
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}
