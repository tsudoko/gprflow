#include <stdio.h>
#include <stdlib.h>

#include "database.h"

int
main(int argc, char **argv)
{
	if(argc < 3) {
		fprintf(stderr, "usage: %s projectfile datfile\n", argv[0]);
		return EXIT_FAILURE;
	}

	Database *d = database_load(argv[1], argv[2]);
	database_print(d);
	database_free(d);
	return 0;
}
