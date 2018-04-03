#include <assert.h>
#include <stdio.h>

#include <wolfrpg.h>

#include "core.h"
#include "frontend.h"

int
game_compare(Game *g, int l, int r, int flags)
{
	int op = flags&0xf;
	l = game_deref_get(g, l);
	if(flags & (1<<4))
		r = game_deref_get(g, r);

	switch(op) {
		case CfCmpGt:  return l >  r;
		case CfCmpGe:  return l >= r;
		case CfCmpEq:  return l == r;
		case CfCmpLe:  return l <= r;
		case CfCmpLt:  return l <  r;
		case CfCmpNe:  return l != r;
		case CfCmpBit: return l & r == r;
		default: printf("game_compare: unknown operator: %d\n", op);
		return 0;
	}
}

int
event_gotobranch(struct event_state *s, int cid, int branch) {
	/* XXX this might be slow for larger events, might want to move it to 1stpass */
	for(int i = s->line; i < s->ncmd; i++) {
		if(s->cmds[i].id == cid &&
		   s->cmds[i].indent == s->cmds[s->line].indent &&
		   s->cmds[i].narg >= 1 &&
		   s->cmds[i].args[0] == branch) {
			assert(i+1 < s->ncmd);
			s->line = i+1;
			return 1;
		} else {
			assert(s->cmds[i].id != CmdBranchEnd);
		}
	}
	return 0;
}

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
	case CmdVarCond: {
		assert(c->narg >= 1);
		int cid, branch = 0,
		    nbranch = c->args[0] & 0xf,
		    else_ = c->args[0] & (1<<4);

		assert(c->narg >= 1 + (nbranch*3));
		for(int i = 1; i < nbranch+1; i+=3) {
			if(game_compare(g, c->args[i], c->args[i+1], c->args[i+2])) {
				branch = i/3+1;
				break;
			}
		}

		if(branch)
			cid = CmdChoiceCase;
		else if(else_)
			cid = CmdElseCase;
		else
			break;

		if(event_gotobranch(s, cid, branch))
			return EvRunning;

		assert(1 == 2); /* XXX */
		break;
	}
	case CmdChoice: {
		/* TODO: parse escape codes (c->strargs) */
		int r = fchoice(g, c->nstrarg, c->strargs, (c->args[0]&0xff)>>4, c->args[0]>>8);
		g->choicepos = 0;
		int cid = r == 0 ? CmdCancelCase : (r >= 100 ? CmdSpecialChoiceCase : CmdChoiceCase);
		if(event_gotobranch(s, cid, r))
			return EvRunning;

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
