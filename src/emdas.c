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

#include "image.h"
#include "analyze.h"

struct cell *pimage;
int isize;

char *input_file;
char *output_file;

int no_lab = 0;

// -----------------------------------------------------------------------
void usage()
{
	printf("Usage: emdas [options] input [output]\n");
	printf("Where options are one or more of:\n");
	printf("   -v        : print version and exit\n");
	printf("   -h        : print help and exit\n");
	printf("   -na       : do not include adresses in asm output\n");
	printf("   -nv       : do not include values in asm output\n");
	printf("   -nl       : do not assign labels\n");
}

// -----------------------------------------------------------------------
int parse_args(int argc, char **argv)
{
	int option;
	while ((option = getopt(argc, argv,"n:vh")) != -1) {
		switch (option) {
			case 'n':
				if (strlen(optarg) > 1) return -1;
				switch (*optarg) {
					case 'a': no_loc = 1; break;
					case 'v': no_val = 1; break;
					case 'l': no_lab = 1; break;
					default: return -1;
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
			default:
				return -1;
		}
	}

	if (optind == argc-1) {
		input_file = argv[optind];
		output_file = NULL;
	} else if (optind == argc-2) {
		input_file = argv[optind];
		output_file = argv[optind+1];
	} else {
		printf("Wrong usage.\n");
		return -1;
	}

	return 0;
}

// -----------------------------------------------------------------------
int main(int argc, char **argv)
{
	FILE *f;
	int res;
	int ret = -1;

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

	res = reader_raw(f, &pimage);
	if (res < 0) {
		printf("Cannot read input file '%s'.\n", input_file);
		fclose(f);
		goto cleanup;
	}

	fclose(f);

	isize = res;

	an_code(pimage, isize);
	an_args(pimage, isize);
	if (!no_lab) an_labels(pimage, isize);

	if (output_file) {
		f = fopen(output_file, "w");
	} else {
		f = stdout;
	}

	if (!f) {
		printf("Cannot open output file '%s' for writing.\n", output_file);
		goto cleanup;
	}

	res = write_asm(pimage, isize, f);
	if (res) {
		printf("Cannot write disassembled source: '%s'.\n", output_file);
		fclose(f);
		goto cleanup;
	}

	fclose(f);

	ret = 0;

cleanup:
	for (int i=0 ; i<isize ; i++) {
		free(pimage[i].label);
	}
	free(pimage);
	return ret;
}

// vim: tabstop=4 autoindent
