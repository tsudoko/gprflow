typedef struct {
	unsigned int tileset;

	unsigned int w, h;
	unsigned int nev;

	unsigned int *tiles; /* w*h*3 */

	Event *evs;
} Map;

Map *map_load(char *filename);
void map_print(Map *m);
void map_free(Map *m);
