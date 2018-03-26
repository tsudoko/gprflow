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

typedef struct {
	int id;
	unsigned char *name;
	int x, y;
	int npage;
	Page *pages;
} MapEvent;

typedef struct {
	unsigned int tileset;

	unsigned int w, h;
	unsigned int nev;

	unsigned int *tiles; /* w*h*3 */

	MapEvent *evs;
} Map;

Map *map_load(char *filename);
void map_print(Map *m);
void map_free(Map *m);
