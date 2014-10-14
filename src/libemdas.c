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
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <inttypes.h>
#include <assert.h>
#include <limits.h>
#include <arpa/inet.h>

#include "emdas.h"
#include "iset.h"

#ifdef DEBUG
static int dtree_level = 0;

// -----------------------------------------------------------------------
static void pdebug(uint16_t addr, char *format, ...)
{
	fprintf(stderr, "debug ^%i @ 0x%04x ", dtree_level, addr);
	for (int i=0 ; i<dtree_level ; i++) {
		fprintf(stderr, "   ");
	}
	va_list ap;
	va_start(ap, format);
	vfprintf(stderr, format, ap);
	fprintf(stderr, "\n");
	va_end(ap);
}

#define PDEBUG(addr, format, ...) pdebug(addr, format, ##__VA_ARGS__)
#else
#define PDEBUG(addr, format, ...) ;
#endif

#define   FANY(v, flags) (((v) & (flags)) != 0)
#define  FNONE(v, flags) (((v) & (flags)) == 0)
#define FMATCH(v, flags) (((v) & (flags)) == (flags))

#define ADDR(cell) (uint16_t)((cell) - emd->cells)
#define CELL(addr) (emd->cells + (uint16_t)(addr))

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
	/* SYN_ELEM_LABEL */	"%17s ", // auto-includes ':'
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

static char * emdas_make_text(struct emdas_cell *cell, uint16_t addr, char **elem_format, unsigned features);
static void emdas_drop_rels(struct emdas_rel *rel);

// Analyzers
static void emdas_analyze_2arg(struct emdas *emd, struct emdas_cell *cell);
static void emdas_analyze_io(struct emdas *emd, struct emdas_cell *cell);
static void emdas_analyze_dword(struct emdas *emd, struct emdas_cell *cell);
static void emdas_analyze_float(struct emdas *emd, struct emdas_cell *cell);
static void emdas_analyze_jump(struct emdas *emd, struct emdas_cell *cell);

typedef void (*emdas_analyzer_fun)(struct emdas *emd, struct emdas_cell *cell);

struct emdas_analyzer {
	const char *name;
	int match;
	int nomatch;
	int required;
	int final;
	emdas_analyzer_fun fun;
};

struct emdas_analyzer emdas_analyzers[] = {
	{
		.name = "2nd arg",
		.match = FL_ARG_NORM | FL_2WORD,
		.nomatch = 0,
		.required = 1,
		.final = 0,
		.fun = emdas_analyze_2arg
	}, {
		.name = "Jump",
		.match = FL_2WORD | FL_ADDR_JUMP,
		.nomatch = FL_MOD_B | FL_MOD_D | FL_MOD_PRE,
		.required = 0,
		.final = 1,
		.fun = emdas_analyze_jump
	}, {
		.name = "Dword",
		.match = FL_2WORD | FL_ADDR_DWORD,
		.nomatch = FL_MOD_B | FL_MOD_D | FL_MOD_PRE,
		.required = 0,
		.final = 1,
		.fun = emdas_analyze_dword
	}, {
		.name = "I/O",
		.match = FL_INS_IO,
		.nomatch = 0,
		.required = 0,
		.final = 1,
		.fun = emdas_analyze_io
	}, {
		.name = "Float",
		.match = FL_2WORD | FL_ADDR_FLOAT,
		.nomatch = FL_MOD_B | FL_MOD_D | FL_MOD_PRE,
		.required = 0,
		.final = 1,
		.fun = emdas_analyze_float
	}, {
		.fun = NULL
	}
};

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

	// analyze by default
	emd->features = FEAT_ANALYZE;

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
		emdas_drop_rels(emd->cells[i].args);
		emdas_drop_rels(emd->cells[i].parents);
		emdas_drop_rels(emd->cells[i].refs);
		emdas_drop_rels(emd->cells[i].referrers);
	}

	free(emd);
}

// -----------------------------------------------------------------------
int emdas_set_syntax(struct emdas *emd, unsigned syn_id, const char *syn)
{
	if (!emd) return -1;
	if (syn_id >= SYN_ELEM_MAX) return -1;
	if (!syn) return -1;

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
	if (!emd) return 0;
	return emd->features;
}

// -----------------------------------------------------------------------
int emdas_set_features(struct emdas *emd, unsigned features)
{
	if (!emd) return -1;
	if ((features & (~FEAT_ALL))) return -1;

	// bump syn generation if a syntax feature has changed
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

	struct emdas_cell *cell = CELL(addr);

	// check if we need to (re-)generate text representation
	if (!cell->text || (cell->syn_generation != emd->syn_generation)) {
		//PDEBUG(addr, "Updating text cache");
		cell->text = emdas_make_text(cell, ADDR(cell), emd->elem_format, emd->features);
	    cell->syn_generation = emd->syn_generation;
	} else {
		//PDEBUG(addr, "Using cached text");
	}

	return cell;
}

// -----------------------------------------------------------------------
static void emdas_fill_data(struct emdas *emd, uint16_t addr)
{
	PDEBUG(addr, "Fill as data");

	assert(emd);
	struct emdas_cell *cell = CELL(addr);

	cell->type = CELL_DATA;

	cell->op_id = OP_NONE;
	cell->op_group = OP_GR_NONE;
	cell->flags = FL_NONE;

	cell->arg_short = INT_MAX;
	cell->arg_name = NULL;
}

// -----------------------------------------------------------------------
static void emdas_fill_arg(struct emdas *emd, uint16_t addr)
{
	assert(emd);
	struct emdas_cell *cell = CELL(addr);

	emdas_fill_data(emd, addr);
	PDEBUG(addr, "Fill as arg");
	cell->type = CELL_ARG;
}

// -----------------------------------------------------------------------
static int emdas_fill_ins(struct emdas *emd, uint16_t addr, const struct opdef *o)
{
	PDEBUG(addr, "Fill as instruction");

	assert(emd && o);
	struct emdas_cell *cell = CELL(addr);

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
static int emdas_add_rel(struct emdas *emd, struct emdas_cell *cell, uint16_t dest_addr, struct emdas_rel **rrel, unsigned type)
{
	assert(emd && cell && rrel);

	struct emdas_rel *rel = malloc(sizeof(struct emdas_rel));
	if (!rel) return -1;
	rel->type = type;
	rel->cell = CELL(dest_addr);
	rel->next = (*rrel);
	(*rrel) = rel;
	return 0;
}

// -----------------------------------------------------------------------
static int emdas_add_arg(struct emdas *emd, struct emdas_cell *cell, uint16_t dest_addr, unsigned type)
{
	PDEBUG(ADDR(cell), "Added arg rel -> 0x%04x (type: %i)", dest_addr, type);
	if (emdas_add_rel(emd, cell, dest_addr, &(cell->args), type)) return -1;
	if (emdas_add_rel(emd, CELL(dest_addr), ADDR(cell), &(CELL(dest_addr)->parents), type)) return -1;
	return 0;
}

// -----------------------------------------------------------------------
static int emdas_add_ref(struct emdas *emd, struct emdas_cell *cell, uint16_t dest_addr, unsigned type)
{
	PDEBUG(ADDR(cell), "Added ref -> 0x%04x (type: %i)", dest_addr, type);
	if (emdas_add_rel(emd, cell, dest_addr, &(cell->refs), type)) return -1;
	if (emdas_add_rel(emd, CELL(dest_addr), ADDR(cell), &(CELL(dest_addr)->referrers), type)) return -1;
	return 0;
}

// -----------------------------------------------------------------------
struct emdas_cell * emdas_get_rel(struct emdas_rel *r, unsigned type)
{
	if (!r) return NULL;

	while (r) {
		if (r->type == type) {
			return r->cell;
		}
		r = r->next;
	}

	return NULL;
}

// -----------------------------------------------------------------------
static void emdas_drop_rel(struct emdas_rel **rel, struct emdas_cell *cell)
{
	struct emdas_rel **r = rel;
	struct emdas_rel *prev = NULL;

	assert(rel && cell);

	while (r && *r) {
		if ((*r)->cell == cell) {
			if (prev) {
				prev->next = (*r)->next;
				free(*r);
			} else {
				free(*r);
				*r = NULL;
			}
		}
	}
}

// -----------------------------------------------------------------------
static void emdas_drop_rels(struct emdas_rel *rel)
{
	struct emdas_rel *r = rel;
	struct emdas_rel *nextr;
	while (r) {
		nextr = r->next;
		free(r);
		r = nextr;
	}
}

// -----------------------------------------------------------------------
static void emdas_analyze_2arg(struct emdas *emd, struct emdas_cell *cell)
{
	emdas_fill_arg(emd, ADDR(cell)+1);
	emdas_add_arg(emd, cell, ADDR(cell)+1, ARG_2ARG);
}

// -----------------------------------------------------------------------
static void emdas_analyze_io(struct emdas *emd, struct emdas_cell *cell)
{
	int ao = ADDR(cell) + 1;
	if (FMATCH(cell->flags, FL_2WORD)) ao++;

	emdas_fill_data(emd, ao+0);
	emdas_fill_data(emd, ao+1);
	emdas_fill_data(emd, ao+2);
	emdas_fill_data(emd, ao+3);
	emdas_add_arg(emd, cell, ao+0, ARG_IO_NO);
	emdas_add_arg(emd, cell, ao+1, ARG_IO_EN);
	emdas_add_arg(emd, cell, ao+2, ARG_IO_OK);
	emdas_add_arg(emd, cell, ao+3, ARG_IO_PE);
}

// -----------------------------------------------------------------------
static void emdas_analyze_dword(struct emdas *emd, struct emdas_cell *cell)
{
	struct emdas_cell *arg = emdas_get_rel(cell->args, ARG_2ARG);
	assert(arg);
	emdas_fill_data(emd, arg->v+0);
	emdas_fill_data(emd, arg->v+1);
}

// -----------------------------------------------------------------------
static void emdas_analyze_float(struct emdas *emd, struct emdas_cell *cell)
{
	struct emdas_cell *arg = emdas_get_rel(cell->args, ARG_2ARG);
	assert(arg);
	emdas_fill_data(emd, arg->v+0);
	emdas_fill_data(emd, arg->v+1);
	emdas_fill_data(emd, arg->v+2);
}

// -----------------------------------------------------------------------
static void emdas_analyze_jump(struct emdas *emd, struct emdas_cell *cell)
{
	struct emdas_cell *arg = emdas_get_rel(cell->args, ARG_2ARG);
	assert(arg);
	emdas_add_ref(emd, cell, arg->v, REF_JUMP);
}

// -----------------------------------------------------------------------
static void emdas_analyze(struct emdas *emd, struct emdas_cell *cell)
{
	assert(emd && cell);

	const struct opdef *o = emdas_get_op(cell->v);
	assert(o);

	// is this an instruction?
	if (o->op_id != OP_NONE) {
		PDEBUG(ADDR(cell), "ANALYZE: OP, value 0x%04x (%s)", cell->v, emdas_ilist_lcase[o->op_id]);
		emdas_fill_ins(emd, ADDR(cell), o);

		// run analyzers
		struct emdas_analyzer *an = emdas_analyzers;
		while (an && an->fun) {
			if ((emd->features & FEAT_ANALYZE) || an->required) {
				if (FMATCH(cell->flags, an->match) && FNONE(cell->flags, an->nomatch)) {
					PDEBUG(ADDR(cell), "Running analyzer: %s", an->name);
					an->fun(emd, cell);
					if (an->final) break;
				} else {
					PDEBUG(ADDR(cell), "Unmatched analyzer: %s", an->name);
				}
			} else {
				PDEBUG(ADDR(cell), "Skipped analyzer: %s", an->name);
			}
			an++;
		}

	// treat cell as data
	} else {
		PDEBUG(ADDR(cell), "ANALYZE: DATA, value 0x%04x", cell->v);
		emdas_fill_data(emd, ADDR(cell));
	}
}

// -----------------------------------------------------------------------
static void emdas_process(struct emdas *emd, struct emdas_cell *cell)
{
	assert(emd && cell);
	assert((cell->op_id >= OP_NONE) && (cell->op_id < OP_MAX));

#ifdef DEBUG
	dtree_level++;
#endif

	struct emdas_rel *rel_stored;
	struct emdas_rel *rel;

	// cell has parents - start analysis from each parent down
	if (cell->parents) {
		PDEBUG(ADDR(cell), "---- CELL HAS PARENTS ----");

		// store all parents, nullify all parents so we don't loop
		rel_stored = rel = cell->parents;
		cell->parents = NULL;

		// process each parent
		while (rel) {
			PDEBUG(ADDR(cell), "parent @ 0x%04x (type %i)", ADDR(rel->cell), rel->type);
			emdas_process(emd, rel->cell);
			rel = rel->next;
		}

		emdas_drop_rels(rel_stored);
		PDEBUG(ADDR(cell), "---- PARENTS DONE ----");

	// cell has no parents (we're at the top cell, start processing)
	} else {
		// if cell has arguments, we need to reprocess them
		// (args can be no longer args after we process the parent)
		if (cell->args) {
			PDEBUG(ADDR(cell), "---- CELL HAS ARGS ----");

			// store all args, nullify all args (we don't need those anymore, we're gonna reprocess them)
			rel_stored = rel = cell->args;
			cell->args = NULL;

			// process each arg
			while (rel) {
				PDEBUG(ADDR(cell), "arg -> 0x%04x (type %i)", ADDR(rel->cell), rel->type);
				// drop relation to parent so we don't loop
				emdas_drop_rel(&rel->cell->parents, cell);
				emdas_process(emd, rel->cell);
				rel = rel->next;
			}

			emdas_drop_rels(rel_stored);
			PDEBUG(ADDR(cell), "---- ARGS DONE ----");
		}

		// if cell has refs, we need to remove them
		// (refs may not be refs anymore after reprocessing)
		if (cell->refs) {
			PDEBUG(ADDR(cell), "---- CELL HAS REFS ----");

			rel = cell->refs;
			while (rel) {
				PDEBUG(ADDR(cell), "ref -> 0x%04x (type %i)", ADDR(rel->cell), rel->type);
				// remove referrer from ref
				emdas_drop_rel(&rel->cell->referrers, cell);
				rel = rel->next;
			}

			// delete all cell's refs
			emdas_drop_rels(cell->refs);
			cell->refs = NULL;

			PDEBUG(ADDR(cell), "---- REFS DONE ----");
		}

		// finally, analyze the parent
		// (no need to process recursively, there should be no relations at this point)
		emdas_analyze(emd, cell);
	}

	//__emdas_dump_cell(stderr, cell);

#ifdef DEBUG
	dtree_level--;
#endif
}

// -----------------------------------------------------------------------
int emdas_import_word(struct emdas *emd, uint16_t addr, uint16_t word)
{
	if (!emd) return -1;

	struct emdas_cell *cell = CELL(addr);

	PDEBUG(addr, "---- IMPORT value: 0x%04x --------------------------------", word);

	// text representation needs to be redone
	free(cell->text);
	cell->text = NULL;
	cell->v = word;
	emdas_process(emd, cell);

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
		struct emdas_cell *arg = emdas_get_rel(cell->args, ARG_2ARG);
		// TODO: handle instruction/arg split by a label
		if (use_name && cell->arg_name) {
			pos += snprintf(buf+pos, maxlen-pos, "%s", cell->arg_name);
		} else if (!arg) {
			pos += snprintf(buf+pos, maxlen-pos, "???");
		} else {
			if (arg->v == 0) {
				pos += snprintf(buf+pos, maxlen-pos, "0");
			} else {
				pos += snprintf(buf+pos, maxlen-pos, elem_format[SYN_ELEM_ARG_16], arg->v);
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
static char * emdas_make_text(struct emdas_cell *cell, uint16_t addr, char **elem_format, unsigned features)
{
	assert(cell && elem_format);
	char buf[EMDAS_LINE_MAX+1];
	int pos = 0;

	int flags = cell->flags;
	int fmt = SYN_INS_DATA; // by default, treat as data

	// address
	if ((features & FEAT_ADDR)) {
		pos += snprintf(buf+pos, EMDAS_LINE_MAX-pos, elem_format[SYN_ELEM_ADDR], addr);
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
		if (FMATCH(flags, FL_ARG_NONE)) {
			fmt = SYN_INS__;
		// register argument is present
		} else if (FMATCH(flags, FL_ARG_REG)) {
			if (FMATCH(flags, FL_ARG_NORM)) {
				fmt = SYN_INS_RN;
			} else if (FMATCH(flags, FL_ARG_SHORT7)) {
				fmt = SYN_INS_RT;
			} else if (FMATCH(flags, FL_ARG_SHORT4)) {
				fmt = SYN_INS_RV;
			} else if (FMATCH(flags, FL_ARG_SHORT8)) {
				assert(fmt != SYN_INS_DATA); // impossibru
			} else {
				fmt = SYN_INS_R;
			}
		// normal argument is present
		} else if (FMATCH(flags, FL_ARG_NORM)) {
			fmt = SYN_INS_N;
		// short arguments...
		} else if (FMATCH(flags, FL_ARG_SHORT7)) {
			fmt = SYN_INS_T;
		} else if (FMATCH(flags, FL_ARG_SHORT8)) {
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
	fprintf(f, "Cell type  : %s\n", cell_type_names[cell->type]);
	fprintf(f, "Cell label : %s\n", cell->label);
	fprintf(f, "Cell value : 0x%04x\n", cell->v);
	fprintf(f, "OP group   : %s\n", op_group_names[cell->op_group]);
	fprintf(f, "OP ID      : %i\n", cell->op_id);
	fprintf(f, "Mnemo      : %s\n", emdas_ilist[cell->op_id]);
	fprintf(f, "Registers  : rA = %i, rB = %i, rC = %i\n", _A(cell->v), _B(cell->v), _C(cell->v));
	fprintf(f, "Arg. name  : %s\n", cell->arg_name);
	fprintf(f, "Arg. short : %i\n", cell->arg_short);
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
		(cell->flags & FL_MOD_PRE) ? "PREMOD, " : "",

		(cell->flags & FL_ADDR_JUMP) ? "A_JUMP, " : "",
		(cell->flags & FL_ADDR_BYTE) ? "A_BYTE, " : "",
		(cell->flags & FL_ADDR_WORD) ? "A_WORD, " : "",
		(cell->flags & FL_ADDR_DWORD) ? "A_DWORD, " : "",
		(cell->flags & FL_ADDR_FLOAT) ? "A_FLOAT, " : ""
	);
	fprintf(f, "Disassm    : %s\n", cell->text);

	return 0;
}


// vim: tabstop=4 shiftwidth=4 autoindent