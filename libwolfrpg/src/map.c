#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iconv.h>

#include "wolfrpg.h"
#include "reader.h"
#include "internal.h"

static const unsigned char CMDTERM[] = "\x03\x00\x00\x00";

static const unsigned char EVMAGIC[] = "90\0\0";
static const unsigned char PAGEMAGIC[] = "\0\0\0\0";

static const unsigned char MAPMAGIC[] = \
	"\0\0\0\0\0"
	"\0\0\0\0\0"
	"WOLFM\0"
	"\0\0\0\0\x64"
	"\0\0\0\x65\x05"
	"\0\0\0\x82\xc8\x82\xb5\0";

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

	p->routes = malloc(sizeof *p->routes * p->nroute);
	for(int i = 0; i < p->nroute; i++) {
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

static void
event_load(Reader *r, MapEvent *e)
{
	if(readncmp(r, EVMAGIC, 4) != 0) {
		printf("invalid event magic\n");
		return;
	}

	e->id = readint(r);
	e->name = readstr(r);
	e->x = readint(r);
	e->y = readint(r);

	e->npage = readint(r);
	e->pages = malloc(sizeof *e->pages * e->npage);

	if(readncmp(r, PAGEMAGIC, 4) != 0) {
		printf("unexpected page magic\n");
		return;
	}

	for(int i = 0; i < e->npage; i++) {
		if(readbyte(r) != '\x79') {
			printf("entity not a page: 0x%x\n", r->buf[0]);
			e->npage = i;
			break;
		}
		page_load(r, &e->pages[i]);
	}

	if(readbyte(r) != '\x70')
		printf("unexpected page list terminator: \\x%x\n", r->buf[0]);
}

static void
event_free(MapEvent *e)
{
	for(int i = 0; i < e->npage; i++)
		page_free(e->pages + i);
	free(e->pages);
	free(e->name);
}

Map *
map_load(char *filename)
{
	Reader *r = rnew("CP932");
	FILE *f = fopen(filename, "rb");
	Map *m = malloc(sizeof *m);

	r->f = f;
	if(r->f == NULL)
		return NULL;

	if(readncmp(r, MAPMAGIC, sizeof MAPMAGIC-1) != 0) {
		printf("%x %x %x %x\n", r->buf[0], r->buf[1], r->buf[2], r->buf[3]);
		return NULL;
	}

	m->tileset = readint(r);
	m->w = readint(r);
	m->h = readint(r);
	m->nev = readint(r);

	m->tiles = malloc(m->w*m->h*3*sizeof (int));
	for(int i = 0; i < m->w*m->h*3; i++)
		m->tiles[i] = readint(r);

	m->evs = malloc(sizeof *m->evs * m->nev);
	for(int i = 0; i < m->nev; i++) {
		assert(readbyte(r) == '\x6f');
		event_load(r, m->evs + i);
		MapEvent *e = m->evs + i;
	}

	if(readbyte(r) != '\x66')
		printf("unexpected event list terminator: \\x%x\n", r->buf[0]);

	fclose(f);
	rfree(r);
	return m;
}

void
map_free(Map *m)
{
	for(int i = 0; i < m->nev; i++)
		event_free(m->evs + i);

	free(m->evs);
	free(m->tiles);
	free(m);
}

void
map_print(Map *m)
{
	printf("tileset: %d\n", m->tileset);
	printf("%d×%d, %d events\n", m->w, m->h, m->nev);
}
