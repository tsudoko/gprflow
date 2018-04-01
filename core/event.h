struct loop_state {
	unsigned int startline;
	unsigned int i;
};

struct event_state {
	long self[100]; /* TODO check if value range is correct */
	unsigned int ncmd;
	Command *cmds;

	unsigned int waitframes;

	int branch;
	unsigned int branches[64];

	unsigned int line;

	Command reserved;
	struct event_state *caller;
	struct event_state *callee;
};

enum event_tickret {
	EvReturn,
	EvRunning,
	EvFork,
};

struct game;

void event_frompage(struct event_state *, Page *);
int event_tick(struct game *, struct event_state *);
