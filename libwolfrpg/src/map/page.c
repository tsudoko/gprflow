#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iconv.h>

#include "util.h"
#include "../reader.h"
#include "../route.h"
#include "../command.h"
#include "page.h"

static const unsigned char CMDTERM[] = "\x03\x00\x00\x00";

void
page_load(Reader *r, Page *p)
{
	p->something = readint(r);
	p->name = readstr(r);
	p->direction = readbyte(r);
	p->frame = readbyte(r);
	p->opacity = readbyte(r);
	p->rendermode = readbyte(r);
	rloadn(r, 1 + 4 + 4*4 + 4*4);
	memcpy(p->conditions, r->buf, 1 + 4 + 4*4 + 4*4);
	rloadn(r, 4);
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
	for(int i = 0; i < p->ncmd; i++)
		command_load(r, p->cmds + i);

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
	for(int i = 0; i < p->ncmd; i++)
		command_free(p->cmds + i);
	free(p->cmds);
	for(int i = 0; i < p->nroute; i++)
		free(p->routes[i].args);
	free(p->routes);
	free(p->name);
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
		printf("command %d: ", i);
		command_print(&p->cmds[i]);
	}
}
