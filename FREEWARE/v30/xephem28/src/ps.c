/* code to support a set of functions which are called like X functions but
 * which may in fact be creating postscript.
 */

#include <stdio.h>

#if defined(__STDC__)
#include <stdlib.h>
#endif

#if defined(_POSIX_SOURCE)
#include <unistd.h>
#else
extern int unlink();
#endif

#include <time.h>

#include <Xm/Xm.h>
#include <Xm/SelectioB.h>
#include <Xm/Text.h>
#include <Xm/ToggleB.h>

#include "patchlevel.h"
#include "P_.h"
#include "ps.h"

extern Widget toplevel_w;

extern char *syserrstr P_((void));
extern FILE *fopenh P_((char *name, char *how));
extern int existsh P_((char *filename));
extern void get_something P_((Widget w, char *resource, XtArgVal value));
extern void get_xmstring P_((Widget w, char *resource, char **txtp));
extern void prompt_map_cb P_((Widget w, XtPointer client, XtPointer call));
extern void query P_((Widget tw, char *msg, char *label1, char *label2,
    char *label3, void (*func1)(), void (*func2)(), void (*func3)()));
extern void set_xmstring P_((Widget w, char *resource, char *txtp));
extern void set_something P_((Widget w, char *resource, XtArgVal value));
extern void xe_msg P_((char *msg, int app_modal));

typedef enum {
    CLOSED=0,	/* not in use */
    ASKING,	/* in process of asking user where to save */
    OPEN, 	/* FILE * is open and ready for writing */
    XDRAWING	/* xforms set up and ready for X coords */
} XPSState;

typedef struct {
    XPSState state;	/* what's happ'nin' */
    void (*go)();	/* user's function to call when ok to proceed */
    FILE *fp;		/* file accumulating PS commands */

    int wantpr;		/* 1 to print, 0 to save in file */
    char *prfile;	/* malloced name of temp file used for printing */

    Window win;		/* window we are capturing */
    int fontsz;		/* font size, points */
    double scale;	/* how to scale X coords to the PS page */

    Font *fids;		/* malloced array of nfonts Font IDs */
    char **fnames;	/* malloced array of corresponding PS font keys */
    int nfonts;		/* number of pairs malloced */
} XPSContext;

/* here is the implied state behind most of the XPS* functions */
static XPSContext xpsc;

/* handy reference to xpsc.fp */
#define	FP	xpsc.fp 

/* annotation row Y coord */
#define	AROWY(r)	(80 + (xpsc.fontsz+1)*(r))

static Widget print_w;			/* print setup dialog */
static Widget saveas_w;			/* save-as setup dialog */
static char deffont[] = "Helvetica";	/* default font */

static int XPSOpen P_((char *fn));
static void doArc P_((XArc *xap, int fill));
static void doPoly P_((XPoint xp[], int nxp, int mode, int fill));
static void doRect P_((int x, int y, int w, int h, int fill));
static void checkState P_((char *funcname, XPSState s));
static void printTime P_((int x, int y));

static void create_print_w P_((void));
static void create_saveas_w P_((void));
static void saveasok_cb P_((Widget w, XtPointer client, XtPointer call));
static void saveas_confirm P_((void));
static void prok_cb P_((Widget w, XtPointer client, XtPointer call));
static void xpsc_close P_((void));
static void no_go P_((void));
static void call_go P_((void));

/* ask user to save or print. if ok, call his go() after we've called XPSOpen()
 */
void
XPSAsk (wantpr, title, go)
int wantpr;	/* 1 to print, 0 to save in file */
char *title;
void (*go)();
{
	char buf[1024];

	/* TODO: make this reentrant somehow.
	 * but then would need to make the xpsc state dynamic too. guck.
	 */
	if (xpsc.state != CLOSED) {
	    xe_msg ("Only one print/save request at one time.", 1);
	    return;
	}

	xpsc.go = go;
	xpsc.state = ASKING;
	xpsc.wantpr = wantpr;

	if (wantpr) {
	    if (!print_w)
		create_print_w();
	    (void) sprintf (buf, "xephem %s Print", title);
	    set_xmstring (print_w, XmNdialogTitle, buf);
	    XtManageChild (print_w);
	} else {
	    if (!saveas_w)
		create_saveas_w();
	    (void) sprintf (buf, "xephem %s Save", title);
	    set_xmstring (saveas_w, XmNdialogTitle, buf);
	    XtManageChild (saveas_w);
	}

}

/* remember the fid/name pair in xpsc */
void
XPSRegisterFont (fid, fname)
Font fid;
char *fname;
{
	int n;

	/* do nothing quietly if not currently building a PS file */
	if (xpsc.state == CLOSED)
	    return;

	/* avoid dups */
	for (n = 0; n < xpsc.nfonts; n++)
	    if (fid == xpsc.fids[n])
		return;

	/* save the new fid/name pair */
	n = xpsc.nfonts + 1;
	xpsc.fids = (Font *) XtRealloc ((void *)xpsc.fids, n * sizeof(Font));
	xpsc.fids[xpsc.nfonts] = fid;
	xpsc.fnames= (char **) XtRealloc((void *)xpsc.fnames, n*sizeof(char *));
	xpsc.fnames[xpsc.nfonts] = XtNewString (fname);
	xpsc.nfonts = n;
}

/* set up to convert subsequent X drawing calls to the given X window
 * into Postscript and send to xpsc.fp. [Xx0,Yy0] is the X coordinates of the
 * upper left corner of a window Xw pixels wide to be mapped to a rectangle Pw
 * points wide whose upper left cornder is at [Px0,Py0], also specified in
 * points. This allows any X rectangle to me mapped into any Postscript
 * rectangle, scaled but with the same aspect ratio.
 * also, fontsz specifies the size of font to use for X strings.
 * we also define some handy functions.
 */
void
XPSXBegin (win, Xx0, Xy0, Xw, Xh, Px0, Py0, Pw, fontsz)
Window win;	/* what window we are capturing */
int Xx0, Xy0;	/* ul origin of X rectangle */
int Xw;		/* width of X rectangle -- clipped if Xh > 0*/
int Xh;		/* height of X rectangle -- > 0 means to clip */
int Px0, Py0;	/* ul origin of Page rectangle */
int Pw;		/* width of Page rectangle */
int fontsz;	/* fontsize for strings, points */
{
	checkState ("Begin", OPEN);

	xpsc.win = win;
	xpsc.fontsz = fontsz;

	/* Postscript */
	fprintf (FP, "%%!\n\n");

	/* define function to do ellipses, courtesy Blue examples */
	fprintf (FP, "%% Define: x y xrad yrad a1 a2 ellipse\n");
	fprintf (FP, "/ellipsedict 8 dict def\n");
	fprintf (FP, "ellipsedict /mtrx matrix put\n");
	fprintf (FP, "/ellipse\n");
	fprintf (FP, "    { ellipsedict begin\n");
	fprintf (FP, "      /endangle exch def\n");
	fprintf (FP, "      /startangle exch def\n");
	fprintf (FP, "      /yrad exch def\n");
	fprintf (FP, "      /xrad exch def\n");
	fprintf (FP, "      /y exch def\n");
	fprintf (FP, "      /x exch def\n");
        fprintf (FP, "  \n");
	fprintf (FP, "      /savematrix mtrx currentmatrix def\n");
	fprintf (FP, "      x y translate\n");
	fprintf (FP, "      xrad yrad scale\n");
	fprintf (FP, "      0 0 1 startangle endangle arc\n");
	fprintf (FP, "      savematrix setmatrix\n");
	fprintf (FP, "      end\n");
	fprintf (FP, "    } def\n");
	fprintf (FP, "\n");

	/* define function, lstr, to place and show a l-justifed string */
	fprintf (FP, "%% Define l-justified string: (..) x y lstr\n");
	fprintf (FP, "/lstr { newpath moveto show } def\n");
	fprintf (FP, "\n");

	/* define function, rstr, to place and show a r-justifed string */
	fprintf (FP, "%% Define r-justified string: (..) x y rstr\n");
	fprintf (FP, "/rstr { newpath moveto\n");
	fprintf (FP, "  dup         %% make copies for stringwidth and show\n");
	fprintf (FP, "  stringwidth %% yields X and Y size\n");
	fprintf (FP, "  pop         %% don't need Y\n");
	fprintf (FP, "  neg 0 rmoveto %% move back\n");
	fprintf (FP, "  show        %% show the string\n");
	fprintf (FP, "} def\n");
	fprintf (FP, "\n");

	/* define function, cstr, to place and show a centered string */
	fprintf (FP, "%% Define centered string: (..) x y cstr\n");
	fprintf (FP, "/cstr { newpath moveto\n");
	fprintf (FP, "  dup         %% make copies for stringwidth and show\n");
	fprintf (FP, "  stringwidth %% yields X and Y size\n");
	fprintf (FP, "  pop         %% don't need Y\n");
	fprintf (FP, "  2 div neg 0 rmoveto %% move back halfway\n");
	fprintf (FP, "  show        %% show the string\n");
	fprintf (FP, "} def\n");
	fprintf (FP, "\n");

	/* set up for possibly-clipped X Windows coord system */
	xpsc.scale = (double)Pw/(double)Xw;
	fprintf (FP, "%% Set up to use %sX Windows coord system.\n",
						    Xh > 0 ? "clipped " : "");
	fprintf (FP,"%% [%d,%d] maps to [%d,%d], scaled by %g and Y-flipped.\n",
					    Xx0, Xy0, Px0, Py0, xpsc.scale);
	fprintf (FP, "gsave\n");
	fprintf (FP, "0 setlinewidth\n");
	fprintf (FP, "%g %g translate\n", Px0 - xpsc.scale*Xx0,
							Py0 + xpsc.scale*Xy0);
	fprintf (FP, "%g %g scale\n", xpsc.scale, -xpsc.scale);
	if (Xh > 0) {
	    /* clip but cut 'em a little slack */
	    fprintf (FP, "newpath\n");
	    fprintf (FP,
	    "  %d %d moveto %d 0 rlineto 0 %d rlineto %d 0 rlineto closepath\n",
					    Xx0-2, Xy0-2, Xw+4, Xh+4, -(Xw+4));
	    fprintf (FP, "clip\n");
	}
	fprintf (FP, "\n");

	xpsc.state = XDRAWING;
}

/* return the postscript coord system back to the original.
 * this prepares for additional direct entries or closing down.
 */
void
XPSXEnd()
{
	checkState ("End", XDRAWING);

	/* back to native PS coord system */
	fprintf (FP, "\n");
	fprintf (FP, "%% Restore native PS coord system.\n");
	fprintf (FP, "grestore\n");

	/* set a font to use */
	fprintf (FP, "\n");
	fprintf (FP, "%% Set up a font to use for remaining text.\n");
	fprintf (FP, "/%s findfont %d scalefont setfont\n",deffont,xpsc.fontsz);

	xpsc.state = OPEN;
}

/* add a raw PS string to the file.
 */
void
XPSDirect (s)
char *s;
{
	checkState ("Direct", OPEN);

	fprintf (FP, "%s", s);
}

/* finish up with any boiler plate, close the file and reset xpsc */
void
XPSClose()
{
	checkState ("Close", OPEN);

	/* add some closing boiler plate */
	fprintf (FP, "\n");
	fprintf (FP, "%% Boiler plate:\n");
	fprintf (FP, "newpath 234 %d moveto 144 0 rlineto stroke\n", AROWY(4));
	fprintf (FP,"(Created by Xephem %s) 306 %d cstr\n",PATCHLEVEL,AROWY(3));
	fprintf (FP, "(%s) 306 %d cstr\n",
			    "Copyright 1995 Elwood Charles Downey", AROWY(2));
	fprintf (FP, "(%s) 306 %d cstr\n",
	     "Xephem WWW home page: http://iraf.noao.edu/~ecdowney/xephem.html",
								    AROWY(1));
	printTime (306, AROWY(0));

	/* file is complete */
	fprintf (FP, "\n%% All finished\nshowpage\n");
	fclose (FP);
	FP = NULL;

	/* print if desired */
	if (xpsc.wantpr) {
	    char *cmd;
	    char buf[1024];
	    char msg[1024];

	    get_xmstring (print_w, XmNtextString, &cmd);
	    (void) sprintf (buf, "%s %s", cmd, xpsc.prfile);
	    XtFree (cmd);
	    (void) sprintf (msg, "Print command: \"%s\"", buf);
	    xe_msg (msg, 0);
	    if (system (buf)) {
		(void) sprintf (msg, "Error running print command:\n%s", buf);
		xe_msg (msg, 1);
	    }
	    (void) unlink (xpsc.prfile);
	    xe_msg ("Print command complete.", 1);
	}

	xpsc_close();
}

/* the following functions always perform their associated X Windows
 * function, then might also draw to the xpsc state if it matches the win.
 */
void
XPSDrawArc (dsp, win, gc, x, y, w, h, a1, a2)
Display *dsp;
Drawable win;
GC gc;
int x, y;
unsigned w, h;
int a1, a2;
{
	XArc xa;

	XDrawArc (dsp, win, gc, x, y, w, h, a1, a2);
	if (xpsc.state != XDRAWING || xpsc.win != win)
	    return;

	xa.x = x;
	xa.y = y;
	xa.width = w;
	xa.height = h;
	xa.angle1 = a1;
	xa.angle2 = a2;
	doArc (&xa, 0);
}

void
XPSDrawArcs (dsp, win, gc, xa, nxa)
Display *dsp;
Drawable win;
GC gc;
XArc xa[];
int nxa;
{
	int i;

	XDrawArcs (dsp, win, gc, xa, nxa);
	if (xpsc.state != XDRAWING || xpsc.win != win)
	    return;

	for (i = 0; i < nxa; i++)
	    doArc (&xa[i], 0);
}

void
XPSDrawLine (dsp, win, gc, x1, y1, x2, y2)
Display *dsp;
Drawable win;
GC gc;
int x1, y1;
int x2, y2;
{
	XDrawLine (dsp, win, gc, x1, y1, x2, y2);
	if (xpsc.state != XDRAWING || xpsc.win != win)
	    return;

	fprintf (FP, "newpath %d %d moveto %d %d lineto stroke\n",
								x1, y1, x2, y2);
}

void
XPSDrawLines (dsp, win, gc, xp, nxp, mode)
Display *dsp;
Drawable win;
GC gc;
XPoint xp[];
int nxp;
int mode;
{
	XDrawLines (dsp, win, gc, xp, nxp, mode);
	if (xpsc.state != XDRAWING || xpsc.win != win || nxp < 2)
	    return;

	doPoly (xp, nxp, mode, 0);
}

void
XPSDrawPoint (dsp, win, gc, x, y)
Display *dsp;
Drawable win;
GC gc;
int x, y;
{
	XArc xa;
	XDrawPoint (dsp, win, gc, x, y);
	if (xpsc.state != XDRAWING || xpsc.win != win)
	    return;

	xa.x = x;
	xa.y = y;
	xa.width = 1;
	xa.height = 1;
	xa.angle1 = 0;
	xa.angle2 = 360*64;
	doArc (&xa, 1);
}

void
XPSDrawPoints (dsp, win, gc, xp, nxp, mode)
Display *dsp;
Drawable win;
GC gc;
XPoint xp[];
int nxp;
int mode;
{
	int i;

	XDrawPoints (dsp, win, gc, xp, nxp, mode);
	if (xpsc.state != XDRAWING || xpsc.win != win || nxp == 0)
	    return;

	for (i = 0; i < nxp; i++) {
	    XArc xa;

	    xa.x = xp[i].x;
	    xa.y = xp[i].y;
	    xa.width = 1;
	    xa.height = 1;
	    xa.angle1 = 0;
	    xa.angle2 = 360*64;
	    doArc (&xa, 1);
	}
}

void
XPSDrawRectangle (dsp, win, gc, x, y, w, h)
Display *dsp;
Drawable win;
GC gc;
int x, y;
unsigned w, h;
{
	XDrawRectangle (dsp, win, gc, x, y, w, h);
	if (xpsc.state != XDRAWING || xpsc.win != win)
	    return;

	doRect (x, y, w, h, 0);
}

void
XPSDrawSegments (dsp, win, gc, xs, nxs)
Display *dsp;
Drawable win;
GC gc;
XSegment xs[];
int nxs;
{
	int i;

	XDrawSegments (dsp, win, gc, xs, nxs);
	if (xpsc.state != XDRAWING || xpsc.win != win || nxs == 0)
	    return;

	fprintf (FP, "newpath\n");
	for (i = 0; i < nxs; i++)
	    fprintf (FP, "  %d %d moveto %d %d lineto\n",
	    				xs[i].x1, xs[i].y1, xs[i].x2, xs[i].y2);
	fprintf (FP, "stroke\n");
			
}

/* draw using the font from gc which matches a previously Registered PS font.
 */
void
XPSDrawString (dsp, win, gc, x, y, s, l)
Display *dsp;
Drawable win;
GC gc;
int x, y;
char *s;
int l;
{
	unsigned long gcm;
	XGCValues gcv;
	char *fname;
	int i;

	XDrawString (dsp, win, gc, x, y, s, l);
	if (xpsc.state != XDRAWING || xpsc.win != win)
	    return;

	gcm = GCFont;
	XGetGCValues (dsp, gc, gcm, &gcv);

	for (i = 0; i < xpsc.nfonts; i++)
	    if (xpsc.fids[i] == gcv.font)
		break;
	if (i == xpsc.nfonts) {
	    /* unregistered -- use our default */
	    fname = deffont;
	} else
	    fname = xpsc.fnames[i];

	/* load desired font and take care not to scale the text */
	fprintf (FP, "newpath %d %d moveto gsave\n", x, y);
	fprintf (FP, "  %g %g scale /%s findfont %d scalefont setfont\n",
			1.0/xpsc.scale, -1.0/xpsc.scale, fname, xpsc.fontsz);
	fprintf (FP, "  (%.*s)\n", l, s);
	fprintf (FP, "show grestore\n");
}

void
XPSFillArc (dsp, win, gc, x, y, w, h, a1, a2)
Display *dsp;
Drawable win;
GC gc;
int x, y;
unsigned w, h;
int a1, a2;
{
	XArc xa;

	XFillArc (dsp, win, gc, x, y, w, h, a1, a2);
	if (xpsc.state != XDRAWING || xpsc.win != win)
	    return;

	xa.x = x;
	xa.y = y;
	xa.width = w;
	xa.height = h;
	xa.angle1 = a1;
	xa.angle2 = a2;
	doArc (&xa, 1);
}

void
XPSFillArcs (dsp, win, gc, xa, nxa)
Display *dsp;
Drawable win;
GC gc;
XArc xa[];
int nxa;
{
	int i;

	XFillArcs (dsp, win, gc, xa, nxa);
	if (xpsc.state != XDRAWING || xpsc.win != win)
	    return;

	for (i = 0; i < nxa; i++)
	    doArc (&xa[i], 1);
}

void
XPSFillPolygon (dsp, win, gc, xp, nxp, shape, mode)
Display *dsp;
Drawable win;
GC gc;
XPoint xp[];
int nxp;
int shape;
int mode;
{
	XFillPolygon (dsp, win, gc, xp, nxp, shape, mode);
	if (xpsc.state != XDRAWING || xpsc.win != win)
	    return;

	fprintf (FP, "gsave 0.9 setgray\n");
	doPoly (xp, nxp, mode, 1);
	fprintf (FP, "grestore\n");
}


void
XPSFillRectangle (dsp, win, gc, x, y, w, h)
Display *dsp;
Drawable win;
GC gc;
int x, y;
unsigned w, h;
{
	XFillRectangle (dsp, win, gc, x, y, w, h);
	if (xpsc.state != XDRAWING || xpsc.win != win)
	    return;

	doRect (x, y, w, h, 1);
}

static void
doArc (xap, fill)
XArc *xap;
int fill;
{
	if (xap->width == xap->height)
	    fprintf (FP, "newpath %g %g %g %g %g arc %s\n",
			    xap->x + xap->width/2.0, xap->y + xap->height/2.0,
			    xap->width/2.0, xap->angle1/64.0,
			    (xap->angle1 + xap->angle2)/64.0,
			    fill ? "fill" : "stroke");
	else
	    fprintf (FP, "newpath %g %g %g %g %g %g ellipse %s\n",
			    xap->x + xap->width/2.0, xap->y + xap->height/2.0,
			    xap->width/2.0, xap->height/2.0, xap->angle1/64.0,
			    (xap->angle1 + xap->angle2)/64.0,
			    fill ? "fill" : "stroke");
}

static void
doPoly (xp, nxp, mode, fill)
XPoint xp[];
int nxp;
int mode;
int fill;
{
	char *lt;
	int i;

	if (mode == CoordModeOrigin)
	    lt = "lineto";
	else if (mode == CoordModePrevious)
	    lt = "rlineto";
	else
	    return;

	fprintf (FP, "newpath %d %d moveto\n", xp[0].x, xp[0].y);
	for (i = 1; i < nxp; i++)
	    fprintf (FP, "  %d %d %s\n", xp[i].x, xp[i].y, lt);
	fprintf (FP, "%s\n", fill ? "fill" : "stroke");
}

static void
doRect (x, y, w, h, fill)
int x, y, w, h;
int fill;
{
	fprintf (FP, "newpath %d %d moveto %d %d lineto %d %d lineto\n",
				    x, y,   x+w, y,   x+w, y+h);
	fprintf (FP, "%d %d lineto %d %d lineto", x, y+h, x, y);
	fprintf (FP, " %s\n", fill ? "fill" : "stroke");
}

static void
checkState (funcname, s)
char *funcname;
XPSState s;
{
	if (xpsc.state != s) {
	    printf ("XPS%s(): state is %d but expected %d\n",
					funcname, (int)xpsc.state, (int)s);
	    exit (1);
	}
}

static void
printTime (x, y)
int x, y;
{
#if defined (__STDC__)
	time_t t;
#else
	long t;
#endif
	char *ct;
	int ctl;

	time (&t);
	ct = ctime (&t);
	ctl = strlen (ct);

	fprintf (FP, "(Generated %.*s, Local Time) %d %d cstr\n", ctl-1,ct,x,y);
}

/* prepare the named file for writing postscript into.
 * return 0 if ok, else report why with xe_msg(), reset xpsc and return -1.
 */
static int
XPSOpen (fn)
char *fn;
{
	checkState ("Open", ASKING);

	xpsc.fp = fopenh (fn, "w");
	if (!xpsc.fp) {
	    char buf[1024];

	    (void) sprintf (buf, "%s: %s", fn, syserrstr());
	    xe_msg (buf, 1);
	    xpsc_close();
	    return (-1);
	}

	xpsc.state = OPEN;

	return (0);
}

/* create the print prompt */
static void
create_print_w()
{
	Arg args[20];
	int n;

	n = 0;
	XtSetArg(args[n], XmNdefaultPosition, False); n++;
	XtSetArg(args[n], XmNdialogStyle, XmDIALOG_APPLICATION_MODAL); n++;
	print_w = XmCreatePromptDialog (toplevel_w, "Print", args, n);
	set_xmstring (print_w, XmNselectionLabelString,
					    "Enter PostScript print command:");
	XtUnmanageChild (XmSelectionBoxGetChild(print_w,XmDIALOG_HELP_BUTTON));
	XtAddCallback (print_w, XmNokCallback, prok_cb, NULL);
	XtAddCallback (print_w, XmNcancelCallback, (XtCallbackProc)no_go,NULL);
	XtAddCallback (print_w, XmNmapCallback, prompt_map_cb, NULL);
}

/* create the saveas filename prompt */
static void
create_saveas_w()
{
	Arg args[20];
	int n;

	n = 0;
	XtSetArg(args[n], XmNdefaultPosition, False); n++;
	XtSetArg(args[n], XmNdialogStyle, XmDIALOG_APPLICATION_MODAL); n++;
	saveas_w = XmCreatePromptDialog (toplevel_w, "SaveAs", args, n);
	set_xmstring (saveas_w, XmNselectionLabelString,
						"Enter PostScript file name:");
	XtUnmanageChild (XmSelectionBoxGetChild(saveas_w,XmDIALOG_HELP_BUTTON));
	XtAddCallback (saveas_w, XmNokCallback, saveasok_cb, NULL);
	XtAddCallback (saveas_w, XmNcancelCallback, (XtCallbackProc)no_go,NULL);
	XtAddCallback (saveas_w, XmNmapCallback, prompt_map_cb, NULL);
}

/* called to reset the pending print query sequence */
static void
no_go()
{
	checkState ("no_go", ASKING);
	xpsc_close();
}

/* called when the Ok button is hit in the saveas prompt */
/* ARGSUSED */
static void
saveasok_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	char *name;

	get_xmstring(w, XmNtextString, &name);

	if (existsh (name) == 0) {
	    char buf[1024];
	    (void) sprintf (buf, "%s exists: Ok to Overwrite?", name);
	    query (toplevel_w, buf, "Yes -- Overwrite", "No -- Cancel",
					    NULL, saveas_confirm, no_go, NULL);
	} else {
	    if (XPSOpen (name) == 0)
		call_go();
	}

	XtFree (name);

}

/* called when we have confirmed it's ok to clobber an existing save file */
static void
saveas_confirm()
{
	char *name;

	get_xmstring(saveas_w, XmNtextString, &name);
	if (XPSOpen (name) == 0)
	    call_go();
	XtFree (name);
}


/* called when the Ok button is hit in the print prompt */
/* ARGSUSED */
static void
prok_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	char *name = tmpnam (NULL);

	xpsc.prfile = XtNewString (name);
	if (XPSOpen (xpsc.prfile) < 0)
	    return;

	call_go();
}

/* finished with xpsc */
static void
xpsc_close()
{
	if (xpsc.wantpr && xpsc.prfile) {
	    XtFree (xpsc.prfile);
	    xpsc.prfile = NULL;
	}

	if (xpsc.nfonts > 0) {
	    int i;
	    for (i = 0; i < xpsc.nfonts; i++)
		XtFree ((void *)xpsc.fnames[i]);
	    XtFree ((void *)xpsc.fnames);
	    xpsc.fnames = NULL;
	    XtFree ((void *)xpsc.fids);
	    xpsc.fids = NULL;
	    xpsc.nfonts = 0;
	}

	xpsc.state = CLOSED;
}

/* called when it's confirmed to try and print or save */
/* ARGSUSED */
static void
call_go ()
{
	/* call the user's function */
	if (xpsc.go)
	    (*xpsc.go) ();
	else {
	    printf ("call_go() but no (*go)(). state = %d\n", (int)xpsc.state);
	    exit(1);
	}

	/* clean up if the user forgot */
	switch (xpsc.state) {
	case CLOSED:
	    break;
	case ASKING:
	    printf ("call_go() state = %d\n", (int)xpsc.state);
	    exit(1);
	    break;	/* :-) */
	case OPEN:
	    XPSClose ();
	    break;
	case XDRAWING:
	    XPSXEnd();
	    XPSClose();
	    break;
	}
}
