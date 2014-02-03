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
	{ 000, C_DATA, NULL, NULL, 0 },
	{ 001, C_DATA, NULL, NULL, 0 },
	{ 002, C_DATA, NULL, NULL, 0 },
	{ 003, C_DATA, NULL, NULL, 0 },
	{ 004, C_DATA, NULL, NULL, 0 },
	{ 005, C_DATA, NULL, NULL, 0 },
	{ 006, C_DATA, NULL, NULL, 0 },
	{ 007, C_DATA, NULL, NULL, 0 },
	{ 010, C_DATA, NULL, NULL, 0 },
	{ 011, C_DATA, NULL, NULL, 0 },
	{ 012, C_DATA, NULL, NULL, 0 },
	{ 013, C_DATA, NULL, NULL, 0 },
	{ 014, C_DATA, NULL, NULL, 0 },
	{ 015, C_DATA, NULL, NULL, 0 },
	{ 016, C_DATA, NULL, NULL, 0 },
	{ 017, C_DATA, NULL, NULL, 0 },
	
	{ 020, C_OP_RN, "lw", NULL, F_NORM },
	{ 021, C_OP_RN, "tw", NULL, F_NORM | F_ADDR },
	{ 022, C_OP_RN, "ls", NULL, F_NORM },
	{ 023, C_OP_RN, "ri", NULL, F_NORM | F_ADDR },
	{ 024, C_OP_RN, "rw", NULL, F_NORM | F_ADDR },
	{ 025, C_OP_RN, "pw", NULL, F_NORM | F_ADDR },
	{ 026, C_OP_RN, "rj", NULL, F_NORM | F_ADDR },
	{ 027, C_OP_RN, "is", NULL, F_NORM | F_ADDR },
	{ 030, C_OP_RN, "bb", NULL, F_NORM },
	{ 031, C_OP_RN, "bm", NULL, F_NORM | F_ADDR },
	{ 032, C_OP_RN, "bs", NULL, F_NORM },
	{ 033, C_OP_RN, "bc", NULL, F_NORM },
	{ 034, C_OP_RN, "bn", NULL, F_NORM },
	{ 035, C_OP_RN, "ou", NULL, F_NORM | F_IO },
	{ 036, C_OP_RN, "in", NULL, F_NORM | F_IO },

	{ 037, C_OP_N, NULL, get_extop_37 },

	{ 040, C_OP_RN, "aw", NULL, F_NORM },
	{ 041, C_OP_RN, "ac", NULL, F_NORM },
	{ 042, C_OP_RN, "sw", NULL, F_NORM },
	{ 043, C_OP_RN, "cw", NULL, F_NORM },
	{ 044, C_OP_RN, "or", NULL, F_NORM },
	{ 045, C_OP_RN, "om", NULL, F_NORM | F_ADDR },
	{ 046, C_OP_RN, "nr", NULL, F_NORM },
	{ 047, C_OP_RN, "nm", NULL, F_NORM | F_ADDR },
	{ 050, C_OP_RN, "er", NULL, F_NORM },
	{ 051, C_OP_RN, "em", NULL, F_NORM | F_ADDR },
	{ 052, C_OP_RN, "xr", NULL, F_NORM },
	{ 053, C_OP_RN, "xm", NULL, F_NORM | F_ADDR },
	{ 054, C_OP_RN, "cl", NULL, F_NORM },
	{ 055, C_OP_RN, "lb", NULL, F_BNORM },
	{ 056, C_OP_RN, "rb", NULL, F_BNORM },
	{ 057, C_OP_RN, "cb", NULL, F_BNORM },

	{ 060, C_OP_RT, "awt", NULL, F_SHORT },
	{ 061, C_OP_RT, "trb", NULL, F_SHORT },
	{ 062, C_OP_RT, "irb", NULL, F_SHORT | F_RELATIVE | F_JUMP },
	{ 063, C_OP_RT, "drb", NULL, F_SHORT | F_RELATIVE | F_JUMP },
	{ 064, C_OP_RT, "cwt", NULL, F_SHORT },
	{ 065, C_OP_RT, "lwt", NULL, F_SHORT },
	{ 066, C_OP_RT, "lws", NULL, F_SHORT | F_RELATIVE | F_ADDR },
	{ 067, C_OP_RT, "rws", NULL, F_SHORT | F_RELATIVE | F_ADDR },

	{ 070, C_OP_T, NULL, get_extop_70, 0 },
	{ 071, C_OP_B, NULL, get_extop_71, 0 },
	{ 072, C_OP_R, NULL, get_extop_72, 0 },
	{ 073, C_OP__, NULL, get_extop_73, 0 },
	{ 074, C_OP_N, NULL, get_extop_74, 0 },
	{ 075, C_OP_N, NULL, get_extop_75, 0 },
	{ 076, C_OP_N, NULL, get_extop_76, 0 },
	{ 077, C_OP_N, NULL, get_extop_77, 0 }
};

struct opdef iset_37[] = {
	{ 0, C_OP_N, "ad", NULL, F_NORM | F_ADDR | F_ADWORD },
	{ 1, C_OP_N, "sd", NULL, F_NORM | F_ADDR | F_ADWORD },
	{ 2, C_OP_N, "mw", NULL, F_NORM | F_ADDR | F_ADWORD },
	{ 3, C_OP_N, "dw", NULL, F_NORM | F_ADDR | F_ADWORD },
	{ 4, C_OP_N, "af", NULL, F_NORM | F_ADDR | F_AFLOAT },
	{ 5, C_OP_N, "sf", NULL, F_NORM | F_ADDR | F_AFLOAT },
	{ 6, C_OP_N, "mf", NULL, F_NORM | F_ADDR | F_AFLOAT },
	{ 7, C_OP_N, "df", NULL, F_NORM | F_ADDR | F_AFLOAT }
};

struct opdef iset_70[] = {
	{ 0, C_OP_T, "ujs", NULL, F_SHORT | F_RELATIVE | F_JUMP },
	{ 1, C_OP_T, "jls", NULL, F_SHORT | F_RELATIVE | F_JUMP },
	{ 2, C_OP_T, "jes", NULL, F_SHORT | F_RELATIVE | F_JUMP },
	{ 3, C_OP_T, "jgs", NULL, F_SHORT | F_RELATIVE | F_JUMP },
	{ 4, C_OP_T, "jvs", NULL, F_SHORT | F_RELATIVE | F_JUMP },
	{ 5, C_OP_T, "jxs", NULL, F_SHORT | F_RELATIVE | F_JUMP },
	{ 6, C_OP_T, "jys", NULL, F_SHORT | F_RELATIVE | F_JUMP },
	{ 7, C_OP_T, "jcs", NULL, F_SHORT | F_RELATIVE | F_JUMP }
};

struct opdef iset_71[] = {
	{ 0, C_OP_B, "blc", NULL, F_BYTE },
	{ 1, C_OP_B, "exl", NULL, F_BYTE },
	{ 2, C_OP_B, "brc", NULL, F_BYTE },
	{ 3, C_OP_B, "nrf", NULL, F_BYTE }
};

struct opdef iset_72[] = {
	{ 0b0000000, C_OP_R, "ric", NULL, 0 },
	{ 0b0000001, C_OP_R, "zlb", NULL, 0 },
	{ 0b0000010, C_OP_R, "sxu", NULL, 0 },
	{ 0b0000011, C_OP_R, "nga", NULL, 0 },
	{ 0b0000100, C_OP_R, "slz", NULL, 0 },
	{ 0b0000101, C_OP_R, "sly", NULL, 0 },
	{ 0b0000110, C_OP_R, "slx", NULL, 0 },
	{ 0b0000111, C_OP_R, "sry", NULL, 0 },
	{ 0b0001000, C_OP_R, "ngl", NULL, 0 },
	{ 0b0001001, C_OP_R, "rpc", NULL, 0 },
	{ 0b0001010, C_DATA, NULL,  NULL, 0 },
	{ 0b0001011, C_DATA, NULL,  NULL, 0 },
	{ 0b0001100, C_DATA, NULL,  NULL, 0 },
	{ 0b0001101, C_DATA, NULL,  NULL, 0 },
	{ 0b0001110, C_DATA, NULL,  NULL, 0 },
	{ 0b0001111, C_DATA, NULL,  NULL, 0 },
	{ 0b0010000, C_OP_t, "shc", NULL, 0 },
	{ 0b0010001, C_OP_t, "shc", NULL, 0 },
	{ 0b0010010, C_OP_t, "shc", NULL, 0 },
	{ 0b0010011, C_OP_t, "shc", NULL, 0 },
	{ 0b0010100, C_OP_t, "shc", NULL, 0 },
	{ 0b0010101, C_OP_t, "shc", NULL, 0 },
	{ 0b0010110, C_OP_t, "shc", NULL, 0 },
	{ 0b0010111, C_OP_t, "shc", NULL, 0 },
	{ 0b0011000, C_DATA, NULL,  NULL, 0 },
	{ 0b0011001, C_DATA, NULL,  NULL, 0 },
	{ 0b0011010, C_DATA, NULL,  NULL, 0 },
	{ 0b0011011, C_DATA, NULL,  NULL, 0 },
	{ 0b0011100, C_DATA, NULL,  NULL, 0 },
	{ 0b0011101, C_DATA, NULL,  NULL, 0 },
	{ 0b0011110, C_DATA, NULL,  NULL, 0 },
	{ 0b0011111, C_DATA, NULL,  NULL, 0 },
	{ 0b0100000, C_DATA, NULL,  NULL, 0 },
	{ 0b0100001, C_DATA, NULL,  NULL, 0 },
	{ 0b0100010, C_DATA, NULL,  NULL, 0 },
	{ 0b0100011, C_DATA, NULL,  NULL, 0 },
	{ 0b0100100, C_DATA, NULL,  NULL, 0 },
	{ 0b0100101, C_DATA, NULL,  NULL, 0 },
	{ 0b0100110, C_DATA, NULL,  NULL, 0 },
	{ 0b0100111, C_DATA, NULL,  NULL, 0 },
	{ 0b0101000, C_DATA, NULL,  NULL, 0 },
	{ 0b0101001, C_DATA, NULL,  NULL, 0 },
	{ 0b0101010, C_DATA, NULL,  NULL, 0 },
	{ 0b0101011, C_DATA, NULL,  NULL, 0 },
	{ 0b0101100, C_DATA, NULL,  NULL, 0 },
	{ 0b0101101, C_DATA, NULL,  NULL, 0 },
	{ 0b0101110, C_DATA, NULL,  NULL, 0 },
	{ 0b0101111, C_DATA, NULL,  NULL, 0 },
	{ 0b0110000, C_DATA, NULL,  NULL, 0 },
	{ 0b0110001, C_DATA, NULL,  NULL, 0 },
	{ 0b0110010, C_DATA, NULL,  NULL, 0 },
	{ 0b0110011, C_DATA, NULL,  NULL, 0 },
	{ 0b0110100, C_DATA, NULL,  NULL, 0 },
	{ 0b0110101, C_DATA, NULL,  NULL, 0 },
	{ 0b0110110, C_DATA, NULL,  NULL, 0 },
	{ 0b0110111, C_DATA, NULL,  NULL, 0 },
	{ 0b0111000, C_DATA, NULL,  NULL, 0 },
	{ 0b0111001, C_DATA, NULL,  NULL, 0 },
	{ 0b0111010, C_DATA, NULL,  NULL, 0 },
	{ 0b0111011, C_DATA, NULL,  NULL, 0 },
	{ 0b0111100, C_DATA, NULL,  NULL, 0 },
	{ 0b0111101, C_DATA, NULL,  NULL, 0 },
	{ 0b0111110, C_DATA, NULL,  NULL, 0 },
	{ 0b0111111, C_DATA, NULL,  NULL, 0 },
	{ 0b1000000, C_OP_R, "rky", NULL, 0 },
	{ 0b1000001, C_OP_R, "zrb", NULL, 0 },
	{ 0b1000010, C_OP_R, "sxl", NULL, 0 },
	{ 0b1000011, C_OP_R, "ngc", NULL, 0 },
	{ 0b1000100, C_OP_R, "svz", NULL, 0 },
	{ 0b1000101, C_OP_R, "svy", NULL, 0 },
	{ 0b1000110, C_OP_R, "svx", NULL, 0 },
	{ 0b1000111, C_OP_R, "srx", NULL, 0 },
	{ 0b1001000, C_OP_R, "srz", NULL, 0 },
	{ 0b1001001, C_OP_R, "lpc", NULL, 0 },
	{ 0b1001010, C_DATA, NULL,  NULL, 0 },
	{ 0b1001011, C_DATA, NULL,  NULL, 0 },
	{ 0b1001100, C_DATA, NULL,  NULL, 0 },
	{ 0b1001101, C_DATA, NULL,  NULL, 0 },
	{ 0b1001110, C_DATA, NULL,  NULL, 0 },
	{ 0b1001111, C_DATA, NULL,  NULL, 0 },
	{ 0b1010001, C_OP_t, "shc", NULL, 0 },
	{ 0b1010000, C_OP_t, "shc", NULL, 0 },
	{ 0b1010010, C_OP_t, "shc", NULL, 0 },
	{ 0b1010011, C_OP_t, "shc", NULL, 0 },
	{ 0b1010100, C_OP_t, "shc", NULL, 0 },
	{ 0b1010101, C_OP_t, "shc", NULL, 0 },
	{ 0b1010110, C_OP_t, "shc", NULL, 0 },
	{ 0b1010111, C_OP_t, "shc", NULL, 0 },
	{ 0b1011000, C_DATA, NULL,  NULL, 0 },
	{ 0b1011001, C_DATA, NULL,  NULL, 0 },
	{ 0b1011010, C_DATA, NULL,  NULL, 0 },
	{ 0b1011011, C_DATA, NULL,  NULL, 0 },
	{ 0b1011100, C_DATA, NULL,  NULL, 0 },
	{ 0b1011101, C_DATA, NULL,  NULL, 0 },
	{ 0b1011110, C_DATA, NULL,  NULL, 0 },
	{ 0b1011111, C_DATA, NULL,  NULL, 0 },
	{ 0b1100000, C_DATA, NULL,  NULL, 0 },
	{ 0b1100001, C_DATA, NULL,  NULL, 0 },
	{ 0b1100010, C_DATA, NULL,  NULL, 0 },
	{ 0b1100011, C_DATA, NULL,  NULL, 0 },
	{ 0b1100100, C_DATA, NULL,  NULL, 0 },
	{ 0b1100101, C_DATA, NULL,  NULL, 0 },
	{ 0b1100110, C_DATA, NULL,  NULL, 0 },
	{ 0b1100111, C_DATA, NULL,  NULL, 0 },
	{ 0b1101000, C_DATA, NULL,  NULL, 0 },
	{ 0b1101001, C_DATA, NULL,  NULL, 0 },
	{ 0b1101010, C_DATA, NULL,  NULL, 0 },
	{ 0b1101011, C_DATA, NULL,  NULL, 0 },
	{ 0b1101100, C_DATA, NULL,  NULL, 0 },
	{ 0b1101101, C_DATA, NULL,  NULL, 0 },
	{ 0b1101110, C_DATA, NULL,  NULL, 0 },
	{ 0b1101111, C_DATA, NULL,  NULL, 0 },
	{ 0b1110001, C_DATA, NULL,  NULL, 0 },
	{ 0b1110000, C_DATA, NULL,  NULL, 0 },
	{ 0b1110010, C_DATA, NULL,  NULL, 0 },
	{ 0b1110011, C_DATA, NULL,  NULL, 0 },
	{ 0b1110100, C_DATA, NULL,  NULL, 0 },
	{ 0b1110101, C_DATA, NULL,  NULL, 0 },
	{ 0b1110110, C_DATA, NULL,  NULL, 0 },
	{ 0b1110111, C_DATA, NULL,  NULL, 0 },
	{ 0b1111000, C_DATA, NULL,  NULL, 0 },
	{ 0b1111001, C_DATA, NULL,  NULL, 0 },
	{ 0b1111010, C_DATA, NULL,  NULL, 0 },
	{ 0b1111011, C_DATA, NULL,  NULL, 0 },
	{ 0b1111100, C_DATA, NULL,  NULL, 0 },
	{ 0b1111101, C_DATA, NULL,  NULL, 0 },
	{ 0b1111110, C_DATA, NULL,  NULL, 0 },
	{ 0b1111111, C_DATA, NULL,  NULL, 0 }
};

struct opdef iset_73[] = {
	{ 0b0000000, C_OP_T, "hlt", NULL, F_NOARG },
	{ 0b0000001, C_OP_T, "hlt", NULL, F_NOARG },
	{ 0b0000010, C_OP_T, "hlt", NULL, F_NOARG },
	{ 0b0000011, C_OP_T, "hlt", NULL, F_NOARG },
	{ 0b0000100, C_OP_T, "hlt", NULL, F_NOARG },
	{ 0b0000101, C_OP_T, "hlt", NULL, F_NOARG },
	{ 0b0000110, C_OP_T, "hlt", NULL, F_NOARG },
	{ 0b0000111, C_OP_T, "hlt", NULL, F_NOARG },

	{ 0b0001000, C_OP__, "mcl", NULL, F_NOARG },
	{ 0b0001001, C_OP__, "mcl", NULL, F_NOARG },
	{ 0b0001010, C_OP__, "mcl", NULL, F_NOARG },
	{ 0b0001011, C_OP__, "mcl", NULL, F_NOARG },
	{ 0b0001100, C_OP__, "mcl", NULL, F_NOARG },
	{ 0b0001101, C_OP__, "mcl", NULL, F_NOARG },
	{ 0b0001110, C_OP__, "mcl", NULL, F_NOARG },
	{ 0b0001111, C_OP__, "mcl", NULL, F_NOARG },

	{ 0b0010000, C_OP__, "cit",  NULL, F_NOARG },
	{ 0b0010001, C_OP__, "sil",  NULL, F_NOARG },
	{ 0b0010010, C_OP__, "siu",  NULL, F_NOARG },
	{ 0b0010011, C_OP__, "sit",  NULL, F_NOARG },
	{ 0b0010100, C_OP__, "sint", NULL, F_NOARG },
	{ 0b0010101, C_DATA, NULL,   NULL, 0 },
	{ 0b0010110, C_DATA, NULL,   NULL, 0 },
	{ 0b0010111, C_DATA, NULL,   NULL, 0 },

	{ 0b0011000, C_OP__, "giu", NULL, 0 },
	{ 0b0011001, C_OP__, "giu", NULL, 0 },
	{ 0b0011010, C_OP__, "giu", NULL, 0 },
	{ 0b0011011, C_OP__, "giu", NULL, 0 },
	{ 0b0011100, C_OP__, "giu", NULL, 0 },
	{ 0b0011101, C_OP__, "giu", NULL, 0 },
	{ 0b0011110, C_OP__, "giu", NULL, 0 },
	{ 0b0011111, C_OP__, "giu", NULL, 0 },

	{ 0b0100000, C_OP__, "lip", NULL, 0 },
	{ 0b0100001, C_OP__, "lip", NULL, 0 },
	{ 0b0100010, C_OP__, "lip", NULL, 0 },
	{ 0b0100011, C_OP__, "lip", NULL, 0 },
	{ 0b0100100, C_OP__, "lip", NULL, 0 },
	{ 0b0100101, C_OP__, "lip", NULL, 0 },
	{ 0b0100110, C_OP__, "lip", NULL, 0 },
	{ 0b0100111, C_OP__, "lip", NULL, 0 },

	{ 0b0101000, C_OP__, "cron", NULL, F_NOARG },
	{ 0b0101001, C_DATA, NULL,   NULL, 0 },
	{ 0b0101010, C_DATA, NULL,   NULL, 0 },
	{ 0b0101011, C_DATA, NULL,   NULL, 0 },
	{ 0b0101100, C_DATA, NULL,   NULL, 0 },
	{ 0b0101101, C_DATA, NULL,   NULL, 0 },
	{ 0b0101110, C_DATA, NULL,   NULL, 0 },
	{ 0b0101111, C_DATA, NULL,   NULL, 0 },

	{ 0b0110000, C_DATA, NULL, NULL, 0 },
	{ 0b0110001, C_DATA, NULL, NULL, 0 },
	{ 0b0110010, C_DATA, NULL, NULL, 0 },
	{ 0b0110011, C_DATA, NULL, NULL, 0 },
	{ 0b0110100, C_DATA, NULL, NULL, 0 },
	{ 0b0110101, C_DATA, NULL, NULL, 0 },
	{ 0b0110110, C_DATA, NULL, NULL, 0 },
	{ 0b0110111, C_DATA, NULL, NULL, 0 },

	{ 0b0111000, C_DATA, NULL, NULL, 0 },
	{ 0b0111001, C_DATA, NULL, NULL, 0 },
	{ 0b0111010, C_DATA, NULL, NULL, 0 },
	{ 0b0111011, C_DATA, NULL, NULL, 0 },
	{ 0b0111100, C_DATA, NULL, NULL, 0 },
	{ 0b0111101, C_DATA, NULL, NULL, 0 },
	{ 0b0111110, C_DATA, NULL, NULL, 0 },
	{ 0b0111111, C_DATA, NULL, NULL, 0 },

	{ 0b1000000, C_OP_T, "hlt", NULL, F_NOARG },
	{ 0b1000001, C_OP_T, "hlt", NULL, F_NOARG },
	{ 0b1000010, C_OP_T, "hlt", NULL, F_NOARG },
	{ 0b1000011, C_OP_T, "hlt", NULL, F_NOARG },
	{ 0b1000100, C_OP_T, "hlt", NULL, F_NOARG },
	{ 0b1000101, C_OP_T, "hlt", NULL, F_NOARG },
	{ 0b1000110, C_OP_T, "hlt", NULL, F_NOARG },
	{ 0b1000111, C_OP_T, "hlt", NULL, F_NOARG },

	{ 0b1001000, C_OP__, "mcl", NULL, F_NOARG },
	{ 0b1001001, C_OP__, "mcl", NULL, F_NOARG },
	{ 0b1001010, C_OP__, "mcl", NULL, F_NOARG },
	{ 0b1001011, C_OP__, "mcl", NULL, F_NOARG },
	{ 0b1001100, C_OP__, "mcl", NULL, F_NOARG },
	{ 0b1001101, C_OP__, "mcl", NULL, F_NOARG },
	{ 0b1001110, C_OP__, "mcl", NULL, F_NOARG },
	{ 0b1001111, C_OP__, "mcl", NULL, F_NOARG },

	{ 0b1010000, C_OP__, "cit",  NULL, F_NOARG },
	{ 0b1010001, C_OP__, "sil",  NULL, F_NOARG },
	{ 0b1010010, C_OP__, "siu",  NULL, F_NOARG },
	{ 0b1010011, C_OP__, "sit",  NULL, F_NOARG },
	{ 0b1010100, C_OP__, "sind", NULL, F_NOARG },
	{ 0b1010101, C_DATA, NULL,   NULL, 0 },
	{ 0b1010110, C_DATA, NULL,   NULL, 0 },
	{ 0b1010111, C_DATA, NULL,   NULL, 0 },

	{ 0b1011000, C_OP__, "gil", NULL, F_NOARG },
	{ 0b1011001, C_OP__, "gil", NULL, F_NOARG },
	{ 0b1011010, C_OP__, "gil", NULL, F_NOARG },
	{ 0b1011011, C_OP__, "gil", NULL, F_NOARG },
	{ 0b1011100, C_OP__, "gil", NULL, F_NOARG },
	{ 0b1011101, C_OP__, "gil", NULL, F_NOARG },
	{ 0b1011110, C_OP__, "gil", NULL, F_NOARG },
	{ 0b1011111, C_OP__, "gil", NULL, F_NOARG },

	{ 0b1100001, C_OP__, "lip", NULL, F_NOARG },
	{ 0b1100000, C_OP__, "lip", NULL, F_NOARG },
	{ 0b1100010, C_OP__, "lip", NULL, F_NOARG },
	{ 0b1100011, C_OP__, "lip", NULL, F_NOARG },
	{ 0b1100101, C_OP__, "lip", NULL, F_NOARG },
	{ 0b1100100, C_OP__, "lip", NULL, F_NOARG },
	{ 0b1100110, C_OP__, "lip", NULL, F_NOARG },
	{ 0b1100111, C_OP__, "lip", NULL, F_NOARG },

	{ 0b1101000, C_DATA, NULL, NULL, 0 },
	{ 0b1101001, C_DATA, NULL, NULL, 0 },
	{ 0b1101010, C_DATA, NULL, NULL, 0 },
	{ 0b1101011, C_DATA, NULL, NULL, 0 },
	{ 0b1101100, C_DATA, NULL, NULL, 0 },
	{ 0b1101101, C_DATA, NULL, NULL, 0 },
	{ 0b1101110, C_DATA, NULL, NULL, 0 },
	{ 0b1101111, C_DATA, NULL, NULL, 0 },

	{ 0b1110000, C_DATA, NULL, NULL, 0 },
	{ 0b1110001, C_DATA, NULL, NULL, 0 },
	{ 0b1110010, C_DATA, NULL, NULL, 0 },
	{ 0b1110011, C_DATA, NULL, NULL, 0 },
	{ 0b1110100, C_DATA, NULL, NULL, 0 },
	{ 0b1110101, C_DATA, NULL, NULL, 0 },
	{ 0b1110110, C_DATA, NULL, NULL, 0 },
	{ 0b1110111, C_DATA, NULL, NULL, 0 },

	{ 0b1111000, C_DATA, NULL, NULL, 0 },
	{ 0b1111001, C_DATA, NULL, NULL, 0 },
	{ 0b1111010, C_DATA, NULL, NULL, 0 },
	{ 0b1111011, C_DATA, NULL, NULL, 0 },
	{ 0b1111100, C_DATA, NULL, NULL, 0 },
	{ 0b1111101, C_DATA, NULL, NULL, 0 },
	{ 0b1111110, C_DATA, NULL, NULL, 0 },
	{ 0b1111111, C_DATA, NULL, NULL, 0 }

};

struct opdef iset_74[] = {
	{ 0, C_OP_N, "uj", NULL, F_NORM | F_JUMP },
	{ 1, C_OP_N, "jl", NULL, F_NORM | F_JUMP },
	{ 2, C_OP_N, "je", NULL, F_NORM | F_JUMP },
	{ 3, C_OP_N, "jg", NULL, F_NORM | F_JUMP },
	{ 4, C_OP_N, "jz", NULL, F_NORM | F_JUMP },
	{ 5, C_OP_N, "jm", NULL, F_NORM | F_JUMP },
	{ 6, C_OP_N, "jn", NULL, F_NORM | F_JUMP },
	{ 7, C_OP_N, "lj", NULL, F_NORM | F_JUMP }
};

struct opdef iset_75[] = {
	{ 0, C_OP_N, "ld", NULL, F_NORM | F_ADDR },
	{ 1, C_OP_N, "lf", NULL, F_NORM | F_ADDR },
	{ 2, C_OP_N, "la", NULL, F_NORM | F_ADDR },
	{ 3, C_OP_N, "ll", NULL, F_NORM | F_ADDR },
	{ 4, C_OP_N, "td", NULL, F_NORM | F_ADDR },
	{ 5, C_OP_N, "tf", NULL, F_NORM | F_ADDR },
	{ 6, C_OP_N, "ta", NULL, F_NORM | F_ADDR },
	{ 7, C_OP_N, "tl", NULL, F_NORM | F_ADDR }
};

struct opdef iset_76[] = {
	{ 0, C_OP_N, "rd", NULL, F_NORM | F_ADDR },
	{ 1, C_OP_N, "rf", NULL, F_NORM | F_ADDR },
	{ 2, C_OP_N, "ra", NULL, F_NORM | F_ADDR },
	{ 3, C_OP_N, "rl", NULL, F_NORM | F_ADDR },
	{ 4, C_OP_N, "pd", NULL, F_NORM | F_ADDR },
	{ 5, C_OP_N, "pf", NULL, F_NORM | F_ADDR },
	{ 6, C_OP_N, "pa", NULL, F_NORM | F_ADDR },
	{ 7, C_OP_N, "pl", NULL, F_NORM | F_ADDR }
};

struct opdef iset_77[] = {
	{ 0, C_OP_N, "mb", NULL, F_NORM | F_ADDR },
	{ 1, C_OP_N, "im", NULL, F_NORM | F_ADDR },
	{ 2, C_OP_N, "ki", NULL, F_NORM | F_ADDR },
	{ 3, C_OP_N, "fi", NULL, F_NORM | F_ADDR },
	{ 4, C_OP_N, "sp", NULL, F_NORM | F_ADDR },
	{ 5, C_OP_N, "md", NULL, F_NORM },
	{ 6, C_OP_N, "rz", NULL, F_NORM | F_ADDR },
	{ 7, C_OP_N, "ib", NULL, F_NORM | F_ADDR }
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
