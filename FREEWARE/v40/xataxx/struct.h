#define EMPTY 0
#define WHITE 1
#define BLACK 2
#define OBSTACLE 3

#define TOKS " \t,\n"
#define MIN_BOARD_SIZE 1
#define MAX_BOARD_SIZE 1024

#define X_PIECE_SIZE 32
#define Y_PIECE_SIZE 32

#define TEXT_SIZE 50

#ifndef DATADIR
#ifndef VMS
#define DATADIR "/usr/games/lib/xataxx"
#else
#define DATADIR "XATAXX_DIR:"
#endif
#endif

#define BLACKSTONE perfection ? "dksmiley.xbm":"dkstone.xbm"
#define WHITESTONE perfection ? "ltsmiley.xbm":"ltstone.xbm"
#define BLACKBLOCK "dkblock.xbm"
#define WHITEBLOCK "ltblock.xbm"
#define TITLE	   "title.xbm"
#define UPCURSOR   perfection ? "smilecurs.xbm":"upcursor.xbm"
#define DOWNCURSOR perfection ? "scowlcurs.xbm":"downcursor.xbm"
#define UPMASK	   perfection ? "smilecursmask.xbm":"upmask.xbm"
#define DOWNMASK   perfection ? "scowlcursmask.xbm":"downmask.xbm"

#define BOARDEXT   ".xbd"
#define BOARDNUMFILE  "boards.num"

#ifndef True
#define True 1
#define False 0
#endif



