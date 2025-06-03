
/*
 * Copyright 1989 Dirk Grunwald
 * 
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Dirk Grunwald or M.I.T.
 * not be used in advertising or publicity pertaining to distribution of
 * the software without specific, written prior permission.  Dirk
 * Grunwald and M.I.T. makes no representations about the suitability of
 * this software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 * 
 * DIRK GRUNWALD AND M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL M.I.T.  BE LIABLE FOR ANY SPECIAL, INDIRECT
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * Author:
 * 	Dr. Dirk Grunwald
 * 	Dept. of Computer Science
 * 	Campus Box 430
 * 	Univ. of Colorado, Boulder
 * 	Boulder, CO 80309
 * 
 * 	grunwald@colorado.edu
 * 	
 */ 

#include <stdio.h>
#include <sys/time.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <stdio.h>
#include <assert.h>
#include <varargs.h>
#include <math.h>
#include <ctype.h>
#include <sys/param.h>

#include "xtex.h"
#include "dvi-simple.h"
#include "DviPageP.h"

#include "DviPageNEWS.h"

void initNEWS()
{ 
}

static Colormap xtex_colormap= (Colormap)0;

static void mangleColormap()
{
  Arg arg;
  int xtex_screen;
  int success;
  int mincol = 10;
  XColor xtex_color;
  unsigned long planeMask;
  unsigned int ncol;
  unsigned long map[256];
  int icol;

  
  /* must modify the colormap so rasters plot in correct colors */
  /* this is a big hack, arrived at by much trial and error */
  xtex_screen = DefaultScreen(XtDisplay(TopLevelWidget));
  xtex_colormap = DefaultColormap((XtDisplay(TopLevelWidget)),xtex_screen);

  /* first we allocate all available color cells */
  ncol = 254;
  do {
    success = XAllocColorCells(XtDisplay(TopLevelWidget),xtex_colormap,
				0,&planeMask,0,map,ncol);
    if (!success) ncol--;
  } while (!success && ncol >= mincol);

  if( !success ) return;

  /* then we redefine those cells that NeWS seems to use for rasters */
  /* and free up those that it does not */

  for (icol=0; icol<ncol; icol++) {
     if (map[icol] >= 116 && map[icol] <= 129) {
	/* a grey ramp */
       	xtex_color.red = (map[icol]-115)*65535/15;
       	xtex_color.green = (map[icol]-115)*65535/15;
       	xtex_color.blue = (map[icol]-115)*65535/15;
       	xtex_color.pixel = map[icol];
	xtex_color.flags = DoRed | DoGreen | DoBlue;
	XStoreColor(XtDisplay(TopLevelWidget),xtex_colormap,&xtex_color);
     }else if (map[icol] == 192) {
	/* a mid-grey */
        xtex_color.red = 32768;
        xtex_color.green = 32768;
        xtex_color.blue = 32768;
        xtex_color.pixel = map[icol];
	xtex_color.flags = DoRed | DoGreen | DoBlue;
	XStoreColor(XtDisplay(TopLevelWidget),xtex_colormap,&xtex_color);
     }else if (map[icol] == 223) {
	/* a lighter-grey */
        xtex_color.red = 48000;
        xtex_color.green = 48000;
        xtex_color.blue = 48000;
        xtex_color.pixel = map[icol];
	xtex_color.flags = DoRed | DoGreen | DoBlue;
	XStoreColor(XtDisplay(TopLevelWidget),xtex_colormap,&xtex_color);
     }else if (map[icol] == 254) {
	/* a white */
        xtex_color.red = 65535;
        xtex_color.green = 65535;
        xtex_color.blue = 65535;
        xtex_color.pixel = map[icol];
	xtex_color.flags = DoRed | DoGreen | DoBlue;
	XStoreColor(XtDisplay(TopLevelWidget),xtex_colormap,&xtex_color);
     }else if (map[icol] == 255 || map[icol] == 130 || map[icol] == 131 || map[icol] == 135 || map[icol] == 136 || map[icol] == 155 || map[icol] == 156 || map[icol] == 160 || map[icol] == 161 ) {
	/* lots of pixels seem to be black */
        xtex_color.red = 0;
        xtex_color.green = 0;
        xtex_color.blue = 0;
        xtex_color.pixel = map[icol];
	xtex_color.flags = DoRed | DoGreen | DoBlue;
	XStoreColor(XtDisplay(TopLevelWidget),xtex_colormap,&xtex_color);
     }else if (map[icol] >= 0 && map[icol] <= 255) {
	XFreeColors(XtDisplay(TopLevelWidget),xtex_colormap,&map[icol],1,0);
     }else {
	fprintf(stderr,"ignoring icol %d map %d\n",icol,map[icol]);
     }
  }


}


void
  psfigBeginNEWS(w,cp,fudge_height)
DviPageWidget w;
char *cp;
int fudge_height;
{
  int bbllx, bblly;
  int bburx, bbury;
  int width, height;
  int rawWidth, rawHeight;
  int haveExtensions;

  
  int rx = fastFromSpHoriz(w, w -> dviPage.dviStackPointer -> h);
  int ry = fastFromSpVert(w, w -> dviPage.dviStackPointer -> v);

  int maxWidth = XtScreen(w) -> width;
  int mmWidth = WidthMMOfScreen( XtScreen(w) );
  
  double dpi = (w -> dviPage.dpiHoriz * w -> dviPage.userMag) / 1000;
  double trueDpi;
  double scale;
  
  if( xtex_colormap == (Colormap)0 ) mangleColormap();

  if ( w -> dviPage.trueDpi == 0.0 ) {
    /* 72 dpi seems to be the correct value under OpenWindows */
    /* also the values returned by maxWidth and mmWidth do not */
    /* seem to be reliable.  So I've stuck with 72. */
    /*	trueDpi = (maxWidth * 25.4) / mmWidth;*/
    trueDpi = 72.;
  }
  else {
    trueDpi = w -> dviPage.trueDpi;
  }
  scale = dpi / trueDpi;
  
  sscanf(cp, " %d %d %d %d %d %d ",
	 &rawWidth, &rawHeight,
	 &bbllx, &bblly,
	 &bburx, &bbury);
  
  width = fastFromSpHoriz(w, rawWidth);
  height = fastFromSpVert(w, rawHeight);

  /*
   * \epsfbox{} apparently causes a situation where the image is
   * expected to grow upward, rather than down from the position on the
   * page, so we fake it here
   */
  if( fudge_height ) ry -= height;
  
  /* catch up on the X side */

  XSync(XtDisplay(w),0);

  if (w -> dviPage.newsfile == NULL) {
    w -> dviPage.newsfile = ps_open_PostScript();
  }

  haveExtensions = w -> dviPage.newsfile != NULL;

  /* check if server has extension */
  
  if ( !haveExtensions ) {
    XDrawLine(XtDisplay(w), XtWindow(w),
	      w -> dviPage.paintGC,
	      rx, ry, rx + width, ry );
    XDrawLine(XtDisplay(w), XtWindow(w),
	      w -> dviPage.paintGC,
	      rx + width, ry, rx + width, ry + height);
    XDrawLine(XtDisplay(w), XtWindow(w),
	      w -> dviPage.paintGC,
	      rx + width, ry + height, rx, ry + height);
    XDrawLine(XtDisplay(w), XtWindow(w),
	      w -> dviPage.paintGC,
	      rx, ry + height, rx, ry);
  }

  if ( haveExtensions ) {
    /* get xid of window, then make this window the NeWS canvas */
    XID xid = XtWindow(w);
    NeWStoken newstoken;
    int len;

    ps_token_from_xid(xid,&newstoken);
    ps_setcanvas(newstoken);
    
    len = strlen(w -> dviPage.dpsPreamble);
    psio_write(w -> dviPage.dpsPreamble, len, 1, PostScript);
    psio_flush(PostScript);
    
    /* set origin, and flip y coordinates to agree with NeWS system */
    psio_fprintf( PostScript,"\n %d %d @newsstart\n",rx,ry);
    
    psio_fprintf( PostScript,"\n%d %f @start\n",
		 w -> dviPage.pixelsPerInchHoriz,
		 scale);
    
    psio_fprintf( PostScript," %d %d %d %d %d %d  startTexFig\n",
		 rawWidth, rawHeight, bbllx, bblly, bburx, bbury);
    
    psio_flush(PostScript);
  }

}

void
  psfigPlotfileNEWS(w,cp)
DviPageWidget w;
char *cp;
{
  if ( w -> dviPage.newsfile )
      {
	while ( cp && *cp && isspace(*cp) ) cp++; /* skip white */
	if (strncmp(cp,"plotfile",8) == 0 ) {
	  char *filename;
	  FILE *psfile;
	  
	  cp += 8; /* skip plotfile */
	  while ( cp && *cp && isspace(*cp) ) cp++; /* skip white */
	  filename = cp;
	  while ( cp && *cp && !isspace(*cp) ) cp++; /* skip non-white */
	  *cp = 0; /* terminate file name */
	  
	  if ( (psfile = fopen(filename, "r")) ) {
	    char buffer[1024];
	    int len;
	    
	    ps_flush_PostScript();
	    
	    for (;;) { 
	      len = fread(buffer, sizeof(char), 1024, psfile);
	      if  ( len == 0 ) break;
	      psio_write(buffer, len, 1, PostScript);
	    }	  
	    fclose(psfile);
	    psio_flush(PostScript);
	  }
	}
	
	psio_fprintf( PostScript,"\n endTexFig\n");
	psio_flush(PostScript);
	
      }
}

void
  psfigEndNEWS(w,cp)
DviPageWidget w;
char *cp;
{
  if (w -> dviPage.newsfile ) {
    psio_flush(PostScript);
    ps_close_PostScript();
    w -> dviPage.newsfile = NULL;
  }
}

int checkNEWS()
{
  PSFILE * newsfile;

  newsfile = ps_open_PostScript();
  if( newsfile != NULL ){
        ps_close_PostScript();
        return 1;
  }
  return 0;
}

