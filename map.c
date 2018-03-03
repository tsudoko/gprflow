#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "reader.h"
#include "page.h"
#include "event.h"
#include "map.h"

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
	Reader reader, *r = &reader;
	FILE *f = fopen(filename, "rb");
	Map *m = malloc(sizeof *m);

	reader.f = f;

	if(readncmp(r, MAGIC, MAGICLEN) != 0) {
		printf("%x %x %x %x\n", r->buf[0], r->buf[1], r->buf[2], r->buf[3]);
		return NULL;
	}

	/* TODO: checked fread, encryption */
	m->tileset = readint(r);
	m->w = readint(r);
	m->h = readint(r);
	m->nevent = readint(r);

	m->tiles = malloc(m->w*m->h*3*sizeof (int));
	for(int i = 0; i < m->w*m->h*3; i++)
		m->tiles[i] = readint(r);

	while(readbyte(r) == '\x6f') {
		Event *e = event_load(r);
		printf("event \"");
		for(int i = 0; i < strlen(e->name); i++)
			printf("\\x%hhx", e->name[i]);
		printf("\" (0x%x)\n", e->id);
		printf("(%d, %d), %d pages\n", e->x, e->y, e->npage);
	}

	if(r->buf[0] != '\x66')
		printf("unexpected event list terminator: \\x%x\n", r->buf[0]);

	fclose(f);
	return m;
}

void
map_free(Map *m)
{
	free(m->tiles);
	free(m);
}