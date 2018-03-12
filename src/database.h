typedef struct {
	unsigned char *name;
	unsigned char type;
	unsigned char *something;
	int nstrarg;
	unsigned char **strargs;
	int narg;
	int *args;
	int default_;
	int offset;
} Field;

typedef struct {
	unsigned char *name;
	int nint;
	int *ints;
	int nstr;
	unsigned char **strs;
} Typedata;

typedef struct {
	unsigned char *name;
	int nfield;
	Field *fields;
	int something;
	int ndata;
	Typedata *data;
	unsigned char *desc;
} Type;

typedef struct {
	int n;
	Type *t;
} Database;

Database *database_load(char *, char *);
void database_print(Database *);
void database_free(Database *);
