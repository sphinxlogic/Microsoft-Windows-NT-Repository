
/*  @(#)x11.c 1.27 91/11/19
 *
 *  X11 dependent graphics routines used by faces,
 *  the visual mail and print job monitor.
 * 
 *  Copyright (c) Rich Burridge - Sun Microsystems Australia.
 *                                All rights reserved.
 *
 *  Permission is given to distribute these sources, as long as the
 *  copyright messages are not removed, and no monies are exchanged. 
 * 
 *  No responsibility is taken for any errors or inaccuracies inherent
 *  either to the comments or the code of this program, but if reported
 *  to me, then an attempt will be made to fix them.
 *
 */
 
#include <stdio.h>
#include <ctype.h>
#if ( !defined(mips) && !defined(TOPIX) ) || defined(ultrix)
#include <sys/types.h>
#include <sys/time.h>
#include <sys/param.h>
#else
#ifdef SYSTYPE_BSD43
#include <sys/types.h>
#include <sys/time.h>
#else
#ifndef TOPIX
#include <bsd/sys/types.h>
#include <bsd/sys/time.h>
#include <sys/param.h>
#else
#include <sys/bsd_time.h>
#endif /*TOPIX*/
#endif /*SYSTYPE_BSD43*/
#endif /*( !defined(mips) && !defined(TOPIX) ) || defined(ultrix)*/
#include "faces.h"
#include "extern.h"

#ifdef HASPOLL
#include <poll.h>
#endif /*HASPOLL*/

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/Xresource.h>
 
#define  DEF_FONT            "fixed"

#ifdef X11R3
#define  FONT                "helvetica.r.10:"
#else
#define  FONT                "-*-helvetica-*-r-*--10-*"
#endif /*X11R3*/

#define  F_ICON              0         /* Icon index to frame array. */
#define  F_WINDOW            1         /* Window index to frame array. */
#define  FACES_BORDER_WIDTH  2
#define  FRAME_MASK          (ButtonPressMask  | ExposureMask | \
                              ButtonMotionMask | KeyPressMask)
#define  ICON_MASK            ExposureMask

Atom protocol_atom, kill_atom ;
Display *dpy ;
GC gc ;
GC tilegc ;
Pixmap faces_icon ;

/* Array of the different icon images. */
Pixmap images[MAXICONS] = {
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
} ;

Pixmap pr[3] = { NULL, NULL, NULL } ;    /* Current memory Pixmaps. */
Pixmap old_pr[2] = { NULL, NULL } ;      /* Previous memory Pixmaps. */
Window frame[2] ;
Window root ;
XClassHint class_hint = { "faces", "Faces" } ;
XFontStruct *sfont ;
int char_width ;             /* width, in pixels, of characters in the font */
int chars_per_icon ;         /* max number of characters to display / icon */
XGCValues gc_val ;
XrmDatabase faces_DB ;       /* Combined resources database. */
XSizeHints size ;
XWMHints wm_hints ;
 
#ifdef NOSELECT
#ifdef HASPOLL
struct pollfd pfd[1] ;
unsigned long npfd ;         /* Number of file descriptors to monitor. */
#endif /*HASPOLL*/
#else
#ifdef NO_43SELECT
int fullmask ;               /* Full mask of file descriptors to check on. */
int readmask ;               /* Readmask used in select call. */
#else
fd_set fullmask ;            /* Full mask of file descriptors to check on. */
fd_set readmask ;            /* Readmask used in select call. */
#endif /*NO_43SELECT*/
#endif /*NOSELECT*/

unsigned long gc_mask ;
int screen ;
int xfd ;                    /* File descriptor for X11 server connection. */
unsigned int depth ;
long backgnd, foregnd ;

static void do_pause P(()) ;
static void process_expose P((XExposeEvent *)) ;
 

void
adjust_image(dtype, itype, row, column)  /* Put new face in memory pixrect. */
enum disp_type dtype ;
enum icon_type itype ;
int row, column ;
{
  if (mtype == MONNEW)
    XCopyArea(dpy, pr[(int) dtype], pr[(int) dtype], gc, 0, 0,
              (maxcols-1)*imagewidth, imageheight, imagewidth, 0) ;

  XSetFunction(dpy, gc, GXclear) ;
  XCopyArea(dpy, pr[(int) dtype], pr[(int) dtype], gc,
                 column*imagewidth, row*imagewidth, imagewidth, imageheight,
                 column*imagewidth, row*imagewidth) ;
  XSetFunction(dpy, gc, GXcopy) ;
  XCopyArea(dpy, images[(int) itype], pr[(int) dtype], gc, 0, 0,
            iconwidth, iconwidth, column*imagewidth, row*imageheight) ;
}

 
void
beep_flash(beeps, flashes)    /* Perform visual feedback. */
int beeps, flashes ;
{
#ifdef AUDIO_SUPPORT
  if (do_audio && bell_sound != NULL) play_sound(bell_sound) ;
  else
#endif /*AUDIO_SUPPORT*/
    while (beeps > 0)
      {
        XBell(dpy, 56) ;
        do_pause() ;
        beeps-- ;
      }

  while (flashes > 0)
    {
      XSetFunction(dpy, gc, GXcopyInverted) ;
      XCopyArea(dpy, frame[F_WINDOW], frame[F_WINDOW], gc, 0, 0,
                (unsigned int) width, (unsigned int) height, 0, 0) ;
      do_pause() ;
      XCopyArea(dpy, frame[F_WINDOW], frame[F_WINDOW], gc, 0, 0,
                (unsigned int) width, (unsigned int) height, 0, 0) ;
      do_pause() ;

      XCopyArea(dpy, frame[F_ICON], frame[F_ICON], gc, 0, 0,
                (unsigned int) width, (unsigned int) height, 0, 0) ;
      do_pause() ;
      XCopyArea(dpy, frame[F_ICON], frame[F_ICON], gc, 0, 0,
                (unsigned int) width, (unsigned int) height, 0, 0) ;
      do_pause() ;
      flashes-- ;
    }
  XSetFunction(dpy, gc, GXcopy) ;
}


void
create_pixrects(width, height)   /* Create pixrects for the face images. */
int width, height ;
{
  old_pr[(int) DISP_NAME]  = pr[(int) DISP_NAME] ;
  old_pr[(int) DISP_OTHER] = pr[(int) DISP_OTHER] ;

  make_area(DISP_NAME, width, height) ;
  repl_image(DISP_NAME, CUROFF, width, height) ;

  if (toclear && mtype == MONNEW)
    {
      repl_image(DISP_OTHER, OLDOFF, width, height) ;
      toclear = 0 ;
    }
    
  make_area(DISP_OTHER, width, height) ;
  repl_image(DISP_OTHER, CUROFF, width, height) ;
 
  if (mtype == MONNEW && old_pr[(int) DISP_NAME] != NULL)
    XCopyArea(dpy, old_pr[(int) DISP_NAME], pr[(int) DISP_NAME], gc, 0, 0,
              (unsigned int) width, (unsigned int) height, 0, 0) ;
  if (mtype == MONNEW && old_pr[(int) DISP_OTHER] != NULL)
    XCopyArea(dpy, old_pr[(int) DISP_OTHER], pr[(int) DISP_OTHER], gc, 0, 0,
              (unsigned int) width, (unsigned int) height, 0, 0) ;
}


void
destroy_image(itype)
enum icon_type itype ;
{
  if (images[(int) itype] != NULL)
    {
      XFreePixmap(dpy, images[(int) itype]) ;
      images[(int) itype] = NULL ;
    }
}


static void
do_pause()
{
  static struct timeval btime = { 0, 250000 } ;   /* Beep timer. */

  XFlush(dpy) ;
#ifdef NOSELECT
  sleep(1) ;
#else
#ifdef NO_43SELECT
  SELECT(0, 0, 0, 0, &btime) ;
#else
  SELECT(0, (fd_set *) 0, (fd_set *) 0, (fd_set *) 0, &btime) ;
#endif /*NO_43SELECT*/
#endif /*NOSELECT*/
}


void
draw_screen(wdtype)
enum disp_type wdtype ;
{
  XCopyArea(dpy, pr[(int) wdtype], frame[F_WINDOW], gc, 0, 0,
            (unsigned int) width, (unsigned int) height, 0, 0) ;
  XCopyArea(dpy, pr[(int) DISP_ICON], frame[F_ICON], gc, 0, 0,
            (unsigned int) imagewidth, (unsigned int) imageheight, 0, 0) ;
}


void
drop_back()
{
  if (lowerwindow)
    {
      XLowerWindow(dpy, frame[F_WINDOW]) ;
      XLowerWindow(dpy, frame[F_ICON]) ;
    }
}


char *
get_resource(rtype)      /* Get Reve resource from merged databases. */
enum res_type rtype ;
{
  char cstr[MAXLINE], nstr[MAXLINE], str[MAXLINE] ;
  char *str_type[20] ;
  XrmValue value ;

  STRCPY(str, resources[(int) rtype]) ;
  SPRINTF(nstr,  "faces.%s", str) ;
  if (islower(str[0])) str[0] = toupper(str[0]) ;
  SPRINTF(cstr, "Faces.%s", str) ;
  if (XrmGetResource(faces_DB, nstr, cstr, str_type, &value) == NULL)
    return((char *) NULL) ;
  else return(value.addr) ;
}


void
init_font()
{
  if (fontname != NULL) sfont = XLoadQueryFont(dpy, fontname) ;
  if (sfont != NULL) return ;

  if ((sfont = XLoadQueryFont(dpy, FONT)) == NULL)
    if ((sfont = XLoadQueryFont(dpy, DEF_FONT)) == NULL)
      {
        perror("couldn't get the default font.") ;
        exit(1) ;
      }
}


/*ARGSUSED*/
void
init_graphics(argc, argv)
int *argc ;
char *argv[] ;
{
  if ((dpy = XOpenDisplay(display)) == NULL)
    {
      FPRINTF(stderr,"%s: Couldn't open display %s\n", progname,
              (getenv ("DISPLAY") ? getenv("DISPLAY") : display)) ;
      exit(1) ;
    }
  faces_DB = NULL ;
}


/*ARGSUSED*/
int
init_ws_type(argc, argv)
int argc ;
char *argv[] ;
{
  int i, j ;
  unsigned short buf[256] ;
  Colormap cmap ;
  XColor selcolor ;

  xfd = ConnectionNumber(dpy) ;

#ifdef NOSELECT
#ifdef HASPOLL
  pfd[0].fd     = xfd ;
  pfd[0].events = POLLIN ;
#endif /*HASPOLL*/
#else
#ifdef NO_43SELECT
  fullmask = 1 << xfd ;
#else
  FD_ZERO(&fullmask) ;
  FD_SET(xfd, &fullmask) ;
#endif /*NO_43SELECT*/
#endif /*NOSELECT*/

  screen = DefaultScreen(dpy) ;
  root = RootWindow(dpy, screen) ;

  if (!iconpos)
    {
      i = XParseGeometry(icon_geometry, &ix, &iy,
                         (unsigned int *) &j, (unsigned int *) &j) ;
      if (i & (XValue | YValue))
        {
          iconpos++ ;
          if (i & XNegative)
            ix = DisplayWidth(dpy, screen) + ix - imagewidth ;
          if (i & YNegative)
            iy = DisplayHeight(dpy, screen) + iy - imageheight ;
        }
    }

  cmap = DefaultColormap(dpy, screen) ;
  if (fgcolor == NULL || XDefaultDepth(dpy, screen) <= 2)
    foregnd = BlackPixel(dpy, screen) ;
  else
    {
      XParseColor(dpy, cmap, fgcolor, &selcolor) ;
      XAllocColor(dpy, cmap, &selcolor) ;
      foregnd = selcolor.pixel ;
    }
  if (bgcolor == NULL || XDefaultDepth(dpy, screen) <= 2)
    backgnd = WhitePixel(dpy, screen) ;
  else
    {
      XParseColor(dpy, cmap, bgcolor, &selcolor) ;
      XAllocColor(dpy, cmap, &selcolor) ;
      backgnd = selcolor.pixel ;
    }
  depth = DefaultDepth(dpy, screen) ;

  init_font() ;
  gc_mask = GCFont | GCForeground | GCBackground | GCGraphicsExposures ;
  gc_val.font = sfont->fid ;
  gc_val.foreground = foregnd ;
  gc_val.background = backgnd ;
  gc_val.graphics_exposures = False ;
  gc = XCreateGC(dpy, root, gc_mask, &gc_val) ;

  tilegc = XCreateGC(dpy, root, gc_mask, &gc_val) ;
  if (depth == 1) XSetFillStyle(dpy, tilegc, FillOpaqueStippled) ;
  else            XSetFillStyle(dpy, tilegc, FillTiled) ;

  for (i = 0; i < 16; i++)    /* Load default gray background. */
    {
      for (j = 0; j < 4; j++) buf[i * 16      + j] = 0x7777 ;
      for (j = 0; j < 4; j++) buf[i * 16 +  4 + j] = 0xDDDD ;
      for (j = 0; j < 4; j++) buf[i * 16 +  8 + j] = 0xBBBB ;
      for (j = 0; j < 4; j++) buf[i * 16 + 12 + j] = 0xEEEE ;
    }
 
  if (bgicon != NULL)
    if (get_x11_icon(bgicon, buf) < 0)
      GET_SUN_ICON(bgicon, buf) ;
#ifdef REVORDER
  load_icon(BACKIMAGE, buf, 1) ;
#else
  load_icon(BACKIMAGE, buf, 0) ;
#endif /*REVORDER*/

  STRCPY(fname[0], "face.xbm") ;
  STRCPY(fname[1], "sun.icon") ;
  STRCPY(fname[2], "48x48x1") ;
  maxtypes = 3 ;
  gtype = X11 ;
  pr[(int) DISP_NAME] = pr[(int) DISP_OTHER] = NULL ;
  old_pr[(int) DISP_NAME] = old_pr[(int) DISP_OTHER] = NULL ;
  return(0) ;
}


void
load_data(itype, cbuf)
enum icon_type itype ;
unsigned char *cbuf ;
{
  images[(int) itype] = XCreatePixmapFromBitmapData(dpy, root,
                                    (char *) cbuf, iconwidth, iconheight,
                                    foregnd, backgnd, depth) ;
}


/*ARGSUSED*/
void
load_icon(itype, sbuf, not_flipped)
enum icon_type itype ;
unsigned short sbuf[256] ;
int not_flipped ;
{
  char cbuf[512] ;
  int i ;

  for (i = 0; i < 256; i++)
    {
      cbuf[i*2+0] = revtable[(sbuf[i] >> 8) & 0xFF] ;
      cbuf[i*2+1] = revtable[sbuf[i] & 0xFF] ;
    }
  images[(int) itype] = XCreatePixmapFromBitmapData(dpy, root,
                                           cbuf, iconwidth, iconheight,
                                           foregnd, backgnd, depth) ;
}


/*  Get the resource databases. These are looked for in the following ways:
 *
 *  Classname file in the app-defaults directory. In this case, Classname
 *  is Faces.
 *
 *  Classname file in the directory specified by the XUSERFILESEARCHPATH
 *  or XAPPLRESDIR environment variable.
 *
 *  Property set using xrdb, accessible through the XResourceManagerString
 *  macro or, if that is empty, the ~/.Xdefaults file.
 *
 *  XENVIRONMENT environment variable or, if not set, .Xdefaults-hostname
 *  file.
 *
 *  FACESDEFAULTS environment variable.
 */

void
load_resources()
{
  XrmDatabase db ;
  char *home, name[MAXPATHLEN], *ptr ;
  int len ;

  home = getenv("HOME") ;
  XrmInitialize() ;
  STRCPY(name, "/usr/lib/X11/app-defaults/Faces") ;
 
/* Get applications defaults file, if any. */
 
  db = XrmGetFileDatabase(name) ;
  XrmMergeDatabases(db, &faces_DB) ;
 
/* Merge server defaults, created by xrdb. If nor defined, use ~/.Xdefaults. */ 
#ifndef X11R3
  if (XResourceManagerString(dpy) != NULL)
    db = XrmGetStringDatabase(XResourceManagerString(dpy)) ;
  else
#endif /*X11R3*/
    {
      SPRINTF(name, "%s/.Xdefaults", home) ;
      db = XrmGetFileDatabase(name) ;
    }
  XrmMergeDatabases(db, &faces_DB) ;
 
/*  Open XENVIRONMENT file or, if not defined, the .Xdefaults, and merge
 *  into existing database.
 */
 
  if ((ptr = getenv("XENVIRONMENT")) == NULL)
    {
      SPRINTF(name, "%s/.Xdefaults-", home) ;
      len = strlen(name) ;
      GETHOSTNAME(name+len, MAXPATHLEN-len) ;
      db = XrmGetFileDatabase(name) ;
    }
  else db = XrmGetFileDatabase(ptr) ;
  XrmMergeDatabases(db, &faces_DB) ;

/* Finally merge in Faces defaults via FACESDEFAULTS (if defined). */

  if ((ptr = getenv("FACESDEFAULTS")) != NULL)
    {
      db = XrmGetFileDatabase(ptr) ;
      XrmMergeDatabases(db, &faces_DB) ;
    }
}


void
make_area(dtype, width, height)
enum disp_type dtype ;
int width, height ;
{
  pr[(int) dtype] = XCreatePixmap(dpy, root,
                      (unsigned int) width, (unsigned int) height, depth) ;
  XSetFunction(dpy, gc, GXclear) ;
  XCopyArea(dpy, pr[(int) dtype], pr[(int) dtype], gc,
                 column*imagewidth, row*imagewidth, imagewidth, imageheight,
                 column*imagewidth, row*imagewidth) ;
  XSetFunction(dpy, gc, GXcopy) ;
}


/*ARGSUSED*/
void
make_frame(argc, argv)
int argc ;
char *argv[] ;
{
  unsigned int h, w ;                /* Window dimensions. */
  int flags ;
  int x, y ;                         /* Window position. */
  XSetWindowAttributes winattrs ;
#ifndef X11R3
  XTextProperty textprop ;
#endif /*!X11R3*/
  char hostname[MAXLINE] ;

  w = width = maxcols * imagewidth ;
  h = height = imageheight ;

  size.flags = PMinSize | PMaxSize | PPosition | PSize ;
  size.x = 0 ;
  size.y = 0 ;
  size.min_width = imagewidth ;
  size.min_height = imageheight ;
  size.max_width = size.width = width ;
  if (mtype == MONNEW) size.max_height = imageheight ;
  else                 size.max_height = 900 ;
  size.height = height ;

  if (geometry != NULL)
    {
      flags = XParseGeometry(geometry, &x, &y, &w, &h) ;
      if (XValue & flags)
        {
          if (XNegative & flags)
            x = DisplayWidth(dpy, screen) + x - size.width ;
            size.flags |= USPosition ;
            size.x = x ;
        }
      if (YValue & flags)
        {
          if (YNegative & flags)
            y = DisplayHeight(dpy, screen) + y - size.height ;
            size.flags |= USPosition ;
            size.y = y ;
        }
    }

  if (!iconpos)
    {
      ix = size.x + size.width - imagewidth ;
      iy = size.y ;
    }

  CLEARMEM((char *) &winattrs, sizeof(winattrs)) ;
  winattrs.background_pixel = backgnd ;
  winattrs.border_pixel = foregnd ;
  winattrs.event_mask = FRAME_MASK ;

  frame[F_WINDOW] = XCreateWindow(dpy, root, size.x, size.y,
                        size.width, size.height, FACES_BORDER_WIDTH,
                        CopyFromParent, InputOutput, CopyFromParent,
                        CWBackPixel | CWBorderPixel | CWEventMask, &winattrs) ;

  winattrs.event_mask = ICON_MASK ;

  frame[F_ICON] = XCreateWindow(dpy, root,
                        ix, iy, imagewidth, imageheight, FACES_BORDER_WIDTH,
                        CopyFromParent, InputOutput, CopyFromParent, 
                        CWBackPixel | CWBorderPixel | CWEventMask, &winattrs) ;

  protocol_atom = XInternAtom(dpy, "WM_PROTOCOLS", False) ;
  kill_atom = XInternAtom(dpy, "WM_DELETE_WINDOW", False) ;

#ifndef X11R3
  XSetWMProtocols(dpy, frame[F_WINDOW], &kill_atom, 1) ;
#endif /*!X11R3*/

  XSetStandardProperties(dpy, frame[F_WINDOW], faces_label, faces_label, None,
                         argv, argc, &size) ;
 
  wm_hints.icon_x = ix ;
  wm_hints.icon_y = iy ;
  wm_hints.input = True ;
  wm_hints.icon_window = frame[F_ICON] ;
  wm_hints.flags = InputHint | IconWindowHint ;
  if (iconpos) wm_hints.flags |= IconPositionHint ;
  if (iconic)
    {    
      wm_hints.initial_state = IconicState ;
      wm_hints.flags |= StateHint ;
    }    
  XSetWMHints(dpy, frame[F_WINDOW], &wm_hints) ;

/* Set XA_WM_CLASS so things that depend on NAME work. */

  XSetClassHint(dpy, frame[F_WINDOW], &class_hint) ;

#ifndef X11R3
  GETHOSTNAME(hostname, MAXLINE) ;
  textprop.value = (unsigned char *) hostname ;
  textprop.encoding = XA_STRING ;
  textprop.format = 8 ;
  textprop.nitems = strlen(hostname) ;
  XSetWMClientMachine(dpy, frame[F_WINDOW], &textprop) ;
#endif /*!X11R3*/

/* Equivalent of make_icon. */

  if (mtype == MONPRINTER) adjust_image(DISP_ICON, NOPRINT, 0, 0) ;
  else                     adjust_image(DISP_ICON, NOMAIL,  0, 0) ;
}
 

void
make_icon()         /* Null routine. */
{}


static void
process_expose(event)
XExposeEvent *event ;
{
  int doframe, doicon ;

  doframe = doicon = 0 ;
  do
    {
           if (event->window == frame[F_WINDOW]) doframe++ ;
      else if (event->window == frame[F_ICON])   doicon++ ;
    }    
  while (XCheckMaskEvent(dpy, ExposureMask, (XEvent *) event)) ;

  if (doframe) draw_screen(wdtype) ;
  if (doicon)  draw_screen(wdtype) ;
}


void
repl_image(dtype, dest, width, height)
enum disp_type dtype ;
enum image_type dest ;
int width, height ;
{
  Pixmap dpm ;

  if (dest == CUROFF) dpm = pr[(int) dtype] ;
  else dpm = old_pr[(int) dtype] ;
  if (depth == 1) XSetStipple(dpy, tilegc, images[(int) BACKIMAGE]) ;
  else XSetTile(dpy, tilegc, images[(int) BACKIMAGE]) ;
  XFillRectangle(dpy, dpm, tilegc, 0, 0, width, height) ;
}


void
save_cmdline(line)       /* Null routine. */
char *line ;
{
}


void
show_display(beep_and_raise)    /* Show the latest set of mail icon faces. */
int beep_and_raise ;
{
  static int first_time = 1 ;

  if (beep_and_raise && raisewindow && newmail && !mboxshrunk)
    {
      XRaiseWindow(dpy, frame[F_WINDOW]) ;
      XRaiseWindow(dpy, frame[F_ICON]) ;
    }

  if (invert)     /* Invert the memory pixrects before displaying. */
    {
      XSetFunction(dpy, gc, GXcopyInverted) ;
      XCopyArea(dpy, pr[(int) DISP_NAME], pr[(int) DISP_NAME], gc, 0, 0,
                (unsigned int) width, (unsigned int) height, 0, 0) ;
      XCopyArea(dpy, pr[(int) DISP_OTHER], pr[(int) DISP_OTHER], gc, 0, 0,
                (unsigned int)  width, (unsigned int) height, 0, 0) ;
    }
  if (first_time)
    {
      XSelectInput(dpy, frame[F_WINDOW], FRAME_MASK) ;
      XMapWindow(dpy, frame[F_WINDOW]) ;
      first_time = 0 ;
    }

  XResizeWindow(dpy, frame[F_WINDOW],
                (unsigned int) width, (unsigned int) height) ;
  draw_screen(wdtype) ;
 
  if (beep_and_raise && newmail && !mboxshrunk) beep_flash(beeps, flashes) ;
  if (old_pr[(int) DISP_NAME])  XFreePixmap(dpy, old_pr[(int) DISP_NAME]) ;
  if (old_pr[(int) DISP_OTHER]) XFreePixmap(dpy, old_pr[(int) DISP_OTHER]) ;
  old_pr[(int) DISP_NAME] = NULL ;
  old_pr[(int) DISP_OTHER] = NULL ;
  XFlush(dpy) ;
}
 

void
start_tool()
{
  struct timeval tval ;          /* To set checking period. */
  XEvent event ;                 /* For events received. */
  XClientMessageEvent *ev ;
  XKeyPressedEvent *key_event ;
  char chs[2] ;
 
  tval.tv_usec = 0 ;
  tval.tv_sec = period ;

  for (;;)
    {
      if (!XPending(dpy))
        {
#ifdef NOSELECT
#ifdef HASPOLL
          POLL(pfd, 1L, period * 1000) ;
#else
          {
            int contr = 0 ;

            sleep(1) ;
            while ((XPending(dpy) == 0) && (contr < period))
              {
                sleep(3) ;
                contr += 3 ;
              }
          }
#endif /*HASPOLL*/
#else
          readmask = fullmask ;
#ifdef NO_43SELECT
          SELECT(32, &readmask, 0, 0, &tval) ;
#else
          SELECT(FD_SETSIZE, &readmask, (fd_set *) 0, (fd_set *) 0, &tval) ;
#endif /*NO_43SELECT*/
#endif /*NOSELECT*/

          if (!XPending(dpy))
            do_check() ;     /* Check the mail/printer/user queue again. */
        }
        else
          {
            if (!XCheckMaskEvent(dpy, ExposureMask, &event))
              XNextEvent(dpy, &event) ;

/* ClientMessage: (catch ICCCM kill from WM). */

            if (event.type == ClientMessage)
              {
                ev = (XClientMessageEvent *) &event ;
                if (ev->message_type == protocol_atom &&
                  ev->data.l[0] == kill_atom)
                exit(0) ;
              }

/* Expose. */

            else if (event.type == Expose)
              process_expose((XExposeEvent *) &event) ;

/* ButtonPress. */

            else if (event.type == ButtonPress)
              {
                if (button1clear && event.xbutton.button == Button1)
                  do_key(KEY_CLEAR) ;
                else if ((button1clear && event.xbutton.button == Button2) ||
                        (!button1clear && event.xbutton.button == Button1))
                  {
                    wdtype = (wdtype == DISP_NAME) ? DISP_OTHER : DISP_NAME ;
                    draw_screen(wdtype) ;
                  }
              }

/* KeyPress. */

            else if (event.type == KeyPress)
              {
                key_event = (XKeyPressedEvent *) &event ;
                (void) XLookupString(key_event, chs, 1, (KeySym *) NULL,
                                     (XComposeStatus *) NULL) ;
                do_key(chs[0]) ;
              }
          }
    }
}
 

void
text(dtype, jtype, str)
enum disp_type dtype ;
enum just_type jtype ;
char *str ;
{
  int len ;
  int c, r ;         /* Column and row position for this face. */
  int x, y ;         /* Position of start of this text string. */
  int textwidth ;
 
  c = column ;
  r = row ;
  switch (dtype)
    {
      case DISP_ALL    : text(DISP_ICON,  jtype, str) ;
      case DISP_BOTH   : text(DISP_NAME,  jtype, str) ;
                         text(DISP_OTHER, jtype, str) ;
                         return ;
      case DISP_ICON   : c = r = 0 ;
    }
 
  if ((len = strlen(str)) > 20) len = 20 ;    /* Character length of text. */

  do
    {
      int dummy ;
      XCharStruct overall ;

      XTextExtents(sfont, str, len, &dummy, &dummy, &dummy, &overall) ;
      textwidth = overall.width ;
      if (textwidth < imagewidth) break ;
      len-- ;
    }
  while (len > 5) ;

  switch (jtype)
    {
      case LEFT  : x =  c      * imagewidth + 2 ;
                   y = (r + 1) * imageheight - 5 ;
                   break ;
      case RIGHT : x = (c + 1) * imagewidth - textwidth - 2 ;
                   y = (r + 1) * imageheight - 5 ;
    }
  XSetFunction(dpy, gc, GXandInverted) ;
  XFillRectangle(dpy, pr[(int) dtype], gc, x, y-9,
                 (unsigned int) textwidth+2, 13) ;
  XSetFunction(dpy, gc, GXxor) ;
  XDrawImageString(dpy, pr[(int) dtype], gc, x, y, str, len) ;
  XSetFunction(dpy, gc, GXcopy) ;
}
