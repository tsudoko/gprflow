#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <wolfrpg.h>

#include "game.h"

#define Pathmax 4096

int
game_dbload(Game *g, char *basename, int id)
{
	char p1[Pathmax];
	char p2[Pathmax];
	if(snprintf(p1, sizeof p1, "%s/Data/BasicData/SysDatabase.project", g->basepath) > sizeof p1)
		return -1;
	if(snprintf(p2, sizeof p2, "%s/Data/BasicData/SysDatabase.dat", g->basepath) > sizeof p2)
		return -1;
	Database *db = database_load(p1, p2);
	if(db == NULL)
		return -1;
	g->db[RefSysDatabase] = db;
	return 0;
}

int
game_cevload(Game *g)
{
	char p[Pathmax];
	if(snprintf(p, sizeof p, "%s/Data/BasicData/CommonEvent.dat", g->basepath) > sizeof p)
		return -1;

	g->cev = commonevent_load(p);
	if(g->cev == NULL)
		return -1;

	return 0;
}

int
game_mapload(Game *g, int id)
{
	char p[Pathmax];
	char *mapname = database_igetstr(g->db[RefSysDatabase], 0, id, 0);
	if(mapname == NULL || strlen(mapname) == 0)
		return -1;

	if(snprintf(p, sizeof p, "%s/Data/%s", g->basepath, mapname) > sizeof p)
		return -1;
	g->map = map_load(p);
	if(g->map == NULL)
		return -1;

	/* TODO: set up bgm, redraw? */
	return 0;
}

Game *
game_load(char *basepath)
{
	Game *g = malloc(sizeof *g);
	g->basepath = basepath;

	if(game_dbload(g, "SysDatabase", RefSysDatabase) < 0)
		return NULL;
	if(game_dbload(g, "DataBase", RefDataBase) < 0)
		return NULL;
	if(game_dbload(g, "CDataBase", RefCDataBase) < 0)
		return NULL;
	if(game_cevload(g) < 0)
		return NULL;

	return g;
}

Game *
game_init(char *basepath)
{
	Game *g = game_load(basepath);
	char p[Pathmax];
	if(g == NULL)
		return NULL;

	if(game_mapload(g, database_igetint(g->db[RefSysDatabase], 7, 0, 0)) < 0)
		return NULL;

	/* TODO: execute 自動イベント from teh current map and common events */
	return g;
}