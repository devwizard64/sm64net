#                   SM64Net - An Internet framework for SM64
#                     Copyright (C) 2019 - 2021  devwizard
#         This project is licensed under the terms of the GNU General
#         Public License version 2.  See LICENSE for more information.

def init():
    return

def destroy():
    return

def update():
    return

def np_update(np):
    return

def np_update_connect(np):
    np.nff_write_file("plugin/hud_font.nff")
    np.nff_write_file("plugin/default/build/main.nff")

def np_update_disconnect(np):
    return

def np_update_tcp(np, cmd, data):
    return
