#include <errno.h>
#include <stdio.h>

#include "cat.h"
#define FLIST_READ
#include "filelist.h"
#include "rng.h"

static int do_cat(FILE *f, void *userdata)
{
	struct prng_t *prng = (struct prng_t *)userdata;
	char buf[1024];

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
			prng_cycle(prng);
	}

	if (prng->reroll_opts.file)
		prng_cycle(prng);

	return 0;
}

int cat_files(struct flist *l, struct prng_t *prng)
{
	int err;

	if (l->head) {
		err = flist_foreach(l, do_cat, prng);
	} else {
		err = do_cat(stdin, prng);
	}

	return err;
}
