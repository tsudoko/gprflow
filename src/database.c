#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "reader.h"

void
type_loadproject(Reader *r, Type *t)
{
	t->name = readstr(r);
	t->nfield = readint(r);

	t->fields = malloc(sizeof *t->fields * t->nfield);
	for(int i = 0; i < t->nfield; i++)
		t->fields[i].name = readstr(r);

	t->ndata = readint(r);
	for(int i = 0; i < t->ndata; i++)
		t->data[i].name = readstr(r);

	t->desc = readstr(r);

	int n = readint(r), i;
	printf("field list size: %d\n", n);
	assert(n >= t->nfield);
	for(i = 0; i < nfield; i++)
		t->fields[i].type = readbyte(r);
	printf("real field list end: 0x%x\n", ftell(f));
	fseek(r->f, n-i, SEEK_CUR);

	n = readint(r);
	assert(n >= t->nfield);
	for(i = 0; i < n; i++)
		t->fields[i].something = readstr(r);

	n = readint(r);
	assert(n >= t->nfield);
	for(i = 0; i < n; i++) {
		Field *f = &t->fields[i];
		f->nstrarg = readint(r);
		f->strargs = malloc(sizeof *f->strargs * f->nstrarg);
		for(int j = 0; j < f->nstrarg; j++)
			f->strargs[j] = readstr(r);
	}

	n = readint(r);
	assert(n >= t->nfield);
	for(i = 0; i < n; i++)
		Field *f = &t->fields[i];
		f->narg = readint(r);
		f->args = malloc(sizeof *f->args * f->narg);
		for(int j = 0; j < f->narg; j++)
			f->args[j] = readint(r);
	}

	n = readint(r);
	assert(n >= t->nfield);
	for(i = 0; i < n; i++)
		t->fields[i].default = readint(r);
}

void
type_loaddat(Reader *r, Type *t)
{
	TODO;
}

void
type_free(Type *t)
{
	free(t->name);
	for(int i = 0; i < t->nfield; i++) {
		free(t->fields[i].name);
		free(t->fields[i].something);
		for(int j = 0; j < t->fields[i].nstrarg; j++)
			free(t->fields[i].strargs[j]);
		free(t->fields[i].strargs);
		free(t->fields[i].args);
	}
	free(t->fields);
	for(int i = 0; i < t->ndata; i++)
		free(t->data[i].name);
	free(t->desc);
	free(t);
}

Database *
database_load(char *projectpath, char *datpath)
{
	Reader r;
	Database *d = malloc(sizeof *d);

	r.f = fopen(projectpath, "rb");
	d->n = readint(r);
	d->t = malloc(d->n * sizeof (int));
	for(int i = 0; i < d->n; i++)
		type_loadproject(&r, d->t + i);
	fclose(r.f);

	r.f = fopen(datpath, "rb");
	for(int i = 0; i < d->n; i++)
		type_loaddat(&r, d->t + i);
	fclose(r.f);

	return d;
}
