#include <string.h>

#include "rng.h"
#include "rng/urandom.h"

struct prng_t *prng_get(const char *name)
{
	if (!strcmp(name, "libc"))
		return prng_get_default();

	if (!strcmp(name, "urandom"))
		return prng_urandom_get();

	return NULL;
}
