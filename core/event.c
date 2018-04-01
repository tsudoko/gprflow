#include <assert.h>
#include <stdio.h>

#include <wolfrpg.h>

#include "core.h"
#include "frontend.h"

void
event_1stpass(struct event_state *e)
{
	/* TODO: handle labels here */
}

void
event_frompage(struct event_state *e, Page *p)
{
	e->ncmd = p->ncmd;
	e->cmds = p->cmds;
	e->line = 0;
	e->waitframes = 0;
	event_1stpass(e);
}

int
event_tick(struct game *g, struct event_state *s)
{
	if(s->line == s->ncmd-1)
		return EvReturn;
	Command *c = s->cmds + s->line;

	if(s->waitframes) {
		--s->waitframes;
		return EvRunning;
	}

	switch(c->id) {
	case CmdNop:
	case CmdComment:
	case CmdBranchEnd:
		break;
	case CmdMsg:
		assert(c->nstrarg >= 1);
		/* TODO: parse escape codes */
		fmsg(g, c->strargs[0]);
		break;
	case CmdChoice: {
		int r = fchoice(g, c->nstrarg, c->strargs, 3); /* FIXME: テキトー */
		/* XXX this might be slow for larger events, might want to move it to 1stpass */
		for(int i = s->line; i < s->ncmd; i++) {
			if(s->cmds[i].id == CmdChoiceCase &&
			   s->cmds[i].indent == c->indent &&
			   s->cmds[i].narg >= 1 &&
			   s->cmds[i].args[0] == r) {
				assert(i+1 < s->ncmd);
				s->line = i+1;
				return EvRunning;
			} else {
				assert(s->cmds[i].id != CmdBranchEnd);
			}
		}
		assert(1 == 2); /* XXX */
		break;
	}
	case CmdWait:
		assert(c->narg >= 1);
		s->waitframes = c->args[0];
		break;
	/* encountering this randomly means we're at the end of the previous case block */
	case CmdChoiceCase:
		/* XXX this might be slow for larger events, might want to move it to 1stpass */
		for(int i = s->line; i < s->ncmd; i++) {
			if(s->cmds[i].id == CmdBranchEnd) {
				s->line = i;
				return EvRunning;
			}
		}
		assert(1 == 2); /* XXX */
		break;
	default:
		printf("unimplemented command ");
		command_print(c);
	}

endtick:
	++s->line;
	return EvRunning;
}
