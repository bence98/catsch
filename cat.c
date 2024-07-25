#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "cat.h"
#define FLIST_READ
#include "filelist.h"
#include "rng.h"
#include "util.h"

struct cat_priv_t {
	struct prng_t *prng;
	int cat_opts;
};

static int _cat_write_out(const char *buf, size_t len, struct cat_priv_t *priv)
{
	struct prng_t *prng = priv->prng;
	int err = 0;

	if (prng->doPrint) {
		size_t written = 0;
		while (written != len && !ferror(stdout))
			written += fwrite(buf + written, 1, len - written, stdout);

		if (written != len) {
			pr_err("Error while writing output!\n");
			return -EIO;
		}
	}

	if (priv->cat_opts & CAT_OPT_REROLL_BLOCK)
		err = prng_cycle(prng);

	return err;
}

static int cat_block(FILE *f, void *userdata)
{
	struct cat_priv_t *priv = (struct cat_priv_t *)userdata;
	struct prng_t *prng = priv->prng;
	char buf[1024];
	int err = 0;

	while (!feof(f)) {
		size_t len = fread(buf, 1, sizeof(buf), f);
		err = _cat_write_out(buf, len, priv);

		if (err)
			return err;
	}

	if (priv->cat_opts & CAT_OPT_REROLL_FILE)
		err = prng_cycle(prng);

	return err;
}

static int cat_tabby(FILE *f, void *userdata)
{
	struct cat_priv_t *priv = (struct cat_priv_t *)userdata;
	struct prng_t *prng = priv->prng;
	size_t buf_len = 0;
	char *buf = NULL;
	ssize_t len;
	int err = 0;

	while ((len = getline(&buf, &buf_len, f)) != -1) {
		err = _cat_write_out(buf, len, priv);

		if (err)
			return err;
	}

	if (priv->cat_opts & CAT_OPT_REROLL_FILE)
		err = prng_cycle(prng);

	free(buf);

	return err;
}

int cat_files(struct flist *l, struct prng_t *prng, int opts)
{
	int (*do_cat)(FILE *f, void *userdata);
	int err;

	struct cat_priv_t priv = {
		.prng = prng,
		.cat_opts = opts,
	};

	do_cat = opts & CAT_OPT_LINEWISE ? cat_tabby : cat_block;

	if (l->head) {
		err = flist_foreach(l, do_cat, &priv);
	} else {
		err = do_cat(stdin, &priv);
	}

	return err;
}
