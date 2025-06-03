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

#include "DviPageDPS.h"

void initDPS()
{
/* we don't do anthing for DPS */
}

static void
  HandleStatus(ctx, status)
DPSContext ctx;
int status;
{
  char *ptr, buf[1000];
  switch (status) {
  case PSRUNNING:	ptr = "[DPS status] running\n"; break;
  case PSNEEDSINPUT:	ptr = "[DPS status] needs input\n"; break;
  case PSZOMBIE:	ptr = "[DPS status] zombie\n"; break;
  case PSFROZEN:	ptr = "[DPS status] frozen\n"; break;
  default:		ptr = "[DPS status] unknown status\n"; break;
  }
  error(0,0,status);
  if (status == PSFROZEN) {
    XDPSUnfreezeContext(ctx);
  }
}

static void TextOut(ctx, buffer, count)
     DPSContext ctx;
     char *buffer;
     unsigned count;
{
  char buff[1024];
  while ( count > 0 ) {
    int n = count;
    if ( count > 1023 ) {
      n = 1023;
    }
    memcpy(buff, buffer, n);
    buff[n] = 0;
    error(0,0,buffer);
    count -= n;
  }
}

/*
 * USE_GSAVE and ALWAYS_KILL were used for debugging memory leaks with
 * the Display Postscript server.
 */

/* USE_GSAVE means to wrap each user invocation in a gsave/grestore pair */

#define USE_GSAVE

/* ALWAYS_KILL means to kill the DPS context after each use, causing a
   new one to be create for the next DPS figure */

#undef ALWAYS_KILL


/* DPS postscript figure rendering */
void
  psfigBeginDPS(w,cp,fudge_height)
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
  

  if ( XRectInRegion( w -> dviPage.updateRegion,
		     rx, ry, width, height) == RectangleOut) {
    w -> dviPage.dpsVisible = False;
    return;
  }
  else {
    w -> dviPage.dpsVisible = True;
  }
  
  if ( w -> dviPage.ctx == NULL ) {
    w -> dviPage.ctx = XDPSCreateSimpleContext(XtDisplay(w),
					       XtWindow(w),
					       DefaultGC(XtDisplay(w), 0),
					       rx, ry,
					       TextOut, DPSDefaultErrorProc,
					       NULL);
  }

  if ( w -> dviPage.ctx != NULL ) {
    XDPSRegisterStatusProc(w -> dviPage.ctx, HandleStatus);
    DPSPrintf(w -> dviPage.ctx, "\n resyncstart\n");
    DPSWritePostScript(w -> dviPage.ctx,
		       w -> dviPage.dpsPreamble,
		       strlen(w -> dviPage.dpsPreamble));
      

  }

  haveExtensions = w -> dviPage.ctx != NULL;

  /* check if server has DPS extension */
  
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
      int maxWidth = XtScreen(w) -> width;
      int mmWidth = WidthMMOfScreen( XtScreen(w) );
      
      double dpi = (w -> dviPage.dpiHoriz * w -> dviPage.userMag) / 1000;
      double trueDpi;
      double scale;
      
      if ( w -> dviPage.trueDpi == 0.0 ) {
	trueDpi = (maxWidth * 25.4) / mmWidth;
      }
      else {
	trueDpi = w -> dviPage.trueDpi;
      }
      scale = dpi / trueDpi;
      

#ifdef USE_GSAVE
      DPSPrintf(w -> dviPage.ctx, "\n gsave save\n");
#else
      DPSPrintf(w -> dviPage.ctx, "\n \n");
#endif

      DPSPrintf( w -> dviPage.ctx, "%lu %lu %lu %lu setXgcdrawable\n",
		XGContextFromGC(DefaultGC(XtDisplay(w),0)),
		XtWindow(w),
		rx, ry);      

      DPSPrintf(w -> dviPage.ctx,"\n%d %f @start\n",
		w -> dviPage.pixelsPerInchHoriz,
		scale);
      
      DPSPrintf(w -> dviPage.ctx, " %d %d %d %d %d %d startTexFig\n",
		rawWidth, rawHeight, bbllx, bblly, bburx, bbury);
    }
}

void
  psfigPlotfileDPS(w,cp)
DviPageWidget w;
char *cp;
{
    if ( w -> dviPage.ctx  && w -> dviPage.dpsVisible ) 
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
	    
	    DPSFlushContext(w -> dviPage.ctx);
	    DPSWaitContext(w -> dviPage.ctx);
	    
	    for (;;) { 
	      len = fread(buffer, sizeof(char), 1024, psfile);
	      if  ( len == 0 ) break;
	      DPSWritePostScript(w -> dviPage.ctx, buffer, len);
	    }	  
	    fclose(psfile);
	    DPSFlushContext(w -> dviPage.ctx);
	    DPSWaitContext(w -> dviPage.ctx);
	  }
	}
	
	DPSPrintf( w -> dviPage.ctx, "\n endTexFig\n");
	
# ifdef USE_GSAVE
	DPSPrintf( w -> dviPage.ctx, "\n restore grestore\n");
	DPSFlushContext(w -> dviPage.ctx);
	DPSWaitContext(w -> dviPage.ctx);
# endif
	
# ifdef ALWAYS_KILL
	DPSDestroySpace( DPSSpaceFromContext( w -> dviPage.ctx ));
	w -> dviPage.ctx = 0;
# endif
	
      }
}

void
  psfigEndDPS(w,cp)
DviPageWidget w;
char *cp;
{
  if ( w -> dviPage.ctx  && w -> dviPage.dpsVisible ) { 
    DPSFlushContext( w -> dviPage.ctx );
    DPSWaitContext( w -> dviPage.ctx );
    /* context is destroyed when widget is destroyed */
  }
}

int checkDPS()
{

  DPSContext ctx;

  ctx = XDPSCreateSimpleContext(XtDisplay(TopLevelWidget),
                                XtWindow(TopLevelWidget),
                                DefaultGC(XtDisplay(TopLevelWidget), 0),
                                0, 0,
                                TextOut, DPSDefaultErrorProc,
                                NULL);
  if( ctx != NULL ){
     DPSFlushContext( ctx );
     DPSWaitContext( ctx );
     DPSDestroySpace( DPSSpaceFromContext( ctx ));
     return 1;
  }

  return 0;

}
 
