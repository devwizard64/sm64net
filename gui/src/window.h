/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                    Copyright (C) 2019 - 2022  devwizard                    *
 *        This project is licensed under the terms of the GNU General         *
 *        Public License version 2.  See LICENSE for more information.        *
 ******************************************************************************/

#ifndef __WINDOW_H__
#define __WINDOW_H__

#include <windows.h>

#include <types.h>

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
};

typedef struct wnd
{
    HWND hwnd;
    LPCTSTR class;
    LPCTSTR name;
    DWORD style;
    int x;
    int y;
    int w;
    int h;
}
WND;

extern WND wnd_table[];
extern LRESULT CALLBACK window_proc(
    HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam
);

#endif /* __WINDOW_H__ */
