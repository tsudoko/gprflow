typedef struct {
	unsigned char *name;
	int nfield;
	Field *fields;
	int ndata;
	Typedata *data;
	unsigned char *desc;
} Type;

typedef struct {
	unsigned char *name; /* str */
	unsigned char type;
	unsigned char *something; /* str */
	int nstrarg;
	unsigned char **strargs; /* *str */
	int narg;
	int *args;
	int default;
} Field;

typedef struct {
	unsigned char *name; /* str, from the project file */
	int nint;
	int *ints;
	int nstr;
	unsigned char **strs;
} Typedata;
