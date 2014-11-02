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

	for (int i=0 ; i<16 ; i++) {
		emd->cellinfo[i] = NULL;
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
	emdas_set_tabs(emd, 0, 10, 20, 26, 50);

	return emd;

cleanup:
	emdas_destroy(emd);
	return NULL;
}

// -----------------------------------------------------------------------
void emdas_destroy(struct emdas *emd)
{
	if (!emd) return;

	for (int i=0 ; i<16 ; i++) {
		emdas_dh_destroy(emd->cellinfo[i]);
	}

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
	static const char allflags[] = "ZMVCLEGYX1234567";
	char flagset[18] = "?";
	int fpos = 1;

	if (flags == 0) {
		return emdas_buf_i(buf, "%i", 0);
	}

	for (int i=15 ; i>=0 ; i--) {
		if (flags & (1<<i)) {
			flagset[fpos++] = allflags[15-i];
		}
	}

	flagset[fpos] = '\0';

	return emdas_buf_s(buf, "%s", flagset);
}

// -----------------------------------------------------------------------
static void emdas_print_arg(struct emdas *emd, struct emdas_op *op, uint16_t *varg, int as_data)
{
	// .word "argument"
	if (as_data || (op->id == EMD_OP_NONE)) {
		emdas_buf_i(emd->dbuf, "0x%04x", op->v);
		return;
	}

	// register argument
	if (op->flags & EMD_FL_ARG_REG) {
		if (op->flags & EMD_FL_ARG2) {
			// arguments continue
			emdas_buf_i(emd->dbuf, "r%i, ", _A(op->v));
		} else {
			// register is the only argument
			emdas_buf_i(emd->dbuf, "r%i", _A(op->v));
			return;
		}
	}

	// short 4-bit argument
	if (op->flags & EMD_FL_ARG_SHORT4) {
		emdas_buf_i(emd->dbuf, "%i", _t(op->v));

	// short 7-bit argument
	} else if (op->flags & EMD_FL_ARG_SHORT7) {
		emdas_buf_i(emd->dbuf, "%i", _T(op->v));

	// short 8-bit argument
	} else if (op->flags & EMD_FL_ARG_SHORT8) {
		uint16_t arg = _b(op->v);
		// BRC and BLC args always refer to CPU flags
		if (op->flags & EMD_FL_ARG_FLAGS) {
			// correction for BLC argument
			if (op->id == EMD_OP_BLC) arg <<= 8;
			emdas_print_flags(emd->dbuf, arg);
		} else {
			emdas_buf_i(emd->dbuf, "%d", arg);
		}

	// normal argument
	} else if (op->flags & EMD_FL_ARG_NORM) {
		if (op->flags & EMD_FL_MOD_D) emdas_buf_c(emd->dbuf, '[');

		// 2nd arg
		if (op->flags & EMD_FL_2WORD) {
			// no memory
			if (!varg) {
				emdas_buf_s(emd->dbuf, "%s", "r0");
			// print small integers as decimal
			} else if (*varg < 16) {
				emdas_buf_i(emd->dbuf, "%i", *varg);
			} else {
				// arg refers to CPU flags
				if (op->flags & EMD_FL_ARG_FLAGS) {
					emdas_print_flags(emd->dbuf, *varg);
				// hex constant
				} else {
					emdas_buf_i(emd->dbuf, "0x%04x", *varg);
				}
			}

		// rC
		} else {
			emdas_buf_i(emd->dbuf, "r%d", _C(op->v));
		}

		// rB
		if (op->flags & EMD_FL_MOD_B) {
			emdas_buf_i(emd->dbuf, "+r%d", _B(op->v));
		}

		if (op->flags & EMD_FL_MOD_D) emdas_buf_c(emd->dbuf, ']');
	}
}

// -----------------------------------------------------------------------
static void emdas_print_op(struct emdas *emd, struct emdas_op *op, int as_data)
{
	int op_id = as_data ? EMD_OP_NONE : op->id;

	int clen = emdas_buf_s(emd->dbuf, "%s", emdas_iset_mnemo[op_id]);
	if (emd->features & EMD_FEAT_LMNEMO) {
		emdas_buf_tolower(emd->dbuf, clen);
	}
}

// -----------------------------------------------------------------------
static void emdas_print_comment(struct emdas *emd, struct emdas_op *op, uint16_t *varg, int as_data)
{
	// cell was printed as data, alt is code
	if (as_data) {
		emdas_buf_s(emd->dbuf, "%s", "; ");
		emdas_print_op(emd, op, 0);
		emdas_buf_c(emd->dbuf, ' ');
		emdas_print_arg(emd, op, NULL, 0);
	// cell was printed as code, alt is data
	} else {
		// first word is always data then
		emdas_buf_i(emd->dbuf, "; .word 0x%04x", op->v);
		// if op is 2-word
		if (op->flags & EMD_FL_2WORD) {
			if (varg) {
				struct emdas_op *aop = emd->ops + *varg;
				if (aop->id != EMD_OP_NONE) {
					emdas_buf_s(emd->dbuf, "%s", "  ");
					emdas_print_op(emd, aop, 0);
					emdas_buf_c(emd->dbuf, ' ');
					emdas_print_arg(emd, aop, NULL, 0);
				} else {
					emdas_buf_i(emd->dbuf, ", 0x%04x", aop->v);
				}
			} else {
				// cannot read memory
				emdas_buf_s(emd->dbuf, ", %s", "???");
			}
		}
	}
}

// -----------------------------------------------------------------------
static void emdas_print_label(struct emdas *emd, int nb, uint16_t addr)
{
	struct emdas_dh_elem *lab = emdas_dh_get(emd->cellinfo[nb], addr);
	if (lab) {
		emdas_buf_tab(emd->dbuf, emd->tabs.label);
		emdas_buf_si(emd->dbuf, "%s_%x:", emdas_lab_types[lab->type], lab->addr);
	}
}

// -----------------------------------------------------------------------
static int emdas_print(struct emdas *emd, int nb, uint16_t addr, int as_data)
{
	int len = 1;
	struct emdas_op *op;
	uint16_t *vop;
	uint16_t *varg = NULL;

	// 1. print address
	if (emd->features & EMD_FEAT_ADDR) {
		emdas_buf_tab(emd->dbuf, emd->tabs.addr);
		emdas_buf_i(emd->dbuf, "0x%04x:", addr);
	}

	// 2. print label
	if (emd->features & EMD_FEAT_LABELS) {
		emdas_print_label(emd, nb, addr);
	}

	emdas_buf_tab(emd->dbuf, emd->tabs.mnemo);

	// get instruction opcode
	vop = emd->memget(nb, addr);
	if (vop) {
		op = emd->ops + *vop;
	} else {
		// no memory
		emdas_buf_s(emd->dbuf, "%s", "???");
		return len;
	}

	// 3. print opcode
	emdas_print_op(emd, op, as_data);

	// get 2nd arg if necessary
	if (!as_data && (op->flags & EMD_FL_2WORD)) {
		varg = emd->memget(nb, addr+1);
		len++;
	}

	// 4. print argument(-s)
	if (as_data || !(op->flags & EMD_FL_ARG_NONE)) {
		emdas_buf_tab(emd->dbuf, emd->tabs.arg);
		emdas_print_arg(emd, op, varg, as_data);
	}

	// 5. print comment with alternatives
	if ((emd->features & EMD_FEAT_ALTS) && (op->id != EMD_OP_NONE)) {
		emdas_buf_tab(emd->dbuf, emd->tabs.alt);
		emdas_print_comment(emd, op, varg, as_data);
	}

	// handle additional IN/OU "arguments"
	if (!as_data && (op->flags & EMD_FL_INS_IO)) {
		uint16_t ioaddr = addr + (op->flags & EMD_FL_2WORD ? 2 : 1);
		int cnt = 4;
		while (cnt > 0) {
			emdas_buf_nl(emd->dbuf);
			len += emdas_print(emd, nb, ioaddr, 1);
			ioaddr++;
			cnt--;
		}
	}

	return len;
}

// -----------------------------------------------------------------------
int emdas_dasm(struct emdas *emd, int nb, uint16_t addr)
{
	int len;

	emdas_buf_reset(emd->dbuf);
	len = emdas_print(emd, nb, addr, 0);
	emdas_buf_nl(emd->dbuf);

	return len;
}

// -----------------------------------------------------------------------
void emdas_analyze(struct emdas *emd, int nb, uint16_t addr, int size)
{
	emdas_dh_destroy(emd->cellinfo[nb]);
	emd->cellinfo[nb] = emdas_dh_create();

	int ic = addr;
	uint16_t *vop, *varg;
	struct emdas_op *op;
	int laddr;
	int ltype;

	while (ic < addr+size) {
		vop = emd->memget(nb, ic);
		ic++;
		if (!vop) {
			continue;
		}

		laddr = -1;
		ltype = EMD_LAB_NONE;
		op = emd->ops + *vop;

		// D-modification on 2-word norm arg
		if (FMATCH(op->flags, EMD_FL_2WORD | EMD_FL_MOD_D)) {
			if (FNMATCH(op->flags, EMD_FL_MOD_B | EMD_FL_MOD_PRE)) {
				varg = emd->memget(nb, ic);
				if (varg) {
					emdas_dh_add(emd->cellinfo[nb], *varg, EMD_LAB_WORD, 0);
				}
			}
		}

		// try setting lab type
		if (op->flags & EMD_FL_ADDR_JUMP) {
			ltype = EMD_LAB_JUMP;
		} else if (op->flags & EMD_FL_ADDR_CALL) {
			ltype = EMD_LAB_CALL;
		} else if (op->flags & EMD_FL_ADDR_WORD) {
			ltype = EMD_LAB_WORD;
		} else if (op->flags & EMD_FL_ADDR_BYTE) {
			ltype = EMD_LAB_BYTE;
		} else if (op->flags & EMD_FL_ADDR_DWORD) {
			ltype = EMD_LAB_DWORD;
		} else if (op->flags & EMD_FL_ADDR_FLOAT) {
			ltype = EMD_LAB_FLOAT;
		}

		// get destination address
		if (ltype != EMD_LAB_NONE) {
			// short, relative
			if (FMATCH(op->flags, EMD_FL_ARG_RELATIVE | EMD_FL_ARG_SHORT7)) {
				laddr = ic + _T(*vop);
			// long, norm-arg, absolute
			} else if (op->flags & EMD_FL_2WORD) {
				// no pre- nor B-modification
				if (FNMATCH(op->flags, EMD_FL_MOD_B | EMD_FL_MOD_PRE)) {
					varg = emd->memget(nb, ic);
					if (varg) {
						// D-modification, we need to go deeper
						if (op->flags & EMD_FL_MOD_D) {
							varg = emd->memget(nb, *varg);
						}
						if (varg) {
							laddr = *varg;
						}
					}
				}
			}
		}

		// we've found something - add label
		if (laddr >= 0) {
			emdas_dh_add(emd->cellinfo[nb], laddr, ltype, 0);
		}

		// advance IC if 2-word
		if (op->flags & EMD_FL_2WORD) {
			ic++;
		}

		// process 4 additional I/O args
		if (op->flags & EMD_FL_INS_IO) {
			ltype = EMD_LAB_IO_NO;
			for (int i=0 ; i<4 ; i++) {
				varg = emd->memget(nb, ic);
				ic++;
				if (varg) {
					emdas_dh_add(emd->cellinfo[nb], *varg, ltype, 0);
				}
				ltype++;
			}
		}

	}
}

// vim: tabstop=4 shiftwidth=4 autoindent
