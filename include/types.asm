;                   SM64Net - An Internet framework for SM64
;                    Copyright (C) 2019, 2020  devwizard
;       This project is licensed under the GNU General Public License
;       version 2.  See LICENSE for more information.

NULL    equ 0
false   equ 0
true    equ 1

.macro li.u, rs, imm
    .if (imm) == 0
    .elseif ((imm) & 0xFFFF8000) == 0 || ((imm) & 0xFFFF8000) == 0xFFFF8000
    .elseif ((imm) & 0xFFFF0000) != 0
        lui     rs, ((imm) >> 16 & 0xFFFF)
    .else
    .endif
.endmacro

.macro li.l, rs, imm
    .if (imm) == 0
        or      rs, r0, r0
    .elseif ((imm) & 0xFFFF8000) == 0 || ((imm) & 0xFFFF8000) == 0xFFFF8000
        addiu   rs, r0, imm
    .elseif ((imm) & 0xFFFF0000) != 0
        .if ((imm) & 0x0000FFFF) != 0
            ori     rs, rs, ((imm) >>  0 & 0xFFFF)
        .endif
    .else
        ori     rs, r0, ((imm) >>  0 & 0xFFFF)
    .endif
.endmacro

.macro li, rs, imm
    li.u    rs, imm
    li.l    rs, imm
.endmacro

.macro la.u, rs, addr
    lui     rs, hi(addr)
.endmacro

.macro la.l, rs, addr
    addiu   rs, rs, lo(addr)
.endmacro

.macro la, rs, addr
    la.u    rs, addr
    la.l    rs, addr
.endmacro

.macro .base, addr
    .headersize (addr) - orga()
.endmacro
