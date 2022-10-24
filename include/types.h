/*
 * Basic types
 *
 * Copyright (c) 2022, Kaimakan71.
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#define EMBED_HELPER(x) #x
#define EMBED(x) EMBED_HELPER(x) // Stringifies 'x'

typedef unsigned char byte;  // Byte (8 bits)
typedef unsigned short word; // Word (16 bits)
typedef unsigned int dword;  // Doubleword (32 bits)

typedef byte UInt8;   // Unsigned 8-bit integer
typedef word UInt16;  // Unsigned 16-bit integer
typedef dword UInt32; // Unsigned 32-bit integer

typedef char Int8;   // Signed 8-bit integer
typedef short Int16; // Signed 16-bit integer
typedef int Int32;   // Signed 32-bit integer

// When 64-bit support is added, this will make it much easier to switch sizes
typedef UInt32 uint; // Unsigned integer

typedef UInt8 bool; // Boolean
#define true 1      // True
#define false 0     // False

#define nullptr NULL // Empty pointer to nothing

#define KiB 1024         // Kilobyte
#define MiB (KiB * 1024) // Megabyte
#define GiB (MiB * 1024) // Gigabyte

#define LSW(x) ((UInt32)(x) & 0xFFFF)         // Least significant word
#define MSW(x) (((UInt32)(x) >> 16) & 0xFFFF) // Most significant word
#define LSB(x) ((x) & 0xFF)                   // Least significant byte
#define MSB(x) (((x)>>8) & 0xFF)              // Most significant byte