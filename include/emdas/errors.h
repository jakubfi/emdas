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

#ifndef ERRORS_H
#define ERRORS_H

extern int emdas_error;

enum emdas_errors {
	EMD_E_UNKNOWN = -32000,
	EMD_E_ALLOC,
	EMD_E_ISET_UNKNOWN,
	EMD_E_GETFUN_MISSING,
	EMD_E_FEATURE_UNKNOWN,
	EMD_E_TABS_MISPLACED,
	EMD_E_MEM_BLOCK,

	EMD_E_OK = 0
};

char * emdas_get_error(int e);

#endif

// vim: tabstop=4 autoindent
