/* Copyright (C) 1992 Nathan Sidwell */
/*{{{  file locking problems*/
/*
 * NOLOCKF file locking as suggested by
 * Daniel Edward Lovinger <del+@cmu.edu>
 * With lockf, we just use the kernel's locking stuff to lock the
 * entire score file while we read, or update it. But some distributed
 * file systems don't support it. I've also discovered that, for some
 * unknown reason, my lock deamon rpc.locd grows its heap unendingly,
 * for each lock-unlock pair. Its not fun when your lockd heap takes up
 * 10MB! This appears to be a bug in SunOS 4.1 its gone in 4.1.1.
 * Rather than say, that's not my fault, (which it isn't), NOLOCKF uses
 * a fallback locking scheme, which uses open(O_CREAT | O_EXCL) to create
 * a lock file in the same directory as the xmris high score file, with
 * the name "xmris.lock".
 * The personal score files are either in the score file directory
 * with names "xmris-<name>", or in the user's home directory with name
 * ".xmris.scores".
 * In order to work correctly if xmris is set_euid'd to get the access
 * permissions to the high score directory, we keep juggling
 * the effective user id between the set_euid'd one and the real uid.
 * This ensures that xmris can open the display on servers which use
 * magic cookie and access control (like openwindows), and that the
 * personal file has the correct attributes when created in the user's
 * home directory.
 */
/*}}}*/
#include "xmris.h"
/*{{{  other includes*/
#include <pwd.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#ifdef NOLOCKF
#include <fcntl.h>
#endif /* NOLOCKF */
/*}}}*/
#define HIGH_SCORES (CELLS_DOWN - 2)
#define NAME_LEN  64
/*{{{  typedef struct High_Score*/
typedef struct High_Score
{
  time_t    stamp;          /* integrity stamp */
  unsigned  marker;         /* flag */
  unsigned long score;      /* score */
  unsigned  screen;         /* final screen */
  unsigned  elapsed;        /* playing time seconds */
  char name[NAME_LEN + 1];  /* name of player */
} HIGH_SCORE;
/*}}}*/
/*{{{  static*/
static HIGH_SCORE high_scores[HIGH_SCORES];
static HIGH_SCORE today_scores[HIGH_SCORES];
static HIGH_SCORE personal_scores[HIGH_SCORES];
static HIGH_SCORE my_score;
static HIGH_SCORE *display_scores;
static char     *score_file = NULL;
static char     *personal_file = NULL;
static char     *personal_home = NULL;
static int      personal_make = 1;
static uid_t    personal_uid = -1;
#ifdef NOLOCKF
static char     *lock_file = NULL;
#endif /* NOLOCKF */
static char const *table_names[2] = {"Roll of honour", "Today's contenders"};
/*}}}*/
/*{{{  prototypes*/
static PROTOANIMATE(animate_board);
static PROTOANIMATE(animate_def_keys);
static PROTOANIMATE(animate_keys);
static PROTOANIMATE(animate_name);
static PROTOANIMATE(animate_score);
static PROTOANIMATE(animate_sprites);
static VOIDFUNC back_mris PROTOARG((unsigned));
static VOIDFUNC back_score PROTOARG((HIGH_SCORE const *, unsigned));
static int back_title PROTOARG((TITLE const *, unsigned, unsigned));
static unsigned expire PROTOARG((time_t, time_t));
static unsigned file_changed PROTOARG((char const *, uid_t));
static FILE *get_lock PROTOARG((char const *, int, uid_t));
static VOIDFUNC get_unlock PROTOARG((FILE *));
static unsigned insert_personal PROTOARG((HIGH_SCORE const *));
static unsigned insert_score
    PROTOARG((HIGH_SCORE *, HIGH_SCORE const *, unsigned));
static unsigned merge_personal PROTOARG((FILE *));
static unsigned merge_scores PROTOARG((FILE *));
static int move_demo PROTOARG((unsigned));
static VOIDFUNC move_mris PROTOARG((void));
static unsigned long read_line
    PROTOARG((FILE *, HIGH_SCORE *, unsigned long));
static VOIDFUNC remove_scores PROTOARG((HIGH_SCORE *, unsigned));
static VOIDFUNC retire_scores PROTOARG((void));
static VOIDFUNC write_personal PROTOARG((FILE *));
static VOIDFUNC write_scores PROTOARG((FILE *));
static unsigned long write_table
    PROTOARG((FILE *, HIGH_SCORE *, unsigned long));
/*}}}*/
/*{{{  ANIMATE animate_board(next)*/
static FUNCANIMATE(animate_board, next)
/*
 * animates a demonstration board. Puts on the M R I & S letters
 * and moves them about a bit, before returning from whence I came.
 */
{
  static PROTOANIMATE((*rts));
  static int state;
  PROTOVOID(*then);
  
  then = (PROTOVOID(*))animate_board;
  if(next)
    /*{{{  start*/
    {
      rts = (PROTOANIMATE((*)))next;
      state = 0;
      global.screen = chaotic() % 10 + 1;
      extra.select = chaotic() % 5;
      extra.got = chaotic() & 0x1F;
      create_xtra_monster(extra.select);
      new_board();
      then = animate_zoom((void (*) PROTOARG((void)))animate_board);
    }
    /*}}}*/
  else if(!global.count || global.pause || global.throw == 1 ||
      global.pressed & (1 << KEY_QUIT | 1 << KEY_KEYBOARD))
    /*{{{  end*/
    {
      timer_set((unsigned long)0, TIMING_OFF);
      assert(rts != (PROTOANIMATE((*)))NULL);
      then = (*rts)((PROTOVOID(*))NULL);
    }
    /*}}}*/
  else if(state == 0)
    /*{{{  start again*/
    {
      global.state = MODE_DEMO;
      draw_center(SPRITE_DEN);
      spawn_monster(4, 3, 3, PLAYER_START_X, PLAYER_START_Y, 0, 0);
      extra.count = FRAMES_PER_SECOND;
      monster.list[0].stop = 1;
      monster.den = 0xF;
      monster.drones = 5;
      monster.normals = 4;
      monster.delay = 0;
      player.old_ball.count = 16;
      global.diamond = 0;
      bounce_ball();
      /*{{{  plonk on M R I S*/
      {
	unsigned  ix;
      
	for(ix = 4; ix--;)
	  {
	    unsigned  x, y;
	    unsigned  j;
	    CELL      *cptr;
	    
	    do
	      {
		do
		  j = chaotic();
		while(j >= CELLS_ACROSS * CELLS_DOWN);
		x = j % CELLS_ACROSS;
		y = j / CELLS_ACROSS;
		cptr = BOARDCELL(x, y);
	      }
	    while(!cptr->visit);
	    spawn_monster(SPRITE_MRIS + ix + (chaotic() & 4), 0, 0,
		(int)x, (int)y, 0, 0);
	  }
      }
      /*}}}*/
      global.count = DISPLAY_HOLD;
      refresh_window();
      timer_set(FRAME_RATE, TIMING_ON);
      state = 1;
    }
    /*}}}*/
  else
    /*{{{  animate*/
    {
      /*{{{  calc distances?*/
      if(monster.den && monster.normals != monster.drones)
	{
	  unsigned  ix;
	  
	  monster.drones = monster.normals;
	  ix = choose_direction(monster.den);
	  monster.den ^= 1 << ix;
	  monster.list[0].cell.y = 0;
	  monster.list[0].cell.x = 4 + ix;
	  calc_distances();
	  monster.list[0].cell.y = PLAYER_START_Y;
	  monster.list[0].cell.x = PLAYER_START_X;
	}
      /*}}}*/
      if(!extra.count--)
	new_xtra();
      if(player.ball.state)
	bounce_ball();
      else if(!monster.delay && chaotic() < 4)
	{
	  global.throw = 1;
	  bounce_ball();
	  global.throw = 0;
	}
      /*{{{  show a diamond?*/
      if(!global.diamond && !monster.delay && !chaotic())
	{
	  unsigned  j;
	  int       x, y;
	  CELL      *cptr;
		
	  do
	    {
	      do
		j = chaotic();
	      while(j >= CELLS_DOWN * CELLS_ACROSS ||
		  j == PLAYER_START_X + PLAYER_START_Y * CELLS_ACROSS ||
		  j == DEN_X + DEN_Y * CELLS_ACROSS);
	      x = j % CELLS_ACROSS;
	      y = j / CELLS_ACROSS;
	      cptr = BOARDCELL(x, y);
	    }
	  while(!cptr->visit || cptr[CELL_STRIDE].visit);
	  global.diamond = 1;
	  spawn_monster(6, 0, 0, x, y, 0, 0)->count = DIAMOND_DELAY;
	}
      /*}}}*/
      if(!monster.delay--)
	monster.delay = 31;
      move_mris();
      if(monster.monsters == 1)
	global.count--;
    }
    /*}}}*/
  return then;
}
/*}}}*/
/*{{{  ANIMATE animate_def_keys(next)*/
static FUNCANIMATE(animate_def_keys, next)
/*
 * Alter the control keys. This is a bit wierd, compared to the other
 * animate functions, 'cos there is no timeout on it. It's entirely
 * driven by the user pressing enough keys.
 * Don't let the user define one key to two functions.
 */
{
  static PROTOANIMATE((*rts));
  PROTOVOID(*then);
  
  then = (PROTOVOID(*))animate_def_keys;
  if(next)
    /*{{{  start*/
    {
      rts = (PROTOANIMATE((*)))next;
      global.state = MODE_KEY_DEF;
      global.key = 0;
      monster.monsters = 0;
      apple.apples = 0;
      update.back.backs = 0;
      update.score.scores = 0;
      player.ball.state = 0;
      player.ball.count = 16;
      player.old_ball.state = 0;
      player.old_ball.count = 16;
      XFillRectangle(display.display, display.back, GCN(GC_CLEAR),
	  0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
      back_mris(0);
      monster.den = 0;
      monster.delay = 0;
      while(!strchr(title_text[monster.delay].text, '%'))
	{
	  back_title(&title_text[monster.delay], monster.delay, 0);
	  monster.delay++;
	}
      global.count = 0;
    }
    /*}}}*/
  else
    /*{{{  start again*/
    {
      TITLE const *tptr;
      unsigned  lookup;
      KeySym    key;
      
      tptr = &title_text[monster.delay + global.count];
      key = global.key;
      if(key == data.keysymbols[KEY_THROW])
	key = data.keysymbols[tptr->ix];
      for(lookup = 0; lookup != KEYS; lookup++)
	if((monster.den & 1 << lookup) && data.keysymbols[lookup] == key)
	  break;
      if(lookup == KEYS)
	{
	  data.keysymbols[tptr->ix] = key;
	  monster.den |= 1 << tptr->ix;
	  back_title(tptr, monster.delay + global.count, 0);
	  global.count++;
	}
    }
    /*}}}*/
  global.pause = 0;
  if(global.count < KEYS)
    /*{{{  animate*/
    {
      TITLE const *tptr;
      
      tptr = &title_text[monster.delay + global.count];
      back_title(tptr, monster.delay + global.count, 1);
      XCopyArea(display.display, display.back, display.copy, GCN(GC_COPY),
	      0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0);
      /*{{{  help line*/
      {
	char    buffer[49];
	size_t  length;
	    
	sprintf(buffer, "Press new key or %s for current",
	    XKeysymToString(data.keysymbols[KEY_THROW]));
	length = strlen(buffer);
	assert(length < sizeof(buffer));
	XFillRectangle(display.display, display.back, GCN(GC_CLEAR),
	    0, PIXELY(CELLS_DOWN, 0), WINDOW_WIDTH, CELL_HEIGHT);
	XDrawImageString(display.display, display.back, GCN(GC_TEXT),
	    CENTER_X - (int)(length * font.width / 2),
	    PIXELY(CELLS_DOWN, CELL_HEIGHT / 2) + font.center,
	    buffer, length);
	XCopyArea(display.display, display.back, display.copy, GCN(GC_COPY),
	    0, PIXELY(CELLS_DOWN, 0), WINDOW_WIDTH, CELL_HEIGHT,
	    0, PIXELY(CELLS_DOWN, 0));
      }
      /*}}}*/
      XCopyArea(display.display, display.back, display.copy, GCN(GC_COPY),
	  0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0);
      refresh_window();
      global.key = 0;
    }
    /*}}}*/
  else
    /*{{{  end*/
    {
      global.throw = 0;
      assert(rts != (PROTOANIMATE((*)))NULL);
      then = (*rts)((PROTOVOID(*))NULL);
    }
    /*}}}*/
  return then;
}
/*}}}*/
/*{{{  ANIMATE animate_demo(next)*/
extern FUNCANIMATE(animate_demo, next)
/*
 * The main demonstration control loop. Start or quit the game
 * as necessary, or just cycle round the key display, demo board, and
 * high score tables.
 */
{
  static int state = 1;
  PROTOVOID(*then);

  then = NULL;
  assert(!next);
  global.pause = 0;
  if(global.pressed & 1 << KEY_QUIT)
    global.quit = 1;
  else if(global.throw == 1)
    {
      state = 0;
      global.throw = 2;
      then = animate_game((PROTOVOID(*))NULL);
    }
  else if(global.pressed & 1 << KEY_KEYBOARD)
    {
      state = 2;
      then = animate_def_keys((PROTOVOID(*))animate_demo);
    }
  else
    {
      /*{{{  switch display_score?*/
      if(state == 1)
	{
	  static HIGH_SCORE *list[3] =
	    {today_scores, high_scores, personal_scores};
	  unsigned  ix;
	  unsigned  count;
	  
	  if(!display_scores)
	    ix = 0;
	  else
	    for(ix = 3; ix--;)
	      if(list[ix] == display_scores)
		break;
	  for(count = 3; (ix = (ix + 1) % 3), count--;)
	    if(list[ix]->score)
	      {
		display_scores = list[ix];
		break;
	      }
	}
      /*}}}*/
      if(state == 0 && display_scores)
	/*{{{  scores*/
	{
	  state = 1;
	  retire_scores();
	  then = animate_score((PROTOVOID(*))animate_demo);
	}
	/*}}}*/
      else if(state == 1 || state == 0)
	/*{{{  name*/
	{
	  state = 2;
	  then = animate_name((PROTOVOID(*))animate_demo);
	}
	/*}}}*/
      else if(state == 2)
	/*{{{  keys*/
	{
	  state = data.sprites == True ? 3 : 4;
	  then = animate_keys((PROTOVOID(*))animate_demo);
	}
	/*}}}*/
      else if(state == 3)
	/*{{{  sprites*/
	{
	  state = 4;
	  assert(data.sprites == True);
	  then = animate_sprites((PROTOVOID(*))animate_demo);
	}
	/*}}}*/
      else if(state == 4)
	/*{{{  board*/
	{
	  state = 0;
	  then = animate_board((PROTOVOID(*))animate_demo);
	}
	/*}}}*/
      else
	{
	  /*
	   * Sun's assert macro is broken, so I have to
	   * put it in a scope
	   */
	  assert(0);
	}
    }
  return then;
}
/*}}}*/
/*{{{  ANIMATE animate_diamond(next)*/
extern FUNCANIMATE(animate_diamond, next)
/*
 * A gem of a screen.
 */
{
  static PROTOANIMATE((*rts));
  PROTOVOID(*then);
  
  then = (PROTOVOID(*))animate_diamond;
  if(next)
    /*{{{  start*/
    {
      rts = (PROTOANIMATE((*)))next;
      monster.monsters = 0;
      apple.apples = 0;
      update.back.backs = 0;
      update.score.scores = 0;
      player.ball.state = 0;
      player.ball.count = 8;
      add_background(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT - 1);
      XFillRectangle(display.display, display.back, GCN(GC_CLEAR),
	  BORDER_LEFT + 1, BORDER_TOP + 1, BOARD_WIDTH - 2, BOARD_HEIGHT - 2);
      spawn_monster(4, 3, 3, CELLS_ACROSS + 1, DEN_Y,
	  -(CELL_WIDTH + GAP_WIDTH) * CELLS_ACROSS, 0);
      spawn_monster(6, 0, 0, DEN_X, DEN_Y, 0, 0);
      draw_extra_letter(extra.select);
      global.state = MODE_GAME_DEMO;
      global.count = 3;
      timer_set(FRAME_RATE, TIMING_PAUSE);
    }
    /*}}}*/
  /*{{{  skip?*/
  if(global.throw == 1)
    {
      global.throw = 2;
      global.count = 0;
    }
  /*}}}*/
  if(!global.count)
    /*{{{  end*/
    {
      assert(rts != (PROTOANIMATE((*)))NULL);
      then = (*rts)((PROTOVOID(*))NULL);
    }
    /*}}}*/
  else
    /*{{{  animate*/
    {
      if(monster.list[0].offset.x ==
	  VEL_X - (CELL_WIDTH + GAP_WIDTH) * (CELLS_ACROSS + 2 - DEN_X))
	{
	  monster.list[0].pushing = 1;
	  new_face(&monster.list[0]);
	  monster.list[1].cell.x = CELLS_ACROSS + 1;
	  monster.list[1].offset.x = -(CELL_WIDTH + GAP_WIDTH) *
	    (CELLS_ACROSS + 1 - DEN_X);
	}
      if(!move_demo(MONSTER_CYCLES))
	global.count--;
    }
    /*}}}*/
  return then;
}
/*}}}*/
/*{{{  ANIMATE animate_extra_life(next)*/
extern FUNCANIMATE(animate_extra_life, next)
/*
 * animate an extra life
 */
{
  static PROTOANIMATE((*rts));
  static char const *text[] = {"Congratulations", "You win extra", NULL};
  PROTOVOID(*then);
  
  then = (PROTOVOID(*))animate_extra_life;
  if(next)
    /*{{{  start*/
    {
      rts = (PROTOANIMATE((*)))next;
      monster.monsters = 0;
      apple.apples = 0;
      update.back.backs = 0;
      update.score.scores = 0;
      player.ball.state = 0;
      player.ball.count = 3;
      player.old_ball.state = 0;
      player.old_ball.count = 16;
      add_background(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT - 1);
      XFillRectangle(display.display, display.back, GCN(GC_CLEAR),
	  BORDER_LEFT + 1, BORDER_TOP + 1, BOARD_WIDTH - 2, BOARD_HEIGHT - 2);
      /*{{{  display some text*/
      {
	unsigned  line;
	char const **tptr;
	    
	line = BORDER_TOP + CELL_HEIGHT;
	for(tptr = text; *tptr; tptr++)
	  {
	    size_t    length;
	    
	    length = strlen(*tptr);
	    XDrawImageString(display.display, display.back, GCN(GC_TEXT),
		CENTER_X - (int)(length * font.width / 2),
		(int)line + font.ascent, *tptr, length);
	    line += font.ascent + font.descent;
	  }
      }
      /*}}}*/
      spawn_monster(4, 3, 3, 3, DEN_Y, -(CELL_WIDTH + GAP_WIDTH) * 3, 0);
      spawn_monster(0, 2, 2, 0, DEN_Y,
	  (CELLS_ACROSS - 1) * (CELL_WIDTH + GAP_WIDTH), 0);
      draw_extra_letter(extra.select);
      /*{{{  add m r i s*/
      {
	unsigned  i;
	    
	for(i = 4; i--;)
	  {
	    back_sprite(SPRITE_MRIS + 4 + i, 0,
		PIXELX(CELLS_ACROSS / 2 - 2 + (int)i, 0), PIXELY(2, 0));
	    spawn_monster(SPRITE_MRIS + i, 1, 1,
		CELLS_ACROSS / 2 - 2 + (int)i, 2,
		0, -(CELL_HEIGHT + GAP_HEIGHT) * 3 -
		  (int)CELL_HEIGHT / 2 * (int)i);
	  }
      }
      /*}}}*/
      /*{{{  create a path*/
      {
	unsigned  i;
	CELL      *cptr;
	    
	apple.apples = 0;
	for(cptr = BOARDCELL(0, DEN_Y), i = CELLS_ACROSS; i--; cptr++)
	  {
	    cptr->visit = 1;
	    cptr->sprite = 0;
	    cptr->distance = CELLS_ACROSS - i;
	    cptr->depths[0] = 0;
	    cptr->depths[1] = 0;
	    cptr->depths[2] = -(CELL_WIDTH + GAP_WIDTH);
	    cptr->depths[3] = CELL_WIDTH + GAP_WIDTH;
	    cptr->holes[0] = 0;
	    cptr->holes[1] = 0;
	    cptr->holes[2] = 0;
	    cptr->holes[3] = 0;
	  }
	BOARDCELL(0, DEN_Y)->depths[2] = 0;
	BOARDCELL(CELLS_ACROSS - 1, DEN_Y)->depths[3] = 0;
      }
      /*}}}*/
      global.state = MODE_GAME_DEMO;
      global.count = 3;
      timer_set(FRAME_RATE, TIMING_PAUSE);
    }
    /*}}}*/
  if(global.throw == 1)
    {
      global.count = 0;
      global.throw = 2;
    }
  if(!global.count)
    /*{{{  end*/
    {
      unsigned  i;
      int       x;
      
      extra.got = 0;
      create_xtra_monster(extra.select);
      for(i = 5; i--;)
	draw_extra_letter(i);
      x = PIXELX((int)global.lives - 1, 0);
      XCopyArea(display.display, sprites[SPRITE_PLAYER + 6].mask,
	  display.back, GCN(GC_MASK), 0, 0,
	  CELL_WIDTH, CELL_HEIGHT, x, PIXELY(CELLS_DOWN, 0));
      XCopyArea(display.display, sprites[SPRITE_PLAYER + 6].image,
	  display.back, GCN(GC_OR), 0, 0,
	  CELL_WIDTH, CELL_HEIGHT, x, PIXELY(CELLS_DOWN, 0));
      add_background(x, PIXELY(CELLS_DOWN, 0), CELL_WIDTH, CELL_HEIGHT);
      global.lives++;
      assert(rts != (PROTOANIMATE((*)))NULL);
      then = (*rts)((PROTOVOID(*))NULL);
    }
    /*}}}*/
  else
    /*{{{  animate*/
    {
      if(!monster.list[0].offset.x && global.count == 3)
	{
	  global.count--;
	  global.throw = 1;
	}
      bounce_ball();
      global.throw = 0;
      if(player.ball.state == 3)
	player.ball.state = 4;
      else if(player.ball.state == 4)
	{
	  player.ball.pixel.x = PIXELX(global.lives - 1, 0) + CELL_WIDTH / 2;
	  player.ball.pixel.y = BORDER_TOP + BOARD_HEIGHT + 1 +
	      CELL_HEIGHT / 2 - (CELL_HEIGHT + GAP_HEIGHT) * 3;
	}
      else if(!player.ball.state && global.count == 2)
	{
	  monster.list[0].face = 10;
	  spawn_monster(SPRITE_PLAYER + 6, 0, 1, (int)global.lives - 1,
	      CELLS_DOWN, 0, -(CELL_HEIGHT + GAP_HEIGHT) * 3);
	  global.count--;
	}
      if(global.count == 1)
	player.ball.count = 8;
      if(!move_demo(MONSTER_CYCLES) && global.count == 1)
	global.count--;
    }
    /*}}}*/
  return then;
}
/*}}}*/
/*{{{  ANIMATE animate_history(next)*/
extern FUNCANIMATE(animate_history, next)
/*
 * shows the history list
 */
{
  static PROTOANIMATE((*rts));
  PROTOVOID(*then);
  
  then = (PROTOVOID(*))animate_history;
  if(next)
    /*{{{  start*/
    {
      rts = (PROTOANIMATE((*)))next;
      monster.monsters = 0;
      apple.apples = 0;
      update.back.backs = 0;
      update.score.scores = 0;
      player.ball.state = 0;
      player.ball.count = 16;
      player.old_ball.state = 0;
      player.old_ball.count = 16;
      add_background(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT - 1);
      draw_extra_letter(extra.select);
      XFillRectangle(display.display, display.back, GCN(GC_CLEAR),
	  BORDER_LEFT + 1, BORDER_TOP + 1, BOARD_WIDTH - 2, BOARD_HEIGHT - 2);
      /*{{{  add in the parts*/
      {
	unsigned  base;
	unsigned  screens;
	unsigned  ix;
	unsigned  sprite;
	MONSTER   *mptr;
	char      text[11];
	size_t    length;
	static unsigned sprites[4] =
	    {SPRITE_CHERRY, SPRITE_NORMAL + 4, 4, 6};
	    
	screens = global.screen < CELLS_DOWN - 2 ?
	    global.screen : CELLS_DOWN - 2;
	base = global.screen - screens;
	for(ix = screens; ix--;)
	  {
	    unsigned long time;
	    
	    sprintf(text, "Screen %d", base + ix + 1);
	    length = strlen(text);
	    XDrawImageString(display.display, display.back, GCN(GC_TEXT),
		PIXELX(CELLS_ACROSS / 2 - 1,
		  -GAP_WIDTH) - (int)(font.width * length),
		PIXELY((int)(screens - ix),
		  (CELL_HEIGHT + GAP_HEIGHT) / 2) + font.center,
		  text, length);
	    time = history.times[screens - 1 - ix] / 1000;
	    sprintf(text, "%3u\'%02u\"", (int)(time / 60), (int)(time % 60));
	    length = strlen(text);
	    XDrawImageString(display.display, display.back, GCN(GC_TEXT),
		PIXELX(CELLS_ACROSS / 2 + 2, 0),
		PIXELY((int)(screens - ix),
		  (CELL_HEIGHT + GAP_HEIGHT) / 2) + font.center,
		  text, length);
	    if(history.prize & 1 << (screens - 1 - ix))
	      spawn_monster(SPRITE_PRIZE_BASE +
		  (base + ix) % SPRITE_PRIZES, 0, 0,
		  CELLS_ACROSS / 2 + 1, (int)(screens - ix),
		  0, (int)ix * (2 * CELL_HEIGHT + GAP_HEIGHT) +
		  CELLS_DOWN * GAP_HEIGHT + CELL_HEIGHT / 2 + GAP_HEIGHT +
		  (CELL_HEIGHT + GAP_HEIGHT) *
		    (2 + CELLS_DOWN - (int)screens));
	    sprite = sprites[(history.ending >> (screens - 1 - ix) * 2) & 3];
	    mptr = spawn_monster(sprite,
		0, 0, CELLS_ACROSS / 2 - 1, (int)(screens - ix),
		0, (int)ix * (2 * CELL_HEIGHT + GAP_HEIGHT) +
		CELLS_DOWN * GAP_HEIGHT + (CELL_HEIGHT + GAP_HEIGHT) *
		(2 + CELLS_DOWN - (int)screens));
	    if(sprite == 4)
	      mptr->face = 10;
	  }
      }
      /*}}}*/
      global.state = MODE_GAME_DEMO;
      global.count = DISPLAY_HOLD;
      timer_set(FRAME_RATE, TIMING_PAUSE);
    }
    /*}}}*/
  /*{{{  skip?*/
  if(global.throw == 1)
    {
      global.throw = 2;
      global.count = 0;
    }
  /*}}}*/
  if(!global.count)
    /*{{{  end*/
    {
      assert(rts != (PROTOANIMATE((*)))NULL);
      then = (*rts)((PROTOVOID(*))NULL);
    }
    /*}}}*/
  else
    /*{{{  animate*/
    {
      if(!move_demo(MONSTER_CYCLES))
	global.count--;
    }
    /*}}}*/
  return then;
}
/*}}}*/
/*{{{  ANIMATE animate_keys(next)*/
static FUNCANIMATE(animate_keys, next)
/*
 * show the current key bindings and stuff
 */
{
  static PROTOANIMATE((*rts));
  PROTOVOID(*then);
  
  then = (PROTOVOID(*))animate_keys;
  if(next)
    /*{{{  start*/
    {
      rts = (PROTOANIMATE((*)))next;
      global.state = MODE_DEMO;
      monster.monsters = 0;
      apple.apples = 0;
      update.back.backs = 0;
      update.score.scores = 0;
      player.ball.state = 0;
      player.ball.count = 16;
      player.old_ball.state = 0;
      player.old_ball.count = 16;
      XFillRectangle(display.display, display.back, GCN(GC_CLEAR),
	  0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
      /*{{{  put on M R I S*/
      {
	unsigned  missing;
	int       x, y;
	    
	missing = chaotic() & 3;
	back_mris((unsigned)(1 << missing));
	y = chaotic() % (CELLS_DOWN + 2) * (CELL_HEIGHT + GAP_HEIGHT);
	x = chaotic() & 1 ? CELLS_ACROSS * (CELL_WIDTH + GAP_WIDTH) :
	    -CELLS_ACROSS * (CELL_WIDTH + GAP_WIDTH);
	spawn_monster(SPRITE_MRIS + missing, 0, 0,
	    4 + (int)missing, -1, x, y);
      }
      /*}}}*/
      XCopyArea(display.display, display.back, display.copy, GCN(GC_COPY),
      0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0);
      /*{{{  put on the title text*/
      {
	TITLE const *tptr;
	unsigned  ix;
	unsigned  gnome;
	    
	gnome = 0;
	for(tptr = title_text, ix = 0; tptr->text; tptr++, ix++)
	  if(back_title(tptr, ix, 0))
	    /*{{{  spawn monster*/
	    {
	      unsigned  type;
	      int       cellx;
	      int       offsetx;
	    
	      do
		{
		  type = chaotic() & 3;
		  if(type & 2)
		    type++;
		}
	      while(type == 4 && gnome);
	      if(type == 4)
		gnome = 1;
	      if(chaotic() & 1)
		{
		  cellx = -2;
		  offsetx = (CELLS_ACROSS + 2) * (CELL_WIDTH + GAP_WIDTH);
		}
	      else
		{
		  cellx = CELLS_ACROSS + 1;
		  offsetx = -(CELLS_ACROSS + 5) * (CELL_WIDTH + GAP_WIDTH);
		}
	      offsetx += CELL_WIDTH * (chaotic() & 3);
	      spawn_monster(type, 0, 0, cellx, (int)ix, offsetx, 0);
	    }
	    /*}}}*/
      }
      /*}}}*/
      global.count = DISPLAY_HOLD;
      global.missed = 0;
      refresh_window();
      timer_set(FRAME_RATE, TIMING_ON);
    }
    /*}}}*/
  if(!global.count || global.pause || global.throw == 1 ||
      global.pressed & (1 << KEY_QUIT | 1 << KEY_KEYBOARD))
    /*{{{  end*/
    {
      timer_set((unsigned long)0, TIMING_OFF);
      assert(rts != (PROTOANIMATE((*)))NULL);
      then = (*rts)((PROTOVOID(*))NULL);
    }
    /*}}}*/
  else
    /*{{{  animate*/
    {
      if(!move_demo(MONSTER_CYCLES))
	global.count--;
    }
    /*}}}*/
  return then;
}
/*}}}*/
/*{{{  ANIMATE animate_name(next)*/
static FUNCANIMATE(animate_name, next)
/*
 * Animates the game name in an exciting way.
 */
{
  static PROTOANIMATE((*rts));
  static int state;
  PROTOVOID(*then);
  
  then = (PROTOVOID(*))animate_name;
  if(next)
    /*{{{  start*/
    {
      rts = (PROTOANIMATE((*)))next;
      state = -1;
      extra.got = 0x1F;
      blank_board(&boards[chaotic() % 10]);
      XCopyArea(display.display, display.back, display.copy, GCN(GC_COPY),
	  0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0);
      player.old_ball.count = 16;
      player.ball.count = 16;
      then = animate_zoom((void (*) PROTOARG((void)))animate_name);
    }
    /*}}}*/
  else if(!global.count || global.pause || global.throw == 1 ||
      global.pressed & (1 << KEY_QUIT | 1 << KEY_KEYBOARD))
    /*{{{  end*/
    {
      assert(rts != (PROTOANIMATE((*)))NULL);
      then = (*rts)((PROTOVOID(*))NULL);
    }
    /*}}}*/
  else if(state == -1)
    /*{{{  start again*/
    {
      global.state = MODE_DEMO;
      player.old_ball.state = 0;
      player.old_ball.count = 16;
      player.ball.state = 3;
      player.ball.count = BALL_EXPLODE;
      refresh_window();
      timer_set(NAME_RATE, TIMING_OFF);
      state = 0;
    }
    /*}}}*/
  else
    /*{{{  animate*/
    {
      unsigned  throw;
      
      throw = global.throw;
      if(player.ball.state == 3)
	/*{{{  exploded?*/
	{
	  if(lettering[state])
	    {
	      player.ball.state = 4;
	      player.ball.count--;
	      if(lettering[state])
		{
		  monster.list[0].pixel.x =
		      PIXELX(LETTERX(lettering[state][0]), 0);
		  monster.list[0].pixel.y =
		      PIXELY(LETTERY(lettering[state][0]), 0);
		}
	    }
	  else
	    global.count--;
	}
	/*}}}*/
      else if(player.ball.state)
	{
	  bounce_ball();
	  /*{{{  imploded?*/
	  if(!player.ball.state)
	    {
	      unsigned char letter;
	      CELL      *cptr;
	      
	      letter = lettering[state][0];
	      spawn_monster(4, 0, 0, LETTERX(letter), LETTERY(letter), 0, 0);
	      cptr = BOARDCELL(LETTERX(letter), LETTERY(letter));
	      cptr->visit = 1;
	      update.set = 0;
	      munch_hole(cptr, PIXELX(LETTERX(letter), 0),
		  PIXELY(LETTERY(letter), 0));
	      add_background(update.tl.x, update.tl.y,
		  (unsigned)(update.br.x - update.tl.x),
		  (unsigned)(update.br.y - update.tl.y));
	      global.count = 1;
	      player.ball.count = 16;
	    }
	  /*}}}*/
	}
      else
	{
	  /*{{{  new direction?*/
	  if(!monster.list[0].count)
	    {
	      unsigned char letter;
	      
	      letter = lettering[state][global.count++];
	      if(letter == LETTEREND)
		{
		  monster.list[0].type = 5;
		  player.ball.pixel.x = monster.list[0].pixel.x +
		      CELL_WIDTH / 2;
		  player.ball.pixel.y = monster.list[0].pixel.y +
		      CELL_HEIGHT / 2;
		  player.ball.state = 2;
		  player.ball.count = 0;
		  global.count = DISPLAY_HOLD * FRAME_RATE / NAME_RATE;
		  state++;
		}
	      else
		{
		  monster.list[0].dir = LETTERDIR(letter);
		  monster.list[0].count = LETTERDIST(letter);
		  new_face(&monster.list[0]);
		}
	    }
	  /*}}}*/
	  if(!monster.list[0].cycle)
	    {
	      monster.list[0].cycle = MONSTER_CYCLES;
	      monster.list[0].image++;
	      if(monster.list[0].image == MONSTER_IMAGES)
		monster.list[0].image = 0;
	    }
	  monster.list[0].cycle--;
	  if(monster.list[0].type != 5 && move_muncher(&monster.list[0]))
	    monster.list[0].count--;
	}
      global.throw = throw;
    }
    /*}}}*/
  return then;
}
/*}}}*/
/*{{{  ANIMATE animate_score(next)*/
static FUNCANIMATE(animate_score, next)
/*
 * pick a high score table and show it
 */
{
  static PROTOANIMATE((*rts));
  PROTOVOID(*then);
  
  then = (PROTOVOID(*))animate_score;
  if(next)
    /*{{{  start*/
    {
      rts = (PROTOANIMATE((*)))next;
      /*{{{  check score file*/
      if(score_file && file_changed(score_file, effective_uid))
	{
	  FILE    *stream;
      
	  stream = get_lock(score_file, 0, effective_uid);
	  if(stream)
	    {
	      unsigned write;
	  
	      write = merge_scores(stream);
	      if(write)
		write_scores(stream);
	      get_unlock(stream);
	      if(write)
		file_changed(score_file, effective_uid);
	    }
	  else
	    score_file = NULL;
	}
      /*}}}*/
      /*{{{  check personal file*/
      if(!personal_make && personal_file &&
	  file_changed(personal_file, personal_uid))
	{
	  FILE    *stream;
      
	  stream = get_lock(personal_file, 0, personal_uid);
	  if(stream)
	    {
	      unsigned write;
	  
	      write = merge_personal(stream);
	      if(write)
		write_personal(stream);
	      get_unlock(stream);
	      if(write)
		file_changed(personal_file, personal_uid);
	    }
	  else
	    personal_file = NULL;
	}
      /*}}}*/
      global.state = MODE_DEMO;
      monster.monsters = 0;
      apple.apples = 0;
      update.back.backs = 0;
      update.score.scores = 0;
      player.ball.state = 0;
      player.ball.count = 16;
      player.old_ball.state = 0;
      player.old_ball.count = 16;
      XFillRectangle(display.display, display.back, GCN(GC_CLEAR),
	  0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
      XDrawLine(display.display, display.back, GCN(GC_SET),
	BORDER_LEFT, PIXELY(CELLS_DOWN, 0) - 1,
	BORDER_LEFT + BOARD_WIDTH, PIXELY(CELLS_DOWN, 0) - 1);
      back_mris(0);
      /*{{{  heading*/
      {
	char const *string;
	size_t    length;
	
	if(display_scores == personal_scores)
	  string = my_score.name;
	else
	  string = table_names[display_scores != high_scores];
	length = strlen(string);
	XDrawImageString(display.display, display.back, GCN(GC_TEXT),
	    PIXELX(CELLS_ACROSS / 2, 0) - (int)(length * font.width / 2),
	    PIXELY(0, CELL_HEIGHT / 2) + font.center,
	    string, length);
	if(display_scores == personal_scores)
	  string = "   Score Screen Time  Date";
	else
	  string = "   Score Screen Time  Name";
	length = strlen(string);
	XDrawImageString(display.display, display.back, GCN(GC_TEXT),
	    PIXELX(0, 0), PIXELY(1, CELL_HEIGHT / 2) + font.center,
	    string, length);
	if(my_score.score)
	  back_score(&my_score, CELLS_DOWN);
      }
      /*}}}*/
      XCopyArea(display.display, display.back, display.copy, GCN(GC_COPY),
	  0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0);
      /*{{{  put on the scores*/
      {
	HIGH_SCORE const *sptr;
	unsigned  ix;
	    
	for(sptr = display_scores, ix = 0;
	    sptr->score && ix != HIGH_SCORES; sptr++, ix++)
	  back_score(sptr, 2 + ix);
	spawn_monster(4, 3, 3, 0, (int)ix + 2,
	    -2 * (CELL_WIDTH + GAP_WIDTH), 0);
      }
      /*}}}*/
      global.count = SCORE_HOLD;
      global.missed = 0;
      refresh_window();
      timer_set(SCORE_RATE, TIMING_OFF);
    }
    /*}}}*/
  if(!global.count || global.pause || global.throw == 1 ||
      global.pressed & (1 << KEY_QUIT | 1 << KEY_KEYBOARD))
    /*{{{  end*/
    {
      assert(rts != (PROTOANIMATE((*)))NULL);
      then = (*rts)((PROTOVOID(*))NULL);
    }
    /*}}}*/
  else
    /*{{{  animate*/
    {
      if(!move_demo(SCORE_CYCLES))
	global.count--;
      /*{{{  turn player?*/
      if(monster.list[0].offset.x == 0 &&
	  monster.list[0].offset.y == 0 && monster.list[0].cell.y > 0)
	{
	  if(monster.list[0].dir == 0)
	    {
	      if(monster.list[0].cell.x)
		/*{{{  go left*/
		{
		  monster.list[0].offset.x =
		      monster.list[0].cell.x * (CELL_WIDTH + GAP_WIDTH);
		  monster.list[0].cell.x = 0;
		}
		/*}}}*/
	      else
		/*{{{  go right*/
		{
		  size_t    length;
		  size_t    temp;
		  unsigned  ix;
				
		  ix = monster.list[0].cell.y - 2;
		  length = strlen(display_scores[ix].name);
		  if(ix != 0)
		    {
		      temp = strlen(display_scores[ix - 1].name);
		      if(temp > length)
			length = temp;
		    }
		  length = ((length + 22) * font.width + GAP_WIDTH) /
		      (CELL_WIDTH + GAP_WIDTH) ;
		  monster.list[0].cell.x = length;
		  monster.list[0].offset.x =
		      -(length * (CELL_WIDTH + GAP_WIDTH));
		}
		/*}}}*/
	    }
	  else if(monster.list[0].cell.y != 2)
	    /*{{{  go up 1*/
	    {
	      monster.list[0].cell.y--;
	      monster.list[0].offset.y = CELL_HEIGHT + GAP_HEIGHT;
	    }
	    /*}}}*/
	  else
	    /*{{{  go off screen*/
	    {
	      monster.list[0].cell.y = -2;
	      monster.list[0].offset.y = 4 * (CELL_HEIGHT + GAP_HEIGHT);
	    }
	    /*}}}*/
	}
      /*}}}*/
    }
    /*}}}*/
  return then;
}
/*}}}*/
/*{{{  ANIMATE animate_sprites(next)*/
static FUNCANIMATE(animate_sprites, next)
/*
 * show off the sprites
 */
{
  static PROTOANIMATE((*rts));
  static unsigned pressed = 0;
  static MONSTER  *happy;
  PROTOVOID(*then);
  
  then = (PROTOVOID(*))animate_sprites;
  if(next)
    /*{{{  initialize*/
    {
      rts = (PROTOANIMATE((*)))next;
      global.state = MODE_DEMO;
      monster.monsters = 0;
      apple.apples = 0;
      update.back.backs = 0;
      update.score.scores = 0;
      player.ball.state = 0;
      player.ball.count = 16;
      player.old_ball.state = 0;
      player.old_ball.count = 16;
      extra.select = 0;
      extra.got = 0;
      XFillRectangle(display.display, display.back, GCN(GC_CLEAR),
	  0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
      /*{{{  M R I S*/
      {
	unsigned  ix;
	
	for(ix = 4; ix--;)
	  {
	    back_sprite(SPRITE_MRIS + ix, 0, PIXELX(4 + (int)ix, 0),
		PIXELY(-1, 0));
	    back_sprite(SPRITE_MRIS + 4 + ix, 0, PIXELX((int)ix, 0),
		PIXELY(-1, 0));
	    back_sprite(SPRITE_MRIS + 4 + ix, 0, PIXELX(8 + (int)ix, 0),
		PIXELY(-1, 0));
	  }
      }
      /*}}}*/
      /*{{{  normal, munch, drone, player*/
      {
	unsigned  ix;
	
	for(ix = 12; ix--;)
	  {
	    back_sprite(SPRITE_NORMAL + ix, 0,
		PIXELX(0, 0), PIXELY((int)ix, 0));
	    back_sprite(SPRITE_MUNCHER + ix, 0,
		PIXELX(1, 0), PIXELY((int)ix, 0));
	    back_sprite(SPRITE_DRONE + ix, 0,
		PIXELX(3, 0), PIXELY((int)ix, 0));
	    back_sprite(SPRITE_PLAYER + ix, 0,
		PIXELX(4, 0), PIXELY((int)ix, 0));
	  }
      }
      /*}}}*/
      /*{{{  xtra*/
      {
	unsigned  ix;
	
	for(ix = 10; ix--;)
	  {
	    if(ix & 1)
	      create_xtra_monster(ix >> 1);
	    back_sprite(SPRITE_XTRA + ix, 0,
		PIXELX(2, 0), PIXELY((int)ix, 0));
	  }
      }
      /*}}}*/
      back_sprite(SPRITE_CHOMP, 0, PIXELX(2, 0), PIXELY(10, 0));
      back_sprite(SPRITE_CHOMP + 1, 0, PIXELX(2, 0), PIXELY(11, 0));
      /*{{{  squished*/
      {
	unsigned  ix;
	
	for(ix = 5; ix--;)
	  {
	    back_sprite(SPRITE_SQUISHED + (ix << 1), 0,
		PIXELX((int)ix, 0), PIXELY(12, 0));
	    back_sprite(SPRITE_SQUISHED + 1 + (ix << 1), 0,
		PIXELX((int)ix, 0), PIXELY(12, CELL_HEIGHT / 2));
	  }
      }
      /*}}}*/
      /*{{{  push, pause, happy, dead*/
      {
	unsigned  ix;
	
	for(ix = 4; ix--;)
	  back_sprite(SPRITE_PLAYER_PUSH + ix, 0,
	      PIXELX(5, 0), PIXELY((int)ix, 0));
	for(ix = 4; ix--;)
	  back_sprite(SPRITE_PLAYER_REST + ix, 0,
	      PIXELX(5, 0), PIXELY((int)ix + 4, 0));
	for(ix = 2; ix--;)
	  back_sprite(SPRITE_PLAYER_HAPPY + ix, 0,
	      PIXELX(5, 0), PIXELY((int)ix + 8, 0));
	if(data.gender == True)
	  for(ix = 2; ix--;)
	    back_sprite(SPRITE_PLAYER_DEAD + ix, 0,
	      PIXELX(5, 0), PIXELY((int)ix + 10, 0));
      }
      /*}}}*/
      back_sprite(SPRITE_BALL, 0, PIXELX(6, CELL_WIDTH / 2 - BALL_WIDTH / 2),
	  PIXELY(0, CELL_HEIGHT / 4 - BALL_HEIGHT / 2));
      back_sprite(SPRITE_SEAT, 0, PIXELX(6, 0), PIXELY(0, CELL_HEIGHT / 2));
      back_sprite(SPRITE_DEN, 0, PIXELX(6, 0), PIXELY(1, 0));
      back_sprite(SPRITE_CHERRY, 0, PIXELX(6, 0), PIXELY(2, 0));
      /*{{{  prizes*/
      {
	unsigned  ix;
	
	for(ix = SPRITE_PRIZES; ix--;)
	  back_sprite(SPRITE_PRIZE_BASE + ix, 0,
	      PIXELX(6, 0), PIXELY((int)ix + 3, 0));
	for(ix = 3; ix--;)
	  back_sprite(SPRITE_DIAMOND + ix, 0, PIXELX(7 + (int)ix, 0),
	      PIXELY(BOARDS, 0));
      }
      /*}}}*/
      /*{{{  lettering*/
      {
	back_sprite(SPRITE_DIGITS, 0, PIXELX(5, GAP_WIDTH +
	    (int)sprites[SPRITE_EXTRA].size.x),
	    PIXELY(CELLS_DOWN - 1, CELL_HEIGHT / 2 - DIGIT_HEIGHT / 2));
	back_sprite(SPRITE_EXTRA, 0, PIXELX(5, 0), PIXELY(CELLS_DOWN - 1,
	    CELL_HEIGHT / 2 - (int)sprites[SPRITE_EXTRA].size.y / 2));
	back_sprite(SPRITE_EXTRA + 1, 0,
	    PIXELX(CELLS_ACROSS - 1, CELL_WIDTH) -
	      (int)sprites[SPRITE_EXTRA + 1].size.x,
	    PIXELY(CELLS_DOWN - 1,
	      CELL_HEIGHT / 2 - (int)sprites[SPRITE_EXTRA + 1].size.y / 2));
      }
      /*}}}*/
      /*{{{  apples*/
      {
	unsigned  ix;
	
	for(ix = 2; ix--;)
	  back_sprite(SPRITE_GHOST + ix, 0,
	      PIXELX(10, CELL_WIDTH / 2), PIXELY((int)ix, 0));
	for(ix = 6; ix--;)
	  back_sprite(SPRITE_APPLE + ix, 0,
	      PIXELX(10, apple_sizes[ix].offset.x + CELL_WIDTH / 2),
	      PIXELY((int)ix + 2, apple_sizes[ix].offset.y));
      }
      /*}}}*/
      /*{{{  backgrounds*/
      {
	unsigned  ix;
	unsigned  count;
	
	for(ix = BOARDS, count = 0; ix--;)
	  {
	    XGCValues gcv;
      
	    /*{{{  color?*/
	    if(data.mono != True)
	      {
		backgrounds[0] = boards[count].colors[0];
		backgrounds[1] = boards[count].colors[1];
		gcv.fill_style = FillSolid;
		gcv.foreground = colors[display.dynamic ?
		    0 : backgrounds[0]].pixel;
		XChangeGC(display.display, GCN(GC_BOARD),
		    GCFillStyle | GCForeground, &gcv);
		XFillRectangle(display.display, display.back, GCN(GC_BOARD),
		    PIXELX(7, -(GAP_WIDTH / 2)),
		    PIXELY((int)ix, -(GAP_HEIGHT / 2)),
		    (CELL_WIDTH + GAP_WIDTH) * 3, CELL_HEIGHT + GAP_HEIGHT);
	      }
	    /*}}}*/
	    /*{{{  draw stipple*/
	    {
	      gcv.fill_style = FillStippled;
	      gcv.foreground = data.mono != False ?
		  display.black : colors[display.dynamic ?
		    1 : backgrounds[1]].pixel;
	      gcv.stipple = fills[boards[count].fill].mask;
	      XChangeGC(display.display, GCN(GC_BOARD),
		  GCStipple | GCFillStyle | GCForeground, &gcv);
	      XFillRectangle(display.display, display.back, GCN(GC_BOARD),
		  PIXELX(7, -(GAP_WIDTH / 2)),
		  PIXELY((int)ix, -(GAP_HEIGHT / 2)),
		  (CELL_WIDTH + GAP_WIDTH) * 3, CELL_HEIGHT + GAP_HEIGHT);
	    }
	    /*}}}*/
	    XDrawRectangle(display.display, display.back, GCN(GC_SET),
		PIXELX(7, -(GAP_WIDTH / 2)),
		PIXELY((int)ix, -(GAP_HEIGHT / 2)),
		(CELL_WIDTH + GAP_WIDTH) * 3 - 1,
		CELL_HEIGHT + GAP_HEIGHT - 1);
	    if(count)
	      count--;
	    else
	      count = BOARDS - 1;
	  }
	for(ix = BOARDS; ix--;)
	  {
	    back_sprite(SPRITE_APPLE, 1, PIXELX(8,
		(CELL_WIDTH + GAP_WIDTH) / 2), PIXELY((int)ix, 0));
	    back_sprite(SPRITE_CHERRY, 1, PIXELX(8,
		-(CELL_WIDTH + GAP_WIDTH) / 2), PIXELY((int)ix, 0));
	  }
	if(display.dynamic == True)
	  set_colors(1);
	global.count = 1;
      }
      /*}}}*/
      /*{{{  spawn things*/
      {
	unsigned  ix;
	
	for(ix = 5; ix--;)
	  spawn_monster(ix, 3, 3, (int)ix, CELLS_DOWN, 0, 0);
	spawn_monster(4, 3, 7, 5, CELLS_DOWN, 0, 0)->pushing = 1;
	spawn_monster(4, 3, 9, 6, CELLS_DOWN, 0, 0);
	happy = spawn_monster(4, 3, 10, 7, CELLS_DOWN, 0, 0);
	spawn_monster(3, 3, 6, 8, CELLS_DOWN, 0, 0)->chew = 1;
	spawn_monster(6, 0, 0, 9, CELLS_DOWN, 0, 0);
	spawn_apple(10, CELLS_DOWN, CELL_WIDTH / 2, 0)->count = 0;
      }
      /*}}}*/
      XCopyArea(display.display, display.back, display.copy, GCN(GC_COPY),
	  0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0);
      refresh_window();
      timer_set(FRAME_RATE * 2, TIMING_OFF);
    }
    /*}}}*/
  if(global.pause || global.pressed & (1 << KEY_QUIT | 1 << KEY_KEYBOARD))
    /*{{{  end*/
    {
      assert(rts != (PROTOANIMATE((*)))NULL);
      then = (*rts)((PROTOVOID(*))NULL);
    }
    /*}}}*/
  else
    /*{{{  animate*/
    {
      MONSTER   *mptr;
      unsigned  count;
      unsigned  dir;
      
      dir = global.pressed & ~pressed & 0xF;
      if(dir)
	dir = choose_direction(dir) | 4;
      /*{{{  throw?*/
      if(global.throw == 1)
	{
	  global.throw = 2;
	  if(display.dynamic == True)
	    /*{{{  new backgrounds*/
	    {
	      global.count++;
	      if(global.count == BOARDS + 1)
		global.count = 0;
	      if(!global.count)
		set_colors(0);
	      else
		{
		  backgrounds[0] = boards[global.count % BOARDS].colors[0];
		  backgrounds[1] = boards[global.count % BOARDS].colors[1];
		  set_colors(1);
		}
	    }
	    /*}}}*/
	  if(happy->type == 4)
	    {
	      unsigned ix;
	      
	      for(ix = 8; ix--;)
		if(monster.list[0].face == player_dies[ix])
		  break;
	      happy->type = SPRITE_PLAYER_DEAD + ix * 2;
	    }
	  else
	    happy->type = 4;
	  /*{{{  xtra*/
	  {
	    unsigned  ix;
	    unsigned  got;
	    
	    got = extra.got;
	    extra.got = happy->type != 4 ? 0x1F : 0;
	    for(ix = 10; ix--;)
	      {
		if(ix & 1)
		  create_xtra_monster(ix >> 1);
		back_sprite(SPRITE_XTRA + ix, 1,
		    PIXELX(2, 0), PIXELY((int)ix, 0));
	      }
	    add_background(PIXELX(2, 0), PIXELY(0, 0),
		CELL_WIDTH, CELL_HEIGHT * 10 + GAP_HEIGHT * 9);
	    extra.got = got;
	    create_xtra_monster(extra.select);
	  }
	  /*}}}*/
	}
      /*}}}*/
      for(mptr = monster.list, count = monster.monsters; count--; mptr++)
	{
	  /*{{{  new face?*/
	  if(dir)
	    {
	      if(mptr->type < 5 && mptr->face < 10)
		{
		  if(mptr->face < 6 || dir >= 6)
		    {
		      unsigned  shift;
		      
		      shift = mptr->face > 7;
		      mptr->dir = dir & 0x3;
		      new_face(mptr);
		      if(shift)
			mptr->face += 6;
		      if(mptr->type == 2)
			{
			  extra.select++;
			  if(extra.select == 5)
			    {
			      extra.select = 0;
			      extra.got ^= 0x1F;
			    }
			  create_xtra_monster(extra.select);
			}
		    }
		}
	      else if(happy->type != 4)
		{
		  unsigned ix;
		  
		  for(ix = 8; ix--;)
		    if(monster.list[0].face == player_dies[ix])
		      break;
		  happy->type = SPRITE_PLAYER_DEAD + ix * 2;
		}
	    }
	  /*}}}*/
	  if(!mptr->cycle)
	    /*{{{  next image*/
	    {
	      mptr->cycle = mptr->type == 6 ? DIAMOND_CYCLES : MONSTER_CYCLES;
	      mptr->image++;
	      if(mptr->image == (mptr->type != 6 ?
		  MONSTER_IMAGES : DIAMOND_IMAGES))
		mptr->image = 0;
	      if(mptr == happy && mptr->type != 4)
		{
		  mptr->type++;
		  if(mptr->type == SPRITE_PLAYER_DEAD + 8)
		    mptr->type = SPRITE_PLAYER_DEAD;
		  else if(mptr->type == SPRITE_PLAYER_DEAD + 16)
		    mptr->type = SPRITE_PLAYER_DEAD + 8;
		}
	    }
	    /*}}}*/
	  mptr->cycle--;
	}
      if(!apple.list[0].count)
	{
	  apple.list[0].count = MONSTER_CYCLES;
	  apple.list[0].state++;
	  if(apple.list[0].state == 6)
	    apple.list[0].state = 0;
	}
      apple.list[0].count--;
      pressed = global.pressed;
    }
    /*}}}*/
  return then;
}
/*}}}*/
/*{{{  void back_mris(shell)*/
static VOIDFUNC back_mris
FUNCARG((shell),
	unsigned  shell
)
/*
 * Stuff a M R I or S sprite onto the background
 */
{
  unsigned  ix;
  
  for(ix = 4; ix--;)
    back_sprite(SPRITE_MRIS + ix + 4 * !!((1 << ix) & shell), 0,
	PIXELX(4 + (int)ix, 0), PIXELY(-1, 0));
  return VOIDRETURN;
}
/*}}}*/
/*{{{  void back_score(sptr, ix)*/
static VOIDFUNC back_score
FUNCARG((sptr, ix),
	HIGH_SCORE const *sptr
ARGSEP  unsigned  ix
)
/*
 * stuff a score onto the background
 */
{
  size_t    length;
  char      string[30];
  COORD     place;
  
  sprintf(string, "%8lu  %3u %3u\'%02u\"",
      sptr->score, sptr->screen, sptr->elapsed / 60, sptr->elapsed % 60);
  length = strlen(string);
  place.x = PIXELX(0, 0);
  place.y = PIXELY((int)ix, CELL_HEIGHT / 2) + font.center;
  XDrawImageString(display.display, display.back, GCN(GC_TEXT),
      place.x, place.y, string, length);
  place.x += (length + 1) * font.width;
  length = strlen(sptr->name);
  XDrawImageString(display.display, display.back, GCN(GC_TEXT),
      place.x, place.y, sptr->name, length);
  return VOIDRETURN;
}
/*}}}*/
/*{{{  int back_title(tptr, ix, flag)*/
static int back_title
FUNCARG((tptr, ix, flag),
	TITLE const *tptr
ARGSEP  unsigned  ix
ARGSEP  unsigned  flag
)
/*
 * stuff a title line onto the background
 */
{
  char      buffer[65];
  size_t    length;
  
  /*{{{  insert key string?*/
  {
    char const *ptr;
	
    ptr = strchr(tptr->text, '%');
    if(!ptr)
      strcpy(buffer, tptr->text);
    else if(!flag)
      sprintf(buffer, tptr->text,
	  XKeysymToString(data.keysymbols[tptr->ix]));
    else
      {
	strcpy(buffer, tptr->text);
	strcpy(&buffer[ptr - tptr->text], &ptr[2]);
      }
  }
  /*}}}*/
  length = strlen(buffer);
  if(length)
    {
      char const *ptr;
      unsigned  shift;
   
      ptr = strchr(buffer, '-');
      if(ptr)
	shift = (ptr - buffer + 1) * font.width;
      else
	shift = 0;
      XDrawImageString(display.display, display.back, GCN(GC_TEXT),
	  CENTER_X - (int)(shift ? shift : length * font.width / 2),
	  PIXELY((int)ix, CELL_HEIGHT / 2) + font.center,
	  buffer, length);
    }
  return length;
}
/*}}}*/
/*{{{  unsigned expire(now, then)*/
static unsigned expire
FUNCARG((now, then),
	time_t    now
ARGSEP  time_t    then
)
/*
 * check to see if the score is now too old
 */
{
  struct tm *ptr;
  int now_day, then_day;
  int now_hour, then_hour;
  
  ptr = localtime(&now);
  now_day = ptr->tm_yday;
  now_hour = ptr->tm_hour;
  ptr = localtime(&then);
  then_day = ptr->tm_yday;
  then_hour = ptr->tm_hour;
  return !(now_day == then_day || (now_day == then_day + 1 &&
      then_hour >= 21 && now_hour < 12));
}
/*}}}*/
/*{{{  void file_changed(name, uid)*/
static unsigned file_changed
FUNCARG((name, uid),
	char const *name
ARGSEP  uid_t       uid
)
/*
 * check if a score file has been changed since last I looked,
 * so that we pick up the new scores
 */
{
  static  time_t last_time[2];
  struct stat buffer;
  unsigned  changed;
  
  assert(name);
  if(uid == effective_uid)
    set_euid(uid);
  if(!stat(name, &buffer))
    {
      changed = buffer.st_mtime != last_time[name == score_file];
      last_time[name == score_file] = buffer.st_mtime;
    }
  else
    changed = 0;
  if(uid == effective_uid)
    set_euid(real_uid);
  return changed;
}
/*}}}*/
/*{{{  void get_lock(name, flag, uid)*/
static FILE *get_lock
FUNCARG((name, flag, uid),
	char const *name
ARGSEP  int       flag
ARGSEP  uid_t     uid
)
/*
 * open and locks a high score file
 * flag < 0 -> "r+", inhibit error messate
 * flag = 0 -> "r+"
 * flag > 0 -> "w+", inhibit error messate
 * uid != real_uid -> set_euid(uid);
 */
{
  FILE    *stream;

#ifdef NOLOCKF
  {
    int       count;
    int       filed;
    
    if(real_uid != effective_uid)
      set_euid(effective_uid);
    count = 3;
    while(count)
      {
	filed = open(lock_file, O_CREAT | O_EXCL, 0666);
	if(filed >= 0)
	  break;
	if(errno == EINTR)
	  continue;
	else if(errno == EEXIST)
	  {
	    sleep(1);
	    if(!file_changed(name, -1))
	      count--;
	  }
	else
	  {
	    perror(lock_file);
	    break;
	  }
      }
    if(filed >= 0)
      close(filed);
    if(uid != effective_uid)
      {
	assert(uid == real_uid);
	set_euid(uid);
      }
  }
#else
  if(uid != real_uid)
    {
      assert(uid == effective_uid);
      set_euid(uid);
    }
#endif /* NOLOCKF */
  stream = fopen(name, flag > 0? "w+" : "r+");
  if(!stream && !flag)
    perror(name);
#ifdef NOLOCKF
  if(!stream && lock_file)
    unlink(lock_file);
#else
  if(stream)
    while(lockf(fileno(stream), F_LOCK, 0L))
      {
	if(errno == EINTR)
	  continue;
      }
#endif /* NOLOCKF */
  if(!stream && uid != real_uid)
    set_euid(real_uid);
  return stream;
}
/*}}}*/
/*{{{  void get_unlock(stream)*/
static VOIDFUNC get_unlock
FUNCARG((stream),
	FILE    *stream
)
/*
 * unlock and close the high score file
 */
{
  fflush(stream);
#ifdef NOLOCKF
  fclose(stream);
  if(lock_file)
    {
      if(real_uid != effective_uid)
	set_euid(effective_uid);
      unlink(lock_file);
    }
#else
  rewind(stream);
  lockf(fileno(stream), F_ULOCK, 0L);
  fclose(stream);
#endif /* NOLOCKF */
  if(effective_uid != real_uid)
    set_euid(real_uid);
  return VOIDRETURN;
}
/*}}}*/
/*{{{  void high_score(score, screen, msec)*/
extern VOIDFUNC high_score
FUNCARG((score, screen, msec),
	unsigned long score
ARGSEP  unsigned  screen
ARGSEP  unsigned long msec
)
/*
 * are we worthy of imortallity?
 */
{
  HIGH_SCORE     *inserted;
  
  my_score.score = score;
  my_score.screen = screen;
  my_score.elapsed = msec / 1000;
  my_score.stamp = time((time_t *)NULL);
  my_score.marker = 0;
  inserted = NULL;
  retire_scores();
  if(insert_score(today_scores, &my_score, 1))
    inserted = today_scores;
  if(score >= SCORE_THRESHOLD && insert_score(high_scores, &my_score, 1))
    inserted = high_scores;
  /*{{{  personal*/
  {
    if(insert_personal(&my_score))
      {
	FILE      *stream;
	
	if(!inserted)
	  inserted = personal_scores;
	if(personal_file)
	  {
	    stream = get_lock(personal_file, personal_make, personal_uid);
	    if(stream)
	      {
		personal_make = 0;
		personal_home = NULL;
	      }
	    else if(!personal_make)
	      personal_file = NULL;
	  }
	else
	  stream = NULL;
	if(personal_make && personal_home)
	  {
	    assert(!stream);
	    personal_uid = real_uid;
	    stream = get_lock(personal_home, 1, personal_uid);
	    if(stream)
	      {
		personal_make = 0;
		personal_file = personal_home;
		personal_home = NULL;
	      }
	  }
	if(stream)
	  {
	    merge_personal(stream);
	    if(personal_scores[0].score)
	      write_personal(stream);
	    get_unlock(stream);
	    file_changed(personal_file, personal_uid);
	  }
	else if(personal_make)
	  {
	    personal_make = 0;
	    if(personal_file)
	      fprintf(stderr, "%s: Cannot create file\n", personal_file);
	    if(personal_home)
	      fprintf(stderr, "%s: Cannot create file\n", personal_home);
	    personal_file = NULL;
	    personal_home = NULL;
	  }
      }
  }
  /*}}}*/
  if(inserted)
    display_scores = inserted;
  if(inserted && inserted != personal_scores && score_file)
    {
      FILE      *stream;

      stream = get_lock(score_file, 0, effective_uid);
      if(stream)
	{
	  merge_scores(stream);
	  write_scores(stream);
	  get_unlock(stream);
	  file_changed(score_file, effective_uid);
	}
      else
	score_file = 0;
    }
  return VOIDRETURN;
}
/*}}}*/
/*{{{  void init_scores()*/
extern VOIDFUNC init_scores FUNCARGVOID
/*
 * check that we have the wherewithall to deal with
 * high scores
 */
{
  my_score.score = 0;
  /*{{{  set the name*/
  {
    char const *user;
    
    {
      struct passwd *ptr;
      
      ptr = getpwuid(real_uid);
      if(ptr)
	user = ptr->pw_name;
      else
	user = NULL;
    }
    if(!user)
      user = getenv("LOGNAME");
    if(!user)
      user = getenv("USER");
    if(!user)
      user = "Unknown";
    strncpy(my_score.name, user, NAME_LEN);
    my_score.name[NAME_LEN] = 0;
  }
  /*}}}*/
  if(data.dir)
    {
      FILE      *stream;
      size_t    length;

      length = strlen(data.dir);
      if(length && data.dir[length - 1] == '/')
	length--;
      score_file = malloc(length + 14);
      if(score_file)
	{
	  strcpy(score_file, data.dir);
	  strcpy(&score_file[length], "/xmris.score");
	}
#ifdef NOLOCKF
      lock_file = malloc(length + 12);
      if(lock_file)
	{
	  strcpy(lock_file, data.dir);
	  strcpy(&lock_file[length], "/xmris.lock");
	}
#endif /* NOLOCKF */
      personal_uid = effective_uid;
      /*{{{  personal file*/
      {
	size_t  nlen;
	char      *home;
	
	nlen = strlen(my_score.name);
	personal_file = malloc(length + 8 + nlen);
	if(personal_file)
	  {
	    strcpy(personal_file, data.dir);
	    strcpy(&personal_file[length], "/xmris-");
	    strcpy(&personal_file[length + 7], my_score.name);
	  }
	home = getenv("HOME");
	if(home)
	  {
	    length = strlen(home);
	    personal_home = malloc(length + 15);
	    if(personal_home)
	      {
		strcpy(personal_home, home);
		strcpy(&personal_home[length], "/.xmris.score");
	      }
	  }
      }
      /*}}}*/
      /*{{{  check score_file*/
      if(score_file)
	{
	  stream = get_lock(score_file, 0, effective_uid);
	  if(stream)
	    {
	      merge_scores(stream);
	      get_unlock(stream);
	    }
	  else
	    score_file = personal_file = NULL;
	}
      /*}}}*/
      /*{{{  check personal_file*/
      if(personal_file)
	{
	  stream = get_lock(personal_file, -1, personal_uid);
	  if(stream)
	    {
	      personal_home = NULL;
	      personal_make = 0;
	      merge_personal(stream);
	      get_unlock(stream);
	    }
	}
      /*}}}*/
      /*{{{  check personal_home*/
      if(personal_home)
	{
	  stream = get_lock(personal_home, -1, real_uid);
	  if(stream)
	    {
	      personal_file = personal_home;
	      personal_home = NULL;
	      personal_make = 0;
	      personal_uid = real_uid;
	      merge_personal(stream);
	      get_unlock(stream);
	    }
	}
      /*}}}*/
    }
  return VOIDRETURN;
}
/*}}}*/
/*{{{  unsigned insert_personal(sptr)*/
static unsigned insert_personal
FUNCARG((sptr),
	HIGH_SCORE const *sptr
)
/* 
 * inserts a score into the personal file
 */
{
  HIGH_SCORE  new;
  char const *ptr;
  
  memcpy(&new, sptr, sizeof(new));
  ptr = ctime(&new.stamp);
  memcpy(&new.name[0], &ptr[11], 5);
  memcpy(&new.name[5], &ptr[7], 3);
  memcpy(&new.name[8], &ptr[3], 5);
  memcpy(&new.name[13], &ptr[22], 2);
  new.name[15] = 0;
  return insert_score(personal_scores, &new, 0);
}
/*}}}*/
/*{{{  unsigned insert_score(sptr, iptr, single)*/
static unsigned insert_score
FUNCARG((sptr, iptr, single),
	HIGH_SCORE *sptr          /* table */
ARGSEP  HIGH_SCORE const *iptr    /* new score */
ARGSEP  unsigned  single          /* single entry */
)
/*
 * inserts a score into a high score table
 */
{
  unsigned  ix;
  unsigned  inserted;
  
  inserted = 0;
  for(ix = HIGH_SCORES; ix--; sptr++)
    if(sptr->score == iptr->score && sptr->stamp == iptr->stamp)
      break;
    else if(sptr->score < iptr->score ||
	(sptr->score == iptr->score && sptr->stamp < iptr->stamp))
      {
	HIGH_SCORE *eptr;

	for(eptr = sptr; ix--; eptr++)
	  if(!*eptr->name)
	    break;
	  else if(single && !strcmp(eptr->name, iptr->name))
	    break;
	for(; eptr != sptr; eptr--)
	  memcpy(eptr, eptr - 1, sizeof(HIGH_SCORE));
	memcpy(sptr, iptr, sizeof(HIGH_SCORE));
	inserted = 1;
	break;
      }
    else if(!strcmp(sptr->name, iptr->name))
      break;
  return inserted;
}
/*}}}*/
/*{{{  void list_scores()*/
extern VOIDFUNC list_scores FUNCARGVOID
/*
 * lists the high score tables to stdout
 */
{
  HIGH_SCORE *sptr;
  unsigned  count;
  unsigned  flag;
  
  flag = 1;
  if(high_scores[0].score)
    {
      printf("%s\n", table_names[0]);
      for(sptr = high_scores, count = HIGH_SCORES;
	  sptr->score && count--; sptr++)
	printf("%9lu %3u %3u\'%02u\" %s\n", sptr->score, sptr->screen,
	    sptr->elapsed / 60, sptr->elapsed % 60, sptr->name);
      flag = 0;
    }
  if(today_scores[0].score)
    {
      printf("%s\n", table_names[1]);
      for(sptr = today_scores, count = HIGH_SCORES;
	  sptr->score && count--; sptr++)
	printf("%9lu %3u %3u\'%02u\" %s\n", sptr->score, sptr->screen,
	    sptr->elapsed / 60, sptr->elapsed % 60, sptr->name);
      flag = 0;
    }
  else if(!flag)
    printf("Everyone's busy today.\n");
  if(personal_scores[0].score)
    {
      printf("%s's personal best\n", my_score.name);
      for(sptr = personal_scores, count = HIGH_SCORES;
	  sptr->score && count--; sptr++)
	printf("%9lu %3u %3u\'%02u\" %s\n", sptr->score, sptr->screen,
	    sptr->elapsed / 60, sptr->elapsed % 60, sptr->name);
      flag = 0;
    }
  if(flag)
    printf("Nobody ever plays with me.\n");
  return VOIDRETURN;
}
/*}}}*/
/*{{{  unsigned merge_personal(stream)*/
static unsigned merge_personal
FUNCARG((stream),
	FILE      *stream
)
/*
 * merges the personal score file into the personal score
 * table. If the score file is corrupted, then we lose the
 * merged entries. The file must have been locked appropriately.
 * returns non-zero if the file is unreadable
 */
{
  unsigned long check;
  unsigned  failed;
  HIGH_SCORE  new;
  
  clearerr(stream);
  rewind(stream);
  check = 0;
  /*{{{  read file*/
  while(1)
    {
      check = read_line(stream, &new, check);
      if(!new.score)
	break;
      insert_score(personal_scores, &new, 0);
    }
  /*}}}*/
  failed = ferror(stream) || check != new.stamp;
  if(failed)
    fprintf(stderr, "Your personal score file '%s' has been corrupted.\n",
	personal_file);
  remove_scores(personal_scores, failed);
  /*{{{  check personal file integrity*/
  if(score_file)
    {
      unsigned  repeat;
      
      for(repeat = 1; repeat &&
	  personal_scores[0].score >= SCORE_THRESHOLD;)
	{
	  HIGH_SCORE *sptr;
	  unsigned  ix;
      
	  repeat = 0;
	  for(sptr = high_scores, ix = HIGH_SCORES; ix--; sptr++)
	    if(personal_scores[0].score > sptr->score)
	      {
		for(sptr = personal_scores, ix = HIGH_SCORES - 1;
		    sptr->score && ix--; sptr++)
		  memcpy(sptr, sptr + 1, sizeof(HIGH_SCORE));
		sptr->score = sptr->stamp = 0;
		repeat = 1;
		break;
	      }
	    else if(sptr->stamp == personal_scores[0].stamp &&
		sptr->score == personal_scores[0].score &&
		!strcmp(sptr->name, my_score.name))
	      break;
	}
    }
  /*}}}*/
  return failed;
}
/*}}}*/
/*{{{  unsigned merge_scores(stream)*/
static unsigned merge_scores
FUNCARG((stream),
	FILE      *stream
)
/*
 * merges the high score file into the current high score
 * table. If the score file is corrupted, then we lose the
 * merged entries. The file must have been locked appropriately.
 * returns non-zero if the file is unreadable
 */
{
  unsigned long check;
  unsigned  failed;
  time_t    now;
  HIGH_SCORE  new;
  
  clearerr(stream);
  rewind(stream);
  now = time((time_t *)NULL);
  check = 0;
  /*{{{  read file*/
  while(1)
    {
      check = read_line(stream, &new, check);
      if(!new.score)
	break;
      if(new.score >= SCORE_THRESHOLD)
	insert_score(high_scores, &new, 1);
      if(!expire(now, new.stamp))
	insert_score(today_scores, &new, 1);
    }
  /*}}}*/
  failed = ferror(stream) || check != new.stamp;
  if(failed)
    fprintf(stderr, "The high score file '%s' has been corrupted.\n",
	score_file);
  remove_scores(high_scores, failed);
  remove_scores(today_scores, failed);
  /*{{{  check with personal scores*/
  {
    HIGH_SCORE  *sptr;
    unsigned  ix;
    
    for(ix = HIGH_SCORES, sptr = high_scores; ix-- && sptr->score; sptr++)
      if(!strcmp(sptr->name, my_score.name))
	{
	  if(sptr->score > personal_scores[0].score)
	    insert_personal(sptr);
	  break;
	}
  }
  /*}}}*/
  return failed;
}
/*}}}*/
/*{{{  int move_demo(cycle)*/
static int move_demo
	FUNCARG((cycle),
unsigned  cycle
)
/*
 * moves the monsters used in the demo screens
 * we take each monster with a non-zero offset, and move it
 * towards a zero offset (changing x first)
 * it might get blown up by the ball
 * returns the number of objects which moved
 */
{
  MONSTER   *mptr;
  unsigned  i;
  unsigned  moved;

  moved = 0;
  for(mptr = monster.list, i = monster.monsters; i--; mptr++)
    {
      if(mptr->shot)
	mptr->type = 5;
      else
	{
	  if(mptr->offset.x)
	    /*{{{  left or right*/
	    {
	      int       dir;
	    
	      moved++;
	      if(mptr->offset.x > 0)
		{
		  dir = 2;
		  mptr->offset.x -= VEL_X;
		  mptr->pixel.x -= VEL_X;
		}
	      else
		{
		  dir = 3;
		  mptr->offset.x += VEL_X;
		  mptr->pixel.x += VEL_X;
		}
	      if(dir != mptr->dir)
		{
		  mptr->dir = dir;
		  new_face(mptr);
		}
	    }
	    /*}}}*/
	  else if(mptr->offset.y)
	    /*{{{  up or down*/
	    {
	      int       dir;
	    
	      moved++;
	      if(mptr->offset.y > 0)
		{
		  dir = 0;
		  mptr->offset.y -= VEL_Y;
		  mptr->pixel.y -= VEL_Y;
		}
	      else
		{
		  dir = 1;
		  mptr->offset.y += VEL_Y;
		  mptr->pixel.y += VEL_Y;
		}
	      if(dir != mptr->dir)
		{
		  mptr->dir = dir;
		  new_face(mptr);
		}
	    }
	    /*}}}*/
	  else
	    mptr->stop = 1;
	  if(!mptr->stop || mptr->type == 6 || mptr->face == 10)
	    {
	      if(!mptr->cycle)
		{
		  mptr->cycle = mptr->type == 6 ? DIAMOND_CYCLES : cycle;
		  mptr->image++;
		  if(mptr->image == (mptr->type == 6 ?
		      DIAMOND_IMAGES : MONSTER_IMAGES))
		    mptr->image = 0;
		}
	      mptr->cycle--;
	    }
	}
	
    }
  return moved;
}
/*}}}*/
/*{{{  void move_mris()*/
static VOIDFUNC move_mris FUNCARGVOID
/*
 * moves M R I S sprites around the board
 * towards the top
 */
{
  unsigned  i;
  MONSTER   *mptr;

  for(mptr = &monster.list[1], i = monster.monsters - 1; i--; mptr++)
    if(mptr->type == 6)
      /*{{{  diamond*/
      {
	if(!mptr->count--)
	  {
	    global.diamond = 2;
	    mptr->type = 5;
	  }
	if(!mptr->cycle)
	  {
	    mptr->cycle = mptr->type == 6 ? DIAMOND_CYCLES : MONSTER_CYCLES;
	    mptr->image++;
	    if(mptr->image == (mptr->type == 6 ?
		DIAMOND_IMAGES : MONSTER_IMAGES))
	      mptr->image = 0;
	  }
	mptr->cycle--;
      }
      /*}}}*/
    else
      /*{{{  letter*/
      {
	CELL      *cptr;
	unsigned  chosen;
	
	assert(mptr->type >= SPRITE_MRIS && mptr->type < SPRITE_MRIS + 8);
	cptr = BOARDCELL(mptr->cell.x, mptr->cell.y);
	chosen = !(monster.den & 1 << ((mptr->type - SPRITE_MRIS) & 3));
	if(mptr->offset.x || mptr->offset.y)
	  move_movable(mptr, cptr);
	else if(!mptr->cell.y && chosen &&
	    mptr->cell.x == ((mptr->type - SPRITE_MRIS) & 3) + 4)
	  {
	    back_sprite(mptr->type, 0, mptr->old_pixel.x, mptr->old_pixel.y);
	    mptr->type = 5;
	    monster.normals--;
	  }
	else
	  {
	    unsigned  valid;
	    unsigned  temp;
	  
	    valid = valid_directions(mptr, cptr);
	    temp = valid & (0xF ^ (1 << (mptr->dir ^ 1)));
	    if(temp)
	      valid &= temp | 0xF0;
	    if(chosen && (temp = valid & (valid >> 4)))
	      valid = temp;
	    else
	      valid &= 0xF;
	    mptr->dir = choose_direction(valid);
	    move_movable(mptr, cptr);
	  }
      }
      /*}}}*/
  return VOIDRETURN;
}
/*}}}*/
/*{{{  unsigned long read_line(stream, sptr, check)*/
static unsigned long read_line
FUNCARG((stream, sptr, check),
	FILE      *stream
ARGSEP  HIGH_SCORE *sptr
ARGSEP  unsigned long check
)
{
  char  line[NAME_LEN + 40];
  
  if(!fgets(line, sizeof(line), stream))
    sptr->score = sptr->stamp = 0;
  else if(line[0] == '+')
    {
      char      *ptr;
      
      sptr->score = 0;
      sptr->stamp = strtol(&line[1], &ptr, 10);
      if(*ptr != '\n')
	sptr->stamp = 0;
    }
  else
    {
      size_t    length;
      char      *name;
      char      *ptr;
      
      for(ptr = line; *ptr; ptr++)
	check += *(unsigned char *)ptr;
      sptr->stamp = strtol(line, &ptr, 10);
      ptr++;
      sptr->score = strtol(ptr, &ptr, 10) / SCORE_ROUND * SCORE_ROUND;
      ptr++;
      sptr->screen = strtol(ptr, &ptr, 10);
      ptr++;
      sptr->elapsed = strtol(ptr, &name, 10);
      if(name == ptr)
	sptr->elapsed = 0;
      else
	ptr = name + 1;
      name = ptr;
      length = strlen(ptr) - 1;
      if(!sptr->score || !sptr->screen ||
	  ptr[length] != '\n' || length > NAME_LEN)
	sptr->score = 0;
      name[length] = 0;
      strcpy(sptr->name, name);
      sptr->marker = 1;
    }
  return check;
}
/*}}}*/
/*{{{  static VOIDFUNC remove_scores(base, flag)*/
static VOIDFUNC remove_scores
FUNCARG((base, flag),
	HIGH_SCORE *base
ARGSEP  unsigned  flag
)
/*
 * throw out the marked scores, 'cos they're from a corrupt score file
 * or just too darn old
 */
{
  HIGH_SCORE  *ptr;
  HIGH_SCORE  *dest;
  unsigned  ix;
	
  for(ptr = base, ix = HIGH_SCORES; ix--; ptr++)
    {
      if(flag && ptr->marker)
	ptr->score = 0;
      ptr->marker = 0;
    }
  for(ptr = dest = base, ix = HIGH_SCORES; ix--; ptr++)
    if(!ptr->score)
      /* EMPTY */;
    else if(ptr == dest)
      dest++;
    else
      {
	memcpy(dest, ptr, sizeof(HIGH_SCORE));
	dest++;
      }
  for(;dest != ptr; dest++)
    dest->score = 0;
  return VOIDRETURN;
}
/*}}}*/
/*{{{  static VOIDFUNC retire_scores()*/
static VOIDFUNC retire_scores FUNCARGVOID
/*
 * gracefully retire the wrinkly scores
 */
{
  HIGH_SCORE *ptr;
  unsigned  count;
  time_t    now;
  
  now = time((time_t *)NULL);
  for(ptr = today_scores, count = 0; count != HIGH_SCORES && ptr->score;
      ptr++, count ++)
    ptr->marker = expire(now, ptr->stamp);
  remove_scores(today_scores, 1);
  return VOIDRETURN;
}
/*}}}*/
/*{{{  void write_personal(stream)*/
static VOIDFUNC write_personal
FUNCARG((stream),
      FILE    *stream
)
/*
 * writes out the personal score table to the file.
 * the file must have been locked appropriately.
 */
{
  unsigned long check;

  clearerr(stream);
  rewind(stream);
  check = 0;
  check = write_table(stream, personal_scores, check);
  if(ferror(stream))
    {
      clearerr(stream);
      rewind(stream);
      check = 0;
    }
  fprintf(stream, "+%lu\n", check);
  return VOIDRETURN;
}
/*}}}*/
/*{{{  void write_scores(stream)*/
static VOIDFUNC write_scores
FUNCARG((stream),
      FILE    *stream
)
/*
 * writes out the high score table to the file.
 * the file must have been locked appropriately.
 */
{
  unsigned long check;

  clearerr(stream);
  rewind(stream);
  check = 0;
  check = write_table(stream, high_scores, check);
  check = write_table(stream, today_scores, check);
  if(ferror(stream))
    {
      clearerr(stream);
      rewind(stream);
      check = 0;
    }
  fprintf(stream, "+%lu\n", check);
  return VOIDRETURN;
}
/*}}}*/
/*{{{  unsigned long write_table(stream, sptr, check)*/
static unsigned long write_table
FUNCARG((stream, sptr, check),
	FILE      *stream
ARGSEP  HIGH_SCORE *sptr
ARGSEP  unsigned long check
)
{
  unsigned  ix;
  
  for(ix = HIGH_SCORES; ix-- && sptr->score; sptr++)
    {
      char      line[NAME_LEN + 40];
      char      *ptr;
      
      sprintf(line, "%lu %lu %u %u %s\n", (unsigned long)sptr->stamp,
	  sptr->score, sptr->screen, sptr->elapsed, sptr->name);
      fputs(line, stream);
      for(ptr = line; *ptr; ptr++)
	check += *(unsigned char *)ptr;
    }
  return check;
}
/*}}}*/
