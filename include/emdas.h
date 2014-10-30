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

#ifndef EMDAS_H
#define EMDAS_H

#include <inttypes.h>

#include "emdas/buf.h"
#include "emdas/iset.h"

#define EMD_DASM_BUF_SIZE 4096

enum emdas_features {
	EMD_FEAT_NONE		= 0,
	EMD_FEAT_ADDR		= 1 << 0,	// print addresses
	EMD_FEAT_LABELS		= 1 << 1,	// use labels
	EMD_FEAT_LMNEMO		= 1 << 2,	// lowercase mnemonics
	EMD_FEAT_ALTS		= 1 << 3,	// print alternatives in comments
};

#define EMD_FEAT_ALL (EMD_FEAT_ADDR | EMD_FEAT_LABELS | EMD_FEAT_LMNEMO | EMD_FEAT_ALTS)

// getter function

typedef uint16_t * (*emdas_getfun)(int nb, uint16_t addr);


// emdas object

struct emdas {
	struct emdas_op *ops;

	emdas_getfun memget;
	struct emdas_buf *dbuf;

	unsigned features;
	struct tabs {
		int addr;
		int label;
		int mnemo;
		int arg;
		int alt;
	} tabs;
};

struct emdas *emdas_create(int iset_type, emdas_getfun getfun);
void emdas_destroy(struct emdas *emd);

unsigned emdas_get_features(struct emdas *emd);
int emdas_set_features(struct emdas *emd, unsigned features);
int emdas_set_tabs(struct emdas *emd, unsigned addr, unsigned label, unsigned mnemo, unsigned arg, unsigned alt);

int emdas_dasm(struct emdas *emd, int nb, uint16_t addr);
char * emdas_get_buf(struct emdas *emd);

#endif

// vim: tabstop=4 shiftwidth=4 autoindent
