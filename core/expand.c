#include <stdio.h>

#include <wolfrpg.h>

#include "core.h"

int
game_deref_get(Game *g, int v)
{
	if(v > -1000000 && v < 1000000)
		return v;
	if(v >= 8000000 && v < 9000000)
		return 4; /* FIXME */
	if(v >= 9000000 && v < 9100000)
		return game_sysgeti(g, v - 9000000);
	if(v >= 9180000 && v < 9180009)
		return g->pc[v - 9180000];

	printf("unimplemented/invalid deref: %d\n", v);
	return v;
}

char *
game_format(Game *g, char *s)
{
	/* TODO: parse escape codes */
	return s;
}
