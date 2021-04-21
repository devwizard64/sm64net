#                   SM64Net - An Internet framework for SM64
#                     Copyright (C) 2019 - 2021  devwizard
#         This project is licensed under the terms of the GNU General
#         Public License version 2.  See LICENSE for more information.

import curses

import sm64net
import server

stdscr = None

def init():
    global stdscr
    stdscr = curses.initscr()
    curses.noecho()
    curses.cbreak()
    stdscr.nodelay(True)

def destroy():
    global stdscr
    curses.nocbreak()
    curses.echo()
    curses.endwin()

def update():
    global stdscr
    y = 0
    stdscr.addstr(y, 0, sm64net.VERSION_STR.center(80), curses.A_REVERSE)
    y += 2
    stdscr.addstr(y, 0, "Clients:".ljust(80), curses.A_REVERSE)
    y += 1
    l = len(server.np_table)
    h = l//2
    for i in range(h):
        y = 3 + (i%h)
        x = 1 + 40*(i//h)
        s = ""
        for x in range(2):
            ln = ""
            n = h*x+i
            np = server.np_table[n]
            if np != None:
                ln += " %02d %s" % (n, np.name if np.name != None else "<null>")
            if len(ln) > 38:
                ln = ln[:35] + "..."
            s += ln.ljust(38)
            if x == 0:
                s += " |"
        stdscr.addstr(y, 0, s.ljust(79))
        y += 1
    stdscr.addstr(y, 0, "Press 'q' to quit.".ljust(80), curses.A_REVERSE)
    return stdscr.getch() != ord("q")
