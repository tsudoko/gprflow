#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "page.h"

#define BUFLEN 64

static const unsigned char ROUTETERM[] = "\x01\x00";
static const unsigned char CMDTERM[] = "\x03\x00\x00\x00";

int
route_load(FILE *f, Route *r)
{
	unsigned char buf[BUFLEN];

	fread(buf, 1, 1, f);
	r->id = *buf;
	fread(buf, 1, 1, f);
	unsigned char narg = *buf;
	printf("(parsing) narg %d\n", narg);
	r->args = malloc(sizeof (int) * narg);
	printf("(parsing) route args [");
	for(int j = 0; j < narg; j++) {
		fread(buf, 1, 4, f);
		r->args[j] = BUF2INT(buf);
		printf("%x ", r->args[j]);
	}
	printf("]\n");
	fread(buf, 1, 2, f);
	if(memcmp(buf, ROUTETERM, 2) != 0) {
		printf("%d: invalid route terminator: \"\\x%x\\x%x\"\n", ftell(f)-2, buf[0], buf[1]);
		return -1;
	}

	return 0;
}

void
page_load(FILE *f, Page *p)
{
	unsigned char buf[BUFLEN];

	fread(buf, 1, 4, f);
	p->something = BUF2INT(buf);

	fread(buf, 1, 4, f);
	int namelen = BUF2INT(buf);
	p->name = malloc(namelen);

	for(int i = 0; i < namelen; i++) {
		fread(buf, 1, 1, f);
		p->name[i] = *buf;
	}

	fread(buf, 1, 1, f);
	p->direction = *buf;

	fread(buf, 1, 1, f);
	p->frame = *buf;

	fread(buf, 1, 1, f);
	p->opacity = *buf;

	fread(buf, 1, 1, f);
	p->rendermode = *buf;

	fread(buf, 1, 1 + 4 + 4*4 + 4*4, f);
	memcpy(p->conditions, buf, 1 + 4 + 4*4 + 4*4);

	fread(buf, 1, 4, f);
	memcpy(p->movement, buf, 4);

	fread(buf, 1, 1, f);
	p->flags = *buf;

	fread(buf, 1, 1, f);
	p->routeflags = *buf;

	fread(buf, 1, 4, f);
	p->nroute = BUF2INT(buf);
	printf("(parsing) %d routes\n", p->nroute);

	p->routes = malloc(sizeof *p->routes * p->nroute);
	for(int i = 0; i < p->nroute; i++) {
		printf("(parsing) parsing route %d\n", i);
		if(route_load(f, &p->routes[i]) < 0) { /* TODO: make fatal? */
			p->nroute = i;
			break;
		}
	}

	fread(buf, 1, 4, f);
	p->ncmd = BUF2INT(buf);

	p->cmds = malloc(sizeof *p->cmds * p->ncmd);
	for(int i = 0; i < p->ncmd; i++) {
		fread(buf, 1, 1, f);
		unsigned char narg = *buf - 1;
		fread(buf, 1, 4, f);
		p->cmds[i].id = BUF2INT(buf);

		p->cmds[i].args = malloc(sizeof (int) * narg);
		for(int j = 0; j < narg; j++) {
			fread(buf, 1, 4, f);
			p->cmds[i].args[j] = BUF2INT(buf);
		}

		fread(buf, 1, 1, f); /* indent */

		fread(buf, 1, 1, f);
		p->cmds[i].nstrarg = *buf;
		p->cmds[i].strargs = malloc(sizeof (unsigned char *) * p->cmds[i].nstrarg);
		for(int j = 0; j < p->cmds[i].nstrarg; j++) {
			fread(buf, 1, 4, f);
			int len = BUF2INT(buf);
			p->cmds[i].strargs[j] = malloc(len);
			for(int k = 0; k < len; k++) {
				fread(buf, 1, 1, f);
				p->cmds[i].strargs[j][k] = *buf;
			}
		}

		fread(buf, 1, 1, f);
		if(*buf == '\x1') {
			fread(buf, 1, 5, f);
			memcpy(p->cmds[i].movedata.something, buf, 5);

			fread(buf, 1, 1, f);
			p->cmds[i].movedata.flags = *buf;
			
			fread(buf, 1, 4, f);
			p->cmds[i].movedata.nroute = BUF2INT(buf);

			p->cmds[i].movedata.routes = malloc(sizeof *(p->cmds[i].movedata.routes) * (p->cmds[i].movedata.nroute));
			for(int j = 0; j < p->cmds[i].movedata.nroute; j++) {
				if(route_load(f, &p->cmds[i].movedata.routes[j]) < 0) { /* TODO: make fatal? */
					p->cmds[i].movedata.nroute = j;
					break;
				}

			}
		} else if(*buf != '\x0') {
			printf("%d: unexpected terminator: \\x%x\n", ftell(f)-1, *buf);
			p->ncmd = i;
			break;
		}
	}

	fread(buf, 1, 4, f);
	if(memcmp(buf, CMDTERM, 4) != 0) {
		printf("%d: unexpected command terminator: \\x%x\\x%x\\x%x\\x%x\n", ftell(f)-4, buf[0], buf[1], buf[2], buf[3]);
	}

	fread(buf, 1, 1, f);
	p->shadow = *buf;

	fread(buf, 1, 1, f);
	p->colw = *buf;

	fread(buf, 1, 1, f);
	p->colh = *buf;

	fread(buf, 1, 1, f);
	if(*buf != '\x7a')
		printf("%d: unexpected page terminator: \\x%x\n", ftell(f)-1, buf[0]);
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
}
