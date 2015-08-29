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
#include <assert.h>

#include <emawp.h>

#include "emdas.h"
#include "emdas/errors.h"
#include "opfields.h"

// -----------------------------------------------------------------------
struct emdas *emdas_create(int iset_type, emdas_getfun getfun)
{
	struct emdas *emd = NULL;

	if (!getfun) {
		emdas_error = EMD_E_GETFUN_MISSING;
		goto cleanup;
	}

	emd = malloc(sizeof(struct emdas));
	if (!emd) {
		emdas_error = EMD_E_ALLOC;
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
	emdas_set_features(emd, EMD_FEAT_DEFAULTS);
	emdas_set_tabs(emd, EMD_TAB_LABEL, EMD_TAB_MNEMO, EMD_TAB_ARG, EMD_TAB_ALT);

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
	assert(emd);

	return emd->features;
}

// -----------------------------------------------------------------------
int emdas_set_features(struct emdas *emd, unsigned features)
{
	assert(emd);

	if ((features & (~EMD_FEAT_ALL))) {
		return EMD_E_FEATURE_UNKNOWN;
	}

	emd->features = features;

	return EMD_E_OK;
}

// -----------------------------------------------------------------------
void emdas_set_nl(struct emdas *emd, char c)
{
	assert(emd);

	emdas_buf_set_nl(emd->dbuf, c);
}

// -----------------------------------------------------------------------
int emdas_set_tabs(struct emdas *emd, unsigned label, unsigned mnemo, unsigned arg, unsigned alt)
{
	assert(emd);

	if (!((label <= mnemo) && (mnemo <= arg) && (arg <= alt) && (alt <= EMD_TAB_MAX))) {
		return EMD_E_TABS_MISPLACED;
	}

	emd->tabs.label = label;
	emd->tabs.mnemo = mnemo;
	emd->tabs.arg = arg;
	emd->tabs.alt = alt;

	return EMD_E_OK;
}

// -----------------------------------------------------------------------
char * emdas_get_buf(struct emdas *emd)
{
	assert(emd);

	return emd->dbuf->buf;
}

// -----------------------------------------------------------------------
int emdas_get_linecnt(struct emdas *emd)
{
	assert(emd);

	return emd->dbuf->lines;
}

// -----------------------------------------------------------------------
static int emdas_print_flags(struct emdas_buf *buf, uint16_t flags)
{
	static const char allflags[] = "ZMVCLEGYX1234567";
	char flagset[18] = "?";
	int fpos = 1;

	if (flags == 0) {
		return emdas_buf_app(buf, "%i", 0);
	}

	for (int i=15 ; i>=0 ; i--) {
		if (flags & (1<<i)) {
			flagset[fpos++] = allflags[15-i];
		}
	}

	flagset[fpos] = '\0';

	return emdas_buf_app(buf, "%s", flagset);
}

// -----------------------------------------------------------------------
static void emdas_print_arg(struct emdas *emd, struct emdas_op *op, uint16_t *varg, struct emdas_dh_elem *ref, int as_data)
{
	// .word "argument"
	if (as_data || (op->id == EMD_OP_NONE)) {
		if (ref) {
			emdas_buf_app(emd->dbuf, "%s_%x", emdas_lab_types[ref->type], ref->addr);
		} else {
			emdas_buf_app(emd->dbuf, "0x%04x", op->v);
		}
		return;
	}

	// register argument
	if (op->flags & EMD_FL_ARG_REG) {
		if (op->flags & EMD_FL_ARG2) {
			// arguments continue
			emdas_buf_app(emd->dbuf, "r%i, ", _A(op->v));
		} else {
			// register is the only argument
			emdas_buf_app(emd->dbuf, "r%i", _A(op->v));
			return;
		}
	}

	// short 4-bit argument
	if (op->flags & EMD_FL_ARG_SHORT4) {
		emdas_buf_app(emd->dbuf, "%i", _t(op->v));

	// short 7-bit argument
	} else if (op->flags & EMD_FL_ARG_SHORT7) {
		if (ref) {
			emdas_buf_app(emd->dbuf, "%s_%x", emdas_lab_types[ref->type], ref->addr);
		} else {
			// unsigned (octal) for HLT
			if (op->id == EMD_OP_HLT) {
				emdas_buf_app(emd->dbuf, "0%02o", _Tuns(op->v));
			// signed for other
			} else {
				emdas_buf_app(emd->dbuf, "%s%i", _Tsign(op->v), _Tabs(op->v));
			}
		}

	// short 8-bit argument
	} else if (op->flags & EMD_FL_ARG_SHORT8) {
		uint16_t arg = _b(op->v);
		// BRC and BLC args always refer to CPU flags
		if (op->flags & EMD_FL_ARG_FLAGS) {
			// correction for BLC argument
			if (op->id == EMD_OP_BLC) arg <<= 8;
			emdas_print_flags(emd->dbuf, arg);
		} else {
			emdas_buf_app(emd->dbuf, "%d", arg);
		}

	// normal argument
	} else if (op->flags & EMD_FL_ARG_NORM) {
		if (op->flags & EMD_FL_MOD_D) emdas_buf_c(emd->dbuf, '[');

		// 2nd arg
		if (op->flags & EMD_FL_2WORD) {
			// no memory or split arg
			if (!varg) {
				emdas_buf_app(emd->dbuf, "%s", "r0");
			// named argument
			} else if (ref) {
				emdas_buf_app(emd->dbuf, "%s_%x", emdas_lab_types[ref->type], ref->addr);
			// arg refers to CPU flags
			} else if (op->flags & EMD_FL_ARG_FLAGS) {
				emdas_print_flags(emd->dbuf, *varg);
			// print small integers as decimal
			} else if (*varg < 16) {
				emdas_buf_app(emd->dbuf, "%i", *varg);
			// hex constant
			} else {
				emdas_buf_app(emd->dbuf, "0x%04x", *varg);
			}

		// rC
		} else {
			emdas_buf_app(emd->dbuf, "r%d", _C(op->v));
		}

		// rB
		if (op->flags & EMD_FL_MOD_B) {
			emdas_buf_app(emd->dbuf, "+r%d", _B(op->v));
		}

		if (op->flags & EMD_FL_MOD_D) emdas_buf_c(emd->dbuf, ']');
	}
}

// -----------------------------------------------------------------------
static void emdas_print_op(struct emdas *emd, struct emdas_op *op, int as_data)
{
	int op_id = as_data ? EMD_OP_NONE : op->id;

	int clen = emdas_buf_app(emd->dbuf, "%s", emdas_iset_mnemo[op_id]);
	if (emd->features & EMD_FEAT_LMNEMO) {
		emdas_buf_tolower(emd->dbuf, clen);
	}
}

// -----------------------------------------------------------------------
static void emdas_print_comment(struct emdas *emd, struct emdas_op *op, uint16_t *varg, int as_data)
{
	// cell was printed as data, so alt is code
	if (as_data) {
		emdas_buf_app(emd->dbuf, "%s", "; ");
		emdas_print_op(emd, op, 0);
		emdas_buf_c(emd->dbuf, ' ');
		emdas_print_arg(emd, op, NULL, NULL, 0);
	// cell was printed as code, alt is data
	} else {
		// first word is always data then
		emdas_buf_app(emd->dbuf, "; .word 0x%04x", op->v);
		// if op is 2-word
		if (op->flags & EMD_FL_2WORD) {
			if (varg) {
				struct emdas_op *aop = emd->ops + *varg;
				if (aop->id != EMD_OP_NONE) {
					emdas_buf_app(emd->dbuf, "%s", "  ");
					emdas_print_op(emd, aop, 0);
					emdas_buf_c(emd->dbuf, ' ');
					emdas_print_arg(emd, aop, NULL, NULL, 0);
				} else {
					emdas_buf_app(emd->dbuf, ", 0x%04x", aop->v);
				}
			} else {
				// cannot read memory
				emdas_buf_app(emd->dbuf, ", %s", "; ???");
			}
		}
	}
}

// -----------------------------------------------------------------------
static int emdas_print_label(struct emdas *emd, unsigned nb, uint16_t addr)
{
	int lab_type = EMD_LAB_NONE;

	struct emdas_dh_elem *lab = emdas_dh_get(emd->cellinfo[nb], addr);
	if (lab && (lab->type != EMD_LAB_NONE)) {
		emdas_buf_tab(emd->dbuf, emd->tabs.label);
		emdas_buf_app(emd->dbuf, "%s_%x:", emdas_lab_types[lab->type], lab->addr);
		lab_type = lab->type;
	}

	return lab_type;
}

// -----------------------------------------------------------------------
static void emdas_print_float(struct emdas *emd, unsigned nb, uint16_t addr)
{
	uint16_t r1, r2, r3;
	double f;
	int cnt = 0;

	cnt += emd->memget(nb, addr, &r1);
	cnt += emd->memget(nb, addr+1, &r2);
	cnt += emd->memget(nb, addr+2, &r3);

	if (cnt == 3) {
		awp_to_double(&f, r1, r2, r3);
		emdas_buf_app(emd->dbuf, "; .float %f", f);
	}
}

// -----------------------------------------------------------------------
static void emdas_print_dword(struct emdas *emd, unsigned nb, uint16_t addr)
{
	uint16_t r1, r2;
	int32_t dw;
	int cnt = 0;

	cnt += emd->memget(nb, addr, &r1);
	cnt += emd->memget(nb, addr+1, &r2);

	if (cnt == 2) {
		dw = r1 << 16;
		dw |= r2 & 0xffff;
		emdas_buf_app(emd->dbuf, "; .dword %i", dw);
	}
}

// -----------------------------------------------------------------------
static int emdas_print(struct emdas *emd, unsigned nb, uint16_t addr, int as_data)
{
	int len = 1;
	struct emdas_op *op;
	uint16_t vop;
	uint16_t varg;
	int has_varg = 0;
	struct emdas_dh_elem *ref = NULL;
	int split_arg = 0;
	int lab_type;

	// 1. print address
	if (emd->features & EMD_FEAT_ADDR) {
		emdas_buf_app(emd->dbuf, "0x%04x:", addr);
	}

	// 2. print label
	lab_type = emdas_print_label(emd, nb, addr);

	emdas_buf_tab(emd->dbuf, emd->tabs.mnemo);

	// get instruction opcode
	if (emd->memget(nb, addr, &vop)) {
		op = emd->ops + vop;
	} else {
		// no memory
		emdas_buf_app(emd->dbuf, "%s", "; ???");
		return len;
	}

	// deopcodize strange ops (but leave op as it is, because we still
	// want to print an alternative
	if ((op->flags & EMD_FL_OP_STRANGE)) {
		as_data = 1;
	}

	// 3. print mnemonic
	emdas_print_op(emd, op, as_data);

	// get 2nd arg if necessary
	if (!as_data && (op->flags & EMD_FL_2WORD)) {
		has_varg = 1;
		ref = emdas_dh_get(emd->cellinfo[nb], addr+1);

		// is there a label on arg? (split op/arg)
		if (ref && ref->type != EMD_LAB_NONE) {
			split_arg = 1;
			has_varg = 0;
		// we want 2nd arg only if there is no label on argument (split op/arg)
		} else {
			has_varg = emd->memget(nb, addr+1, &varg);
			len++;
		}
	// get current word as ref otherwise
	} else {
		ref = emdas_dh_get(emd->cellinfo[nb], addr);
	}

	// get referenced cell (for named argument)
	if (ref) {
		ref = ref->ref;
	}

	// 4. print argument(-s)
	if (as_data || !(op->flags & EMD_FL_ARG_NONE)) {
		emdas_buf_tab(emd->dbuf, emd->tabs.arg);
		emdas_print_arg(emd, op, has_varg ? &varg : NULL, ref, as_data);
	}

	// 5. print comment with alternatives
	if (emd->features & EMD_FEAT_ALTS) {
		if (op->id != EMD_OP_NONE) {
			emdas_buf_tab(emd->dbuf, emd->tabs.alt);
			emdas_print_comment(emd, op, has_varg ? &varg : NULL, as_data);
		}

		if (lab_type == EMD_LAB_FLOAT) {
			emdas_buf_tab(emd->dbuf, emd->tabs.alt);
			emdas_print_float(emd, nb, addr);
		} else if (lab_type == EMD_LAB_DWORD) {
			emdas_buf_tab(emd->dbuf, emd->tabs.alt);
			emdas_print_dword(emd, nb, addr);
		}
	}

	// handle split arg
	if (split_arg) {
		emdas_buf_nl(emd->dbuf);
		len += emdas_print(emd, nb, addr+1, 1);
	}

	// handle additional IN/OU "arguments"
	if ((emd->features & EMD_FEAT_IOARGS) && !as_data && (op->flags & EMD_FL_INS_IO)) {
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
int emdas_dasm(struct emdas *emd, unsigned nb, uint16_t addr)
{
	assert(emd);

	if (nb > 15) {
		emdas_error = EMD_E_MEM_BLOCK;
		return 0;
	}

	int len;

	emdas_buf_reset(emd->dbuf);
	len = emdas_print(emd, nb, addr, 0);
	emdas_buf_nl(emd->dbuf);

	return len;
}

// -----------------------------------------------------------------------
int emdas_analyze(struct emdas *emd, unsigned nb, uint16_t addr, unsigned size)
{
	assert(emd);

	if (nb > 15) {
		return EMD_E_MEM_BLOCK;
	}

	// redo hash, old one is useless now
	emdas_dh_destroy(emd->cellinfo[nb]);
	emd->cellinfo[nb] = emdas_dh_create();
	if (!emd->cellinfo) {
		return emdas_error;
	}

	int ic = addr;
	uint16_t vop, varg;
	struct emdas_op *op;
	int laddr;
	int ltype;
	struct emdas_dh_elem *ref;
	int ref_ic;

	int max_ic = addr+size-1;
	if (max_ic > 0xffff) max_ic = 0xffff;

	while (ic <= max_ic) {

		// get instruction at IC
		if (!emd->memget(nb, ic, &vop)) {
			ic++;
			continue;
		}
		op = emd->ops + vop;

		// do nothing for suspicious ops
		if (FMATCH(op->flags, EMD_FL_OP_STRANGE)) {
			ic++;
			continue;
		}

		laddr = -1;
		ltype = EMD_LAB_NONE;

		// First, check for D-modification on 2-word norm arg.
		// Doesn't matter if effective argument is an address - in such cases we put label only on address that is
		// a direct result of D-modification, effective argument is considered a variable and not analyzed for labels.
		// This is also due to the fact that this may cause cell wanting to reference more than one cell, e.g.:
		//  * PF argument is [X], we put label X at the destination (X is constant)
		//  * at [X] we have value Y, which is a float address (Y is variable)
		//  * it may happen, that value Y also resolves as opcode that references another cell
		if (FMATCH(op->flags, EMD_FL_2WORD | EMD_FL_MOD_D)) {
			if (FNMATCH(op->flags, EMD_FL_MOD_B | EMD_FL_MOD_PRE)) {
				ref_ic = ic+1;
				int has_varg = emd->memget(nb, ref_ic, &varg);
				if (has_varg && (varg >= addr) && (varg < addr+size)) {
					ref = emdas_dh_add(emd->cellinfo[nb], varg, EMD_LAB_WORD, NULL);
					if (!ref) {
						return emdas_error;
					}
					if (!emdas_dh_add(emd->cellinfo[nb], ref_ic, EMD_LAB_NONE, ref)) {
						return emdas_error;
					}
				}
			}
		}

		// Check for effective arguments that are addresses
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
		} else if (op->flags & EMD_FL_ADDR_WORD7) {
			ltype = EMD_LAB_WORD7;
		}

		// Effective argument is an address...
		if (ltype != EMD_LAB_NONE) {
			// ...short, relative
			if (FMATCH(op->flags, EMD_FL_ARG_RELATIVE | EMD_FL_ARG_SHORT7)) {
				ref_ic = ic;
				laddr = (uint16_t) (ref_ic + 1 + _T(vop));
			// ...long, norm-arg, absolute
			} else if (op->flags & EMD_FL_2WORD) {
				// Make sure there is no pre- nor B-modification.
				// also, D-modified arguments have been handled earlier
				if (FNMATCH(op->flags, EMD_FL_MOD_D | EMD_FL_MOD_B | EMD_FL_MOD_PRE)) {
					ref_ic = ic+1;
					if (emd->memget(nb, ref_ic, &varg)) {
						laddr = varg;
					}
				}
			}
		}

		// we've found something - add label
		if ((laddr >= 0) && (laddr >= addr) && (laddr < addr+size)) {
			ref = emdas_dh_add(emd->cellinfo[nb], laddr, ltype, NULL);
			if (!ref) {
				return emdas_error;
			}
			if (!emdas_dh_add(emd->cellinfo[nb], ref_ic, EMD_LAB_NONE, ref)) {
				return emdas_error;
			}
		}

		// advance IC
		ic++;
		if (op->flags & EMD_FL_2WORD) ic++;

		// process 4 additional I/O args
		if ((emd->features & EMD_FEAT_IOARGS) && (op->flags & EMD_FL_INS_IO)) {
			ltype = EMD_LAB_IO_NO;
			for (int i=0 ; i<4 ; i++) {
				int has_varg = emd->memget(nb, ic, &varg);
				if (has_varg && (varg >= addr) && (varg < addr+size)) {
					ref = emdas_dh_add(emd->cellinfo[nb], varg, ltype, NULL);
					if (!ref) {
						return emdas_error;
					}
					if (!emdas_dh_add(emd->cellinfo[nb], ic, EMD_LAB_NONE, ref)) {
						return emdas_error;
					}
				}
				ic++;
				ltype++;
			}
		}
	}

	return EMD_E_OK;
}

// vim: tabstop=4 shiftwidth=4 autoindent
