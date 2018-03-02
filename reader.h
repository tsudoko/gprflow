typedef struct {
	FILE *f;
	unsigned char buf[4]; /* TODO change to 64 or something higher after testing */
} Reader;

void           readn(Reader *r, int n);
unsigned char  readbyte(Reader *r);
int            readint(Reader *r);
unsigned char *readstr(Reader *r);
int            readncmp(Reader *r, char *s2, int len);
