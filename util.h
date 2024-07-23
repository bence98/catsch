#pragma once

long   util_parse_seed(const char *, int *);
double util_parse_prob(const char *);

/* Convenience macro. Needs <stdio.h>! */
#define pr_err(...) fprintf(stderr, __VA_ARGS__)
