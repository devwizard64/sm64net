/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                    Copyright (C) 2019 - 2021  devwizard                    *
 *        This project is licensed under the terms of the GNU General         *
 *        Public License version 2.  See LICENSE for more information.        *
 ******************************************************************************/

#include <windows.h>

#include <commctrl.h>

#include <types.h>

#include "window.h"

int WINAPI WinMain(
    HINSTANCE hInstance, unused HINSTANCE hPrevInstance, unused LPSTR lpCmdLine,
    int nShowCmd
)
{
    INITCOMMONCONTROLSEX icex;
    WNDCLASS wc;
    HWND hwnd;
    MSG msg;
    icex.dwSize = sizeof(icex);
    icex.dwICC  = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&icex);
    wc.style          = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc    = window_proc;
    wc.cbClsExtra     = 0;
    wc.cbWndExtra     = 0;
    wc.hInstance      = hInstance;
    wc.hIcon          = LoadIcon(hInstance, IDI_APPLICATION);
    wc.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground  = GetSysColorBrush(COLOR_3DFACE);
    wc.lpszMenuName   = NULL;
    wc.lpszClassName  = "SM64NETLAUNCHERCLASS";
    RegisterClass(&wc);
    hwnd = CreateWindow(
        wc.lpszClassName,
        "SM64Net Launcher",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        0, 0, W_MAIN_W, W_MAIN_H,
        NULL, NULL, hInstance, NULL
    );
    ShowWindow(hwnd, nShowCmd);
    UpdateWindow(hwnd);
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}
