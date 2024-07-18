#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define FLIST_READ
#include "filelist.h"

struct flist_entry {
	struct flist_entry *next;
	const char *name;
	FILE *f;
};

struct flist *flist_new()
{
	return calloc(1, sizeof(struct flist));
}

void flist_delete(struct flist *l)
{
	struct flist_entry *e = l->head;

	while (e) {
		struct flist_entry *next = e->next;

		fclose(e->f);
		free(e);

		if (e == l->tail)
			break;
		e = next;
	}

	free(l);
}

int flist_add(struct flist *l, const char *path)
{
	FILE *f = fopen(path, "r");

	if (!f) {
		fprintf(stderr, "Failed to open input file '%s': %s", path, strerror(errno));
		return -ENOENT;
	}

	struct flist_entry *e = malloc(sizeof(struct flist_entry));

	if (!e) {
		perror("Failed to build file list");
		return -ENOMEM;
	}

	e->next = NULL;
	e->name = path;
	e->f    = f;

	if (!l->head)
		l->head = e;

	if (l->tail)
		l->tail->next = e;
	l->tail = e;

	return 0;
}

int flist_foreach(struct flist *l, int (*fn)(FILE *, void *), void *userdata)
{
	struct flist_entry *e = l->head;
	int err = 0;

	while (e) {
		err = fn(e->f, userdata);

		if (err < 0)
			return err;

		e = e->next;
	}

	return err;
}
