/* this header file depends on reader.h and page.h */

typedef enum {
	EV_BLANK,
} Evid;

typedef struct _event {
	int id;
	unsigned char *name; /* looks like sjis */
	int x, y;
	int npage;
	Page *pages;

	struct _event *next;
} Event;

Event *event_load(Reader *r);
void event_free(Event *e);
