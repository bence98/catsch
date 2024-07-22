#pragma once

struct flist_entry;

struct flist {
	struct flist_entry *head, *tail;
};

struct flist *flist_new(void);
int flist_delete(struct flist *);
int flist_add(struct flist *, const char *);

#ifdef FLIST_READ
#include <stdio.h>

int flist_foreach(struct flist *, int (*)(FILE *, void *), void *);
#endif //FLIST_READ
