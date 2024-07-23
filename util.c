#include <errno.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "util.h"

long util_parse_seed(const char *str, int *parse_ok)
{
	char *end;
	errno = 0;
	long seed = strtol(str, &end, 0);
	if (errno)
		pr_err("Unable to parse '%s': %s\n", str, strerror(errno));
	else if (end == str || *end)
		pr_err("Invalid number '%s'\n", str);
	else
		*parse_ok = 1;

	return seed;
}

double util_parse_prob(const char *str)
{
	char *end;
	errno = 0;
	double p = strtod(str, &end);
	if (errno)
		pr_err("Unable to parse '%s': %s\n", str, strerror(errno));
	else if (end == str)
		pr_err("Invalid number '%s'\n", str);
	else if (*end == '%')
		p *= 0.01;
	else if (*end)
		pr_err("Unrecognized suffix ignored '%s'\n", end);

	if (isnan(p) || p < 0.0) {
		pr_err("Probability must be positive! (%lf given)\n", p);
		return 0.0;
	} else if (p > 1.0) {
		pr_err("Probability must be at most 1.0! (%lf given)\n", p);
		return 1.0;
	}

	return p;
}
