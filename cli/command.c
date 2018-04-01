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
fchoice(Game *g, int n, char **c, int def)
{
	printf("[choice]\n");
	for(int i = 0; i < n; i++)
		printf("  [%d] %s\n", i+1, c[i]);
	printf("> ");

	char choice = getchar();
	if(choice == EOF)
		return def;

	choice -= '0';
	if(choice > n || choice <= 0) {
		printf("\n[not in range (%d, %d)]\n", 1, n);
		return fchoice(g, n, c, def);
	}

	return choice+1; /* first choice is 2 */
}
