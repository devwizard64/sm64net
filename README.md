# SM64Net - An Internet framework for SM64
Copyright (C) 2019 - 2022  devwizard

This project is licensed under the terms of the GNU General Public License
version 2.  See `LICENSE` for more information.

## About
This project provides a framework for developing modifications to *Super Mario
64* that utilise an Internet connection.

## How to play
Download the latest release from the
[Releases](https://github.com/devwizard64/sm64net/releases/) tab.  Extract
`SM64Net.zip`, then run `SM64NetGUI.exe`.  Fill out the information required,
then click `Launch` while your emulator is running.

We strongly recommend using [Parallel Launcher](https://parallel-launcher.ca/),
as that works best with this project.  Before running, open the settings menu,
then check `Show advanced ROM options`, then click OK.  Then, with your SM64 ROM
selected, check `Use Interpreter Core`.
**If you do not do this step, SM64Net will not work.**

## Using the Server
Use WSL to compile the server on Windows.  Run the server on a separate machine
as the client you join with, as running the client may cause interference with
the server.

### Dependencies
* `apt install make gcc-mips-linux-gnu`
* `pip install windows-curses` **(Windows, no WSL)**

## Using the Client

### Dependencies
* `apt install make gcc` **(Native)**
* `apt install make gcc-mingw-w64-x86-64` **(Windows cross-compile)**

### Usage
`./sm64net <proc> <addr> <port> [nff ...]`
* `proc` - Emulator process name
* `addr` - Server address
* `port` - Server port, default is `4352`
* `nff` - Optional NFF file to read, you may specify multiple

## Using the USB Client (experimental)

### Dependencies
* `apt install make gcc gcc-mips-linux-gnu libftdi1-dev`
* An English SM64 ROM - Name it `UNSME00.z64` and place it in `donor/`.
