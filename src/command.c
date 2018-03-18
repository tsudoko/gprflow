#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iconv.h>

#include "reader.h"
#include "route.h"
#include "command.h"

const char *command_idstr(Command *);

void
command_load(Reader *r, Command *c)
{
	c->movedata.nroute = 0;
	c->movedata.routes = NULL;

	c->narg = readbyte(r) - 1;
	c->id = readint(r);

	c->args = malloc(sizeof *c->args * c->narg);
	for(int j = 0; j < c->narg; j++) {
		c->args[j] = readint(r);
	}

	rloadn(r, 1); /* indent */

	c->nstrarg = readbyte(r);
	c->strargs = malloc(sizeof *c->strargs * c->nstrarg);
	for(int j = 0; j < c->nstrarg; j++)
		c->strargs[j] = readstr(r);

	rloadn(r, 1);
	if(r->buf[0] == '\x1') {
		rloadn(r, 5);
		memcpy(c->movedata.something, r->buf, 5);
		c->movedata.flags = readbyte(r);
		c->movedata.nroute = readint(r);

		c->movedata.routes = malloc(sizeof *(c->movedata.routes) * (c->movedata.nroute));
		for(int j = 0; j < c->movedata.nroute; j++) {
			if(route_load(r, &c->movedata.routes[j]) < 0) { /* TODO: make fatal? */
				c->movedata.nroute = j;
				return;
			}

		}
	} else if(r->buf[0] != '\x0') {
		printf("%d: unexpected terminator: \\x%x\n", ftell(r->f)-1, r->buf[0]);
		return;
	}
}

void
command_free(Command *c)
{
	free(c->args);
	for(int j = 0; j < c->nstrarg; j++)
		free(c->strargs[j]);
	free(c->strargs);
	for(int j = 0; j < c->movedata.nroute; j++)
		free(c->movedata.routes[j].args);
	if(c->movedata.routes)
		free(c->movedata.routes);
}

void
command_print(Command *c)
{
	printf("%d: %s\n", c->id, command_idstr(c));
	printf("narg %d\n", c->narg);
	printf("  args: [");
	for(int j = 0; j < c->narg; j++) {
		printf("%d", c->args[j]);
		if(j + 1 != c->narg) printf(", ");
	}
	printf("]\n");
	printf("  string args: [");
	for(int j = 0; j < c->nstrarg; j++) {
		printf("\"%s\"", c->strargs[j]);
		if(j + 1 != c->nstrarg) printf(", ");
	}
	printf("]\n");
}
