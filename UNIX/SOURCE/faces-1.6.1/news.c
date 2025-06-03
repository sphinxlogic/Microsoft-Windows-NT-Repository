
/*  @(#)news.c 1.19 91/09/05
 *
 *  NeWS dependent graphics routines used by faces,
 *  the visual mail and print job monitor.
 * 
 *  Copyright (c) Rich Burridge - Sun Microsystems Australia.
 *                                All rights reserved.
 *
 *  Rewritten to use NeWS cps routines by Pat Lashley - Sun Microsystems Inc.
 *
 *  Permission is given to distribute these sources, as long as the
 *  copyright messages are not removed, and no monies are exchanged. 
 * 
 *  No responsibility is taken for any errors on inaccuracies inherent
 *  either to the comments or the code of this program, but if reported
 *  to me, then an attempt will be made to fix them.
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sundev/kbd.h>
#include <sundev/kbio.h>
#include <NeWS/psmacros.h>
#include "faces.h"
#include "extern.h"
#include "faces_cps.h"

#define  F_ICON    0                  /* Icon index to frame array. */
#define  F_WINDOW  1                  /* Window index to frame array. */

char *dnames[] = { "PR", "MPR" } ;

char *inames[] = {
    "BackgroundIcon",	"NofaceIcon",	"NomailIcon",	"NopaperIcon",
    "NoprintIcon",	"NousersIcon",	"CurfaceIcon"
  } ;

#ifdef NO_43SELECT
int fullmask ;               /* Full mask of file descriptors to check on. */
int readmask ;               /* Readmask used in select call. */
#else
fd_set fullmask ;            /* Full mask of file descriptors to check on. */
fd_set readmask ;            /* Readmask used in select call. */
#endif /*NO_43SELECT*/

int kbdfd ;                  /* File descriptor for the keyboard. */
int psfd ;                   /* NeWS connection file descriptor. */
unsigned short ibuf[256] ;   /* Ikon/icon image buffer. */

extern PSFILE *PostScript ;
extern PSFILE *PostScriptInput ;

static void clear_animates P(()) ;
static void do_background P(()) ;
static void do_news_ps P((struct psinfo *)) ;
static void handle_mouse P(()) ;


void
adjust_image(dtype, itype, row, column)
    enum disp_type dtype ;
    enum icon_type itype ;
    int row, column ;
{
    switch ((int) mtype)
    {
      case MONNEW:
	ps_shiftimage (imagewidth, (maxcols-1)*imagewidth,
		       imageheight, dnames[(int) dtype]) ;
	
	if  (facetype == NEWSTYPE) {
	    ps_clearbox (column*imagewidth, row*imageheight,
			 imagewidth, imageheight, dnames[(int) dtype]) ;
	    
	    return ;
	} /* else fall into default */
	
      default:
	ps_moveimage (inames[(int) itype], imagewidth, imageheight,
		      column*imagewidth, row*imageheight,
		      dnames[(int) dtype]) ;
    }
}   /* adjust_image() */


/*ARGSUSED*/
void
beep_flash(beeps, flashes)    /* Perform visual feedback. */
    int beeps, flashes ;
{
    int i, x ;

#ifdef AUDIO_SUPPORT
    if (do_audio && bell_sound != NULL)
    {
      play_sound(bell_sound) ;
      return ;
    }
#endif /*AUDIO_SUPPORT*/

    for (i = 0; i < beeps; i++)
    {
	x = KBD_CMD_BELL ;
	IOCTL(kbdfd, KIOCCMD, &x) ;
	usleep(250000) ;
	x = KBD_CMD_NOBELL ;
	IOCTL(kbdfd, KIOCCMD, &x) ;
	usleep(250000) ;
    }
}   /* beep_flash() */


static void
clear_animates()      /* Remove chain of animate records. */
{
    struct psinfo *next, *this ;   /* For removing current chain of records. */

    for  (this = psrecs  ;  this != NULL  ;  this = next)
    {
	next = this->next ;
	if (this->name != NULL) FREE(this->name) ;
	FREE((char *) this) ;   /* Remove this record. */
	this = next ;
    }
    
    psrecs = plast = NULL ;
}   /* clear_animates() */


void
create_pixrects(width, height)     /* Create pixrects for the face images. */
int width, height ;
{
    int h, w ;
    
    if  ((mtype == MONNEW) && !firsttime)	return ;
    ps_dopixrects (width, height, "PR") ;
    h = height ;
    w = width ;
    if (mtype == MONPRINTER)
    {
	h = imageheight ;
	w = imagewidth ;
    }
    ps_dopixrects (w, h, "MPR") ;
    do_background () ;          /* Setup background pattern. */
} /* create_pixrects() */


void
destroy_image()    /* Null routine. */
{}


static void
do_background()     /* Set up background pattern. */
{
    int i, j ;
    
    
    for  (i = 0; i < (height / imageheight); i++)
    {
	for (j = 0; j < (width / imagewidth); j++)
	{
	    ps_moveimage (inames[BACKIMAGE],
			  imagewidth,	imageheight,
			  j,		i,
			  dnames[(int) F_WINDOW]) ;
	    ps_moveimage (inames[BACKIMAGE],
			  imagewidth,	imageheight,
			  j,		i,
			  dnames[(int) F_ICON]) ;
	}
    }
}   /* do_background() */


static void
do_news_ps(psrecs)     /* Display chain of NeWS animations. */
    struct psinfo *psrecs ;
{
    struct psinfo	*this ;
    
    
    for  (this = psrecs  ;  this != NULL  ;  this = this->next)
    {
	if ((mtype != MONNEW) || (this->column < maxcols))
        {
	    ps_execfile (this->name, imagewidth, imageheight,
			 this->column*imagewidth, this->row*imageheight) ;
	    ps_flush_PostScript() ;
        }
    }
}   /* do_news_ps() */


/*ARGSUSED*/
char *
get_resource(rtype)      /* Null routine (currently only X11 and XView). */
enum res_type rtype ;
{
  return((char *) NULL) ;
}


static void
handle_mouse()     /* Clear canvas to background and recheck for new mail. */
{
    do_background() ;    /* Setup background pattern. */
    clear_animates() ;   /* Remove chain of animate records. */
    do_check() ;
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
    
    if (ps_open_PostScript() < 0) return -1 ;
    ps_flush_PostScript() ;
    if (ferror(PostScript))
    {
	ps_close_PostScript() ;
	return(-1) ;
    }
    
    for (i = 0; i < 16; i++)    /* Load default gray background. */
    {
	for (j = 0; j < 4; j++) buf[i * 16      + j] = 0x8888 ;
	for (j = 0; j < 4; j++) buf[i * 16 +  4 + j] = 0x2222 ;
	for (j = 0; j < 4; j++) buf[i * 16 +  8 + j] = 0x8888 ;
	for (j = 0; j < 4; j++) buf[i * 16 + 12 + j] = 0x2222 ;
    }
    
    if (bgicon != NULL) GET_SUN_ICON(bgicon, buf) ;
    load_icon(BACKIMAGE, buf) ;
    
    if (invert) pprintf(PostScript, "%p ", 3, "/Invert false def") ;
    else pprintf(PostScript, "%p ", 3, "/Invert true def") ;
    STRCPY(fname[0], "face.ps") ;
    STRCPY(fname[1], "sun.icon") ;
    STRCPY(fname[2], "48x48x1") ;
    STRCPY(fname[3], "face.xbm") ;
    maxtypes = 4 ;
    gtype = NEWS ;
    return(0) ;
}


/*ARGSUSED*/
void
load_data(itype, cbuf)
    enum icon_type itype ;
    unsigned char *cbuf ;
{
    ps_buildicon (inames[(int) itype], iconwidth, iconheight,
                  (unsigned short *) cbuf, 256) ;
}   /* load_data() */


/*ARGSUSED*/
void
load_icon(itype, sbuf, not_flipped)
    enum icon_type itype ;
    unsigned short sbuf[256] ;
    int not_flipped ;
{
    ps_buildicon (inames[(int) itype], iconwidth, iconheight, sbuf, 256) ;
}   /* load_icon() */


void
load_resources()    /* Dummy routine; used with X11 and XView versions. */
{
}


void
make_area(dtype, width, height)
    enum disp_type dtype ;
    int width, height ;
{
}   /* make_area() */


/*ARGSUSED*/
void
make_frame(argc, argv)
    int argc ;
    char *argv[] ;
{
    if ((kbdfd = open("/dev/kbd", 0)) == -1)
    {
	FPRINTF(stderr, "%s: can't open keyboard.\n", progname) ;
	exit(1) ;
    }
    
    psfd = fileno(PostScriptInput) ;
    
#ifdef NO_43SELECT
    fullmask = 1 << psfd ;
#else
    FD_ZERO(&fullmask) ;
    FD_SET(psfd, &fullmask) ;
#endif /*NO_43SELECT*/
    
    ps_makeframe (wx, wy, maxcols*imagewidth+10, imageheight*10+10,
		  ix, iy, iconic) ;
    ps_initfont() ;
    width = maxcols * imagewidth ;
    height = imageheight ;
}   /* make_frame() */


void
make_icon()   /* Null routine. */
{}


void
repl_image(dtype, dest, width, height)
    enum disp_type dtype ;
    enum image_type dest ;
    int width, height ;
{
    int i, j ;
    
    if (dest != CUROFF) return ;
    for (i = 0; i < (height / imageheight); i++)
    {
	for (j = 0; j < (width / imagewidth); j++)
	{
	    ps_moveimage (inames[BACKIMAGE],
			  imagewidth, imageheight, j, i, dnames[(int) dtype]) ;
	}
    }
}   /* repl_image() */


void
save_cmdline(line)       /* Null routine. */
    char *line ;
{
}


void
show_display(beep_and_raise)  /* Show the latest set of mail icon faces. */
int beep_and_raise ;
{
    if (mtype != MONPRINTER)
    {
	ps_showdisplay (width, height, width, height) ;
    } else {
	ps_showdisplay (width, height, imagewidth, imageheight) ;
    }
    
    if  (beep_and_raise && newmail)	beep_flash (beeps, flashes) ;
    if  (psrecs != NULL)		do_news_ps (psrecs) ;
    
    ps_flush_PostScript() ;
}   /* show_display() */



void
start_tool()
{
    int type ;                     /* Value from NeWS server. */
    struct timeval tval ;

    
    tval.tv_usec = 0 ;
    tval.tv_sec = period ;
    
    for (;;)
    {
	readmask = fullmask ;
#ifdef NO_43SELECT
	SELECT(32, &readmask, 0, 0, &tval) ;
	if (readmask && (1 << psfd))
#else
	SELECT(FD_SETSIZE, &readmask, (fd_set *) 0, (fd_set *) 0, &tval) ;
	if (FD_ISSET(psfd, &readmask))
#endif /*NO_43SELECT*/
	{
	    if (pscanf(PostScriptInput, "d", &type) == EOF) exit(1) ;
	    switch (type)
	    {
	      case DIED     : exit(0) ;
	      case LEFTDOWN : if (mtype == MONNEW)
	      {
		  do_background() ;
		  clear_animates() ;
		  do_check() ;
	      }
		break ;
	      case PAINTED  : if (psrecs != NULL) do_news_ps(psrecs) ;
	    }
	}
	else
	{
	    if (mtype != MONNEW) clear_animates() ;
	    do_check() ;        /* Check the mail/printer again. */
	}
    }
}


void
text(dtype, jtype, str)
    enum disp_type dtype ;
    enum just_type jtype ;
    char *str ;
{
    int i, len ;
    int c, r ;             /* Column and row position for this face. */
    int x, y ;             /* Position of start of this text string. */
    char line[MAXLINE] ;   /* PostScript text string to be builtup. */
    
    c = column ;
    r = row ;
    switch (dtype)
    {
      case DISP_ALL:
	text(DISP_ICON,  jtype, str) ;
	/* Fall into DISP_BOTH */
      case DISP_BOTH:
	text(DISP_NAME,  jtype, str) ;
	text(DISP_OTHER, jtype, str) ;
	return ;
	
      case DISP_ICON:
	c = r = 0 ;
    }
    
    if ((len = strlen(str)) > 10)         /* Character length of text. */
    {
	len = 10 ;
	str[10] = '\0' ;                  /* Maximum of 10 characters. */
    }
    
    line[0] = '\0' ;
    for (i = 0; i < len; i++)
    {
	switch (str[i])
	{
	  case '\\':	STRCAT(line, "\\\\") ;		break ;
	  case '(':	STRCAT(line, "\\(") ;		break ;
	  case ')':	STRCAT(line, "\\)") ;		break ;
	  default:	STRNCAT(line, &str[i],1) ;
	}
    }
    
    switch (jtype)
    {
      case LEFT:
	x = c * imagewidth + 2 ;
	y = r * imageheight + 2 ;
	break ;
	
      case RIGHT:
	x = (c + 1) * imagewidth - (len * 6) - 2 ;
	y = r * imageheight + 2 ;
    }

    ps_clearbox (x, y, len*6+2, 10, dnames[(int) dtype]) ;
    ps_maketext (line, x, y, dnames[(int) dtype]) ;
}   /* text() */
