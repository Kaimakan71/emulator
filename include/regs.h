/*
 * Register storage
 *
 * Copyright (c) 2022, Kaimakan71.
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <types.h>

#define FL_CF (1 << 0)
#define FL_PF (1 << 2)
#define FL_AF (1 << 4)
#define FL_ZF (1 << 6)
#define FL_SF (1 << 7)
#define FL_TF (1 << 8)
#define FL_IF (1 << 9)
#define FL_DF (1 << 10)
#define FL_OF (1 << 11)

typedef struct {
	union {
		struct {
			word ax;
			word cx;
			word dx;
			word bx;
		};

		struct {
			byte al;
			byte ah;
			byte cl;
			byte ch;
			byte dl;
			byte dh;
			byte bl;
			byte bh;
		};
	};

	word si;
	word di;
	word bp;
	word sp;

	word ip;

	word cs;
	word ds;
	word es;
	word ss;
} Registers;