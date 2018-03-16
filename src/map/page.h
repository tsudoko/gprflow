/* this header file depends on reader.h and command.h */

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
