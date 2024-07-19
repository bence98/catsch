#include <errno.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

double util_parse_prob(const char *str)
{
	char *end;
	errno = 0;
	double p = strtod(str, &end);
	if(errno)
		fprintf(stderr, "Invalid probability '%s': %s\n", str, strerror(errno));
	else if (end == str)
		fprintf(stderr, "Invalid number '%s'\n", str);
	else if (*end == '%')
		p *= 0.01;
	else if (*end)
		fprintf(stderr, "Unrecognized suffix ignored '%s'\n", end);

	if (isnan(p) || p < 0.0) {
		fprintf(stderr, "Probability must be positive! (%lf given)\n", p);
		return 0.0;
	} else if (p > 1.0) {
		fprintf(stderr, "Probability must be at most 1.0! (%lf given)\n", p);
		return 1.0;
	}

	return p;
}
