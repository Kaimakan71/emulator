/*
 * x86 emulator
 *
 * Copyright (c) 2022, Kaimakan71.
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <types.h>
#include <regs.h>

#define DEBUG true
#define debug if(DEBUG) printf

#define RAM_SIZE (640 * KiB)
#define LOAD_ADDR 0x7c00
#define SECTOR 512

FILE* bootdisk;
long disk_size;

Registers regs;
byte* memory = nullptr;
word flags = 0;
bool hlt = false;

#define getb(addr) memory[addr]
#define getw(addr) *(word*)(memory + (addr))

void dumpRegs() {
	printf(
		"CS:IP=%X:%X SS:SP=%X:%X DS=%X ES=%X\n"
		"AX=%X BX=%X CX=%X DX=%X SI=%X DI=%X FL=%X\n",
		regs.cs,regs.ip, regs.ss,regs.sp, regs.ds, regs.es,
		regs.ax, regs.bx, regs.cx, regs.dx, regs.si, regs.di, flags
	);
}

void fatal(const char* msg) {
	printf("%s", msg);
	if(memory) free(memory);
	if(bootdisk) fclose(bootdisk);
	exit(1);
}

void* regcode8[] = {
	&regs.al,
	&regs.cl,
	&regs.dl,
	&regs.bl,

	&regs.ah,
	&regs.ch,
	&regs.dh,
	&regs.bh
};

void* regcode16[] = {
	&regs.ax,
	&regs.cx,
	&regs.dx,
	&regs.bx,
	&regs.sp,
	&regs.bp,
	&regs.si,
	&regs.di
};

void cycle() {
	word start_ip = regs.ip;
	byte opcode = getb(regs.ip++);

	if(opcode == 0x31) { // XOR RMW, RW
		byte reg = getb(regs.ip) - 0xC0;
		*(word*)(regcode16[reg % 4]) ^= *(word*)(regcode16[reg / 8]);
		regs.ip += sizeof(byte);
	} else if(opcode >= 0x50 && opcode <= 0x57) { // PUSH RX/P/I
		byte reg = opcode - 0x50;

		regs.sp -= sizeof(word);
		memory[regs.sp] = *(word*)(regcode16[reg]);
	} else if(opcode >= 0x58 && opcode <= 0x5F) { // POP RX/P/I
		byte reg = opcode - 0x58;

		*(word*)(regcode16[reg]) = memory[regs.sp];
		regs.sp += sizeof(word);
	} else if(opcode == 0xA0) { // MOV AL, RMB
		regs.al = getb(getw(regs.ip));
		regs.ip += sizeof(word);
	} else if(opcode == 0xA1) { // MOV AX, RMW
		regs.al = getw(getw(regs.ip));
		regs.ip += sizeof(word);
	} else if(opcode >= 0xB0 && opcode <= 0xBF) { // MOV RX/L/H, IB/W
		byte reg = opcode - 0xB0;
		
		if(reg & (1 << 3)) {
			reg &= ~(1 << 3);
			*(word*)(regcode16[reg]) = getw(regs.ip);
			regs.ip += sizeof(word);
		} else {
			*(byte*)(regcode8[reg]) = getb(regs.ip);
			regs.ip += sizeof(byte);
		}
	} else if(opcode == 0xEB) regs.ip += ((signed char)(getb(regs.ip))) + 1; // JMP
	else if(opcode == 0xF4) hlt = true;      // HLT
	else if(opcode == 0xF8) flags &= ~FL_CF; // CLC
	else if(opcode == 0xFA) flags &= ~FL_IF; // CLI
	else if(opcode == 0xFB) flags |= FL_IF;  // STI
	else if(opcode == 0xFC) flags &= ~FL_DF; // CLD
	else if(opcode == 0xFD) flags |= FL_DF;  // STD
	else {
		hlt = true;
		printf("Invalid opcode %X at %X\n", opcode, start_ip);
	}
}

int main(int argc, char* argv[]) {
	if(argc > 2) fatal("Too many arguments\n");

	// Try to open the disk image for reading
	FILE* bootdisk = fopen((argc > 1) ? argv[1]:"test.bin", "r");
	if(!bootdisk) fatal("Boot failed: could not read the boot disk\n");

	// Check the file's size
	fseek(bootdisk, 0, SEEK_END);
	disk_size = ftell(bootdisk);
	fseek(bootdisk, 0, SEEK_SET);
	if(disk_size < SECTOR) fatal("Boot failed: not a bootable disk\n");

	// Time to actually allocate the ram
	memory = (byte*)malloc(RAM_SIZE);

	// Read the data
	fread((void*)(memory + LOAD_ADDR), SECTOR, 1, bootdisk);
	// if(getw(LOAD_ADDR + 510) != 0xaa55) fatal("Boot failed: not a bootable disk\n");

	// Dump data for debugging purposes
	for(int i = 0; i < 512; i++) printf("%X ", getb(LOAD_ADDR + i));
	printf("\n");

	// Initialize state
	memset(&regs, 0, sizeof(regs));
	regs.ip = LOAD_ADDR; // Start where the code was loaded
	regs.sp = 0x7b00;    // 29.5KiB initial real mode stack (the bootsector probably changes this instantly)
	flags |= FL_IF;      // Enable interrupts on boot

	// Run loop
	while(!hlt) cycle();
	dumpRegs();

	free(memory);
	fclose(bootdisk);
	return 0;
}