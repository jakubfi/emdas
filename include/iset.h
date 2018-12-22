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

#ifndef EMDAS_ISET_H
#define EMDAS_ISET_H

// opcode identifiers (opcode name indexes)

enum emdas_op_ids {
			EMD_OP_NONE = 0,
/* NORM */	EMD_OP_LW, EMD_OP_TW, EMD_OP_LS, EMD_OP_RI, EMD_OP_RW, EMD_OP_PW, EMD_OP_RJ, EMD_OP_IS,
			EMD_OP_BB, EMD_OP_BM, EMD_OP_BS, EMD_OP_BC, EMD_OP_BN, EMD_OP_OU, EMD_OP_IN,
/* F/D */	EMD_OP_AD, EMD_OP_SD, EMD_OP_MW, EMD_OP_DW, EMD_OP_AF, EMD_OP_SF, EMD_OP_MF, EMD_OP_DF,
/* NORM */	EMD_OP_AW, EMD_OP_AC, EMD_OP_SW, EMD_OP_CW, EMD_OP_OR, EMD_OP_OM, EMD_OP_NR, EMD_OP_NM,
			EMD_OP_ER, EMD_OP_EM, EMD_OP_XR, EMD_OP_XM, EMD_OP_CL, EMD_OP_LB, EMD_OP_RB, EMD_OP_CB,
/* KA1 */	EMD_OP_AWT, EMD_OP_TRB, EMD_OP_IRB, EMD_OP_DRB, EMD_OP_CWT, EMD_OP_LWT, EMD_OP_LWS, EMD_OP_RWS,
/* JS */	EMD_OP_UJS, EMD_OP_NOP, EMD_OP_JLS, EMD_OP_JES, EMD_OP_JGS, EMD_OP_JVS, EMD_OP_JXS, EMD_OP_JYS, EMD_OP_JCS,
/* KA2 */	EMD_OP_BLC, EMD_OP_EXL, EMD_OP_BRC, EMD_OP_NRF,
/* C */		EMD_OP_RIC, EMD_OP_ZLB, EMD_OP_SXU, EMD_OP_NGA, EMD_OP_SLZ, EMD_OP_SLY, EMD_OP_SLX, EMD_OP_SRY,
			EMD_OP_NGL, EMD_OP_RPC, EMD_OP_SHC, EMD_OP_RKY, EMD_OP_ZRB, EMD_OP_SXL, EMD_OP_NGC, EMD_OP_SVZ,
			EMD_OP_SVY, EMD_OP_SVX, EMD_OP_SRX, EMD_OP_SRZ, EMD_OP_LPC,
/* S */		EMD_OP_HLT, EMD_OP_MCL, EMD_OP_CIT, EMD_OP_SIL, EMD_OP_SIU, EMD_OP_SIT, EMD_OP_GIU, EMD_OP_LIP, EMD_OP_GIL,
			EMD_OP_CRON, EMD_OP_SINT, EMD_OP_SIND,
/* J */		EMD_OP_UJ, EMD_OP_JL, EMD_OP_JE, EMD_OP_JG, EMD_OP_JZ, EMD_OP_JM, EMD_OP_JN, EMD_OP_LJ,
/* L */		EMD_OP_LD, EMD_OP_LF, EMD_OP_LA, EMD_OP_LL, EMD_OP_TD, EMD_OP_TF, EMD_OP_TA, EMD_OP_TL,
/* G */		EMD_OP_RD, EMD_OP_RF, EMD_OP_RA, EMD_OP_RL, EMD_OP_PD, EMD_OP_PF, EMD_OP_PA, EMD_OP_PL,
/* B/N */	EMD_OP_MB, EMD_OP_IM, EMD_OP_KI, EMD_OP_FI, EMD_OP_SP, EMD_OP_MD, EMD_OP_RZ, EMD_OP_IB,
			EMD_OP_MAX
};

// cell flags

enum emdas_flags {
	EMD_FL_NONE			= 0,

	// instruction flags
	EMD_FL_INS_OS		= 1 << 0,	// instruction is user-illegal (OS level only)
	EMD_FL_INS_IO		= 1 << 1,	// I/O instruction
	EMD_FL_INS_MX16		= 1 << 2,	// additional MX-16 instruction
	EMD_FL_INS_STRANGE	= 1 << 3,	// instruction is valid (would execute just fine) but suspicious (assembler would not encode it that way)

	// argument flags
	EMD_FL_ARG_NONE		= 1 << 4,	// no arguments
	EMD_FL_ARG_REG		= 1 << 5,	// rA register argument is present
	EMD_FL_ARG_REGIND	= 1 << 6,	// rA register indirect addressing
	EMD_FL_ARG_SHORT4	= 1 << 7,	// 4-bit short argument is present (SHC only)
	EMD_FL_ARG_SHORT7	= 1 << 8,	// 7-bit short argument is present
	EMD_FL_ARG_SHORT8	= 1 << 9,	// 8-bit short argument is present
	EMD_FL_ARG_SHORT	= EMD_FL_ARG_SHORT4 | EMD_FL_ARG_SHORT7 | EMD_FL_ARG_SHORT8, // any short argument
	EMD_FL_ARG_RELATIVE	= 1 << 10,	// short argument is PC-relative
	EMD_FL_ARG_NORM		= 1 << 11,	// normal argument is present
	EMD_FL_ARG2			= EMD_FL_ARG_SHORT | EMD_FL_ARG_NORM, // any 2nd argument
	EMD_FL_ARG_FLAGS	= 1 << 12,	// argument may refer to CPU flags

	// normarg flags
	EMD_FL_2WORD		= 1 << 13,	// normal argument uses additional word (rC=0)
	EMD_FL_MOD_D		= 1 << 14,	// normal argument is D-modified
	EMD_FL_MOD_B		= 1 << 15,	// normal argument is B-modified

	// normarg address flags
	EMD_FL_ADDR_JUMP	= 1 << 16,	// argument is a jump address
	EMD_FL_ADDR_CALL	= 1 << 17,	// argument is a "call" address
	EMD_FL_ADDR_BYTE	= 1 << 18,	// argument is a byte address
	EMD_FL_ADDR_WORD	= 1 << 19,	// argument is a word address
	EMD_FL_ADDR_DWORD	= 1 << 20,	// argument is a dword address
	EMD_FL_ADDR_FLOAT	= 1 << 21,	// argument is a float address
	EMD_FL_ADDR_WORD7	= 1 << 22,	// argument is a a 7-word
};

#define FMATCH(v, flags)  (((v) & (flags)) == (flags))
#define FNMATCH(v, flags) (((v) & (flags)) == 0)

struct emdas_op {
	uint8_t id;
	uint32_t flags;
	uint16_t v;
};

struct emdas_op * emdas_iset_create(int type);
void emdas_iset_destroy(struct emdas_op *op_tab);
char * emdas_iset_get_mneno(int i);

#endif

// vim: tabstop=4 shiftwidth=4 autoindent
