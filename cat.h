#pragma once

/* External declarations */
struct flist;
struct prng_t;

enum cat_opts {
	CAT_OPT_LINEWISE = 1,
};

int cat_files(struct flist *, struct prng_t *, int);
