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

// -----------------------------------------------------------------------
void version()
{
	printf("EMDAS version %s - MERA-400 dissassembler\n", EMDAS_VERSION);
}

// -----------------------------------------------------------------------
void usage()
{
	version();
	printf(
		"Usage: emdas [options] input\n"
		"Where options are one or more of:\n"
		"   -o <output> : specify output file (stdout otherwise)\n"
		"   -c <cpu>    : set CPU type: mera400, mx16 (default is mera400)\n"
		"   -a <addr>   : set base address\n"
		"   -l          : use lowercase mnemonics\n"
		"   -na         : do not print adresses\n"
		"   -nc         : do not print alternatives in comments\n"
		"   -nl         : do not assign labels\n"
		"   -v          : print version and exit\n"
		"   -h          : print help and exit\n"
	);
}

// -----------------------------------------------------------------------
int parse_args(int argc, char **argv)
{
	char *oa;
	int option;

	while ((option = getopt(argc, argv,"c:lo:n:a:vh")) != -1) {
		switch (option) {
			case 'c':
				if (!strcmp(optarg, "mera400")) {
					iset = EMD_ISET_MERA400;
				} else if (!strcmp(optarg, "mx16")) {
					iset = EMD_ISET_MX16;
				} else {
					fprintf(stderr, "Unknown CPU type: '%s'\n", optarg);
					return -1;
				}
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
				version();
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
		fprintf(stderr, "Missing input file name.\n");
		return -1;
	}

	return 0;
}

// -----------------------------------------------------------------------
int memget(int nb, uint16_t addr, uint16_t *dest)
{
	*dest = mem[addr];
	return 1;
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

	int bin_size;

	res = parse_args(argc, argv);
	if (res) {
		// wrong usage, parse_args() prints the error
		goto cleanup;
	}

	if (stat(input_file, &sb) == -1) {
		fprintf(stderr, "Cannot stat file '%s'.\n", input_file);
		goto cleanup;
	}

	if (sb.st_size > 2*(0x10000-base_addr)) {
		fprintf(stderr, "Warning: File is bigger than available address space (%li bytes > %i words). Output will be truncated.\n", sb.st_size, 0x10000-base_addr);
	}

	fi = fopen(input_file, "r");
	if (!fi) {
		fprintf(stderr, "Cannot open input file '%s'.\n", input_file);
		goto cleanup;
	}

	emd = emdas_create(iset, memget);
	if (!emd) {
		fprintf(stderr, "Cannot setup disassembler: %s\n", emdas_get_error(emdas_error));
		goto cleanup;
	}

	res = emdas_set_tabs(emd, TAB_LABEL, TAB_MNEMO, TAB_ARG, TAB_COMMENT);
	if (res != EMD_E_OK) {
		fprintf(stderr, "Cannot set tabulators: %s\n", emdas_get_error(res));
		goto cleanup;
	}
	res = emdas_set_features(emd, features);
	if (res) {
		fprintf(stderr, "Cannot set disassembler features: %s\n", emdas_get_error(res));
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
			fprintf(stderr, "Memory allocation error\n");
			goto cleanup;
		}
		rewind(fi);
		bin_size = fread(mem+base_addr, sizeof(uint16_t), 0x10000-base_addr, fi);
		for (int i=base_addr ; i<base_addr+bin_size ; i++) {
			mem[i] = ntohs(mem[i]);
		}
	}

	if (bin_size < 0) {
		fprintf(stderr, "Cannot read input file '%s'.\n", input_file);
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
		fprintf(stderr, "Cannot open output file '%s' for writing.\n", output_file);
		goto cleanup;
	}

	// print CPU type
	fprintf(fo, "%*s.cpu%*s%s\n\n", TAB_MNEMO, "", TAB_ARG-TAB_MNEMO-4, "", iset == EMD_ISET_MX16 ? "mx16" : "mera400");

	// disassemble and write output
	if (use_labels) {
		res = emdas_analyze(emd, 0, base_addr, bin_size);
		if (res != EMD_E_OK) {
			fprintf(stderr, "Cannot assign labels: %s\n", emdas_get_error(res));
			goto cleanup;
		}
	}

	int ic = base_addr;
	int words;
	while (ic < base_addr+bin_size) {
		words = emdas_dasm(emd, 0, ic);
		if (words <= 0) {
			fprintf(stderr, "Disassembly error: %s\n", emdas_get_error(emdas_error));
		}
		fprintf(fo, "%s", emdas_get_buf(emd));
		ic += words;
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
