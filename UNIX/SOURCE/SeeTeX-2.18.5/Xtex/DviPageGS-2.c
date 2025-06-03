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
#include <unistd.h>		/* for access() */

#include "xtex.h"
#include "dvi-simple.h"
#include "DviPageP.h"

#include "DviPageGS-2.h"

static int rx, ry;
static int width, height;
static int psfigError;
static double tx, ty;
static double psfigWidth, psfigHeight;
static char prevDir[MAXPATHLEN] = "", curDir[MAXPATHLEN];

#if defined (__hpux) || defined (__svr4__)
#define getwd(d) getcwd(d, MAXPATHLEN)
#endif

void
psfigBeginGHOST (w, cp, fudge_height)
DviPageWidget w;
char *cp;
int fudge_height;
	{
int bbllx, bblly;
int bburx, bbury;
int rawWidth, rawHeight;
int rawPsfigWidth, rawPsfigHeight;
double scaleXY, psfigScaleX, psfigScaleY, magScaleX, magScaleY;
char ghostSetupStr[1024], ghostInputStr[1024];

	if (sscanf(cp, " %d %d %d %d %d %d ", &rawWidth, &rawHeight, 
			&bbllx, &bblly, &bburx, &bbury) != 6)
		{
		DialogMessage ("Error reading info at ps::begin\n", False);
		psfigError = True;
		return;
		}
	
	if (w->dviPage.ghostPixmap == None)
		{
		psfigError = True;
		return;
		}

	/* Set up pixmap for ghostscript to draw on */
	/* Make sure pixmap is appropriate */
	if ((w->dviPage.ghostPixmap != None) && 
			((w->dviPage.ghostPixWidth != w->dviPage.pixelsWide) || 
			(w->dviPage.ghostPixHeight != w->dviPage.pixelsHigh)))
		{
		/* Pixmap exists, but something changed. 
		 * Kill the interpreter if the queue is empty.
		 */
#ifdef DEBUG
		fprintf (stderr, "Size changed : %d %d --> %d %d\n",
			w->dviPage.ghostPixWidth, w->dviPage.ghostPixHeight,
			w->dviPage.pixelsWide, w->dviPage.pixelsHigh);
		fflush (stderr);
#endif
		gsintResize (w);
		}

	if (w->dviPage.ghostPixmap == None)
		{
		psfigError = True;
		return;
		}

	/* Check if directory chnaged */
	getwd (curDir);
	if (strcmp (prevDir, curDir) != 0)
		{
		/* Yes; kill interpreter. */
		StopInterpreter (w);
		strcpy (prevDir, curDir);
		}

	rx = fastFromSpHoriz(w, w -> dviPage.dviStackPointer -> h);
	ry = fastFromSpVert(w, w -> dviPage.dviStackPointer -> v);
	width = fastFromSpHoriz (w, rawWidth);
	height = fastFromSpVert(w, rawHeight);

	if (w->dviPage.ghostPID == -1)
		{
		StartInterpreter (w);
		if (w->dviPage.ghostPID == -1)
			{
			psfigError = True;
			/* Couldn't start gs; just draw an outline */
			XDrawRectangle (XtDisplay (w), XtWindow (w), w->dviPage.paintGC, 
				rx, ry, width, height);
			return;
			}
		}

/* 
 * \epsfbox{} apparently causes a situation where the image is 
 * expected to grow upward, rather than down from the position on the 
 * page, so we fake it here
 */
        if( fudge_height ) ry -= height;

	psfigError = False;
	DialogMessage ("startTexFig\n", False);

	rawPsfigWidth = bburx - bbllx;
	rawPsfigHeight = bbury - bblly;
	psfigWidth = fastFromSpHoriz (w, (double)(rawPsfigWidth));
	psfigHeight = fastFromSpVert (w, (double)(rawPsfigHeight));
	psfigScaleX = (double) width / psfigWidth;
	psfigScaleY = (double) height / psfigHeight;

#ifdef DEBUG
	fprintf (stderr, "rx=%d ry=%d W = %d H = %d\n\
(w, h) = (%d %d) :: (%d %d)\n\
%d %d :: %f %f\n",
		rx, ry, w->dviPage.pixelsWide, w->dviPage.pixelsHigh,
		rawWidth, rawHeight, width, height, 
		rawPsfigWidth, rawPsfigHeight, psfigWidth, psfigHeight);
	fprintf (stderr, "ll = (%d %d) :: (%d %d)\nur = (%d %d) :: (%d %d)\n",
		bbllx, bblly, fastFromSpHoriz (w, bbllx), fastFromSpVert (w, bblly), 
		bburx, bbury, fastFromSpHoriz (w, bburx), fastFromSpVert (w, bbury));
#endif

	/* Save state */
	WriteBytes (w, "startTexFig\n");
#ifdef DEBUG
	fprintf (stderr, "startTexFig\n");
#endif

	/* Scale per psfig instructions */
	sprintf (ghostInputStr, "%f %f scale\n", psfigScaleX, psfigScaleY);
	WriteBytes (w, ghostInputStr);
#ifdef DEBUG
	fprintf (stderr, "%s", ghostInputStr);
#endif
	
	/* Scale for pixels per inch goodals + magnification */
	magScaleX = (double) w->dviPage.pixelsPerInchHoriz / 72.0;
	magScaleY = (double) w->dviPage.pixelsPerInchVert / 72.0;
	sprintf (ghostInputStr, "%f %f scale\n", magScaleX, magScaleY);
	WriteBytes (w, ghostInputStr);
#ifdef DEBUG
	fprintf (stderr, "%s", ghostInputStr);
#endif

	/* Translate origin to lower-left-corner of figure on page */
	tx = -1.0 * fastFromSpHoriz (w, bbllx) / magScaleX;
	ty = -1.0 * fastFromSpVert (w, bblly) / magScaleY;
	sprintf (ghostInputStr, "%f %f translate\n", tx, ty);
	WriteBytes (w, ghostInputStr);
#ifdef DEBUG
	fprintf (stderr, "%s", ghostInputStr);
#endif 

	/* Show that were busy */
	busyCursor ();
	/*
	TeXPageMakeInsensitive (w);
	*/
	}


void
psfigEndGHOST (w, cp)
DviPageWidget w;
char *cp;
	{
char ghostSetupStr[1024];

	if (psfigError || (w->dviPage.ghostPID == -1))
		return;

	WriteBytes (w, "endTexFig\n");
#ifdef DEBUG
	fprintf (stderr, "endTexFig\n");
#endif


	waitCopyAndAcknowledge (w, True, 0, w->dviPage.pixelsHigh - height, 
		width, height, rx, ry);

	DialogMessage ("endTexFig\n", False);
	unbusyCursor ();
	/*
	TeXPageMakeSensitive (w);
	*/
	}

void
psfigPlotfileGHOST (w, cp)
DviPageWidget w;
char *cp;
{
	char *fileName, ghostInputStr[1024];

	if (psfigError || (w->dviPage.ghostPID == -1))
		return;

	/* Skip white space */
	while (cp && *cp && isspace(*cp)) 
		cp++;
	if (strncmp (cp, "plotfile", 8) != 0)
		return;

	/* Skip 'plotfile' */
	cp += 8;

	/* Skip white space */
	while (cp && *cp && isspace(*cp)) 
		cp++;
	fileName = cp;
	/* Skip non-white */
	while (cp && *cp && !isspace(*cp)) 
		cp++;
	/* Terminate file name */
	*cp = 0;

	if (access(fileName,R_OK) == 0) {
	    DialogMessage ("\t", False);
	    DialogMessage (fileName, False);
	    DialogMessage ("\n", False);
	    sprintf (ghostInputStr, "(%s) run\n", fileName);
	} else {		/* error in reading file */
	    double lx = -tx;
	    double ly = -ty;
	    sprintf(ghostInputStr,"Cannot access file %s. Drawing box.\n",
		    fileName);
	    DialogMessage(ghostInputStr, False);
	    sprintf(ghostInputStr,
		    "newpath %lf %lf moveto %lf %lf lineto %lf %lf \
lineto %lf %lf lineto closepath gsave 0.9 setgray fill grestore\n",
		    lx, ly,
		    lx + psfigWidth, ly,
		    lx + psfigWidth, ly + psfigHeight,
		    lx, ly + psfigHeight);
	}

	WriteBytes (w, ghostInputStr);
#ifdef DEBUG
	fprintf (stderr, "%s", ghostInputStr);
#endif
}
