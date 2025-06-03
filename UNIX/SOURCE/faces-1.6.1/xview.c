
/*  @(#)xview.c 1.18 91/09/05
 *
 *  XView dependent graphics routines used by faces,
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
 */

#include <stdio.h>
#include <ctype.h>
#include "faces.h"
#include "extern.h"
#include <sys/param.h>
#include <xview/xview.h>
#include <xview/panel.h>
#include <xview/canvas.h>
#include <xview/icon.h>
#include <X11/Xlib.h>
#include <X11/Xresource.h>

#define  DEF_FONT  "fixed"
#define  F_ICON    0                  /* Icon index to frame array. */
#define  F_WINDOW  1                  /* Window index to frame array. */
#define  FONT      "times-roman-10"

#define  FRAME_BORDER_SIZE  7
#define  FRAME_LABEL_SIZE   30

#define  NOTIFY_INTERPOSE_EVENT_FUNC  (void) notify_interpose_event_func
#define  NOTIFY_SET_ITIMER_FUNC       (void) notify_set_itimer_func
#define  XV_CREATE                    (void) xv_create
#define  XV_DESTROY                   (void) xv_destroy
#define  XV_SET                       (void) xv_set

#define  ITIMER_NULL             ((struct itimerval *) 0)

enum popup_pos { P_BELOW, P_RIGHT } ;  /* Positions relative to main frame. */

static int xview_error_proc P((Xv_object, Attr_avlist)) ;

static void canvas_proc P((Canvas, Event *, caddr_t)) ;
static void display_prop_sheet P(()) ;
static void do_pause P(()) ;
static void make_prop_sheet P(()) ;
static void position_popup P((Frame, Frame, int)) ;
static void repaint_proc P((Canvas, Xv_Window, Rectlist *)) ;
static void set_prop_values P(()) ;
static void show_props P((Menu, Menu_item)) ;
static void x_error_proc P((Display *, XErrorEvent *)) ;

static Notify_value check_mail P(()) ;
static Notify_value frame_interpose P((Frame, Event *,
                                       Notify_arg, Notify_event_type)) ;
static Notify_value prop_apply_proc P((Panel_item, Event *)) ;
static Notify_value prop_choice_proc P((Panel_item, int, Event *)) ;
static Notify_value prop_defs_proc P((Panel_item, Event *)) ;
static Notify_value prop_mail_proc P((Panel_item, int, Event *)) ;
static Notify_value prop_reset_proc P((Panel_item, Event *)) ;

Canvas canvas ;
Canvas_paint_window pw ;
Frame frame, props_frame ;
Icon faces_icon ;
static Menu props_menu ;
static Panel props_panel ;
static Panel_item props_ctoggle, props_mtoggle, props_ptoggle, props_utoggle ;
static Panel_item props_cmd, props_file, props_host, props_mail, props_name ;
static Panel_item props_apply, props_defs, props_reset ;
static Panel_item props_no, props_period ;
static Panel_item props_tstamp, props_uname ;
#ifdef AUDIO_SUPPORT
static Panel_item props_audio ;
#endif /*AUDIO_SUPPORT*/

static enum mon_type prop_mtype ;         /* Current active monitor type. */
static char *cmdline = NULL ;             /* Saved command line options. */

Display *dpy ;
Drawable xid[2] ;               /* Xlib pointers to the screen and the icon. */
Pixmap pr[3] = { NULL, NULL, NULL } ;     /* Current memory pixmaps. */
Pixmap old_pr[2] = { NULL, NULL } ;       /* Previous memory pixmaps. */

/* Array of the different icon images. */
Pixmap images[MAXICONS] = {
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
} ;

GC gc ;                       /* Main drawing graphics context. */
GC tilegc ;                   /* Graphics context for tiling background. */
Window root ;
XFontStruct *sfont ;
XGCValues gc_val ;            /* Used to setup graphics context values. */
XrmDatabase faces_DB ;       /* Combined resources database. */
int screen ;                  /* Default graphics display screen. */
unsigned int depth ;
unsigned long backgnd ;       /* Default background color. */
unsigned long foregnd ;       /* Default foreground color. */
unsigned long gc_mask ;       /* Mask for setting graphic context values. */


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
            imagewidth, imageheight, column*imagewidth, row*imageheight) ;
}


/*ARGSUSED*/
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
      XCopyArea(dpy, xid[F_WINDOW], xid[F_WINDOW], gc, 0, 0,
                (unsigned int) width, (unsigned int) height, 0, 0) ;
      do_pause() ;
      XCopyArea(dpy, xid[F_WINDOW], xid[F_WINDOW], gc, 0, 0,
                (unsigned int) width, (unsigned int) height, 0, 0) ;
      do_pause() ;

      XCopyArea(dpy, xid[F_ICON], xid[F_ICON], gc, 0, 0,
                (unsigned int) width, (unsigned int) height, 0, 0) ;
      do_pause() ;
      XCopyArea(dpy, xid[F_ICON], xid[F_ICON], gc, 0, 0,
                (unsigned int) width, (unsigned int) height, 0, 0) ;
      do_pause() ;
      flashes-- ; 
    }             
  XSetFunction(dpy, gc, GXcopy) ;
}


/*ARGSUSED*/
static void
canvas_proc(canvas, event, arg)
Canvas canvas ;
Event *event ;
caddr_t arg ;
{
  int down, nextc ;

  down = event_is_down(event) ;
  nextc = event_action(event) ;

  if (down)
    {
           if (button1clear && nextc == ACTION_SELECT) do_key(KEY_CLEAR) ;
      else if (nextc == ACTION_ERASE_CHAR_BACKWARD)    do_key(KEY_CLEAR) ;
      else if (nextc == ACTION_MENU)
        menu_show((Menu) xv_get(canvas, WIN_MENU), canvas, event, 0) ;
      else if ((button1clear && nextc == ACTION_ADJUST) ||
              (!button1clear && nextc == ACTION_SELECT))
        {
          wdtype = (wdtype == DISP_NAME) ? DISP_OTHER : DISP_NAME ;
          draw_screen(wdtype) ;
        }
    }
  else if (event_is_ascii(event)) do_key(nextc) ;

  if (nextc == ACTION_PROPS) display_prop_sheet() ;
}


static Notify_value
check_mail()
{
  do_check() ;
  return(NOTIFY_DONE) ;
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
display_prop_sheet()
{
  if (xv_get(props_frame, XV_SHOW) == FALSE)
    {
      XV_SET(props_frame, FRAME_PROPS_PUSHPIN_IN, TRUE, 0) ;
      position_popup(frame, props_frame, P_RIGHT) ;
      XV_SET(props_frame, XV_SHOW, TRUE, 0) ;
    }
}


static void
do_pause()
{
  static struct timeval btime = { 0, 250000 } ;   /* Beep timer. */

  XSync(dpy, 0) ;

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
  XCopyArea(dpy, pr[(int) wdtype], xid[F_WINDOW], gc, 0, 0,
            width, height, 0, 0) ;
}


void
drop_back()
{
  if (lowerwindow) wmgr_bottom(frame) ;
}


static Notify_value
frame_interpose(frame, event, arg, type)
Frame frame ;
Event *event ;
Notify_arg arg ;
Notify_event_type type ;
{
  Notify_value rc ;

  rc = notify_next_event_func(frame, (Notify_event) event, arg, type) ;
  if (event_action(event) == ACTION_CLOSE)
    {
      XCopyArea(dpy, pr[(int) DISP_ICON], xid[F_ICON], gc, 0, 0, 
                imagewidth, imageheight, 0, 0) ; 
    }
  return(rc) ;
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


void
init_graphics(argc, argv)
int *argc ;
char *argv[] ;
{
  xv_init(XV_INIT_ARGC_PTR_ARGV, argc, argv,
          XV_ERROR_PROC,         xview_error_proc,
          XV_X_ERROR_PROC,       x_error_proc,
          0) ;
  frame = (Frame) xv_create(XV_NULL, FRAME, 0) ;
  dpy = (Display *) xv_get(frame, XV_DISPLAY) ;
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

  faces_icon = xv_create(XV_NULL,      ICON,
                         XV_WIDTH,     imagewidth,
                         XV_HEIGHT,    imageheight,
                         WIN_RETAINED, TRUE,
                         0) ;
  xid[F_ICON] = (Drawable) xv_get(faces_icon, XV_XID) ;

  screen  = DefaultScreen(dpy) ;
  root = RootWindow(dpy, screen) ;
  foregnd = BlackPixel(dpy, screen) ;
  backgnd = WhitePixel(dpy, screen) ;
  depth = DefaultDepth(dpy, screen) ;

  if (debug) XSynchronize(dpy, TRUE) ;
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

  if (bgicon != NULL) GET_SUN_ICON(bgicon, buf) ;
#ifdef REVORDER
  load_icon(BACKIMAGE, buf, 1) ;
#else
  load_icon(BACKIMAGE, buf, 0) ;
#endif /*REVORDER*/

  STRCPY(fname[0], "sun.icon") ;
  STRCPY(fname[1], "48x48x1") ;
  STRCPY(fname[2], "face.xbm") ;
  maxtypes = 3 ;
  gtype = XVIEW ;
  pr[(int) DISP_NAME] = pr[(int) DISP_OTHER] = NULL ;
  old_pr[(int) DISP_NAME] = old_pr[(int) DISP_OTHER] = NULL ;
  return 0 ;
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


/* Create a server image from given image data. */

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
  pr[(int) dtype] = XCreatePixmap(dpy, root, (unsigned int) width,
                                  (unsigned int) height, depth) ;
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
  Menu_item tmp_item ;

  XV_SET(frame,
         FRAME_ICON,                  faces_icon,
         FRAME_LABEL,                 faces_label,
         FRAME_NO_CONFIRM,            TRUE,
         FRAME_SHOW_FOOTER,           FALSE,
         XV_WIDTH,                    maxcols * imagewidth,
         XV_HEIGHT,                   imageheight * 10,
         0) ;
  NOTIFY_INTERPOSE_EVENT_FUNC(frame, frame_interpose, 0) ;

  canvas = xv_create(frame,               CANVAS,
                     CANVAS_REPAINT_PROC, repaint_proc,
                     CANVAS_RETAINED,     FALSE,
                     CANVAS_PAINTWINDOW_ATTRS,
                     WIN_CONSUME_EVENTS,
                       MS_LEFT, MS_RIGHT, WIN_ASCII_EVENTS, WIN_REPAINT,
                       WIN_LEFT_KEYS, WIN_TOP_KEYS, WIN_RIGHT_KEYS,
                       0,
                     WIN_IGNORE_EVENTS,
                       LOC_MOVE, LOC_DRAG,
                       0,
                     WIN_EVENT_PROC, canvas_proc,
                       0,
                     0) ;

  pw = canvas_paint_window(canvas) ;
  xid[F_WINDOW] = (Drawable) xv_get(pw, XV_XID) ;

  width = maxcols * imagewidth ;
  height = imageheight ;

  props_menu = xv_create(XV_NULL,         MENU,
                         MENU_TITLE_ITEM, "Faces",
                         0) ;

  tmp_item = xv_create(XV_NULL,          MENUITEM,
                       MENU_STRING,      "Properties",
                       MENU_ACTION_PROC, show_props,
                       MENU_CLIENT_DATA, 0,
                       0) ;

  XV_SET(props_menu, MENU_APPEND_ITEM, tmp_item, 0) ;
  XV_SET(pw,
         WIN_MENU,        props_menu,
         WIN_BIT_GRAVITY, ForgetGravity,
         0) ;

  make_prop_sheet() ;
  set_prop_values() ;
}


void
make_icon()
{
  if (mtype == MONPRINTER) adjust_image(DISP_ICON, NOPRINT, 0, 0) ;
  else                     adjust_image(DISP_ICON, NOMAIL,  0, 0) ;
}


static void
make_prop_sheet()
{
  props_frame = (Frame) xv_create(frame,  FRAME_PROPS,
                                 XV_LABEL,          "Faces properties",
                                 XV_X,              0,
                                 XV_Y,              0,
                                 XV_SHOW,           FALSE,
                                 FRAME_SHOW_FOOTER, TRUE,
                                 0) ;

  props_panel = (Panel) xv_get(props_frame, FRAME_PROPS_PANEL) ;
  XV_CREATE(props_panel,        PANEL_MESSAGE,
            XV_X,               xv_col(props_panel, 0),
            XV_Y,               xv_row(props_panel, 0),
            PANEL_LABEL_BOLD,   TRUE,
            PANEL_LABEL_STRING, "Monitor:",
            0) ;

  props_mtoggle = xv_create(props_panel,          PANEL_TOGGLE,
                            XV_X,                 xv_col(props_panel, 8),
                            XV_Y,                 xv_row(props_panel, 0),
                            PANEL_NOTIFY_PROC,    prop_choice_proc,
                            PANEL_CHOICE_STRINGS,
                              "Mail",
                              0,
                            0) ;
  props_utoggle = xv_create(props_panel,          PANEL_TOGGLE,
                            XV_X,                 xv_col(props_panel, 8),
                            XV_Y,                 xv_row(props_panel, 1),
                            PANEL_NOTIFY_PROC,    prop_choice_proc,
                            PANEL_CHOICE_STRINGS,
                              "Users",
                              0,
                            0) ;
  props_ptoggle = xv_create(props_panel,          PANEL_TOGGLE,
                            XV_X,                 xv_col(props_panel, 8),
                            XV_Y,                 xv_row(props_panel, 2),
                            PANEL_NOTIFY_PROC,    prop_choice_proc,
                            PANEL_CHOICE_STRINGS,
                              "Printer",
                              0,
                            0) ;
  props_ctoggle = xv_create(props_panel,          PANEL_TOGGLE,
            XV_X,                 xv_col(props_panel, 8),
            XV_Y,                 xv_row(props_panel, 3),
            PANEL_NOTIFY_PROC,    prop_choice_proc,
            PANEL_CHOICE_STRINGS,
              "Custom",
              0,
            0) ;

  props_mail  = xv_create(props_panel,          PANEL_CHOICE,
                          XV_X,                 xv_col(props_panel, 16),
                          XV_Y,                 xv_row(props_panel, 0),
                          PANEL_NOTIFY_PROC,    prop_mail_proc,
                          PANEL_CHOICE_STRINGS,
                            "New",
                            "All",
                            0,
                          0) ;
  props_file  = xv_create(props_panel,                PANEL_TEXT,
                          PANEL_VALUE_X,              xv_col(props_panel, 32),
                          XV_Y,                       xv_row(props_panel, 0),
                          PANEL_LABEL_STRING,         "File:",
                          PANEL_LABEL_BOLD,           TRUE,
                          PANEL_VALUE_DISPLAY_LENGTH, 15,
                          PANEL_VALUE_STORED_LENGTH,  MAXLINE,
                          0) ;
  props_host  = xv_create(props_panel,                PANEL_TEXT,
                          PANEL_VALUE_X,              xv_col(props_panel, 32),
                          XV_Y,                       xv_row(props_panel, 1),
                          PANEL_LABEL_STRING,         "Host:",
                          PANEL_LABEL_BOLD,           TRUE,
                          PANEL_VALUE_DISPLAY_LENGTH, 15,
                          PANEL_VALUE_STORED_LENGTH,  MAXLINE,
                          0) ;
  props_name  = xv_create(props_panel,                PANEL_TEXT,
                          PANEL_VALUE_X,              xv_col(props_panel, 32),
                          XV_Y,                       xv_row(props_panel, 2),
                          PANEL_LABEL_STRING,         "Name:",
                          PANEL_LABEL_BOLD,           TRUE,
                          PANEL_VALUE_DISPLAY_LENGTH, 15,
                          PANEL_VALUE_STORED_LENGTH,  MAXLINE,
                          0) ;
  props_cmd   = xv_create(props_panel,                PANEL_TEXT,
                          PANEL_VALUE_X,              xv_col(props_panel, 32),
                          XV_Y,                       xv_row(props_panel, 3),
                          PANEL_LABEL_STRING,         "Command:",
                          PANEL_LABEL_BOLD,           TRUE,
                          PANEL_VALUE_DISPLAY_LENGTH, 15,
                          PANEL_VALUE_STORED_LENGTH,  MAXLINE,
                          0) ;

  props_period = xv_create(props_panel,                PANEL_NUMERIC_TEXT,
                           PANEL_VALUE_X,              xv_col(props_panel, 32),
                           XV_Y,                       xv_row(props_panel, 4),
                           PANEL_VALUE_DISPLAY_LENGTH, 4,
                           PANEL_VALUE_STORED_LENGTH,  4,
                           PANEL_LABEL_STRING,         "Monitoring interval",
                           0) ;
  XV_CREATE(props_panel,        PANEL_MESSAGE,
            XV_X,               xv_col(props_panel, 41),
            XV_Y,               xv_row(props_panel, 4),
            PANEL_LABEL_STRING, "seconds",
            0) ;
#ifdef AUDIO_SUPPORT
  props_audio = xv_create(props_panel,          PANEL_CHECK_BOX,
                           PANEL_VALUE_X,        xv_col(props_panel, 8),
                           XV_Y,                 xv_row(props_panel, 5),
                           PANEL_CHOICE_STRINGS,
                             "Audio support",
                             0,
                           0) ;
#endif /*AUDIO_SUPPORT*/
  props_no     = xv_create(props_panel,          PANEL_CHECK_BOX,
                           PANEL_VALUE_X,        xv_col(props_panel, 8),
                           XV_Y,                 xv_row(props_panel, 6),
                           PANEL_CHOICE_STRINGS,
                             "Show number of messages",
                             0,
                           0) ;
  props_tstamp = xv_create(props_panel,          PANEL_CHECK_BOX,
                           PANEL_VALUE_X,        xv_col(props_panel, 8),
                           XV_Y,                 xv_row(props_panel, 7),
                           PANEL_CHOICE_STRINGS,
                             "Show timestamp",
                             0,
                           0) ;
  props_uname = xv_create(props_panel,          PANEL_CHECK_BOX,
                          PANEL_VALUE_X,        xv_col(props_panel, 8),
                          XV_Y,                 xv_row(props_panel, 8),
                          PANEL_CHOICE_STRINGS,
                            "Show username",
                            0,
                          0) ;

  props_apply = xv_create(props_panel,        PANEL_BUTTON,
                          XV_X,               xv_col(props_panel, 7),
                          XV_Y,               xv_row(props_panel, 10),
                          PANEL_LABEL_BOLD,   TRUE,
                          PANEL_LABEL_STRING, "Apply",
                          PANEL_NOTIFY_PROC,  prop_apply_proc,
                          0) ;
  props_reset = xv_create(props_panel,        PANEL_BUTTON,
                          XV_X,               xv_col(props_panel, 20),
                          XV_Y,               xv_row(props_panel, 10),
                          PANEL_LABEL_BOLD,   TRUE,
                          PANEL_LABEL_STRING, "Reset",
                          PANEL_NOTIFY_PROC,  prop_reset_proc,
                          0) ;
  props_defs  = xv_create(props_panel,        PANEL_BUTTON,
                          XV_X,               xv_col(props_panel, 33),
                          XV_Y,               xv_row(props_panel, 10),
                          PANEL_LABEL_BOLD,   TRUE,
                          PANEL_LABEL_STRING, "Defaults",
                          PANEL_NOTIFY_PROC,  prop_defs_proc,
                          0) ;
  window_fit(props_panel) ;
  window_fit(props_frame) ;
}


static void
position_popup(parent, child, position)
Frame parent, child ;
int position ;
{
  Rect crect, prect ;
  int height, width ;

  if (((int) xv_get(child, XV_SHOW)) && position == P_RIGHT) return ;
  frame_get_rect(parent, &prect) ;
  frame_get_rect(child,  &crect) ;
  height = (int) xv_get(parent, XV_HEIGHT) ;
  width  = (int) xv_get(parent, XV_WIDTH) ;

  switch (position)
    {
      case P_BELOW : crect.r_left = prect.r_left + FRAME_BORDER_SIZE ;
                     crect.r_top  = prect.r_top + height + FRAME_LABEL_SIZE +
                                    FRAME_BORDER_SIZE ;
                     break ;
      case P_RIGHT : crect.r_left = prect.r_left + width +
                                                   (2 * FRAME_BORDER_SIZE) ;
                     crect.r_top  = prect.r_top ;
    }
  frame_set_rect(child, &crect) ;
}


/*ARGSUSED*/
static Notify_value
prop_apply_proc(item, event)
Panel_item item ;
Event *event ;
{
  mtype = prop_mtype ;
  period       =   (int) xv_get(props_period, PANEL_VALUE) ;
#ifdef AUDIO_SUPPORT
  do_audio     =   (int) xv_get(props_audio,  PANEL_VALUE) ;
#endif /*AUDIO_SUPPORT*/
  dontshowno   = !((int) xv_get(props_no,     PANEL_VALUE)) ;
  dontshowtime = !((int) xv_get(props_tstamp, PANEL_VALUE)) ;
  dontshowuser = !((int) xv_get(props_uname,  PANEL_VALUE)) ;

  read_str(&spoolfile, (char *) xv_get(props_file, PANEL_VALUE)) ;
  read_str(&rhostname, (char *) xv_get(props_host, PANEL_VALUE)) ;
  read_str(&printer,   (char *) xv_get(props_name, PANEL_VALUE)) ;
  read_str(&userprog,  (char *) xv_get(props_cmd,  PANEL_VALUE)) ;
  do_prop_check() ;
  return(NOTIFY_DONE) ;
}


/*ARGSUSED*/
static Notify_value
prop_choice_proc(item, value, event)
Panel_item item ;
int value ;
Event *event ;
{
       if (item == props_mtoggle)
    prop_mtype = (enum mon_type) xv_get(props_mail, PANEL_VALUE) ;
  else if (item == props_ptoggle) prop_mtype = MONPRINTER ;
  else if (item == props_ctoggle) prop_mtype = MONPROG ;
  else if (item == props_utoggle) prop_mtype = MONUSERS ;
 
/* Activate or inactive all the panel items appropriately. */
 
  XV_SET(props_mtoggle, PANEL_VALUE, (item == props_mtoggle), 0) ;
  XV_SET(props_utoggle, PANEL_VALUE, (item == props_utoggle), 0) ;
  XV_SET(props_ptoggle, PANEL_VALUE, (item == props_ptoggle), 0) ;
  XV_SET(props_ctoggle, PANEL_VALUE, (item == props_ctoggle), 0) ;
 
  XV_SET(props_mail, PANEL_INACTIVE, (item != props_mtoggle), 0) ;
  XV_SET(props_file, PANEL_INACTIVE, (item != props_mtoggle), 0) ;
  XV_SET(props_host, PANEL_INACTIVE, (item != props_utoggle), 0) ;
  XV_SET(props_name, PANEL_INACTIVE, (item != props_ptoggle), 0) ;
  XV_SET(props_cmd,  PANEL_INACTIVE, (item != props_ctoggle), 0) ;
  return(NOTIFY_DONE) ;
}


/*ARGSUSED*/
static Notify_value
prop_defs_proc(item, event)
Panel_item item ;
Event *event ;
{
  mtype        = MONNEW ;
  period       = 60 ;
  dontshowno   = 0 ;
  dontshowtime = 0 ;
  dontshowuser = 0 ;
  spoolfile    = NULL ;
  rhostname    = NULL ;
  printer      = NULL ;
  userprog     = NULL ;
  set_prop_values() ;
  do_prop_check() ;
  return(NOTIFY_DONE) ;
}


/*ARGSUSED*/
static Notify_value
prop_mail_proc(item, value, event)
Panel_item item ;
int value ;
Event *event ;
{
  prop_mtype = (enum mon_type) value ;
  return(NOTIFY_DONE) ;
}


/*ARGSUSED*/
static Notify_value
prop_reset_proc(item, event)
Panel_item item ;
Event *event ;
{
  set_prop_values() ;
  do_prop_check() ;
  return(NOTIFY_DONE) ;
}


/*ARGSUSED*/
static void
repaint_proc(canvas, window, repaint_area)
Canvas canvas ;
Xv_Window window ;
Rectlist *repaint_area ;
{
  draw_screen(wdtype) ;
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


static void
set_prop_values()
{
  Panel_item item ;

       if (mtype == MONALL || mtype == MONNEW) item = props_mtoggle ;
  else if (mtype == MONPRINTER)                item = props_ptoggle ;
  else if (mtype == MONPROG)                   item = props_ctoggle ;
  else if (mtype == MONUSERS)                  item = props_utoggle ;
  prop_mtype = mtype ;

/* Activate or inactive all the panel items appropriately. */

  XV_SET(props_mtoggle, PANEL_VALUE, (item == props_mtoggle), 0) ;
  XV_SET(props_utoggle, PANEL_VALUE, (item == props_utoggle), 0) ;
  XV_SET(props_ptoggle, PANEL_VALUE, (item == props_ptoggle), 0) ;
  XV_SET(props_ctoggle, PANEL_VALUE, (item == props_ctoggle), 0) ;

  if (mtype == MONNEW || mtype == MONALL)
    XV_SET(props_mail, PANEL_VALUE, mtype, 0) ;

#ifdef AUDIO_SUPPORT
  XV_SET(props_audio,  PANEL_VALUE,  do_audio,     0) ;
#endif /*AUDIO_SUPPORT*/
  XV_SET(props_period, PANEL_VALUE,  period,       0) ;
  XV_SET(props_no,     PANEL_VALUE, !dontshowno,   0) ;
  XV_SET(props_tstamp, PANEL_VALUE, !dontshowtime, 0) ;
  XV_SET(props_uname,  PANEL_VALUE, !dontshowuser, 0) ;

  if (spoolfile != NULL) XV_SET(props_file, PANEL_VALUE, spoolfile, 0) ;
  if (rhostname != NULL) XV_SET(props_host, PANEL_VALUE, rhostname, 0) ;
  if (printer   != NULL) XV_SET(props_name, PANEL_VALUE, printer,   0) ;
  if (userprog  != NULL) XV_SET(props_cmd,  PANEL_VALUE, userprog,  0) ;

  XV_SET(props_mail, PANEL_INACTIVE, (item != props_mtoggle), 0) ;
  XV_SET(props_file, PANEL_INACTIVE, (item != props_mtoggle), 0) ;
  XV_SET(props_host, PANEL_INACTIVE, (item != props_utoggle), 0) ;
  XV_SET(props_name, PANEL_INACTIVE, (item != props_ptoggle), 0) ;
  XV_SET(props_cmd,  PANEL_INACTIVE, (item != props_ctoggle), 0) ;
}


void
save_cmdline(line)       /* Null routine. */
char *line ;
{
  if (cmdline != NULL) FREE(cmdline) ;
  cmdline = (char *) malloc((unsigned int) strlen(line)) ;
  STRCPY(cmdline, line) ;
  XV_SET(frame, WIN_CMD_LINE, cmdline, 0) ;
}


void
show_display(beep_and_raise)    /* Show the latest set of mail icon faces. */
int beep_and_raise ;
{
  Rect *temprect ;

  if (beep_and_raise && raisewindow && newmail && !mboxshrunk)
    wmgr_top(frame) ;

  if (invert)     /* Invert the memory pixrects before displaying. */
    {
      XSetFunction(dpy, gc, GXcopyInverted) ;
      XCopyArea(dpy, pr[(int) DISP_NAME], pr[(int) DISP_NAME], gc, 0, 0,
                (unsigned int) width, (unsigned int) height, 0, 0) ;
      XCopyArea(dpy, pr[(int) DISP_OTHER], pr[(int) DISP_OTHER], gc, 0, 0,
                (unsigned int)  width, (unsigned int) height, 0, 0) ;
    }

  XCopyArea(dpy, pr[(int) DISP_ICON], xid[F_ICON], gc, 0, 0,
            (unsigned int) width, (unsigned int) height, 0, 0) ;
 
  temprect = (Rect *) xv_get(frame, FRAME_OPEN_RECT) ;
  temprect->r_height = height ;
  temprect->r_width = width ;
  XV_SET(frame, FRAME_OPEN_RECT, temprect, 0) ;
  draw_screen(wdtype) ;

  if (beep_and_raise && newmail && !mboxshrunk) beep_flash(beeps, flashes) ;
  if (old_pr[(int) DISP_NAME])  XFreePixmap(dpy, old_pr[(int) DISP_NAME]) ;
  if (old_pr[(int) DISP_OTHER]) XFreePixmap(dpy, old_pr[(int) DISP_OTHER]) ;
  old_pr[(int) DISP_NAME] = NULL ;
  old_pr[(int) DISP_OTHER] = NULL ;
  XSync(dpy, 0) ;
}


/*ARGSUSED*/
static void
show_props(m, mi)
Menu m ;
Menu_item mi ;
{
  display_prop_sheet() ;
}


void
start_tool()
{
  struct itimerval tval ;

  tval.it_interval.tv_usec = 0 ;
  tval.it_interval.tv_sec = period ;
  tval.it_value.tv_usec = 0 ;
  tval.it_value.tv_sec = period ;
  NOTIFY_SET_ITIMER_FUNC(frame, check_mail,
                         ITIMER_REAL, &tval, ITIMER_NULL) ;
  xv_main_loop(frame) ;
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

  if ((len = strlen(str)) > 10)      /* Character length of text. */
    {
      len = 10 ;
      str[10] = '\0' ;               /* Maximum of 10 characters. */
    }
  switch (jtype)
    {
      case LEFT  : x =  c      * imagewidth + 2 ;
                   y = (r + 1) * imageheight - 5 ;
                   break ;
      case RIGHT : x = (c + 1) * imagewidth - (len * 6) - 2 ;
                   y = (r + 1) * imageheight - 5 ;
    }
  XSetFunction(dpy, gc, GXandInverted) ;
  XFillRectangle(dpy, pr[(int) dtype], gc, x, y-9, (unsigned int) len*6+2, 13) ;
  XSetFunction(dpy, gc, GXxor) ;
  XDrawImageString(dpy, pr[(int) dtype], gc, x, y, str, strlen(str)) ;
  XSetFunction(dpy, gc, GXcopy) ;
}


static void
x_error_proc(display, error)
Display *display ;
XErrorEvent *error ;
{
  char msg[80] ;
  char *cwd ;           /* Current working directory */

/*  There are two types of error handlers in Xlib; one to handle fatal
 *  conditions, and one to handle error events from the event server.
 *  This function handles the latter, and aborts with a core dump so
 *  we can determine where the X Error really occurred.
 *
 *  Also: you cannot directly or indirectly perform any operations on the
 *  server while in this error handler.
 */

  XGetErrorText(display, error->error_code, msg, 80);
  FPRINTF(stderr, "\nX Error (intercepted): %s\n", msg) ;
  FPRINTF(stderr, "Major Request Code   : %d\n",   error->request_code) ;
  FPRINTF(stderr, "Minor Request Code   : %d\n",   error->minor_code) ;
  FPRINTF(stderr, "Resource ID (XID)    : %u\n",   error->resourceid) ;
  FPRINTF(stderr, "Error Serial Number  : %u\n",   error->serial) ;
  FPRINTF(stderr, "\nDumping core file") ;

  cwd = (char *) getcwd(NULL, MAXPATHLEN) ;
  if (cwd) FPRINTF(stderr, " in %s\n", cwd) ;
  else     FPRINTF(stderr, "...\n") ;

  abort() ;

/*NOTREACHED*/
}


/*ARGSUSED*/
static int
xview_error_proc(object, avlist)     /* Intercept XView error calls. */
Xv_object object ;
Attr_avlist avlist ;
{
  Attr_avlist attrs ;
  Error_severity severity = ERROR_RECOVERABLE ;

  FPRINTF(stderr, "\nXView Error (Intercepted)\n") ;

  for (attrs = avlist; *attrs ; attrs = attr_next(attrs))
    {
      switch((int) attrs[0])
        {
          case ERROR_BAD_ATTR : FPRINTF(stderr, "Bad Attribute:%s\n",
                                        attr_name(attrs[1])) ;
                                break ;

          case ERROR_BAD_VALUE : FPRINTF(stderr,
                                       "Bad Value (0x%x) for attribute: %s\n",
                                        attrs[1], attr_name(attrs[2])) ;
                                 break ;

          case ERROR_INVALID_OBJECT : FPRINTF(stderr, "Invalid Object: %s\n",
                                              (char *) attrs[1]) ;
                                      break ;

          case ERROR_STRING : {
                                char *c = (char *) attrs[1] ;

                                if (c[strlen(c)] == '\n')
                                  c[strlen(c)] = NULL ;
                                FPRINTF(stderr, "Error: %s\n",
                                        (char *) attrs[1]) ;
                              }
                              break ;

          case ERROR_PKG : FPRINTF(stderr, "Package: %s\n",
                                   ((Xv_pkg *) attrs[1])->name) ;
                           break ;

          case ERROR_SEVERITY : severity = attrs[1] ;
                                break ;

          default : FPRINTF(stderr, "Unknown XView error Attribute (%s)\n",
                            (char *) attrs[1]) ;
        }
    }  

/* If a critical error or debugging, then core dump */

  if (severity == ERROR_NON_RECOVERABLE || debug)
    {
      char *cwd ;         /* current working directory */

      if (severity == ERROR_NON_RECOVERABLE)
        FPRINTF(stderr, "Non-recoverable Error: dumping core file") ;
      else
        FPRINTF(stderr, "Debug mode: Recoverable Error: dumping core file") ;

      cwd = (char *) getcwd(NULL, MAXPATHLEN) ;
      if (cwd) FPRINTF(stderr, " in %s\n", cwd) ;
      else     FPRINTF(stderr, "...\n") ;

      abort() ;
    }
  else
    {
      FPRINTF(stderr, "Recoverable Error: continuing...\n") ;
      return(XV_OK) ;
    }
/*NOTREACHED*/
}
