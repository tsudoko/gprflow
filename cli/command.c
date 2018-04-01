#include <stdarg.h>
#include <stdio.h>

#include <wolfrpg.h>

#include "../core/core.h"

void
fmsg(Game *g, char *msg)
{
	printf("---\n%s\n", msg);
	getchar();
}

int
fchoice(Game *g, int n, char **c, int def, int keyflags)
{
	printf("[choice (def: %d)]\n", def);
	for(int i = 0; i < n; i++)
		printf("  [%d] %s\n", i+1, c[i]);
	if(keyflags & 1)
		printf("  [left] ??? (can't choose in cli)\n");
	if(keyflags & (1<<1))
		printf("  [right] ??? (can't choose in cli)\n");
	if(keyflags & (1<<2))
		printf("  [interrupt] ??? (can't choose in cli)\n");
	printf("> ");

	char choice = getchar();
	if(choice == EOF) {
		if(def == 1) {
			printf("\n[can't cancel]\n");
			return fchoice(g, n, c, def, keyflags);
		} else {
			return def;
		}
	}

	choice -= '0';
	if(choice > n || choice <= 0) {
		printf("\n[not in range (%d, %d)]\n", 1, n);
		return fchoice(g, n, c, def, keyflags);
	}

	return choice+1; /* first choice is 2 */
}
