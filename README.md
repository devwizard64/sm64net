# SM64Net - An Internet framework for SM64
Copyright (C) 2019, 2020  devwizard

This project is licensed under the GNU General Public License version 2.  See
`LICENSE` for more information.

## About
This project provides a framework for developing modifications to *Super Mario
64* that utilise an Internet connection.

SM64Net Discord server: https://discord.gg/N2HCm62

## How to use
We strongly recommend using the latest nightly build of Project64, as that works
best with this project.  Download it here:
https://www.pj64-emu.com/nightly-builds

When running SM64Net for the first time on Windows, a dialogue box from Windows
Firewall will appear asking you if you want to allow network access to SM64Net.
Check **both** boxes on the bottom, then click 'Allow access.'

## Development

### Usage (Client)
`./sm64net <proc> <addr> <port> [nff ...]`
* `proc` - Emulator process name
* `addr` - Server address
* `port` - Server port, default is `4352`
* `nff` - Optional NFF file to read, you may specify multiple

### Usage (Server)
*Python 2 is also supported.*

`python3 main.py [plugin]`
* `plugin` - Server plugin, defaults to `default`

### Dependencies (Client)
* **(Only for compiling natively)** `apt install gcc`
* **(Only for cross-compiling for Windows)** `apt install gcc-mingw-w64-i686`

### Dependencies (USB client)
* `apt install gcc-mips-linux-gnu libftdi1-dev`
* An English SM64 ROM - Name it `UNSME0.z64` and place it in `donor/`.

### Dependencies (Server)
* `apt install gcc-mips-linux-gnu`
* armips - Download it here: https://github.com/Kingcom/armips/releases and
  place `armips` or `armips.exe` in `tools/`.
* `python3 -m pip install windows-curses`
