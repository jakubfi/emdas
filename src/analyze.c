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

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

#include "analyze.h"
#include "image.h"

// -----------------------------------------------------------------------
char *lab_add(struct cell *image, int imgsize, uint16_t addr, int type)
{
	static char *lab_names[] = { "jmp", "dat", "io_no", "io_en", "io_ok", "io_pe", "???" };
	static unsigned lab_cnt[LAB_MAX];
	static char buf[MAX_LAB_LEN+1];
	
	struct cell *labcell;

	assert(image && (type < LAB_MAX));

	if (addr >= imgsize) {
		return NULL;
	}

	labcell = image + addr;

	// do not try putting label on norm arg
	if (labcell->type == C_NORM) {
		return NULL;
	}

	snprintf(buf, MAX_LAB_LEN, "%s_%d_", lab_names[type], lab_cnt[type]);
	if (!labcell->label) {
		labcell->label = strdup(buf);
		lab_cnt[type]++;
	}

	return labcell->label;
}

// -----------------------------------------------------------------------
int an_labels(struct cell *image, int size)
{
	int i = 0;

	while (i < size) {

		// if instruction is in/ou, handle additional arguments
		if (image[i].flags & F_IO) {
			int d = _C(image[i].v) ? 1 : 2;
			image[i+d+0].argname = lab_add(image, size, image[i+d+0].v, LAB_IO_NO);
			image[i+d+1].argname = lab_add(image, size, image[i+d+1].v, LAB_IO_EN);
			image[i+d+2].argname = lab_add(image, size, image[i+d+2].v, LAB_IO_OK);
			image[i+d+3].argname = lab_add(image, size, image[i+d+3].v, LAB_IO_PE);
		}

		// if instruction is jump
		if (image[i].flags & F_JUMP) {
			// if argument is absolute and it's the only part of norm arg
			if ((image[i].flags & F_NORM) && (!_C(image[i].v) && !_B(image[i].v))) {
				image[i].argname = lab_add(image, size, image[i+1].v, LAB_JMP);
			}
			// if argument is relative (short)
			if (image[i].flags & F_SHORT) {
				image[i].argname = lab_add(image, size, i+1+_T(image[i].v), LAB_JMP);
			}
		}

		// if instruction takes address as an argument
		if (image[i].flags & F_ADDR) {
			if ((image[i].flags & F_NORM) && (!_B(image[i].v))) {
				image[i].argname = lab_add(image, size, image[i+1].v, LAB_DATA);
			}
			if (image[i].flags & F_SHORT) {
				image[i].argname = lab_add(image, size, i+1+_T(image[i].v), LAB_DATA);
			}
		// if argument is norm indirect
		} else if ((image[i].flags & F_NORM) && !_B(image[i].v) && !_C(image[i].v) && (_D(image[i].v))) {
			image[i].argname = lab_add(image, size, image[i+1].v, LAB_DATA);
		// if argument is byte-norm indirect
		} else if ((image[i].flags & F_BNORM) && !_B(image[i].v) && !_C(image[i].v) && (_D(image[i].v))) {
			image[i].argname = lab_add(image, size, image[i+1].v >> 1, LAB_DATA);
		}

		i++;
	}
	return 0;
}

// -----------------------------------------------------------------------
void set_as_data(struct cell *c)
{
	c->type = C_DATA;
	c->flags = 0;
	c->mnemo = NULL;
}

// -----------------------------------------------------------------------
int an_args(struct cell *image, int size)
{
	int i = 0;
	int d;

	while (i < size) {
		// handle norm arg
		if (((image[i].flags & F_NORM) || (image[i].flags & F_BNORM)) && (!_C(image[i].v))) {
			d = 1;
			if (i+1 < size) {
				set_as_data(image+i+1);
				image[i+1].type = C_NORM;
			} else {
				set_as_data(image+i);
				break; // false-positive norm arg at EOF
			}
		} else {
			d = 0;
		}

		// in/ou additional arguments
		if ((image[i].flags & F_IO) && (i+4+d < size)) {
			set_as_data(image+i+1+d);
			set_as_data(image+i+2+d);
			set_as_data(image+i+3+d);
			set_as_data(image+i+4+d);
		}

		// only if argument is constant norm
		if (!_C(image[i].v) && !_B(image[i].v) && (image[i+1].v < size)) {
			// arithmetic: dwords
			if (image[i].flags & F_ADWORD) {
				set_as_data(image + image[i+1].v + 0);
				set_as_data(image + image[i+1].v + 1);
			}
			// arithmetic: floats
			if (image[i].flags & F_AFLOAT) {
				set_as_data(image + image[i+1].v + 0);
				set_as_data(image + image[i+1].v + 1);
				set_as_data(image + image[i+1].v + 2);
			}
		}

		i++;
	}
	return 0;
}

// -----------------------------------------------------------------------
int an_code(struct cell *image, int size)
{
	int i = 0;

	while (i < size) {
		struct opdef *op = get_op(image[i].v);

		assert(op);

		image[i].type = op->type;
		image[i].mnemo = op->mnemo;
		image[i].flags = op->flags;

		i++;
	}

	return 0;
}

// vim: tabstop=4 autoindent
