#include <stdio.h>
#include <stdlib.h>

#include "map.h"

void
map_print(Map *m)
{
	printf("tileset: %d\n", m->tileset);
	printf("%d×%d, %d events\n", m->w, m->h, m->nevent);
}

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
