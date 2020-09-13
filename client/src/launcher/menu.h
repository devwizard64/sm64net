/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                   Copyright (C) 2019, 2020  devwizard                      *
 *      This project is licensed under the GNU General Public License         *
 *      version 2.  See LICENSE for more information.                         *
 ******************************************************************************/

#ifndef _MENU_H_
#define _MENU_H_

#include "types.h"

extern void menu_h_m(char *, const u8 *, size_t);
extern void menu_a_m(char *, const u8 *, ssize_t);
extern void menu_m_a(u8 *, const char *, ssize_t);

#endif /* _MENU_H_ */
