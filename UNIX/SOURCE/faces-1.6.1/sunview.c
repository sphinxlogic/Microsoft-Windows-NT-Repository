
/*  @(#)sunview.c 1.20 91/09/05
 *
 *  SunView dependent graphics routines used by faces,
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
#include <sys/file.h>
#include "faces.h"
#include "extern.h"
#include <suntool/sunview.h>
#include <suntool/canvas.h>

#define  FONT                    "/usr/lib/fonts/fixedwidthfonts/screen.r.7"

#define  ICON_SET                (void) icon_set
#define  NOTIFY_SET_ITIMER_FUNC  (void) notify_set_itimer_func
#define  PR_DESTROY              (void) pr_destroy
#define  PR_REPLROP              (void) pr_replrop
#define  PR_ROP                  (void) pr_rop

#ifdef NO_PR_TTEXT
#define  PR_TTEXT                (void) pf_ttext
#else
#define  PR_TTEXT                (void) pr_ttext
#endif /*NO_PR_TTEXT*/

#define  PW_ROP                  (void) pw_rop
#define  WIN_BELL                (void) win_bell
#define  WINDOW_SET              (void) window_set

#define  ITIMER_NULL  ((struct itimerval *) 0)
#define  ROOT_NAME    "/dev/win0"

static void canvas_proc P((Canvas, Event *, caddr_t)) ;
static void repaint_proc P((Canvas, Pixwin *, Rectlist *)) ;
static Notify_value check_mail P(()) ;

Canvas canvas ;
Frame frame ;
Icon faces_icon ;
Pixfont *sfont ;              /* Small font for timestamp and count. */
Pixrect *pr[3] ;              /* Pointers to current pixrects. */
Pixrect *old_pr[2] ;          /* Pointers to previous pixrects. */

/* Array of the different icon images. */
Pixrect *images[MAXICONS] = {
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
} ;

Pixwin *fpw, *pw ;
int ffd ;                     /* File descriptor of faces frame. */
int rootfd ;


void
adjust_image(dtype, itype, row, column)  /* Put new face in memory pixrect. */
enum disp_type dtype ;
enum icon_type itype ;
int row, column ;
{
  if (mtype == MONNEW)
    PR_ROP(pr[(int) dtype], imagewidth, 0, (maxcols-1)*imagewidth,
           imageheight, PIX_SRC, pr[(int) dtype], 0, 0) ;

  PR_ROP(pr[(int) dtype], column*imagewidth, row*imageheight,
         imagewidth, imageheight, PIX_CLR,
         pr[(int) dtype], column*imagewidth, row*imageheight) ;
  PR_ROP(pr[(int) dtype], column*imagewidth, row*imageheight,
         iconwidth, iconheight, PIX_SRC, images[(int) itype], 0, 0) ;
}


void
beep_flash(beeps, flashes)    /* Perform visual feedback. */
int beeps, flashes ;
{
  static struct timeval btime = { 0, 250000 } ;   /* Beep timer. */
  int bfd ;        /* File descriptor for faces window, or -1. */
  Pixwin *bpw ;    /* Pixwin pointer for faces window, or 0. */

#ifdef AUDIO_SUPPORT
  if (do_audio && bell_sound != NULL)
    {
      play_sound(bell_sound) ;
      beeps = 0 ;
    }
#endif /*AUDIO_SUPPORT*/

  if (beeps || flashes)
    do
      {
        bfd = beeps > 0 ? ffd : -1 ;
        bpw = flashes > 0 ? fpw : (Pixwin *) 0 ;
        WIN_BELL(bfd, btime, bpw) ;
        SELECT(0, (fd_set *) 0, (fd_set *) 0, (fd_set *) 0, &btime) ;
        beeps-- ;
        flashes-- ;
      } 
    while (beeps > 0) ;
}


/*ARGSUSED*/
static void
canvas_proc(canvas, event, arg)
Canvas canvas ;
Event *event ;
caddr_t arg ;
{
  int nextc ;

  nextc = event_id(event) ;
  if (event_is_down(event))
    {
      if (button1clear && nextc == MS_LEFT) do_key(KEY_CLEAR) ;
      else if ((button1clear && nextc == MS_MIDDLE) ||
              (!button1clear && nextc == MS_LEFT))
        {
          wdtype = (wdtype == DISP_NAME) ? DISP_OTHER : DISP_NAME ;
          draw_screen(wdtype) ;
        }
    }
  if (event_is_ascii(event)) do_key(nextc) ;
  else window_default_event_proc(canvas, event, arg) ;
}


static Notify_value
check_mail()
{
  do_check() ;
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
    PR_ROP(    pr[(int) DISP_NAME], 0, 0, width, height, PIX_SRC,
           old_pr[(int) DISP_NAME], 0, 0) ;
  if (mtype == MONNEW && old_pr[(int) DISP_OTHER] != NULL)
    PR_ROP(    pr[(int) DISP_OTHER], 0, 0, width, height, PIX_SRC,
           old_pr[(int) DISP_OTHER], 0, 0) ;
}


void
destroy_image(itype)
enum icon_type itype ;
{
  if (images[(int) itype] != NULL)
    {
      pr_destroy(images[(int) itype]) ;
      images[(int) itype] = NULL ;
    }
}


void
draw_screen(wdtype)
enum disp_type wdtype ;
{
  PW_ROP(pw, 0, 0, width, height, PIX_SRC, pr[(int) wdtype], 0, 0) ;
}


void
drop_back()
{
  if (lowerwindow)
    wmgr_bottom((int) window_get(frame, WIN_FD), rootfd) ;
}


/*ARGSUSED*/
char *
get_resource(rtype)      /* Null routine (currently only X11 and XView). */
enum res_type rtype ;
{
  return((char *) NULL) ;
}


void
init_font()
{
  sfont = pf_open(FONT) ;
}


/*ARGSUSED*/
void
init_graphics(argc, argv)
int *argc ;
char *argv[] ;
{
}


/*ARGSUSED*/
int
init_ws_type(argc, argv)
int argc ;
char *argv[] ;
{
  int i, j ;
  unsigned short buf[256] ;

  if (getenv("WINDOW_PARENT") == NULL)
    {
      FPRINTF(stderr,"%s: Not a native SunView window\n", progname) ;
      return -1 ;
    }
  for (i = 0; i < 16; i++)    /* Load default gray background. */
    {
      for (j = 0; j < 4; j++) buf[i * 16      + j] = 0x8888 ;
      for (j = 0; j < 4; j++) buf[i * 16 +  4 + j] = 0x2222 ;
      for (j = 0; j < 4; j++) buf[i * 16 +  8 + j] = 0x8888 ;
      for (j = 0; j < 4; j++) buf[i * 16 + 12 + j] = 0x2222 ;
    }

  if (bgicon != NULL) GET_SUN_ICON(bgicon, buf) ;
  load_icon(BACKIMAGE, buf, 0) ;

  STRCPY(fname[0], "sun.icon") ;
  STRCPY(fname[1], "48x48x1") ;
  STRCPY(fname[2], "face.xbm") ;
  maxtypes = 3 ;
  gtype = SVIEW ;
  pr[(int) DISP_NAME] = pr[(int) DISP_OTHER] = NULL ;
  old_pr[(int) DISP_NAME] = old_pr[(int) DISP_OTHER] = NULL ;
  rootfd = open(ROOT_NAME, O_RDONLY, 0) ;
  return 0 ;
}


void
load_data(itype, cbuf)
enum icon_type itype ;
unsigned char *cbuf ;
{
  unsigned short *dst, *src ;
  int i ;

  images[(int) itype] = mem_create(iconwidth, iconheight, 1) ;
  dst = (unsigned short *)
        ((struct mpr_data *) images[(int) itype]->pr_data)->md_image ;
  for (i = 0; i < 256; i++)
    dst[i] = (unsigned short)
             ((revtable[cbuf[i*2]     & 0xFF] & 0xFF) << 8) +
              (revtable[cbuf[(i*2)+1] & 0xFF] & 0xFF) ;
}


/* Create a memory pixrect from given image data. */

/*ARGSUSED*/
void
load_icon(itype, buf, not_flipped)
enum icon_type itype ;
unsigned short buf[256] ;
int not_flipped ;
{
  unsigned short *ptr ;
  int i ;

  images[(int) itype] = mem_create(iconwidth, iconheight, 1) ;
  ptr = (unsigned short *)
        ((struct mpr_data *) images[(int) itype]->pr_data)->md_image ;
  for (i = 0; i < 256; i++) ptr[i] = buf[i] ;
#ifdef REVORDER
  if (not_flipped)
    ((struct mpr_data *) images[(int) itype]->pr_data)->md_flags &= ~MP_I386 ;
#endif /*REVORDER*/
}


void
load_resources()     /* Dummy routine; used with X11 and XView versions. */
{
}


void
make_area(dtype, width, height)
enum disp_type dtype ;
{
  pr[(int) dtype] = mem_create(width, height, 1) ;
  PR_ROP(pr[(int) dtype], 0, 0, width, height, PIX_CLR, pr[(int) dtype], 0, 0) ;
}


void
make_frame(argc, argv)
int argc ;
char *argv[] ;
{
  frame = window_create((Window) 0,        FRAME,
                        FRAME_ICON,        faces_icon,
                        FRAME_LABEL,       faces_label,
                        FRAME_SUBWINDOWS_ADJUSTABLE, FALSE,
                        FRAME_NO_CONFIRM,  TRUE,
                        WIN_WIDTH,         maxcols * imagewidth + 10,
                        WIN_HEIGHT,        imageheight * 10 + 10,
                        FRAME_ARGS,        argc, argv,
                        0) ;
  fpw = (Pixwin *) LINT_CAST(window_get(frame, WIN_PIXWIN)) ;
  ffd = (int) window_get(frame, WIN_FD) ;

  canvas = window_create(frame, CANVAS,
                         WIN_EVENT_PROC,      canvas_proc,
                         CANVAS_REPAINT_PROC, repaint_proc,
                         CANVAS_RETAINED,     TRUE,
                         0) ;
  WINDOW_SET(canvas, WIN_CONSUME_KBD_EVENTS, WIN_ASCII_EVENTS,
                     WIN_LEFT_KEYS, WIN_TOP_KEYS, WIN_RIGHT_KEYS,
                     WIN_UP_EVENTS, 0, 0) ;
  WINDOW_SET(canvas, WIN_IGNORE_PICK_EVENT, LOC_MOVE, 0) ;
  pw = canvas_pixwin(canvas) ;
  init_font() ;
  width = maxcols * imagewidth ;
  height = imageheight ;
}


void
make_icon()
{
  faces_icon = icon_create(WIN_WIDTH,  imagewidth,
                           WIN_HEIGHT, imageheight,
                           0) ;
  if (mtype == MONPRINTER) adjust_image(DISP_ICON, NOPRINT, 0, 0) ;
  else                     adjust_image(DISP_ICON, NOMAIL,  0, 0) ;
}


/*ARGSUSED*/
static void
repaint_proc(canvas, pw, repaint_area)
Canvas canvas ;
Pixwin *pw ;
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
  if (dest == CUROFF)
    {
      if (pr[(int) dtype])
        PR_REPLROP(pr[(int) dtype], 0, 0, width, height, PIX_SRC,
                   images[(int) BACKIMAGE], 0, 0) ;
    }
  else
    {
      if (old_pr[(int) dtype])
        PR_REPLROP(old_pr[(int) dtype], 0, 0, width, height, PIX_SRC,
                   images[(int) BACKIMAGE], 0, 0) ;
    }
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
  Rect *temprect ;

  if (beep_and_raise && raisewindow && newmail && !mboxshrunk)
    wmgr_top((int) window_get(frame, WIN_FD), rootfd) ;

  if (invert)     /* Invert the memory pixrects before displaying. */
    {
      PR_ROP(pr[(int) DISP_NAME],  0, 0, width, height, PIX_NOT(PIX_DST),
             pr[(int) DISP_NAME],  0, 0) ;
      PR_ROP(pr[(int) DISP_OTHER], 0, 0, width, height, PIX_NOT(PIX_DST),
             pr[(int) DISP_OTHER], 0, 0) ;
    }

  ICON_SET(faces_icon, ICON_IMAGE, pr[(int) DISP_ICON], 0) ;
  WINDOW_SET(frame, FRAME_ICON, faces_icon, 0) ;

  temprect = (Rect *) LINT_CAST(window_get(frame, FRAME_OPEN_RECT)) ;
  temprect->r_height = height + 10 ;
  if (nolabel == FALSE) temprect->r_height += 12 ;
  temprect->r_width = width + 10 ;
  WINDOW_SET(frame, FRAME_OPEN_RECT, temprect, 0) ;
  draw_screen(wdtype) ;

  if (beep_and_raise && newmail && !mboxshrunk) beep_flash(beeps, flashes) ;
  if (old_pr[(int) DISP_NAME])    PR_DESTROY(old_pr[(int) DISP_NAME]) ;
  if (old_pr[(int) DISP_OTHER])   PR_DESTROY(old_pr[(int) DISP_OTHER]) ;
  old_pr[(int) DISP_NAME]  = NULL ;
  old_pr[(int) DISP_OTHER] = NULL ;
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
  window_main_loop(frame) ;
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
      case DISP_ICON   : c = r = 0 ;;
                         break ;
    }

  if ((len = strlen(str)) > 10)         /* Character length of text. */
    {
      len = 10 ;
      str[10] = '\0' ;   /* Maximum of 10 characters. */
    }
  switch (jtype)
    {
      case LEFT  : x =  c      * imagewidth + 2 ;
                   y = (r + 1) * imageheight - 4 ;
                   break ;
      case RIGHT : x = (c + 1) * imagewidth - (len * 6) - 2 ;
                   y = (r + 1) * imageheight - 4 ;
    }
  PR_ROP(pr[(int) dtype], x, y - 9, len * 6 + 2, 10, PIX_CLR,
         (Pixrect *) NULL, 0, 0) ;
  PR_TTEXT(pr[(int) dtype], x, y, PIX_SRC ^ PIX_NOT(PIX_DST), sfont, str) ;
}
