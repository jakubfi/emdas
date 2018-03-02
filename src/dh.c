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

#include <stdlib.h>
#include <stdio.h>

#include "emdas.h"
#include "dh.h"

char *emdas_lab_types[] = { "none", "jmp", "call", "iono", "ioen", "iook", "iope", "byte", "word", "dword", "float", "word7" };

// -----------------------------------------------------------------------
struct emdas_dh_table * emdas_dh_create()
{
	struct emdas_dh_table *dh = malloc(sizeof(struct emdas_dh_table));
	if (!dh) {
		emdas_error = EMD_E_ALLOC;
		return NULL;
	}

	dh->slots = calloc(EMD_HASH_SIZE, sizeof(struct emdas_dh_elem));
	if (!dh->slots) {
		free(dh);
		emdas_error = EMD_E_ALLOC;
		return NULL;
	}

	return dh;
}

// -----------------------------------------------------------------------
#define emdas_dh_hash(addr) ((addr) & 0b1111111111)

// -----------------------------------------------------------------------
struct emdas_dh_elem * emdas_dh_get(struct emdas_dh_table *dh, uint16_t addr)
{
	if (!dh) return NULL;

	unsigned hash = emdas_dh_hash(addr);
	struct emdas_dh_elem *elem = dh->slots[hash];

	while (elem) {
		if (elem->addr == addr) {
			return elem;
		}
		elem = elem->next;
	}

	return NULL;
}

// -----------------------------------------------------------------------
struct emdas_dh_elem * emdas_dh_add(struct emdas_dh_table *dh, uint16_t addr, short type, struct emdas_dh_elem *ref)
{
	if (!dh) return NULL;

	unsigned hash = emdas_dh_hash(addr);
	struct emdas_dh_elem *elem = dh->slots[hash];

	while (elem) {
		if (elem->addr == addr) {
			if ((type == EMD_LAB_NONE) && ref) {
				elem->ref = ref;
			} else if ((type != EMD_LAB_NONE) && !ref) {
				elem->type = type;
			}
			return elem;
		}
		elem = elem->next;
	}

	struct emdas_dh_elem *new_elem = malloc(sizeof(struct emdas_dh_elem));
	if (!new_elem) {
		emdas_error = EMD_E_ALLOC;
		return NULL;
	}
	new_elem->addr = addr;
	new_elem->type = type;
	new_elem->ref = ref;
	new_elem->next = dh->slots[hash];
	dh->slots[hash] = new_elem;

	return new_elem;
}

// -----------------------------------------------------------------------
int emdas_dh_delete(struct emdas_dh_table *dh, uint16_t addr)
{
	unsigned hash = emdas_dh_hash(addr);
	struct emdas_dh_elem *prev = NULL;
	struct emdas_dh_elem *elem = dh->slots[hash];

    while (elem) {
        if (elem->addr == addr) {
			if (prev) {
				prev->next = elem->next;
			} else {
				dh->slots[hash] = elem->next;
			}
			free(elem);
            return 0;
        }
		prev = elem;
        elem = elem->next;
    }
	return -1;
}

// -----------------------------------------------------------------------
void emdas_dh_destroy(struct emdas_dh_table *dh)
{
	int i;
	struct emdas_dh_elem *elem;
	struct emdas_dh_elem *tmp;

	if (!dh) return;

	for(i=0 ; i<EMD_HASH_SIZE ; i++) {
		elem = dh->slots[i];
		while (elem) {
			tmp = elem->next;
			free(elem);
			elem = tmp;
		}
	}

	free(dh->slots);
	free(dh);
}

// -----------------------------------------------------------------------
void emdas_dh_dump_stats(struct emdas_dh_table *dh)
{
	int i;
	int elem_total = 0, collisions = 0, max_depth = 0;

	struct emdas_dh_elem *elem;

	if (!dh) return;

	unsigned *elem_slot = calloc(EMD_HASH_SIZE, sizeof(int));

	for(i=0 ; i<EMD_HASH_SIZE ; i++) {
		elem = dh->slots[i];
		int depth = 0;
		while (elem) {
			depth++;
			elem_total++;
			elem_slot[i]++;
			if (elem_slot[i] > 1) {
				collisions++;
			}
			elem = elem->next;
		}
		if (depth > max_depth) max_depth = depth;
	}

	printf("-----------------------------------\n");
	printf("      Slots: %d\n", EMD_HASH_SIZE);
	printf("   Elements: %d\n", elem_total);
	printf("  Max depth: %d\n", max_depth);
	printf(" Collisions: %d\n", collisions);

	free(elem_slot);
}

// vim: tabstop=4 autoindent
