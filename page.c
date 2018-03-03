#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "reader.h"
#include "page.h"

static const unsigned char ROUTETERM[] = "\x01\x00";
static const unsigned char CMDTERM[] = "\x03\x00\x00\x00";

int
route_load(Reader *r, Route *ro)
{
	ro->id = readbyte(r);
	unsigned char narg = readbyte(r);
	printf("(parsing) narg %d\n", narg);
	ro->args = malloc(sizeof (int) * narg);
	printf("(parsing) route args [");
	for(int j = 0; j < narg; j++) {
		ro->args[j] = readint(r);
		printf("%x ", ro->args[j]);
	}
	printf("]\n");
	if(readncmp(r, ROUTETERM, 2) != 0) {
		printf("%d: invalid route terminator: \"\\x%x\\x%x\"\n", ftell(r->f)-2, r->buf[0], r->buf[1]);
		return -1;
	}

	return 0;
}

void
page_load(Reader *r, Page *p)
{
	p->something = readint(r);
	p->name = readstr(r);
	p->direction = readbyte(r);
	p->frame = readbyte(r);
	p->opacity = readbyte(r);
	p->rendermode = readbyte(r);
	readn(r, 1 + 4 + 4*4 + 4*4);
	memcpy(p->conditions, r->buf, 1 + 4 + 4*4 + 4*4);
	readn(r, 4);
	memcpy(p->movement, r->buf, 4);
	p->flags = readbyte(r);
	p->routeflags = readbyte(r);

	p->nroute = readint(r);
	printf("(parsing) %d routes\n", p->nroute);

	p->routes = malloc(sizeof *p->routes * p->nroute);
	for(int i = 0; i < p->nroute; i++) {
		printf("(parsing) parsing route %d\n", i);
		if(route_load(r, &p->routes[i]) < 0) { /* TODO: make fatal? */
			p->nroute = i;
			break;
		}
	}

	p->ncmd = readint(r);
	p->cmds = malloc(sizeof *p->cmds * p->ncmd);
	for(int i = 0; i < p->ncmd; i++) {
		unsigned char narg = readbyte(r) - 1;
		p->cmds[i].id = readint(r);

		p->cmds[i].args = malloc(sizeof (int) * narg);
		for(int j = 0; j < narg; j++) {
			p->cmds[i].args[j] = readint(r);
		}

		readn(r, 1); /* indent */

		p->cmds[i].nstrarg = readbyte(r);
		p->cmds[i].strargs = malloc(sizeof (unsigned char *) * p->cmds[i].nstrarg);
		for(int j = 0; j < p->cmds[i].nstrarg; j++)
			p->cmds[i].strargs[j] = readstr(r);

		readn(r, 1);
		if(r->buf[0] == '\x1') {
			readn(r, 5);
			memcpy(p->cmds[i].movedata.something, r->buf, 5);
			p->cmds[i].movedata.flags = readbyte(r);
			p->cmds[i].movedata.nroute = readint(r);

			p->cmds[i].movedata.routes = malloc(sizeof *(p->cmds[i].movedata.routes) * (p->cmds[i].movedata.nroute));
			for(int j = 0; j < p->cmds[i].movedata.nroute; j++) {
				if(route_load(r, &p->cmds[i].movedata.routes[j]) < 0) { /* TODO: make fatal? */
					p->cmds[i].movedata.nroute = j;
					break;
				}

			}
		} else if(r->buf[0] != '\x0') {
			printf("%d: unexpected terminator: \\x%x\n", ftell(r->f)-1, r->buf[0]);
			p->ncmd = i;
			break;
		}
	}

	if(readncmp(r, CMDTERM, 4) != 0) {
		printf("%d: unexpected command terminator: \\x%x\\x%x\\x%x\\x%x\n", ftell(r->f)-4, r->buf[0], r->buf[1], r->buf[2], r->buf[3]);
	}

	p->shadow = readbyte(r);
	p->colw = readbyte(r);
	p->colh = readbyte(r);

	if(readbyte(r) != '\x7a')
		printf("%d: unexpected page terminator: \\x%x\n", ftell(r->f)-1, r->buf[0]);
}

void
page_free(Page *p)
{
	for(int i = 0; i < p->ncmd; i++) {
		free(p->cmds[i].args);
		for(int j = 0; j < p->cmds[i].nstrarg; j++)
			free(p->cmds[i].strargs[j]);
		free(p->cmds[i].strargs);
		for(int j = 0; j < p->cmds[i].movedata.nroute; j++)
			free(p->cmds[i].movedata.routes[j].args);
		free(p->cmds[i].movedata.routes);
	}
	free(p->cmds);
	for(int i = 0; i < p->nroute; i++)
		free(p->routes[i].args);
	free(p->routes);
	free(p->name);
	free(p);
}

void
page_print(Page *p)
{
	printf("id 0x%x, something 0x%x\n", p->id, p->something);
	printf("name \"%s\", direction 0x%x frame 0x%x opacity 0x%x rendermode 0x%x\n", p->name, p->direction, p->frame, p->opacity, p->rendermode);

	printf("conditions \"");
	for(int i = 0; i < 1 + 4 + 4*4 + 4*4; i++)
		printf("\\x%x", p->conditions[i]);
	printf("\"\n");

	printf("movement \"");
	for(int i = 0; i < 4; i++)
		printf("\\x%x", p->movement[i]);
	printf("\"\n");

	printf("flags 0x%x routeflags 0x%x\n", p->flags, p->routeflags);
	printf("%d routes, %d commands\n", p->nroute, p->ncmd);

	for(int i = 0; i < p->ncmd; i++) {
		printf("command %d: %x\n", i, p->cmds[i].id);
		printf("  args: [");
		for(int j = 0; j < p->cmds[i].narg; j++) {
			printf("%d", p->cmds[i].args[j]);
			if(j + 1 != p->cmds[i].narg) printf(", ");
		}
		printf("]\n");
		printf("  string args: [");
		for(int j = 0; j < p->cmds[i].nstrarg; j++) {
			printf("\"%s\"", p->cmds[i].strargs[j]);
			if(j + 1 != p->cmds[i].nstrarg) printf(", ");
		}
		printf("]\n");
	}
}
