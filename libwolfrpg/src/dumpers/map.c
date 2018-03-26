#include <stdio.h>
#include <stdlib.h>

#define _POSIX_SOURCE
#include <iconv.h>

#include "../reader.h"
#include "../route.h"
#include "../command.h"
#include "../map.h"

int
main(int argc, char **argv)
{
	if(argc < 2) {
		fprintf(stderr, "usage: %s mapfile\n", argv[0]);
		return EXIT_FAILURE;
	}

	Map *m = map_load(argv[1]);
	map_print(m);
	map_free(m);
	return 0;
}
