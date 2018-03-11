typedef struct {
	FILE *f;
	unsigned char buf[64];
} Reader;

void           rloadn(Reader *r, int n);
unsigned char  readbyte(Reader *r);
int            readint(Reader *r);
unsigned char *readstr(Reader *r);
int            readncmp(Reader *r, const unsigned char *s2, int len);
