/* this header file depends on reader.h and page.h */

typedef enum {
	EV_BLANK,
} Evid;

typedef struct {
	int id;
	unsigned char *name;
	int x, y;
	int npage;
	Page *pages;
} Event;

void event_load(Reader *r, Event *);
void event_free(Event *e);
