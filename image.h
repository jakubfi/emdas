//  Copyright (c) 2014 Jakub Filipowicz <jakubf@gmail.com>
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc.,
//  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#ifndef IMAGE_H
#define IMAGE_H

#include <inttypes.h>
#include <stdio.h>

#include "iset.h"

#define MAX_IMAGE 64 * 1024

enum cell_types {
	C_NONE = 0,
	C_DATA,
	C_OP__,
	C_OP_R,
	C_OP_RT,
	C_OP_T,
	C_OP_B,
	C_OP_t,
	C_OP_RN,
	C_OP_N,
	C_MAX,
};

struct cell {
	uint16_t v;
	int type;
	unsigned flags;
	char *mnemo;
	int size;
	char *label;
	char *argname;
};

extern int no_loc, no_val;

int reader_raw(FILE *file, struct cell **image);
int write_asm(struct cell *image, int size, FILE *f);

#endif

// vim: tabstop=4 autoindent
