#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "../rng.h"
#include "urandom.h"

static int prng_urandom_init(struct prng_t *prng)
{
	prng->ctx = fopen("/dev/urandom", "r");

	if (!prng->ctx)
		return errno;

	return 0;
}

static int prng_urandom_seed(struct prng_t *prng, long seed)
{
	return -EOPNOTSUPP;
}

static int prng_urandom_cycle(struct prng_t *prng)
{
	int ch = fgetc((FILE *)prng->ctx);

	if (ch < 0)
		return -EIO;

	prng->doPrint = ch < (UCHAR_MAX * prng->p);
	return 0;
}

static int prng_urandom_destroy(struct prng_t *prng)
{
	int err = fclose((FILE *)prng->ctx);

	return err ? errno : 0;
}

static struct prng_t prng_urandom = {
	.init = prng_urandom_init,
	.seed = prng_urandom_seed,
	.cycle = prng_urandom_cycle,
	.destroy = prng_urandom_destroy,
};

struct prng_t *prng_urandom_get(void)
{
	return &prng_urandom;
}
