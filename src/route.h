typedef struct {
	unsigned char id;
	int *args;
} Route;

int route_load(Reader *, Route *);
