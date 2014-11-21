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

#ifndef EMDAS_DH_H
#define EMDAS_DH_H

#include <inttypes.h>

#define EMD_HASH_SIZE 1024

enum emdas_lab_types {
	EMD_LAB_NONE = 0,
	EMD_LAB_JUMP,
	EMD_LAB_CALL,
	EMD_LAB_IO_NO,
	EMD_LAB_IO_EN,
	EMD_LAB_IO_OK,
	EMD_LAB_IO_PE,
	EMD_LAB_BYTE,
	EMD_LAB_WORD,
	EMD_LAB_DWORD,
	EMD_LAB_FLOAT,
	EMD_LAB_WORD7,
};

extern char *emdas_lab_types[];

struct emdas_dh_elem {
	uint16_t addr;
	short type;
	struct emdas_dh_elem *ref;
	struct emdas_dh_elem *next;
};

struct emdas_dh_table {
	struct emdas_dh_elem **slots;
};

struct emdas_dh_table * emdas_dh_create();
unsigned emdas_dh_hash(struct emdas_dh_table *dh, uint16_t addr);
struct emdas_dh_elem * emdas_dh_get(struct emdas_dh_table *dh, uint16_t addr);
struct emdas_dh_elem * emdas_dh_add(struct emdas_dh_table *dh, uint16_t addr, short type, struct emdas_dh_elem *ref);
int emdas_dh_delete(struct emdas_dh_table *dh, uint16_t addr);
void emdas_dh_destroy(struct emdas_dh_table *dh);

#endif

// vim: tabstop=4 autoindent
