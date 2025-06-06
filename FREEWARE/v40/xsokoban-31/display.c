#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sys/limits.h>

#include "externs.h"
#include "globals.h"
#include "defaults.h"
#include "help.h"

#if USE_XPM
#include <xpm.h>
#endif

/* mnemonic defines to help orient some of the text/line drawing, sizes */
#define HELPLINE ((bit_height * MAXROW) + 30)
#define STATUSLINE ((bit_height * MAXROW) + 5)
#define HELP_H (bit_height * MAXROW)
#define HELP_W (bit_width * MAXCOL)

/* local to this file */
static Window win;
static GC gc, rgc, drgc;
static unsigned int width, height, depth;
static XFontStruct *finfo;
static Boolean optwalls;
static Cursor this_curs;
static Pixmap help[HELP_PAGES], floor;
static Pixmap blank, work, man, saveman, goal, object,
       treasure, walls[NUM_WALLS];
static Boolean font_alloc = False, gc_alloc = False,
        pix_alloc = False;
static int hlpscrn = -1;
static char buf[500];
static Boolean win_alloc = _false_;

/* globals */
Display *dpy;
int scr;
unsigned bit_width, bit_height;
Atom wm_delete_window, wm_protocols;
Boolean display_alloc = False;
Colormap cmap;

/* names of the fancy wall bitmap files.  If you define a set of fancy
 * wall bitmaps, they must use these names
 */
static char *wallname[] = {
 "lonewall", "southwall", "westwall", "llcornerwall",
 "northwall", "vertiwall", "ulcornerwall", "west_twall",
 "eastwall", "lrcornerwall", "horizwall", "south_twall",
 "urcornerwall", "east_twall", "north_twall", "centerwall"
};

/* Do all the nasty X stuff like making the windows, setting all the defaults,
 * creating all the pixmaps, loading everything, and mapping the window.
 * This does NOT do the XOpenDisplay(), so that the -display switch can be
 * handled cleanly.
 */
short InitX(void)
{
  int i;
  Boolean reverse = _false_, tmpwalls = _false_;
  char *rval;
  unsigned long fore, back, bord, curs, gc_mask;
  XSizeHints szh;
  XWMHints wmh;
  XSetWindowAttributes wattr;
  XClassHint clh;
  XGCValues val, reval;
  XTextProperty wname, iname;
  XColor cfg, cbg;
  Atom protocols[1];

  /* these are always needed */
  scr = DefaultScreen(dpy);
  cmap = DefaultColormap(dpy, scr);
  depth = DefaultDepth(dpy, scr);

  /* here is where we figure out the resources and set the defaults.
   * resources can be either on the command line, or in your .Xdefaults/
   * .Xresources files.  They are read in and parsed in main.c, but used
   * here.
   */
  finfo = GetFontResource(FONT);
  if (!finfo) return E_NOFONT;
  font_alloc = _true_;

  rval = GetResource(REVERSE);
  if(rval != (char *)0) {
    reverse = StringToBoolean(rval);
  }

  if(!GetColorResource(FOREG, &fore))
    fore = BlackPixel(dpy, scr);

  if(!GetColorResource(BACKG, &back))
    back = WhitePixel(dpy, scr);

  if(reverse) {
    unsigned long t;
    t = fore;
    fore = back;
    back = t;
  }

  if(!GetColorResource(BORDER, &bord))
    bord = fore;
  if(!GetColorResource(CURSOR, &curs))
    curs = fore;

  bitpath = GetResource(BITDIR);
  rval = GetResource(WALLS);
  if(rval != (char *)0)
    tmpwalls = StringToBoolean(rval);

  /* Walls are funny.  If a alternate bitpath has been defined, assume
   * !fancywalls unless explicitly told fancy walls.  If the default 
   * bitpath is being used, you can assume fancy walls.
   */
  if(bitpath && !tmpwalls)
    optwalls = _false_;
  else
    optwalls = _true_;

  width = MAXCOL * DEF_BITW;
  height = MAXROW * DEF_BITH + 50;

  wmh.initial_state = NormalState;
  wmh.input = True;
  wmh.flags = (StateHint | InputHint);

  clh.res_class = clh.res_name = progname;

  /* Make sure the window and icon names are set */
  if(!XStringListToTextProperty(&progname, 1, &wname))
    return E_NOMEM;
  if(!XStringListToTextProperty(&progname, 1, &iname))
    return E_NOMEM;

  /* load in a cursor, and recolor it so it looks pretty */
  this_curs = XCreateFontCursor(dpy, DEF_CURSOR);
  cfg.pixel = curs;
  cbg.pixel = back;
  XQueryColor(dpy, cmap, &cfg);
  XQueryColor(dpy, cmap, &cbg);
  XRecolorCursor(dpy, this_curs, &cfg, &cbg);

  /* set up the funky little window attributes */
  wattr.background_pixel = back;
  wattr.border_pixel = bord;
  wattr.backing_store = Always;
  wattr.event_mask = (KeyPressMask | ExposureMask | ButtonPressMask |
		      ButtonReleaseMask);
  wattr.cursor = this_curs;

  /* Create the window. we create it with NO size so that we
   * can load in the bitmaps; we later resize it correctly.
   */
  win = XCreateWindow(dpy, RootWindow(dpy, scr), 0, 0, width, height, 4,
		      CopyFromParent, InputOutput, CopyFromParent,
                      (CWBackPixel | CWBorderPixel | CWBackingStore |
                       CWEventMask | CWCursor), &wattr);
  win_alloc = _true_;

  /* this will set the bit_width and bit_height as well as loading
   * in the pretty little bitmaps
   */
  if(LoadBitmaps() == E_NOBITMAP)
    return E_NOBITMAP;
  blank = XCreatePixmap(dpy, win, bit_width, bit_height, 1);
  pix_alloc = _true_;

  width = MAXCOL * bit_width;
  height = MAXROW * bit_height + 50;
  
  /* whee, resize the window with the correct size now that we know it */
  XResizeWindow(dpy, win, width, height);

  /* set up the size hints, we don't want manual resizing allowed. */
  szh.min_width = szh.width = szh.max_width = width;
  szh.min_height = szh.height = szh.max_height = height;
  szh.x = szh.y = 0;
  szh.flags = (PSize | PPosition | PMinSize | PMaxSize);

  /* now SET all those hints we create above */
  XSetWMNormalHints(dpy, win, &szh);
  XSetWMHints(dpy, win, &wmh);
  XSetClassHint(dpy, win, &clh);
  XSetWMName(dpy, win, &wname);
  XSetWMIconName(dpy, win, &iname);

  /* Turn on WM_DELETE_WINDOW */
  wm_delete_window = XInternAtom(dpy, "WM_DELETE_WINDOW", 0);
  wm_protocols = XInternAtom(dpy, "WM_PROTOCOLS", 0);
  protocols[0] = wm_delete_window;
  XSetWMProtocols(dpy, win, protocols, 1);

  work = XCreatePixmap(dpy, win, width, height, depth);

  /* set up all the relevant GC's */
  val.foreground = reval.background = fore;
  val.background = reval.foreground = back;
  val.function = reval.function = GXcopy;
  val.font = reval.font = finfo->fid;
  gc_mask = (GCForeground | GCBackground | GCFunction | GCFont);
  gc = XCreateGC(dpy, work, gc_mask, &val);
  rgc = XCreateGC(dpy, blank, gc_mask, &reval);
  drgc = XCreateGC(dpy, work, gc_mask, &reval);

  /* make the help windows and the working bitmaps */
  /* we need to do this down here since it requires GCs to be allocated */
  for(i = 0; i < HELP_PAGES; i++)
    help[i] = XCreatePixmap(dpy, win, HELP_W, HELP_H, depth);
  MakeHelpWindows();
  XFillRectangle(dpy, blank, rgc, 0, 0, bit_width, bit_height);

  gc_alloc = _true_;

  /* display the friendly little clear screen */
  ClearScreen();
  XMapWindow(dpy, win);
  RedisplayScreen();
  
  return 0;
}

/* deallocate all the memory and structures used in creating stuff */
void DestroyDisplay(void)
{
  int i;

  if (!display_alloc) return;

  /* kill the font */
  if(font_alloc)
    XFreeFont(dpy, finfo);

  /* destroy everything allocted right around the gcs.  Help windows are
   * freed here cause they are created about the same time.  (Yes, I know
   * this could cause problems, it hasn't yet.
   */
  if(gc_alloc) {
    XFreeGC(dpy, gc);
    XFreeGC(dpy, rgc);
    XFreeGC(dpy, drgc);
    XFreePixmap(dpy, work);
    for (i = 0; i < HELP_PAGES; i++)
      XFreePixmap(dpy, help[i]);
  }
  /* free up all the allocated pix */
  if(pix_alloc) {
    XFreePixmap(dpy, man);
    XFreePixmap(dpy, saveman);
    XFreePixmap(dpy, goal);
    XFreePixmap(dpy, treasure);
    XFreePixmap(dpy, object);
    XFreePixmap(dpy, floor);
    XFreePixmap(dpy, blank);
    for(i = 0; i < NUM_WALLS; i++)
      if(i == 0 || optwalls)
        XFreePixmap(dpy, walls[i]);
  }
  /* okay.. NOW we can destroy the main window and the display */
    if (win_alloc) XDestroyWindow(dpy, win);
    XCloseDisplay(dpy);
}

static Boolean full_pixmap[256];

static Boolean TryBitmapFile(char *template, Pixmap *pix, char *bitpath,
			     char *fname, int map)
{
  unsigned int width, height;
  int dum3, dum4;
  sprintf(buf, template, bitpath, fname);

  if(XReadBitmapFile(dpy, win, buf, &width, &height, pix, &dum3, &dum4) ==
       BitmapSuccess) {
       if (width > bit_width) bit_width = width;
       if (height > bit_height) bit_height = height;
       full_pixmap[map] = _false_;
       return _true_;
  }
  return _false_;
}

#if USE_XPM
/*
   Try to load an XPM file. Return "_true_" if success, "_false_" otherwise.
   Print an error message to stderr if an invalid XPM file was found.
*/
static Boolean TryPixmapFile(char *template, Pixmap *pix, char *bitpath,
                             char *fname, int map)
{
    int ret;
    XpmAttributes attr;
    XWindowAttributes wa;
    if (!XGetWindowAttributes(dpy, win, &wa)) {
	fprintf(stderr, "What? Can't get attributes of window\n");
	abort();
    }
    if (wa.depth < 8) {
	/* Hopeless! Not enough colors...*/
	return _false_;
    }

    attr.valuemask = XpmCloseness | XpmExactColors | XpmColorKey | XpmColormap |
		    XpmDepth;
    attr.colormap = wa.colormap;
    attr.depth = wa.depth;
    attr.color_key = XPM_COLOR;
    attr.closeness = 2;
    attr.exactColors = _false_;
    sprintf(buf, template, bitpath, fname);
    if ((ret = XpmReadFileToPixmap(dpy, win, buf, pix, NULL, &attr)) ==
	 XpmSuccess) {
	
	if (attr.width > bit_width) bit_width = attr.width;
	if (attr.height > bit_height) bit_height = attr.height;
	full_pixmap[map] = _true_;
	return _true_;
    }
    if (ret != XpmOpenFailed) {
	char *errmsg;
	switch(ret) {
	    case 1: errmsg = "XPM: color error"; break;
	    case 0: errmsg = "XPM: success"; break;
	    case -1: errmsg = "XPM: open failed"; break;
	    case -2: errmsg = "XPM: file format invalid"; break;
	    case -3: errmsg = "XPM: No memory"; break;
	    case -4: errmsg = "XPM: color failed"; break;
	    default: errmsg = "Unknown error code from XPM"; break;
	}
	fprintf(stderr, "XpmReadFileToPixmap (%s) failed, %s\n", buf, errmsg);
    }
    return _false_;
}
#endif

/* Load in a single bitmap.  If this bitmap is the largest in the x or
 * y direction, set bit_width or bit_height appropriately.  If your pixmaps
 * are of varying sizes, a bit_width by bit_height box is guaranteed to be
 * able to surround all of them.
 */
Boolean LoadOneBitmap(char *fname, char *altname, Pixmap *pix, int map)
{
  if(bitpath && *bitpath) {
    /* we have something to try other than the default, let's do it */
#if USE_XPM
    if (TryPixmapFile("%s/%s.xpm", pix, bitpath, fname, map)) return _true_;
    if (TryPixmapFile("%s/%s.xpm", pix, bitpath, altname, map)) return _true_;
#endif
    if (TryBitmapFile("%s/%s.xbm", pix, bitpath, fname, map)) return _true_;
    if (TryBitmapFile("%s/%s.xbm", pix, bitpath, altname, map)) return _true_;
    return _false_;
  }

#if USE_XPM
#ifdef VMS
  if (TryPixmapFile("%s:%s.xpm", pix, BITPATH, fname, map)) return _true_;
  if (TryPixmapFile("%s:%s.xpm", pix, BITPATH, altname, map)) return _true_;
#else
  if (TryPixmapFile("%s/%s.xpm", pix, BITPATH, fname, map)) return _true_;
  if (TryPixmapFile("%s/%s.xpm", pix, BITPATH, altname, map)) return _true_;
#endif /* VMS */
#endif

#ifdef VMS
  if (TryBitmapFile("%s:%s.xbm", pix, BITPATH, fname, map)) return _true_;
  if (TryBitmapFile("%s:%s.xbm", pix, BITPATH, altname, map)) return _true_;
#else
  if (TryBitmapFile("%s/%s.xbm", pix, BITPATH, fname, map)) return _true_;
  if (TryBitmapFile("%s/%s.xbm", pix, BITPATH, altname, map)) return _true_;
#endif
  return _false_;
}

/* loads all the bitmaps in.. if any fail, it returns E_NOBITMAP up a level
 * so the program can report the error to the user.  It tries to load in the
 * alternates as well.
 */
short LoadBitmaps(void)
{
  register int i;

  if(!LoadOneBitmap("man", NULL, &man, player)) return E_NOBITMAP;
  if(!LoadOneBitmap("saveman", "man", &saveman, playerstore)) return E_NOBITMAP;
  if(!LoadOneBitmap("object", NULL, &object, packet)) return E_NOBITMAP;
  if(!LoadOneBitmap("treasure", NULL, &treasure, save)) return E_NOBITMAP;
  if(!LoadOneBitmap("goal", NULL, &goal, store)) return E_NOBITMAP;
  if(!LoadOneBitmap("floor", NULL, &floor, ground)) return E_NOBITMAP;

  if(optwalls) {
    for(i = 0; i < NUM_WALLS; i++) {
      if(!LoadOneBitmap(wallname[i], "wall", &walls[i], wall))
		return E_NOBITMAP;
    }
  } else {
    if(!LoadOneBitmap("wall", NULL, &walls[0], wall)) return E_NOBITMAP;
  }
  return 0;
}

static void DrawPixmap(Drawable w, Pixmap p, int mapchar, int x, int y)
{
  if (full_pixmap[mapchar])
      XCopyArea(dpy, p, w, gc, 0, 0, bit_width, bit_height, x, y);
  else
      XCopyPlane(dpy, p, w, gc, 0, 0, bit_width, bit_height, x, y, 1);
}

/* Create and draw all the help windows.  This is not wholly foolproof with
 * the variable-size bitmap code yet, as the constants to place things on the
 * screen, are just that, constants.  This should be rewritten.
 */
void MakeHelpWindows(void)
{
  int i;
  int ypos = 0;
  char *title = "    Sokoban  --  X version 3.1 --  Help page %d";
  char *next = "     Press <Return> to exit";

  for(i = 0; i < HELP_PAGES; i++) {
    XFillRectangle(dpy, help[i], drgc, 0, 0, HELP_W, HELP_H);
    sprintf(buf, title, (i+1));
    XDrawImageString(dpy, help[i], gc, 0, 11, buf, strlen(buf));
    XDrawLine(dpy, help[i], gc, 0, 17, HELP_W, 17);
    XDrawLine(dpy, help[i], gc, 0, HELP_H-20, HELP_W, HELP_H-20);
    XDrawImageString(dpy, help[i], gc, 2, HELP_H-7, next, strlen(next));
  }
  for(i = 0; help_pages[i].textline != NULL; i++) {
    ypos += help_pages[i].ydelta;
    XDrawImageString(dpy,help[help_pages[i].page], gc,
                     help_pages[i].xpos * (finfo->max_bounds.width),
                     ypos, help_pages[i].textline,
                     strlen(help_pages[i].textline));
  }

  DrawPixmap(help[0], man, player, 180, 340);
  DrawPixmap(help[0], goal, store, 280, 340);
  DrawPixmap(help[0], walls[0], wall, 389, 340);
  DrawPixmap(help[0], object, packet, 507, 340);
  DrawPixmap(help[0], treasure, save, 270, 388);
  DrawPixmap(help[0], saveman, playerstore, 507, 388);
}

/* wipe out the entire contents of the screen */
void ClearScreen(void)
{
  register int i,j;

  XFillRectangle(dpy, work, drgc, 0, 0, width, height);
  for(i = 0; i < MAXROW; i++)
    for(j = 0; j < MAXCOL; j++)
      DrawPixmap(work, floor, ground, j*bit_width, i*bit_height);
  XDrawLine(dpy, work, gc, 0, bit_height*MAXROW, bit_width*MAXCOL,
            bit_height*MAXROW);
}

/* redisplay the current screen.. Has to handle the help screens if one
 * is currently active..  Copys the correct bitmaps onto the window.
 */
void RedisplayScreen(void)
{
  if(hlpscrn == -1)
    XCopyArea(dpy, work, win, gc, 0, 0, width, height, 0, 0);
  else
    XCopyArea(dpy, help[hlpscrn], win, gc, 0, 0, HELP_W, HELP_H, 0, 0);
  XFlush(dpy);
}

/* Flush all X events to the screen and wait for them to get there. */
void SyncScreen(void)
{
  XSync(dpy, 0);
}

/* Draws all the neat little pictures and text onto the working pixmap
 * so that RedisplayScreen is happy.
 */
void ShowScreen(void)
{
  register int i,j;

  for(i = 0; i < rows; i++)
    for(j = 0; j < cols && map[i][j] != 0; j++)
      MapChar(map[i][j], i, j, 0);
  DisplayLevel();
  DisplayPackets();
  DisplaySave();
  DisplayMoves();
  DisplayPushes();
  DisplayHelp();
  RedisplayScreen();
}

/* Draws a single pixmap, translating from the character map to the pixmap
 * rendition. If "copy_area", also push the change through to the actual window.
 */
void MapChar(char c, int i, int j, Boolean copy_area)
{
  Pixmap this;

  this = GetObjectPixmap(i, j, c); /* i, j are passed so walls can be done */
  if (full_pixmap[(int)c])
  XCopyArea(dpy, this, work, gc, 0, 0, bit_width, bit_height, cX(j), cY(i));
  else
  XCopyPlane(dpy, this, work, gc, 0, 0, bit_width, bit_height, cX(j), cY(i), 1);
  if (copy_area) {
    XCopyArea(dpy, work, win, gc, cX(j), cY(i), bit_width, bit_height,
	      cX(j), cY(i));
  }
}

/* figures out the appropriate pixmap from the internal game representation.
 * Handles fancy walls.
 */
Pixmap GetObjectPixmap(int i, int j, char c)
{
  switch(c) {
    case player: return man;
    case playerstore: return saveman;
    case store: return goal;
    case save: return treasure;
    case packet: return object;
    case wall:
       if(optwalls) return walls[PickWall(i,j)];
       else return walls[0];
    case ground: return floor;
    default: return blank;
  }
}

/* returns and index into the fancy walls array. works by assigning a value
 * to each 'position'.. the type of fancy wall is computed based on how
 * many neighboring walls there are.
 */
int PickWall(int i, int j)
{
  int ret = 0;

  if(i > 0 && map[i-1][j] == wall) ret += 1;
  if(j < cols && map[i][j+1] == wall) ret += 2;
  if(i < rows && map[i+1][j] == wall) ret += 4;
  if(j > 0 && map[i][j-1] == wall) ret += 8;
  return ret;
}

/* Draws a string onto the working pixmap */
void DrawString(int x, int y, char *text)
{
  int x_off, y_off;

  x_off = x * finfo->max_bounds.width;
  y_off = y + finfo->ascent;

  XDrawImageString(dpy, work, gc, x_off, y_off, text, strlen(text));
}

/* The following routines display various 'statusline' stuff (ie moves, pushes,
 * etc) on the screen.  they are called as they are needed to be changed to
 * avoid unnecessary drawing */
void DisplayLevel(void)
{
   sprintf(buf, "Level: %3d", level);
   DrawString(0, STATUSLINE, buf);
}

void DisplayPackets(void)
{
   sprintf(buf, "Packets: %3d", packets);
   DrawString(12, STATUSLINE, buf);
}

void DisplaySave(void)
{
  sprintf(buf, "Saved: %3d", savepack);
  DrawString(26, STATUSLINE, buf);
}

void DisplayMoves(void)
{
  sprintf(buf, "Moves: %5d", moves);
  DrawString(38, STATUSLINE, buf);
}

void DisplayPushes(void)
{
  sprintf(buf, "Pushes: %3d", pushes);
  DrawString(52, STATUSLINE, buf);
}

void DisplayHelp(void)
{
  DrawString(0, HELPLINE, "Press ? for help.");
}

/* Function used by the help pager.  We ONLY want to flip pages if a key
 * key is pressed.. We want to exit the help pager if ENTER is pressed.
 * As above, <shift> and <control> and other such fun keys are NOT counted
 * as a keypress.
 */
Boolean WaitForEnter(void)
{
  KeySym keyhit;
  char buf[1];
  int bufs = 1;
  XComposeStatus compose;
  XEvent xev;

  while (1) {
    XNextEvent(dpy, &xev);
    switch(xev.type) {
      case Expose:
	RedisplayScreen();
	break;
      case KeyPress:
	buf[0] = '\0';
	XLookupString(&xev.xkey, buf, bufs, &keyhit, &compose);
	if(buf[0]) {
	  return (keyhit == XK_Return) ? _true_ : _false_;
	}
	break;
      default:
	break;
    }
  }
}

/* Displays the first help page, and flips help pages (one per key press)
 * until a return is pressed.
 */
void ShowHelp(void)
{
  int i = 0;
  Boolean done = _false_;

  hlpscrn = 0;
  XCopyArea(dpy, help[i], win, gc, 0, 0, HELP_W, HELP_H, 0, 0);
  XFlush(dpy);
  while(!done) {
    done = WaitForEnter();
    if(done) {
      hlpscrn = -1;
      return;
    } else {
      i = (i+1)%HELP_PAGES;
      hlpscrn = i;
      XCopyArea(dpy, help[i], win, gc, 0, 0, HELP_W, HELP_H, 0, 0);
      XFlush(dpy);
    }
  }
}

short DisplayScores()
{
    return DisplayScores_(dpy, win);
}


/* since the 'press ? for help' is ALWAYS displayed, just beep when there is
 * a problem.
 */
void HelpMessage(void)
{
  XBell(dpy, 0);
  RedisplayScreen();
}
