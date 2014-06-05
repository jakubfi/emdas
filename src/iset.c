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

#include "image.h"
#include "iset.h"

struct opdef iset[] = {
	{ 000, OP_ILL, OP_GR_NONE, C_DATA, NULL, NULL, 0 },
	{ 001, OP_ILL, OP_GR_NONE, C_DATA, NULL, NULL, 0 },
	{ 002, OP_ILL, OP_GR_NONE, C_DATA, NULL, NULL, 0 },
	{ 003, OP_ILL, OP_GR_NONE, C_DATA, NULL, NULL, 0 },
	{ 004, OP_ILL, OP_GR_NONE, C_DATA, NULL, NULL, 0 },
	{ 005, OP_ILL, OP_GR_NONE, C_DATA, NULL, NULL, 0 },
	{ 006, OP_ILL, OP_GR_NONE, C_DATA, NULL, NULL, 0 },
	{ 007, OP_ILL, OP_GR_NONE, C_DATA, NULL, NULL, 0 },
	{ 010, OP_ILL, OP_GR_NONE, C_DATA, NULL, NULL, 0 },
	{ 011, OP_ILL, OP_GR_NONE, C_DATA, NULL, NULL, 0 },
	{ 012, OP_ILL, OP_GR_NONE, C_DATA, NULL, NULL, 0 },
	{ 013, OP_ILL, OP_GR_NONE, C_DATA, NULL, NULL, 0 },
	{ 014, OP_ILL, OP_GR_NONE, C_DATA, NULL, NULL, 0 },
	{ 015, OP_ILL, OP_GR_NONE, C_DATA, NULL, NULL, 0 },
	{ 016, OP_ILL, OP_GR_NONE, C_DATA, NULL, NULL, 0 },
	{ 017, OP_ILL, OP_GR_NONE, C_DATA, NULL, NULL, 0 },
	
	{ 020, OP_LW, OP_GR_NORM, C_OP_RN, "lw", NULL, ARG_REG | ARG_NORM },
	{ 021, OP_TW, OP_GR_NORM, C_OP_RN, "tw", NULL, ARG_REG | ARG_NORM | ARG_A_WORD },
	{ 022, OP_LS, OP_GR_NORM, C_OP_RN, "ls", NULL, ARG_REG | ARG_NORM },
	{ 023, OP_RI, OP_GR_NORM, C_OP_RN, "ri", NULL, ARG_REG | ARG_NORM | ARG_REGIND },
	{ 024, OP_RW, OP_GR_NORM, C_OP_RN, "rw", NULL, ARG_REG | ARG_NORM | ARG_A_WORD },
	{ 025, OP_PW, OP_GR_NORM, C_OP_RN, "pw", NULL, ARG_REG | ARG_NORM | ARG_A_WORD },
	{ 026, OP_RJ, OP_GR_NORM, C_OP_RN, "rj", NULL, ARG_REG | ARG_NORM | ARG_A_JUMP },
	{ 027, OP_IS, OP_GR_NORM, C_OP_RN, "is", NULL, ARG_REG | ARG_NORM | ARG_A_JUMP },
	{ 030, OP_BB, OP_GR_NORM, C_OP_RN, "bb", NULL, ARG_REG | ARG_NORM },
	{ 031, OP_BM, OP_GR_NORM, C_OP_RN, "bm", NULL, ARG_REG | ARG_NORM | ARG_A_JUMP },
	{ 032, OP_BS, OP_GR_NORM, C_OP_RN, "bs", NULL, ARG_REG | ARG_NORM },
	{ 033, OP_BC, OP_GR_NORM, C_OP_RN, "bc", NULL, ARG_REG | ARG_NORM },
	{ 034, OP_BN, OP_GR_NORM, C_OP_RN, "bn", NULL, ARG_REG | ARG_NORM },
	{ 035, OP_OU, OP_GR_NORM, C_OP_RN, "ou", NULL, ARG_REG | ARG_NORM },
	{ 036, OP_IN, OP_GR_NORM, C_OP_RN, "in", NULL, ARG_REG | ARG_NORM },

	{ 037, OP_ILL, OP_GR_NONE, NULL, get_extop_37, 0 },

	{ 040, OP_AW, OP_GR_NORM, C_OP_RN, "aw", NULL, ARG_REG | ARG_NORM },
	{ 041, OP_AC, OP_GR_NORM, C_OP_RN, "ac", NULL, ARG_REG | ARG_NORM },
	{ 042, OP_SW, OP_GR_NORM, C_OP_RN, "sw", NULL, ARG_REG | ARG_NORM },
	{ 043, OP_CW, OP_GR_NORM, C_OP_RN, "cw", NULL, ARG_REG | ARG_NORM },
	{ 044, OP_OR, OP_GR_NORM, C_OP_RN, "or", NULL, ARG_REG | ARG_NORM },
	{ 045, OP_OM, OP_GR_NORM, C_OP_RN, "om", NULL, ARG_REG | ARG_NORM | ARG_A_WORD },
	{ 046, OP_NR, OP_GR_NORM, C_OP_RN, "nr", NULL, ARG_REG | ARG_NORM },
	{ 047, OP_NM, OP_GR_NORM, C_OP_RN, "nm", NULL, ARG_REG | ARG_NORM | ARG_A_WORD },
	{ 050, OP_ER, OP_GR_NORM, C_OP_RN, "er", NULL, ARG_REG | ARG_NORM },
	{ 051, OP_EM, OP_GR_NORM, C_OP_RN, "em", NULL, ARG_REG | ARG_NORM | ARG_A_WORD },
	{ 052, OP_XR, OP_GR_NORM, C_OP_RN, "xr", NULL, ARG_REG | ARG_NORM },
	{ 053, OP_XM, OP_GR_NORM, C_OP_RN, "xm", NULL, ARG_REG | ARG_NORM | ARG_A_WORD },
	{ 054, OP_CL, OP_GR_NORM, C_OP_RN, "cl", NULL, ARG_REG | ARG_NORM },
	{ 055, OP_LB, OP_GR_NORM, C_OP_RN, "lb", NULL, ARG_REG | ARG_NORM | ARG_A_BYTE },
	{ 056, OP_RB, OP_GR_NORM, C_OP_RN, "rb", NULL, ARG_REG | ARG_NORM | ARG_A_BYTE },
	{ 057, OP_CB, OP_GR_NORM, C_OP_RN, "cb", NULL, ARG_REG | ARG_NORM | ARG_A_BYTE },

	{ 060, OP_AWT, OP_GR_KA1, C_OP_RT, "awt", NULL, ARG_REG | ARG_SHORT7 },
	{ 061, OP_TRB, OP_GR_KA1, C_OP_RT, "trb", NULL, ARG_REG | ARG_SHORT7 },
	{ 062, OP_IRB, OP_GR_KA1, C_OP_RT, "irb", NULL, ARG_REG | ARG_SHORT7 | ARG_RELATIVE | ARG_A_JUMP },
	{ 063, OP_DRB, OP_GR_KA1, C_OP_RT, "drb", NULL, ARG_REG | ARG_SHORT7 | ARG_RELATIVE | ARG_A_JUMP },
	{ 064, OP_CWT, OP_GR_KA1, C_OP_RT, "cwt", NULL, ARG_REG | ARG_SHORT7 },
	{ 065, OP_LWT, OP_GR_KA1, C_OP_RT, "lwt", NULL, ARG_REG | ARG_SHORT7 },
	{ 066, OP_LWS, OP_GR_KA1, C_OP_RT, "lws", NULL, ARG_REG | ARG_SHORT7 | ARG_RELATIVE | ARG_A_WORD },
	{ 067, OP_RWS, OP_GR_KA1, C_OP_RT, "rws", NULL, ARG_REG | ARG_SHORT7 | ARG_RELATIVE | ARG_A_WORD },

	{ 070, OP_ILL, OP_GR_NONE, NULL, get_extop_70, 0 },
	{ 071, OP_ILL, OP_GR_NONE, NULL, get_extop_71, 0 },
	{ 072, OP_ILL, OP_GR_NONE, NULL, get_extop_72, 0 },
	{ 073, OP_ILL, OP_GR_NONE, NULL, get_extop_73, 0 },
	{ 074, OP_ILL, OP_GR_NONE, NULL, get_extop_74, 0 },
	{ 075, OP_ILL, OP_GR_NONE, NULL, get_extop_75, 0 },
	{ 076, OP_ILL, OP_GR_NONE, NULL, get_extop_76, 0 },
	{ 077, OP_ILL, OP_GR_NONE, NULL, get_extop_77, 0 }
};

struct opdef iset_37[] = {
	{ 0, OP_AD, OP_GR_FD, C_OP_N, "ad", NULL, ARG_NORM | ARG_A_DWORD },
	{ 1, OP_SD, OP_GR_FD, C_OP_N, "sd", NULL, ARG_NORM | ARG_A_DWORD },
	{ 2, OP_MW, OP_GR_FD, C_OP_N, "mw", NULL, ARG_NORM | ARG_A_DWORD },
	{ 3, OP_DW, OP_GR_FD, C_OP_N, "dw", NULL, ARG_NORM | ARG_A_DWORD },
	{ 4, OP_AF, OP_GR_FD, C_OP_N, "af", NULL, ARG_NORM | ARG_A_FLOAT },
	{ 5, OP_SF, OP_GR_FD, C_OP_N, "sf", NULL, ARG_NORM | ARG_A_FLOAT },
	{ 6, OP_MF, OP_GR_FD, C_OP_N, "mf", NULL, ARG_NORM | ARG_A_FLOAT },
	{ 7, OP_DF, OP_GR_FD, C_OP_N, "df", NULL, ARG_NORM | ARG_A_FLOAT }
};

struct opdef iset_70[] = {
	{ 0, OP_UJS, OP_GR_JS, C_OP_T, "ujs", NULL, ARG_SHORT7 | ARG_RELATIVE | ARG_A_JUMP },
	{ 1, OP_JLS, OP_GR_JS, C_OP_T, "jls", NULL, ARG_SHORT7 | ARG_RELATIVE | ARG_A_JUMP },
	{ 2, OP_JES, OP_GR_JS, C_OP_T, "jes", NULL, ARG_SHORT7 | ARG_RELATIVE | ARG_A_JUMP },
	{ 3, OP_JGS, OP_GR_JS, C_OP_T, "jgs", NULL, ARG_SHORT7 | ARG_RELATIVE | ARG_A_JUMP },
	{ 4, OP_JVS, OP_GR_JS, C_OP_T, "jvs", NULL, ARG_SHORT7 | ARG_RELATIVE | ARG_A_JUMP },
	{ 5, OP_JXS, OP_GR_JS, C_OP_T, "jxs", NULL, ARG_SHORT7 | ARG_RELATIVE | ARG_A_JUMP },
	{ 6, OP_JYS, OP_GR_JS, C_OP_T, "jys", NULL, ARG_SHORT7 | ARG_RELATIVE | ARG_A_JUMP },
	{ 7, OP_JCS, OP_GR_JS, C_OP_T, "jcs", NULL, ARG_SHORT7 | ARG_RELATIVE | ARG_A_JUMP }
};

struct opdef iset_71[] = {
	{ 0, OP_BLC, OP_GR_KA2, C_OP_B, "blc", NULL, ARG_SHORT8 },
	{ 1, OP_EXL, OP_GR_KA2, C_OP_B, "exl", NULL, ARG_SHORT8 },
	{ 2, OP_BRC, OP_GR_KA2, C_OP_B, "brc", NULL, ARG_SHORT8 },
	{ 3, OP_NRF, OP_GR_KA2, C_OP_B, "nrf", NULL, ARG_SHORT8 }
};

struct opdef iset_72[] = {
	{ 0b0000000, OP_RIC, OP_GR_C, C_OP_R, "ric", NULL, ARG_REG },
	{ 0b0000001, OP_ZLB, OP_GR_C, C_OP_R, "zlb", NULL, ARG_REG },
	{ 0b0000010, OP_SXU, OP_GR_C, C_OP_R, "sxu", NULL, ARG_REG },
	{ 0b0000011, OP_NGA, OP_GR_C, C_OP_R, "nga", NULL, ARG_REG },
	{ 0b0000100, OP_SLZ, OP_GR_C, C_OP_R, "slz", NULL, ARG_REG },
	{ 0b0000101, OP_SLY, OP_GR_C, C_OP_R, "sly", NULL, ARG_REG },
	{ 0b0000110, OP_SLX, OP_GR_C, C_OP_R, "slx", NULL, ARG_REG },
	{ 0b0000111, OP_SRY, OP_GR_C, C_OP_R, "sry", NULL, ARG_REG },
	{ 0b0001000, OP_NGL, OP_GR_C, C_OP_R, "ngl", NULL, ARG_REG },
	{ 0b0001001, OP_RPC, OP_GR_C, C_OP_R, "rpc", NULL, ARG_REG },
	{ 0b0001010, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b0001011, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b0001100, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b0001101, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b0001110, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b0001111, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b0010000, OP_SHC, OP_GR_C, C_OP_t, "shc", NULL, ARG_REG | ARG_SHORT4 },
	{ 0b0010001, OP_SHC, OP_GR_C, C_OP_t, "shc", NULL, ARG_REG | ARG_SHORT4 },
	{ 0b0010010, OP_SHC, OP_GR_C, C_OP_t, "shc", NULL, ARG_REG | ARG_SHORT4 },
	{ 0b0010011, OP_SHC, OP_GR_C, C_OP_t, "shc", NULL, ARG_REG | ARG_SHORT4 },
	{ 0b0010100, OP_SHC, OP_GR_C, C_OP_t, "shc", NULL, ARG_REG | ARG_SHORT4 },
	{ 0b0010101, OP_SHC, OP_GR_C, C_OP_t, "shc", NULL, ARG_REG | ARG_SHORT4 },
	{ 0b0010110, OP_SHC, OP_GR_C, C_OP_t, "shc", NULL, ARG_REG | ARG_SHORT4 },
	{ 0b0010111, OP_SHC, OP_GR_C, C_OP_t, "shc", NULL, ARG_REG | ARG_SHORT4 },
	{ 0b0011000, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b0011001, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b0011010, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b0011011, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b0011100, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b0011101, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b0011110, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b0011111, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b0100000, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b0100001, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b0100010, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b0100011, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b0100100, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b0100101, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b0100110, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b0100111, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b0101000, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b0101001, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b0101010, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b0101011, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b0101100, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b0101101, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b0101110, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b0101111, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b0110000, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b0110001, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b0110010, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b0110011, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b0110100, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b0110101, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b0110110, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b0110111, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b0111000, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b0111001, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b0111010, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b0111011, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b0111100, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b0111101, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b0111110, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b0111111, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b1000000, OP_RKY, OP_GR_C, C_OP_R, "rky", NULL, ARG_REG },
	{ 0b1000001, OP_ZRB, OP_GR_C, C_OP_R, "zrb", NULL, ARG_REG },
	{ 0b1000010, OP_SXL, OP_GR_C, C_OP_R, "sxl", NULL, ARG_REG },
	{ 0b1000011, OP_NGC, OP_GR_C, C_OP_R, "ngc", NULL, ARG_REG },
	{ 0b1000100, OP_SVZ, OP_GR_C, C_OP_R, "svz", NULL, ARG_REG },
	{ 0b1000101, OP_SVY, OP_GR_C, C_OP_R, "svy", NULL, ARG_REG },
	{ 0b1000110, OP_SVX, OP_GR_C, C_OP_R, "svx", NULL, ARG_REG },
	{ 0b1000111, OP_SRX, OP_GR_C, C_OP_R, "srx", NULL, ARG_REG },
	{ 0b1001000, OP_SRZ, OP_GR_C, C_OP_R, "srz", NULL, ARG_REG },
	{ 0b1001001, OP_LPC, OP_GR_C, C_OP_R, "lpc", NULL, ARG_REG },
	{ 0b1001010, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b1001011, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b1001100, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b1001101, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b1001110, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b1001111, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b1010001, OP_SHC, OP_GR_C, C_OP_t, "shc", NULL, ARG_REG | ARG_SHORT4 },
	{ 0b1010000, OP_SHC, OP_GR_C, C_OP_t, "shc", NULL, ARG_REG | ARG_SHORT4 },
	{ 0b1010010, OP_SHC, OP_GR_C, C_OP_t, "shc", NULL, ARG_REG | ARG_SHORT4 },
	{ 0b1010011, OP_SHC, OP_GR_C, C_OP_t, "shc", NULL, ARG_REG | ARG_SHORT4 },
	{ 0b1010100, OP_SHC, OP_GR_C, C_OP_t, "shc", NULL, ARG_REG | ARG_SHORT4 },
	{ 0b1010101, OP_SHC, OP_GR_C, C_OP_t, "shc", NULL, ARG_REG | ARG_SHORT4 },
	{ 0b1010110, OP_SHC, OP_GR_C, C_OP_t, "shc", NULL, ARG_REG | ARG_SHORT4 },
	{ 0b1010111, OP_SHC, OP_GR_C, C_OP_t, "shc", NULL, ARG_REG | ARG_SHORT4 },
	{ 0b1011000, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b1011001, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b1011010, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b1011011, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b1011100, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b1011101, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b1011110, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b1011111, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b1100000, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b1100001, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b1100010, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b1100011, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b1100100, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b1100101, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b1100110, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b1100111, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b1101000, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b1101001, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b1101010, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b1101011, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b1101100, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b1101101, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b1101110, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b1101111, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b1110001, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b1110000, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b1110010, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b1110011, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b1110100, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b1110101, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b1110110, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b1110111, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b1111000, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b1111001, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b1111010, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b1111011, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b1111100, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b1111101, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b1111110, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 },
	{ 0b1111111, OP_ILL, OP_GR_NONE, C_DATA, NULL,  NULL, 0 }
};

struct opdef iset_73[] = {
	{ 0b0000000, OP_HLT, OP_GR_S, C_OP_T, "hlt", NULL, ARG_SHORT7 },
	{ 0b0000001, OP_HLT, OP_GR_S, C_OP_T, "hlt", NULL, ARG_SHORT7 },
	{ 0b0000010, OP_HLT, OP_GR_S, C_OP_T, "hlt", NULL, ARG_SHORT7 },
	{ 0b0000011, OP_HLT, OP_GR_S, C_OP_T, "hlt", NULL, ARG_SHORT7 },
	{ 0b0000100, OP_HLT, OP_GR_S, C_OP_T, "hlt", NULL, ARG_SHORT7 },
	{ 0b0000101, OP_HLT, OP_GR_S, C_OP_T, "hlt", NULL, ARG_SHORT7 },
	{ 0b0000110, OP_HLT, OP_GR_S, C_OP_T, "hlt", NULL, ARG_SHORT7 },
	{ 0b0000111, OP_HLT, OP_GR_S, C_OP_T, "hlt", NULL, ARG_SHORT7 },

	{ 0b0001000, OP_MCL, OP_GR_S, C_OP__, "mcl", NULL, ARG_NONE },
	{ 0b0001001, OP_MCL, OP_GR_S, C_OP__, "mcl", NULL, ARG_NONE },
	{ 0b0001010, OP_MCL, OP_GR_S, C_OP__, "mcl", NULL, ARG_NONE },
	{ 0b0001011, OP_MCL, OP_GR_S, C_OP__, "mcl", NULL, ARG_NONE },
	{ 0b0001100, OP_MCL, OP_GR_S, C_OP__, "mcl", NULL, ARG_NONE },
	{ 0b0001101, OP_MCL, OP_GR_S, C_OP__, "mcl", NULL, ARG_NONE },
	{ 0b0001110, OP_MCL, OP_GR_S, C_OP__, "mcl", NULL, ARG_NONE },
	{ 0b0001111, OP_MCL, OP_GR_S, C_OP__, "mcl", NULL, ARG_NONE },

	{ 0b0010000, OP_CIT, OP_GR_S, C_OP__, "cit",  NULL, ARG_NONE },
	{ 0b0010001, OP_SIL, OP_GR_S, C_OP__, "sil",  NULL, ARG_NONE },
	{ 0b0010010, OP_SIU, OP_GR_S, C_OP__, "siu",  NULL, ARG_NONE },
	{ 0b0010011, OP_SIT, OP_GR_S, C_OP__, "sit",  NULL, ARG_NONE },
	{ 0b0010100, OP_SINT, OP_GR_S, C_OP__, "sint", NULL, ARG_NONE },
	{ 0b0010101, OP_ILL, OP_GR_NONE, C_DATA, NULL,   NULL, 0 },
	{ 0b0010110, OP_ILL, OP_GR_NONE, C_DATA, NULL,   NULL, 0 },
	{ 0b0010111, OP_ILL, OP_GR_NONE, C_DATA, NULL,   NULL, 0 },

	{ 0b0011000, OP_GIU, OP_GR_S, C_OP__, "giu", NULL, ARG_NONE },
	{ 0b0011001, OP_GIU, OP_GR_S, C_OP__, "giu", NULL, ARG_NONE },
	{ 0b0011010, OP_GIU, OP_GR_S, C_OP__, "giu", NULL, ARG_NONE },
	{ 0b0011011, OP_GIU, OP_GR_S, C_OP__, "giu", NULL, ARG_NONE },
	{ 0b0011100, OP_GIU, OP_GR_S, C_OP__, "giu", NULL, ARG_NONE },
	{ 0b0011101, OP_GIU, OP_GR_S, C_OP__, "giu", NULL, ARG_NONE },
	{ 0b0011110, OP_GIU, OP_GR_S, C_OP__, "giu", NULL, ARG_NONE },
	{ 0b0011111, OP_GIU, OP_GR_S, C_OP__, "giu", NULL, ARG_NONE },

	{ 0b0100000, OP_LIP, OP_GR_S, C_OP__, "lip", NULL, ARG_NONE },
	{ 0b0100001, OP_LIP, OP_GR_S, C_OP__, "lip", NULL, ARG_NONE },
	{ 0b0100010, OP_LIP, OP_GR_S, C_OP__, "lip", NULL, ARG_NONE },
	{ 0b0100011, OP_LIP, OP_GR_S, C_OP__, "lip", NULL, ARG_NONE },
	{ 0b0100100, OP_LIP, OP_GR_S, C_OP__, "lip", NULL, ARG_NONE },
	{ 0b0100101, OP_LIP, OP_GR_S, C_OP__, "lip", NULL, ARG_NONE },
	{ 0b0100110, OP_LIP, OP_GR_S, C_OP__, "lip", NULL, ARG_NONE },
	{ 0b0100111, OP_LIP, OP_GR_S, C_OP__, "lip", NULL, ARG_NONE },

	{ 0b0101000, OP_CRON, OP_GR_S, C_OP__, "cron", NULL, ARG_NONE },
	{ 0b0101001, OP_ILL, OP_GR_NONE, C_DATA, NULL,   NULL, 0 },
	{ 0b0101010, OP_ILL, OP_GR_NONE, C_DATA, NULL,   NULL, 0 },
	{ 0b0101011, OP_ILL, OP_GR_NONE, C_DATA, NULL,   NULL, 0 },
	{ 0b0101100, OP_ILL, OP_GR_NONE, C_DATA, NULL,   NULL, 0 },
	{ 0b0101101, OP_ILL, OP_GR_NONE, C_DATA, NULL,   NULL, 0 },
	{ 0b0101110, OP_ILL, OP_GR_NONE, C_DATA, NULL,   NULL, 0 },
	{ 0b0101111, OP_ILL, OP_GR_NONE, C_DATA, NULL,   NULL, 0 },

	{ 0b0110000, OP_ILL, OP_GR_NONE, C_DATA, NULL, NULL, 0 },
	{ 0b0110001, OP_ILL, OP_GR_NONE, C_DATA, NULL, NULL, 0 },
	{ 0b0110010, OP_ILL, OP_GR_NONE, C_DATA, NULL, NULL, 0 },
	{ 0b0110011, OP_ILL, OP_GR_NONE, C_DATA, NULL, NULL, 0 },
	{ 0b0110100, OP_ILL, OP_GR_NONE, C_DATA, NULL, NULL, 0 },
	{ 0b0110101, OP_ILL, OP_GR_NONE, C_DATA, NULL, NULL, 0 },
	{ 0b0110110, OP_ILL, OP_GR_NONE, C_DATA, NULL, NULL, 0 },
	{ 0b0110111, OP_ILL, OP_GR_NONE, C_DATA, NULL, NULL, 0 },

	{ 0b0111000, OP_ILL, OP_GR_NONE, C_DATA, NULL, NULL, 0 },
	{ 0b0111001, OP_ILL, OP_GR_NONE, C_DATA, NULL, NULL, 0 },
	{ 0b0111010, OP_ILL, OP_GR_NONE, C_DATA, NULL, NULL, 0 },
	{ 0b0111011, OP_ILL, OP_GR_NONE, C_DATA, NULL, NULL, 0 },
	{ 0b0111100, OP_ILL, OP_GR_NONE, C_DATA, NULL, NULL, 0 },
	{ 0b0111101, OP_ILL, OP_GR_NONE, C_DATA, NULL, NULL, 0 },
	{ 0b0111110, OP_ILL, OP_GR_NONE, C_DATA, NULL, NULL, 0 },
	{ 0b0111111, OP_ILL, OP_GR_NONE, C_DATA, NULL, NULL, 0 },

	{ 0b1000000, OP_HLT, OP_GR_S, C_OP_T, "hlt", NULL, ARG_SHORT7 },
	{ 0b1000001, OP_HLT, OP_GR_S, C_OP_T, "hlt", NULL, ARG_SHORT7 },
	{ 0b1000010, OP_HLT, OP_GR_S, C_OP_T, "hlt", NULL, ARG_SHORT7 },
	{ 0b1000011, OP_HLT, OP_GR_S, C_OP_T, "hlt", NULL, ARG_SHORT7 },
	{ 0b1000100, OP_HLT, OP_GR_S, C_OP_T, "hlt", NULL, ARG_SHORT7 },
	{ 0b1000101, OP_HLT, OP_GR_S, C_OP_T, "hlt", NULL, ARG_SHORT7 },
	{ 0b1000110, OP_HLT, OP_GR_S, C_OP_T, "hlt", NULL, ARG_SHORT7 },
	{ 0b1000111, OP_HLT, OP_GR_S, C_OP_T, "hlt", NULL, ARG_SHORT7 },

	{ 0b1001000, OP_MCL, OP_GR_S, C_OP__, "mcl", NULL, ARG_NONE },
	{ 0b1001001, OP_MCL, OP_GR_S, C_OP__, "mcl", NULL, ARG_NONE },
	{ 0b1001010, OP_MCL, OP_GR_S, C_OP__, "mcl", NULL, ARG_NONE },
	{ 0b1001011, OP_MCL, OP_GR_S, C_OP__, "mcl", NULL, ARG_NONE },
	{ 0b1001100, OP_MCL, OP_GR_S, C_OP__, "mcl", NULL, ARG_NONE },
	{ 0b1001101, OP_MCL, OP_GR_S, C_OP__, "mcl", NULL, ARG_NONE },
	{ 0b1001110, OP_MCL, OP_GR_S, C_OP__, "mcl", NULL, ARG_NONE },
	{ 0b1001111, OP_MCL, OP_GR_S, C_OP__, "mcl", NULL, ARG_NONE },

	{ 0b1010000, OP_CIT, OP_GR_S, C_OP__, "cit",  NULL, ARG_NONE },
	{ 0b1010001, OP_SIL, OP_GR_S, C_OP__, "sil",  NULL, ARG_NONE },
	{ 0b1010010, OP_SIU, OP_GR_S, C_OP__, "siu",  NULL, ARG_NONE },
	{ 0b1010011, OP_SIT, OP_GR_S, C_OP__, "sit",  NULL, ARG_NONE },
	{ 0b1010100, OP_SIND, OP_GR_S, C_OP__, "sind", NULL, ARG_NONE },
	{ 0b1010101, OP_ILL, OP_GR_NONE, C_DATA, NULL,   NULL, 0 },
	{ 0b1010110, OP_ILL, OP_GR_NONE, C_DATA, NULL,   NULL, 0 },
	{ 0b1010111, OP_ILL, OP_GR_NONE, C_DATA, NULL,   NULL, 0 },

	{ 0b1011000, OP_GIL, OP_GR_S, C_OP__, "gil", NULL, ARG_NONE },
	{ 0b1011001, OP_GIL, OP_GR_S, C_OP__, "gil", NULL, ARG_NONE },
	{ 0b1011010, OP_GIL, OP_GR_S, C_OP__, "gil", NULL, ARG_NONE },
	{ 0b1011011, OP_GIL, OP_GR_S, C_OP__, "gil", NULL, ARG_NONE },
	{ 0b1011100, OP_GIL, OP_GR_S, C_OP__, "gil", NULL, ARG_NONE },
	{ 0b1011101, OP_GIL, OP_GR_S, C_OP__, "gil", NULL, ARG_NONE },
	{ 0b1011110, OP_GIL, OP_GR_S, C_OP__, "gil", NULL, ARG_NONE },
	{ 0b1011111, OP_GIL, OP_GR_S, C_OP__, "gil", NULL, ARG_NONE },

	{ 0b1100001, OP_LIP, OP_GR_S, C_OP__, "lip", NULL, ARG_NONE },
	{ 0b1100000, OP_LIP, OP_GR_S, C_OP__, "lip", NULL, ARG_NONE },
	{ 0b1100010, OP_LIP, OP_GR_S, C_OP__, "lip", NULL, ARG_NONE },
	{ 0b1100011, OP_LIP, OP_GR_S, C_OP__, "lip", NULL, ARG_NONE },
	{ 0b1100101, OP_LIP, OP_GR_S, C_OP__, "lip", NULL, ARG_NONE },
	{ 0b1100100, OP_LIP, OP_GR_S, C_OP__, "lip", NULL, ARG_NONE },
	{ 0b1100110, OP_LIP, OP_GR_S, C_OP__, "lip", NULL, ARG_NONE },
	{ 0b1100111, OP_LIP, OP_GR_S, C_OP__, "lip", NULL, ARG_NONE },

	{ 0b1101000, OP_ILL, OP_GR_NONE, C_DATA, NULL, NULL, 0 },
	{ 0b1101001, OP_ILL, OP_GR_NONE, C_DATA, NULL, NULL, 0 },
	{ 0b1101010, OP_ILL, OP_GR_NONE, C_DATA, NULL, NULL, 0 },
	{ 0b1101011, OP_ILL, OP_GR_NONE, C_DATA, NULL, NULL, 0 },
	{ 0b1101100, OP_ILL, OP_GR_NONE, C_DATA, NULL, NULL, 0 },
	{ 0b1101101, OP_ILL, OP_GR_NONE, C_DATA, NULL, NULL, 0 },
	{ 0b1101110, OP_ILL, OP_GR_NONE, C_DATA, NULL, NULL, 0 },
	{ 0b1101111, OP_ILL, OP_GR_NONE, C_DATA, NULL, NULL, 0 },

	{ 0b1110000, OP_ILL, OP_GR_NONE, C_DATA, NULL, NULL, 0 },
	{ 0b1110001, OP_ILL, OP_GR_NONE, C_DATA, NULL, NULL, 0 },
	{ 0b1110010, OP_ILL, OP_GR_NONE, C_DATA, NULL, NULL, 0 },
	{ 0b1110011, OP_ILL, OP_GR_NONE, C_DATA, NULL, NULL, 0 },
	{ 0b1110100, OP_ILL, OP_GR_NONE, C_DATA, NULL, NULL, 0 },
	{ 0b1110101, OP_ILL, OP_GR_NONE, C_DATA, NULL, NULL, 0 },
	{ 0b1110110, OP_ILL, OP_GR_NONE, C_DATA, NULL, NULL, 0 },
	{ 0b1110111, OP_ILL, OP_GR_NONE, C_DATA, NULL, NULL, 0 },

	{ 0b1111000, OP_ILL, OP_GR_NONE, C_DATA, NULL, NULL, 0 },
	{ 0b1111001, OP_ILL, OP_GR_NONE, C_DATA, NULL, NULL, 0 },
	{ 0b1111010, OP_ILL, OP_GR_NONE, C_DATA, NULL, NULL, 0 },
	{ 0b1111011, OP_ILL, OP_GR_NONE, C_DATA, NULL, NULL, 0 },
	{ 0b1111100, OP_ILL, OP_GR_NONE, C_DATA, NULL, NULL, 0 },
	{ 0b1111101, OP_ILL, OP_GR_NONE, C_DATA, NULL, NULL, 0 },
	{ 0b1111110, OP_ILL, OP_GR_NONE, C_DATA, NULL, NULL, 0 },
	{ 0b1111111, OP_ILL, OP_GR_NONE, C_DATA, NULL, NULL, 0 }

};

struct opdef iset_74[] = {
	{ 0, OP_UJ, OP_GR_J, C_OP_N, "uj", NULL, ARG_NORM | ARG_A_JUMP },
	{ 1, OP_JL, OP_GR_J, C_OP_N, "jl", NULL, ARG_NORM | ARG_A_JUMP },
	{ 2, OP_JE, OP_GR_J, C_OP_N, "je", NULL, ARG_NORM | ARG_A_JUMP },
	{ 3, OP_JG, OP_GR_J, C_OP_N, "jg", NULL, ARG_NORM | ARG_A_JUMP },
	{ 4, OP_JZ, OP_GR_J, C_OP_N, "jz", NULL, ARG_NORM | ARG_A_JUMP },
	{ 5, OP_JM, OP_GR_J, C_OP_N, "jm", NULL, ARG_NORM | ARG_A_JUMP },
	{ 6, OP_JN, OP_GR_J, C_OP_N, "jn", NULL, ARG_NORM | ARG_A_JUMP },
	{ 7, OP_LJ, OP_GR_J, C_OP_N, "lj", NULL, ARG_NORM | ARG_A_JUMP }
};

struct opdef iset_75[] = {
	{ 0, OP_LD, OP_GR_L, C_OP_N, "ld", NULL, ARG_NORM | ARG_A_WORD },
	{ 1, OP_LF, OP_GR_L, C_OP_N, "lf", NULL, ARG_NORM | ARG_A_WORD },
	{ 2, OP_LA, OP_GR_L, C_OP_N, "la", NULL, ARG_NORM | ARG_A_WORD },
	{ 3, OP_LL, OP_GR_L, C_OP_N, "ll", NULL, ARG_NORM | ARG_A_WORD },
	{ 4, OP_TD, OP_GR_L, C_OP_N, "td", NULL, ARG_NORM | ARG_A_WORD },
	{ 5, OP_TF, OP_GR_L, C_OP_N, "tf", NULL, ARG_NORM | ARG_A_WORD },
	{ 6, OP_TA, OP_GR_L, C_OP_N, "ta", NULL, ARG_NORM | ARG_A_WORD },
	{ 7, OP_TL, OP_GR_L, C_OP_N, "tl", NULL, ARG_NORM | ARG_A_WORD }
};

struct opdef iset_76[] = {
	{ 0, OP_RD, OP_GR_G, C_OP_N, "rd", NULL, ARG_NORM | ARG_A_WORD },
	{ 1, OP_RF, OP_GR_G, C_OP_N, "rf", NULL, ARG_NORM | ARG_A_WORD },
	{ 2, OP_RA, OP_GR_G, C_OP_N, "ra", NULL, ARG_NORM | ARG_A_WORD },
	{ 3, OP_RL, OP_GR_G, C_OP_N, "rl", NULL, ARG_NORM | ARG_A_WORD },
	{ 4, OP_PD, OP_GR_G, C_OP_N, "pd", NULL, ARG_NORM | ARG_A_WORD },
	{ 5, OP_PF, OP_GR_G, C_OP_N, "pf", NULL, ARG_NORM | ARG_A_WORD },
	{ 6, OP_PA, OP_GR_G, C_OP_N, "pa", NULL, ARG_NORM | ARG_A_WORD },
	{ 7, OP_PL, OP_GR_G, C_OP_N, "pl", NULL, ARG_NORM | ARG_A_WORD }
};

struct opdef iset_77[] = {
	{ 0, OP_MB, OP_GR_BN, C_OP_N, "mb", NULL, ARG_NORM | ARG_A_WORD },
	{ 1, OP_IM, OP_GR_BN, C_OP_N, "im", NULL, ARG_NORM | ARG_A_WORD },
	{ 2, OP_KI, OP_GR_BN, C_OP_N, "ki", NULL, ARG_NORM | ARG_A_WORD },
	{ 3, OP_FI, OP_GR_BN, C_OP_N, "fi", NULL, ARG_NORM | ARG_A_WORD },
	{ 4, OP_SP, OP_GR_BN, C_OP_N, "sp", NULL, ARG_NORM | ARG_A_WORD },
	{ 5, OP_MD, OP_GR_BN, C_OP_N, "md", NULL, ARG_NORM },
	{ 6, OP_RZ, OP_GR_BN, C_OP_N, "rz", NULL, ARG_NORM | ARG_A_WORD },
	{ 7, OP_IB, OP_GR_BN, C_OP_N, "ib", NULL, ARG_NORM | ARG_A_WORD }
};

// -----------------------------------------------------------------------
struct opdef * get_op(uint16_t opcode)
{
	struct opdef *opdef = iset + _OP(opcode);
	if (!opdef->extop_fun) {
		return opdef;
	} else {
		return opdef->extop_fun(opcode);
	}
}

// -----------------------------------------------------------------------
struct opdef * get_extop_37(uint16_t opcode)
{
	return iset_37 + _A(opcode);
}

// -----------------------------------------------------------------------
struct opdef * get_extop_70(uint16_t opcode)
{
	return iset_70 + _A(opcode);
}

// -----------------------------------------------------------------------
struct opdef * get_extop_71(uint16_t opcode)
{
	return iset_71 + ((opcode & 0b0000001100000000) >> 8);
}

// -----------------------------------------------------------------------
struct opdef * get_extop_72(uint16_t opcode)
{
	return iset_72 + (((opcode & 0b0000001000000000) >> 3) | (opcode & 0b0000000000111111));
}

// -----------------------------------------------------------------------
struct opdef * get_extop_73(uint16_t opcode)
{
	return iset_73 + (((opcode & 0b0000001111000000) >> 3) | (opcode & 0b0000000000000111));
}

// -----------------------------------------------------------------------
struct opdef * get_extop_74(uint16_t opcode)
{
	return iset_74 + _A(opcode);
}

// -----------------------------------------------------------------------
struct opdef * get_extop_75(uint16_t opcode)
{
	return iset_75 + _A(opcode);
}

// -----------------------------------------------------------------------
struct opdef * get_extop_76(uint16_t opcode)
{
	return iset_76 + _A(opcode);
}

// -----------------------------------------------------------------------
struct opdef * get_extop_77(uint16_t opcode)
{
	return iset_77 + _A(opcode);
}


// vim: tabstop=4 shiftwidth=4 autoindent
