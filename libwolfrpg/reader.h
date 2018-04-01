/* this header depends on iconv.h */

typedef struct {
	FILE *f;
	iconv_t iconv;
	unsigned char buf[64];
} Reader;

Reader *       rnew(char *);
void           rfree(Reader *);
void           rloadn(Reader *r, int n);
unsigned char  readbyte(Reader *r);
int            readint(Reader *r);
unsigned char *readbytearray(Reader *, int *);
unsigned char *readstr(Reader *r);
int            readncmp(Reader *r, const unsigned char *s2, int len);
