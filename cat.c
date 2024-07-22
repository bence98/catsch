#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "cat.h"
#define FLIST_READ
#include "filelist.h"
#include "rng.h"

static int cat_block(FILE *f, void *userdata)
{
	struct prng_t *prng = (struct prng_t *)userdata;
	char buf[1024];
	int err = 0;

	while (!feof(f)) {
		size_t len = fread(buf, 1, sizeof(buf), f);
		if (prng->doPrint) {
			size_t written = 0;
			while (written != len && !ferror(stdout))
				written += fwrite(buf + written, 1, len - written, stdout);

			if (written != len) {
				fprintf(stderr, "Error while writing output!\n");
				return -EIO;
			}
		}

		if (prng->reroll_opts.block)
			err = prng_cycle(prng);

		if (err)
			return err;
	}

	if (prng->reroll_opts.file)
		err = prng_cycle(prng);

	return err;
}

static int cat_tabby(FILE *f, void *userdata)
{
	struct prng_t *prng = (struct prng_t *)userdata;
	size_t buf_len = 0;
	char *buf = NULL;
	ssize_t len;
	int err = 0;

	while ((len = getline(&buf, &buf_len, f)) != -1) {
		if (prng->doPrint) {
			size_t written = 0;
			while (written != len && !ferror(stdout))
				written += fwrite(buf + written, 1, len - written, stdout);

			if (written != len) {
				fprintf(stderr, "Error while writing output!\n");
				return -EIO;
			}
		}

		if (prng->reroll_opts.block)
			err = prng_cycle(prng);

		if (err)
			return err;
	}

	if (prng->reroll_opts.file)
		err = prng_cycle(prng);

	free(buf);

	return err;
}

int cat_files(struct flist *l, struct prng_t *prng, int opts)
{
	int (*do_cat)(FILE *f, void *userdata);
	int err;

	do_cat = opts & CAT_OPT_LINEWISE ? cat_tabby : cat_block;

	if (l->head) {
		err = flist_foreach(l, do_cat, prng);
	} else {
		err = do_cat(stdin, prng);
	}

	return err;
}
