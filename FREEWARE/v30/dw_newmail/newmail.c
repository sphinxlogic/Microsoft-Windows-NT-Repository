#module newmail "V4.0"

/*
 * Periodically check the VMS MAIL new-messages count.
 *
 * dave porter  3-Mar-1989
 * updated for DECwindows Motif, 12-Sep-1991
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <ssdef.h>
#include <maildef.h>
#include <decw$include/Xlib.h>
#include <decw$include/Xutil.h>
#include <decw$include/Xatom.h>
#include <decw$include/Xresource.h>

#define icon_width 32
#define icon_height 32
static char icon_bits[] = {
   0xff, 0xff, 0xff, 0xff, 0x01, 0x00, 0x00, 0x80, 0x71, 0x1c, 0x00, 0x80,
   0x89, 0x22, 0x00, 0x80, 0x89, 0x22, 0x00, 0x80, 0x41, 0x10, 0x00, 0x80,
   0x21, 0x08, 0x00, 0x80, 0x21, 0x08, 0x00, 0x80, 0x21, 0x08, 0x00, 0x80,
   0x01, 0x00, 0x00, 0x80, 0x21, 0x08, 0x00, 0x80, 0x21, 0x08, 0x00, 0x80,
   0x01, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0x80, 0xc1, 0xff, 0xff, 0xbf,
   0x41, 0x00, 0x00, 0xa0, 0x41, 0x00, 0x00, 0xa6, 0x41, 0x00, 0x00, 0xa6,
   0x41, 0x00, 0x00, 0xa6, 0x41, 0xfe, 0x07, 0xa0, 0x41, 0x00, 0x00, 0xa0,
   0x41, 0xfe, 0x7f, 0xa0, 0x41, 0x00, 0x00, 0xa0, 0x41, 0xfe, 0x07, 0xa0,
   0x41, 0x00, 0x00, 0xa0, 0x41, 0xfe, 0x00, 0xa0, 0x41, 0x00, 0x00, 0xa0,
   0x41, 0x00, 0x00, 0xa0, 0xc1, 0xff, 0xff, 0xbf, 0x01, 0x00, 0x00, 0x80,
   0x01, 0x00, 0x00, 0x80, 0xff, 0xff, 0xff, 0xff};

#define DEFAULTS "decw$user_defaults:newmail.dat"

Display *dpy;
Screen *scr;
Window win;
GC gc;
XrmDatabase db;

static ww, wh, wx, wy;
static tw, th, tx, ty;
static tick;
static msgcount;
static leaveclosed;
static mapped;

void InitDisplay(int, char *[]);
int GetOptions(int, char *[]);
char *GetResource(char *, char *);
int AlloColour(char *);
int TrueOrFalse(char *);
void SetProps(char *, int);
void HandleEvents(void);
void TimerAst(void);
void Replot(void);
void Resize(int, int);
int NewMessages(void);
void Slimdown(void);

/*
 * Main routine
 */
main(argc, argv)
int argc;
char *argv[];
{
    InitDisplay(argc, argv);
    Slimdown();
    TimerAst();
    HandleEvents();

}


/*
 * Initialise all display and window resources
 *
 */
void InitDisplay(argc, argv)
int argc;
char *argv[];
{
    int tmp;
    XSetWindowAttributes attr;
    Font font;
    XCharStruct textinfo;
    XGCValues gcval;
    int fg_pix, bg_pix;
    char defgeo[32];
    char *dname, *geometry, *fname, *wname;

    if (GetOptions(argc, argv)==0) exit(-1);

    dname = GetResource("display", NULL);
    dpy = XOpenDisplay(dname);
    if (dpy==NULL) {
	printf("can't open display %s\n", (dname?dname:""));
	exit(-1);
    }

    scr = XDefaultScreenOfDisplay(dpy);

    bg_pix = AlloColour(GetResource("background", "White"));
    if (bg_pix<0) exit(-1);
    fg_pix = AlloColour(GetResource("foreground", "Black"));
    if (fg_pix<0) exit(-1);

    fname = GetResource("font", "-Adobe-Courier-Bold-R-Normal--*-240-*-*-M-*-ISO8859-1");
    font = XLoadFont(dpy, fname);
    if (font==0) {
	printf("can't load font %s\n", fname);
	exit(-1);
    }

    XQueryTextExtents(dpy, font, "99", 2, &tmp, &tmp, &tmp, &textinfo);
    th = textinfo.ascent + textinfo.descent;
    tw = textinfo.width;
    tx = 0;
    ty = 0;

    geometry = GetResource("geometry", "");
    sprintf(defgeo, "=%dx%d+%d+%d", (tw+10<100?100:tw+10), th+10, 100, 100);
    XGeometry(
	     dpy, XDefaultScreen(dpy), 
	     geometry, defgeo, 
	     0, 1, 1, 0, 0, 
	     &wx, &wy, &ww, &wh);

    attr.event_mask = ExposureMask|StructureNotifyMask;
    attr.background_pixel = bg_pix;

    win = XCreateWindow(
		       dpy, XRootWindowOfScreen(scr), 
		       wx, wy, ww, wh, 0,
		       XDefaultDepthOfScreen(scr),
		       InputOutput,
		       XDefaultVisualOfScreen(scr),
		       CWEventMask|CWBackPixel, &attr
		       );

    gcval.foreground = fg_pix;
    gcval.font = font;

    gc = XCreateGC(dpy, win, GCForeground|GCFont, &gcval);

    wname = GetResource("title", "NewMail");

    SetProps(wname, strlen(geometry));

    Resize(ww, wh);

    tick = atoi(GetResource("update", "30"));
    if (tick<1 || tick>300) {
	printf("illegal update interval %d\n", tick);
	exit(-1);
    }

    mapped = 0;
}


/*
 * Routine to get all options and store them on a
 * resource manager database
 *	- newmail user defaults file
 *	- command line options
 *
 */
int GetOptions(argc, argv)
int argc;
char *argv[];
{
    static XrmOptionDescRec options[] =
		{
		{"-background", ".background", XrmoptionSepArg, (caddr_t) NULL},
		{"-bg",         ".background", XrmoptionSepArg, (caddr_t) NULL},
		{"-d      ",    ".display",    XrmoptionSepArg, (caddr_t) NULL},
		{"-display",    ".display",    XrmoptionSepArg, (caddr_t) NULL},
    		{"-fn",         ".font",       XrmoptionSepArg, (caddr_t) NULL},
    		{"-font",       ".font",       XrmoptionSepArg, (caddr_t) NULL},
    		{"-foreground", ".foreground", XrmoptionSepArg, (caddr_t) NULL},
    		{"-fg",         ".foreground", XrmoptionSepArg, (caddr_t) NULL},
		{"-geometry",	".geometry",   XrmoptionSepArg, (caddr_t) NULL},
		{"-t",		".title",      XrmoptionSepArg, (caddr_t) NULL},
		{"-title",	".title",      XrmoptionSepArg, (caddr_t) NULL},
		{"-geometry",	".geometry",   XrmoptionSepArg, (caddr_t) NULL},
		{"-update",	".update",     XrmoptionSepArg, (caddr_t) NULL}
                };
    static optcount = (sizeof options) / (sizeof (XrmOptionDescRec));

    XrmInitialize();

    db = XrmGetFileDatabase(DEFAULTS);

    XrmParseCommand(&db, options, optcount, "newmail", &argc, argv);

    if(argc>1) {
	printf("\nusage:  %s [-option ...]\n\n", argv[0]);
	printf("where -option includes:\n\n");
	printf("          -d <string>\n");
	printf("          -display <string>\n");
	printf("          -fn <fontname>\n");
	printf("          -font <fontname>\n");
	printf("          -foreground <colour>\n");
	printf("          -fg <colour>\n");
	printf("          -background <colour>\n");
	printf("          -bg <colour>\n");
	printf("          -t <string>\n");
	printf("          -title <string>\n");
	printf("          -geometry <geomstring>\n");
	printf("          -update <seconds>\n");
	printf("\n");
        return 0;
    }

    return 1;

}


/*
 * Routine to get one resource value, supplying
 * a default if it's not defined
 * 
 */
char *GetResource(resrc, def)
char *resrc, *def;
{
    XrmValue value;
    char *reptype;
    char fullname[64], fullclass[64];
    char *ptr;
    int up;
    
    strcpy(fullname, "newmail.");
    strcat(fullname, resrc);
    strcpy(fullclass, fullname);

    ptr = fullclass;	/* capitalize class name! */
    up  = 1;
    while (*ptr) {
	if (up && islower(*ptr)) *ptr = _toupper(*ptr);
	up = *ptr++ == '.';
    }

    if (XrmGetResource(db, fullname, fullclass, &reptype, &value))
	return value.addr ;

    if (dpy) {
	ptr = XGetDefault(dpy, "newmail", resrc);
	if (ptr) return ptr;
    }

    return def;

}


/*
 * Parse colour specifier (name or #nnnn)
 * and allocate shareable colour map entry
 *
 */
int AlloColour(str)
char *str;
{
    Colormap cmap;
    XColor colour;
    int tmp;

    cmap = XDefaultColormapOfScreen(scr);

    tmp = XParseColor(dpy, cmap, str, &colour);
    if (tmp==0) {
	printf("can't parse colour %s\n", str);
	return -1;
    }

    tmp = XAllocColor(dpy, cmap, &colour);
    if (tmp==0) {
	printf("can't allocate colour %s\n", str);
	return -1;
    }

    return colour.pixel;   

}


/*
 * Parse true/false string
 */
TrueOrFalse(str)
char *str;
{
    char s[8];
    int i;

    for (i=0; i<8; i++) s[i] = _tolower(str[i]);

    if (strncmp(str, "true", 8) == 0)
	return (1);

    if (strncmp(str, "yes", 8) == 0)
	return (1);

    return (0);
}
 
	
/*
 * Set window properties
 *
 */
void SetProps(char *title, int userflag)
{
    XSizeHints sizehints;
    XWMHints wmhints;

    /* size hints */
    sizehints.flags = PPosition|PSize|PMinSize;
    if (userflag) sizehints.flags = USPosition|USSize|PMinSize;
    sizehints.x          = wx;
    sizehints.y          = wy;
    sizehints.width      = ww;
    sizehints.height     = wh;
    sizehints.min_width  = th;
    sizehints.min_height = th;
    XSetNormalHints(dpy, win, &sizehints);

    /* icon and window names */
    XStoreName(dpy, win, title);
    XSetIconName(dpy, win, title);

    /* window manager hints */
    wmhints.flags = InputHint|IconPixmapHint;
    wmhints.input = False;
    wmhints.icon_pixmap = XCreateBitmapFromData(
			 		       dpy,
			 		       XRootWindowOfScreen(scr),
					       icon_bits,
					       icon_width, icon_height
					       );
    XSetWMHints(dpy, win, &wmhints);

}


/*
 * Process window events
 *
 */
void HandleEvents()
{
    XEvent event;

    for(;;) {
	XNextEvent(dpy, &event);

	switch (event.type) {

	case Expose: 
	    if (event.xexpose.count==0) Replot(); 
	    break;

	case ConfigureNotify:
	    if (event.xconfigure.width!=ww || event.xconfigure.height!=wh) 
		Resize(event.xconfigure.width, event.xconfigure.height);
	    break;

	case ReparentNotify:
	    if (mapped)	XRaiseWindow(dpy, win);
	    break;

	}
    }

}


/*
 *  Process timer ASTs
 *
 */
void TimerAst()
{
    int timer[2], oldcount;
    
    timer[0] = -10000000 * tick;
    timer[1] = -1;

    oldcount = msgcount;
    msgcount = NewMessages();
 
    if (msgcount!=oldcount) {

	if (msgcount)
	    if (mapped) {
		Replot();
		XRaiseWindow(dpy, win);
	    }
	    else {
		XMapRaised(dpy, win);
		mapped = 1;
	    }

	else
	    if (mapped) {
		XUnmapWindow(dpy, win);
		XWithdrawWindow(dpy, win, XDefaultScreen(dpy));
		mapped = 0;
	    }
	    else
		printf("huh?\n");

	XFlush(dpy);
    }

    sys$setimr(0, timer, TimerAst, 0, 0);

}


/*
 * Replot the window contents.  Called from exposure handler,
 * and from the timer AST if the count changes.
 *
 */
void Replot()
{
    char buff[16];

    XClearWindow(dpy, win);
    if (msgcount<99) 
	sprintf(buff, "%02d", msgcount);
    else
	sprintf(buff, "**");
    XDrawString(dpy, win, gc, tx, ty, buff, strlen(buff));

}


/*
 * Window resized, so compute text origin again
 *
 */
void Resize(neww, newh)
{
    ww = neww;
    wh = newh;

    tx = (ww - tw) / 2;	
    ty = (wh + th) / 2;
}


/*
 * Determine current new message count.
 * (Note: not re-entrant, so call only at AST level 
 *  or disable ASTs first).
 *
 */
int NewMessages()
{
    static int ctx = 0;
    int count = 0;
    struct {int item; int *buff, *retlen; int term;} usritem;
    struct {int nuthin;} nulitem;

    nulitem.nuthin = 0;

    usritem.item   = (MAIL$_USER_NEW_MESSAGES<<16) + 2;
    usritem.buff   = &count;
    usritem.retlen = NULL;
    usritem.term   = 0;

    if (ctx==0) 
	mail$user_begin(&ctx, &nulitem, &usritem);
    else
    	mail$user_get_info(&ctx, &nulitem, &usritem);

    if (leaveclosed) {
    	mail$user_end(&ctx, &nulitem, &nulitem);
    	ctx = 0;
    }

    return count;
}


/*
 * Delete resource database and purge working set 
 * (called after initialization)
 */
void Slimdown()
{
    long inadr[2];
    int status;

    inadr[0] = 0x00000000;
    inadr[1] = 0x7fffffff;

    if (db) {
        XrmFreeDatabase(db);
        db = NULL;
    }

    if (dpy->db) {
	XrmFreeDatabase(dpy->db);
	dpy->db = NULL;
    }

    status = sys$purgws(inadr);
    if ((status&1)==0) printf("WS purge failed\n"); 

}
