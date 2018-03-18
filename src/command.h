/* this header depends on reader.h and route.h */

typedef struct {
	unsigned char narg;
	int id;
	unsigned char *args;
	unsigned char nstrarg;
	unsigned char **strargs;

	/* FIXME: move out or make optional, it's not used in most commands */
	struct {
		unsigned char something[5];
		unsigned char flags;
		int nroute;
		Route *routes;
	} movedata;
} Command;

enum command_id /* enumstr: Command id */ {
	CmdNop          = 0,
	CmdCheckpoint   = 99,
	CmdMsg          = 101,
	CmdChoice       = 102,
	CmdComment      = 103,
	CmdForceStopMsg = 105,
	CmdDebugMsg     = 106,
	CmdClearDebugText = 107,
	CmdVarCond      = 111,
	CmdStrCond      = 112,
	CmdSetVar       = 121,
	CmdSetStr       = 122,
	CmdInputKey     = 123,
	CmdSetVar2      = 124,
	CmdAutoInput    = 125,
	CmdBanInput     = 126,
	CmdTeleport     = 130,
	CmdSound        = 140,
	CmdPicture      = 150,
	CmdChangeColor  = 151,
	CmdSetTrans     = 160,
	CmdPrepTrans    = 161,
	CmdExecTrans    = 162,
	CmdStartLoop    = 170,
	CmdBreakLoop    = 171,
	CmdBreakEvent   = 172,
	CmdEraseEvent   = 173,
	CmdGoToTitle    = 174,
	CmdEndGame      = 175,
	CmdStartLoop2   = 176,
	CmdStopNonPic   = 177,
	CmdResumeNonPic = 178,
	CmdLoopTimes    = 179,
	CmdWait         = 180,
	CmdMove         = 201,
	CmdWaitForMove  = 202,
	CmdCommonEvent  = 210,
	CmdCommonEventReserve = 211,
	CmdSetLabel     = 212,
	CmdJumpLabel    = 213,
	CmdSaveLoad     = 220,
	CmdLoadGame     = 221,
	CmdSaveGame     = 222,
	CmdEventMoveOn  = 230,
	CmdEventMoveOff = 231,
	CmdChip         = 240,
	CmdChipSet      = 241,
	CmdDatabase     = 250,
	CmdImportDatabase = 251,
	CmdParty        = 270,
	CmdMapEffect    = 280,
	CmdScrollScreen = 281,
	CmdEffect       = 290,
	CmdCommonEventByName = 300,
	CmdChoiceCase   = 401,
	CmdSpecialChoiceCase = 402,
	CmdElseCase     = 420,
	CmdCancelCase   = 421,
	CmdLoopEnd      = 498,
	CmdBranchEnd    = 499,
};
	
void command_load(Reader *r, Command *c);
void command_free(Command *c);
void command_print(Command *c);
