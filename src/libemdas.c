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
#include <ctype.h>
#include <inttypes.h>
#include <assert.h>
#include <limits.h>
#include <arpa/inet.h>

#include "emdas.h"
#include "iset.h"

#define   FANY(v, flags) (((v) & (flags)) != 0)
#define  FNONE(v, flags) (((v) & (flags)) == 0)
#define FMATCH(v, flags) (((v) & (flags)) == (flags))

static char *emdas_ilist_lcase[OP_MAX] = {
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

static char *emdas_ilist_ucase[OP_MAX] = {
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

static char **emdas_ilist = emdas_ilist_lcase;

// Default element formats (those can be changed by the user)

static const char *emdas_default_elem_format[SYN_ELEM_MAX] = {
	/* SYN_ELEM_MNEMO */	"%-5s",
	/* SYN_ELEM_REG */		"r%i",
	/* SYN_ELEM_ARG_7 */	"%i",
	/* SYN_ELEM_ARG_4 */	"%i",
	/* SYN_ELEM_ARG_8 */	"%i",
	/* SYN_ELEM_ARG_16 */	"0x%x",
	/* SYN_ELEM_ADDR */		"0x%04x: ",
	/* SYN_ELEM_LABEL */	"%10s ", // auto-includes ':'
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
	%d - cell value as data
*/

// Instruction formats (cannot be changed, this is fixed emdas syntax)

static const char *emdas_ins_format[SYN_INS_MAX] = {
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

static char * emdas_make_text(struct emdas_cell *cell, char **elem_format, unsigned features);

// -----------------------------------------------------------------------
struct emdas * emdas_init()
{
	struct emdas *emd = calloc(1, sizeof(struct emdas));
	if (!emd) {
		return NULL;
	}

	// setup default element formats
	if (emdas_reset_syntax(emd)) {
		emdas_shutdown(emd);
		return NULL;
	}

	return emd;
}

// -----------------------------------------------------------------------
void emdas_shutdown(struct emdas *emd)
{
	if (!emd) return;

	for (int i=0 ; i<SYN_ELEM_MAX ; i++) {
		free(emd->elem_format[i]);
	}

	for (int i=0 ; i<0xffff ; i++) {
		free(emd->cells[i].text);
		free(emd->cells[i].label);
		free(emd->cells[i].arg_name);
	}

	free(emd);
}

// -----------------------------------------------------------------------
int emdas_set_syntax(struct emdas *emd, unsigned syn_id, const char *syn)
{
	if (!emd) return -1;
	if (syn_id >= SYN_ELEM_MAX) return -1;

	free(emd->elem_format[syn_id]);

	int len = strlen(syn) + 1;
	emd->elem_format[syn_id] = malloc(len);
	if (!emd->elem_format[syn_id]) {
		return -1;
	}
	memcpy(emd->elem_format[syn_id], syn, len);

	emd->syn_generation++;

	return 0;
}

// -----------------------------------------------------------------------
int emdas_reset_syntax(struct emdas *emd)
{
	if (!emd) return -1;

	for (int i=0 ; i<SYN_ELEM_MAX ; i++) {
		if (emdas_set_syntax(emd, i, emdas_default_elem_format[i])) {
			return -1;
		}
	}
	return 0;
}

// -----------------------------------------------------------------------
unsigned emdas_get_features(struct emdas *emd)
{
	return emd->features;
}

// -----------------------------------------------------------------------
int emdas_set_features(struct emdas *emd, unsigned features)
{
	if (!emd) return -1;
	if ((features & (~FEAT_ALL))) return -1;

	// bump syn generation if any syntax feature changed
	if ((features & FEAT_SYN) != (emd->features & FEAT_SYN)) {
		emd->syn_generation++;
	}

	// choose instruction set
	if ((features & FEAT_UCASE)) {
		emdas_ilist = emdas_ilist_ucase;
	} else {
		emdas_ilist = emdas_ilist_lcase;
	}

	emd->features = features;

	return 0;
}

// -----------------------------------------------------------------------
struct emdas_cell * emdas_get_cell(struct emdas *emd, uint16_t addr)
{
	if (!emd) return NULL;

	struct emdas_cell *cell = emd->cells + addr;

	// check if we need to (re-)generate text representation
	if (!cell->text || (cell->syn_generation != emd->syn_generation)) {
		cell->text = emdas_make_text(cell, emd->elem_format, emd->features);
	    cell->syn_generation = emd->syn_generation;
	}

	return cell;
}

// -----------------------------------------------------------------------
static void emdas_fill_data(struct emdas_cell *cell, uint16_t addr, uint16_t word)
{
	assert(cell);

	cell->addr = addr;
	cell->v = word;
	cell->type = CELL_DATA;

	cell->op_id = OP_NONE;
	cell->op_group = OP_GR_NONE;
	cell->flags = FL_NONE;

	cell->arg_short = INT_MAX;
	cell->arg_16 = NULL;
	cell->arg_name = NULL;

	cell->label = NULL;
}

// -----------------------------------------------------------------------
static void emdas_fill_arg(struct emdas_cell *cell, uint16_t addr, uint16_t word)
{
	emdas_fill_data(cell, addr, word);
	cell->type = CELL_ARG;
}

// -----------------------------------------------------------------------
static int emdas_fill_ins(struct emdas_cell *cell, const struct opdef *o, uint16_t addr, uint16_t word)
{
	assert(cell && o);

	cell->addr = addr;
	cell->v = word;
	cell->type = CELL_INS;

	// copy everything, we may want to change it later on
	cell->op_id = o->op_id;
	cell->op_group = o->group_id;
	cell->flags = o->flags;

	// set normarg-related things
	if (FMATCH(cell->flags, FL_ARG_NORM)) {
		if (!_C(cell->v)) cell->flags |= FL_2WORD;
		if (_D(cell->v)) cell->flags |= FL_MOD_D;
		if (_B(cell->v)) cell->flags |= FL_MOD_B;
	}

	// fill in short argument value
	if (FMATCH(cell->flags, FL_ARG_SHORT4)) {
		cell->arg_short = _t(cell->v);
	} else if (FMATCH(cell->flags, FL_ARG_SHORT7)) {
		cell->arg_short = _T(cell->v);
	} else if (FMATCH(cell->flags, FL_ARG_SHORT8)) {
		cell->arg_short = _b(cell->v);
		// early fix for BLC argument - needs to be here for proper emdas syntax
		if (cell->op_id == OP_BLC) {
			cell->arg_short <<= 8;
		}
	}

	return 0;
}

// -----------------------------------------------------------------------
int emdas_import_word(struct emdas *emd, uint16_t addr, uint16_t word)
{
	if (!emd) return -1;

	const struct opdef *o = emdas_get_op(word);

	assert(o);

	struct emdas_cell *cell = emd->cells + addr;
	struct emdas_cell *next_cell = emd->cells + (uint16_t)(addr+1);

	// is this an instruction?
	if ((o->op_id > OP_NONE) && (o->op_id < OP_MAX)) {
		emdas_fill_ins(cell, o, addr, word);
		// check if instruction needs another cell for normal argument
		if (FMATCH(cell->flags, FL_ARG_NORM | FL_2WORD)) {
			cell->arg_16 = next_cell;
		}
	// treat as data
	} else {
		emdas_fill_data(cell, addr, word);
	}

	return 1;
}

// -----------------------------------------------------------------------
int emdas_import_tab(struct emdas *emd, uint16_t addr, int size, uint16_t *data)
{
	if (!emd) return -1;

	int offset;

	for (offset=0 ; offset<size ; offset++) {
		emdas_import_word(emd, addr+offset, data[offset]);
	}

	return offset;
}

// -----------------------------------------------------------------------
int emdas_import_stream(struct emdas *emd, uint16_t addr, int size, FILE *stream)
{
	if (!emd) return -1;

	int read_size = -1;

	uint16_t *data = malloc(size * sizeof(uint16_t));
	if (!data) {
		goto cleanup;
	}

	read_size = fread(data, sizeof(uint16_t), size, stream);
	if (read_size < 0) {
		goto cleanup;
	}

	for (int offset=0 ; offset<read_size ; offset++) {
		emdas_import_word(emd, addr+offset, ntohs(data[offset]));
	}

cleanup:
	free(data);
	return read_size;
}
/*
// -----------------------------------------------------------------------
int emdas_analyze(struct emdas *emd)
{
	struct emdas_cell *cell = emd->cells;

	if (!cell) {
		return -1;
	}

	for (int offset=0 ; offset<emd->size ; offset++, cell++) {

		// correction for I/O address arguments
		if (FMATCH(cell->flags, FL_INS_IO)) {
			emdas_update_type(emd, offset+2, CELL_DATA);
			emdas_update_type(emd, offset+3, CELL_DATA);
			emdas_update_type(emd, offset+4, CELL_DATA);
			if (FMATCH(cell->flags, FL_2WORD)) {
				emdas_update_type(emd, offset+5, CELL_DATA);
			} else {
				emdas_update_type(emd, offset+1, CELL_DATA);
			}

		// correction for long and fp address arguments
		} else if (cell->arg_16
			&& FMATCH(cell->flags, FL_ARG_NORM | FL_2WORD)
			&& FNONE(cell->flags, FL_MOD_B | FL_MOD_D | FL_PREMOD)
		) {
			if (FMATCH(cell->flags, FL_ARG_A_DWORD)) {
				emdas_update_type(emd, cell->arg_16->v, CELL_DATA);
				emdas_update_type(emd, cell->arg_16->v+1, CELL_DATA);
			} else if (FMATCH(cell->flags, FL_ARG_A_FLOAT)) {
				emdas_update_type(emd, cell->arg_16->v, CELL_DATA);
				emdas_update_type(emd, cell->arg_16->v+1, CELL_DATA);
				emdas_update_type(emd, cell->arg_16->v+2, CELL_DATA);
			}
		}
	}
	return 0;
}
*/
// -----------------------------------------------------------------------
static int emdas_normarg_format(char *buf, int maxlen, char **elem_format, struct emdas_cell *cell, int use_name)
{
	assert(buf && elem_format && cell);

	int pos = 0;

	// D-modification is present
	if (FMATCH(cell->flags, FL_MOD_D)) {
		pos += snprintf(buf+pos, maxlen-pos, "[");
	}

	// rC != 0, print it
	if (FNONE(cell->flags, FL_2WORD)) {
		pos += snprintf(buf+pos, maxlen-pos, elem_format[SYN_ELEM_REG], _C(cell->v));
	// rC == 0, value in 2nd arg
	} else {
		// TODO: handle arg split from op by label on arg
		if (use_name && cell->arg_name) {
			pos += snprintf(buf+pos, maxlen-pos, "%s", cell->arg_name);
		} else if (!cell->arg_16) {
			pos += snprintf(buf+pos, maxlen-pos, "???");
		} else {
			if (cell->arg_16->v == 0) {
				pos += snprintf(buf+pos, maxlen-pos, "0");
			} else {
				pos += snprintf(buf+pos, maxlen-pos, elem_format[SYN_ELEM_ARG_16], cell->arg_16->v);
			}
		}
	}

	// rB is present, B-modification
	if (FMATCH(cell->flags, FL_MOD_B)) {
		pos += snprintf(buf+pos, maxlen-pos, "+");
		pos += snprintf(buf+pos, maxlen-pos, elem_format[SYN_ELEM_REG], _B(cell->v));
	}

	// D-modification is present
	if (FMATCH(cell->flags, FL_MOD_D)) {
		pos += snprintf(buf+pos, maxlen-pos, "]");
	}

	return pos;
}

// -----------------------------------------------------------------------
static int emdas_format(char *buf, int buf_len, char **elem_format, const char *instr_format, struct emdas_cell *cell)
{
	assert(buf && elem_format && instr_format && cell);

	char *bcur;
	int pos = 0;
	int bmax;
	int esc = 0;

	while (instr_format && *instr_format) {
		bmax = buf_len-pos;
		bcur = buf+pos;
		if (esc) { // processing esc seq
			switch (*instr_format) {
				case '%': // literal %
					pos += snprintf(bcur, bmax, "%%");
					break;
				case 'm': // mnemonic
					pos += snprintf(bcur, bmax, elem_format[SYN_ELEM_MNEMO], emdas_ilist[cell->op_id]);
					break;
				case 'r': // register A
					pos += snprintf(bcur, bmax, elem_format[SYN_ELEM_REG], _A(cell->v));
					break;
				case 't': // short argument (7-bit) (using label name, if available)
					if (cell->arg_name) {
						pos += snprintf(bcur, bmax, "%s", cell->arg_name);
					} else {
						pos += snprintf(bcur, bmax, elem_format[SYN_ELEM_ARG_7], cell->arg_short);
					}
					break;
				case 'T': // short argument (7-bit) (numeric)
					pos += snprintf(bcur, bmax, elem_format[SYN_ELEM_ARG_7], cell->arg_short);
					break;
				case 'v': // very short argument (4-bit, SHC only)
					pos += snprintf(bcur, bmax, elem_format[SYN_ELEM_ARG_4], cell->arg_short);
					break;
				case 'B': // byte argument (8-bit)
					pos += snprintf(bcur, bmax, elem_format[SYN_ELEM_ARG_8], cell->arg_short);
					break;
				case 'n': // normal argument (using argument name, if available)
					pos += emdas_normarg_format(bcur, bmax, elem_format, cell, 1);
					break;
				case 'N': // normal argument (numeric)
					pos += emdas_normarg_format(bcur, bmax, elem_format, cell, 0);
					break;
				case 'd':
					if (cell->v == 0) {
						pos += snprintf(bcur, bmax, "0");
					} else {
						pos += snprintf(bcur, bmax, elem_format[SYN_ELEM_ARG_16], cell->v);
					}
					break;
				default: // print out unknown escape sequence
					pos += snprintf(bcur, bmax, "%%%c", *instr_format);
					break;
			}
			esc = 0;
		} else if (*instr_format == '%') { // start esc seq
			esc = 1;
		} else { // literal
			pos += snprintf(bcur, bmax, "%c", *instr_format);
		}
		instr_format++;
	}

	return pos;
}

// -----------------------------------------------------------------------
static char * emdas_make_text(struct emdas_cell *cell, char **elem_format, unsigned features)
{
	assert(cell && elem_format);

	char buf[EMDAS_LINE_MAX+1];
	int pos = 0;

	int flags = cell->flags;
	int fmt = SYN_INS_DATA; // by default, treat as data

	// address
	if ((features & FEAT_ADDR)) {
		pos += snprintf(buf+pos, EMDAS_LINE_MAX-pos, elem_format[SYN_ELEM_ADDR], cell->addr);
	}

	// label
	if ((features & FEAT_LABELS)) {
		if (cell->label) {
			int len = strlen(cell->label) + 2;
			char *tlabel = malloc(len);
			snprintf(tlabel, len, "%s:", cell->label);
			if (tlabel) {
				pos += snprintf(buf+pos, EMDAS_LINE_MAX-pos, elem_format[SYN_ELEM_LABEL], tlabel);
			} else {
				pos += snprintf(buf+pos, EMDAS_LINE_MAX-pos, "%s:", cell->label);
			}
			free(tlabel);
		} else {
			pos += snprintf(buf+pos, EMDAS_LINE_MAX-pos, elem_format[SYN_ELEM_LABEL], "");
		}
	}

	// instruction/data
	if (cell->type == CELL_INS) {
		// no arguments
		if ((flags & FL_ARG_NONE)) {
			fmt = SYN_INS__;
		// register argument is present
		} else if ((flags & FL_ARG_REG)) {
			if ((flags & FL_ARG_NORM)) {
				fmt = SYN_INS_RN;
			} else if ((flags & FL_ARG_SHORT7)) {
				fmt = SYN_INS_RT;
			} else if ((flags & FL_ARG_SHORT4)) {
				fmt = SYN_INS_RV;
			} else if ((flags & FL_ARG_SHORT8)) {
				assert(fmt != SYN_INS_DATA); // impossibru
			} else {
				fmt = SYN_INS_R;
			}
		// normal argument is present
		} else if ((flags & FL_ARG_NORM)) {
			fmt = SYN_INS_N;
		// short arguments...
		} else if ((flags & FL_ARG_SHORT7)) {
			fmt = SYN_INS_T;
		} else if ((flags & FL_ARG_SHORT8)) {
			fmt = SYN_INS_B;
		}
	}

	pos += emdas_format(buf+pos, EMDAS_LINE_MAX-pos, elem_format, emdas_ins_format[fmt], cell);

	buf[pos] = '\0';

	char *str = malloc(pos+1);
	if (str) {
		memcpy(str, buf, pos+1);
	}

	return str;
}

// -----------------------------------------------------------------------
int __emdas_dump_cell(FILE *f, struct emdas_cell *cell)
{
	static char *cell_type_names[] = {
		 "DATA", "OP", "ARG"
	};
	static char *op_group_names[] = {
		"NONE", "NORM", "FD", "KA1", "JS", "KA2", "C", "S", "J", "L", "G", "BN"
	};

	fprintf(f, "---------------------------------------------\n");
	fprintf(f, "Address    : %i\n", cell->addr);
	fprintf(f, "Cell type  : %s\n", cell_type_names[cell->type]);
	fprintf(f, "Cell label : %s\n", cell->label);
	fprintf(f, "Cell value : 0x%04x\n", cell->v);
	fprintf(f, "OP group   : %s\n", op_group_names[cell->op_group]);
	fprintf(f, "OP ID      : %i\n", cell->op_id);
	fprintf(f, "Mnemo      : %s\n", emdas_ilist[cell->op_id]);
	fprintf(f, "Registers  : rA = %i, rB = %i, rC = %i\n", _A(cell->v), _B(cell->v), _C(cell->v));
	fprintf(f, "Arg. name  : %s\n", cell->arg_name);
	fprintf(f, "Arg. short : %i\n", cell->arg_short);
	if (cell->arg_16) {
		fprintf(f, "Arg. 16    : %i\n", cell->arg_16->v);
	} else {
		fprintf(f, "Arg. 16    : NULL\n");
	}
	fprintf(f, "Flags      : %s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s\n",
		(cell->flags & FL_INS_OS) ? "OS, " : "",
		(cell->flags & FL_INS_IO) ? "IO, " : "",
		(cell->flags & FL_INS_MX16) ? "MX16, " : "",

		(cell->flags & FL_ARG_NONE) ? "NONE, " : "",
		(cell->flags & FL_ARG_REG) ? "REG, " : "",
		(cell->flags & FL_ARG_REGIND) ? "REGIND, " : "",
		(cell->flags & FL_ARG_SHORT4) ? "SHORT4, " : "",
		(cell->flags & FL_ARG_SHORT7) ? "SHORT7, " : "",
		(cell->flags & FL_ARG_SHORT8) ? "SHORT8, " : "",
		(cell->flags & FL_ARG_RELATIVE) ? "RELATIVE, " : "",
		(cell->flags & FL_ARG_NORM) ? "NORM, " : "",

		(cell->flags & FL_2WORD) ? "2WORD, " : "",
		(cell->flags & FL_MOD_D) ? "MOD_D, " : "",
		(cell->flags & FL_MOD_B) ? "MOD_B, " : "",
		(cell->flags & FL_PREMOD) ? "PREMOD, " : "",

		(cell->flags & FL_ARG_A_JUMP) ? "A_JUMP, " : "",
		(cell->flags & FL_ARG_A_BYTE) ? "A_BYTE, " : "",
		(cell->flags & FL_ARG_A_WORD) ? "A_WORD, " : "",
		(cell->flags & FL_ARG_A_DWORD) ? "A_DWORD, " : "",
		(cell->flags & FL_ARG_A_FLOAT) ? "A_FLOAT, " : ""
	);
	fprintf(f, "Disassm    : %s\n", cell->text);

	return 0;
}


// vim: tabstop=4 shiftwidth=4 autoindent

