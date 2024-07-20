// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Schrödinger's `cat`: Maybe print the contents of a file
 */

#include <errno.h>
#include <getopt.h>
#include <stdio.h>

#include "filelist.h"
#include "cat.h"
#include "rng.h"
#include "util.h"

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
		"\t-p/--probability [p] \t- probability of printing, in decimal between 0.0-1.0, or in percent 0%%-100%%\n"
		"", prog);
}

static const struct option opts[] = {
	{ "help", 0, NULL, 'h' },
	{ "seed", 1, NULL, 's' },
	{ "probability", 1, NULL, 'p' },
	{ }
};

int main(int argc, char* argv[])
{
	int err, opt;

	struct prng_t *prng = prng_get_default();
	prng_init(prng);
	prng->p = 0.5;

	while ((opt = getopt_long(argc, argv, "hs:p:", opts, NULL)) != -1)
		switch (opt) {
		case 's':
			int parse_ok = 0;
			long seed = util_parse_seed(optarg, &parse_ok);
			if(parse_ok)
				prng_seed(prng, seed);
			break;
		case 'p':
			prng->p = util_parse_prob(optarg);
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

	err = cat_files(l, prng);

	flist_delete(l);
	prng_destroy(prng);

	return err;
}
