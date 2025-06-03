/* ta=4	 (tabstop=4)
 *
 * Xmandel - written by John Freeman at Cray Research
 *
 *	modified to work on Interactive Unix X11 R4 and added gif output.
 *	  Tony Field (tony@ajfcal.cuc.ab.ca)
 *
 * This file contains the main routine for driving the mandelbrot generator
 */

#include <X11/IntrinsicP.h>
#include <X11/Shell.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Cardinals.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/Dialog.h>

#include <stdio.h>
#include "mandel.h"
#include "mandel.bit"

#ifdef __STDC__
static void do_mandel(Widget w, caddr_t client_data, caddr_t call_data);
static void do_julia(Widget w, caddr_t client_data, caddr_t call_data);
static void do_remove(Widget w, caddr_t client_data, caddr_t call_data);
static void do_quit(Widget w, caddr_t client_data, caddr_t call_data);
static void do_toggle(Widget w, caddr_t client_data, caddr_t call_data);
static void do_recalc(Widget w, caddr_t client_data, caddr_t call_data);
static void set_image(char *s);
static void do_gif(Widget w, caddr_t client_data, caddr_t call_data);
static String tidy(String s);
static void MandelFname(Widget w, caddr_t client_data, caddr_t call_data);
static void JuliaFname(Widget w, caddr_t client_data, caddr_t call_data);
static void FnameCancel(Widget w, caddr_t client_data, caddr_t call_data);
static void do_colour(Widget w, caddr_t client_data, caddr_t call_data);
static void MandelRotate(Widget w, caddr_t client_data, caddr_t call_data);
static void JuliaRotate(Widget w, caddr_t client_data, caddr_t call_data);
static void rotate_selected(void);
static void rotateplus(Widget w, caddr_t client_data, caddr_t call_data);
static void rotateminus(Widget w, caddr_t client_data, caddr_t call_data);
static void CancelRotate(Widget w, caddr_t client_data, caddr_t call_data);
static void rotate(struct imageparm *parm, int val);
static XtActionProc nothing(Widget w, XButtonEvent *event, String *params, Cardinal *num_params);
static void reshape_widgets(Widget parent, char **full_size, char **small_size);
static Widget *ConvertNamesToWidgets(Widget parent, char **names);
#else
static void do_mandel();
static void do_julia();
static void do_remove();
static void do_quit();
static void do_toggle();
static void do_recalc();
static void set_image();
static void do_gif();
static String tidy();
static void MandelFname();
static void JuliaFname();
static void FnameCancel();
static void do_colour();
static void MandelRotate();
static void JuliaRotate();
static void rotate_selected();
static void rotateplus();
static void rotateminus();
static void CancelRotate();
static void rotate();
static XtActionProc nothing();
static void reshape_widgets();
static Widget *ConvertNamesToWidgets();
#endif

Display *display;
Screen	*screen;
Widget drawform;
Window root;
int depth;							/*	bits per pixel (colour depth) 	*/
int dots_per_byte;					/*	pixel dots in a byte			*/

extern struct imageparm manparm;
extern struct imageparm juliaparm;
struct imageparm *activeimage = &manparm;
static Widget toplevel, juliashell = NULL;
extern int julia_in_progress;

static Pixmap 		mandel_pixmap, julim_pixmap;

Widget pshell, pressme, fnameDialog, dialogDone, dialogCancel, juliaFname;
Widget jshell, cshell, colourit, rotate_mandel, rotate_julia, rotate_cancel;
Widget toggle;

/*	kill use of the carriage return */

char *kset = "#override \n\
    Ctrl<Key>M:     nothing() \n\
    <Key>Return:    nothing()";

XtActionsRec gif_actions[] =
{	{ (String) "nothing", (XtActionProc) nothing }
} ;


/*	buttons name, callback, parent for basic user buttons	*/

struct button_list button[] = 
{	{ "mandel",		do_mandel	 },
	{ "julia",		do_julia	 },
	{ "toggle",		do_toggle	 },
	{ "zoom",		do_zoom		 },
	{ "unzoom",		do_unzoom	 },
	{ "colour",		do_colour	 },
	{ "+colour",	rotateplus	 },
	{ "-colour",	rotateminus	 },
	{ "recalc",		do_recalc    },
	{ "gif",		do_gif		 },
	{ "remove",		do_remove	 },
	{ "quit",		do_quit		 },
	{ NULL,			NULL		 }
};

char *is_mandel = "Image: Mandel ";
char *is_julia  = "Image: Julia  ";
int	mandel_window_high;

main (argc, argv)
int argc;
char **argv;
{
	Widget pane, box, form, new;
	Arg av[10];
	Cardinal i;
	int	n;
	int	wide, high;
	double dx, dy;
	char depth_str[10];
	XtTranslations mytranslations;
	static char *on_separate_lines[] = { "label", "value", NULL };
	static char *on_single_line[] = { "Mandelbrot", "Julia", "Cancel", NULL };

	toplevel = XtInitialize (argv[0], "XMandel", NULL, 0, &argc, argv);

	display = XtDisplay(toplevel);
	root = RootWindow (display, DefaultScreen(display));
	depth = DefaultDepthOfScreen (XtScreen(toplevel));
	screen  = XtScreen (toplevel);
	switch (depth)
	{
	case 8:		dots_per_byte = 1;  break;
	case 4:		dots_per_byte = 2;  break;
	case 2:		dots_per_byte = 4;	break;
	case 1:		dots_per_byte = 8;	break;
	default:	printf ("Cannot handle %d bit colour\n", depth);
				exit (1);
	}
	sprintf (depth_str, "%d", depth);		/*	default for "colour" button */
	manparm.rotation = juliaparm.rotation = depth;
	printf ("%d bit color\n", depth);
	manparm.totrot   = 0;
	manparm.window   = 0;
	manparm.pix      = NULL;
	juliaparm.totrot = 0;
	juliaparm.window = 0;
	juliaparm.pix    = NULL;

	/*	create a resizable container for user buttons and graphics */

	pane = XtCreateManagedWidget ("pane", panedWidgetClass, toplevel, NULL, ZERO);

	/*	create a button box within the pane for user selection */

	box = XtCreateManagedWidget ("box", boxWidgetClass, pane, NULL, ZERO);

	/*	create all user button widgets + associated callbacks, add to "box" */

	for (n = 0;  button[n].ident;  n++)
	{	new =  XtCreateManagedWidget (button[n].ident, commandWidgetClass, box, NULL, ZERO);
		XtAddCallback (new, XtNcallback, button[n].bcall , 0);
	}

    new = XtNameToWidget (box, "mandel");
	i = 0;	
	XtSetArg (av[i], XtNlabel, "  Mandelbrot  "); i++;
	XtSetValues(new, av, i);

    new = XtNameToWidget (box, "julia");
	i = 0;	
	XtSetArg (av[i], XtNlabel, "    Julia     "); i++;
	XtSetValues(new, av, i);

	toggle = XtNameToWidget (box, "toggle");
	set_image (is_mandel);

	/*	create a form widget within the pane to hold the graphics. */

	form = XtCreateManagedWidget ("form", formWidgetClass, pane, NULL, ZERO);

	dx = START_UX - START_LX;
	dx = ABS (dx);
	dy = START_UY - START_LY;
	dy = ABS (dy);
	wide = 225;
	high = (double) wide * (dy / dx);
	mandel_window_high = high;

	/*	add the drawing area to the form widget */

	i = 0;
	XtSetArg (av[i], XtNtop, XtChainTop); i++;
	XtSetArg (av[i], XtNbottom, XtChainBottom); i++;
	XtSetArg (av[i], XtNleft, XtChainLeft); i++;
	XtSetArg (av[i], XtNright, XtChainRight); i++;
	XtSetArg (av[i], XtNwidth, wide); i++;
	XtSetArg (av[i], XtNheight, high); i++;
	drawform = XtCreateManagedWidget ("mandelbrot", widgetClass, form, av, i);

	/*	popup to request gif output file name */

	i = 0;
	pshell = XtCreatePopupShell("Write Gif", transientShellWidgetClass, toplevel, av, i);

	i = 0;
    XtSetArg (av[i], XtNvalue, ""); i++;
    XtSetArg (av[i], XtNlabel, "Output GIF file name. Select Mandelbrot or Julia.");  i++;
	fnameDialog = XtCreateManagedWidget("fname", dialogWidgetClass, pshell, av, i);

	i = 0;
	dialogDone = XtCreateManagedWidget("Mandelbrot", commandWidgetClass, fnameDialog, av, i);

	i = 0;
	juliaFname = XtCreateManagedWidget("Julia", commandWidgetClass, fnameDialog, av, i);

	i = 0;
	dialogCancel = XtCreateManagedWidget("Cancel", commandWidgetClass, fnameDialog, av, i);

	XtAddCallback(dialogDone, XtNcallback, MandelFname, fnameDialog);
	XtAddCallback(juliaFname, XtNcallback, JuliaFname, fnameDialog);
	XtAddCallback(dialogCancel, XtNcallback, FnameCancel, fnameDialog);

	/*	popup to request colour rotate */

	i = 0;
	cshell = XtCreatePopupShell("Colour Rotate", transientShellWidgetClass, toplevel, av, i);

	i = 0;
    XtSetArg (av[i], XtNvalue, ""); i++;
    XtSetArg (av[i], XtNlabel, "Rotate current colour values by:");  i++;
    XtSetArg (av[i], XtNvalue, depth_str);  i++;
	colourit = XtCreateManagedWidget("colour", dialogWidgetClass, cshell, av, i);

	i = 0;
	rotate_mandel = XtCreateManagedWidget("Mandelbrot", commandWidgetClass, colourit, av, i);

	i = 0;
	rotate_julia = XtCreateManagedWidget("Julia", commandWidgetClass, colourit, av, i);

	i = 0;
	rotate_cancel = XtCreateManagedWidget("Cancel", commandWidgetClass, colourit, av, i);

	XtAddCallback(rotate_mandel, XtNcallback, MandelRotate, colourit);
	XtAddCallback(rotate_julia, XtNcallback, JuliaRotate, colourit);
	XtAddCallback(rotate_cancel, XtNcallback, CancelRotate, colourit);

	/*	disable <enter> in fnameDialog box */

	XtAddActions (gif_actions, XtNumber (gif_actions));

	/*	create julia window as a popup shell */

	i = 0;
	jshell = XtCreatePopupShell("Julia Set", transientShellWidgetClass, toplevel, av, i);
	XtAddEventHandler (jshell, ExposureMask, FALSE,  expose_julia, NULL);

	/*	setup function to handle exposure events	*/

	XtAddEventHandler (drawform, ExposureMask, FALSE,  expose_mandel, NULL);
	reshape_widgets (fnameDialog, on_separate_lines, on_single_line);
	reshape_widgets (colourit, on_separate_lines, on_single_line);

	/*	kill use of the carriage return */

    new = XtNameToWidget (fnameDialog, "value");
	mytranslations = XtParseTranslationTable (kset);
	XtOverrideTranslations (new,   mytranslations);

    new = XtNameToWidget (colourit, "value");
	mytranslations = XtParseTranslationTable (kset);
	XtOverrideTranslations (new,   mytranslations);

	mandel_pixmap = XCreateBitmapFromData (XtDisplay (toplevel),
						RootWindowOfScreen (XtScreen (toplevel)),
						mandel_bits,
						mandel_width,
						mandel_height);
	XtSetArg (av[0], XtNiconPixmap, mandel_pixmap);
	XtSetValues (toplevel, av, 1);

	XtRealizeWidget (toplevel);
	juliaparm.pix = NULL;
	manparm.pix = NULL;
	XtMainLoop ();
}

/*	callbacks	/

/************************************************************************
*	do_mandel()		button = mandel:	create/use mandelbrot window	*
************************************************************************/

static void do_mandel (w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{	unsigned int	high, wide;
	double			dx, dy;

	manparm.lx = START_LX;
	manparm.ly = START_LY;
	manparm.ux = START_UX;
	manparm.uy = START_UY;
	dx = START_UX - START_LX;
	dx = ABS (dx);
	dy = START_UY - START_LY;
	dy = ABS (dy);
	wide = 225;
	high = (double) wide * (dy / dx);
	manparm.window = XtWindow (drawform);
	manparm.totrot = 0;
	start_mandel ();
}

/************************************************************************
*	do_julia()		button = julia:		create/use julia window			*
************************************************************************/

static void do_julia (w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{	int		i;
	Arg		av[10];

	if (manparm.window == 0)
		return;

	juliaparm.lx = -1.5;
	juliaparm.ly = -1.5;
	juliaparm.ux = 1.5;
	juliaparm.uy = 1.5;
	if (juliaparm.window == 0)
	{
		if (juliashell == NULL)
		{	i = 0;
			XtSetArg (av[i], XtNwidth,   	mandel_window_high);		i++;
			XtSetArg (av[i], XtNheight,  	mandel_window_high);		i++;
			XtSetArg (av[i], XtNtitle,		"Julia");					i++;
			XtSetArg (av[i], XtNiconName,	"Julia");					i++;
			juliashell = XtCreateApplicationShell ("Juliashell", topLevelShellWidgetClass, av, i);
			XtAddEventHandler (juliashell, ExposureMask, FALSE,  expose_julia, NULL);

			julim_pixmap = XCreateBitmapFromData (XtDisplay (juliashell),
						RootWindowOfScreen (XtScreen (juliashell)),
						julim_bits,
						julim_width,
						julim_height);
			XtSetArg (av[0], XtNiconPixmap, julim_pixmap);
			XtSetValues (juliashell, av, 1);

			XtRealizeWidget (juliashell);
		}
		else
			XtMapWidget (juliashell);
		juliaparm.window = XtWindow (juliashell);
	}
	juliaparm.window = XtWindow (juliashell);
	juliaparm.totrot = 0;
	start_julia (1, 1);
	printf ("\n");
}

/************************************************************************
*	do_recalc			button = recalc.	recalculate current set.	*
************************************************************************/

static void do_recalc (w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	if (activeimage == &manparm)
		redo_mandel();
	else
		julia (1,0);
}


/************************************************************************
*	do_remove():		button = remove.  unmap julia widget			*
************************************************************************/

static void do_remove (w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	if (julia_in_progress)
		return;

	if (juliaparm.window)
		XtUnmapWidget (juliashell);

	juliaparm.window = 0;
	activeimage = &manparm;
	set_image (is_mandel);
}

/************************************************************************
*	do_quit():		button = quit,   leave xmandel.						*
************************************************************************/

static void do_quit (w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	printf ("\n");
	exit (0);
}

/************************************************************************
*	do_toggle():		button = quit,   leave xmandel.						*
************************************************************************/

static void do_toggle (w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{	char	*which;

	if (juliaparm.window == 0)
		return;
	if (activeimage == &manparm)
	{	activeimage = &juliaparm;
		which = is_julia;
	}
	else
	{	activeimage = &manparm;
		which = is_mandel;
	}
	set_image (which);
		
}

static void set_image (s)
char *s;
{	int		i;
	Arg		av[10];

	i = 0;	
	XtSetArg (av[i], XtNlabel, s); i++;
	XtSetValues(toggle, av, i);
}

/************************************************************************
*	do_gif ()		button = gif,   ask user for file name				*
************************************************************************/

static void do_gif (w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	Position x, y;
	Dimension width, height;
	Arg arg[2];
	int i;

	/*	 get the coordinates of the middle of toplevel widget.	*/

	i = 0;
	XtSetArg(arg[i], XtNwidth, &width);    i++;
	XtSetArg(arg[i], XtNheight, &height);  i++;
	XtGetValues(toplevel, arg, i);
	
	/*	translate coordinates in application top-level window
		into coordinates from root window origin.
	*/
	XtTranslateCoords(toplevel,				/* Widget */
                (Position) width/2,			/* x */
                (Position) height/2,		/* y */
                &x, &y);					/* coords on root window */

	/* move popup shell to this position (it's not visible yet) */

	i = 0;
	XtSetArg(arg[i], XtNx, x);  		i++;
	XtSetArg(arg[i], XtNy, y);  		i++;
	XtSetValues(pshell, arg, i);

	/*	Indicate to user that no other application functions are
		valid while fnameDialog is popped up...
	*/
	XtPopup(pshell, XtGrabNonexclusive);
}

/************************************************************************
*	tidy()		return the first non-blank word (ascii dependent!)		*
************************************************************************/

static String tidy (s)
String s;
{	String cs;

	while (*s  &&  *s == ' ')
		s++;
	cs = s;
	while (*s  &&  *s > ' '  &&  *s <= 'z')
		s++;
	*s = 0;
	return (cs);
}

/************************************************************************
*	MandelFname ()	button = gif/accept   write image to gif file		*
************************************************************************/

static void MandelFname (w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	String string, s;

    string = XawDialogGetValueString(fnameDialog);
	s = tidy (string);
	if (*s  && manparm.pix)
	{	activeimage = &manparm;
		printf ("Mandelbrot gif file = %s\n", s);
	    write_gif_image (s, activeimage);
	}
    XtPopdown(pshell);
    set_image (is_mandel);
    XFlush (display);
}

/************************************************************************
*	JuliaFname ()	button = gif/accept   write image to gif file		*
************************************************************************/

static void JuliaFname (w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	String string, s;

    string = XawDialogGetValueString(fnameDialog);
	s = tidy (string);
	if (*s  &&  juliaparm.pix)
	{	activeimage = &juliaparm;
		printf ("Julia gif file = %s\n", s);
	    write_gif_image (s, activeimage);
	}
    XtPopdown(pshell);
    set_image (is_julia);
    XFlush (display);
}

/************************************************************************
*	FnameCancel()		button = gif/cancel		ignore file write rq.	*
************************************************************************/

static void FnameCancel (w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	XtPopdown(pshell);
    XFlush (display);
}


/************************************************************************
*	do_colour()		button = gif,   ask user for file name				*
************************************************************************/

static void do_colour (w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	Position x, y;
	Dimension width, height;
	Arg arg[2];
	int i;

	/*	 get the coordinates of the middle of toplevel widget.	*/

	i = 0;
	XtSetArg(arg[i], XtNwidth, &width);    i++;
	XtSetArg(arg[i], XtNheight, &height);  i++;
	XtGetValues(w, arg, i);

	XtTranslateCoords(w,					/* Widget */
                (Position) 0,			/* x */
                (Position) height+2,		/* y */
                &x, &y);					/* coords on root window */

	/* move popup shell to this position (it's not visible yet) */

	i = 0;
	XtSetArg(arg[i], XtNx, x);  		i++;
	XtSetArg(arg[i], XtNy, y);  		i++;
	XtSetValues(cshell, arg, i);

	/*	Indicate to user that no other application functions are
		valid while fnameDialog is popped up...
	*/
	XtPopup(cshell, XtGrabNonexclusive);
}

/************************************************************************
*	MandelRotate	 		button = colour/mandelbort:	rotate colour	*
************************************************************************/

static void MandelRotate (w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	if (manparm.window)
	{	activeimage = &manparm;
		set_image (is_mandel);
		rotate_selected ();
	}
}

/************************************************************************
*	JuliaRotate()			button = colour/julia	rotate julia colour	*
************************************************************************/

static void JuliaRotate (w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	if (juliaparm.window)
	{	activeimage = &juliaparm;
		set_image (is_julia);
		rotate_selected ();
	}
}

/************************************************************************
*	JuliaRotate()			button = colour/julia	rotate julia colour	*
************************************************************************/

static void rotate_selected ()
{	String string, s;
	int		atoi();

    XtPopdown(cshell);
    XFlush (display);
    string = XawDialogGetValueString(colourit);
	s = tidy (string);
	if (*s  && manparm.pix)
	{	activeimage->rotation = atoi (s);
	}
}

/************************************************************************
*	rotateplus			button = +colour	rotate colour positive		*
************************************************************************/

static void rotateplus (w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	rotate (activeimage,  activeimage->rotation);
	if (activeimage == &manparm)
		redraw_mandel (NULL);
	else
		redraw_julia (NULL);
}

/************************************************************************
*	rotateminus			button = -colour	rotate colour negative		*
************************************************************************/

static void rotateminus (w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	rotate (activeimage,  -activeimage->rotation);
	if (activeimage == &manparm)
		redraw_mandel (NULL);
	else
		redraw_julia (NULL);
}


/************************************************************************
*	CancelRotate()		button = colour/ancel	ignore colour rotation	*
************************************************************************/

static void CancelRotate (w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
    XtPopdown(cshell);
    XFlush (display);
}


/************************************************************************
*	rotate()			rotate the image through colours by val offset	*
************************************************************************/

static void rotate (parm, val)
struct imageparm *parm;
int val;
{
	int		nbits, x, y, im, i, ib;
	unsigned char *b;
	unsigned int mask[8];
	unsigned int v[8];

	/*	with a touch of brute force */

	parm->totrot += val;
	switch (depth)
	{
	case 8:
		nbits = parm->pixheight * parm->pixwidth;
		b = parm->pix;
		while (nbits--)
			*b++ += val;
		break;

	case 4:
		for (i = 0;  i < 2;  i++)
		{	mask[i] = (0x0f << i*4);
			v[i] = val << i*4;
		}
		for (y = 0;  y < parm->pixheight;  y++)
		{	b = parm->pix + y * parm->bytewidth;
			for (x = 0;  x < parm->pixwidth;  x++)
			{	im = (x % 2);
				ib = x >> 1;
				b[ib] = ((((unsigned int) (b[ib] & mask[im]) + (v[im]) & mask[im])) & mask[im])
						| ( b[ib] & ~mask[im]);
			}
		}
		break;

	case 2:
		for (i = 0;  i < 4;  i++)
		{	mask[i] = (0x03 << i*2);
			v[i] = val << i*2;
		}
		for (y = 0;  y < parm->pixheight;  y++)
		{	b = parm->pix + y * parm->bytewidth;
			for (x = 0;  x < parm->pixwidth;  x++)
			{	im = x % 4;
				ib = x >> 2;
				b[ib] = ((((unsigned int) (b[ib] & mask[im]) + (v[im]) & mask[im])) & mask[im])
						| ( b[ib] & ~mask[im]);
			}
		}
		break;

	case 1:
		for (y = 0;  y < parm->pixheight;  y++)
		{	b = parm->pix + y * parm->bytewidth;
			for (x = 0;  x < parm->bytewidth;  x++)
				b[x] = ~b[x];
		}
		break;
	}
}


/* actions  */

/************************************************************************
*	nothing()			do nothing if <enter> is received in dialogue	*
************************************************************************/

static XtActionProc nothing (w, event, params, num_params)
Widget w;
XButtonEvent *event;
String *params;
Cardinal *num_params;
{
	String 	s, ss;
	Widget	parent;
	int		atoi();


	parent = w->core.parent;
    s = XawDialogGetValueString(parent);
	if (s)
	{	ss = tidy (s);
		if (*ss)
		{	if (strcmp (parent->core.name, "fname") == 0)
			{	XtPopdown(pshell);
			    XFlush (display);
				if (activeimage->pix)
				{	printf ("%s gif file = %s\n", (activeimage == &manparm ? "Mandelbrot" : "Julia"), s);
				    write_gif_image (ss, activeimage);
				}
			}
			else if (strcmp (parent->core.name, "colour") == 0)
			{    XtPopdown(cshell);
			    XFlush (display);
				activeimage->rotation = atoi (ss);
				rotate (activeimage,  activeimage->rotation);
				if (activeimage == &manparm)
					redraw_mandel (NULL);
				else
					redraw_julia (NULL);
			}
		}
		XtFree (s);
	}
}

/************************************************************************
*	reshape_widgets ()		make widgets uniform in size				*
************************************************************************/

/*	the following code to reshape a widget set is mangled from xman sources 
	full_size widgets are placed on independent lines
	small_size widgest are scaled so that all short widgets
	           fit into the same total width as the longest widget.
*/

static void reshape_widgets(parent, full_size, small_size)
Widget parent;
char **full_size, **small_size;
{
	Widget *full_widgets, *half_widgets, *temp, long_widget;
	Dimension longest, length, b_width;
	int interior_dist;
	Arg arglist[2];
	int	nhalf;

	for (nhalf = 0;	small_size[nhalf];	nhalf++)
		;
	full_widgets = ConvertNamesToWidgets(parent, full_size);
	half_widgets = ConvertNamesToWidgets(parent, small_size);
	
	long_widget = NULL;
	longest = 0;
	XtSetArg(arglist[0], XtNwidth, &length);
	XtSetArg(arglist[1], XtNborderWidth, &b_width);

	/* Find Longest widget. */

	for ( temp = full_widgets ; *temp != (Widget) NULL ; temp++) 
	{ XtGetValues(*temp, arglist, (Cardinal) 2);
		length += 2 * b_width;
		if (length > longest) 
		{ longest = length;
			long_widget = *temp;
		}
	}

	if (long_widget == (Widget) NULL) 
	{	printf ("Could not find longest widget, aborting...");
		XtFree(full_widgets);
		XtFree(half_widgets);
		return;
	}

	/* Set all other full_widgets to this length. */

	for ( temp = full_widgets ; *temp != (Widget) NULL ; temp++ )
	{	if ( long_widget != *temp) 
		{	Dimension width, border_width;

			XtSetArg(arglist[0], XtNborderWidth, &border_width);
			XtGetValues(*temp, arglist, (Cardinal) 1);
		
			width = longest - 2 * border_width;
			XtSetArg(arglist[0], XtNwidth, width);
			XtSetValues(*temp, arglist, (Cardinal) 1);
		}
	}

	/*	Set all the half widgets to the right length. */

	XtSetArg(arglist[0], XtNdefaultDistance, &interior_dist);
	XtGetValues(parent, arglist, (Cardinal) 1);
	
	for ( temp = half_widgets ; *temp != (Widget) NULL ; temp++) 
	{	Dimension width, border_width;

		XtSetArg(arglist[0], XtNwidth, &length);
		XtSetArg(arglist[1], XtNborderWidth, &b_width);
		XtGetValues(*temp, arglist, (Cardinal) 2);
		
		XtSetArg(arglist[0], XtNborderWidth, &border_width);
		XtGetValues(*temp, arglist, (Cardinal) 1);
		
		width = (longest - interior_dist)/nhalf - nhalf * border_width;
		if (width > b_width)
		{	 XtSetArg(arglist[0], XtNwidth, width);
			 XtSetValues(*temp, arglist, (Cardinal) 1);
		}
	}
	XtFree(full_widgets);
	XtFree(half_widgets);
}

/************************************************************************
*	ConvertNamesToWidgets()		get a widget vector from a name vector	*
************************************************************************/

static Widget *
ConvertNamesToWidgets(parent, names)
Widget parent;
char ** names;
{
	char ** temp;
	Widget * ids, * temp_ids;
	int count;

	for (count = 0, temp = names; *temp != NULL ; count++, temp++);

	ids = (Widget *) XtMalloc( (count + 1) * sizeof(Widget));

	for ( temp_ids = ids; *names != NULL ; names++, temp_ids++) 
	{	*temp_ids = XtNameToWidget(parent, *names);
		if (*temp_ids == NULL) 
		{	printf("Could not find widget named '%s'", *names);
			XtFree(ids);
			return(NULL);
		}
	}
	*temp_ids = (Widget) NULL;
	return(ids);
}

/************************************************************************
*	update_coordinates()	write zoom coordinates to stdout			*
************************************************************************/

void update_coordinates (image, lf)
struct imageparm *image;
int		lf;
{
	if (lf < 0)
		return;
	printf ("%s (lx,ly) (ux,uy) = (%11.7lf,%11.7lf) (%11.7lf,%11.7lf)%c",
				image == &manparm ? "mandel:" : "julia: ",
				image->lx, image->ly, image->ux, image->uy, (lf ? '\n' : '\r'));
	fflush (stdout);
	XFlush (display);
}

/************************************************************************
*	update_point()			write point coord and colour to stdout		*
************************************************************************/

void update_point (image)
struct imageparm *image;
{
	printf ("point:  (x,y) = (%11.7lf,%11.7lf)  colour = %d  \r",
			image->px, image->py, image->pcolour);
	fflush (stdout);
	XFlush (display);
                        
}


#ifdef NEEDMEMCPY

/************************************************************************
*	memcpy()				slow version of memcpy						*
************************************************************************/

memcpy (dest, src, n)
char *dest, *src;
int n;
{
	if (n > 0)
	{	while (n--)
			dest++ = *src++;
	}
}

#endif


