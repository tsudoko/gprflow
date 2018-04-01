#include <stdio.h>
#include <stdlib.h>

#include <iconv.h>

#include "wolfrpg.h"
#include "reader.h"

static const unsigned char ROUTETERM[] = "\x01\x00";

int
route_load(Reader *r, Route *ro)
{
	ro->id = readbyte(r);
	unsigned char narg = readbyte(r);
	ro->args = malloc(sizeof (int) * narg);
	for(int j = 0; j < narg; j++)
		ro->args[j] = readint(r);

	if(readncmp(r, ROUTETERM, 2) != 0) {
		printf("%d: invalid route terminator: \"\\x%x\\x%x\"\n", ftell(r->f)-2, r->buf[0], r->buf[1]);
		return -1;
	}

	return 0;
}
