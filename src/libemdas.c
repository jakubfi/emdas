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
#include "p_emdas.h"

// -----------------------------------------------------------------------
struct emdas *emdas_create(int iset_type, int printer_type, emdas_getfun getfun)
{
	struct emdas *emd = NULL;

	if ((printer_type != EMD_PRINTER_DEFAULT) && (printer_type != EMD_PRINTER_EMDAS)) {
		goto cleanup;
	}

	if (!getfun) {
		goto cleanup;
	}

	emd = malloc(sizeof(struct emdas));
	if (!emd) {
		goto cleanup;
	}

	emd->ops = emdas_iset_create(iset_type);
	if (!emd->ops) {
		goto cleanup;
	}

	emd->dbuf = emdas_buf_create(EMD_DASM_BUF_SIZE);
	if (!emd->dbuf) {
		goto cleanup;
	}

	emd->memget = getfun;
	emdas_set_features(emd, EMD_FEAT_ADDR | EMD_FEAT_LABELS | EMD_FEAT_ALTS);
	emdas_set_tabs(emd, 0, 10, 20, 25, 50);

	// this is hardcoded for now
	emd->mnemo = emdas_p_emdas_mnemo;
	emd->build_arg_fun = emdas_p_emdas_build_arg;

	return emd;

cleanup:
	emdas_destroy(emd);
	return NULL;
}

// -----------------------------------------------------------------------
void emdas_destroy(struct emdas *emd)
{
	if (!emd) return;

	emdas_iset_destroy(emd->ops);
	emdas_buf_destroy(emd->dbuf);
	free(emd);
}

// -----------------------------------------------------------------------
unsigned emdas_get_features(struct emdas *emd)
{
	if (!emd) return 0;

	return emd->features;
}

// -----------------------------------------------------------------------
int emdas_set_features(struct emdas *emd, unsigned features)
{
	if (!emd) return -1;
	if ((features & (~EMD_FEAT_ALL))) return -1;

	emd->features = features;

	return 0;
}

// -----------------------------------------------------------------------
int emdas_set_tabs(struct emdas *emd, unsigned addr, unsigned label, unsigned mnemo, unsigned arg, unsigned alt)
{
	emd->tabs.addr = addr;
	emd->tabs.label = label;
	emd->tabs.mnemo = mnemo;
	emd->tabs.arg = arg;
	emd->tabs.alt = alt;

	return 0;
}

// -----------------------------------------------------------------------
char * emdas_get_buf(struct emdas *emd)
{
	return emd->dbuf->buf;
}

// -----------------------------------------------------------------------
int emdas_dasm(struct emdas *emd, int nb, uint16_t addr)
{
	int len = 1;
	uint16_t *vop, *varg;
	struct emdas_op *op;

	emdas_buf_reset(emd->dbuf);

	// 1. print address
	if (emd->features & EMD_FEAT_ADDR) {
		emdas_buf_ti(emd->dbuf, emd->tabs.addr, "0x%04x:", addr);
	}

	// 2. print label
	if (emd->features & EMD_FEAT_LABELS) {
	}

	// 3. print opcode
	vop = emd->memget(nb, addr);
	if (vop) {
		op = emd->ops + *vop;
		int len = emdas_buf_ts(emd->dbuf, emd->tabs.mnemo, "%s", emd->mnemo[op->id]);
		if (emd->features & EMD_FEAT_LMNEMO) {
			emdas_buf_tolower(emd->dbuf, len);
		}
	} else {
		// cannot read memory
		emdas_buf_ts(emd->dbuf, emd->tabs.mnemo, "%s", "???");
		goto fin;
	}

	// 4. print arg(s)
	if (op->id == EMD_OP_NONE) {
		// .word "arg"
		emdas_buf_ti(emd->dbuf, emd->tabs.addr, "0x%04x", *vop);
	} else {
		// real instruction argument
		if (!(op->flags & EMD_FL_ARG_NONE)) {
			emdas_buf_tab(emd->dbuf, emd->tabs.arg);
			if (op->flags & EMD_FL_2WORD) {
				// get long arg
				varg = emd->memget(nb, addr+1);
				len++;
			}
			emd->build_arg_fun(emd->dbuf, *vop, op, varg);
		}
	}

	// 5. print comment
	if (emd->features & EMD_FEAT_ALTS) {
		emdas_buf_ti(emd->dbuf, emd->tabs.alt, "; .word 0x%04x", *vop);
		if (op->flags & EMD_FL_2WORD) {
			if (varg) {
				emdas_buf_i(emd->dbuf, ", 0x%04x", *varg);
			} else {
				emdas_buf_s(emd->dbuf, ", %s", "???");
			}
		}
	}

fin:
	emdas_buf_nl(emd->dbuf);
	return len;
}


// vim: tabstop=4 shiftwidth=4 autoindent
