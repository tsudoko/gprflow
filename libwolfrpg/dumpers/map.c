#include <stdio.h>
#include <stdlib.h>

#include "../wolfrpg.h"

int
main(int argc, char **argv)
{
	if(argc < 2) {
		fprintf(stderr, "usage: %s mapfile\n", argv[0]);
		return EXIT_FAILURE;
	}

	Map *m = map_load(argv[1]);
	map_print(m);
	for(int i = 0; i < m->nev; i++)
		for(int j = 0; j < m->evs[i].npage; j++)
			page_print(m->evs[i].pages + j);
	map_free(m);
	return 0;
}
