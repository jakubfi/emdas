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

#include <stdlib.h>

#include "emdas.h"
#include "iset.h"

static const struct opdef * get_extop_37(uint16_t opcode);
static const struct opdef * get_extop_70(uint16_t opcode);
static const struct opdef * get_extop_71(uint16_t opcode);
static const struct opdef * get_extop_72(uint16_t opcode);
static const struct opdef * get_extop_73(uint16_t opcode);
static const struct opdef * get_extop_74(uint16_t opcode);
static const struct opdef * get_extop_75(uint16_t opcode);
static const struct opdef * get_extop_76(uint16_t opcode);
static const struct opdef * get_extop_77(uint16_t opcode);

static const struct opdef iset[] = {
	{ 000, OP_NONE, NULL, FL_NONE },
	{ 001, OP_NONE, NULL, FL_NONE },
	{ 002, OP_NONE, NULL, FL_NONE },
	{ 003, OP_NONE, NULL, FL_NONE },
	{ 004, OP_NONE, NULL, FL_NONE },
	{ 005, OP_NONE, NULL, FL_NONE },
	{ 006, OP_NONE, NULL, FL_NONE },
	{ 007, OP_NONE, NULL, FL_NONE },
	{ 010, OP_NONE, NULL, FL_NONE },
	{ 011, OP_NONE, NULL, FL_NONE },
	{ 012, OP_NONE, NULL, FL_NONE },
	{ 013, OP_NONE, NULL, FL_NONE },
	{ 014, OP_NONE, NULL, FL_NONE },
	{ 015, OP_NONE, NULL, FL_NONE },
	{ 016, OP_NONE, NULL, FL_NONE },
	{ 017, OP_NONE, NULL, FL_NONE },
	
	{ 020, OP_LW, NULL, FL_ARG_REG | FL_ARG_NORM },
	{ 021, OP_TW, NULL, FL_ARG_REG | FL_ARG_NORM | FL_ADDR_WORD },
	{ 022, OP_LS, NULL, FL_ARG_REG | FL_ARG_NORM },
	{ 023, OP_RI, NULL, FL_ARG_REG | FL_ARG_NORM | FL_ARG_REGIND },
	{ 024, OP_RW, NULL, FL_ARG_REG | FL_ARG_NORM | FL_ADDR_WORD },
	{ 025, OP_PW, NULL, FL_ARG_REG | FL_ARG_NORM | FL_ADDR_WORD },
	{ 026, OP_RJ, NULL, FL_ARG_REG | FL_ARG_NORM | FL_ADDR_JUMP },
	{ 027, OP_IS, NULL, FL_ARG_REG | FL_ARG_NORM | FL_ADDR_JUMP },
	{ 030, OP_BB, NULL, FL_ARG_REG | FL_ARG_NORM },
	{ 031, OP_BM, NULL, FL_ARG_REG | FL_ARG_NORM | FL_ADDR_JUMP },
	{ 032, OP_BS, NULL, FL_ARG_REG | FL_ARG_NORM },
	{ 033, OP_BC, NULL, FL_ARG_REG | FL_ARG_NORM },
	{ 034, OP_BN, NULL, FL_ARG_REG | FL_ARG_NORM },
	{ 035, OP_OU, NULL, FL_ARG_REG | FL_ARG_NORM | FL_INS_OS | FL_INS_IO },
	{ 036, OP_IN, NULL, FL_ARG_REG | FL_ARG_NORM | FL_INS_OS | FL_INS_IO },

	{ 037, OP_NONE, get_extop_37, FL_NONE },

	{ 040, OP_AW, NULL, FL_ARG_REG | FL_ARG_NORM },
	{ 041, OP_AC, NULL, FL_ARG_REG | FL_ARG_NORM },
	{ 042, OP_SW, NULL, FL_ARG_REG | FL_ARG_NORM },
	{ 043, OP_CW, NULL, FL_ARG_REG | FL_ARG_NORM },
	{ 044, OP_OR, NULL, FL_ARG_REG | FL_ARG_NORM },
	{ 045, OP_OM, NULL, FL_ARG_REG | FL_ARG_NORM | FL_ADDR_WORD },
	{ 046, OP_NR, NULL, FL_ARG_REG | FL_ARG_NORM },
	{ 047, OP_NM, NULL, FL_ARG_REG | FL_ARG_NORM | FL_ADDR_WORD },
	{ 050, OP_ER, NULL, FL_ARG_REG | FL_ARG_NORM },
	{ 051, OP_EM, NULL, FL_ARG_REG | FL_ARG_NORM | FL_ADDR_WORD },
	{ 052, OP_XR, NULL, FL_ARG_REG | FL_ARG_NORM },
	{ 053, OP_XM, NULL, FL_ARG_REG | FL_ARG_NORM | FL_ADDR_WORD },
	{ 054, OP_CL, NULL, FL_ARG_REG | FL_ARG_NORM },
	{ 055, OP_LB, NULL, FL_ARG_REG | FL_ARG_NORM | FL_ADDR_BYTE },
	{ 056, OP_RB, NULL, FL_ARG_REG | FL_ARG_NORM | FL_ADDR_BYTE },
	{ 057, OP_CB, NULL, FL_ARG_REG | FL_ARG_NORM | FL_ADDR_BYTE },

	{ 060, OP_AWT, NULL, FL_ARG_REG | FL_ARG_SHORT7 },
	{ 061, OP_TRB, NULL, FL_ARG_REG | FL_ARG_SHORT7 },
	{ 062, OP_IRB, NULL, FL_ARG_REG | FL_ARG_SHORT7 | FL_ARG_RELATIVE | FL_ADDR_JUMP },
	{ 063, OP_DRB, NULL, FL_ARG_REG | FL_ARG_SHORT7 | FL_ARG_RELATIVE | FL_ADDR_JUMP },
	{ 064, OP_CWT, NULL, FL_ARG_REG | FL_ARG_SHORT7 },
	{ 065, OP_LWT, NULL, FL_ARG_REG | FL_ARG_SHORT7 },
	{ 066, OP_LWS, NULL, FL_ARG_REG | FL_ARG_SHORT7 | FL_ARG_RELATIVE | FL_ADDR_WORD },
	{ 067, OP_RWS, NULL, FL_ARG_REG | FL_ARG_SHORT7 | FL_ARG_RELATIVE | FL_ADDR_WORD },

	{ 070, OP_NONE, get_extop_70, FL_NONE },
	{ 071, OP_NONE, get_extop_71, FL_NONE },
	{ 072, OP_NONE, get_extop_72, FL_NONE },
	{ 073, OP_NONE, get_extop_73, FL_NONE },
	{ 074, OP_NONE, get_extop_74, FL_NONE },
	{ 075, OP_NONE, get_extop_75, FL_NONE },
	{ 076, OP_NONE, get_extop_76, FL_NONE },
	{ 077, OP_NONE, get_extop_77, FL_NONE }
};

static const struct opdef iset_37[] = {
	{ 0, OP_AD, NULL, FL_ARG_NORM | FL_ADDR_DWORD },
	{ 1, OP_SD, NULL, FL_ARG_NORM | FL_ADDR_DWORD },
	{ 2, OP_MW, NULL, FL_ARG_NORM | FL_ADDR_WORD },
	{ 3, OP_DW, NULL, FL_ARG_NORM | FL_ADDR_WORD },
	{ 4, OP_AF, NULL, FL_ARG_NORM | FL_ADDR_FLOAT },
	{ 5, OP_SF, NULL, FL_ARG_NORM | FL_ADDR_FLOAT },
	{ 6, OP_MF, NULL, FL_ARG_NORM | FL_ADDR_FLOAT },
	{ 7, OP_DF, NULL, FL_ARG_NORM | FL_ADDR_FLOAT }
};

static const struct opdef iset_70[] = {
	{ 0, OP_UJS, NULL, FL_ARG_SHORT7 | FL_ARG_RELATIVE | FL_ADDR_JUMP },
	{ 1, OP_JLS, NULL, FL_ARG_SHORT7 | FL_ARG_RELATIVE | FL_ADDR_JUMP },
	{ 2, OP_JES, NULL, FL_ARG_SHORT7 | FL_ARG_RELATIVE | FL_ADDR_JUMP },
	{ 3, OP_JGS, NULL, FL_ARG_SHORT7 | FL_ARG_RELATIVE | FL_ADDR_JUMP },
	{ 4, OP_JVS, NULL, FL_ARG_SHORT7 | FL_ARG_RELATIVE | FL_ADDR_JUMP },
	{ 5, OP_JXS, NULL, FL_ARG_SHORT7 | FL_ARG_RELATIVE | FL_ADDR_JUMP },
	{ 6, OP_JYS, NULL, FL_ARG_SHORT7 | FL_ARG_RELATIVE | FL_ADDR_JUMP },
	{ 7, OP_JCS, NULL, FL_ARG_SHORT7 | FL_ARG_RELATIVE | FL_ADDR_JUMP }
};

static const struct opdef iset_71[] = {
	{ 0, OP_BLC, NULL, FL_ARG_SHORT8 },
	{ 1, OP_EXL, NULL, FL_ARG_SHORT8 },
	{ 2, OP_BRC, NULL, FL_ARG_SHORT8 },
	{ 3, OP_NRF, NULL, FL_ARG_SHORT8 }
};

static const struct opdef iset_72[] = {
	{ 0000, OP_RIC, NULL, FL_ARG_REG },
	{ 0001, OP_ZLB, NULL, FL_ARG_REG },
	{ 0002, OP_SXU, NULL, FL_ARG_REG },
	{ 0003, OP_NGA, NULL, FL_ARG_REG },
	{ 0004, OP_SLZ, NULL, FL_ARG_REG },
	{ 0005, OP_SLY, NULL, FL_ARG_REG },
	{ 0006, OP_SLX, NULL, FL_ARG_REG },
	{ 0007, OP_SRY, NULL, FL_ARG_REG },
	{ 0010, OP_NGL, NULL, FL_ARG_REG },
	{ 0011, OP_RPC, NULL, FL_ARG_REG },
	{ 0012, OP_NONE, NULL, FL_NONE },
	{ 0013, OP_NONE, NULL, FL_NONE },
	{ 0014, OP_NONE, NULL, FL_NONE },
	{ 0015, OP_NONE, NULL, FL_NONE },
	{ 0016, OP_NONE, NULL, FL_NONE },
	{ 0017, OP_NONE, NULL, FL_NONE },
	{ 0020, OP_SHC,  NULL, FL_ARG_REG | FL_ARG_SHORT4 },
	{ 0021, OP_SHC,  NULL, FL_ARG_REG | FL_ARG_SHORT4 },
	{ 0022, OP_SHC,  NULL, FL_ARG_REG | FL_ARG_SHORT4 },
	{ 0023, OP_SHC,  NULL, FL_ARG_REG | FL_ARG_SHORT4 },
	{ 0024, OP_SHC,  NULL, FL_ARG_REG | FL_ARG_SHORT4 },
	{ 0025, OP_SHC,  NULL, FL_ARG_REG | FL_ARG_SHORT4 },
	{ 0026, OP_SHC,  NULL, FL_ARG_REG | FL_ARG_SHORT4 },
	{ 0027, OP_SHC,  NULL, FL_ARG_REG | FL_ARG_SHORT4 },
	{ 0030, OP_NONE, NULL, FL_NONE },
	{ 0031, OP_NONE, NULL, FL_NONE },
	{ 0032, OP_NONE, NULL, FL_NONE },
	{ 0033, OP_NONE, NULL, FL_NONE },
	{ 0034, OP_NONE, NULL, FL_NONE },
	{ 0035, OP_NONE, NULL, FL_NONE },
	{ 0036, OP_NONE, NULL, FL_NONE },
	{ 0037, OP_NONE, NULL, FL_NONE },
	{ 0040, OP_NONE, NULL, FL_NONE },
	{ 0041, OP_NONE, NULL, FL_NONE },
	{ 0042, OP_NONE, NULL, FL_NONE },
	{ 0043, OP_NONE, NULL, FL_NONE },
	{ 0044, OP_NONE, NULL, FL_NONE },
	{ 0045, OP_NONE, NULL, FL_NONE },
	{ 0046, OP_NONE, NULL, FL_NONE },
	{ 0047, OP_NONE, NULL, FL_NONE },
	{ 0050, OP_NONE, NULL, FL_NONE },
	{ 0051, OP_NONE, NULL, FL_NONE },
	{ 0052, OP_NONE, NULL, FL_NONE },
	{ 0053, OP_NONE, NULL, FL_NONE },
	{ 0054, OP_NONE, NULL, FL_NONE },
	{ 0055, OP_NONE, NULL, FL_NONE },
	{ 0056, OP_NONE, NULL, FL_NONE },
	{ 0057, OP_NONE, NULL, FL_NONE },
	{ 0060, OP_NONE, NULL, FL_NONE },
	{ 0061, OP_NONE, NULL, FL_NONE },
	{ 0062, OP_NONE, NULL, FL_NONE },
	{ 0063, OP_NONE, NULL, FL_NONE },
	{ 0064, OP_NONE, NULL, FL_NONE },
	{ 0065, OP_NONE, NULL, FL_NONE },
	{ 0066, OP_NONE, NULL, FL_NONE },
	{ 0067, OP_NONE, NULL, FL_NONE },
	{ 0070, OP_NONE, NULL, FL_NONE },
	{ 0071, OP_NONE, NULL, FL_NONE },
	{ 0072, OP_NONE, NULL, FL_NONE },
	{ 0073, OP_NONE, NULL, FL_NONE },
	{ 0074, OP_NONE, NULL, FL_NONE },
	{ 0075, OP_NONE, NULL, FL_NONE },
	{ 0076, OP_NONE, NULL, FL_NONE },
	{ 0077, OP_NONE, NULL, FL_NONE },
	{ 0100, OP_RKY, NULL, FL_ARG_REG },
	{ 0101, OP_ZRB, NULL, FL_ARG_REG },
	{ 0102, OP_SXL, NULL, FL_ARG_REG },
	{ 0103, OP_NGC, NULL, FL_ARG_REG },
	{ 0104, OP_SVZ, NULL, FL_ARG_REG },
	{ 0105, OP_SVY, NULL, FL_ARG_REG },
	{ 0106, OP_SVX, NULL, FL_ARG_REG },
	{ 0107, OP_SRX, NULL, FL_ARG_REG },
	{ 0110, OP_SRZ, NULL, FL_ARG_REG },
	{ 0111, OP_LPC, NULL, FL_ARG_REG },
	{ 0112, OP_NONE, NULL, FL_NONE },
	{ 0113, OP_NONE, NULL, FL_NONE },
	{ 0114, OP_NONE, NULL, FL_NONE },
	{ 0115, OP_NONE, NULL, FL_NONE },
	{ 0116, OP_NONE, NULL, FL_NONE },
	{ 0117, OP_NONE, NULL, FL_NONE },
	{ 0121, OP_SHC, NULL, FL_ARG_REG | FL_ARG_SHORT4 },
	{ 0120, OP_SHC, NULL, FL_ARG_REG | FL_ARG_SHORT4 },
	{ 0122, OP_SHC, NULL, FL_ARG_REG | FL_ARG_SHORT4 },
	{ 0123, OP_SHC, NULL, FL_ARG_REG | FL_ARG_SHORT4 },
	{ 0124, OP_SHC, NULL, FL_ARG_REG | FL_ARG_SHORT4 },
	{ 0125, OP_SHC, NULL, FL_ARG_REG | FL_ARG_SHORT4 },
	{ 0126, OP_SHC, NULL, FL_ARG_REG | FL_ARG_SHORT4 },
	{ 0127, OP_SHC, NULL, FL_ARG_REG | FL_ARG_SHORT4 },
	{ 0130, OP_NONE, NULL, FL_NONE },
	{ 0131, OP_NONE, NULL, FL_NONE },
	{ 0132, OP_NONE, NULL, FL_NONE },
	{ 0133, OP_NONE, NULL, FL_NONE },
	{ 0134, OP_NONE, NULL, FL_NONE },
	{ 0135, OP_NONE, NULL, FL_NONE },
	{ 0136, OP_NONE, NULL, FL_NONE },
	{ 0137, OP_NONE, NULL, FL_NONE },
	{ 0140, OP_NONE, NULL, FL_NONE },
	{ 0141, OP_NONE, NULL, FL_NONE },
	{ 0142, OP_NONE, NULL, FL_NONE },
	{ 0143, OP_NONE, NULL, FL_NONE },
	{ 0144, OP_NONE, NULL, FL_NONE },
	{ 0145, OP_NONE, NULL, FL_NONE },
	{ 0146, OP_NONE, NULL, FL_NONE },
	{ 0147, OP_NONE, NULL, FL_NONE },
	{ 0150, OP_NONE, NULL, FL_NONE },
	{ 0151, OP_NONE, NULL, FL_NONE },
	{ 0152, OP_NONE, NULL, FL_NONE },
	{ 0153, OP_NONE, NULL, FL_NONE },
	{ 0154, OP_NONE, NULL, FL_NONE },
	{ 0155, OP_NONE, NULL, FL_NONE },
	{ 0156, OP_NONE, NULL, FL_NONE },
	{ 0157, OP_NONE, NULL, FL_NONE },
	{ 0161, OP_NONE, NULL, FL_NONE },
	{ 0160, OP_NONE, NULL, FL_NONE },
	{ 0162, OP_NONE, NULL, FL_NONE },
	{ 0163, OP_NONE, NULL, FL_NONE },
	{ 0164, OP_NONE, NULL, FL_NONE },
	{ 0165, OP_NONE, NULL, FL_NONE },
	{ 0166, OP_NONE, NULL, FL_NONE },
	{ 0167, OP_NONE, NULL, FL_NONE },
	{ 0170, OP_NONE, NULL, FL_NONE },
	{ 0171, OP_NONE, NULL, FL_NONE },
	{ 0172, OP_NONE, NULL, FL_NONE },
	{ 0173, OP_NONE, NULL, FL_NONE },
	{ 0174, OP_NONE, NULL, FL_NONE },
	{ 0175, OP_NONE, NULL, FL_NONE },
	{ 0176, OP_NONE, NULL, FL_NONE },
	{ 0177, OP_NONE, NULL, FL_NONE }
};

static const struct opdef iset_73[] = {
	{ 0000, OP_HLT, NULL, FL_ARG_SHORT7 | FL_INS_OS },
	{ 0001, OP_HLT, NULL, FL_ARG_SHORT7 | FL_INS_OS },
	{ 0002, OP_HLT, NULL, FL_ARG_SHORT7 | FL_INS_OS },
	{ 0003, OP_HLT, NULL, FL_ARG_SHORT7 | FL_INS_OS },
	{ 0004, OP_HLT, NULL, FL_ARG_SHORT7 | FL_INS_OS },
	{ 0005, OP_HLT, NULL, FL_ARG_SHORT7 | FL_INS_OS },
	{ 0006, OP_HLT, NULL, FL_ARG_SHORT7 | FL_INS_OS },
	{ 0007, OP_HLT, NULL, FL_ARG_SHORT7 | FL_INS_OS },

	{ 0010, OP_MCL, NULL, FL_ARG_NONE | FL_INS_OS },
	{ 0011, OP_MCL, NULL, FL_ARG_NONE | FL_INS_OS },
	{ 0012, OP_MCL, NULL, FL_ARG_NONE | FL_INS_OS },
	{ 0013, OP_MCL, NULL, FL_ARG_NONE | FL_INS_OS },
	{ 0014, OP_MCL, NULL, FL_ARG_NONE | FL_INS_OS },
	{ 0015, OP_MCL, NULL, FL_ARG_NONE | FL_INS_OS },
	{ 0016, OP_MCL, NULL, FL_ARG_NONE | FL_INS_OS },
	{ 0017, OP_MCL, NULL, FL_ARG_NONE | FL_INS_OS },

	{ 0020, OP_CIT, NULL, FL_ARG_NONE | FL_INS_OS },
	{ 0021, OP_SIL, NULL, FL_ARG_NONE | FL_INS_OS },
	{ 0022, OP_SIU, NULL, FL_ARG_NONE | FL_INS_OS },
	{ 0023, OP_SIT, NULL, FL_ARG_NONE | FL_INS_OS },
	{ 0024, OP_SINT, NULL, FL_ARG_NONE | FL_INS_OS | FL_INS_MX16 },
	{ 0025, OP_NONE, NULL, FL_NONE },
	{ 0026, OP_NONE, NULL, FL_NONE },
	{ 0027, OP_NONE, NULL, FL_NONE },

	{ 0030, OP_GIU, NULL, FL_ARG_NONE | FL_INS_OS },
	{ 0031, OP_GIU, NULL, FL_ARG_NONE | FL_INS_OS },
	{ 0032, OP_GIU, NULL, FL_ARG_NONE | FL_INS_OS },
	{ 0033, OP_GIU, NULL, FL_ARG_NONE | FL_INS_OS },
	{ 0034, OP_GIU, NULL, FL_ARG_NONE | FL_INS_OS },
	{ 0035, OP_GIU, NULL, FL_ARG_NONE | FL_INS_OS },
	{ 0036, OP_GIU, NULL, FL_ARG_NONE | FL_INS_OS },
	{ 0037, OP_GIU, NULL, FL_ARG_NONE | FL_INS_OS },

	{ 0040, OP_LIP, NULL, FL_ARG_NONE | FL_INS_OS },
	{ 0041, OP_LIP, NULL, FL_ARG_NONE | FL_INS_OS },
	{ 0042, OP_LIP, NULL, FL_ARG_NONE | FL_INS_OS },
	{ 0043, OP_LIP, NULL, FL_ARG_NONE | FL_INS_OS },
	{ 0044, OP_LIP, NULL, FL_ARG_NONE | FL_INS_OS },
	{ 0045, OP_LIP, NULL, FL_ARG_NONE | FL_INS_OS },
	{ 0046, OP_LIP, NULL, FL_ARG_NONE | FL_INS_OS },
	{ 0047, OP_LIP, NULL, FL_ARG_NONE | FL_INS_OS },

	{ 0050, OP_CRON, NULL, FL_ARG_NONE | FL_INS_OS | FL_INS_MX16 },
	{ 0051, OP_NONE, NULL, FL_NONE },
	{ 0052, OP_NONE, NULL, FL_NONE },
	{ 0053, OP_NONE, NULL, FL_NONE },
	{ 0054, OP_NONE, NULL, FL_NONE },
	{ 0055, OP_NONE, NULL, FL_NONE },
	{ 0056, OP_NONE, NULL, FL_NONE },
	{ 0057, OP_NONE, NULL, FL_NONE },

	{ 0060, OP_NONE, NULL, FL_NONE },
	{ 0061, OP_NONE, NULL, FL_NONE },
	{ 0062, OP_NONE, NULL, FL_NONE },
	{ 0063, OP_NONE, NULL, FL_NONE },
	{ 0064, OP_NONE, NULL, FL_NONE },
	{ 0065, OP_NONE, NULL, FL_NONE },
	{ 0066, OP_NONE, NULL, FL_NONE },
	{ 0067, OP_NONE, NULL, FL_NONE },

	{ 0070, OP_NONE, NULL, FL_NONE },
	{ 0071, OP_NONE, NULL, FL_NONE },
	{ 0072, OP_NONE, NULL, FL_NONE },
	{ 0073, OP_NONE, NULL, FL_NONE },
	{ 0074, OP_NONE, NULL, FL_NONE },
	{ 0075, OP_NONE, NULL, FL_NONE },
	{ 0076, OP_NONE, NULL, FL_NONE },
	{ 0077, OP_NONE, NULL, FL_NONE },

	{ 0100, OP_HLT, NULL, FL_ARG_SHORT7 | FL_INS_OS },
	{ 0101, OP_HLT, NULL, FL_ARG_SHORT7 | FL_INS_OS },
	{ 0102, OP_HLT, NULL, FL_ARG_SHORT7 | FL_INS_OS },
	{ 0103, OP_HLT, NULL, FL_ARG_SHORT7 | FL_INS_OS },
	{ 0104, OP_HLT, NULL, FL_ARG_SHORT7 | FL_INS_OS },
	{ 0105, OP_HLT, NULL, FL_ARG_SHORT7 | FL_INS_OS },
	{ 0106, OP_HLT, NULL, FL_ARG_SHORT7 | FL_INS_OS },
	{ 0107, OP_HLT, NULL, FL_ARG_SHORT7 | FL_INS_OS },

	{ 0110, OP_MCL, NULL, FL_ARG_NONE | FL_INS_OS },
	{ 0111, OP_MCL, NULL, FL_ARG_NONE | FL_INS_OS },
	{ 0112, OP_MCL, NULL, FL_ARG_NONE | FL_INS_OS },
	{ 0113, OP_MCL, NULL, FL_ARG_NONE | FL_INS_OS },
	{ 0114, OP_MCL, NULL, FL_ARG_NONE | FL_INS_OS },
	{ 0115, OP_MCL, NULL, FL_ARG_NONE | FL_INS_OS },
	{ 0116, OP_MCL, NULL, FL_ARG_NONE | FL_INS_OS },
	{ 0117, OP_MCL, NULL, FL_ARG_NONE | FL_INS_OS },

	{ 0120, OP_CIT, NULL, FL_ARG_NONE | FL_INS_OS },
	{ 0121, OP_SIL, NULL, FL_ARG_NONE | FL_INS_OS },
	{ 0122, OP_SIU, NULL, FL_ARG_NONE | FL_INS_OS },
	{ 0123, OP_SIT, NULL, FL_ARG_NONE | FL_INS_OS },
	{ 0124, OP_SIND, NULL, FL_ARG_NONE | FL_INS_OS | FL_INS_MX16 },
	{ 0125, OP_NONE, NULL, FL_NONE },
	{ 0126, OP_NONE, NULL, FL_NONE },
	{ 0127, OP_NONE, NULL, FL_NONE },

	{ 0130, OP_GIL, NULL, FL_ARG_NONE | FL_INS_OS },
	{ 0131, OP_GIL, NULL, FL_ARG_NONE | FL_INS_OS },
	{ 0132, OP_GIL, NULL, FL_ARG_NONE | FL_INS_OS },
	{ 0133, OP_GIL, NULL, FL_ARG_NONE | FL_INS_OS },
	{ 0134, OP_GIL, NULL, FL_ARG_NONE | FL_INS_OS },
	{ 0135, OP_GIL, NULL, FL_ARG_NONE | FL_INS_OS },
	{ 0136, OP_GIL, NULL, FL_ARG_NONE | FL_INS_OS },
	{ 0137, OP_GIL, NULL, FL_ARG_NONE | FL_INS_OS },

	{ 0141, OP_LIP, NULL, FL_ARG_NONE | FL_INS_OS },
	{ 0140, OP_LIP, NULL, FL_ARG_NONE | FL_INS_OS },
	{ 0142, OP_LIP, NULL, FL_ARG_NONE | FL_INS_OS },
	{ 0143, OP_LIP, NULL, FL_ARG_NONE | FL_INS_OS },
	{ 0145, OP_LIP, NULL, FL_ARG_NONE | FL_INS_OS },
	{ 0144, OP_LIP, NULL, FL_ARG_NONE | FL_INS_OS },
	{ 0146, OP_LIP, NULL, FL_ARG_NONE | FL_INS_OS },
	{ 0147, OP_LIP, NULL, FL_ARG_NONE | FL_INS_OS },

	{ 0150, OP_NONE, NULL, FL_NONE },
	{ 0151, OP_NONE, NULL, FL_NONE },
	{ 0152, OP_NONE, NULL, FL_NONE },
	{ 0153, OP_NONE, NULL, FL_NONE },
	{ 0154, OP_NONE, NULL, FL_NONE },
	{ 0155, OP_NONE, NULL, FL_NONE },
	{ 0156, OP_NONE, NULL, FL_NONE },
	{ 0157, OP_NONE, NULL, FL_NONE },

	{ 0160, OP_NONE, NULL, FL_NONE },
	{ 0161, OP_NONE, NULL, FL_NONE },
	{ 0162, OP_NONE, NULL, FL_NONE },
	{ 0163, OP_NONE, NULL, FL_NONE },
	{ 0164, OP_NONE, NULL, FL_NONE },
	{ 0165, OP_NONE, NULL, FL_NONE },
	{ 0166, OP_NONE, NULL, FL_NONE },
	{ 0167, OP_NONE, NULL, FL_NONE },

	{ 0170, OP_NONE, NULL, FL_NONE },
	{ 0171, OP_NONE, NULL, FL_NONE },
	{ 0172, OP_NONE, NULL, FL_NONE },
	{ 0173, OP_NONE, NULL, FL_NONE },
	{ 0174, OP_NONE, NULL, FL_NONE },
	{ 0175, OP_NONE, NULL, FL_NONE },
	{ 0176, OP_NONE, NULL, FL_NONE },
	{ 0177, OP_NONE, NULL, FL_NONE }

};

static const struct opdef iset_74[] = {
	{ 0, OP_UJ, NULL, FL_ARG_NORM | FL_ADDR_JUMP },
	{ 1, OP_JL, NULL, FL_ARG_NORM | FL_ADDR_JUMP },
	{ 2, OP_JE, NULL, FL_ARG_NORM | FL_ADDR_JUMP },
	{ 3, OP_JG, NULL, FL_ARG_NORM | FL_ADDR_JUMP },
	{ 4, OP_JZ, NULL, FL_ARG_NORM | FL_ADDR_JUMP },
	{ 5, OP_JM, NULL, FL_ARG_NORM | FL_ADDR_JUMP },
	{ 6, OP_JN, NULL, FL_ARG_NORM | FL_ADDR_JUMP },
	{ 7, OP_LJ, NULL, FL_ARG_NORM | FL_ADDR_JUMP }
};

static const struct opdef iset_75[] = {
	{ 0, OP_LD, NULL, FL_ARG_NORM | FL_ADDR_WORD },
	{ 1, OP_LF, NULL, FL_ARG_NORM | FL_ADDR_WORD },
	{ 2, OP_LA, NULL, FL_ARG_NORM | FL_ADDR_WORD },
	{ 3, OP_LL, NULL, FL_ARG_NORM | FL_ADDR_WORD },
	{ 4, OP_TD, NULL, FL_ARG_NORM | FL_ADDR_WORD },
	{ 5, OP_TF, NULL, FL_ARG_NORM | FL_ADDR_WORD },
	{ 6, OP_TA, NULL, FL_ARG_NORM | FL_ADDR_WORD },
	{ 7, OP_TL, NULL, FL_ARG_NORM | FL_ADDR_WORD }
};

static const struct opdef iset_76[] = {
	{ 0, OP_RD, NULL, FL_ARG_NORM | FL_ADDR_WORD },
	{ 1, OP_RF, NULL, FL_ARG_NORM | FL_ADDR_WORD },
	{ 2, OP_RA, NULL, FL_ARG_NORM | FL_ADDR_WORD },
	{ 3, OP_RL, NULL, FL_ARG_NORM | FL_ADDR_WORD },
	{ 4, OP_PD, NULL, FL_ARG_NORM | FL_ADDR_WORD },
	{ 5, OP_PF, NULL, FL_ARG_NORM | FL_ADDR_WORD },
	{ 6, OP_PA, NULL, FL_ARG_NORM | FL_ADDR_WORD },
	{ 7, OP_PL, NULL, FL_ARG_NORM | FL_ADDR_WORD }
};

static const struct opdef iset_77[] = {
	{ 0, OP_MB, NULL, FL_ARG_NORM | FL_ADDR_WORD | FL_INS_OS },
	{ 1, OP_IM, NULL, FL_ARG_NORM | FL_ADDR_WORD | FL_INS_OS },
	{ 2, OP_KI, NULL, FL_ARG_NORM | FL_ADDR_WORD | FL_INS_OS },
	{ 3, OP_FI, NULL, FL_ARG_NORM | FL_ADDR_WORD | FL_INS_OS },
	{ 4, OP_SP, NULL, FL_ARG_NORM | FL_ADDR_WORD | FL_INS_OS },
	{ 5, OP_MD, NULL, FL_ARG_NORM },
	{ 6, OP_RZ, NULL, FL_ARG_NORM | FL_ADDR_WORD },
	{ 7, OP_IB, NULL, FL_ARG_NORM | FL_ADDR_WORD }
};

// -----------------------------------------------------------------------
const struct opdef * emdas_get_op(uint16_t opcode)
{
	const struct opdef *opdef = iset + _OP(opcode);
	if (!opdef->extop_fun) {
		return opdef;
	} else {
		return opdef->extop_fun(opcode);
	}
}

// -----------------------------------------------------------------------
static const struct opdef * get_extop_37(uint16_t opcode)
{
	return iset_37 + _A(opcode);
}

// -----------------------------------------------------------------------
static const struct opdef * get_extop_70(uint16_t opcode)
{
	return iset_70 + _A(opcode);
}

// -----------------------------------------------------------------------
static const struct opdef * get_extop_71(uint16_t opcode)
{
	return iset_71 + ((opcode & 0b0000001100000000) >> 8);
}

// -----------------------------------------------------------------------
static const struct opdef * get_extop_72(uint16_t opcode)
{
	return iset_72 + (((opcode & 0b0000001000000000) >> 3) | (opcode & 0b0000000000111111));
}

// -----------------------------------------------------------------------
static const struct opdef * get_extop_73(uint16_t opcode)
{
	return iset_73 + (((opcode & 0b0000001111000000) >> 3) | (opcode & 0b0000000000000111));
}

// -----------------------------------------------------------------------
static const struct opdef * get_extop_74(uint16_t opcode)
{
	return iset_74 + _A(opcode);
}

// -----------------------------------------------------------------------
static const struct opdef * get_extop_75(uint16_t opcode)
{
	return iset_75 + _A(opcode);
}

// -----------------------------------------------------------------------
static const struct opdef * get_extop_76(uint16_t opcode)
{
	return iset_76 + _A(opcode);
}

// -----------------------------------------------------------------------
static const struct opdef * get_extop_77(uint16_t opcode)
{
	return iset_77 + _A(opcode);
}


// vim: tabstop=4 shiftwidth=4 autoindent
