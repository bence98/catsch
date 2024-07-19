#include <string.h>

#include "rng.h"

struct prng_t *prng_get(const char *name)
{
	if (!strcmp(name, "libc"))
		return prng_get_default();

	return NULL;
}
