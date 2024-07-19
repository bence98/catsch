// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Schrödinger's `cat`: Maybe print the contents of a file
 */

#include <errno.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define FLIST_READ
#include "filelist.h"
#include "rng.h"

int do_cat(FILE *f, bool doPrint)
{
	char buf[1024];

	while (!feof(f)) {
		size_t len = fread(buf, 1, sizeof(buf), f);
		if (doPrint) {
			size_t written = 0;
			while (written != len && !ferror(stdout))
				written += fwrite(buf + written, 1, len - written, stdout);

			if (written != len) {
				fprintf(stderr, "Error while writing output!\n");
				return -EIO;
			}
		}
	}
}

static int cb_do_cat(FILE *f, void *userdata)
{
	return do_cat(f, *((bool *)userdata));
}

static void print_help(const char *prog)
{
	fprintf(stderr, "Usage: %s [opts...] [FILEs...]\n\n"
		"'FILEs' is zero or more input files. "
		"These will(/won't) be concatenated. "
		"If no files are given, read from the standard "
		"input. Additionally, '-' may be specified "
		"among the files as a synonym for the standard "
		"input.\n"
		"Opts:\n"
		"\t-s/--seed [seed] \t- the seed for the PRNG\n"
		"", prog);
}

static const struct option opts[] = {
	{ "help", 0, NULL, 'h' },
	{ "seed", 1, NULL, 's' },
	{ }
};

int main(int argc, char* argv[])
{
	int err, opt;

	struct prng_t *prng = prng_get_default();
	prng_init(prng);
	prng->p = 0.5;

	while ((opt = getopt_long(argc, argv, "hs:", opts, NULL)) != -1)
		switch (opt) {
		case 's':
			errno = 0;
			long seed = strtol(optarg, NULL, 0);
			if(errno)
				fprintf(stderr, "Invalid seed '%s': %s", optarg, strerror(errno));
			else
				prng_seed(prng, seed);
			break;
		default:
			print_help(argv[0]);
			return (opt == 'h') ? 0 : -EINVAL;
		}

	prng_cycle(prng);

	struct flist *l = flist_new();
	if (!l) {
		perror("Could not set up input file list");
		return -ENOMEM;
	}

	for (int i = optind; i < argc; i++) {
		err = flist_add(l, argv[i]);
		if (err)
			return err;
	}

	if (l->head) {
		err = flist_foreach(l, cb_do_cat, &prng->doPrint);
	} else {
		err = do_cat(stdin, prng->doPrint);
	}

	flist_delete(l);
	prng_destroy(prng);

	return err;
}
