/* this header depends on reader.h and route.h */

typedef struct {
	unsigned char narg;
	int id;
	int *args;
	unsigned char indent;
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

enum command_id /* enumstr: Command id Cmd */ {
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

enum {
	CfCmpGt,
	CfCmpGe,
	CfCmpEq,
	CfCmpLe,
	CfCmpLt,
	CfCmpNe,
	CfCmpBit,
};

enum command_flag {
	CfClamp      = 1<<0,
	CfRealCalc   = 1<<1,
	CfLeftInt    = 1<<2,
	CfRightInt   = 1<<3,
	CfDeref      = 1<<4,
	CfLeftDeref  = 1<<5,
	CfRightDeref = 1<<6,

	CfAsEqu = 0x0000, /* v = op(a, b) */
	CfAsAdd = 0x0100, /* v += op(a, b) */
	CfAsSub = 0x0200, /* v -= op(a, b) */
	CfAsMul = 0x0300, /* v *= op(a, b) */
	CfAsDiv = 0x0400, /* v /= op(a, b) */
	CfAsMod = 0x0500, /* v %= op(a, b) */
	CfAsMax = 0x0600, /* v = max(l, op(a, b)) */
	CfAsMin = 0x0700, /* v = min(l, op(a, b)) */
	CfAsAbs = 0x0800, /* v = abs(op(a, b)) */

	CfOpAdd = 0x0000, /* op(a, b) { return a + b } */
	CfOpSub = 0x1000, /* op(a, b) { return a - b } */
	CfOpMul = 0x2000, /* op(a, b) { return a * b } */
	CfOpDiv = 0x3000, /* op(a, b) { return a / b } */
	CfOpMod = 0x4000, /* op(a, b) { return a % b } */
	CfOpAnd = 0x5000, /* op(a, b) { return a & b } */
	CfOpNot = 0x6000, /* op(a, b) { return a ~ b } */
};

enum {
	TpPrecisePos = 1<<1,
	TpTransSimple = 1<<4,
	TpTransBlackout = 1<<5,
};

void command_free(Command *c);
void command_print(Command *c);
const char *command_idstr(Command *c);
