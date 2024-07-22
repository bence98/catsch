#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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

static int _flist_close(struct flist_entry *e)
{
	int err = 0;

	if (e->f)
		err = fclose(e->f);
	e->f = NULL;

	return err ? errno : 0;
}

int flist_delete(struct flist *l)
{
	struct flist_entry *e = l->head;
	int err;

	while (e) {
		struct flist_entry *next = e->next;

		err = _flist_close(e);
		free(e);

		if (err) {
			l->head = next;
			return err;
		}

		if (e == l->tail)
			break;
		e = next;
	}

	free(l);
}

static FILE *_get_stdin(void)
{
	int fd = dup(STDIN_FILENO);

	if (fd < 0)
		return NULL;

	return fdopen(fd, "r");
}

static int _flist_open(struct flist_entry *e)
{
	if (e->f)
		return 0;

	if (strcmp(e->name, "-"))
		e->f = fopen(e->name, "r");
	else
		e->f = _get_stdin();

	if (!e->f) {
		fprintf(stderr, "Failed to open input file '%s': %s", e->name, strerror(errno));
		return -ENOENT;
	}

	return 0;
}

int flist_add(struct flist *l, const char *path)
{
	struct flist_entry *e = malloc(sizeof(struct flist_entry));

	if (!e) {
		perror("Failed to build file list");
		return -ENOMEM;
	}

	e->next = NULL;
	e->name = path;
	e->f    = NULL;

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
		err = _flist_open(e);
		if (err < 0)
			return err;

		err = fn(e->f, userdata);
		if (err < 0)
			return err;

		err = _flist_close(e);
		if (err < 0)
			return err;

		e = e->next;
	}

	return err;
}
