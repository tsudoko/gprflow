#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "event.h"
#include "map.h"

#define BUFLEN 64
#define nelem(x) (sizeof(x)/sizeof((x)[0]))

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
	FILE *f = fopen(filename, "rb");
	Map *m = malloc(sizeof *m);
	unsigned char buf[BUFLEN];

	fread(buf, 1, MAGICLEN, f);
	assert(BUFLEN >= MAGICLEN); /* could make this compile-time */
	if(memcmp(buf, MAGIC, MAGICLEN) != 0)
		return NULL;
	for(int i = 0; i < MAGICLEN; i++)
		printf("\\x%x", buf[i]);
	printf("\n");

	/* TODO: checked fread, encryption */
	fread(buf, 1, 4, f);
	m->tileset = BUF2INT(buf);

	fread(buf, 1, 4, f);
	m->w = BUF2INT(buf);

	fread(buf, 1, 4, f);
	m->h = BUF2INT(buf);

	fread(buf, 1, 4, f);
	m->nevent = BUF2INT(buf);

	m->tiles = malloc(m->w*m->h*3*sizeof (int));
	for(int i = 0; i < m->w*m->h*3; i++) {
		fread(buf, 1, 4, f);
		m->tiles[i] = BUF2INT(buf);
	}

	fread(buf, 1, 1, f);
	if(buf[0] == '\x6f') {
		Event *e = event_load(f);
		printf("event \"");
		for(int i = 0; i < strlen(e->name); i++)
			printf("\\x%hhx", e->name[i]);
		printf("\" (0x%x)\n", e->id);
		printf("(%d, %d), %d pages\n", e->x, e->y, e->npage);
	} else {
		printf("unknown entity type 0x%x\n", buf[0]);
	}

	fclose(f);
	return m;
}

void
map_free(Map *m)
{
	free(m->tiles);
	free(m);
}