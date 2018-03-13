#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <iconv.h>

#include "reader.h"
#include "database.h"

static const int OFFINT = 0x3e8;
static const int OFFSTR = 0x7d0;
static const unsigned char DATMAGIC[] = "W\0\0OL\0FM\0\xc1";

const char *
field_typestr(Field *f)
{
	switch(f->type) {
	case FieldRegular:   return "Regular"; break;
	case FieldFilename:  return "Filename"; break;
	case FieldReference: return "Reference"; break;
	case FieldEnum:      return "Enum"; break;
	default:             return "???";
	}
}

const char *
field_refdeststr(Field *f)
{
	switch(f->args[0]) {
	case RefSysDatabase: return "SysDatabase"; break;
	case RefDataBase:    return "DataBase"; break;
	case RefCDataBase:   return "CDataBase"; break;
	case RefCommonEvent: return "CommonEvent"; break;
	default:             return "???";
	}
}

void
type_countvalues(Type *t, int *nint, int *nstr)
{
	*nint = 0, *nstr = 0;
	for(int i = 0; i < t->nfield; i++)
		if(t->fields[i].offset >= OFFSTR)
			(*nstr)++;
		else
			(*nint)++;
}

void
type_loadproject(Reader *r, Type *t)
{
	t->name = readstr(r);
	t->nfield = readint(r);

	t->fields = malloc(sizeof *t->fields * t->nfield);
	for(int i = 0; i < t->nfield; i++)
		t->fields[i].name = readstr(r);

	t->ndata = readint(r);
	t->data = malloc(sizeof *t->data * t->ndata);
	for(int i = 0; i < t->ndata; i++)
		t->data[i].name = readstr(r);

	t->desc = readstr(r);

	int n = readint(r), i;
	assert(n >= t->nfield);
	for(i = 0; i < t->nfield; i++)
		t->fields[i].type = readbyte(r);
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
	for(i = 0; i < n; i++) {
		Field *f = &t->fields[i];
		f->narg = readint(r);
		if(f->type == FieldReference)
			assert(f->narg == 3);
		f->args = malloc(sizeof *f->args * f->narg);
		for(int j = 0; j < f->narg; j++)
			f->args[j] = readint(r);
	}

	n = readint(r);
	assert(n >= t->nfield);
	for(i = 0; i < n; i++)
		t->fields[i].default_ = readint(r);
}

void
type_loaddat(Reader *r, Type *t)
{
	if(readncmp(r, (unsigned char *)"\xfe\xff\xff\xff", 4) != 0)
		printf("unexpected dat separator: \\x%x\\x%x\\x%x\\x%x\n", r->buf[0], r->buf[1], r->buf[2], r->buf[3]);

	t->something = readint(r);
	int nfield = readint(r);
	assert(nfield <= t->nfield);
	for(int i = 0; i < nfield; i++)
		t->fields[i].offset = readint(r);

	int ndata = readint(r), nint, nstr;
	assert(ndata <= t->ndata);

	type_countvalues(t, &nint, &nstr);

	for(int i = 0; i < ndata; i++) {
		t->data[i].nint = nint;
		t->data[i].ints = malloc(sizeof *t->data[i].ints * nint);
		for(int j = 0; j < t->data[i].nint; j++)
			t->data[i].ints[j] = readint(r);

		t->data[i].nstr = nstr;
		t->data[i].strs = malloc(sizeof *t->data[i].strs * nstr);
		for(int j = 0; j < t->data[i].nstr; j++)
			t->data[i].strs[j] = readstr(r);
	}
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
	for(int i = 0; i < t->ndata; i++) {
		free(t->data[i].name);
		free(t->data[i].ints);
		for(int j = 0; j < t->data[i].nstr; j++)
			free(t->data[i].strs[j]);
		free(t->data[i].strs);
	}
	free(t->data);
	free(t->desc);
}

void
type_print(Type *t)
{
	printf("%s\n", t->name);
	printf("%s\n", t->desc);
	printf("  %d fields, %d data\n", t->nfield, t->ndata);
	for(int f = 0; f < t->nfield; f++) {
		printf("  - %s: %s<%s>[%d]", t->fields[f].name, field_typestr(&t->fields[f]), (t->fields[f].offset >= OFFSTR ? "str" : "int"), t->fields[f].narg);


		if(t->fields[f].type == FieldReference)
			printf("\t-> %s[%d]", field_refdeststr(&t->fields[f]), t->fields[f].args[1]);
		printf("\n");
	}
	for(int i = 0; i < t->ndata; i++) {
		printf("  %s\n", t->data[i].name);
		for(int f = 0; f < t->nfield; f++) {
			printf("    %s: ", t->fields[f].name);
			if(t->fields[f].offset >= OFFSTR)
				printf("%s\n", t->data[i].strs[t->fields[f].offset - OFFSTR]);
			else
				printf("%d\n", t->data[i].ints[t->fields[f].offset - OFFINT]);
		}
	}
}

Database *
database_load(char *projectpath, char *datpath)
{
	Reader *r = rnew("SHIFT_JIS");
	Database *d = malloc(sizeof *d);

	r->f = fopen(projectpath, "rb");
	d->n = readint(r);
	d->t = malloc(sizeof *d->t * d->n);
	for(int i = 0; i < d->n; i++)
		type_loadproject(r, d->t + i);
	fclose(r->f);

	printf("にゃーん\n");

	r->f = fopen(datpath, "rb");
	assert(readbyte(r) == 0); /* encrypted if !0 */
	if(readncmp(r, DATMAGIC, sizeof DATMAGIC-1) != 0) {
		printf("unexpected dat magic\n");
		return NULL;
	}

	int dn = readint(r);
	assert(dn == d->n);

	for(int i = 0; i < d->n; i++)
		type_loaddat(r, d->t + i);

	if(readbyte(r) != (unsigned char)'\xc1')
		printf("unexpected dat terminator: \\x%x\n", r->buf[0]);
	fclose(r->f);
	rfree(r);
	return d;
}

void
database_free(Database *d)
{
	for(int i = 0; i < d->n; i++)
		type_free(d->t + i);
	free(d->t);
	free(d);
}

void
database_print(Database *d)
{
	printf("%d types\n", d->n);
	for(int i = 0; i < d->n; i++) {
		printf("%d: ", i);
		type_print(d->t + i);
	}
}
