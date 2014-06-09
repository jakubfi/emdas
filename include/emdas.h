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

#define EMDAS_LINE_MAX 4 * 1024

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

// cell types

enum emdas_cell_types {
	CELL_UNKNOWN	= 0, // cell hasn't been processed yet, all cell data is invalid
	CELL_NA			= 1, // cell could not be processed (address not available)
	CELL_DATA		= 2, // cell does not match any opcode (likely data)
	CELL_INS			= 3, // cell matches an opcode (likely instruction)
	CELL_ARG		= 4, // cell contains additional instruction argument
};

// opcode groups

enum emdas_op_groups {
	OP_GR_NONE = 0,	// unknown group
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

// convenient opcode flags highlighting cases that may require additional treatment

enum emdas_op_flags {
	OP_FL_NONE		= 0,		// not an instruction or nothing special
	OP_FL_OS		= 1 << 1,	// instruction is user-illegal (OS level only)
	OP_FL_IO		= 1 << 2,	// I/O instruction
	OP_FL_MX16		= 1 << 3,	// additional MX-16 instruction
};

// instruction arguments description

enum emdas_arg_flags {
	ARG_UNKNOWN		= 0,		// cell is not a known instruction
	ARG_NONE		= 1 << 0,	// no arguments
	ARG_REG			= 1 << 1,	// rA register argument is present
	ARG_REGIND		= 1 << 2,	// rA register indirect addressing
	ARG_SHORT4		= 1 << 2,	// 4-bit short argument is present (SHC only)
	ARG_SHORT7		= 1 << 3,	// 7-bit short argument is present
	ARG_SHORT8		= 1 << 4,	// byte argument is present
	ARG_RELATIVE	= 1 << 5,	// short argument is PC-relative
	ARG_NORM		= 1 << 6,	// normal argument is present
	ARG_A_JUMP		= 1 << 7,	// argument is a jump address
	ARG_A_BYTE		= 1 << 8,	// argument is a byte address
	ARG_A_WORD		= 1 << 9,	// argument is a word address
	ARG_A_DWORD		= 1 << 10,	// argument is a dword address
	ARG_A_FLOAT		= 1 << 11,	// argument is a float address
// filled at runtime:
	ARG_2WORD		= 1 << 12,	// normal argument uses additional word (rC=0)
	ARG_MOD_D		= 1 << 13,	// normal argument is D-modified
	ARG_MOD_B		= 1 << 14,	// normal argument is B-modified
};

// convenience macros:

#define ARG_SHORT (ARG_SHORT4 | ARG_SHORT7 | ARG_SHORT8)
#define ARG_ADDR (ARG_A_JUMP | ARG_A_BYTE | ARG_A_WORD | ARG_A_DWORD | ARG_A_FLOAT)
#define ARG_IMM (ARG_SHORT4 | ARG_SHORT7 | ARG_SHORT8 | ARG_2WORD)

// syntax elements identifiers

enum emdas_syn_elem {
	SYN_ELEM_MNEMO = 0,
	SYN_ELEM_REG,
	SYN_ELEM_ARG_7,
	SYN_ELEM_ARG_4,
	SYN_ELEM_ARG_8,
	SYN_ELEM_ARG_16,
	SYN_ELEM_MAX
};

struct emdas_cell {
	uint16_t addr;
	uint16_t v;
	int type;
	int len;

	int ra, rb, rc;

	int op_id;
	int op_group;
	unsigned op_flags;
	unsigned arg_flags;

	int arg_short;
	struct emdas_cell *arg_16;
	char *arg_name;

	char *label;
};

typedef int (*get_word_f)(uint16_t addr);

struct emdas {
	get_word_f get_word;
	char *emdas_elem_format[SYN_ELEM_MAX];
	char buf[EMDAS_LINE_MAX+1];
};

struct emdas * emdas_init(get_word_f get_word);
void emdas_shutdown(struct emdas *emd);

struct emdas_cell * emdas_dasm(struct emdas *emd, uint16_t start_addr, int word_count);
void __emdas_cell_dump(FILE *f, struct emdas *emd, struct emdas_cell *cell);
char * emdas_make_text(struct emdas *emd, struct emdas_cell *cell);
char * emdas_analyze(struct emdas *emd, struct emdas_cell *cell, int cell_count);


#endif

// vim: tabstop=4 shiftwidth=4 autoindent
