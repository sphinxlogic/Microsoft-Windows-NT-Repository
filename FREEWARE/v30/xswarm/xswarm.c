/*
** XSwarm
** ======
**
** Purpose:	Display a swarm of bees chasing a wasp.
**
** Features:	uses only integer math
**		can be put in the root window
**		uses non-blocking delays to decrease cpu usage and control speed
**		has no redeeming social value
**
** Comments:	I used psychoII (I assume it's a derivative of ico.) as an
**		an example of how to open up a window and draw some lines.
**		Most of this program is now totally rewritten by me, but the
**		initial window manager hints stuff is left over from the older
**		program.
**
**		Any comments, enhances, or fixes are welcome!
**
** Created:	by Jeff Butterworth on 7/11/90
**		butterwo@cs.unc.edu or butterwo@ncsc.org
**
** Updated:	by Jeff Butterworth on 8/1/90
**
*/

/* These are needed for the nap function. */
#ifdef VMS
#include <time.h>
#include <decw$include/Xlib.h>
#include <decw$include/Xatom.h>
#include <decw$include/Xutil.h>
#else
#include <sys/time.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#endif

#include <signal.h>

/* Standard Includes */
#include <stdio.h>

/* Includes for this project. */
#include "xswarm.h"

/* These variables are used to analyze the command line parameters. */
int option;
int optind = 0;
char *optarg = NULL;
char *window_title = "XSwarm, Version 1.1, by Jeff Butterworth";

char getopt(int argc, 
            char **argv, 
            char *optstring)
    {
    char *opt;
    char *pos;
    int len = strlen(optstring);
    
    /*
     * trivial case ... everything on the command line was processed
     */
    if (++optind == argc)
        {
        return(EOF);
        }
    
    /*
     * process the next thing on the argument list
     */
    else
        {
        /*
         * if argument is not an option, return EOF ... optind contains index
         * of a non-option
         */
        if (argv[optind][0] != '-')
            {
            return (EOF);
            }
        
        /*
         * if option is end-of-options, bump optind to ignore '--', and return
         * EOF
         */
        if (argv[optind][1] == '-')
            {
            ++optind;
            return(EOF);
            }
        
        /*
         * if option is not found in optstring, return '?'
         */
        if ((opt = strchr(optstring, argv[optind][1])) == 0)
            {
            return ('?');
            }
        
        /*
         * if option requires an argument, figure out where it is and set
         * optarg to point to it
         */
        if (opt[1] == ':')
            {
            int nn = 2;
            
            if (argv[optind][nn] == ':') nn = nn + 1;
            if (argv[optind][nn] != '\0')
                optarg = &argv[optind][nn];
            else
                optarg = argv[++optind];
            }
        
        /*
         * 
         * character found
         */
        return(*opt);
        }
    }


/* Main()
** process command line parameters, setup X stuff
*/

main(argc, argv)
int argc;
char **argv;
{
    char		*geom = NULL;
    int			root = FALSE;
    XSetWindowAttributes xswa;
    XWMHints		wmhints;
    unsigned char	wname[32];
    XSizeHints		sizehint;


    /* Check the command line. */
    while ((option = getopt(argc,argv,"hq?vrxg:b:a:s:A:S:d:B:w:c:C:t:")) != EOF)
    {
        switch (option)
        {
            case 'v':
                verbose = TRUE;
                break;
	    case 'r':
		root = TRUE;
		break;
	    case 'x':
		xor = TRUE;
		break;
	    case 'g':
		geom = optarg;
		break;
	    case 'b':
		bees = atoi(optarg);
		break;
	    case 'a':
		bee_acc = atoi(optarg);
		break;
	    case 's':
		bee_vel = atoi(optarg);
		break;
	    case 'A':
		wasp_acc = atoi(optarg);
		break;
	    case 'S':
		wasp_vel = atoi(optarg);
		break;
	    case 'd':
		delay = atoi(optarg) * 1000; /* convert to microseconds */
		break;
	    case 'B':
		border = atoi(optarg);
		break;
	    case 'w':
		wasp_color = optarg;
		break;
	    case 'c':
		bee_color = optarg;
		break;
	    case 'C':
		bg_color = optarg;
		break;
	    case 't':
		window_title = optarg;
		break;
            case 'h':
            case 'q':
            case '?':
                Usage(*argv);
                HandleError("The command line parameters were incorrect.",
                    FATAL);
		break;
        }
    }

    /* Display the arguments. */
    if (verbose)
    {
	printf("Here are the current settings...\n");
	if (xor)
	    printf("The logical raster operation GXxor is being used.\n");
	else
	    printf("The logical raster operation GXcopy is being used.\n");
	printf("There are %d bees.\n", bees);
	printf("Bee acceleration is %d pixels per frame per frame.\n",bee_acc);
	printf("Bee speed limit is %d pixels per frame.\n", bee_vel);
	printf("Wasp acceleration is %d pixels per frame per frame.\n",
	    wasp_acc);
	printf("Wasp speed limit is %d pixels per frame.\n", wasp_vel);
	printf("The non-blocking delay is %d milliseconds per frame.\n",
	    delay / 1000);
	printf("The wasp can't go within %d pixels of the window edge.\n",
	    border);
	printf("The wasp color is \"%s\"\n", wasp_color);
	printf("The bee color is \"%s\"\n", bee_color);
	printf("The background color is \"%s\"\n", bg_color);
    }

    /* This is a remnant of an earlier program.  I don't remember if it is
       necessary. */
    display.dname = NULL;

    winW = winH = winX = winY = 0;

    /* Open the display. */
    if (!(display.dpy = XOpenDisplay(display.dname)))
    {
	HandleError("Cannot open display.\n");
	exit(-1);
    }

    /* Set the colors. */
    display.cmap = XDefaultColormap(display.dpy, DefaultScreen(display.dpy));
    if (!display.cmap) HandleError("There was no default colormap!", FATAL);

    if (wasp_color == NULL)
	display.wasp =WhitePixel(display.dpy, DefaultScreen(display.dpy));
    else
	display.wasp = GetColor(&display, wasp_color);

    if (bee_color == NULL)
	display.bee =WhitePixel(display.dpy, DefaultScreen(display.dpy));
    else
	display.bee = GetColor(&display, bee_color);

    if (bg_color == NULL)
	display.bg =BlackPixel(display.dpy, DefaultScreen(display.dpy));
    else
	display.bg = GetColor(&display, bg_color);

    /* Set up window parameters, create and map window if necessary */
    if (!root)
    {
	winW = WINWIDTH;
	winH = WINHEIGHT;
	winX = (DisplayWidth(display.dpy,
	    DefaultScreen(display.dpy)) - winW) >> 1;
	winY = (DisplayHeight(display.dpy, 
	    DefaultScreen(display.dpy)) - winH) >> 1;
	if (geom) 
	    XParseGeometry(geom, &winX, &winY, &winW, &winH);
    }
    else
    {
	winW = DisplayWidth(display.dpy, DefaultScreen(display.dpy));
	winH = DisplayHeight(display.dpy, DefaultScreen(display.dpy));
	winX = 0;
	winY = 0;
    }

    if (!root)
    {
	xswa.event_mask = 0;
	xswa.background_pixel = display.bg;
	xswa.border_pixel = display.wasp;
	display.win = XCreateWindow(display.dpy, 
	    DefaultRootWindow(display.dpy), 
	    winX + winW + 50, winY, 
	    winW, winH, 0, 
	    DefaultDepth(display.dpy, DefaultScreen(display.dpy)), 
	    InputOutput, DefaultVisual(display.dpy, 
	    DefaultScreen(display.dpy)),
	    CWEventMask | CWBackPixel | CWBorderPixel, &xswa);

	sizehint.flags = PPosition | PSize;
	    
	XSetNormalHints(display.dpy, display.win, &sizehint);
	display.protocol_atom = XInternAtom(display.dpy, "WM_PROTOCOLS", False);
	display.kill_atom = XInternAtom(display.dpy, "WM_DELETE_WINDOW", False);
#ifdef X11R4
	XSetWMProtocols(display.dpy, display.win, &display.kill_atom, 1);
#endif

	/* Title */
	sprintf(wname, window_title);
	XChangeProperty(display.dpy, display.win,
	    XA_WM_NAME, XA_STRING, 8, PropModeReplace, wname, 
	    strlen(wname));

	/* Window Manager Hints (This is supposed to make input work.) */
	wmhints.flags = InputHint;
	wmhints.input = True;
	XSetWMHints(display.dpy, display.win, &wmhints);

	XMapWindow(display.dpy, display.win);
    }
    else
	display.win = display.dpy->screens[display.dpy->default_screen].root;

    /* Event Mask */
    XSelectInput(display.dpy, display.win, KeyPressMask | CWEventMask);

    /* Set up the bees' graphics context. */ 
    display.bee_gc = XCreateGC(display.dpy, display.win, 0, NULL); 
    XSetForeground(display.dpy, display.bee_gc, display.bee); 
    XSetBackground(display.dpy, display.bee_gc, display.bg);

    /* Set up the wasp's graphics context. */ 
    display.wasp_gc = XCreateGC(display.dpy, display.win, 0, NULL); 
    XSetForeground(display.dpy, display.wasp_gc, display.wasp); 
    XSetBackground(display.dpy, display.wasp_gc, display.bg);

    /* Set up an erasing graphics context. */
    display.erase_gc = XCreateGC(display.dpy, display.win, 0, NULL); 
    XCopyGC(display.dpy, display.bee_gc, 0xffffffff, display.erase_gc); 
    XSetForeground(display.dpy, display.erase_gc, display.bg);

    /* Set up an xor wasp graphics context. */
    display.wasp_xor_gc = XCreateGC(display.dpy, display.win, 0, NULL); 
    XCopyGC(display.dpy, display.wasp_gc, 0xffffffff, display.wasp_xor_gc); 
    XSetFunction(display.dpy, display.wasp_xor_gc, GXxor);
    XSetForeground(display.dpy, display.wasp_xor_gc, display.bg); 

    /* Set up an xor bee graphics context. */
    display.bee_xor_gc = XCreateGC(display.dpy, display.win, 0, NULL); 
    XCopyGC(display.dpy, display.bee_gc, 0xffffffff, display.bee_xor_gc); 
    XSetFunction(display.dpy, display.bee_xor_gc, GXxor);
    XSetForeground(display.dpy, display.bee_xor_gc, display.bg); 

    /* Clear the background. */
    if (!xor)
    {
	XSetWindowBackground(display.dpy, display.win, display.bg);
	XFillRectangle(display.dpy, display.win, display.erase_gc,
	    0,0, winW, winH);
    }

    /* Animate the swarm. */
    Animate();
}



Animate()
{
    register int	b;		/* bee index */
    XSegment		*segs;		/* bee lines */
    XSegment		*old_segs;	/* old bee lines */
    short		*x, *y;		/* bee positions x[time][bee#] */
    short		*xv, *yv;	/* bee velocities xv[bee#] */
    short		wx[3], wy[3];
    short		wxv, wyv;
    XEvent		xev;
    short		dx,dy,distance;
    int			init = TRUE;


    /* Get the random number generator ready. */
#ifdef VMS
    srand((int) time(0) % 231);
#else
    srandom((int) time(0) % 231);
#endif
    /* Allocate memory. */
    segs = (XSegment *) malloc(sizeof(XSegment) * bees);
    old_segs = (XSegment *) malloc(sizeof(XSegment) * bees);
    x = (short *) malloc(sizeof(short) * bees * repeat);
    y = (short *) malloc(sizeof(short) * bees * repeat);
    xv = (short *) malloc(sizeof(short) * bees);
    yv = (short *) malloc(sizeof(short) * bees);

    /* Initialize point positions, velocities, etc. */

    /* wasp */
#ifdef VMS
    wx[0] = BORDER + rand() % (winW - 2*BORDER);
    wy[0] = BORDER + rand() % (winH - 2*BORDER);
#else
    wx[0] = BORDER + random() % (winW - 2*BORDER);
    wy[0] = BORDER + random() % (winH - 2*BORDER);
#endif
    wx[1] = wx[0];
    wy[1] = wy[0];
    wxv = 0;
    wyv = 0;

    /* bees */
    for (b = 0 ; b < bees ; b++)
    {
#ifdef VMS
	X(0,b) = rand() % winW;
#else
        X(0,b) = random() % winW;
#endif
	X(1,b) = X(0,b);
#ifdef VMS
        Y(0,b) = rand() % winH;
#else
	Y(0,b) = random() % winH;
#endif
	Y(1,b) = Y(0,b);
	xv[b] = RAND(7);
	yv[b] = RAND(7);
    }

    /* Seemingly endless loop. */
    for (;;)
    {
	/* <=- Wasp -=> */
	/* Age the arrays. */
	wx[2] = wx[1];
	wx[1] = wx[0];
	wy[2] = wy[1];
	wy[1] = wy[0];
	/* Accelerate */
	wxv += RAND(wasp_acc);
	wyv += RAND(wasp_acc);

	/* Speed Limit Checks */
	if (wxv > wasp_vel) wxv = wasp_vel;
	if (wxv < -wasp_vel) wxv = -wasp_vel;
	if (wyv > wasp_vel) wyv = wasp_vel;
	if (wyv < -wasp_vel) wyv = -wasp_vel;

	/* Move */
	wx[0] = wx[1] + wxv;
	wy[0] = wy[1] + wyv;

	/* Bounce Checks */
	if ((wx[0] < border) || (wx[0] > winW-border-1))
	{
	    wxv = -wxv;
	    wx[0] += wxv;
	}
	if ((wy[0] < border) || (wy[0] > winH-border-1))
	{
	    wyv = -wyv;
	    wy[0] += wyv;
	}

	/* Don't let things settle down. */
#ifdef VMS
	xv[rand() % bees] += RAND(3);
	yv[rand() % bees] += RAND(3);
#else
	xv[random() % bees] += RAND(3);
	yv[random() % bees] += RAND(3);
#endif
	/* <=- Bees -=> */
	for (b = 0 ; b < bees ; b++)
	{
	    /* Age the arrays. */
	    X(2,b) = X(1,b);
	    X(1,b) = X(0,b);
	    Y(2,b) = Y(1,b);
	    Y(1,b) = Y(0,b);

	    /* Accelerate */
	    dx = wx[1] - X(1,b);
	    dy = wy[1] - Y(1,b);
	    distance = abs(dx)+abs(dy); /* approximation */
	    if (distance == 0) distance = 1;
	    xv[b] += (dx*bee_acc)/distance;
	    yv[b] += (dy*bee_acc)/distance;

	    /* Speed Limit Checks */
	    if (xv[b] > bee_vel) xv[b] = bee_vel;
	    if (xv[b] < -bee_vel) xv[b] = -bee_vel;
	    if (yv[b] > bee_vel) yv[b] = bee_vel;
	    if (yv[b] < -bee_vel) yv[b] = -bee_vel;

	    /* Move */
	    X(0,b) = X(1,b) + xv[b];
	    Y(0,b) = Y(1,b) + yv[b];

	    /* Fill the segment lists. */
	    segs[b].x1 = X(0,b);
	    segs[b].y1 = Y(0,b);
	    segs[b].x2 = X(1,b);
	    segs[b].y2 = Y(1,b);
	    old_segs[b].x1 = X(1,b);
	    old_segs[b].y1 = Y(1,b);
	    old_segs[b].x2 = X(2,b);
	    old_segs[b].y2 = Y(2,b);
	}

	/* Erase previous, draw current, sync for smoothness. */

	/* Wasp */
	if (xor)
	{
	    if (init == FALSE)
		XDrawLine(display.dpy, display.win, display.wasp_xor_gc,
		    wx[1], wy[1], wx[2], wy[2]);
	    XDrawLine(display.dpy, display.win, display.wasp_xor_gc,
		wx[0], wy[0], wx[1], wy[1]);
	}
	else
	{
	    if (init == FALSE)
		XDrawLine(display.dpy, display.win, display.erase_gc,
		    wx[1], wy[1], wx[2], wy[2]);
	    XDrawLine(display.dpy, display.win, display.wasp_gc,
		wx[0], wy[0], wx[1], wy[1]);
	}

	/* Bees */
	if (xor)
	{
	    if (init == FALSE)
		XDrawSegments(display.dpy, display.win, display.bee_xor_gc,
		    old_segs, bees);
	    else
		init = FALSE;
	    XDrawSegments(display.dpy, display.win, display.bee_xor_gc,
		segs, bees);
	}
	else
	{
	    if (init == FALSE)
		XDrawSegments(display.dpy, display.win, display.erase_gc,
		    old_segs, bees);
	    else
		init = FALSE;
	    XDrawSegments(display.dpy, display.win, display.bee_gc, segs, bees);
	}

	XSync(display.dpy, False);

	/* Check for events. */
	if (XPending(display.dpy))
	{
	    XNextEvent(display.dpy, &xev);
	    HandleEvent(&xev);
	}

	/* Clean up and shut down. */
	if (stop)
	{
	    if (xor)
	    {
		XDrawSegments(display.dpy, display.win, display.bee_xor_gc,
		    segs, bees);
		XDrawLine(display.dpy, display.win, display.wasp_xor_gc,
		    wx[0], wy[0], wx[1], wy[1]);
	    }
	    else
		XFillRectangle(display.dpy, display.win, display.erase_gc,
		    0,0, winW, winH);
	    XSync(display.dpy, 0);
	    exit(0);
	}

	/* Delay so we don't use all of the cpu time. */
	if (delay != 0) nap(0,delay);
    }
}


/*
** HandleEvent()
**
** process X events
*/

HandleEvent(event)
XEvent	*event;
{
    switch (event->type)
    {
	case ClientMessage: /* sent by f.delete from twm */
	{
	    XClientMessageEvent	*ev = (XClientMessageEvent *) event;
	    if (ev->message_type == display.protocol_atom && ev->data.l[0] == display.kill_atom)
		stop = TRUE;
	}
	break;
	case KeyPress:
	{
	    XKeyEvent *key_event = (XKeyEvent *) event;
            char buf[128];
            KeySym ks;
            XComposeStatus status;

            XLookupString(key_event,buf,128,&ks,&status);
            if (buf[0]=='q' || buf[0]=='Q')
		stop = TRUE;
	}
	break;
	default:
	break;
    }
}


/* nap
**
** put the process to sleep for a while
*/

void catchalarm() { }	/* SIGALRM (null) signal handler */

void nap(sec,usec)
long sec, usec;
{
#ifdef unix
    extern int setitimer();
    extern void sigpause();
    struct itimerval wait;
    static int sigset = FALSE;

    /* If usec is larger than 1 second: */
    while(usec >= 1e6) { sec++;  usec -= 1e6; }

    if(!sigset) {
	signal(SIGALRM,catchalarm);
	sigset = TRUE;
    }

    wait.it_value.tv_sec = sec;
    wait.it_value.tv_usec = usec;
    wait.it_interval.tv_sec = 0;
    wait.it_interval.tv_usec = 0;

    if((setitimer(ITIMER_REAL,&wait,NULL)) == -1) {
	HandleError("bad setitimer()", FATAL);
    }

    sigpause(0);
#else
    /* If usec is larger than 1 second: */
  while(usec >= 1e6) { sec++;  usec -= 1e6; }
  /* ok, it's not the same but it seems to work */
  sleep(sec);
#endif
}


void
Usage(program)
char *program;
{
    printf("\n%s [options]\n\n", program);
    printf("-r		use root window\n");
    printf("-d delay	non-blocking delay between screen updates in msec\n");
    printf("		(Less than 10 is dangerous!)\n");
    printf("-x		use the logical xor raster operation\n");
    printf("		(This is buggy, but it leaves your root bitmap alone!)\n");
    printf("-g geom		window geometry\n");
    printf("-b bees		number of bees\n");
    printf("-a bee_acc	bee acceleration in pixels per frame per frame\n");
    printf("-s bee_vel	bee speed limit in pixels per frame\n");
    printf("-A wasp_acc	wasp max acceleration in pixels per frame per frame\n");
    printf("-S wasp_vel	wasp speed limit in pixels per frame\n");
    printf("-B width	border width that wasp can't cross\n");
    printf("-w wasp_clr	wasp color\n");
    printf("-c bee_clr	bee color\n");
    printf("-C bg_color	background color\n");
    printf("-t title        window title\n");
    printf("-h|q|?		display this message\n");
    printf("\nPress q in the window to stop the insanity.\n\n");
}


void
HandleError(description, degree)
char    *description;
int     degree;
{
    fprintf(stderr, "An error has occurred.  The description is below...\n");
    fprintf(stderr, "%s\n", description);

    if (degree == FATAL)
    {
        fprintf(stderr, "Program aborting...\n");
        exit(-1);
    }
}

long
GetColor(display, color)
disp		*display;
char		*color;
{
    XColor	cdef;
    char	error_str[STD_STR];

    if (XParseColor(display->dpy, display->cmap, color, &cdef) &&
	XAllocColor(display->dpy, display->cmap, &cdef))
	return(cdef.pixel);
    else
    {
	sprintf(error_str, "Color \"%s\" wasn't found.", color);
	HandleError(error_str, FATAL);
    }
}

