struct loop_state {
	unsigned int startline;
	unsigned int i;
};

struct event_state {
	long self[100]; /* TODO check if value range is correct */
	/* not sure if these two are needed */
	unsigned int ncmd;
	Command *cmds;
	unsigned int line;
	unsigned int labels[64]; /* arbitrary length, meant to be a hash table (labelname -> linenum) */
	struct loop_state loops[64]; /* index is loop depth */
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
