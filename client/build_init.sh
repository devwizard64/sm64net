#!/bin/bash

#                   SM64Net - An Internet framework for SM64
#                    Copyright (C) 2019, 2020  devwizard
#       This project is licensed under the GNU General Public License
#       version 2.  See LICENSE for more information.

set -e
VERSION=1.0.1
MKDIR()
{
    if [ ! -d $1 ]
    then
        mkdir $1
    fi
}
MKDIR build
