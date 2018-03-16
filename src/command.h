/* this header depends on reader.h and route.h */

typedef struct {
	unsigned char narg;
	int id;
	unsigned char *args;
	unsigned char nstrarg;
	unsigned char **strargs;

	/* FIXME: move out or make optional, it's not used in most commands */
	struct {
		unsigned char something[5];
		unsigned char flags;
		int nroute;
		Route *routes;
	} movedata;
} Command;

void command_load(Reader *r, Command *c);
void command_free(Command *c);
void command_print(Command *c);
