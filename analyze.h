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

#ifndef ANALYZE_H
#define ANALYZE_H

#include <stdlib.h>
#include <stdio.h>

#include "image.h"

#define MAX_LAB_LEN 64

enum label_types {
	LAB_JMP = 0,
	LAB_DATA,
	LAB_IO_NO,
	LAB_IO_EN,
	LAB_IO_OK,
	LAB_IO_PE,
	LAB_MAX,
};

int an_code(struct cell *image, int size);
int an_args(struct cell *image, int size);
int an_labels(struct cell *image, int size);

#endif

// vim: tabstop=4 autoindent
