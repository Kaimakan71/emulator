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
		"AX=%X BX=%X CX=%X DX=%X FL=%X\n",
		regs.cs,regs.ip, regs.ss,regs.sp, regs.ds, regs.es,
		regs.ax, regs.bx, regs.cx, regs.dx, flags
	);
}

void fatal(const char* msg) {
	printf("%s", msg);
	if(memory) free(memory);
	if(bootdisk) fclose(bootdisk);
	exit(1);
}

void cycle() {
	word start_ip = regs.ip;
	byte opcode = getb(regs.ip++);

	if(opcode == 0xEB) {        // JMP (relative)
		regs.ip += ((signed char)(getb(regs.ip))) + 1;
		printf("JMP\n");
	} else if(opcode == 0xF4) { // HLT
		hlt = true;
		printf("HLT\n");
	} else if(opcode == 0xFA) { // CLI
		flags &= ~FL_IF;
		printf("CLI\n");
	} else if(opcode == 0xFB) { // STI
		flags |= FL_IF;
		printf("STI\n");
	} else if(opcode == 0xFC) { // CLD
		flags &= ~FL_DF;
		printf("CLD\n");
	} else if(opcode == 0xFD) { // STD
		flags |= FL_DF;
		printf("STD\n");
	} else {
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
	if(getw(LOAD_ADDR + 510) != 0xaa55) fatal("Boot failed: not a bootable disk\n");

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