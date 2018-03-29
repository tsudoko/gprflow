typedef struct {
	char *basepath;
	Database *db[3];
	CommonEvents *cev;
	Map *map;
} Game;

Game *game_init(char *);
