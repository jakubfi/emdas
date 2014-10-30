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
#include <stdio.h>
#include <ctype.h>

#include "emdas/buf.h"

// -----------------------------------------------------------------------
struct emdas_buf * emdas_buf_create(int len)
{
	struct emdas_buf *buf = NULL;

	if (len < 1) {
		goto cleanup;
	}

	buf = malloc(sizeof(struct emdas_buf));
	if (!buf) {
		goto cleanup;
	}

	buf->buf = malloc(len+1);
	if (!buf->buf) {
		goto cleanup;
	}

	buf->len = len;
	emdas_buf_reset(buf);

	return buf;

cleanup:
	emdas_buf_destroy(buf);
	return NULL;
}

// -----------------------------------------------------------------------
void emdas_buf_destroy(struct emdas_buf *buf)
{
	if (!buf) return;

	free(buf->buf);
	free(buf);
}

// -----------------------------------------------------------------------
int emdas_buf_reset(struct emdas_buf *buf)
{
	if (!buf) return -1;

	buf->pos = 0;
	buf->lpos = 0;
	buf->buf[0] = '\0';

	return 0;
}

// -----------------------------------------------------------------------
int emdas_buf_c(struct emdas_buf *buf, char c)
{
	if (!buf) return 0;

	int clen = 0;
	if (buf->pos < buf->len) {
		buf->buf[buf->pos] = c;
		buf->pos++;
		buf->lpos++;
		clen++;
	}
	buf->buf[buf->pos] = '\0';

	return clen;
}

// -----------------------------------------------------------------------
int emdas_buf_nl(struct emdas_buf *buf)
{
	if (!buf) return 0;

	int clen = emdas_buf_c(buf, '\n');
	if (clen != 0) {
		buf->lpos = 0;
	}

	return clen;
}

// -----------------------------------------------------------------------
int emdas_buf_i(struct emdas_buf *buf, const char *fmt, int i)
{
	if (!buf) return 0;

	int clen = snprintf(buf->buf + buf->pos, buf->len - buf->pos, fmt, i);
	buf->lpos += clen;
	buf->pos += clen;
	buf->buf[buf->pos] = '\0';

	return clen;
}

// -----------------------------------------------------------------------
int emdas_buf_s(struct emdas_buf *buf, const char *fmt, char *s)
{
	if (!buf) return 0;

	int clen = snprintf(buf->buf + buf->pos, buf->len - buf->pos, fmt, s);
	buf->lpos += clen;
	buf->pos += clen;
	buf->buf[buf->pos] = '\0';

	return clen;
}

// -----------------------------------------------------------------------
int emdas_buf_tab(struct emdas_buf *buf, int tab)
{
	if (!buf) return 0;

	int clen = 0;
	int dtab = tab;

	// if desired tab position is lower than current line position
	// (but not in a first column) make sure to insert one space
	if ((buf->lpos != 0) && (dtab <= buf->lpos)) {
		dtab = buf->lpos + 1;
	}

	while ((buf->pos < buf->len) && (buf->lpos < dtab)) {
		buf->buf[buf->pos] = ' ';
		buf->pos++;
		buf->lpos++;
		clen++;
	}

	return clen;
}

// -----------------------------------------------------------------------
int emdas_buf_ti(struct emdas_buf *buf, int tab, const char *fmt, int i)
{
	if (!buf) return 0;

	int clen = 0;

	clen += emdas_buf_tab(buf, tab);
	clen += emdas_buf_i(buf, fmt, i);

	return clen;
}

// -----------------------------------------------------------------------
int emdas_buf_ts(struct emdas_buf *buf, int tab, const char *fmt, char *s)
{
	if (!buf) return 0;

	int clen = 0;

	clen += emdas_buf_tab(buf, tab);
	clen += emdas_buf_s(buf, fmt, s);

	return clen;
}

// -----------------------------------------------------------------------
int emdas_buf_tolower(struct emdas_buf *buf, int back)
{
	if (!buf) return 0;

	int clen = 0;

	while (clen < back) {
		buf->buf[buf->pos - (clen+1)] = tolower(buf->buf[buf->pos - (clen+1)]);
		clen++;
	}

	return clen;
}


// vim: tabstop=4 shiftwidth=4 autoindent
