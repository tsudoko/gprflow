/* this header file depends on reader.h */

typedef struct {
	unsigned char id;
	int *args;
} Route;

typedef struct {
	unsigned char narg;
	int id;
	unsigned char *args;
	unsigned char nstrarg;
	unsigned char **strargs;

	/* FIXME: move out or make optional, it's not used in most commands */
	struct {
		unsigned char something[5];
		unsigned char flags;
		int nroute;
		Route *routes;
	} movedata;
} Command;

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
	Route *routes;
	int ncmd;
	Command *cmds;

	unsigned char shadow;
	unsigned char colw, colh;
} Page;

void page_load(Reader *r, Page *p);
void page_free(Page *p);
void page_print(Page *p);
