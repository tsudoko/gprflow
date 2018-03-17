#include <stdio.h>
#include <stdlib.h>

#define _POSIX_SOURCE
#include <iconv.h>

#include "reader.h"
#include "route.h"
#include "command.h"
#include "commonevent.h"

int
main(int argc, char **argv)
{
	if(argc < 2) {
		fprintf(stderr, "usage: %s CommonEvent.dat\n", argv[0]);
		return EXIT_FAILURE;
	}

	CommonEvents *c = cev_load(argv[1]);
	cev_print(c);
	cev_free(c);
	return 0;
}
