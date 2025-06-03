/* Copyright (C) 1992 Nathan Sidwell */
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Xaw/Paned.h>
/*{{{  Xtoolkit has got XtRKeySym by now?*/
#ifndef XtRKeySym
#define XtRKeySym "KeySym"
#endif /* XtRKeySym */
/*}}}*/
#define XtRGender       "Gender"
#define COLOR_PLANES 3  /* max color planes for sprite generation */
/*{{{  some macros*/
#define RGB2X(r, g, b)  (int)(((long)(b) - (long)(r)) * (56756 / 2) / 65536)
#define RGB2Y(r, g, b)  (int)(((long)(g) - ((long)(r) + (long)(b)) / 2) / 2)
#define RGB2H(r, g, b)  (int)(((long)(g) * 4 + (long)(r) * 3 + (long)(b)) / 8)
/*}}}*/
/*{{{  structs*/
/*{{{  typedef struct Help*/
typedef struct Help
/* command line argument specifier */
{
  char const *help;     /* help message */
  char const *arg;      /* arg text */
  char const *resource; /* resource name */
  char const *option;   /* option type */
} HELP;
/*}}}*/
/*{{{  typedef struct Context*/
typedef struct Context
/* GC definer */
{
  int       function;         /* graphics function */
  unsigned long const *fgp;   /* foreground color pointer */
  unsigned long const *bgp;   /* background color pointer */
} CONTEXT;
/*}}}*/
/*{{{  typedef struct Sprite_Def*/
typedef struct Sprite_Def
/* sprite definition */
{
  unsigned char *bitmap;  /* image bitmap */
  unsigned  planes;       /* number of planes in bitmap */
  unsigned long *colors[2];    /* colors arrays */
  SIZE      size;         /* size of sprite */
  SIZE      expected;     /* expected size */
  unsigned  copy;         /* generated from this sprite */
  unsigned  flags;        /* reflection mode, or border bits */
} SPRITE_DEF;
#define REFLECT_ALIAS       0 /* alias */
#define REFLECT_VERTICAL    1 /* vertical axis reflect */
#define REFLECT_HORIZONTAL  2 /* horizontal axis reflect */
#define REFLECT_DIAGONAL    3 /* 45' axis reflect */
#define BORDER_HALO_MASK         1  /* halo */
#define BORDER_OUTER_EDGE_MASK   2 /* -swap outer white */
/*}}}*/
/*{{{  typedef struct Color_Def*/
typedef struct Color_Def
{
  char const *name;       /* my name for the color */
  unsigned  type;         /* type of color
			   * bit0-3, mris, mris.swap, msit, msit.swap
			   * bit4 - background
			   * bit5 - dynamic
			   */
  char const *source[4];  /* possible names for the color */
  int       coord[3];     /* position in colorspace */
  struct Color_Def  *nearest;    /* nearest allocated color */
  unsigned long distance; /* distance to nearest */
  unsigned alloc;         /* 
			   * 0 - unallocated
			   * 1 - allocated
			   * 2 - copy
			   * 3 - unrequired
			   * 4 - info
			   */
} COLOR_DEF;
/*}}}*/
/*{{{  typedef struct Visual_Class*/
typedef struct Visual_Class
{
  char const *name;       /* class name */
  int       class;        /* visual class */
} VISUAL_CLASS;
/*}}}*/
/*}}}*/
extern unsigned long color_zero;
extern unsigned long color_one;
extern Boolean  default_gender;
/*{{{  create tables*/
extern XrmOptionDescRec options[];
extern XtResource resources[];
extern COLOR_DEF color_names[];
/*{{{  color numbers*/
#define COLOR_WHITE          2
#define COLOR_BLACK          3
#define COLOR_GREEN_BACK     4
#define COLOR_GREEN_FORE     5
#define COLOR_RED_BACK       6
#define COLOR_RED_FORE       7
#define COLOR_BLUE_BACK      8
#define COLOR_BLUE_FORE      9
#define COLOR_DRONE_BACK    10
#define COLOR_DRONE_FORE    11
#define COLOR_BACKGROUND    12
#define COLOR_FOREGROUND    13
#define COLOR_BALL          14
#define COLOR_CHERRY        15
#define COLOR_CHERRY_STALK  16
#define COLOR_APPLE_1       17
#define COLOR_APPLE_2       18
#define COLOR_APPLE_FAINT   19
#define COLOR_GEM_1         20
#define COLOR_GEM_2         21
#define COLOR_LETTER_GOT    22
#define COLOR_LETTER_NOT_GOT 23
#define COLOR_NORMAL        24
#define COLOR_MUNCH_1       25
#define COLOR_MUNCH_2       26
#define COLOR_DRONE         27
#define COLOR_DRONE_TONGUE  28
#define COLOR_XTRA          29
#define COLOR_PLAYER        30
#define COLOR_PLAYER_BOBBLE 31
#define COLOR_PLAYER_FACE   32
#define COLOR_PLAYER_BOOT   33
#define COLOR_CHOMP         34
#define COLOR_CHOMP_LIP     35
#define COLOR_CHOMP_TONGUE  36
#define COLOR_SEAT          37
#define COLOR_CAKE          38
#define COLOR_CAKE_ICING    39
#define COLOR_CAKE_JAM      40
#define COLOR_SPANNER       41
#define COLOR_SPANNER_SHADOW 42
#define COLOR_BROLLY_1      43
#define COLOR_BROLLY_2      44
#define COLOR_BROLLY_HANDLE 45
#define COLOR_MUSHROOM_STALK 46
#define COLOR_MUSHROOM_CAP  47
#define COLOR_CLOCK_FACE    48
#define COLOR_CLOCK_BELL    49
#define COLOR_CLOCK_EDGE    50
#define COLORS              51
/*}}}*/
extern CONTEXT gcsdefine[GCS];
extern HELP const help[];
extern SPRITE_DEF const she_nadger[];
extern SPRITE_DEF const icons[2];
extern COORD const she_hold[16];
extern SPRITE_DEF sprites_def[SPRITES];
extern SPRITE_DEF fills_def[BOARD_FILLS];
extern unsigned char letter_mris[];
extern unsigned char letter_msit[];
extern unsigned char letter_the[];
extern unsigned char letter_game[];
extern VISUAL_CLASS const visual_class[];
/*}}}*/
