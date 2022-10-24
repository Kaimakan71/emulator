# x86 emulator makefile
# Copyright (c) 2022, Kaimakan71.
# SPDX-License-Identifier: BSD-2-Clause

all: emulate test.bin run

emulate: main.c
	@echo "Compiling $<..."; gcc $< -o $@ -Iinclude

test.bin: test.asm
	@echo "Assembling $<..."; nasm $< -f bin -o $@

run:
	@./emulate

qemu:
	@qemu-system-i386 -drive format=raw,file=test.bin,if=floppy