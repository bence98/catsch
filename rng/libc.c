#include <time.h>
#include <stdlib.h>

#include "../rng.h"

static int prng_libc_init(struct prng_t *prng)
{
	srand(time(NULL));
	return 0;
}

static int prng_libc_seed(struct prng_t *prng, long seed)
{
	srand(seed);
	return 0;
}

static int prng_libc_cycle(struct prng_t *prng)
{
	prng->doPrint = rand() < (RAND_MAX * prng->p);
	return 0;
}

static int prng_libc_destroy(struct prng_t *prng)
{
	return 0;
}

static struct prng_t prng_libc = {
	.init = prng_libc_init,
	.seed = prng_libc_seed,
	.cycle = prng_libc_cycle,
	.destroy = prng_libc_destroy,
};

struct prng_t *prng_get_default()
{
	return &prng_libc;
}
