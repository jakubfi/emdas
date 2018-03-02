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

#ifndef EMDAS_BUF_H
#define EMDAS_BUF_H

struct emdas_buf;

struct emdas_buf * emdas_buf_create(unsigned len);
void emdas_buf_destroy(struct emdas_buf *buf);

void emdas_buf_reset(struct emdas_buf *buf);
void emdas_buf_set_nl(struct emdas_buf *buf, char nl);

int emdas_buf_nl(struct emdas_buf *buf);
int emdas_buf_app(struct emdas_buf *buf, const char *fmt, ...);
int emdas_buf_c(struct emdas_buf *buf, char c);
int emdas_buf_tab(struct emdas_buf *buf, unsigned tab);
int emdas_buf_tolower(struct emdas_buf *buf, unsigned back);

char * emdas_buf_getbuf(struct emdas_buf *buf);
int emdas_buf_getlines(struct emdas_buf *buf);

#endif

// vim: tabstop=4 shiftwidth=4 autoindent
