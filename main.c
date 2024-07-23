// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Schrödinger's `cat`: Maybe print the contents of a file
 */

#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <string.h>

#include "filelist.h"
#include "cat.h"
#include "rng.h"
#include "util.h"

static int _switch_rng(struct prng_t **pprng, const char *name)
{
	float p = (*pprng)->p;
	int err = prng_destroy(*pprng);
	if (err)
		return err;

	*pprng = prng_get(name);
	if (!*pprng)
		return -EINVAL;

	(*pprng)->p = p;
	return prng_init(*pprng);
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
		"\t-p/--probability [p] \t- probability of printing, in decimal between 0.0-1.0, or in percent 0%%-100%%\n"
		"\t-r/--reroll \t- re-roll random number after each write (1 KiB by default)\n"
		"\t-f/--reroll-files \t- re-roll random number for each input file\n"
		"\t-l/--linewise \t- read files line-by-line. Use with -r to randomize lines\n"
		"\t-g/--generator [gen] \t- use a specific RNG. Possible values: libc, urandom"
		"", prog);
}

static const struct option opts[] = {
	{ "help", 0, NULL, 'h' },
	{ "seed", 1, NULL, 's' },
	{ "probability", 1, NULL, 'p' },
	{ "reroll", 0, NULL, 'r' },
	{ "reroll-files", 0, NULL, 'f' },
	{ "linewise", 0, NULL, 'l' },
	{ "generator", 1, NULL, 'g' },
	{ }
};

int main(int argc, char* argv[])
{
	int err, opt, cat_opts = 0;

	struct prng_t *prng = prng_get_default();
	err = prng_init(prng);
	if (err) {
		fprintf(stderr, "Could not set up RNG: %s\n", strerror(err));
		return err;
	}
	prng->p = 0.5;

	while ((opt = getopt_long(argc, argv, "hs:p:g:rfl", opts, NULL)) != -1)
		switch (opt) {
		case 's':
			int parse_ok = 0;
			long seed = util_parse_seed(optarg, &parse_ok);
			if(parse_ok)
				err = prng_seed(prng, seed);
			if (err) {
				fprintf(stderr, "Could not seed RNG: %s\n", strerror(err));
				return err;
			}
			break;
		case 'p':
			prng->p = util_parse_prob(optarg);
			break;
		case 'g':
			err = _switch_rng(&prng, optarg);
			if (err) {
				fprintf(stderr, "Could not switch RNG: %s\n", strerror(err));
				return err;
			}
			break;
		case 'r':
			prng->reroll_opts.block = true;
			break;
		case 'f':
			prng->reroll_opts.file = true;
			break;
		case 'l':
			cat_opts |= CAT_OPT_LINEWISE;
			break;
		default:
			print_help(argv[0]);
			return (opt == 'h') ? 0 : -EINVAL;
		}

	err = prng_cycle(prng);
	if (err) {
		fprintf(stderr, "Could not read RNG: %s\n", strerror(err));
		return err;
	}

	struct flist *l = flist_new();
	if (!l) {
		perror("Could not set up input file list");
		return -ENOMEM;
	}

	for (int i = optind; i < argc; i++) {
		err = flist_add(l, argv[i]);
		if (err) {
			fprintf(stderr, "Error while parsing input list: %s\n", strerror(err));
			return err;
		}
	}

	err = cat_files(l, prng, cat_opts);
	if (err) {
		fprintf(stderr, "Unexpected error: %s\n", strerror(err));
	}

	int err2 = flist_delete(l);
	if (err2) {
		fprintf(stderr, "Error while closing input: %s\n", strerror(err));
		if (!err)
			err = err2;
	}

	err2 = prng_destroy(prng);
	if (err2) {
		fprintf(stderr, "Error while closing RNG: %s\n", strerror(err));
		if (!err)
			err = err2;
	}

	return err;
}
