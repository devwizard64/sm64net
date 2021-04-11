# SM64Net - An Internet framework for SM64
Copyright (C) 2019 - 2021  devwizard

This project is licensed under the terms of the GNU General Public License
version 2.  See `LICENSE` for more information.

## About
This project provides a framework for developing modifications to *Super Mario
64* that utilise an Internet connection.

## How to use
We strongly recommend using the latest nightly build of Project64, as that works
best with this project.  Download it here:
https://www.pj64-emu.com/nightly-builds

When running SM64Net for the first time on Windows, a dialogue box from Windows
Firewall will appear asking you if you want to allow network access to SM64Net.
Check **both** boxes on the bottom, then click 'Allow access.'

## Server

### Usage
*Python 2 is also supported.*

`python3 main.py [plugin]`
* `plugin` - Server plugin, defaults to `default`

### Dependencies (Server)
* `apt install gcc-mips-linux-gnu`
* `pip install windows-curses`

## Client

### Usage
`./sm64net <proc> <addr> <port> [nff ...]`
* `proc` - Emulator process name
* `addr` - Server address
* `port` - Server port, default is `4352`
* `nff` - Optional NFF file to read, you may specify multiple

### Dependencies
* **(Only for compiling natively)** `apt install gcc`
* **(Only for cross-compiling for Windows)** `apt install gcc-mingw-w64-i686`

## USB

### Dependencies
* `apt install gcc gcc-mips-linux-gnu libftdi1-dev`
* An English SM64 ROM - Name it `UNSME0.z64` and place it in `usb/`.
