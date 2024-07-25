#pragma once

/* External declarations */
struct flist;
struct prng_t;

enum cat_opts {
	CAT_OPT_LINEWISE = 1,
	CAT_OPT_REROLL_FILE = 2,
	CAT_OPT_REROLL_BLOCK = 4,
};

int cat_files(struct flist *, struct prng_t *, int);
