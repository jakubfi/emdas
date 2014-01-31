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
#include <inttypes.h>
#include <arpa/inet.h>

#include "image.h"

// -----------------------------------------------------------------------
int dump_norm(struct cell *image, FILE *f)
{
	int i = 0;
	if (_D(image[i].v)) fprintf(f, "[");

	if (_C(image[i].v)) {
		fprintf(f, "r%i", _C(image[i].v));
		if (_B(image[i].v)) {
			fprintf(f, "+r%i", _B(image[i].v));
		}
	} else {
		if (_B(image[i].v)) {
			fprintf(f, "r%i", _B(image[i].v));
			fprintf(f, "+0x%04x", (uint16_t) image[i+1].v);
		} else {
			if (image[i].argname) {
				fprintf(f, "%s", image[i].argname);
			} else {
				fprintf(f, "0x%04x", (uint16_t) image[i+1].v);
			}
		}
	}

	if (_D(image[i].v)) fprintf(f, "]");
	return 0;
}

// -----------------------------------------------------------------------
int write_asm(struct cell *image, int size, FILE *f)
{
	int i = 0;

	while (i < size) {
		fprintf(f, "x%04x: ", i);
		if (image[i].label) {
			fprintf(f, "%10s:  ", image[i].label);
		} else {
			fprintf(f, "             ");
		}

		switch (image[i].type) {
			case C_DATA:
				fprintf(f, ".word ");
				for (int d=0 ; d<image[i].size ; d++) {
					if (image[i].argname) {
						fprintf(f, "%s", image[i+d].argname);
					} else {
						fprintf(f, "0x%04x", image[i+d].v);
					}
					if (d < image[i].size-1) fprintf(f, ", ");
				}
				break;
			case C_OP__:
				fprintf(f,"%-5s", image[i].mnemo);
				break;
			case C_OP_R:
				fprintf(f,"%-5s r%i", image[i].mnemo, _A(image[i].v));
				break;
			case C_OP_RT:
				if (image[i].argname) {
					fprintf(f,"%-5s r%i, %s", image[i].mnemo, _A(image[i].v), image[i].argname);
				} else {
					fprintf(f,"%-5s r%i, %i", image[i].mnemo, _A(image[i].v), _T(image[i].v));
				}
				break;
			case C_OP_T:
				if (image[i].argname) {
					fprintf(f,"%-5s %s", image[i].mnemo, image[i].argname);
				} else {
					if (image[i].flags & F_RELATIVE) {
						fprintf(f,"%-5s %i ; -> %i", image[i].mnemo, _T(image[i].v), i+1+_T(image[i].v));
					} else {
						fprintf(f,"%-5s %i", image[i].mnemo, _T(image[i].v));
					}
				}
				break;
			case C_OP_B:
				fprintf(f,"%-5s %i", image[i].mnemo, _b(image[i].v));
				break;
			case C_OP_t:
				fprintf(f,"%-5s r%i, %i", image[i].mnemo, _A(image[i].v), _t(image[i].v));
				break;
			case C_OP_RN:
				fprintf(f,"%-5s r%i, ", image[i].mnemo, _A(image[i].v));
				dump_norm(image+i, f);
				break;
			case C_OP_N:
				fprintf(f,"%-5s ", image[i].mnemo);
				dump_norm(image+i, f);
				break;
			default:
				assert(!"invalid cell type");
				break;
		}   
		i += image[i].size;
		fprintf(f, "\n");
	}

	return 0;
}

// -----------------------------------------------------------------------
int reader_raw(FILE *file, struct cell **image)
{
	uint16_t *buf = malloc(sizeof(uint16_t) * MAX_IMAGE);
	if (!buf) {
		return -1;
	}

	int len = fread(buf, sizeof(uint16_t), MAX_IMAGE, file);
	if (len <= 0) {
		return -1;
	}

	*image = calloc(sizeof(struct cell), len);
	if (!image) {
		return -1;
	}

	for (int i=0 ; i<len ; i++) {
		((*image)+i)->v = ntohs(buf[i]);
	}

	free(buf);
	return len;
}

// vim: tabstop=4 autoindent
