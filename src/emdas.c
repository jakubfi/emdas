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
#include <getopt.h>
#include <string.h>
#include <emelf.h>

#include "emdas.h"

int isize;

char *input_file;
char *output_file;

int skip_labels, skip_addresses, skip_values, skip_analysis;
int base_addr;

// -----------------------------------------------------------------------
void usage()
{
	printf("Usage: emdas [options] input\n");
	printf("Where options are one or more of:\n");
	printf("   -o <output> : specify output file (stdout otherwise)\n");
	printf("   -na         : do not include adresses in asm output\n");
	printf("   -nv         : do not include values in asm output\n");
	printf("   -nl         : do not assign labels\n");
	printf("   -nr         : skip argument analysis (implies -nl)\n");
	printf("   -a <addr>   : set base address\n");
	printf("   -v          : print version and exit\n");
	printf("   -h          : print help and exit\n");
}

// -----------------------------------------------------------------------
int parse_args(int argc, char **argv)
{
	char *oa;
	int option;

	while ((option = getopt(argc, argv,"o:n:a:vh")) != -1) {
		switch (option) {
			case 'n':
				oa = optarg;
				while (oa && *oa){
					switch (*oa) {
						case 'a': skip_addresses = 1; break;
						case 'v': skip_values = 1; break;
						case 'l': skip_labels = 1; break;
						case 'r': skip_analysis = 1; break;
						default: 
							fprintf(stderr, "Unknown parameter for -n: '%c'\n", *oa);
							return -1;
					}
					oa++;
				}
				break;
			case 'h':
				usage();
				exit(0);
				break;
			case 'v':
				printf("EMDAS v%s - MERA 400 dissassembler\n", EMDAS_VERSION);
				exit(0);
				break;
			case 'o':
				output_file = strdup(optarg);
				break;
			case 'a':
				sscanf(optarg, "%i", &base_addr);
				break;
			default:
				return -1;
		}
	}

	if (optind == argc-1) {
		input_file = argv[optind];
	} else {
		printf("Missing input file name.\n");
		return -1;
	}

	return 0;
}

// -----------------------------------------------------------------------
int write_asm(FILE *f, struct emdas *emd, uint16_t base_addr, uint16_t size)
{
	int res;
	int bytes = 0;
	struct emdas_cell *cell;
	const int bsize = 1024;
	char buf[bsize];
	int pos = 0;

	for (int addr=base_addr ; addr<base_addr+size ; addr++, pos=0) {
		cell = emdas_get_cell(emd, addr);

		//__emdas_dump_cell(stdout, cell);

		// skip args
		if (cell->type == CELL_ARG) continue;

		// instruction
		pos += snprintf(buf+pos, bsize-pos, "%-50s", cell->text);

		// comment
		if ((!skip_values) && ((cell->type != CELL_DATA) || (cell->arg_name))) {
			if ((cell->flags & FL_2WORD)) {
				struct emdas_cell *arg = emdas_get_rel(cell->args, ARG_2ARG);
				if (arg) {
					pos += snprintf(buf+pos, bsize-pos, " ; .word 0x%04x, 0x%04x", cell->v, arg->v);
				} else {
					pos += snprintf(buf+pos, bsize-pos, " ; .word 0x%04x, ??? (missing arg ref, this is weird)", cell->v);
				}
			} else {
				pos += snprintf(buf+pos, bsize-pos, " ; .word 0x%04x", cell->v);
			}
		}

		// newline
		pos += snprintf(buf+pos, bsize-pos, "\n");

		res = fwrite(buf, 1, pos, f);
		if (res < 0) {
			return res;
		}
		bytes += res;
	}

	return bytes;
}

// -----------------------------------------------------------------------
int main(int argc, char **argv)
{
	FILE *f;
	int res;
	int ret = -1;
	struct emdas *emd = NULL;
	struct emelf *e = NULL;

	res = parse_args(argc, argv);
	if (res) {
		printf("\n");
		usage();
		goto cleanup;
	}

	f = fopen(input_file, "r");
	if (!f) {
		printf("Cannot open input file '%s'.\n", input_file);
		goto cleanup;
	}

	emd = emdas_init();
	if (!emd) {
		printf("Cannot setup disassembler.\n");
		goto cleanup;
	}

	int features = FEAT_ALL & ~FEAT_UCASE;
	if (skip_analysis) {
		features &= ~FEAT_ANALYZE;
	}

	res = emdas_set_features(emd, features);
	if (res) {
		printf("Cannot set disassembler features.\n");
		goto cleanup;
	}

	// try as emelf
	e = emelf_load(f);

	if (e) {
		isize = emdas_import_tab(emd, base_addr, e->image_size, e->image);
		emelf_destroy(e);
	} else {
		rewind(f);
		isize = emdas_import_stream(emd, base_addr, 65536-base_addr, f);
	}

	if (isize < 0) {
		printf("Cannot read input file '%s'.\n", input_file);
		fclose(f);
		goto cleanup;
	}
	fclose(f);

	if (output_file) {
		f = fopen(output_file, "w");
	} else {
		f = stdout;
	}

	if (!f) {
		printf("Cannot open output file '%s' for writing.\n", output_file);
		goto cleanup;
	}

	res = write_asm(f, emd, base_addr, isize);
	if (res < 0) {
		printf("Cannot write disassembled source: '%s'.\n", output_file);
		if (output_file) {
			fclose(f);
		}
		goto cleanup;
	}
	if (output_file) {
		fclose(f);
	}
	ret = 0;

cleanup:
	emdas_shutdown(emd);
	free(output_file);
	return ret;
}

// vim: tabstop=4 autoindent
