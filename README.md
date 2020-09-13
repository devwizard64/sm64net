# SM64Net - An Internet framework for SM64
Copyright (C) 2019, 2020  devwizard

This project is licensed under the GNU General Public License version 2.  See `LICENSE` for more information.

## About
This project provides a framework for developing modifications to *Super Mario 64* that utilise an Internet connection.

## How to use
We strongly recommend using the latest nightly build of Project64, as that works best with this project.  Download it here: https://www.pj64-emu.com/nightly-builds

### Launcher instructions
1. **(Only if you use a different emulator)** Type in the name of the emulator you are using in the 'Emulator name' box; for example, Project64 would be `Project64.exe`.
2. Type in the IP Address of the server in the 'IP Address' box.
3. **(Only if the server owner uses a different port number)** Type in the port that the server uses into the 'Port' box.  If you are unsure, leave it at `4352`.
4. Type in your username in the 'Username' box.  There is a limit of 31 characters.  Click on the 'Punctuation List...' and 'Macro Character List...' buttons to see a list of all available characters in addition to `0-9`, `A-Z` and `a-z`.
5. Select a colour for your username by clicking on the '...' button on the bottom-right corner of the window.
6. Open your emulator and load a SM64 English ROM.
7. Click on the 'Launch SM64Net' button.
8. **(First time only)** A dialogue box from Windows Firewall will appear asking you if you want to allow network access to SM64Net.  Check **both** boxes on the bottom, then click 'Allow access.'

### Usage (Client)
`./sm64net <proc> <addr> <port> <colour> <name>`
* `proc` - Emulator process name
* `addr` - Server address
* `port` - Server port, default is `4352`
* `colour` - Username colour, `FFFFFF` for white
* `name` - Username, `1624352C32` for "Mario"

### Usage (Server)
*Python 2 is also supported.*

`python3 main.py [plugin]`
* `plugin` - Server plugin, defaults to `default`

## Development

### Dependencies
* `gcc` (for building the **client** natively)
* `gcc-mingw-w64-i686` (for building the Windows **client** on Linux)
* `libftdi1-dev` (for the **USB client**)
* An English SM64 ROM (for the **USB client**) - name it `nsme.z64` and place it in `donor/`
* `gcc-mips-linux-gnu` (for the **USB client** and **server plugins**)
* armips (for the **server plugins**) - Download it here: https://github.com/Kingcom/armips/releases and place `armips` or `armips.exe` in `tools/`
