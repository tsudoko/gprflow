typedef struct {
	unsigned int startline;
	unsigned int i;
} LoopState;

typedef struct {
	long self[100]; /* TODO check if value range is correct */
	/* not sure if these two are needed */
	unsigned int ncmd;
	Command *cmds;
	unsigned int labels[64]; /* arbitrary length, meant to be a hash table (labelname -> linenum) */
	LoopState loops[64]; /* index is loop depth */
	Command reserved;
} EventState;
