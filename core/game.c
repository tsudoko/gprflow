#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <wolfrpg.h>

#include "core.h"

#define Pathmax 4096

/* note: posix-1.2001 source (localtime_r) */

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
	/* compatibility note: wolfrpg returns "<<ERROR>>" on invalid string queries */
	char *mapname = database_igetstr(g->db[RefSysDatabase], 0, id, 0);
	if(mapname == NULL || strlen(mapname) == 0)
		return -1;

	if(snprintf(p, sizeof p, "%s/Data/%s", g->basepath, mapname) > sizeof p)
		return -1;
	Map *m = map_load(p);
	if(m == NULL)
		return -1;

	/* TODO: copy auto event first
	if(g->map != NULL)
		map_free(g->map);
	*/
	g->map = m;
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

int
game_sysgeti(Game *g, int i)
{
	switch(i) {
	case SysYear:
	case SysMonth:
	case SysDay:
	case SysHour:
	case SysMin:
	case SysSec: {
		struct tm t;
		time_t ts = time(NULL);
		localtime_r(&ts, &t);
		switch(i) {
		case SysYear:
			return t.tm_year;
		case SysMonth:
			return t.tm_mon;
		case SysDay:
			return t.tm_mday;
		case SysHour:
			return t.tm_hour;
		case SysMin:
			return t.tm_min;
		case SysSec:
			return t.tm_sec;
		}
		break;
	}
	case SysScreenSize:
		return 0;
	case SysTestPlay:
		return 1;
	default:
		printf("unimplemented sysvar: %d\n", i);
		return -1;
	}
}

Game *
game_init(char *basepath)
{
	Game *g = game_load(basepath);
	if(g == NULL)
		return NULL;

	g->map = NULL;
	g->autoev.ncmd = 0;
	g->running = 1;

	g->pc[EvX] = database_igetint(g->db[RefSysDatabase], 7, 0, 1);
	g->pc[EvY] = database_igetint(g->db[RefSysDatabase], 7, 0, 2);
	g->pc[EvPX] = g->pc[EvX]*2;
	g->pc[EvPY] = g->pc[EvY]*2;

	if(game_mapload(g, database_igetint(g->db[RefSysDatabase], 7, 0, 0)) < 0)
		return NULL;

	return g;
}

int
game_maptick(Game *g)
{
	if(g->autoev.ncmd != 0) {
		switch(event_tick(g, &g->autoev)) {
		case EvReturn:
			g->autoev.ncmd = 0;
			return 0;
		case EvRunning:
			return 1;
		default:
			return -1;
		}
	}

	/* TODO: parallel common events */
	/* TODO: auto common events (after auto map events) */
	for(int i = 0; i < g->map->nev; i++) {
		MapEvent *e = g->map->evs + i;
		for(int j = 0; j < e->npage; j++) {
			Page *p = e->pages + j;
			/* look for an automatic event (TODO parallel) */
			#define PageEvAuto 1
			if(p->conditions[0] != PageEvAuto)
				continue;
			#undef PageEvAuto

			event_frompage(&g->autoev, p);
			return 1;
		}
	}

	return -1;
}
