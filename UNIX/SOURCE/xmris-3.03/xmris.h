/* Copyright (C) 1992 Nathan Sidwell */
#include "patchlevel.h"
#ifndef EXTERN
#define EXTERN extern
#endif /* EXTERN */
/*{{{  ANSI or K&R?*/
#if __STDC__
#  include <stdarg.h>
#  define PROTOARG(list) list
#  define VARARG ...
#  define FUNCARG(list, defs) (defs)
#  define FUNCVARARG(list, defs) (defs, ...)
#  define ARGSEP ,
#  define FUNCARGVOID ()
#  define VARARGSET(args, last) va_start(args, last)
#  define VOIDFUNC void
#  define VOIDRETURN
#  ifdef BROKEN_FPTRS
#    define FUNCANIMATE(name, arg) PROTOVOID(*name(arg)) PROTOVOID(*arg);
#  else
#    define FUNCANIMATE(name, arg) PROTOVOID(*name(PROTOVOID(*arg)))
#  endif /* BROKENFPTRS */
#else /* !__STDC__ */
#  include <varargs.h>
#  define PROTOARG(list) ()
#  define const
#  define volatile
#  define VARARG va_alist
#  define FUNCARG(list, defs) list defs;
#  define FUNCVARARG(list, defs) list defs; va_dcl
#  define ARGSEP  ;
#  define FUNCARGVOID ()
#  define VARARGSET(args, last) va_start(args)
#  define VOIDFUNC int
#  define VOIDRETURN 0
#  define FUNCANIMATE(name, arg) PROTOVOID(*name(arg)) PROTOVOID(*arg);
#  define BROKEN_FPTRS
#endif /* __STDC__ */
/*}}}*/
/*{{{  include*/
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/Intrinsic.h>
/*}}}*/
/*{{{  recursive function defs*/
/* see xmris.c for explanation */
#ifdef BROKEN_FPTRS
#  define PROTOVOID(name) VOIDFUNC (name)()
#  define PROTOANIMATE(name) PROTOVOID(*(name()))
#else
#  define PROTOVOID(name) VOIDFUNC (name) PROTOARG((void))
#  define PROTOANIMATE(name) PROTOVOID(*(name(PROTOVOID(*))))
#endif /* BROKEN_FPTRS */
/*}}}*/
/*{{{  set_euid*/
#ifdef __hpux
#define set_euid(euid) setresuid((uid_t)-1, euid, (uid_t)-1)
#else
#define set_euid(euid) seteuid(euid)
#endif
/*}}}*/
/*{{{  defines*/
/*{{{  board sizes*/
#define CELLS_ACROSS  12  /* size of the board */
#define CELLS_DOWN    13
#define CELL_WIDTH    32  /* size of the internal cell */
#define CELL_HEIGHT   32  /* monster sprites must be this size */
#define GAP_WIDTH     4   /* spacing of the cells */
#define GAP_HEIGHT    4
#define PLAYER_START_X  ((CELLS_ACROSS - 1) >> 1)
#define PLAYER_START_Y  (CELLS_DOWN - 1)
#define DEN_X           ((CELLS_ACROSS - 1) >> 1)
#define DEN_Y           (CELLS_DOWN >> 1)
#define CELL_STRIDE   16  /* board array is bigger than the board */
#define CELL_TOP      2   /* and the board is offset by this much */
#define CELL_LEFT     2
#define KNOCK_THROUGH /* how far we go to knock through a new cell */\
    ((CELL_WIDTH + CELL_HEIGHT + GAP_WIDTH + GAP_HEIGHT) / 3)
#define BORDER_TOP    CELL_HEIGHT /* placing of the board */
#define BORDER_BOTTOM (CELL_HEIGHT + 1) /* on the window */
#define BORDER_LEFT   (-1)
#define BORDER_RIGHT  (-1)
#define EDGING        1   /* edge round the game box */
#define FLOOD_FILL    32  /* how many branches in the distance flood fill */
#define BOARD_WIDTH   ((CELL_WIDTH + GAP_WIDTH) * CELLS_ACROSS + GAP_WIDTH)
#define BOARD_HEIGHT  ((CELL_HEIGHT + GAP_HEIGHT) * CELLS_DOWN + GAP_WIDTH)
#define WINDOW_WIDTH  (BOARD_WIDTH + BORDER_LEFT + BORDER_RIGHT)
#define WINDOW_HEIGHT (BOARD_HEIGHT + BORDER_TOP + BORDER_BOTTOM)
#define XTRA_SPACING  ((CELL_WIDTH * 4 + GAP_WIDTH * 3) / 5)
#define CENTER_X      (BORDER_LEFT + BOARD_WIDTH / 2)
#define CENTER_Y      (BORDER_TOP + BOARD_HEIGHT / 2)
/*}}}*/
/*{{{  frame rate*/
/* these are in uS, if it is too fast, the load line
 * at the bottom will start to grow */
#define FRAME_RATE    (unsigned long)37000 /* interrupt delay */
#define FRAMES_PER_SECOND (1000000 / FRAME_RATE)
#define ZOOM_RATE (FRAME_RATE / 4)
#define SCORE_RATE (FRAME_RATE / 4)
#define NAME_RATE (FRAME_RATE / 2)
#define FRAME_SCALE 1024    /* precision of frame delay scaling */
#define FRAME_RATIO_UPDATE (5 * FRAMES_PER_SECOND) /* frame ratio calc */
#define FRAME_MISS_START  15  /* start dilation calc */
#define FRAME_MISS_STOP   5   /* stop dilation calc */
#define SCORE_SCALE 1024    /* precision of score scaling */
/*}}}*/
/*{{{  font name*/
#ifndef FONT_NAME
#define FONT_NAME "-*-courier-*-r-*-*-18-*-*-*-*-*-*-*"
#endif /* FONT_NAME */
/*}}}*/
/*{{{  key indices*/
#define KEY_THROW     4
#define KEY_PAUSE     5
#define KEY_QUIT      6
#define KEY_ICONIZE   7
#define KEY_KEYBOARD  8
#define KEYS          9
/*}}}*/
/*{{{  score file*/
#ifndef SCORE_DIR
#define SCORE_DIR NULL
#endif /* SCORE_DIR */
#define SCORE_THRESHOLD 10000 /* minimum score for fame */
#define SCORE_ROUND     5     /* scores are rounded */
/*}}}*/
/*{{{  game gender*/
#ifndef GAME_GENDER
#define GAME_GENDER (chaotic() & 1 ? True : False)
#endif /* GAME_GENDER */
/*}}}*/
#define START_LIVES     3
/*{{{  velocities*/
#define VEL_X           GAP_WIDTH             /* how far we move per step */
#define VEL_Y           GAP_HEIGHT
#define VEL_X_FAST      (VEL_X * 5 / 4)       /* how far a fast stride is */
#define VEL_Y_FAST      (VEL_Y * 5 / 4)
#define FAST_STEPS      (CELL_WIDTH / VEL_X / 4) /* 1/2 no of fast strides */
#define APPLE_VEL_Y     (VEL_Y * 3 / 2)       /* how fast the apple falls */
#define APPLE_VEL_X     (VEL_X / 2)           /* how fast it goes sideways */
#define APPLE_ACC       (VEL_Y / 2)
#define BALL_STEPS      2                     /* relative ball speed */
#define BALL_EX         (CELL_WIDTH / 2)      /* how fast we explode */
#define BALL_EY         (CELL_HEIGHT / 2)
#define ZOOM_X          VEL_X                 /* how fast the zoom is */
#define ZOOM_Y          VEL_Y
#define STEP_OFF        1                     /* steps taken for granted */
/*}}}*/
/*{{{  ball stuff*/
#define BALL_EXPLODE    (BOARD_WIDTH / BALL_EX) /* how much we explode */
#define BALL_DISTANCE   6    /* how close to runaway from the ball */
/*}}}*/
#define DIE_DELAY       4  /* ticks for death spiral */
#define SCORE_SHOW      (2 * FRAMES_PER_SECOND) /* frames for board scores */
#define DISPLAY_HOLD    (SCORE_SHOW * 2)
#define SCORE_HOLD      (SCORE_SHOW * 12)
#define HISTORY_SHOW    3   /* how often we show the history */
/*{{{  apple stuff*/
#define APPLE_ROCK_DELAY  8    /* ticks for it to rock */
#define APPLE_SPLIT_DELAY 8    /* ticks for split */
#define APPLE_DECAY_DELAY 8    /* ticks for decay */
#define APPLE_ROT_DELAY   8    /* ticks for rot */
#define APPLE_FALL_SPLIT  (CELL_HEIGHT + GAP_HEIGHT + APPLE_VEL_Y)
    /* safe fall distance */
#define APPLE_GHOST_DELAY (CELL_WIDTH / VEL_X * 2)
/*}}}*/
/*{{{  den escape*/
#define DEN_ESCAPE_PROB   8     /* normal monster gets out of the den */
#define DEN_ESCAPE_DELAY  6     /* how many den flashes before escape */
#define DEN_ESCAPE_FLASH  0x8   /* time per den flash */
/*}}}*/
/*{{{  normal & muncher*/
#define GO_MUNCH_PROB      1    /* difficulty scale that normal goes munchy */
#define DIFFICULTY_PEDESTAL 2   /* offset for difficulty */
#define STOP_TOGGLE_CONT_PEDESTAL  2
#define STOP_TOGGLE_CONT_SCREEN_SCALE  5
#define SET_CONT_PEDESTAL      18    /* continue flag set */
#define CLEAR_CONT_PEDESTAL    24    /* continue flag clear */
#define CLEAR_CONT_SCREEN_SCALE 3
#define CLEAR_CONT_VISIT_SCALE (CELLS_ACROSS * CELLS_DOWN / 8)
#define CLEAR_CONT_MONSTER_SCALE 3
#define PUSH_TURN_PROB    64    /* muncher turns around when pushing */
#define MUNCH_CONT_PEDESTAL     18  /* continue munching */
#define MUNCH_CONT_SCREEN_SCALE 4   /* continue munching scale */
#define GO_MUNCH_DELAY    16    /* pause when we start munching */
#define STOP_MUNCH_DELAY  16    /* pause when we stop munching */
#define PANIC_COUNT       (CELLS_DOWN * (CELL_HEIGHT + GAP_HEIGHT) / VEL_Y)
/*}}}*/
/*{{{  xtra & drone*/
#define XTRA_NEW_DELAY    (FRAMES_PER_SECOND * 4)
#define XTRA_GOT_DELAY    (FRAMES_PER_SECOND * 2)
#define XTRA_BIRTH_DELAY  ((CELL_WIDTH + GAP_WIDTH) / VEL_X + 4)
    /* pause while giving birth to drones */
#define XTRA_CONT_OFF_PROB 16   /* that xtra stops continuing */
#define CHOMP_DELAY     (CELL_WIDTH / VEL_X * 6)
    /* how long to eat an apple */
#define XTRA_HOME_DELAY   (FRAMES_PER_SECOND * 30)
/*}}}*/
#define MONSTER_CYCLES    6     /* cycle counter */
#define SCORE_CYCLES (MONSTER_CYCLES / 2)
#define MONSTER_IMAGES    2     /* how many different images */
#define DIAMOND_IMAGES    3
#define DIAMOND_CYCLES    2
/*{{{  diamond stuff*/
#define DIAMOND_PROB      1     /* chance of apple turning to diamond */
#define DIAMOND_VEL_Y     ((CELL_HEIGHT + GAP_HEIGHT) / 4)
#define DIAMOND_DELAY     (16 * FRAMES_PER_SECOND)
/*}}}*/
/*{{{  limits*/
#define INITIAL_APPLES  6
#define APPLES          (INITIAL_APPLES + 8)    /* max number of apples */
#define MONSTERS        ((CELLS_DOWN - 2) * 2)  /* max number of monsters */
#define BOARD_SCORES    4   /* number of on board displayed scores */
#define BACK_UPDATES    64  /* background updates we can cope with */
/*}}}*/
/*{{{  whizz*/
#define WHIZZ_STEPS     32
#define WHIZZ_CYCLES    2
#define WHIZZ_SIZE      (int)(0xFFFF / (WHIZZ_STEPS - 1))
#define WHIZZ_PROB      64
/*}}}*/
/*{{{  colour flags*/
#define COLOUR_ZERO     0   /* colour is all zeros */
#define COLOUR_ONE      1   /* colour is all ones */
#define COLOUR_WEIRD    2   /* colour is neither zeros or ones */
/*}}}*/
/*{{{  timing states*/
#define TIMING_OFF    0
#define TIMING_ON     1
#define TIMING_PAUSE  2
/*}}}*/
/*}}}*/
/*{{{  macros*/
#define PIXELX(CX, OX) \
    ((CX) * (CELL_WIDTH + GAP_WIDTH) + (OX) + GAP_WIDTH + BORDER_LEFT)
#define PIXELY(CY, OY) \
    ((CY) * (CELL_HEIGHT + GAP_HEIGHT) + (OY) + GAP_HEIGHT + BORDER_TOP)
#define BOARDCELL(CX, CY) \
    (&board[(CY) * CELL_STRIDE + (CX) + CELL_LEFT + CELL_TOP * CELL_STRIDE])
/*
 * The range macro does one test, rather than (straight foward) two,
 * provided l and u are compile time constants, we win. I suppose
 * a decent optimizer may even know about this trick, but gcc doesn't,
 * and that's usually a good indicator of what happens.
 * This works on 2s complement machines, 'cos casting to unsigned
 * _doesn't_ change the bits, just how they're interpreted, AND
 * overflow on unsigneds are _defined_ to wrap round, those on signeds
 * are permitted to turn your computer into a moth. Everything is cast
 * to unsigned, because the promotion rules in this area differ between
 * ANSI and K&R.
 */
#ifndef NOT2SCOMP
#define INRANGE(x, l, u) \
    ((unsigned)(x) - (unsigned)(l) < (unsigned)(u) - (unsigned)(l))
#else /* weirdo */
#define INRANGE(x, l, u) ((x) >= (l) && (x) < (u))
#endif /* NOT2SCOMP */
#define LETTERDIR(l)      ((l) & 3)
#define LETTERDIST(l)     ((l) >> 2)
#define LETTERPATH(d, l)  ((d) | ((l) << 2))
#define LETTEREND         0
#define LETTERPOS(x, y)   (((x) << 4) | (y))
#define LETTERX(l)        ((l) >> 4)
#define LETTERY(l)        ((l) & 0xF)
/*}}}*/
/*{{{  structs*/
/*{{{  typedef struct Data*/
typedef struct Data
{
  Font    font;     /* font to use */
  String  dir;      /* score directory */
  Boolean help;     /* gimme some help */
  Boolean pause;    /* pause when leaving the window */
  Boolean gender;   /* he or she? */
  Boolean swap;     /* swap colours? */
  Boolean mono;     /* force black & white */
  Boolean scores;   /* high scores only */
  Boolean colors;   /* color allocation */
  Boolean sprites;  /* show sprites */
  Boolean private;  /* own colormap */
  int     distinct; /* distinct colors allocated */
  Visual  *visual;  /* visual */
  KeySym keysymbols[KEYS];
} DATA;
/*}}}*/
/*{{{  typedef struct Title*/
typedef struct Title
/* title text */
{
  char const *text;       /* the text */
  unsigned  ix;           /*  key index */
} TITLE;
/*}}}*/
/*{{{  typedef struct Cell*/
typedef struct Cell
/* information about 1 cell on the board */
{
  int     depths[4];      /* depths moved to from cell */
  int     holes[4];       /* apple holes above and below (tl, tr, bl, br) */
  unsigned  distance;     /* distance from player */
  unsigned  xtra;         /* distance from xtra base */
  unsigned  ball;         /* distance from ball */
  unsigned  visit;        /* has been visited */
  unsigned  apple;        /* apple temp flag */
  unsigned  sprite;       /* has a sprite in it (cherry, or center stuff) */
} CELL;
/*}}}*/
/*{{{  typedef struct Coord*/
typedef struct Coord
/* general coordinate store */
{
  int       x;
  int       y;
} COORD;
/*}}}*/
/*{{{  typedef struct Size*/
typedef struct Size
/* general size store */
{
  unsigned  x;
  unsigned  y;
} SIZE;
/*}}}*/
/*{{{  typedef struct Sprite*/
typedef struct Sprite
/* sprite definition */
{
  Pixmap    image;            /* image pixmap */
  Pixmap    mask;             /* mask pixmap */
  SIZE      size;             /* size of sprite */
} SPRITE;
/*}}}*/
/*{{{  typedef struct Board*/
typedef struct Board
/* initial board information */
{
  unsigned  fill;   /* fill pattern */
  unsigned  colors[2]; /* the two colours to use */
  char const map[CELLS_DOWN][CELLS_ACROSS + 1];
} BOARD;
/*}}}*/
/*{{{  typedef struct Background*/
typedef struct Background
/* area to update from back to copy to window */
{
  COORD   place;  /* top left area */
  SIZE    size;   /* size of area */
} BACKGROUND;
/*}}}*/
/*{{{  typedef struct Score*/
typedef struct Score
/* on board score display */
{
  Pixmap    mask;   /* the mask to use */
  Pixmap    image;  /* the image to display */
  COORD     place;  /* where to bung it */
  unsigned  count;  /* removal countdown */
} SCORE;
/*}}}*/
/*{{{  typedef struct Ball*/
typedef struct Ball
/* the ball state */
{
  COORD     cell;
  COORD     offset;
  COORD     pixel;
  unsigned  state;  /* state of the ball
		     * 0 held by player
		     * 1 bouncing
		     * 2 exploding
		     * 3 exploded
		     * 4 imploding
		     */
  unsigned  count;  /* count or direction or player type */
  unsigned  image;  /* player image */
} BALL;
/*}}}*/
/*{{{  typedef struct Apple*/
typedef struct Apple
/* non-background apple information */
{
  COORD     cell;       /* apple's cell */
  COORD     offset;     /* offset from center */
  COORD     pixel;      /* pixel for sprite */
  int       push;       /* horizontal push */
  unsigned  count;      /* general counter */
  unsigned  state;      /* state
			 * 0 stationary
			 * 1 rock
			 * 2 falling
			 * 3 split
			 * 4 decay
			 * 5 rot
			 * 6 delete
			 */
  unsigned  ghost;      /* ghost count */
  unsigned  distance;   /* distance we've fallen */
  unsigned  monsters;   /* monsters we've squashed */
  unsigned  chewed;     /* chewed */
  int       maypush;    /* temp push vector */
  struct Monster *list; /* list of monsters for initial fall */
  COORD     old_pixel;  /* where i was */
  unsigned  old_state;  /* what I looked like */
  unsigned  back;       /* background set */
} APPLE;
/*}}}*/
/*{{{  typedef struct Apple_Size*/
typedef struct Apple_Size
{
  SIZE    size;
  COORD   offset;
} APPLE_SIZE;
/*}}}*/
/*{{{  typedef struct Monster*/
typedef struct Monster
/* monster information */
{
  COORD     cell;     /* board cell we're related to */
  COORD     offset;   /* offset from this cell */
  COORD     pixel;    /* pixel coordinate */
  unsigned  dir;      /* direction we're moving in */
  unsigned  pause;    /* we're paused for one */
  unsigned  stop;     /* we're stopped (player only) */
  unsigned  type;     /* type
		       * 0 - normal
		       * 1 - munch
		       * 2 - xtra
		       * 3 - drone
		       * 4 - player
		       * 5 - delete
		       *>5 - demo sprite
		       */
  unsigned  face;     /* direction we're facing
		       * 0 up left,
		       * 1 down right
		       * 2 left
		       * 3 right
		       * 4 up right
		       * 5 down left
		       * 6 push left
		       * 7 push right
		       * 8 squish left
		       * 9 squish right
		       */
  int       push;     /* being pushed in this direction */
  unsigned  gomunch;  /* change munch state */
  unsigned  cont;     /* continue */
  unsigned  chew;     /* chewing */
  unsigned  count;    /* counter */
  unsigned  panic;    /* help I'm about to be squashed */
  unsigned  shot;     /* has been shot */
  unsigned  cycle;    /* image cycler */
  unsigned  image;    /* which image to display */
  unsigned  fast;     /* fast speed */
  unsigned  pushing;  /* pushing apple */
  unsigned  squished; /* under an apple */
  struct Monster *list; /* list of monsters for initial apple fall */
  struct Monster *tptr; /* temporay pointer */
  COORD     old_pixel;  /* where we were */
  int       old_sprite; /* what we looked like */
  unsigned  back;       /* background set */
  unsigned  on;         /* visible */
} MONSTER;
/*}}}*/
/*}}}*/
/*{{{  display*/
EXTERN struct
{
  XtAppContext  context;  /* my context */
  Widget    toplevel;   /* toplevel widget */
  Display   *display;   /* server name stuff */
  Colormap  colormap;   /* colormap */
  int       screen;     /* screen */
  Window    window;     /* game window */
  unsigned  long black; /* black pixel index */
  unsigned  long white; /* white pixel index */
  unsigned  long xor;   /* black xor white */
  Atom      DEC_icon_atom; /* DEC iconizing hack */
  Pixmap    back;       /* background store */
  Pixmap    copy;       /* backing store */
  unsigned  background; /* type of background colour */
  unsigned  foreground; /* type of foreground colour */
  unsigned  dynamic;    /* dynamic colors */
} display;
/*}}}*/
/*{{{  font*/
EXTERN struct
{
  unsigned  width;    /* width of a character */
  int       ascent;   /* max ascent */
  int       descent;  /* max descent */
  int       center;   /* center offset to add */
} font;
/*}}}*/
EXTERN CELL board[(CELLS_DOWN + CELL_TOP * 2) * CELL_STRIDE];
/*{{{  gc*/
#define GCN(n)    gcs[n]
#define GC_COPY   0   /* src */
#define GC_CLEAR  1   /* background */
#define GC_SET    2   /* foreground */
#define GC_MASK   3   /* NOT src AND dst */
#define GC_OR     4   /* src OR dst */
#define GC_TEXT   5   /* text drawer */
#define GC_AND    6   /* and */
#define GC_BOARD  7   /* board background */
#define GC_LOAD   8   /* load line draw */
#define GCS       9
EXTERN GC gcs[GCS];
/*}}}*/
EXTERN DATA data;
/*{{{  player*/
EXTERN struct
/* player specific information
 * note, the player place info is stored as monster 0
 */
{
  unsigned  next;       /* direction at next intersection */
  unsigned  bashed;     /* we bashed into a wall */
  BALL      old_ball;   /* what was the ball */
  BALL      ball;       /* ball information */
  unsigned  count;      /* ball return dec count */
  unsigned  thrown;     /* how many times its been thrown */
  unsigned  cherry;     /* consecutive cherry count */
  unsigned  distance;   /* distance to next cherry */
  unsigned  pressed;    /* keys we had pressed */
} player;
/*}}}*/
EXTERN unsigned long seed;      /* chaotic number seed */
EXTERN uid_t real_uid;          /* who I really am */
EXTERN uid_t effective_uid;     /* who I'm pretending to be */
/*{{{  global*/
EXTERN struct
{
  unsigned  screen;         /* current screen number */
  unsigned long score;      /* score */
  unsigned  lives;          /* lives left (including on screen) */
  unsigned long msec;       /* number of milliseconds this screen */
  unsigned  dilation;       /* frame delay dilation factor */
  unsigned  scale;          /* score scale factor */
  unsigned  difficulty;     /* increments in to make it harder */
  unsigned  broken;         /* broken through a new path */
  unsigned  stepped;        /* player moved to new cell */
  unsigned  cherries;       /* number of cherries left */
  unsigned  state;          /* den state
			     * 0 - den on screen
			     * 1 - cake on screen
			     * 2 - xtras & drone running around
			     * 3 - done xtras
			     * 4 - end game
			     * 5 - mid game demo
			     * 6 - demo
			     * 7 - defining keys
			     * 8 - zooming
			     */
#define MODE_GAME_DEMO 5
#define MODE_DEMO      6
#define MODE_KEY_DEF   7
#define MODE_ZOOM      8
  unsigned  whizz;          /* whizzing */
  unsigned  diamond;        /* diamond state */
  unsigned  missed;         /* missed interrupt count */
  unsigned  count;          /* general counter */
  unsigned  visited;        /* amount of cells visited */
  unsigned  quit;           /* quit selected */
  unsigned  pause;          /* game paused */
  unsigned  iconic;         /* game iconized */
  unsigned  pressed;        /* keys pressed */
  unsigned  throw;          /* throw key state */
  KeySym    key;            /* last key pressed */
} global;
/*}}}*/
/*{{{  extra*/
EXTERN struct
{
  unsigned  got;      /* one we've got */
  unsigned  select;   /* the one which is selected */
  unsigned  escape;   /* its out/homesick */
  unsigned  score;    /* last checked score */
  unsigned  count;    /* change countdown */
} extra;
/*}}}*/
/*{{{  apple*/
EXTERN struct
{
  APPLE     list[APPLES]; /* apple list */
  unsigned  apples;       /* number of apples out */
} apple;
/*}}}*/
/*{{{  history*/
EXTERN struct
{
  unsigned  prize;        /* did we get the prize? */
  unsigned  ending;       /* how did the screen end? */
  unsigned long msec;     /* how many milliseconds have been played? */
  unsigned long times[CELLS_DOWN];  /* how many milliseconds per screen? */
} history;
/*}}}*/
/*{{{  monster*/
EXTERN struct
{
  MONSTER   list[MONSTERS]; /* monsters [0] is player */
  CELL      *player;        /* where the player is */
  unsigned  monsters;       /* number of monsters out (inc player) */
  unsigned  delay;          /* escape delay */
  unsigned  den;            /* monster spawn count */
  unsigned  normals;        /* normal monsters alive */
  unsigned  drones;         /* drones out */
  unsigned  nearest;        /* what was the nearest distance to player */
  unsigned  farthest;       /* what was the farthest distance to player */
} monster;
/*}}}*/
/*{{{  update*/
EXTERN struct
{
  COORD     tl;       /* top left */
  COORD     br;       /* bottom right */
  unsigned  set;      /* tl & br set */
  struct
  {
    unsigned  backs; /* number of areas to update to window */
    BACKGROUND list[BACK_UPDATES]; /* the area information */
  } back;
  struct
  {
    unsigned  scores;  /* number of displayed scores */
    SCORE     list[BOARD_SCORES];   /* the displayed scores */
  } score;
} update;
/*}}}*/
/*{{{  tables*/
/*{{{  sprite numbers*/
#define SPRITE_CENTER_BASE     0
#define SPRITE_EDGE_BASE       2
#define SPRITE_DIGITS          4
#define SPRITE_DEN             5
#define SPRITE_BALL            6
#define SPRITE_CHERRY          7
#define SPRITE_APPLE           8
#define SPRITE_GHOST          14
#define SPRITE_DIAMOND        17
#define SPRITE_EXTRA          20
#define SPRITE_XTRA_SOURCE    22
#define SPRITE_MONSTERS       24
#define SPRITE_NORMAL         24
#define SPRITE_MUNCHER        36
#define SPRITE_XTRA           48
#define SPRITE_DRONE          60
#define SPRITE_PLAYER         72
#define SPRITE_PLAYER_PUSH    84
#define SPRITE_PLAYER_REST    88
#define SPRITE_PLAYER_HAPPY   92
#define SPRITE_PLAYER_DEAD    94
#define SPRITE_SQUISHED       110
#define SPRITE_CHOMP          120
#define SPRITE_MRIS           122
#define SPRITE_SEAT           130
#define SPRITE_PRIZE_BASE     131
#define SPRITES               136
/*}}}*/
#define SPRITE_PRIZES          5
#define SPRITE_MUSHROOM        3
/*{{{  chaotic sizes*/
#define MUNCH_WIDTH     (VEL_X * 4)
#define MUNCH_HEIGHT    (VEL_Y * 4)
#define EDGE_WIDTH      (CELL_WIDTH + GAP_WIDTH * 2)
#define EDGE_HEIGHT     (CELL_HEIGHT + GAP_HEIGHT * 2)
#define DIGIT_HEIGHT    (CELL_HEIGHT / 2)
#define DIGIT_WIDTH     (CELL_WIDTH / 4)
#define BALL_WIDTH      7     /* size of ball */
#define BALL_HEIGHT     7
#define INTERNAL_WIDTH  4     /* size of internal corner */
#define INTERNAL_HEIGHT 4
#define EXTERNAL_WIDTH  4     /* size of external corner */
#define EXTERNAL_HEIGHT 4
#define XTRA_LETTER_X   8     /* position of lettering on xtra */
#define XTRA_LETTER_Y   10
/*}}}*/
EXTERN SPRITE sprites[SPRITES];
#define BOARD_FILLS           4
EXTERN SPRITE fills[BOARD_FILLS];
extern APPLE_SIZE const apple_sizes[6];
#define BOARDS 10
extern BOARD const boards[BOARDS];
extern COORD ball_hold[16];
extern COORD const ball_throw[8];
#define BALL_RETURNS 3
extern unsigned const ball_returns[BALL_RETURNS];
extern unsigned const ball_dir[8];
extern unsigned const player_dies[8];
extern TITLE const title_text[];
extern char const hex_text[];
EXTERN XColor *colors;
EXTERN unsigned backgrounds[2];
#define SQUISH_SCORES 7
extern unsigned const squish_scores[SQUISH_SCORES];
extern unsigned char const *lettering[];
/*}}}*/
/*{{{  prototypes*/
/*{{{  apple*/
extern int apple_stop PROTOARG((MONSTER *, CELL *));
extern VOIDFUNC move_apples PROTOARG((void));
extern VOIDFUNC panic_monsters PROTOARG((int, int, CELL *));
extern APPLE *spawn_apple PROTOARG((int, int, int, int));
/*}}}*/
/*{{{  create*/
extern VOIDFUNC close_toolkit PROTOARG((void));
extern VOIDFUNC create_widget PROTOARG((void));
extern VOIDFUNC destroy_widget PROTOARG((void));
extern VOIDFUNC list_help PROTOARG((char const *));
extern VOIDFUNC open_toolkit PROTOARG((int, char **));
extern VOIDFUNC set_colors PROTOARG((int));
/*}}}*/
/*{{{  demo*/
extern PROTOANIMATE(animate_demo);
extern PROTOANIMATE(animate_diamond);
extern PROTOANIMATE(animate_extra_life);
extern PROTOANIMATE(animate_history);
extern VOIDFUNC high_score PROTOARG((unsigned long, unsigned, unsigned long));
extern VOIDFUNC init_scores PROTOARG((void));
extern VOIDFUNC list_scores PROTOARG((void));
/*}}}*/
/*{{{  draw*/
extern VOIDFUNC add_background PROTOARG((int, int, unsigned, unsigned));
extern PROTOANIMATE(animate_zoom);
extern VOIDFUNC blank_board PROTOARG((BOARD const *));
extern VOIDFUNC bounding_box PROTOARG((int, int, unsigned, unsigned));
extern VOIDFUNC create_xtra_monster PROTOARG((unsigned));
extern VOIDFUNC draw_center PROTOARG((unsigned));
extern VOIDFUNC draw_extra PROTOARG((void));
extern VOIDFUNC draw_extra_letter PROTOARG((unsigned));
extern VOIDFUNC new_board PROTOARG((void));
extern VOIDFUNC refresh_window PROTOARG((void));
extern VOIDFUNC show_updates PROTOARG((void));
/*}}}*/
/*{{{  monster*/
extern VOIDFUNC extra_dies PROTOARG((void));
extern MONSTER *extra_escape PROTOARG((void));
extern VOIDFUNC move_monsters PROTOARG((void));
extern VOIDFUNC new_xtra PROTOARG((void));
extern MONSTER *spawn_monster
    PROTOARG((unsigned, unsigned, unsigned, int, int, int, int));
/*}}}*/
/*{{{  move*/
extern VOIDFUNC back_sprite PROTOARG((unsigned, unsigned, int, int));
extern unsigned choose_direction PROTOARG((unsigned));
extern CELL *drop_apple PROTOARG((APPLE *, CELL *));
extern CELL *move_diamond PROTOARG((MONSTER *, CELL *));
extern CELL *move_movable PROTOARG((MONSTER *, CELL *));
extern CELL *move_muncher PROTOARG((MONSTER *));
extern VOIDFUNC munch_hole PROTOARG((CELL *, int, int));
extern VOIDFUNC new_face PROTOARG((MONSTER *));
extern unsigned valid_directions PROTOARG((MONSTER *, CELL *));
/*}}}*/
/*{{{  player*/
extern VOIDFUNC bounce_ball PROTOARG((void));
extern VOIDFUNC killed_player PROTOARG((void));
extern VOIDFUNC move_player PROTOARG((void));
/*}}}*/
/*{{{  timer*/
extern VOIDFUNC timer_close PROTOARG((void));
extern VOIDFUNC timer_open PROTOARG((void));
extern unsigned timer_set PROTOARG((unsigned long, unsigned));
extern VOIDFUNC timer_wait PROTOARG((void));
/*}}}*/
/*{{{  xmris*/
extern PROTOANIMATE(animate_game);
extern VOIDFUNC add_score PROTOARG((unsigned, int, int));
extern VOIDFUNC calc_distances PROTOARG((void));
extern VOIDFUNC fatal_error PROTOARG((char const *, ...));
extern size_t itoa PROTOARG((char *, unsigned long, unsigned));
extern int main PROTOARG((int, char **));
extern unsigned chaotic PROTOARG((void));
/*}}}*/
/*}}}*/
