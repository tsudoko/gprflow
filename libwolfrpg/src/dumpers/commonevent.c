#include <stdio.h>
#include <stdlib.h>

#include "../wolfrpg.h"

int
main(int argc, char **argv)
{
	if(argc < 2) {
		fprintf(stderr, "usage: %s CommonEvent.dat\n", argv[0]);
		return EXIT_FAILURE;
	}

	CommonEvents *c = cev_load(argv[1]);
	cev_print(c);
	for(int i = 0; i < c->n; i++) {
		printf("event %d\n", i);
		for(int j = 0; j < c->e[i].ncmd; j++) {
			printf("line %3d: ", j);
			Command *cmd = &c->e[i].cmds[j];
			command_print(cmd);
		}
	}
	cev_free(c);
	return 0;
}
