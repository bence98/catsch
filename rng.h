#pragma once

#include <stdbool.h>

struct prng_t {
	int (*init)(struct prng_t *);
	int (*seed)(struct prng_t *, long);
	int (*cycle)(struct prng_t *);
	int (*destroy)(struct prng_t *);
	void *ctx;
	double p;
	bool doPrint;
	struct {
		bool block, file;
	} reroll_opts;
};

struct prng_t *prng_get_default(void);
struct prng_t *prng_get(const char *);

#define prng_init(pp)		(pp)->init((pp))
#define prng_seed(pp, ls)	(pp)->seed((pp), (ls))
#define prng_cycle(pp)		(pp)->cycle((pp))
#define prng_destroy(pp)	(pp)->destroy((pp))
