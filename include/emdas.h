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

#ifdef __cplusplus
extern "C" {
#endif

// instruction sets

enum emdas_iset_type {
	EMD_ISET_MERA400,
	EMD_ISET_MX16,
};

// disassembly features

enum emdas_features {
	EMD_FEAT_NONE		= 0,		// all features off
	EMD_FEAT_ADDR		= 1 << 0,	// print addresses
	EMD_FEAT_UMNEMO		= 1 << 1,	// use uppercase mnemonics
	EMD_FEAT_ALTS		= 1 << 2,	// print alternatives in comments
	EMD_FEAT_IOARGS		= 1 << 3,	// print IN/OU return addresses as instruction arguments
	EMD_FEAT_ALL		= EMD_FEAT_ADDR | EMD_FEAT_UMNEMO | EMD_FEAT_ALTS | EMD_FEAT_IOARGS,
	EMD_FEAT_DEFAULT	= EMD_FEAT_ADDR | EMD_FEAT_ALTS | EMD_FEAT_IOARGS,
};

// errors

enum emdas_errors {
	EMD_E_UNKNOWN = -32000,
	EMD_E_ALLOC,
	EMD_E_ISET_UNKNOWN,
	EMD_E_GETFUN_MISSING,
	EMD_E_FEATURE_UNKNOWN,
	EMD_E_TABS_MISPLACED,
	EMD_E_MEM_BLOCK,

	EMD_E_OK = 0
};

extern int emdas_error;

typedef int (*emdas_getfun)(int nb, uint16_t addr, uint16_t *dest);

struct emdas;

struct emdas *emdas_create(int iset_type, emdas_getfun getfun);
void emdas_destroy(struct emdas *emd);

unsigned emdas_get_features(struct emdas *emd);
int emdas_set_features(struct emdas *emd, unsigned features);
int emdas_set_tabs(struct emdas *emd, unsigned label, unsigned mnemo, unsigned arg, unsigned alt);
void emdas_set_nl(struct emdas *emd, char c);

char * emdas_get_buf(struct emdas *emd);
int emdas_get_linecnt(struct emdas *emd);

int emdas_dasm(struct emdas *emd, unsigned nb, uint16_t addr);
int emdas_analyze(struct emdas *emd, unsigned nb, uint16_t addr, unsigned size);

char * emdas_get_error(int e);

#ifdef __cplusplus
}
#endif

#endif

// vim: tabstop=4 shiftwidth=4 autoindent
