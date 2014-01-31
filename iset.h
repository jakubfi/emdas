//  Copyright (c) 2012-2013 Jakub Filipowicz <jakubf@gmail.com>
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

#ifndef ISET_H
#define ISET_H

#include <inttypes.h>

typedef struct opdef * (*opfun)(uint16_t opcode);

// macros to access opcode fields
#define _OP(x)  (((x) & 0b1111110000000000) >> 10)
#define _D(x)   (((x) & 0b0000001000000000) >> 9)
#define _A(x)   (((x) & 0b0000000111000000) >> 6)
#define _B(x)   (((x) & 0b0000000000111000) >> 3)
#define _C(x)   (((x) & 0b0000000000000111) >> 0)
#define _T(x)   (int8_t) (((x) & 0b0000000000111111) * (((x) & 0b0000001000000000) ? -1 : 1))
#define _t(x)   (uint8_t) (((x) & 0b0000000000000111) | (((x) & 0b0000001000000000) >> 6)) // only SHC uses it
#define _b(x)   ((x) & 0b0000000011111111)

struct opdef {
	uint16_t opcode;
	int type;
	char *mnemo;
	opfun extop_fun;
	unsigned flags;
};

enum op_flags {
	F_RELATIVE	= 1 << 0,
	F_JUMP		= 1 << 1,
	F_NORM		= 1 << 2,
	F_BNORM		= 1 << 3,
	F_SHORT		= 1 << 4,
	F_BYTE		= 1 << 5,
	F_NOARG		= 1 << 6,
	F_ADDR		= 1 << 7,
	F_ADWORD	= 1 << 8,
	F_AFLOAT	= 1 << 9,
	F_IO		= 1 << 10,
};

struct opdef * get_op(uint16_t opcode);
struct opdef * get_extop_37(uint16_t opcode);
struct opdef * get_extop_70(uint16_t opcode);
struct opdef * get_extop_71(uint16_t opcode);
struct opdef * get_extop_72(uint16_t opcode);
struct opdef * get_extop_73(uint16_t opcode);
struct opdef * get_extop_74(uint16_t opcode);
struct opdef * get_extop_75(uint16_t opcode);
struct opdef * get_extop_76(uint16_t opcode);
struct opdef * get_extop_77(uint16_t opcode);


#endif

// vim: tabstop=4 shiftwidth=4 autoindent
