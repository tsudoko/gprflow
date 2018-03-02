#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "page.h"
#include "event.h"

#define BUFLEN 4

static const unsigned char EVMAGIC[] = "90\0\0";
static const unsigned char PAGEMAGIC[] = "\0\0\0\0";

Event *
event_load(FILE *f)
{
	Event *e = malloc(sizeof *e);
	unsigned char buf[BUFLEN];
	int namelen;

	e->next = NULL;

	fread(buf, 1, 4, f);
	if(memcmp(buf, EVMAGIC, 4) != 0) {
		printf("invalid event magic\n");
		return NULL;
	}

	fread(buf, 1, 4, f);
	e->id = BUF2INT(buf);

	fread(buf, 1, 4, f);
	namelen = BUF2INT(buf);
	assert(namelen > 0);

	e->name = malloc(namelen);
	for(int i = 0; i < namelen; i++) {
		fread(buf, 1, 1, f);
		e->name[i] = *buf;
	}
	assert(e->name[namelen-1] == '\0');

	fread(buf, 1, 4, f);
	e->x = BUF2INT(buf);

	fread(buf, 1, 4, f);
	e->y = BUF2INT(buf);

	fread(buf, 1, 4, f);
	e->npage = BUF2INT(buf);
	e->pages = malloc(sizeof *e->pages * e->npage);

	fread(buf, 1, 4, f);
	if(memcmp(buf, PAGEMAGIC, 4) != 0)
		return NULL;

	for(int i = 0; i < e->npage; i++) {
		fread(buf, 1, 1, f);
		if(buf[0] != '\x79') {
			printf("entity not a page: 0x%x\n", buf[0]);
			e->npage = i;
			break;
		}
		printf("loading page %d\n", i);
		page_load(f, &(e->pages[i]));
		page_print(&(e->pages[i]));
	}

	fread(buf, 1, 1, f);
	if(*buf != '\x70')
		printf("unexpected page list terminator: \\x%x\n", *buf);

	return e;
}

void
event_free(Event *e)
{
	free(e->pages);
	free(e->name);
	free(e);
}
