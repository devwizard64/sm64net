# SM64Net - An Internet framework for SM64
Copyright (C) 2019 - 2022  devwizard

This project is licensed under the terms of the GNU General Public License
version 2.  See `LICENSE` for more information.

## About
This project provides a framework for developing modifications to *Super Mario
64* that utilise an Internet connection.

## How to use
**If you are hosting a server,** follow the instructions for the server
installation and usage.  Run the server on a separate machine as the client
you join with, as running the client may cause interference with the server.

**If you are joining a server,** download the latest release from the Releases
tab: https://github.com/devwizard64/sm64net/releases/ (`SM64Net.zip`) and follow
the instructions below:

We strongly recommend using the latest nightly build of Project64 from 64DD.com,
as that works best with this project.  Download it here:
https://64dd.org/downloads.html (Click the link that says `64DD.org Build
[date]` to download it as a portable ZIP.)

When running SM64Net for the first time on Windows, a dialogue box from Windows
Firewall will appear asking you if you want to allow network access to SM64Net.
Check **both** boxes on the bottom, then click 'Allow access.'

## Server

Use WSL to compile the server on Windows.

### Dependencies (Server)
* `apt install make gcc-mips-linux-gnu`
* `pip install windows-curses` **(Windows)**

## Client

### Dependencies
* `apt install make gcc` **(Native)**
* `apt install make gcc-mingw-w64-i686` **(Windows cross-compile)**

### Usage
`./sm64net <proc> <addr> <port> [nff ...]`
* `proc` - Emulator process name
* `addr` - Server address
* `port` - Server port, default is `4352`
* `nff` - Optional NFF file to read, you may specify multiple

## USB (experimental)

### Dependencies
* `apt install make gcc gcc-mips-linux-gnu libftdi1-dev`
* An English SM64 ROM - Name it `UNSME0.z64` and place it in `donor/`.
