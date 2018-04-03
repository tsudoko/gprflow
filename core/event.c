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
		/* TODO: sync with actual frames */
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
		/* TODO: parse escape codes (c->strargs[0]) */
		fmsg(g, c->strargs[0]);
		break;
	case CmdChoice: {
		/* TODO: parse escape codes (c->strargs) */
		int r = fchoice(g, c->nstrarg, c->strargs, (c->args[0]&0xff)>>4, c->args[0]>>8);
		int cid = r == 0 ? CmdCancelCase : (r >= 100 ? CmdSpecialChoiceCase : CmdChoiceCase);
		/* XXX this might be slow for larger events, might want to move it to 1stpass */
		for(int i = s->line; i < s->ncmd; i++) {
			if(s->cmds[i].id == cid &&
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
	case CmdTeleport:
		assert(c->narg >= 5);
		if(c->args[0] <= -1) { /* location from sysdb/7 */
			if(c->args[4]&TpPrecisePos) {
				g->pc[EvX] = c->args[1]/2;
				g->pc[EvY] = c->args[2]/2;
				g->pc[EvPX] = c->args[1];
				g->pc[EvPY] = c->args[2];
			} else {
				g->pc[EvX] = c->args[1];
				g->pc[EvY] = c->args[2];
				g->pc[EvPX] = c->args[1]*2;
				g->pc[EvPY] = c->args[2]*2;
			}
			if(c->args[0] < -1) {
				/* TODO */
				printf("Teleport: using predefined locations is not implemented\n");
			} else {
				/* TODO: handle transitions */
				if(game_mapload(g, c->args[3]) < 0)
					ferrmsg(g, "failed to change map to %d\n", c->args[3]);
			}
		} else {
			printf("Teleport: moving non-pc events is not implemented\n");
			/* TODO */
		}
		break;
	case CmdEndGame:
		g->running = 0;
		break;
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
