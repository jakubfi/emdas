//  Copyright (c) 2012-2014 Jakub Filipowicz <jakubf@gmail.com>
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
#include <stdio.h>
#include <limits.h>
#include <emelf.h>

#define EMDAS_LINE_MAX 1024

// --- opcodes -----------------------------------------------------------

// opcode identifiers (also act as indexes for opcode names)

enum emdas_op_ids {
			OP_NONE = 0,
/* NORM */	OP_LW, OP_TW, OP_LS, OP_RI, OP_RW, OP_PW, OP_RJ, OP_IS,
			OP_BB, OP_BM, OP_BS, OP_BC, OP_BN, OP_OU, OP_IN,
/* F/D */	OP_AD, OP_SD, OP_MW, OP_DW, OP_AF, OP_SF, OP_MF, OP_DF,
/* NORM */	OP_AW, OP_AC, OP_SW, OP_CW, OP_OR, OP_OM, OP_NR, OP_NM,
			OP_ER, OP_EM, OP_XR, OP_XM, OP_CL, OP_LB, OP_RB, OP_CB,
/* KA1 */	OP_AWT, OP_TRB, OP_IRB, OP_DRB, OP_CWT, OP_LWT, OP_LWS, OP_RWS,
/* JS */	OP_UJS, OP_NOP, OP_JLS, OP_JES, OP_JGS, OP_JVS, OP_JXS, OP_JYS, OP_JCS,
/* KA2 */	OP_BLC, OP_EXL, OP_BRC, OP_NRF,
/* C */		OP_RIC, OP_ZLB, OP_SXU, OP_NGA, OP_SLZ, OP_SLY, OP_SLX, OP_SRY,
			OP_NGL, OP_RPC, OP_SHC, OP_RKY, OP_ZRB, OP_SXL, OP_NGC, OP_SVZ,
			OP_SVY, OP_SVX, OP_SRX, OP_SRZ, OP_LPC,
/* S */		OP_HLT, OP_MCL, OP_CIT, OP_SIL, OP_SIU, OP_SIT, OP_GIU, OP_LIP, OP_GIL,
			OP_CRON, OP_SINT, OP_SIND,
/* J */		OP_UJ, OP_JL, OP_JE, OP_JG, OP_JZ, OP_JM, OP_JN, OP_LJ,
/* L */		OP_LD, OP_LF, OP_LA, OP_LL, OP_TD, OP_TF, OP_TA, OP_TL,
/* G */		OP_RD, OP_RF, OP_RA, OP_RL, OP_PD, OP_PF, OP_PA, OP_PL,
/* B/N */	OP_MB, OP_IM, OP_KI, OP_FI, OP_SP, OP_MD, OP_RZ, OP_IB,
			OP_MAX
};

// --- syntax ------------------------------------------------------------

enum emdas_syn_elem {
	SYN_ELEM_MNEMO,
	SYN_ELEM_REG,
	SYN_ELEM_ARG_7,
	SYN_ELEM_ARG_4,
	SYN_ELEM_ARG_8,
	SYN_ELEM_ARG_16,
	SYN_ELEM_ADDR,
	SYN_ELEM_LABEL,
	SYN_ELEM_MAX
};

// --- ref ---------------------------------------------------------------

struct emdas_ref {
	int type;
	struct emdas_cell *cell;
	struct emdas_ref *next;
};

enum emdas_refs {
	REF_ARG,
	REF_WORD,
	REF_DWORD,
	REF_FLOAT,
	REF_JUMP,
	REF_CALL,
	REF_BRANCH,
	REF_IO_OK,
	REF_IO_EN,
	REF_IO_NO,
	REF_IO_PE,
};

// --- cell --------------------------------------------------------------

struct emdas_cell {
	uint16_t addr;
	uint16_t v;
	uint8_t type;

	uint8_t op_id;
	uint8_t op_group;
	unsigned flags;

	int arg_short;
	char *arg_name;

	char *label;
	int syn_generation;
	char *text;

	struct emdas_ref *ref;
	struct emdas_ref *rref;
};

// cell types

enum emdas_cell_types {
	CELL_DATA,	// cell does not match any opcode (likely data)
	CELL_INS,	// cell matches an opcode (likely instruction)
	CELL_ARG,	// cell contains additional instruction argument
};

// opcode groups

enum emdas_op_groups {
	OP_GR_NONE, // unknown group
	OP_GR_NORM,
	OP_GR_FD,
	OP_GR_KA1,
	OP_GR_JS,
	OP_GR_KA2,
	OP_GR_C,
	OP_GR_S,
	OP_GR_J,
	OP_GR_L,
	OP_GR_G,
	OP_GR_BN,
};

// cell flags

enum emdas_flags {
	FL_NONE			= 0,

	// instruction flags
	FL_INS_OS		= 1 << 0,	// instruction is user-illegal (OS level only)
	FL_INS_IO		= 1 << 1,	// I/O instruction
	FL_INS_MX16		= 1 << 2,	// additional MX-16 instruction

	// argument flags
	FL_ARG_NONE		= 1 << 3,	// no arguments
	FL_ARG_REG		= 1 << 4,	// rA register argument is present
	FL_ARG_REGIND	= 1 << 5,	// rA register indirect addressing
	FL_ARG_SHORT4	= 1 << 6,	// 4-bit short argument is present (SHC only)
	FL_ARG_SHORT7	= 1 << 7,	// 7-bit short argument is present
	FL_ARG_SHORT8	= 1 << 8,	// byte argument is present
	FL_ARG_RELATIVE	= 1 << 9,	// short argument is PC-relative
	FL_ARG_NORM		= 1 << 10,	// normal argument is present

	// normarg flags
	FL_2WORD		= 1 << 11,	// normal argument uses additional word (rC=0)
	FL_MOD_D		= 1 << 12,	// normal argument is D-modified
	FL_MOD_B		= 1 << 13,	// normal argument is B-modified
	FL_PREMOD		= 1 << 14,	// instruction is premodified

	// normarg address flags
	FL_ARG_A_JUMP	= 1 << 15,	// argument is a jump address
	FL_ARG_A_BYTE	= 1 << 16,	// argument is a byte address
	FL_ARG_A_WORD	= 1 << 17,	// argument is a word address
	FL_ARG_A_DWORD	= 1 << 18,	// argument is a dword address
	FL_ARG_A_FLOAT	= 1 << 19,	// argument is a float address

};

// flags convenience macros

#define FL_ARG_SHORT (FL_ARG_SHORT4 | FL_ARG_SHORT7 | FL_ARG_SHORT8)
#define FL_ARG_IMMEDIATE (FL_ARG_SHORT | FL_ARG_2WORD)
#define FL_ARG_INDIRECT (FL_ARG_A_BYTE | FL_ARG_A_WORD | FL_ARG_A_DWORD | FL_ARG_A_FLOAT)

// --- emdas -------------------------------------------------------------

typedef int (*emdas_getfun)(uint16_t addr);

struct emdas {
	struct emdas_cell cells[64*1024];
	unsigned features;
	int syn_generation;
	char *elem_format[SYN_ELEM_MAX];
};

// --- features ----------------------------------------------------------

enum emdas_features {
	FEAT_NONE	= 0,
	FEAT_ADDR	= 1 << 0,
	FEAT_LABELS	= 1 << 1,
	FEAT_UCASE	= 1 << 2,
};

// features convenience macros
#define FEAT_SYN (FEAT_ADDR | FEAT_LABELS | FEAT_UCASE)
#define FEAT_ALL (FEAT_SYN)

// --- prototypes --------------------------------------------------------

struct emdas * emdas_init();
void emdas_shutdown(struct emdas *emd);

int emdas_set_syntax(struct emdas *emd, unsigned syn_id, const char *syn);
int emdas_reset_syntax(struct emdas *emd);

unsigned emdas_get_features(struct emdas *emd);
int emdas_set_features(struct emdas *emd, unsigned features);

struct emdas_cell * emdas_get_ref(struct emdas_cell *cell, unsigned type);

int emdas_import_word(struct emdas *emd, uint16_t addr, uint16_t word);
int emdas_import_tab(struct emdas *emd, uint16_t addr, int size, uint16_t *tab);
int emdas_import_stream(struct emdas *emd, uint16_t addr, int size, FILE *stream);
int emdas_import_emelf(struct emdas *emd, uint16_t addr, int size, struct emelf *e);

struct emdas_cell * emdas_get_cell(struct emdas *emd, uint16_t addr);
int __emdas_dump_cell(FILE *f, struct emdas_cell *cell);

#endif

// vim: tabstop=4 shiftwidth=4 autoindent
