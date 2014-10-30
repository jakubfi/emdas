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

#include "opfields.h"

char *emdas_p_emdas_mnemo[EMD_OP_MAX] = {
			".word",
/* NORM */	"LW", "TW", "LS", "RI", "RW", "PW", "RJ", "IS",
			"BB", "BM", "BS", "BC", "BN", "OU", "IN",
/* F/D */	"AD", "SD", "MW", "DW", "AF", "SF", "MF", "DF",
/* NORM */	"AW", "AC", "SW", "CW", "OR", "OM", "NR", "NM",
			"ER", "EM", "XR", "XM", "CL", "LB", "RB", "CB",
/* KA1 */	"AWT", "TRB", "IRB", "DRB", "CWT", "LWT", "LWS", "RWS",
/* JS */	"UJS", "NOP", "JLS", "JES", "JGS", "JVS", "JXS", "JYS", "JCS",
/* KA2 */	"BLC", "EXL", "BRC", "NRF",
/* C */		"RIC", "ZLB", "SXU", "NGA", "SLZ", "SLY", "SLX", "SRY", "NGL", "RPC",
			"SHC",
			"RKY", "ZRB", "SXL", "NGC", "SVZ", "SVY", "SVX", "SRX", "SRZ", "LPC",
/* S */		"HLT", "MCL", "CIT", "SIL", "SIU", "SIT", "GIU", "LIP", "GIL",
			"CRON", "SINT", "SIND",
/* J */		"UJ", "JL", "JE", "JG", "JZ", "JM", "JN", "LJ",
/* L */		"LD", "LF", "LA", "LL", "TD", "TF", "TA", "TL",
/* G */		"RD", "RF", "RA", "RL", "PD", "PF", "PA", "PL",
/* B/N */	"MB", "IM", "KI", "FI", "SP", "MD", "RZ", "IB"
};

// -----------------------------------------------------------------------
static int emdas_p_emdas_print_flags(struct emdas_buf *buf, uint16_t flags)
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
void emdas_p_emdas_build_arg(struct emdas_buf *buf, uint16_t vop, struct emdas_op *op, uint16_t *varg)
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
			emdas_p_emdas_print_flags(buf, arg);
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
					emdas_p_emdas_print_flags(buf, *varg);
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

// vim: tabstop=4 shiftwidth=4 autoindent
