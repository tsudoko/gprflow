#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iconv.h>

#include "util.h"
#include "../reader.h"
#include "../route.h"
#include "../command.h"
#include "page.h"
#include "event.h"
#include "../map.h"

static const unsigned char MAGIC[] = \
	"\0\0\0\0\0"
	"\0\0\0\0\0"
	"WOLFM\0"
	"\0\0\0\0\x64"
	"\0\0\0\x65\x05"
	"\0\0\0\x82\xc8\x82\xb5\0";
static const unsigned char MAGICLEN = 34; /* XXX */

Map *
map_load(char *filename)
{
	Reader *r = rnew("CP932");
	FILE *f = fopen(filename, "rb");
	Map *m = malloc(sizeof *m);

	r->f = f;

	if(readncmp(r, MAGIC, MAGICLEN) != 0) {
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
		Event *e = m->evs + i;
		printf("event \"%s\" (0x%x)\n", e->name, e->id);
		printf("(%d, %d), %d pages\n", e->x, e->y, e->npage);
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
