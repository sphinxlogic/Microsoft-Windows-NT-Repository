/*****************************************************************************/
/**                Copyright 1990 by Solbourne Computer, Inc		    **/
/**                           Longmont, Colorado                            **/
/**                                                                         **/
/**                           All Rights Reserved                           **/
/**                                                                         **/
/**    Permission to use, copy, modify, and distribute this software and    **/
/**    its documentation  for  any  purpose  and  without  fee is hereby    **/
/**    granted, provided that the above copyright notice appear  in  all    **/
/**    copies and that both  that  copyright  notice  and  this  permis-    **/
/**    sion  notice appear in supporting  documentation,  and  that  the    **/
/**    name  of  Solbourne  not  be  used  in  advertising  in publicity    **/
/**    pertaining  to  distribution  of  the software  without specific,    **/
/**    written prior permission.                                            **/
/**                                                                         **/
/**    SOLBOURNE  DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,    **/
/**    INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND  FITNESS,    **/
/**    IN  NO  EVENT SHALL SOLBROUNE BE LIABLE FOR ANY SPECIAL, INDIRECT    **/
/**    OR CONSEQUENTIAL DAMAGES OR  ANY  DAMAGES  WHATSOEVER   RESULTING    **/
/**    FROM LOSS OF USE,  DATA OR  PROFITS,  WHETHER  IN  AN  ACTION  OF    **/
/**    CONTRACT, NEGLIGENCE  OR OTHER TORTIOUS ACTION, ARISING OUT OF OR    **/
/**    IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.          **/
/*****************************************************************************/

/*
 *	xshuffle.c -- makes your display into one of those 16 square puzzles.
 *
 *	Author		: M. Warner Losh	(imp@Solbourne.COM)
 *	Version		: V01.00
 *	Version date	: Wed Aug 15 10:58:36 1990
 *	Creation date	: Sun Aug  5 10:00:03 1990
 *
 *	Usage:	xshuffle
 *			[-numsquares {number-of-squares}] Num squares both ways
 *			[-xnumsquares {num-squares-x}]	Num squares across
 *			[-ynumsquares {num-squares-y}]	Num squares down
 *			[-display {display}]		Display to use
 *			[-border]			Draw a border
 *			[-solve]			Undo moves at end
 *			[-xmax n]			Width of area
 *			[-ymax n]			Height of area
 *			[-max n]			Size of square area
 *                      [-moves n]                      Maximum moves to make
 */

/* Include files */

# include <stdio.h>
# include <signal.h>
# include <X11/Xlib.h>
# include <X11/Xatom.h>

/*
 * Macro Definitions
 */

/* Config parameters. */

#define NUM_SQUARES 16			/* Default numberof squares */
#define MAX_SOLVE	1024*64	        /* Max number of moves saved (1024*64)*/

/* Magic numbers */
#define DOWN 0				/* Move down */
#define UP 1				/* Move up */
#define RIGHT 2				/* Move rigth */
#define LEFT 3				/* Move left */

#define MODE_SCRAMBLE	0		/* We're scrambling now */
#define MODE_SOLVE	1		/* We're solving now */
#define MODE_EXIT	2		/* Time to exit */

#define X_FLAG	1			/* X coord specified */
#define Y_FLAG	2			/* Y coord specified */

/*
 * Variables
 */
int moves[MAX_SOLVE];			/* Saved moves. */
int cur_move = 0;			/* Current move number */
int max_moves = MAX_SOLVE;		/* Max moves to make */
int solve = 0;				/* True when we are solving puzzle */
int mode = MODE_SCRAMBLE;		/* Mode we are in */
int boarder_width = 2;			/* Size of the boarder */
int nxsquares = NUM_SQUARES;		/* Number of squares across */
int nysquares = NUM_SQUARES;		/* Number of squares veritcally */
int speed = 1;				/* How fast to move.  Keep at 1 */
int last = -1;				/* What was the last move */
int x_size;				/* Size in pixels of square */
int y_size;				/* Size in pixels of square */
int y_max;				/* Biggest y we have. */
int x_max;				/* Biggest x we have. */
int x;					/* current X of upper left of blank */
int y;					/* Current Y of upper left of blank*/
int x_start = 0;			/* Start X location */
int y_start = 0;			/* Start Y location */

Window w_root;				/* Puzzle window */
Window root;				/* Real root window */
Display *dpy;				/* display connection */
GC mygc;				/* Drawing GC */
Pixmap pm = 0;				/* Pixmap to save first square */
unsigned long fg, bg;			/* Foreground and background pixel */
unsigned long scr;			/* Screen number */
int (*old_handler)();			/* Old Error handler We restore this */

/* Function Declarations */

int create_pm_handler ();
void die_usage ();
void done ();
int main ();
void move_a_square ();
void move_down ();
void move_left ();
void move_right ();
void move_up ();
void process_event ();
void rand_move_a_square ();
void save_move ();
void time_to_go();
void undo_move_a_square ();
int unique_match ();
void usage ();

/**********************************************************************
 *     create_pm_handler () --- This is the handler for the one
 * 				XCreatePixmap request that we have.
 * 				All it does is to set pm to zero.  It
 * 				assumes that the only error it can get
 * 				is from CreatePixmap.  That is the
 * 				reason for the XSyncs around the
 * 				XCreatePixmap.
 * 
 * Arguments:
 * 
 *     dpy		    -i- Display the error happened on
 *     e		    -i- Event that caused us to get called.
 * 
 * Returns:
 * 
 */

int create_pm_handler (dpy, e)
Display *dpy;
XErrorEvent *e;
{
	pm = 0;
        return;
}    /* end of create_pm_handler () */

/**********************************************************************
 *     time_to_go () --- This function is the signal handler for ^C.
 * 			 It gets called when the user wants to exit.
 * 			 If the user requested solving of the puzzle,
 * 			 then we just bump the mode one.  Otherwise,
 * 			 we set the mode to exit.
 * 
 * Arguments:
 * 
 *     sig	     -i- Signal number that caused us to execute.
 *     code	     -i- Code for that signal
 *     scp	     -i- Signal contexted
 *     addr	     -i- Address of the exception.
 */

void time_to_go (sig, code, scp, addr)
int sig;
int code;
struct sigcontext *scp;
char *addr;
{
	if (solve)
		mode++;
	else
		mode = MODE_EXIT;
}    /* end of time_to_go () */

/**********************************************************************
 *     done () --- Gets called when we are done.  This function sleeps
 * 		   a while if we just got done solving the puzzle.
 * 		   Then it maps a big window over all of the other
 * 		   windows to force them to repaint.  Then it exits.
 */

void done ()
{
	Window w;		/* big window */
	XSetWindowAttributes att; /* Attributes for the window */
	Visual visual;		/* Visual info */
	unsigned long mask;	/* Mask for atributes */

	/*
	 * If we just got done solving the puzzle, then we wait a
	 * while.  We also flush all of Xlib's buffers.
	 */
	if (solve && cur_move == 0)
	{
		if (pm)
			XCopyArea (dpy, pm, w_root, mygc, 0, 0, x_size, y_size,
				   x_start, y_start);
		XSync(dpy, False);
		sleep (3);
	}

	/*
	 * Free the Pixmap if we need to.
	 */
	if (pm)
		XFreePixmap (dpy, pm);
	
	/*
	 * Create an override redirect window, map it over all the
	 * clients, then unmap it.  This will cause the screen to
	 * refresh.  Exit when done.
	 */
/*
	att.override_redirect = True;
	att.backing_store = False;
	att.save_under = False;
	mask = CWOverrideRedirect | CWBackingStore | CWSaveUnder;
	visual.visualid = CopyFromParent;
	w = XCreateWindow (dpy, w_root, 0, 0, 9999, 9999, 0,
			   DefaultDepth (dpy, scr), InputOutput, &visual,
			   mask, &att);
	XMapRaised (dpy, w);
	XSync(dpy, False);
	XDestroyWindow(dpy, w);
*/
	exit(1);
}    /* end of done () */

/**********************************************************************
 *     main () --- Main routine.
 * 
 * Arguments:
 * 
 *     argc    -i- Num args
 *     argv    -i- args
 */

int main (argc, argv)
int argc;
char **argv;
{
	int i;			/* Index for loops */
	int do_border = False;	/* Draw a border around all the squares */
	char *display = NULL;	/* Text of display to use. */
	int max_flag = 0;	/* Flag for what max's are specified. */
	XSetWindowAttributes att; /* Attributes for the window */
	Visual visual;		/* Visual info */
	unsigned long mask;	/* Mask for atributes */

	
	/*
	 * Parse the arguments.  This is a real kludge right now, so
	 * please be kind...
	 */
	while (--argc)
	{
		if (unique_match(*++argv, "-numsquares", 2))
		{
			if (--argc) {
				nysquares = nxsquares = atoi(*++argv);
				if (nysquares < 2) {
				  nysquares = nxsquares = 2;
				}
			}
			else
				die_usage ("-numsquares requires another arg");
			continue;
		}
		if (unique_match(*argv, "-moves", 2))
		{
			if (--argc) {
				max_moves = atoi (*++argv);
                                if (max_moves < 2) {
                                max_moves = 2;
                                }
                        }
			else
				die_usage ("-moves requires another arg");
			continue;
		}
		if (unique_match(*argv, "-display", 2))
		{
			if (--argc)
				display = *++argv;
			else
				die_usage ("-display requires another arg");
			continue;
		}
		if (unique_match (*argv, "-xnumsquares", 3))
		{
			if (--argc) {
				nxsquares = atoi (*++argv);
				if (nxsquares < 2) {
				  nxsquares = 2;
				}
			}
			else
				die_usage("-xnumsquares requires another arg");
			continue;
		}
		if (unique_match (*argv, "-ynumsquares", 3))
		{
			if (--argc) {
				nysquares = atoi (*++argv);
				if (nysquares < 2) {
				  nysquares = 2;
				}
			}
			else
				die_usage("-ynumsquares requires another arg");
			continue;
		}
		if (unique_match (*argv, "-border", 2))
		{
			do_border = True;
			continue;
		}
		if (unique_match (*argv, "-solve", 2))
		{
			solve = 1;
			continue;
		}
		if (unique_match (*argv, "-max", 2))
		{
			if (--argc)
			{
				max_flag |= X_FLAG | Y_FLAG;
				x_max = y_max = atoi (*++argv);
			}
			else
				die_usage("-max requires another arg");
			continue;
		}
		if (unique_match (*argv, "-ymax", 3))
		{
			if (--argc)
			{
				max_flag |= Y_FLAG;
				y_max = atoi (*++argv);
			}
			else
				die_usage("-ymax requires another arg");
			continue;
		}
		if (unique_match (*argv, "-xmax", 3))
		{
			if (--argc)
			{
				max_flag |= X_FLAG;
				x_max = atoi (*++argv);
			}
			else
				die_usage("-xmax requires another arg");
			continue;
		}
		fprintf (stderr,"Unknown option %s\n", *argv);
		usage();
	}
	
	/*
	 * Setup the random number seed.
	 */
	srand (getpid());

	/*
	 * Open this display.
	 */
	dpy = XOpenDisplay(display); 
	if (!dpy)
	{ 
		fprintf(stderr, "xfroot: Can't open display.\n");
		exit(1); 
	}

	/*
	 * Trap ^C so we can do a refresh
	 */
	(void) signal (SIGINT, time_to_go);
	
	/*
	 * Get the id of the root window.  No need for pseudo root
	 * here becuase we draw on top of everything that is ontop of
	 * the root.
	 */
	root = DefaultRootWindow(dpy);

	/*
	 * Get the default fg and bg colors.
	 */
	scr = DefaultScreen(dpy);
	fg = WhitePixel(dpy, scr);
	bg = BlackPixel(dpy, scr);
	
	att.override_redirect = True;
	att.backing_store = False;
	att.save_under = False;
	mask = CWOverrideRedirect | CWBackingStore | CWSaveUnder;
	visual.visualid = CopyFromParent;
	w_root = XCreateWindow (dpy, root, 0, 0,
	  DisplayWidth (dpy, scr), DisplayHeight(dpy, scr), 0,
	  DefaultDepth (dpy, scr), InputOutput, &visual,
	  mask, &att);
	XMapWindow (dpy, w_root);

	/*
	 * Setup the GC that will let us do the magic.  Key points are
	 * the drawing mode IncludeInferios.  This is what lets us
	 * draw on top of things.
	 */
	mygc = XCreateGC (dpy, w_root, 0, 0);
	XSetForeground (dpy, mygc, fg);
	XSetBackground (dpy, mygc, bg);
	XSetSubwindowMode (dpy, mygc, IncludeInferiors);

	/*
	 * OK, find out how big to make things...
	 */
	if ((max_flag & X_FLAG) == 0)
		x_max = DisplayWidth (dpy, scr);
	if ((max_flag & Y_FLAG) == 0)
		y_max = DisplayHeight (dpy, scr);
	x_size = x_max / nxsquares;
	y_size = y_max / nysquares;
	x_max = x_size * nxsquares;
	y_max = y_size * nysquares;
	
	/*
	 * Create the grid.
	 */
	if (do_border)
	{
		XSetLineAttributes (dpy, mygc, boarder_width, LineSolid,
				    CapNotLast, JoinMiter);
		for (i = 0; i <= y_max; i+= y_size)
			XDrawLine (dpy, w_root, mygc, 0, i, x_max, i);
		for (i = 0; i <= x_max; i+= x_size)
			XDrawLine (dpy, w_root, mygc, i, 0, i, y_max);
	}
	
	/*
	 * Start in the upper left hand corner.
	 */
	x = x_start;
	y = y_start;
	
	/*
	 * Save the upper left hand corner square if we are going to
	 * solve this later.  We will later assume that if pm is
	 * non-zero, that it must be freed.  Also, check to make sure
	 * that we can create this pixmap.  Some servers can't handle
	 * all pixmap create requests.  We have to do this the hard
	 * way.  If you know a better way (other than ignoring the
	 * condition), please let me know at the above address.
	 */
	if (solve)
	{
		XSync (dpy, False);
		old_handler = XSetErrorHandler (create_pm_handler);
		pm = XCreatePixmap (dpy, w_root, x_size, y_size,
				    DefaultDepth(dpy, scr));
		XSync (dpy, False);
		XSetErrorHandler (old_handler);
		if (pm)
			XCopyArea (dpy, w_root, pm, mygc, x_start, y_start,
				   x_size, y_size, 0, 0);
		else
			fprintf (stderr, "Warning: Start square not saved.\n");
	}
	
	/*
	 * Setup the mask of events.  I'm just looking for a key press
	 * right now.
	 */
	XSelectInput (dpy, w_root, KeyPressMask | ButtonPressMask);
	
	/*
	 * Loop forever.  Get a random number, move a square if it
	 * hasn't been just moved and is also on the screen.
	 */
	while (1)
	{
		if (mode == MODE_EXIT)
			done();
		if (XEventsQueued (dpy, QueuedAfterReading) == 0) {
			move_a_square();
		}
		else
			process_event();
	}		
}    /* end of main () */

/**********************************************************************
 *     process_event () --- Processes events that we are interested
 * 			    in.  All others are ignored.
 */

void process_event ()
{
	XEvent e;		/* Event we are looking at.*/

	/*
	 * Read the event, and dispatch it.
	 */
	XNextEvent(dpy, &e);
	switch (e.type)
	{
	/*
	 * KeyPress events cause us to go to the next mode.
	 */
	case KeyPress:
	case ButtonPress:
		if (solve)
			mode++;
		else
			mode = MODE_EXIT;
		break;
		
	/*
	 * All XLib clients need to do this.
	 */
	case MappingNotify:
		XRefreshKeyboardMapping (&e);
		break;

	}
}    /* end of process_event () */

/**********************************************************************
 *     move_a_square () --- Moves a square.  Will shuffle it when we
 * 			    are randomizing the screen.  Will
 * 			    unshuffle it when we aren't.
 */

void move_a_square ()
{
	if (mode == MODE_SCRAMBLE)
		rand_move_a_square();
	else
		undo_move_a_square();
}    /* end of move_a_square () */

/**********************************************************************
 *     rand_move_a_square () --- Moves a square randomly.  Updates the
 * 				 current X and Y positions.  Also
 * 				 saves the move.
 */

void rand_move_a_square ()
{
	switch ((rand() >> 15) & 3)
	{
	case DOWN:			/* Move down */
		if (last != UP && y - y_size >= 0)
		{
			move_down (x,y);
			y -= y_size;
			save_move(DOWN);
		}				
		break;
			
	case UP:			/* Move up */
		if (last != DOWN && y + y_size < y_max)
		{
			move_up (x, y);
			y += y_size;
			save_move(UP);
		}
		break;

	case RIGHT:			/* Move right */
		if (last != LEFT && x - x_size >= 0)
		{
			move_right (x,y);
			x -= x_size;
			save_move(RIGHT);
		}
		break;

	case LEFT:			/* Move left */
		if (last != RIGHT && x + x_size < x_max)
		{
			move_left (x, y);
			x += x_size;
			save_move(LEFT);
		}
		break;
	}
}    /* end of rand_move_a_square () */

/**********************************************************************
 *     undo_move_a_square () --- Undoes the latest move that hasn't
 * 				 been undone.
 */

void undo_move_a_square ()
{
	switch (moves[--cur_move])
	{
	case DOWN:			/* Move down */
		move_up (x,y);
		y += y_size;
		break;
			
	case UP:			/* Move up */
		move_down (x, y);
		y -= y_size;
		break;

	case RIGHT:			/* Move right */
		move_left (x,y);
		x += x_size;
		break;

	case LEFT:			/* Move left */
		move_right (x, y);
		x -= x_size;
		break;
	}
	if (cur_move <= 0)
		mode = MODE_EXIT;
}    /* end of undo_move_a_square () */

/**********************************************************************
 *     save_move () --- Saves the move, iff we are solving this thing
 * 			later.
 * 
 * Arguments:
 * 
 *     move	    -i- Move to save (eg LEFT, RIGHT)
 */

void save_move (move)
int move;
{
	last = move;
	if (solve)
	{
		moves[cur_move++] = move;
		if (cur_move >= max_moves)
			mode = MODE_SOLVE;
	}
}    /* end of save_move () */

/**********************************************************************
 *     unique_match () --- Does st match pat for at least len
 * 			   characters?
 * 
 * Arguments:
 * 
 *     st	       -i- String to match.  This is what the user
 * 			   types
 *     pat	       -i- Pattern to match against
 *     len	       -i- Minimum unque length.
 * 
 * Returns:
 * 
 *     SUCCESS	       --- String matches to at least the length
 * 			   requested.
 *     FAILURE	       --- Strings don't match.
 */

int unique_match (st, pat, len)
char *st;
char *pat;
int len;
{
	return len <= strlen (st) && strncmp (st, pat, strlen (st)) == 0;
	
}    /* end of unique_match () */

/**********************************************************************
 *     move_right () --- Moves the square on the left of the blank
 * 			 square to the right.
 * 
 */

void move_right ()
{
	int i;
	
	for (i = x - x_size; i < x; i+=speed)
		XCopyArea (dpy, w_root, w_root, mygc, i, y, x_size, y_size,
			   i+1,y);
}    /* end of move_right () */

/**********************************************************************
 *     move_left () --- Moves the square to the right of the blank
 * 			square to the left
 */

void move_left ()
{
	int i;
	
	for (i = x + x_size; i > x; i-=speed)
		XCopyArea (dpy, w_root, w_root, mygc, i, y, x_size, y_size,
			   i-1,y);
}    /* end of move_left () */

/**********************************************************************
 *     move_up () --- Moves the square below the blank square up.
 */

void move_up ()
{
	int i;

	for (i = y + y_size; i > y; i-=speed)
		XCopyArea (dpy, w_root, w_root, mygc, x, i, x_size, y_size,
			   x,i-1);
}

/**********************************************************************
 *     move_down () --- Moves the square above the blank square down.
 */

void move_down ()
{
	int i;

	for (i = y - y_size; i < y; i+=speed)
		XCopyArea (dpy, w_root, w_root, mygc, x, i, x_size, y_size,
			   x,i+1);
}

/**********************************************************************
 *     usage () --- Tells the user how to use this program.
 */

void usage ()
{
        fprintf (stderr, "\n");
	fprintf (stderr, "XShuffle [-options]\n"); 
        fprintf (stderr, "\n");
        fprintf (stderr, "         [-numsquares n]       : Number of squares both ways\n");
        fprintf (stderr, "         [-xnumsquares n]      : Number of squares across\n");
        fprintf (stderr, "         [-ynumsquares n]      : Number of squares down\n");
        fprintf (stderr, "         [-display disp]       : Display to use\n");
        fprintf (stderr, "         [-border]             : Draw a border\n");
        fprintf (stderr, "         [-solve]              : Undo moves at end\n");
        fprintf (stderr, "         [-max n]              : Size of square area\n");
        fprintf (stderr, "         [-xmax n]             : Width of area\n");
        fprintf (stderr, "         [-ymax n]             : Height of area\n");
        fprintf (stderr, "         [-moves n]            : Maximum moves to make\n");
        fprintf (stderr, "\n");
	exit(1);
}    /* end of usage () */

/**********************************************************************
 *     die_usage () --- Prints the error, then calls usage.
 * 
 * Arguments:
 * 
 *     msg	    -i- Message to print.
 */

void die_usage (msg)
char *msg;
{
	fprintf (stderr, "%s\n", msg);
	usage();
}    /* end of die_usage () */

