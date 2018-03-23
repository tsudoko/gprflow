#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define _POSIX_SOURCE
#include <errno.h>
#include <iconv.h>

#include "reader.h"

#define nelem(x) (sizeof(x)/sizeof((x)[0]))

Reader *
rnew(char *strenc)
{
	Reader *r = malloc(sizeof *r);
	if(strenc != NULL)
		r->iconv = iconv_open("UTF-8", strenc);
	else
		r->iconv = NULL;
	return r;
}

void
rfree(Reader *r)
{
	if(r->iconv != NULL)
		iconv_close(r->iconv);
	free(r);
}

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
readbytearray(Reader *r, int *len_)
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
	if(len_ != NULL)
		*len_ = len;
	return str;
}

static inline unsigned char *
readstrraw(Reader *r)
{
	int len;
	unsigned char *str = readbytearray(r, &len);
	assert(str[len-1] == '\0');
	return str;
}

#define BUFLEN 64
static inline unsigned char *
readstriconv(Reader *r)
{
	int totalleft = readint(r);
	assert(totalleft > 0);

	size_t inleft = 0;
	char in[BUFLEN], *ip = in;
	size_t outlen = BUFLEN, outleft = outlen;
	char *out = malloc(outlen), *op = out;
	while(totalleft) {
		int nread = BUFLEN - inleft;
		if(totalleft-inleft < nread)
			nread = totalleft-inleft;

		rloadn(r, nread);
		memcpy(in+inleft, r->buf, nread);
		inleft += nread;

		ip = in;
		int iow = iconv(r->iconv, &ip, &inleft, &op, &outleft);
		totalleft -= ip-in;
		int ierr = errno;

		if(in != ip && inleft)
			memmove(in, ip, inleft);

		if(iow != (size_t)-1)
			continue;

		if(ierr == E2BIG) {
			outlen += BUFLEN;
			outleft += BUFLEN;
			char *new = realloc(out, outlen);
			assert(new != NULL);
			out = new;
			op = out + (outlen-outleft);
		} else if(ierr != EINVAL) {
			size_t err1len = strlen("...decoding error: "),
			       err2len = strlen(strerror(ierr)),
			       errlen = err1len + err2len - 1;
			if(outleft < errlen) {
				outlen += errlen;
				outleft += errlen;
				char *new = realloc(out, outlen);
				assert(new != NULL);
				out = new;
				op = out + (outlen-outleft);
			}
			strcpy(op, "...decoding error: ");
			op += err1len;
			strcpy(op, strerror(ierr));
			op += err2len + 1;
			break;
		}
	}
	assert(op[-1] == '\0');
	return (unsigned char *)out;
}
#undef BUFLEN

unsigned char *
readstr(Reader *r)
{
	if(r->iconv != NULL)
		return readstriconv(r);
	else
		return readstrraw(r);
}

/* guaranteed to leave the first differing part of the string in r->buf */
/* TODO: more diagnostic info? */
int
readncmp(Reader *r, const unsigned char *s2, int len)
{
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
