typedef struct game Game;
struct game {
	char *basepath;
	int running;
	struct event_state autoev;
	int pc[10];
	Database *db[3];
	CommonEvents *cev;
	Map *map;
};

enum {
	EvX,
	EvY,
	EvPX,
	EvPY,
};

enum {
	/* --- */

	SysMouseX = 71,
	SysMouseY,
	SysMouseInput,
	SysMouseShow,

	SysYear = 77,
	SysMonth,
	SysDay,
	SysHour,
	SysMin,
	SysSec,

	SysScreenSize = 84,
	SysSoftRender = 85,

	SysParty1Shadow = 87,
	SysParty2Shadow,
	SysParty3Shadow,
	SysParty4Shadow,
	SysParty5Shadow,

	/* --- */

	SysRandomSeed = 110,
	SysScreenActive,
	SysTestPlay,

	SysGameVer = 115,
	SysScreenW,
	SysScreenH,
	SysTileSize,

	/* --- */
};

Game *game_init(char *);
int game_maptick(Game *);
int game_mapload(Game *, int);
