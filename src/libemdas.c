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
#include "opfields.h"

// -----------------------------------------------------------------------
struct emdas *emdas_create(int iset_type, emdas_getfun getfun)
{
	struct emdas *emd = NULL;

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
static int emdas_print_flags(struct emdas_buf *buf, uint16_t flags)
{
	static char allflags[] = "ZMVCLEGYX1234567";
	char flagset[18];

	int fpos = 0;

	flagset[fpos++] = '?';

	for (int i=15 ; i>=0 ; i--) {
		if (flags & (1<<i)) {
			flagset[fpos++] = allflags[15-i];
		}
	}

	flagset[fpos] = '\0';

	return emdas_buf_s(buf, "%s", flagset);
}

// -----------------------------------------------------------------------
static void emdas_print_arg(struct emdas_buf *buf, uint16_t vop, struct emdas_op *op, uint16_t *varg)
{
	// register argument
	if (op->flags & EMD_FL_ARG_REG) {
		if (op->flags & EMD_FL_ARG2) {
			// arguments continue
			emdas_buf_i(buf, "r%i, ", _A(vop));
		} else {
			// register is the only argument
			emdas_buf_i(buf, "r%i", _A(vop));
			return;
		}
	}

	// short 4-bit argument
	if (op->flags & EMD_FL_ARG_SHORT4) {
		emdas_buf_i(buf, "%i", _t(vop));

	// short 7-bit argument
	} else if (op->flags & EMD_FL_ARG_SHORT7) {
		emdas_buf_i(buf, "%i", _T(vop));

	// short 8-bit argument
	} else if (op->flags & EMD_FL_ARG_SHORT8) {
		uint16_t arg = _b(vop);
		// BRC and BLC args always refer to CPU flags
		if (op->flags & EMD_FL_ARG_FLAGS) {
			// correction for BLC argument
			if (op->id == EMD_OP_BLC) arg <<= 8;
			emdas_print_flags(buf, arg);
		} else {
			emdas_buf_i(buf, "%d", arg);
		}

	// normal argument
	} else if (op->flags & EMD_FL_ARG_NORM) {
		if (op->flags & EMD_FL_MOD_D) emdas_buf_c(buf, '[');

		// 2nd arg
		if (op->flags & EMD_FL_2WORD) {
			// no memory
			if (!varg) {
				emdas_buf_s(buf, "%s", "???");
			// print small integers as decimal
			} else if (*varg < 16) {
				emdas_buf_i(buf, "%i", *varg);
			} else {
				// arg refers to CPU flags
				if (op->flags & EMD_FL_ARG_FLAGS) {
					emdas_print_flags(buf, *varg);
				// hex constant
				} else {
					emdas_buf_i(buf, "0x%04x", *varg);
				}
			}

		// rC
		} else {
			emdas_buf_i(buf, "r%d", _C(vop));
		}

		// rB
		if (op->flags & EMD_FL_MOD_B) {
			emdas_buf_i(buf, "+r%d", _B(vop));
		}

		if (op->flags & EMD_FL_MOD_D) emdas_buf_c(buf, ']');
	}
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
		int len = emdas_buf_ts(emd->dbuf, emd->tabs.mnemo, "%s", emdas_iset_mnemo[op->id]);
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
			emdas_print_arg(emd->dbuf, *vop, op, varg);
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
