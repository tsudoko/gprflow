#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define _POSIX_SOURCE
#include <iconv.h>

#include "reader.h"
#include "route.h"
#include "command.h"
#include "commonevent.h"

#define nelem(x) (sizeof(x)/sizeof((x)[0]))

static const unsigned char CEVMAGIC[] = "\0W\0\0OL\0FC\0\x8f";
static const unsigned char EVMAGIC[] = "\x0a\0\0\0";

static int
event_load(Reader *r, Event *e)
{
	unsigned char b;
	e->unknown10 = NULL;
	e->unknown12 = 0;
	if((b = readbyte(r)) != 0x8e) {
		printf("unexpected event header: 0x%x\n", b);
		return -1;
	}

	e->id = readint(r);
	e->unknown1 = readint(r);
	rloadn(r, sizeof e->unknown2);
	memcpy(e->unknown2, r->buf, sizeof e->unknown2);
	e->name = readstr(r);

	e->ncmd = readint(r);
	e->cmds = malloc(sizeof *e->cmds * e->ncmd);
	for(int i = 0; i < e->ncmd; i++)
		command_load(r, &e->cmds[i]);

	e->unknown11 = readstr(r);
	e->desc = readstr(r);

	if((b = readbyte(r)) != 0x8f) {
		printf("unexpected event data header: 0x%x\n", b);
		return -1;
	}
	if(readncmp(r, EVMAGIC, sizeof EVMAGIC-1) != 0) {
		printf("unexpected event magic\n");
		return -1;
	}
	for(int i = 0; i < nelem(e->unknown3); i++)
		e->unknown3[i] = readstr(r);

	if(readncmp(r, EVMAGIC, sizeof EVMAGIC-1) != 0) {
		printf("unexpected event magic\n");
		return -1;
	}
	for(int i = 0; i < nelem(e->unknown4); i++)
		e->unknown4[i] = readbyte(r);

	if(readncmp(r, EVMAGIC, sizeof EVMAGIC-1) != 0) {
		printf("unexpected event magic\n");
		return -1;
	}
	for(int i = 0; i < nelem(e->unknown5); i++) {
		struct _ev_u5 *u = &e->unknown5[i];
		u->n = readint(r);
		u->v = malloc(sizeof *u->v * u->n);
		for(int j = 0; j < u->n; j++)
			u->v[j] = readstr(r);
	}

	if(readncmp(r, EVMAGIC, sizeof EVMAGIC-1) != 0) {
		printf("unexpected event magic\n");
		return -1;
	}
	for(int i = 0; i < nelem(e->unknown6); i++) {
		struct _ev_u6 *u = &e->unknown6[i];
		u->n = readint(r);
		u->v = malloc(sizeof *u->v * u->n);
		for(int j = 0; j < u->n; j++)
			u->v[j] = readint(r);
	}

	for(int i = 0; i < nelem(e->unknown7); i++)
		e->unknown7[i] = readbyte(r);

	for(int i = 0; i < nelem(e->unknown8); i++)
		e->unknown8[i] = readstr(r);

	if((b = readbyte(r)) != 0x91) {
		printf("unexpected byte: 0x%x\n", b);
		return -1;
	}
	e->unknown9 = readstr(r);

	b = readbyte(r);
	if(b == 0x91) {
		return 0;
	} else if(b != 0x92) {
		printf("unexpected byte: 0x%x\n", b);
		return -1;
	}

	e->unknown10 = readstr(r);
	e->unknown12 = readint(r);
	if((b = readbyte(r)) != 0x92) {
		printf("unexpected byte: 0x%x\n", b);
		return -1;
	}

	return 0;
}

static void
event_free(Event *e)
{
	free(e->name);
	for(int i = 0; i < e->ncmd; i++)
		command_free(e->cmds + i);
	free(e->cmds);
	free(e->unknown11);
	free(e->desc);
	for(int i = 0; i < nelem(e->unknown3); i++)
		free(e->unknown3[i]);
	for(int i = 0; i < nelem(e->unknown5); i++) {
		for(int j = 0; j < e->unknown5[i].n; j++)
			free(e->unknown5[i].v[j]);
		free(e->unknown5[i].v);
	}
	for(int i = 0; i < nelem(e->unknown6); i++)
		free(e->unknown6[i].v);
	for(int i = 0; i < nelem(e->unknown8); i++)
		free(e->unknown8[i]);
	free(e->unknown9);
	if(e->unknown10 != NULL)
		free(e->unknown10);
}

static void
event_print(Event *e)
{
	printf("%s (%x)\n", e->name, e->id);
	printf("%d commands\n", e->ncmd);
	printf("%s\n", e->desc);
	printf("  unknowns\n");
	printf("     1 %d\n", e->unknown1);
	printf("     2 ");
	for(int i = 0; i < sizeof e->unknown2; i++)
		printf("\\x%02x", e->unknown2[i]);
	printf("\n");
	printf("    11 %s\n", e->unknown11);
	printf("     3 [");
	for(int i = 0; i < nelem(e->unknown3); i++)
		printf("\"%s\" ", e->unknown3[i]);
	printf("]\n");
	printf("     4 ");
	for(int i = 0; i < sizeof e->unknown4; i++)
		printf("\\x%02x", e->unknown4[i]);
	printf("\n");
	printf("     5 [");
	for(int i = 0; i < nelem(e->unknown5); i++) {
		printf("[");
		for(int j = 0; j < e->unknown5[i].n; j++)
			printf("\"%s\" ", e->unknown5[i].v[j]);
		printf("] ");
	}
	printf("]\n");
	printf("     6 [");
	for(int i = 0; i < nelem(e->unknown6); i++) {
		printf("[");
		for(int j = 0; j < e->unknown6[i].n; j++)
			printf("%d ", e->unknown6[i].v[j]);
		printf("] ");
	}
	printf("]\n");
	printf("     7 ");
	for(int i = 0; i < sizeof e->unknown7; i++)
		printf("\\x%02x", e->unknown7[i]);
	printf("\n");
	printf("     8 [");
	for(int i = 0; i < nelem(e->unknown8); i++)
		printf("\"%s\" ", e->unknown8[i]);
	printf("]\n");
	printf("     9 %s\n", e->unknown9);
	printf("    10 %s\n", e->unknown10);
	printf("    12 %d\n", e->unknown10);
}

CommonEvents *
cev_load(char *path)
{
	Reader *r = rnew("CP932");
	CommonEvents *c = malloc(sizeof *c);
	unsigned char b;
	r->f = fopen(path, "rb");

	if(readncmp(r, CEVMAGIC, sizeof CEVMAGIC-1) != 0) {
		printf("unexpected common event magic\n");
		return NULL;
	}

	c->n = readint(r);
	c->e = malloc(sizeof *c->e * c->n);

	for(int i = 0; i < c->n; i++)
		if(event_load(r, c->e + i) < 0)
			break;

	if((b = readbyte(r)) != 0x8f)
		printf("unexpected common event terminator: 0x%x\n", b);

	fclose(r->f);
	rfree(r);

	return c;
}

void
cev_free(CommonEvents *c)
{
	for(int i = 0; i < c->n; i++)
		event_free(c->e + i);
	free(c->e);
	free(c);
}

void
cev_print(CommonEvents *c)
{
	for(int i = 0; i < c->n; i++)
		event_print(c->e + i);
}
