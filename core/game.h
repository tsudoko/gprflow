typedef struct game Game;
struct game {
	char *basepath;
	struct event_state autoev;
	Database *db[3];
	CommonEvents *cev;
	Map *map;
};

Game *game_init(char *);
int game_maptick(Game *);
