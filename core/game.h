typedef struct game Game;
struct game {
	char *basepath;
	int running;
	struct event_state autoev;
	struct {
		int x;
		int y;
		int px;
		int py;
	} pc;
	Database *db[3];
	CommonEvents *cev;
	Map *map;
};

Game *game_init(char *);
int game_maptick(Game *);
int game_mapload(Game *, int);
