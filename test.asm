# x86 emulator test
# Copyright (c) 2022, Kaimakan71.
# SPDX-License-Identifier: BSD-2-Clause
[bits 16]
[org 0x7c00]

cli
sti
cld
std
hlt

times 510-($-$$) db 0
dw 0xaa55