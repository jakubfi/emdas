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
#include <inttypes.h>

#include "emdas/iset.h"
#include "opfields.h"

char *emdas_iset_mnemo[EMD_OP_MAX] = {
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

// varmasks tell which bits contain variable instruction elements
enum emdas_iset_var_masks {
	VARMASK_ALL		= 0b1111111111111111,
	VARMASK_DABC	= 0b0000001111111111,
	VARMASK_DAC		= 0b0000001111000111,
	VARMASK_DBC		= 0b0000001000111111,
	VARMASK_DB1C	= 0b0000001000111100,
	VARMASK_B23C	= 0b0000000000111100,
	VARMASK_DB		= 0b0000001000111000,
	VARMASK_BYTE	= 0b0000000011111111,
	VARMASK_BC		= 0b0000000000111111,
	VARMASK_A		= 0b0000000111000000,
	VARMASK_B		= 0b0000000000111000,
};

#define O(x) ((x) << 10)

struct emdas_instr {
	uint16_t opcode;	// instruction opcode (and extended opcode)
	uint16_t var_mask;	// variable bits mask
	struct emdas_op op;	// opcode definition
};

static struct emdas_instr emdas_ilist[] = {
	{ 0, VARMASK_ALL, { EMD_OP_NONE, EMD_FL_NONE } }, // ilegal instructions

	{ O(020), VARMASK_DABC, { EMD_OP_LW, EMD_FL_ARG_REG | EMD_FL_ARG_NORM } },
	{ O(021), VARMASK_DABC, { EMD_OP_TW, EMD_FL_ARG_REG | EMD_FL_ARG_NORM | EMD_FL_ADDR_WORD } },
	{ O(022), VARMASK_DABC, { EMD_OP_LS, EMD_FL_ARG_REG | EMD_FL_ARG_NORM } },
	{ O(023), VARMASK_DABC, { EMD_OP_RI, EMD_FL_ARG_REG | EMD_FL_ARG_NORM | EMD_FL_ARG_REGIND } },
	{ O(024), VARMASK_DABC, { EMD_OP_RW, EMD_FL_ARG_REG | EMD_FL_ARG_NORM | EMD_FL_ADDR_WORD } },
	{ O(025), VARMASK_DABC, { EMD_OP_PW, EMD_FL_ARG_REG | EMD_FL_ARG_NORM | EMD_FL_ADDR_WORD } },
	{ O(026), VARMASK_DABC, { EMD_OP_RJ, EMD_FL_ARG_REG | EMD_FL_ARG_NORM | EMD_FL_ADDR_CALL } },
	{ O(027), VARMASK_DABC, { EMD_OP_IS, EMD_FL_ARG_REG | EMD_FL_ARG_NORM | EMD_FL_ADDR_JUMP } },
	{ O(030), VARMASK_DABC, { EMD_OP_BB, EMD_FL_ARG_REG | EMD_FL_ARG_NORM } },
	{ O(031), VARMASK_DABC, { EMD_OP_BM, EMD_FL_ARG_REG | EMD_FL_ARG_NORM | EMD_FL_ADDR_JUMP } },
	{ O(032), VARMASK_DABC, { EMD_OP_BS, EMD_FL_ARG_REG | EMD_FL_ARG_NORM } },
	{ O(033), VARMASK_DABC, { EMD_OP_BC, EMD_FL_ARG_REG | EMD_FL_ARG_NORM } },
	{ O(034), VARMASK_DABC, { EMD_OP_BN, EMD_FL_ARG_REG | EMD_FL_ARG_NORM } },
	{ O(035), VARMASK_DABC, { EMD_OP_OU, EMD_FL_ARG_REG | EMD_FL_ARG_NORM | EMD_FL_INS_OS | EMD_FL_INS_IO } },
	{ O(036), VARMASK_DABC, { EMD_OP_IN, EMD_FL_ARG_REG | EMD_FL_ARG_NORM | EMD_FL_INS_OS | EMD_FL_INS_IO } },

	{ O(037)+0000, VARMASK_DBC, { EMD_OP_AD, EMD_FL_ARG_NORM | EMD_FL_ADDR_DWORD } },
	{ O(037)+0100, VARMASK_DBC, { EMD_OP_SD, EMD_FL_ARG_NORM | EMD_FL_ADDR_DWORD } },
	{ O(037)+0200, VARMASK_DBC, { EMD_OP_MW, EMD_FL_ARG_NORM | EMD_FL_ADDR_WORD } },
	{ O(037)+0300, VARMASK_DBC, { EMD_OP_DW, EMD_FL_ARG_NORM | EMD_FL_ADDR_WORD } },
	{ O(037)+0400, VARMASK_DBC, { EMD_OP_AF, EMD_FL_ARG_NORM | EMD_FL_ADDR_FLOAT } },
	{ O(037)+0500, VARMASK_DBC, { EMD_OP_SF, EMD_FL_ARG_NORM | EMD_FL_ADDR_FLOAT } },
	{ O(037)+0600, VARMASK_DBC, { EMD_OP_MF, EMD_FL_ARG_NORM | EMD_FL_ADDR_FLOAT } },
	{ O(037)+0700, VARMASK_DBC, { EMD_OP_DF, EMD_FL_ARG_NORM | EMD_FL_ADDR_FLOAT } },

	{ O(040), VARMASK_DABC, { EMD_OP_AW, EMD_FL_ARG_REG | EMD_FL_ARG_NORM } },
	{ O(041), VARMASK_DABC, { EMD_OP_AC, EMD_FL_ARG_REG | EMD_FL_ARG_NORM } },
	{ O(042), VARMASK_DABC, { EMD_OP_SW, EMD_FL_ARG_REG | EMD_FL_ARG_NORM } },
	{ O(043), VARMASK_DABC, { EMD_OP_CW, EMD_FL_ARG_REG | EMD_FL_ARG_NORM } },
	{ O(044), VARMASK_DABC, { EMD_OP_OR, EMD_FL_ARG_REG | EMD_FL_ARG_NORM } },
	{ O(045), VARMASK_DABC, { EMD_OP_OM, EMD_FL_ARG_REG | EMD_FL_ARG_NORM | EMD_FL_ADDR_WORD } },
	{ O(046), VARMASK_DABC, { EMD_OP_NR, EMD_FL_ARG_REG | EMD_FL_ARG_NORM } },
	{ O(047), VARMASK_DABC, { EMD_OP_NM, EMD_FL_ARG_REG | EMD_FL_ARG_NORM | EMD_FL_ADDR_WORD } },
	{ O(050), VARMASK_DABC, { EMD_OP_ER, EMD_FL_ARG_REG | EMD_FL_ARG_NORM } },
	{ O(051), VARMASK_DABC, { EMD_OP_EM, EMD_FL_ARG_REG | EMD_FL_ARG_NORM | EMD_FL_ADDR_WORD } },
	{ O(052), VARMASK_DABC, { EMD_OP_XR, EMD_FL_ARG_REG | EMD_FL_ARG_NORM } },
	{ O(053), VARMASK_DABC, { EMD_OP_XM, EMD_FL_ARG_REG | EMD_FL_ARG_NORM | EMD_FL_ADDR_WORD } },
	{ O(054), VARMASK_DABC, { EMD_OP_CL, EMD_FL_ARG_REG | EMD_FL_ARG_NORM } },
	{ O(055), VARMASK_DABC, { EMD_OP_LB, EMD_FL_ARG_REG | EMD_FL_ARG_NORM | EMD_FL_ADDR_BYTE } },
	{ O(056), VARMASK_DABC, { EMD_OP_RB, EMD_FL_ARG_REG | EMD_FL_ARG_NORM | EMD_FL_ADDR_BYTE } },
	{ O(057), VARMASK_DABC, { EMD_OP_CB, EMD_FL_ARG_REG | EMD_FL_ARG_NORM | EMD_FL_ADDR_BYTE } },

	{ O(060), VARMASK_DABC, { EMD_OP_AWT, EMD_FL_ARG_REG | EMD_FL_ARG_SHORT7 } },
	{ O(061), VARMASK_DABC, { EMD_OP_TRB, EMD_FL_ARG_REG | EMD_FL_ARG_SHORT7 } },
	{ O(062), VARMASK_DABC, { EMD_OP_IRB, EMD_FL_ARG_REG | EMD_FL_ARG_SHORT7 | EMD_FL_ARG_RELATIVE | EMD_FL_ADDR_JUMP } },
	{ O(063), VARMASK_DABC, { EMD_OP_DRB, EMD_FL_ARG_REG | EMD_FL_ARG_SHORT7 | EMD_FL_ARG_RELATIVE | EMD_FL_ADDR_JUMP } },
	{ O(064), VARMASK_DABC, { EMD_OP_CWT, EMD_FL_ARG_REG | EMD_FL_ARG_SHORT7 } },
	{ O(065), VARMASK_DABC, { EMD_OP_LWT, EMD_FL_ARG_REG | EMD_FL_ARG_SHORT7 } },
	{ O(066), VARMASK_DABC, { EMD_OP_LWS, EMD_FL_ARG_REG | EMD_FL_ARG_SHORT7 | EMD_FL_ARG_RELATIVE | EMD_FL_ADDR_WORD } },
	{ O(067), VARMASK_DABC, { EMD_OP_RWS, EMD_FL_ARG_REG | EMD_FL_ARG_SHORT7 | EMD_FL_ARG_RELATIVE | EMD_FL_ADDR_WORD } },

	{ O(070)+0000, VARMASK_DBC, { EMD_OP_UJS, EMD_FL_ARG_SHORT7 | EMD_FL_ARG_RELATIVE | EMD_FL_ADDR_JUMP } },
	{ O(070)+0100, VARMASK_DBC, { EMD_OP_JLS, EMD_FL_ARG_SHORT7 | EMD_FL_ARG_RELATIVE | EMD_FL_ADDR_JUMP } },
	{ O(070)+0200, VARMASK_DBC, { EMD_OP_JES, EMD_FL_ARG_SHORT7 | EMD_FL_ARG_RELATIVE | EMD_FL_ADDR_JUMP } },
	{ O(070)+0300, VARMASK_DBC, { EMD_OP_JGS, EMD_FL_ARG_SHORT7 | EMD_FL_ARG_RELATIVE | EMD_FL_ADDR_JUMP } },
	{ O(070)+0400, VARMASK_DBC, { EMD_OP_JVS, EMD_FL_ARG_SHORT7 | EMD_FL_ARG_RELATIVE | EMD_FL_ADDR_JUMP } },
	{ O(070)+0500, VARMASK_DBC, { EMD_OP_JXS, EMD_FL_ARG_SHORT7 | EMD_FL_ARG_RELATIVE | EMD_FL_ADDR_JUMP } },
	{ O(070)+0600, VARMASK_DBC, { EMD_OP_JYS, EMD_FL_ARG_SHORT7 | EMD_FL_ARG_RELATIVE | EMD_FL_ADDR_JUMP } },
	{ O(070)+0700, VARMASK_DBC, { EMD_OP_JCS, EMD_FL_ARG_SHORT7 | EMD_FL_ARG_RELATIVE | EMD_FL_ADDR_JUMP } },

	{ O(071)+00000, VARMASK_BYTE, { EMD_OP_BLC, EMD_FL_ARG_SHORT8 } },
	{ O(071)+00400, VARMASK_BYTE, { EMD_OP_EXL, EMD_FL_ARG_SHORT8 } },
	{ O(071)+01000, VARMASK_BYTE, { EMD_OP_BRC, EMD_FL_ARG_SHORT8 } },
	{ O(071)+01400, VARMASK_BYTE, { EMD_OP_NRF, EMD_FL_ARG_SHORT8 } },

	{ O(072)+00000, VARMASK_A,   { EMD_OP_RIC, EMD_FL_ARG_REG } },
	{ O(072)+00001, VARMASK_A,   { EMD_OP_ZLB, EMD_FL_ARG_REG } },
	{ O(072)+00002, VARMASK_A,   { EMD_OP_SXU, EMD_FL_ARG_REG } },
	{ O(072)+00003, VARMASK_A,   { EMD_OP_NGA, EMD_FL_ARG_REG } },
	{ O(072)+00004, VARMASK_A,   { EMD_OP_SLZ, EMD_FL_ARG_REG } },
	{ O(072)+00005, VARMASK_A,   { EMD_OP_SLY, EMD_FL_ARG_REG } },
	{ O(072)+00006, VARMASK_A,   { EMD_OP_SLX, EMD_FL_ARG_REG } },
	{ O(072)+00007, VARMASK_A,   { EMD_OP_SRY, EMD_FL_ARG_REG } },
	{ O(072)+00010, VARMASK_A,   { EMD_OP_NGL, EMD_FL_ARG_REG } },
	{ O(072)+00011, VARMASK_A,   { EMD_OP_RPC, EMD_FL_ARG_REG } },
	{ O(072)+00020, VARMASK_DAC, { EMD_OP_SHC, EMD_FL_ARG_REG | EMD_FL_ARG_SHORT4 } },
	{ O(072)+01000, VARMASK_A,   { EMD_OP_RKY, EMD_FL_ARG_REG } },
	{ O(072)+01001, VARMASK_A,   { EMD_OP_ZRB, EMD_FL_ARG_REG } },
	{ O(072)+01002, VARMASK_A,   { EMD_OP_SXL, EMD_FL_ARG_REG } },
	{ O(072)+01003, VARMASK_A,   { EMD_OP_NGC, EMD_FL_ARG_REG } },
	{ O(072)+01004, VARMASK_A,   { EMD_OP_SVZ, EMD_FL_ARG_REG } },
	{ O(072)+01005, VARMASK_A,   { EMD_OP_SVY, EMD_FL_ARG_REG } },
	{ O(072)+01006, VARMASK_A,   { EMD_OP_SVX, EMD_FL_ARG_REG } },
	{ O(072)+01007, VARMASK_A,   { EMD_OP_SRX, EMD_FL_ARG_REG } },
	{ O(072)+01010, VARMASK_A,   { EMD_OP_SRZ, EMD_FL_ARG_REG } },
	{ O(072)+01011, VARMASK_A,   { EMD_OP_LPC, EMD_FL_ARG_REG } },

	{ O(073)+00000, VARMASK_DBC,  { EMD_OP_HLT,  EMD_FL_ARG_SHORT7 | EMD_FL_INS_OS } },
	{ O(073)+00100, VARMASK_DBC,  { EMD_OP_MCL,  EMD_FL_ARG_NONE | EMD_FL_INS_OS } },
	{ O(073)+00200, VARMASK_DB1C, { EMD_OP_CIT,  EMD_FL_ARG_NONE | EMD_FL_INS_OS } },
	{ O(073)+00201, VARMASK_DB1C, { EMD_OP_SIL,  EMD_FL_ARG_NONE | EMD_FL_INS_OS } },
	{ O(073)+00202, VARMASK_DB1C, { EMD_OP_SIU,  EMD_FL_ARG_NONE | EMD_FL_INS_OS } },
	{ O(073)+00203, VARMASK_DB1C, { EMD_OP_SIT,  EMD_FL_ARG_NONE | EMD_FL_INS_OS } },
	{ O(073)+00300, VARMASK_BC,   { EMD_OP_GIU,  EMD_FL_ARG_NONE | EMD_FL_INS_OS } },
	{ O(073)+00400, VARMASK_DBC,  { EMD_OP_LIP,  EMD_FL_ARG_NONE | EMD_FL_INS_OS } },
	{ O(073)+01300, VARMASK_BC,   { EMD_OP_GIL,  EMD_FL_ARG_NONE | EMD_FL_INS_OS } },

	{ O(073)+00204, VARMASK_B23C, { EMD_OP_SINT, EMD_FL_ARG_NONE | EMD_FL_INS_OS | EMD_FL_INS_MX16 } },
	{ O(073)+01204, VARMASK_B23C, { EMD_OP_SIND, EMD_FL_ARG_NONE | EMD_FL_INS_OS | EMD_FL_INS_MX16 } },
	{ O(073)+00500, VARMASK_DBC,  { EMD_OP_CRON, EMD_FL_ARG_NONE | EMD_FL_INS_OS | EMD_FL_INS_MX16 } },

	{ O(074)+0000, VARMASK_DBC, { EMD_OP_UJ, EMD_FL_ARG_NORM | EMD_FL_ADDR_JUMP } },
	{ O(074)+0100, VARMASK_DBC, { EMD_OP_JL, EMD_FL_ARG_NORM | EMD_FL_ADDR_JUMP } },
	{ O(074)+0200, VARMASK_DBC, { EMD_OP_JE, EMD_FL_ARG_NORM | EMD_FL_ADDR_JUMP } },
	{ O(074)+0300, VARMASK_DBC, { EMD_OP_JG, EMD_FL_ARG_NORM | EMD_FL_ADDR_JUMP } },
	{ O(074)+0400, VARMASK_DBC, { EMD_OP_JZ, EMD_FL_ARG_NORM | EMD_FL_ADDR_JUMP } },
	{ O(074)+0500, VARMASK_DBC, { EMD_OP_JM, EMD_FL_ARG_NORM | EMD_FL_ADDR_JUMP } },
	{ O(074)+0600, VARMASK_DBC, { EMD_OP_JN, EMD_FL_ARG_NORM | EMD_FL_ADDR_JUMP } },
	{ O(074)+0700, VARMASK_DBC, { EMD_OP_LJ, EMD_FL_ARG_NORM | EMD_FL_ADDR_CALL } },

	{ O(075)+0000, VARMASK_DBC, { EMD_OP_LD, EMD_FL_ARG_NORM | EMD_FL_ADDR_WORD } },
	{ O(075)+0100, VARMASK_DBC, { EMD_OP_LF, EMD_FL_ARG_NORM | EMD_FL_ADDR_WORD } },
	{ O(075)+0200, VARMASK_DBC, { EMD_OP_LA, EMD_FL_ARG_NORM | EMD_FL_ADDR_WORD } },
	{ O(075)+0300, VARMASK_DBC, { EMD_OP_LL, EMD_FL_ARG_NORM | EMD_FL_ADDR_WORD } },
	{ O(075)+0400, VARMASK_DBC, { EMD_OP_TD, EMD_FL_ARG_NORM | EMD_FL_ADDR_WORD } },
	{ O(075)+0500, VARMASK_DBC, { EMD_OP_TF, EMD_FL_ARG_NORM | EMD_FL_ADDR_WORD } },
	{ O(075)+0600, VARMASK_DBC, { EMD_OP_TA, EMD_FL_ARG_NORM | EMD_FL_ADDR_WORD } },
	{ O(075)+0700, VARMASK_DBC, { EMD_OP_TL, EMD_FL_ARG_NORM | EMD_FL_ADDR_WORD } },

	{ O(076)+0000, VARMASK_DBC, { EMD_OP_RD, EMD_FL_ARG_NORM | EMD_FL_ADDR_WORD } },
	{ O(076)+0100, VARMASK_DBC, { EMD_OP_RF, EMD_FL_ARG_NORM | EMD_FL_ADDR_WORD } },
	{ O(076)+0200, VARMASK_DBC, { EMD_OP_RA, EMD_FL_ARG_NORM | EMD_FL_ADDR_WORD } },
	{ O(076)+0300, VARMASK_DBC, { EMD_OP_RL, EMD_FL_ARG_NORM | EMD_FL_ADDR_WORD } },
	{ O(076)+0400, VARMASK_DBC, { EMD_OP_PD, EMD_FL_ARG_NORM | EMD_FL_ADDR_WORD } },
	{ O(076)+0500, VARMASK_DBC, { EMD_OP_PF, EMD_FL_ARG_NORM | EMD_FL_ADDR_WORD } },
	{ O(076)+0600, VARMASK_DBC, { EMD_OP_PA, EMD_FL_ARG_NORM | EMD_FL_ADDR_WORD } },
	{ O(076)+0700, VARMASK_DBC, { EMD_OP_PL, EMD_FL_ARG_NORM | EMD_FL_ADDR_WORD } },

	{ O(077)+0000, VARMASK_DBC, { EMD_OP_MB, EMD_FL_ARG_NORM | EMD_FL_ADDR_WORD | EMD_FL_INS_OS } },
	{ O(077)+0100, VARMASK_DBC, { EMD_OP_IM, EMD_FL_ARG_NORM | EMD_FL_ADDR_WORD | EMD_FL_INS_OS } },
	{ O(077)+0200, VARMASK_DBC, { EMD_OP_KI, EMD_FL_ARG_NORM | EMD_FL_ADDR_WORD | EMD_FL_INS_OS } },
	{ O(077)+0300, VARMASK_DBC, { EMD_OP_FI, EMD_FL_ARG_NORM | EMD_FL_ADDR_WORD | EMD_FL_INS_OS } },
	{ O(077)+0400, VARMASK_DBC, { EMD_OP_SP, EMD_FL_ARG_NORM | EMD_FL_ADDR_WORD | EMD_FL_INS_OS } },
	{ O(077)+0500, VARMASK_DBC, { EMD_OP_MD, EMD_FL_ARG_NORM } },
	{ O(077)+0600, VARMASK_DBC, { EMD_OP_RZ, EMD_FL_ARG_NORM | EMD_FL_ADDR_WORD } },
	{ O(077)+0700, VARMASK_DBC, { EMD_OP_IB, EMD_FL_ARG_NORM | EMD_FL_ADDR_WORD } },

	{ 0, 0, { 0, 0 } }
};

// -----------------------------------------------------------------------
static int emdas_iset_register_op(struct emdas_op *op_tab, uint16_t opcode, uint16_t mask, struct emdas_op *op)
{
	int i, pos;
	int offsets[16];
	int one_count = 0;
	int max;
	uint16_t result;

	// if mask is empty - nothing to do
	if (mask == 0) return -1;

	// store 1's positions in mask, count 1's
	for (i=0 ; i<16 ; i++) {
		if (mask & (1<<i)) {
			offsets[one_count] = i;
			one_count++;
		}
	}

	max = (1 << one_count) - 1;

	// iterate over all variants (as indicated by the mask)
	for (i=0 ; i<=max ; i++) {
		result = 0;

		// shift 1's into positions
		for (pos=one_count-1 ; pos>=0 ; pos--) {
			result |= ((i >> pos) & 1) << offsets[pos];
		}

		// register the op
		int dopcode = opcode | result;
		struct emdas_op *dop = op_tab + (dopcode);

		dop->id = op->id;
		dop->flags = op->flags;
		dop->v = dopcode;

		// set norm arg flags
		if (dop->flags & EMD_FL_ARG_NORM) {
			if (!_C(dopcode)) dop->flags |= EMD_FL_2WORD;
			if (_D(dopcode)) dop->flags |= EMD_FL_MOD_D;
			if (_B(dopcode)) dop->flags |= EMD_FL_MOD_B;
		}

		// check for argument that reffers to cpuflags
		// (instruction is a bit-branch and register is r0)
		if (((dop->id >= EMD_OP_BB) && (dop->id <= EMD_OP_BN) && (_A(dopcode) == 0))
		|| (dop->id == EMD_OP_BRC)
		|| (dop->id == EMD_OP_BLC)) {
			dop->flags |= EMD_FL_ARG_FLAGS;
		}
	}
	return 0;
}

// -----------------------------------------------------------------------
struct emdas_op * emdas_iset_create(int type)
{
	int res;

	struct emdas_op *op_tab = malloc(sizeof(struct emdas_op) * 0x10000);
	if (!op_tab) {
		return NULL;
	}

	// initialize instruction decoder
	struct emdas_instr *instr = emdas_ilist;
	while (instr->var_mask) {
		if (!(instr->op.flags & EMD_FL_INS_MX16) || ((type == EMD_ISET_MX16) && (instr->op.flags & EMD_FL_INS_MX16))) {
			res = emdas_iset_register_op(op_tab, instr->opcode, instr->var_mask, &instr->op);
			if (res) {
				free(op_tab);
				return NULL;
			}
		}
		instr++;
	}
	return op_tab;
}

// -----------------------------------------------------------------------
void emdas_iset_destroy(struct emdas_op *op_tab)
{
	free(op_tab);
}

// vim: tabstop=4 shiftwidth=4 autoindent
