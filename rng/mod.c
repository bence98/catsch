#include <dlfcn.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../rng.h"
#include "../util.h"
#include "mod.h"

struct prng_mod_priv_t {
	void *handle;
	struct prng_t *inner;
};

static int _prng_module_release(void *handle)
{
	int err = dlclose(handle);
	if (err)
		pr_err("Could not release module: %s\n", dlerror());
	return err;
}

static int prng_module_init(struct prng_t *prng)
{
	struct prng_mod_priv_t *priv = prng->ctx;

	return prng_init(priv->inner);
}

static int prng_module_destroy(struct prng_t *prng)
{
	struct prng_mod_priv_t *priv = prng->ctx;

	int err = prng_destroy(priv->inner);

	int err2 = _prng_module_release(priv->handle);
	if (!err)
		err = err2;

	free(priv);
	free(prng);
	return err;
}

static int prng_module_seed(struct prng_t *prng, long seed)
{
	struct prng_mod_priv_t *priv = prng->ctx;

	return prng_seed(priv->inner, seed);
}

static int prng_module_cycle(struct prng_t *prng)
{
	struct prng_mod_priv_t *priv = prng->ctx;

	priv->inner->p = prng->p;
	int err = prng_cycle(priv->inner);
	prng->doPrint = priv->inner->doPrint;
	return err;
}

static struct prng_t prng_module = {
        .init = prng_module_init,
        .seed = prng_module_seed,
        .cycle = prng_module_cycle,
        .destroy = prng_module_destroy,
};

struct prng_t *prng_get_module(const char *name)
{
	void *handle = dlopen(name, RTLD_NOW);

	if (!handle) {
		pr_err("Could not load module '%s': %s\n", name, dlerror());
		return NULL;
	}

	/* Clear errors */
	dlerror();
	char fn_name[strlen(name) + strlen("prng__get") + 1];
	sprintf(fn_name, "prng_%s_get", name);

	void *fn = dlsym(handle, fn_name);
	/* dlsym() can retrn NULL on success, but we treat it as an error (invalid module) */
	if (!fn) {
		pr_err("Could not read module entry '%s': %s\n", fn_name, dlerror());
		goto release_mod;
	}

	struct prng_t *(*get_prng)(void);
	*(void **)&get_prng = fn;

	struct prng_t *prng = malloc(sizeof(struct prng_t));

	if (!prng)
		goto release_mod;

	struct prng_mod_priv_t *priv = malloc(sizeof(struct prng_mod_priv_t));

	if (!priv)
		goto free_prng;

	memcpy(prng, &prng_module, sizeof(struct prng_t));
	priv->handle = handle;
	priv->inner = get_prng();
	if (!priv->inner)
		goto free_priv;
	prng->ctx = priv;
	return prng;

free_priv:
	free(priv);
free_prng:
	free(prng);
release_mod:
	_prng_module_release(handle);
	return NULL;
}
