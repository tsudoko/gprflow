/* this header file depends on stdio.h */

typedef struct {
	char narg;
	int id;
	char *args;
	char nstrarg;
	char **strargs;
} Command;

typedef struct {
	unsigned char id;
	int *args;
} Routecmd;

typedef struct {
	int id;
	int something;

	unsigned char *name;
	unsigned char direction;
	unsigned char frame;
	unsigned char opacity;
	unsigned char rendermode;

	unsigned char conditions[1 + 4 + 4*4 + 4*4];
	unsigned char movement[4];

	unsigned char flags;
	unsigned char routeflags;

	int nroute;
	Routecmd *routes;
	int ncmd;
	Command *cmds;

	int shadow;
	int colw, colh;
} Page;

void page_load(FILE *f, Page *p);
void page_free(Page *p);
void page_print(Page *p);
