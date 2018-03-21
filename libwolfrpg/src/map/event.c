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

static const unsigned char EVMAGIC[] = "90\0\0";
static const unsigned char PAGEMAGIC[] = "\0\0\0\0";

void
event_load(Reader *r, Event *e)
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
		page_print(&e->pages[i]);
	}

	if(readbyte(r) != '\x70')
		printf("unexpected page list terminator: \\x%x\n", r->buf[0]);
}

void
event_free(Event *e)
{
	for(int i = 0; i < e->npage; i++)
		page_free(e->pages + i);
	free(e->pages);
	free(e->name);
}
