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
#include <inttypes.h>
#include <arpa/inet.h>

#include "image.h"

int no_loc, no_val;

// -----------------------------------------------------------------------
int dump_norm(struct cell *image, FILE *f)
{
	assert(image);

	int ll = 0;
	int i = 0;

	if (_D(image[i].v)) ll += fprintf(f, "[");

	if (_C(image[i].v)) {
		ll += fprintf(f, "r%i", _C(image[i].v));
		if (_B(image[i].v)) {
			ll += fprintf(f, "+r%i", _B(image[i].v));
		}
	} else {
		if (_B(image[i].v)) {
			ll += fprintf(f, "r%i", _B(image[i].v));
			ll += fprintf(f, "+0x%04x", (uint16_t) image[i+1].v);
		} else {
			if (image[i].argname) {
				ll += fprintf(f, "%s", image[i].argname);
			} else {
				ll += fprintf(f, "0x%04x", (uint16_t) image[i+1].v);
			}
		}
	}

	if (_D(image[i].v)) ll += fprintf(f, "]");
	return ll;
}

// -----------------------------------------------------------------------
int dump_data(struct cell *image, FILE *f)
{
	int ll = 0;
	assert(image);

	ll += fprintf(f, ".word ");
	if (image->argname) {
		ll += fprintf(f, "%s", image->argname);
	} else {
		ll += fprintf(f, "0x%04x", image->v);
	}
	return ll;
}

#define MAX_LINE 50

// -----------------------------------------------------------------------
int write_asm(struct cell *image, int size, FILE *f)
{
	int i = 0;
	struct cell *c;
	int ll;
	char *spaces = malloc(MAX_LINE+1);
	memset(spaces, ' ', MAX_LINE);
	spaces[MAX_LINE] ='\0';

	while (i < size) {
		c = image + i;
		ll = 0;

		if (!no_loc) {
			ll += fprintf(f, "0x%04x: ", i);
		}
		if (c->label) {
			ll += fprintf(f, "%15s:  ", c->label);
		} else {
			ll += fprintf(f, "                  ");
		}

		switch (c->type) {
			case C_DATA:
				ll += dump_data(c, f);
				break;
			case C_OP__:
				ll += fprintf(f,"%-5s", c->mnemo);
				break;
			case C_OP_R:
				ll += fprintf(f,"%-5s r%i", c->mnemo, _A(c->v));
				break;
			case C_OP_RT:
				if (c->argname) {
					ll += fprintf(f,"%-5s r%i, %s", c->mnemo, _A(c->v), c->argname);
				} else {
					ll += fprintf(f,"%-5s r%i, %i", c->mnemo, _A(c->v), _T(c->v));
				}
				break;
			case C_OP_T:
				if (c->argname) {
					ll += fprintf(f,"%-5s %s", c->mnemo, c->argname);
				} else {
					if (c->flags & F_RELATIVE) {
						ll += fprintf(f,"%-5s %i ; -> %i", c->mnemo, _T(c->v), i+1+_T(c->v));
					} else {
						ll += fprintf(f,"%-5s %i", c->mnemo, _T(c->v));
					}
				}
				break;
			case C_OP_B:
				// fix for blc
				if ((c->v & 0b1111111100000000) == 0b1110010000000000) {
					ll += fprintf(f,"%-5s %i", c->mnemo, (uint16_t)_b(c->v)<<8);
				} else {
					ll += fprintf(f,"%-5s %i", c->mnemo, _b(c->v));
				}
				break;
			case C_OP_t:
				ll += fprintf(f,"%-5s r%i, %i", c->mnemo, _A(c->v), _t(c->v));
				break;
			case C_OP_RN:
				ll += fprintf(f,"%-5s r%i, ", c->mnemo, _A(c->v));
				ll += dump_norm(c, f);
				break;
			case C_OP_N:
				ll += fprintf(f,"%-5s ", c->mnemo);
				ll += dump_norm(c, f);
				break;
			default:
				assert(!"invalid cell type");
				break;
		}   
		if ((!no_val) && (ll < MAX_LINE) && ((c->type != C_DATA) || (c->argname))) {
			ll += fprintf(f, "%s ; .word 0x%04x", spaces+ll, c->v);
		}
		fprintf(f, "\n");
		i++;
		if (((c->type == C_OP_N) || (c->type == C_OP_RN)) && !_C(c->v)) i++;
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
	if (len < 0) {
		free(buf);
		return -1;
	}

	*image = calloc(sizeof(struct cell), len);
	if (!image) {
		free(buf);
		return -1;
	}

	for (int i=0 ; i<len ; i++) {
		(*image)[i].v = ntohs(buf[i]);
	}

	free(buf);
	return len;
}

// vim: tabstop=4 autoindent
