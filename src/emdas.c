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
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <emelf.h>

#include "emdas.h"

#define TAB_LABEL 8
#define TAB_MNEMO 20
#define TAB_ARG 26
#define TAB_COMMENT 50

char *input_file;
char *output_file;

int iset = EMD_ISET_MERA400;
int features = EMD_FEAT_ALL & ~EMD_FEAT_LMNEMO;
int use_labels = 1;
int base_addr;

uint16_t *mem;
int bin_size;

// -----------------------------------------------------------------------
void usage()
{
	printf(
		"Usage: emdas [options] input\n"
		"Where options are one or more of:\n"
		"   -x          : use extended MX-16 instruction set\n"
		"   -l          : use lowercase mnemonics\n"
		"   -o <output> : specify output file (stdout otherwise)\n"
		"   -na         : do not include adresses in asm output\n"
		"   -nc         : do not include comments with alternatives in asm output\n"
		"   -nl         : do not assign labels\n"
		"   -a <addr>   : set base address\n"
		"   -v          : print version and exit\n"
		"   -h          : print help and exit\n"
	);
}

// -----------------------------------------------------------------------
int parse_args(int argc, char **argv)
{
	char *oa;
	int option;

	while ((option = getopt(argc, argv,"xlo:n:a:vh")) != -1) {
		switch (option) {
			case 'x':
				iset = EMD_ISET_MX16;
				break;
			case 'l':
				features |= EMD_FEAT_LMNEMO;
				break;
			case 'n':
				oa = optarg;
				while (oa && *oa){
					switch (*oa) {
						case 'a': features &= ~EMD_FEAT_ADDR; break;
						case 'c': features &= ~EMD_FEAT_ALTS; break;
						case 'l': use_labels = 0; break;
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
uint16_t * memget(int nb, uint16_t addr)
{
	return mem + addr;
}

// -----------------------------------------------------------------------
int main(int argc, char **argv)
{
	FILE *fi, *fo;
	int res;
	int ret = -1;
	struct emdas *emd = NULL;
	struct emelf *e = NULL;
	struct stat sb;

	res = parse_args(argc, argv);
	if (res) {
		printf("\n");
		usage();
		goto cleanup;
	}

	if (stat(input_file, &sb) == -1) {
		printf("Cannot stat file '%s'.\n", input_file);
		goto cleanup;
	}

	if (sb.st_size > 2*(0x10000-base_addr)) {
		fprintf(stderr, "Warning: File is bigger than available address space (%li bytes > %i words). Output will be truncated.\n", sb.st_size, 0x10000-base_addr);
	}

	fi = fopen(input_file, "r");
	if (!fi) {
		printf("Cannot open input file '%s'.\n", input_file);
		goto cleanup;
	}

	emd = emdas_create(iset, memget);
	if (!emd) {
		printf("Cannot setup disassembler.\n");
		goto cleanup;
	}

	emdas_set_tabs(emd, TAB_LABEL, TAB_MNEMO, TAB_ARG, TAB_COMMENT);
	res = emdas_set_features(emd, features);
	if (res) {
		printf("Cannot set disassembler features.\n");
		goto cleanup;
	}

	// try as emelf first
	e = emelf_load(fi);

	if (e) {
		bin_size = e->image_size;
		mem = e->image;
	} else {
		mem = calloc(0x10000, sizeof(uint16_t));
		if (!mem) {
			fclose(fi);
			printf("Memory allocation error\n");
			goto cleanup;
		}
		rewind(fi);
		bin_size = fread(mem+base_addr, sizeof(uint16_t), 0x10000-base_addr, fi);
		for (int i=base_addr ; i<base_addr+bin_size ; i++) {
			mem[i] = ntohs(mem[i]);
		}
	}

	if (bin_size < 0) {
		printf("Cannot read input file '%s'.\n", input_file);
		fclose(fi);
		goto cleanup;
	}
	fclose(fi);

	if (output_file) {
		fo = fopen(output_file, "w");
	} else {
		fo = stdout;
	}

	if (!fo) {
		printf("Cannot open output file '%s' for writing.\n", output_file);
		goto cleanup;
	}

	// print CPU type
	fprintf(fo, "%*s.cpu%*s%s\n\n", TAB_MNEMO, "", TAB_ARG-TAB_MNEMO-4, "", iset == EMD_ISET_MX16 ? "mx16" : "mera400");

	// disassemble and write output
	if (use_labels) {
		emdas_analyze(emd, 0, base_addr, bin_size);
	}

	int ic = base_addr;
	while (ic < base_addr+bin_size) {
		ic += emdas_dasm(emd, 0, ic);
		fprintf(fo, "%s", emdas_get_buf(emd));
	}

	if (output_file) {
		fclose(fo);
	}
	ret = 0;

cleanup:
	if (e) emelf_destroy(e);
	else free(mem);
	emdas_destroy(emd);
	free(output_file);
	return ret;
}

// vim: tabstop=4 autoindent
