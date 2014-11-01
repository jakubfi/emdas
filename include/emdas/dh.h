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

#ifndef DH_H
#define DH_H

#include <inttypes.h>

#define EMD_HASH_SIZE 1024

struct emd_dh_elem {
	uint16_t addr;
	short type;
	unsigned flags;
	struct emd_dh_elem *next;
};

struct emd_dh_table {
	struct emd_dh_elem **slots;
};

struct emd_dh_table * emd_dh_create();
unsigned emd_dh_hash(struct emd_dh_table *dh, uint16_t addr);
struct emd_dh_elem * emd_dh_get(struct emd_dh_table *dh, uint16_t addr);
struct emd_dh_elem * emd_dh_add(struct emd_dh_table *dh, uint16_t addr, short type, unsigned flags);
int emd_dh_delete(struct emd_dh_table *dh, uint16_t addr);
void emd_dh_destroy(struct emd_dh_table *dh);

#endif

// vim: tabstop=4 autoindent
