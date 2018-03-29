#include <stdio.h>
#include <stdlib.h>

#include <wolfrpg.h>

#include "../core/core.h"

int
main(void)
{
	Game *g = game_init(".");
	if(g == NULL) {
		fprintf(stderr, "init failed\n");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
