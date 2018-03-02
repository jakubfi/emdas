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

#include "emdas.h"

int emdas_error;

struct emdas_err_desc {
	int code;
	char *message;
} emdas_errordesc[] = {
	{ EMD_E_OK, "no error" },
	{ EMD_E_ALLOC, "memory allocation error" },
	{ EMD_E_ISET_UNKNOWN, "unknown instruction set" },
	{ EMD_E_GETFUN_MISSING, "missing memory getter function" },
	{ EMD_E_FEATURE_UNKNOWN, "unknown feature" },
	{ EMD_E_TABS_MISPLACED, "misplaced format tabulators" },
	{ EMD_E_MEM_BLOCK, "wrong memory block number" },

	{ EMD_E_UNKNOWN, "unknown error" }
};

// -----------------------------------------------------------------------
char * emdas_get_error(int e)
{
	struct emdas_err_desc *edict = emdas_errordesc;
	while (edict->code != EMD_E_UNKNOWN) {
		if (e == edict->code) {
			return edict->message;
		}
		edict++;
	}
	return edict->message;
}

// vim: tabstop=4 autoindent
