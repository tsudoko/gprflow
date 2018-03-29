/* this header depends on command.h */

typedef struct {
	int id;
	int unknown1;
	unsigned char unknown2[7];
	unsigned char *name;
	int ncmd;
	Command *cmds;
	unsigned char *unknown11; /* str */
	unsigned char *desc;
	unsigned char *unknown3[10]; /* str */
	unsigned char unknown4[10];
	struct _ev_u5 {
		int n;
		unsigned char **v; /* str */
	} unknown5[10];
	struct _ev_u6 {
		int n;
		int *v;
	} unknown6[10];
	unsigned char unknown7[0x1d]; /* not str */
	unsigned char *unknown8[100]; /* str */
	unsigned char *unknown9; /* str */
	unsigned char *unknown10; /* str */
	int unknown12;
} Event;

typedef struct {
	int n;
	Event *e;
} CommonEvents;

CommonEvents *cev_load(char *);
void cev_free(CommonEvents *);
void cev_print(CommonEvents *);
#define commonevent_load cev_load
#define commonevent_free cev_free
#define commonevent_print cev_print
