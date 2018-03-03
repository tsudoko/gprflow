typedef struct {
	unsigned int tileset;

	unsigned int w, h;
	unsigned int nevent;

	unsigned int *tiles; /* w*h*3 */

	/* Event *ev; */
} Map;

Map *map_load(char *filename);
void map_print(Map *m);
void map_free(Map *m);
