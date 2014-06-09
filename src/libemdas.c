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
#include <string.h>
#include <inttypes.h>
#include <assert.h>
#include <limits.h>

#include "emdas.h"
#include "iset.h"

const char *emdas_ilist[] = {
			".word",
/* NORM */	"lw", "tw", "ls", "ri", "rw", "pw", "rj", "is",
			"bb", "bm", "bs", "bc", "bn", "ou", "in",
/* F/D */	"ad", "sd", "mw", "dw", "af", "sf", "mf", "df",
/* NORM */	"aw", "ac", "sw", "cw", "or", "om", "nr", "nm",
			"er", "em", "xr", "xm", "cl", "lb", "rb", "cb",
/* KA1 */	"awt", "trb", "irb", "drb", "cwt", "lwt", "lws", "rws",
/* JS */	"ujs", "nop", "jls", "jes", "jgs", "jvs", "jxs", "jys", "jcs",
/* KA2 */	"blc", "exl", "brc", "nrf",
/* C */		"ric", "zlb", "sxu", "nga", "slz", "sly", "slx", "sry", "ngl", "rpc",
			"shc",
			"rky", "zrb", "sxl", "ngc", "svz", "svy", "svx", "srx", "srz", "lpc",
/* S */		"hlt", "mcl", "cit", "sil", "siu", "sit", "giu", "lip", "gil",
			"cron", "sint", "sind",
/* J */		"uj", "jl", "je", "jg", "jz", "jm", "jn", "lj",
/* L */		"ld", "lf", "la", "ll", "td", "tf", "ta", "tl",
/* G */		"rd", "rf", "ra", "rl", "pd", "pf", "pa", "pl",
/* B/N */	"mb", "im", "ki", "fi", "sp", "md", "rz", "ib"
};

// Default element formats (those can be changed by the user)

const char *emdas_default_elem_format[SYN_ELEM_MAX] = {
	/* SYN_MNEMO */		"%-5s",
	/* SYN_REG */		"r%i",
	/* SYN_ARG_7 */		"%i",
	/* SYN_ARG_4 */		"%i",
	/* SYN_ARG_8 */		"%i",
	/* SYN_ARG_16 */	"0x%04x",
};

// Instructions syntax identifiers

enum emdas_syn_ins {
	SYN_INS_DATA,
	SYN_INS_RN,
	SYN_INS_N,
	SYN_INS_RT,
	SYN_INS_RV,
	SYN_INS_T,
	SYN_INS_B,
	SYN_INS_R,
	SYN_INS__,
	SYN_INS_MAX
};

/*
	Instruction output formatting:

	%% - literal %
	%m - mnemonic
	%r - reg. A argument
	%t - short argument (7-bit) (using label name, if available)
	%T - short argument (7-bit) (numeric)
	%v - very short argument (4-bit, SHC only)
	%B - byte argument (8-bit)
	%n - normal argument (using label name, if available)
	%N - normal argument (numeric)
	%d - cell data
	%A - cell address
*/

// Instruction formats (cannot be changed, this is fixed emdas syntax)

const char *emdas_ins_format[SYN_INS_MAX] = {
	/* SYN_INS_DATA */	"%m %d",
	/* SYN_INS_RN */	"%m %r, %n",
	/* SYN_INS_N */		"%m %n",
	/* SYN_INS_RT */	"%m %r, %t",
	/* SYN_INS_RV */	"%m %r, %v",
	/* SYN_INS_T */		"%m %t",
	/* SYN_INS_B */		"%m %B",
    /* SYN_INS_R */		"%m %r",
    /* SYN_INS__ */		"%m",
};

// -----------------------------------------------------------------------
struct emdas * emdas_init(get_word_f get_word)
{
	if (!get_word) return NULL;

	struct emdas *emd = calloc(1, sizeof(struct emdas));
	if (!emd) {
		return NULL;
	}

	emd->get_word = get_word;

	// setup default element formats
	for (int i=0 ; i<SYN_ELEM_MAX ; i++) {
		int len = strlen(emdas_default_elem_format[i])+1;
		emd->emdas_elem_format[i] = malloc(len);
		if (!emd->emdas_elem_format[i]) {
			emdas_shutdown(emd);
			return NULL;
		}
		memcpy(emd->emdas_elem_format[i], emdas_default_elem_format[i], len);
	}

	return emd;
}

// -----------------------------------------------------------------------
void emdas_shutdown(struct emdas *emd)
{
	if (!emd) return;

	for (int i=0 ; i<SYN_ELEM_MAX ; i++) {
		free(emd->emdas_elem_format[i]);
	}
	free(emd);
}

// -----------------------------------------------------------------------
static int emdas_cell_fill(struct emdas_cell *cell, int want_type, int addr, uint16_t word)
{
	assert(cell);

	const struct opdef *o;

	// cell is believed to be initialized with 0s
	// if particular field is not used, its value is garbage

	cell->addr = addr;
	cell->v = word;
	cell->type = want_type;

	switch (want_type) {
		case CELL_INS:
			o = emdas_get_op(word);
			assert(o);
			if (o->op_id == OP_NONE) {
				return emdas_cell_fill(cell, CELL_DATA, addr, word);
			} else {
				// copy everything, we may want to change it later on
				cell->op_id = o->op_id;
				cell->op_group = o->group_id;
				cell->op_flags = o->op_flags;
				cell->arg_flags = o->arg_flags;

				cell->len = 1;

				// rA register argument
				if ((cell->arg_flags & ARG_REG)) {
					cell->ra = _A(cell->v);
				}

				// set normarg-related things
				if ((cell->arg_flags & ARG_NORM)) {
					cell->rc = _C(cell->v);
					cell->rb = _B(cell->v);
					if (!_C(cell->v)) cell->arg_flags |= ARG_2WORD;
					if (_D(cell->v)) cell->arg_flags |= ARG_MOD_D;
					if (_B(cell->v)) cell->arg_flags |= ARG_MOD_B;
				}

				// fill in argument value
				if ((cell->arg_flags & ARG_SHORT4)) {
					cell->arg_short = _t(cell->v);
				} else if ((cell->arg_flags & ARG_SHORT7)) {
					cell->arg_short = _T(cell->v);
				} else if ((cell->arg_flags & ARG_SHORT8)) {
					cell->arg_short = _b(cell->v);
					// early fix for BLC argument
					if (cell->op_id == OP_BLC) {
						cell->arg_short <<= 8;
					}
				}

				// check if instruction needs another cell for normarg argument
				if ((cell->arg_flags & ARG_NORM) && (cell->arg_flags & ARG_2WORD)) {
					return -1;
				} else {
					return 0;
				}
			}
			break;
		case CELL_NA:
			break;
		case CELL_DATA:
		case CELL_ARG:
			cell->len = 1;
			break;
	}

	return 0;
}

// -----------------------------------------------------------------------
struct emdas_cell * emdas_dasm(struct emdas *emd, uint16_t start_addr, int word_count)
{
	struct emdas_cell *cells = calloc(word_count, sizeof(struct emdas_cell));
	if (!cells) {
		return NULL;
	}

	int word;
	struct emdas_cell *cell = cells;
	struct emdas_cell *missing_arg = NULL;

	for (int count = 0 ; count < word_count ; count++, cell++) {

		word = emd->get_word(start_addr+count);

		if (word < 0) { // could not fetch word
			emdas_cell_fill(cell, CELL_NA, start_addr+count, 0);
		} else if (missing_arg) { // previous cell was missing an argument, fill it now
			emdas_cell_fill(cell, CELL_ARG, start_addr+count, word);
			missing_arg->arg_16 = cell;
			missing_arg->len += 1;
			missing_arg = NULL;
		} else { // fill current cell, try as instruction
			if (emdas_cell_fill(cell, CELL_INS, start_addr+count, word) < 0) {
				missing_arg = cell;
			}
		}
	}

	return cells;
}

// -----------------------------------------------------------------------
static int emdas_normarg_format(struct emdas *emd, char *buf, int maxlen, struct emdas_cell *cell, int use_name)
{
	int pos = 0;

	// D-modification is present
	if ((cell->arg_flags & ARG_MOD_D)) {
		pos += snprintf(buf+pos, maxlen-pos, "[");
	}

	// no 2nd word needed
	if (!(cell->arg_flags & ARG_2WORD)) {
		pos += snprintf(buf+pos, maxlen-pos, emd->emdas_elem_format[SYN_ELEM_REG], cell->rc);
		if ((cell->arg_flags & ARG_MOD_B)) {
			pos += snprintf(buf+pos, maxlen-pos, "+");
			pos += snprintf(buf+pos, maxlen-pos, emd->emdas_elem_format[SYN_ELEM_REG], cell->rb);
		}
	// 16-bit arg in 2nd word
	} else {
		// B-modification is present
		if ((cell->arg_flags & ARG_MOD_B)) {
			pos += snprintf(buf+pos, maxlen-pos, emd->emdas_elem_format[SYN_ELEM_REG], cell->rb);
			if (use_name && cell->arg_name) {
				pos += snprintf(buf+pos, maxlen-pos, "+%s", cell->arg_name);
			} else if (!cell->arg_16) {
				pos += snprintf(buf+pos, maxlen-pos, "+???");
			} else {
				pos += snprintf(buf+pos, maxlen-pos, "+");
				pos += snprintf(buf+pos, maxlen-pos, emd->emdas_elem_format[SYN_ELEM_ARG_16], cell->arg_16->v);
			}
		// no B-modification
		} else {
			if (use_name && cell->arg_name) {
				pos += snprintf(buf+pos, maxlen-pos, "%s", cell->arg_name);
			} else if (!cell->arg_16) {
				pos += snprintf(buf+pos, maxlen-pos, "???");
			} else {
				pos += snprintf(buf+pos, maxlen-pos, emd->emdas_elem_format[SYN_ELEM_ARG_16], cell->arg_16->v);
			}
		}
	}

	// D-modification is present
	if ((cell->arg_flags & ARG_MOD_D)) {
		pos += snprintf(buf+pos, maxlen-pos, "]");
	}

	return pos;
}

// -----------------------------------------------------------------------
static char * emdas_format(struct emdas *emd, int format, struct emdas_cell *cell)
{
	char *fmt = (char *) emdas_ins_format[format];
	char *bret;
	char *bcur;
	int bpos = 0;
	int bmax;
	int esc = 0;

	while (fmt && *fmt) {
		bmax = EMDAS_LINE_MAX-bpos;
		bcur = (emd->buf)+bpos;
		if (esc) { // processing esc seq
			switch (*fmt) {
				case '%': // literal %
					bpos += snprintf(bcur, bmax, "%%");
					break;
				case 'm': // mnemonic
					bpos += snprintf(bcur, bmax, emd->emdas_elem_format[SYN_ELEM_MNEMO], emdas_ilist[cell->op_id]);
					break;
				case 'r': // register A
					bpos += snprintf(bcur, bmax, emd->emdas_elem_format[SYN_ELEM_REG], cell->ra);
					break;
				case 't': // short argument (7-bit) (using label name, if available)
					if (cell->arg_name) {
						bpos += snprintf(bcur, bmax, "%s", cell->arg_name);
					} else {
						bpos += snprintf(bcur, bmax, emd->emdas_elem_format[SYN_ELEM_ARG_7], cell->arg_short);
					}
					break;
				case 'T': // short argument (7-bit) (numeric)
					bpos += snprintf(bcur, bmax, emd->emdas_elem_format[SYN_ELEM_ARG_7], cell->arg_short);
					break;
				case 'v': // very short argument (4-bit, SHC only)
					bpos += snprintf(bcur, bmax, emd->emdas_elem_format[SYN_ELEM_ARG_4], cell->arg_short);
					break;
				case 'B': // byte argument (8-bit)
					bpos += snprintf(bcur, bmax, emd->emdas_elem_format[SYN_ELEM_ARG_8], cell->arg_short);
					break;
				case 'n': // normal argument (using argument name, if available)
					bpos += emdas_normarg_format(emd, bcur, bmax, cell, 1);
					break;
				case 'N': // normal argument (numeric)
					bpos += emdas_normarg_format(emd, bcur, bmax, cell, 0);
					break;
				case 'd':
					bpos += snprintf(bcur, bmax, emd->emdas_elem_format[SYN_ELEM_ARG_16], cell->v);
					break;
				default: // print out unknown escape sequence
					bpos += snprintf(bcur, bmax, "%%%c", *fmt);
					break;
			}
			esc = 0;
		} else if (*fmt == '%') { // start esc seq
			esc = 1;
		} else { // literal
			bpos += snprintf(bcur, bmax, "%c", *fmt);
		}
		fmt++;
	}

	bret = malloc(strlen(emd->buf)+1);
	if (bret) {
		memcpy(bret, emd->buf, strlen(emd->buf)+1);
	}
	return bret;
}

// -----------------------------------------------------------------------
char * emdas_make_text(struct emdas *emd, struct emdas_cell *cell)
{
	char *str;
	int type = cell->type;
	int arg = cell->arg_flags;

	// cell is data
	if (type == CELL_DATA) {
		str = emdas_format(emd, SYN_INS_DATA, cell);
	// cell is instruction
	} else if (type == CELL_INS) {
		if ((arg & ARG_NONE)) {
			str = emdas_format(emd, SYN_INS__, cell);
		} else if ((arg & ARG_REG)) {
			if ((arg & ARG_NORM)) {
				str = emdas_format(emd, SYN_INS_RN, cell);
			} else if ((arg & ARG_SHORT7)) {
				str = emdas_format(emd, SYN_INS_RT, cell);
			} else if ((arg & ARG_SHORT4)) {
				str = emdas_format(emd, SYN_INS_RV, cell);
			} else if ((arg & ARG_SHORT8)) {
				str = NULL;
			} else {
				str = emdas_format(emd, SYN_INS_R, cell);
			}
		} else if ((arg & ARG_NORM)) {
			str = emdas_format(emd, SYN_INS_N, cell);
		} else if ((arg & ARG_SHORT7)) {
			str = emdas_format(emd, SYN_INS_T, cell);
		} else if ((arg & ARG_SHORT8)) {
			str = emdas_format(emd, SYN_INS_B, cell);
		} else {
			str = NULL;
		}
	// cell is unknown, N/A or ARG
	} else {
		str = NULL;
	}

	return str;
}

// -----------------------------------------------------------------------
void __emdas_cell_dump(FILE *f, struct emdas *emd, struct emdas_cell *cell)
{
	static char *cell_type_names[] = {
		 "UNKNOWN", "N/A", "DATA", "OP", "ARG"
	};
	static char *op_group_names[] = {
		"NONE", "NORM", "FD", "KA1", "JS", "KA2", "C", "S", "J", "L", "G", "BN"
	};

	fprintf(f, "---------------------------------------------\n");
	fprintf(f, "Address    : 0x%04x\n", cell->addr);
	fprintf(f, "Cell type  : %s\n", cell_type_names[cell->type]);
	fprintf(f, "Cell label : %s\n", cell->label);
	fprintf(f, "Cell value : 0x%04x\n", cell->v);
	fprintf(f, "OP group   : %s\n", op_group_names[cell->op_group]);
	fprintf(f, "OP flags   : %s%s%s\n",
		(cell->op_flags & OP_FL_OS) ? "OS, " : "",
		(cell->op_flags & OP_FL_IO) ? "IO, " : "",
		(cell->op_flags & OP_FL_MX16) ? "MX16, " : ""
	);
	fprintf(f, "OP ID      : %i\n", cell->op_id);
	fprintf(f, "Mnemo      : %s\n", emdas_ilist[cell->op_id]);
	fprintf(f, "Registers  : rA = %i, rB = %i, rC = %i\n", cell->ra, cell->rb, cell->rc);
	fprintf(f, "Arg. name  : %s\n", cell->arg_name);
	fprintf(f, "Arg. short : %i\n", cell->arg_short);
	if (cell->arg_16) {
		fprintf(f, "Arg. 16    : %i\n", cell->arg_16->v);
	} else {
		fprintf(f, "Arg. 16    : NULL\n");
	}
	fprintf(f, "Arg flags  : %s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s\n",
		(cell->arg_flags & ARG_NONE) ? "NONE, " : "",
		(cell->arg_flags & ARG_REG) ? "REG, " : "",
		(cell->arg_flags & ARG_REGIND) ? "REGIND, " : "",
		(cell->arg_flags & ARG_SHORT4) ? "SHORT4, " : "",
		(cell->arg_flags & ARG_SHORT7) ? "SHORT7, " : "",
		(cell->arg_flags & ARG_SHORT8) ? "SHORT8, " : "",
		(cell->arg_flags & ARG_RELATIVE) ? "RELATIVE, " : "",
		(cell->arg_flags & ARG_NORM) ? "NORM, " : "",
		(cell->arg_flags & ARG_A_JUMP) ? "A_JUMP, " : "",
		(cell->arg_flags & ARG_A_BYTE) ? "A_BYTE, " : "",
		(cell->arg_flags & ARG_A_WORD) ? "A_WORD, " : "",
		(cell->arg_flags & ARG_A_DWORD) ? "A_DWORD, " : "",
		(cell->arg_flags & ARG_A_FLOAT) ? "A_FLOAT, " : "",
		(cell->arg_flags & ARG_2WORD) ? "2WORD, " : "",
		(cell->arg_flags & ARG_MOD_D) ? "MOD_D, " : "",
		(cell->arg_flags & ARG_MOD_B) ? "MOD_B, " : ""
	);
	char *d = emdas_make_text(emd, cell);
	fprintf(f, "Disassm    : %s\n", d);
	free(d);
}


// vim: tabstop=4 shiftwidth=4 autoindent
