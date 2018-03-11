#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "reader.h"

#define nelem(x) (sizeof(x)/sizeof((x)[0]))

void
rloadn(Reader *r, int n)
{
	assert(n <= nelem(r->buf));
	/* TODO: checked fread, encryption */
	fread(r->buf, 1, n, r->f);
}

unsigned char
readbyte(Reader *r)
{
	rloadn(r, 1);
	return *r->buf;
}

int
readint(Reader *r)
{
	rloadn(r, 4);
	return r->buf[0] | (r->buf[1] << 8) | (r->buf[2] << 16) | (r->buf[3] << 24);
}

unsigned char *
readstr(Reader *r)
{
	int len = readint(r);
	assert(len > 0);
	unsigned char *str = malloc(len), *b = str;
	int n = len/nelem(r->buf);
	for(int i = 0; i < n; i++) {
		rloadn(r, nelem(r->buf));
		memcpy(b, r->buf, nelem(r->buf));
		b += nelem(r->buf);
	}
	rloadn(r, len%nelem(r->buf));
	memcpy(b, r->buf, len%nelem(r->buf));
	assert(str[len-1] == '\0');
	return str;
}

/* guaranteed to leave the first differing part of the string in r->buf */
/* TODO: more diagnostic info? */
int
readncmp(Reader *r, const unsigned char *s2, int len)
{
	unsigned char *str = malloc(len), *b = str;
	int n = len/nelem(r->buf), ret;
	for(int i = 0; i < n; i++) {
		rloadn(r, nelem(r->buf));
		if((ret = memcmp(r->buf, s2, nelem(r->buf))) != 0)
			return ret;
		s2 += nelem(r->buf);
	}
	rloadn(r, len%nelem(r->buf));
	return memcmp(r->buf, s2, len%nelem(r->buf));
}
