//

/*
 * Schrödinger's `cat`: Maybe print the contents of a file
 */

#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int do_cat(FILE *f, bool doPrint)
{
	char buf[1024];

	while (!feof(f)) {
		size_t len = fread(buf, 1, sizeof(buf), f);
		if (doPrint) {
			size_t written = 0;
			while (written != len && !ferror(stdout))
				written += fwrite(buf + written, 1, len - written, stdout);

			if (written != len) {
				fprintf(stderr, "Error while writing output!\n");
				return -EIO;
			}
		}
	}
}

int main(int argc, char* argv[])
{
	srand(time(NULL));
	FILE *f = stdin;
	int err;

	bool doPrint = rand() < (RAND_MAX * 0.5);

	err = do_cat(f, doPrint);

	return err;
}
