/* ta=4 		tabs set to 4 */
/************************************************************************
*			mandebrot and julia set generator							*
************************************************************************/
/*
 * $Id:$
 * $Log:$
 *
*/

#include <stdio.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#ifdef VASIMPLE
#include <Xm/VaSimple.h>
#endif
#include <Xm/Xm.h>
#include <Xm/PanedW.h>
#include <Xm/PushB.h>
#include <Xm/DrawingA.h>
#include <Xm/BulletinB.h>
#include <Xm/Text.h>

#include <Xm/Label.h>

#include <Xm/LabelG.h>
#include <Xm/RowColumn.h>
#include <Xm/ToggleBG.h>

#include "mandel.h"
#include "mandel.bit"

#ifdef __STDC__
static int make_buttons(struct button_list *button, int text_wide, int text_high, int left_offset, int top_offset);
static int make_dialogue(Widget w, struct enter_buttons *but, int x, int y, int labelchars, int textchars, int font_wide, int text_high);
static int make_selector(Widget parent, int xoffset, int nexty, int text_wide, int text_high, int font_wide);
static void do_mandel(Widget w, caddr_t client_data, caddr_t call_data);
static void do_julia(Widget w, caddr_t client_data, caddr_t call_data);
static void do_quit(Widget w, caddr_t client_data, caddr_t call_data);
static void do_select(Widget w, caddr_t client_data, XmToggleButtonCallbackStruct *call_data);
static void button_set(Widget w, int how);
static void set_zoom(Widget w, caddr_t client_data, caddr_t call_data);
static void recalc(Widget w, caddr_t client_data, caddr_t call_data);
static void do_remove(Widget w, caddr_t client_data, caddr_t call_data);
static void generate_gif(Widget w, caddr_t client_data, caddr_t call_data);
static void rotateplus(Widget w, caddr_t client_data, caddr_t call_data);
static void rotateminus(Widget w, caddr_t client_data, caddr_t call_data);
static void rotate(struct imageparm *parm, int val);
static int current_rotate(void);
static void save_coord(struct imageparm *image);
static int check_coord_change(struct imageparm *image);
static void update_control(struct imageparm *image);
static void updoubleval(char *name, double x);
static void upintval(char *name, int x);
static void font_box_size(Widget w, int *wide, int *high);
static String tidy(String s);
static char *anynumber(char *s);
#else
static int make_buttons();
static int make_dialogue();
static int make_selector();
static void do_mandel();
static void do_julia();
static void do_quit();
static void do_select();
static void button_set();
static void set_zoom();
static void recalc();
static void do_remove();
static void generate_gif();
static void rotateplus();
static void rotateminus();
static void rotate();
static int current_rotate();
static void save_coord();
static int check_coord_change();
static void update_control();
static void updoubleval();
static void upintval();
static void font_box_size();
static String tidy();
static char *anynumber();
#endif

extern struct imageparm manparm;
extern struct imageparm juliaparm;

char			*progname;

Display *display;
Screen	*screen;
Window root;

Widget toplevel;
Widget mandelshell = NULL;
Widget juliashell  = NULL;

int depth;
int dots_per_byte;					/*	pixel dots in a byte		*/
extern int julia_in_progress;
extern struct imageparm manparm;
extern struct imageparm juliaparm;
struct imageparm *activeimage = &manparm;

static Widget		bboard;
static Widget	mandel_select, julia_select, cstep_value;
static Pixmap 		mandel_pixmap, manim_pixmap, julim_pixmap;

static Dimension mandel_initial_height;

struct enter_buttons enter[] =
{	{ "GIF\nfile:", 0, 0, 	"fname", 30 },
	{ "ZOOM COORDINATES", 0, 1, NULL, 0 },
	{ "lx:", 0, 2, 		"flx", 15 },
	{ "ly:", 1, 2, 		"fly", 15 },
	{ "ux:", 0, 3, 		"fux", 15 },
	{ "uy:", 1, 3, 		"fuy", 15 },
	{ "POINT COORDINATES", 0, 4, NULL, 0 },
	{ "x:", 0, 5,  		"px", 15 },
	{ "y:", 1, 5,  		"py", 15 },
	{ NULL }
} ;

struct button_list button[] = 
{
	{ "mandel",		do_mandel	 ,    0,     0 },
	{ "julia",		do_julia	 ,    1,     0 },
	{ "quit",		do_quit		 ,    2,     0 },
	{ NULL,			NULL		 ,    5,     0 }
};

struct button_list work[] = 
{
	{ "zoom",		set_zoom	 ,    1,     0 },
	{ "unzoom",		do_unzoom	 ,    1,     1 },
	{ "+colour",	rotateplus	 ,    2,     0 },
	{ "-colour",	rotateminus	 ,    2,     1 },
	{ "recalc",		recalc		 ,    3,     0 },
	{ NULL,			NULL		 ,    5,     0 }
};

/********************************************************************
*	main ()															*
********************************************************************/

main (argc, argv)
int		argc;
char	**argv;
{
	Widget		new, prev;
	Widget		gifout;
	Arg			av[10];
	int			i, n;
	int			font_high, font_wide;
	char		depth_str[10];
	Dimension	text_high, text_wide;
	short		marg_high, marg_wide, mm;
	Position	text_loc;
	Position	nexty, y_button1, y_button2, y_selector;
	Dimension	ww, bb, hh;
	Position	xx, yy;

	/*	initialize Xt Intrinsics */
	
	toplevel = XtInitialize (argv[0], "XMandel", NULL, 0, &argc, argv);

	i = 0;
	XtSetArg (av[i], XmNallowShellResize, TRUE);		i++;
	XtSetValues (toplevel, av, i);

	display = XtDisplay(toplevel);
	root = RootWindow (display, DefaultScreen(display));
	screen  = XtScreen (toplevel);
	depth = DefaultDepthOfScreen (XtScreen(toplevel));
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

	/*	create a resizable container for user buttons and graphics */

	i = 0;
	XtSetArg (av[i], XmNallowOverlap, True);			i++;
	XtSetArg (av[i], XmNresizePolicy, XmRESIZE_ANY);	i++;
	XtSetArg (av[i], XmNx, 0);  						i++;
	XtSetArg (av[i], XmNy,0);  							i++;
	bboard = XmCreateBulletinBoard (toplevel, "name", av, i);

	i = 0;
	XtSetArg (av[i], XmNmarginHeight, &marg_high);			i++;
	XtSetArg (av[i], XmNmarginWidth,  &marg_wide);			i++;
	XtGetValues (bboard, av, i);
	
	font_box_size (bboard, &font_wide, &font_high);
	text_wide = font_wide * 10;
	text_high = font_high * 2;
	text_loc = 0;

	/*	top 2 rows of push buttons */
	
	y_button1 = make_buttons (button, (5 * text_wide) / 3, text_high, marg_wide, marg_high);
	y_button2 = nexty = make_buttons (work, text_wide, text_high, marg_wide, y_button1);

	/*	radio button and colour step */

	y_selector = nexty = make_selector (bboard, marg_wide, y_button1, text_wide, text_high, font_wide);

	/*	various x-y coordinate boxes */

	nexty = MAX (y_button1, y_button2);
	nexty = MAX (nexty, y_selector);
	nexty = make_dialogue (bboard, enter, 6, nexty, 6, 15, font_wide, text_high + 6);

	/*	disable editing of "point coordinates" */
	
	i = 0;
	XtSetArg(av[i], XmNeditable, False);			i++;
	prev = XtNameToWidget(bboard, "px");
	XtSetValues(prev, av, i);
	prev = XtNameToWidget(bboard, "py");
	XtSetValues(prev, av, i);

	/*	"save" button for gif file: first get end of "fname" widget */

	prev = XtNameToWidget(bboard, "fname");
	i = 0;
	XtSetArg(av[i], XmNwidth, &ww);					i++;
	XtSetArg(av[i], XmNborderWidth, &bb);			i++;
	XtSetArg(av[i], XmNx, &xx);						i++;
	XtGetValues(prev, av, i);

	i = 0;
	XtSetArg (av[i], XmNx, 		xx + ww + bb * 2 + 4);	i++;
	XtSetArg (av[i], XmNy, 		y_button2 + 3); 	 		i++;
	XtSetArg (av[i], XmNwidth,  text_wide);			i++;
	XtSetArg (av[i], XmNheight, text_high);			i++;
	gifout =  XtCreateManagedWidget ("Save", xmPushButtonWidgetClass, bboard, av, i);
	XtAddCallback (gifout, XmNactivateCallback, generate_gif, 0);
	XtManageChild (bboard);

	manparm.lx = START_LX;
	manparm.ly = START_LY;
	manparm.ux = START_UX;
	manparm.uy = START_UY;
	activeimage = &manparm;
	update_coordinates (&manparm, 0);
	
	mandel_pixmap = XCreateBitmapFromData (XtDisplay (toplevel),
						RootWindowOfScreen (XtScreen (toplevel)),
						mandel_bits,
						mandel_width,
						mandel_height);
	XtSetArg (av[0], XtNiconPixmap, mandel_pixmap);
	XtSetValues (toplevel, av, 1);

	XtRealizeWidget (toplevel);
	XtMainLoop ();
}

/***********	button creation routines	*****************/

/************************************************************************
*	make_button()  make a set of buttons based on the button table		*
************************************************************************/

static int make_buttons (button, text_wide, text_high, left_offset, top_offset)
struct button_list *button;
int		text_wide;
int 	text_high;
int 	left_offset;
int 	top_offset;
{
	Arg			av[10];
	int			i, n;
	Position	nexty;
	int			text_loc;
	Widget 		prev, new;
	
	text_loc = 0;
	for (prev = NULL, n = 0;  button[n].ident;  n++)
	{	i = 0;
		XtSetArg (av[i], XmNx, 		button[n].x * (text_wide + 4) + left_offset);  i++;
		XtSetArg (av[i], XmNy, 		button[n].y * (text_high + 4) + top_offset);  i++;
		XtSetArg (av[i], XmNwidth,   text_wide);					i++;
		XtSetArg (av[i], XmNheight,  text_high);					i++;
		new =  XtCreateManagedWidget (button[n].ident, xmPushButtonWidgetClass, bboard, av, i);
		XtAddCallback (new, XmNactivateCallback, button[n].bcall , 0);
		prev = new;
		text_loc = MAX (text_loc, button[n].y);
	}
	nexty = (text_loc+1) * (text_high + 4) + top_offset;
	return (nexty);
}

/*	routines to make various buttons */

/************************************************************************
*	make_dialogue()				make widgets to allow data entry		*
************************************************************************/

#define XSEP	6
#define YSEP	0

static int make_dialogue (w, but, x, y, labelchars, textchars, font_wide, text_high)
Widget 		w;
struct enter_buttons *but;		/*	list of buttons to create 		*/
int			x;					/*	x-offset in pixels				*/
int			y;					/*	y-offset in pixels				*/
int 		labelchars;			/*	labelwidth in characters		*/
int 		textchars;			/*	textwidth in chars				*/
int			font_wide;
int			text_high;
{
	Widget		editor, activate;
	int			ncol;
	Dimension	labelwide, fieldwide;
	Position	labelx[10], fieldx[10];
	int			i, n;
	Arg			av[10];
	Position	thisy, nexty;
	XmString	str;

	
	nexty = y;
	labelwide = labelchars;
	fieldwide = textchars + 2;
	ncol = 0;
	for (n = 0;  but[n].button;  n++)
	{	ncol  = MAX (ncol, but[n].bx);
	}
	ncol++;
	for (n = 0;  n < ncol;  n++)
	{
		labelx[n] = ((labelwide + fieldwide) * font_wide + XSEP ) * n + x;
		fieldx[n] = labelx[n] + labelwide * font_wide + XSEP;
	}
	for (n = 0;  but[n].button;  n++)
	{
		i = 0;
		str = XmStringCreateLtoR (but[n].button, XmSTRING_DEFAULT_CHARSET);
		XtSetArg (av[i], XmNx, 		labelx[but[n].bx]);  i++;
		thisy = but[n].by * (text_high + YSEP) + y;
		nexty = MAX (thisy + text_high + YSEP, nexty);
		XtSetArg (av[i], XmNy, 		thisy);  i++;
		if (but[n].field)
		{	XtSetArg (av[i], XmNwidth,  labelwide * font_wide );			i++;
		}
		else
		{	XtSetArg (av[i], XmNwidth,  strlen (but[n].button) * font_wide + XSEP);	i++;
		}		
		XtSetArg (av[i], XmNheight,  text_high);					i++;
		XtSetArg (av[i], XmNlabelString, str);   i++;
		XtSetArg (av[i], XmNalignment, XmALIGNMENT_END);			i++;
		activate =  XtCreateManagedWidget (but[n].button, xmLabelGadgetClass, w, av, i);

		if (but[n].field)
		{	i = 0;
			XtSetArg (av[i], XmNx, 		fieldx[but[n].bx]);  i++;
			XtSetArg (av[i], XmNy, 		but[n].by * (text_high + YSEP) + y);  i++;
			XtSetArg (av[i], XmNwidth,  (but[n].fsize + 2) * font_wide);					i++;
			XtSetArg (av[i], XmNheight,  text_high);					i++;
			editor = XtCreateManagedWidget (but[n].field, xmTextWidgetClass, w, av, i);
		}
		free (str);
	}
	return (nexty);
}


/************************************************************************
*	make_selector()			make radio buttons and colour step widgets	*
************************************************************************/

static int make_selector (parent, xoffset, nexty, text_wide, text_high, font_wide)
Widget 		parent;
int			xoffset;
int			nexty;
int			text_wide;
int			text_high;
int			font_wide;
{
	int			i;
	Arg			av[10];
	Widget		row_column;
	Widget		activate;
	XmString	str;
	char		s[100];
	Dimension	hh;
	Position	yy;
	short		mm;
	char		*text = "Rotate:";

	i = 0;
	XtSetArg (av[i], XmNx, 		xoffset);			i++;
	XtSetArg (av[i], XmNy, 		nexty); 			i++;
	XtSetArg (av[i], XmNorientation, XmVERTICAL);	i++;
	XtSetArg (av[i], XmNradioBehavior, True);		i++;
	XtSetArg (av[i], XmNradioAlwaysOne, True);		i++;
	XtSetArg (av[i], XmNisHomogeneous, 	False);		i++;
	row_column = XmCreateRowColumn (parent, "rowcol", av, i);
	XtManageChild (row_column);
	
	i = 0;
	XtSetArg (av[i], XmNset, 			 True);  	i++;
	XtSetArg (av[i], XmNshadowThickness, 0);  		i++;
	XtSetArg (av[i], XmNmarginHeight, 	 0);  		i++;
	XtSetArg (av[i], XmNmarginWidth, 	 0);  		i++;
	mandel_select =	XmCreateToggleButtonGadget (row_column, "mandel", av, i);
	XtManageChild (mandel_select);
	XtAddCallback (mandel_select, XmNdisarmCallback, do_select , 0);

	i = 0;
	XtSetArg (av[i], XmNshadowThickness, 0);  		i++;
	XtSetArg (av[i], XmNmarginHeight, 	 0);  		i++;
	XtSetArg (av[i], XmNmarginWidth, 	 0);  		i++;
	julia_select = 	XmCreateToggleButtonGadget (row_column, "julia", av, i);
	XtManageChild (julia_select);
	XtAddCallback (julia_select, XmNdisarmCallback, do_select , 0);

	button_set (NULL, False);

	i = 0;
	XtSetArg (av[i], XmNx, 		(text_wide + xoffset) * 3); 	i++;
	XtSetArg (av[i], XmNy, 		nexty + text_high + 6);		i++;
	str = XmStringCreateLtoR ("Rotate:", XmSTRING_DEFAULT_CHARSET);
	XtSetArg (av[i], XmNlabelString, str);   i++;
	activate =  XtCreateManagedWidget ("ctext", xmLabelGadgetClass, parent, av, i);

	sprintf (s, "%d", activeimage->rotation);
	i = 0;
	yy = (text_wide + xoffset) * 3 + font_wide * strlen (text) + xoffset;
	XtSetArg (av[i], XmNx, 		yy);				i++;
	XtSetArg (av[i], XmNy, 		nexty + text_high);	i++;
	XtSetArg (av[i], XmNwidth,  6 * font_wide);		i++;
	XtSetArg (av[i], XmNvalue, s);  i++;
	cstep_value = XtCreateManagedWidget ("cstep", xmTextWidgetClass, parent, av, i);
	XtFree (str);

	i = 0;
	XtSetArg(av[i], XmNheight, &hh);				i++;
	XtSetArg(av[i], XmNy, &yy);						i++;
	XtSetArg (av[i], XmNmarginHeight, &mm);			i++;
	XtGetValues(row_column, av, i);
	return (yy + hh + mm*2);
}

/************	callbacks	***************/

/************************************************************************
*	do_mandel()		button = mandel:	create/use mandelbrot window	*
************************************************************************/

static void do_mandel (w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{	Dimension		wide;
	double			dx, dy;
	int				i;
	Arg				av[10];

	if (manparm.window == 0)
	{	manparm.lx = START_LX;
		manparm.ly = START_LY;
		manparm.ux = START_UX;
		manparm.uy = START_UY;
		dx = START_UX - START_LX;
		dx = ABS (dx);
		dy = START_UY - START_LY;
		dy = ABS (dy);
		wide = 200;
		mandel_initial_height = (double) wide * (dy / dx);

		i = 0;
		XtSetArg (av[i], XmNwidth,   	wide);					i++;
		XtSetArg (av[i], XmNheight,  	mandel_initial_height);	i++;
		XtSetArg (av[i], XmNtitle,		"Mandelbrot");			i++;
		XtSetArg (av[i], XmNiconName,	"Mandel");				i++;
		mandelshell = XtCreateApplicationShell ("Mandelshell", topLevelShellWidgetClass, av, i);
		XtAddEventHandler (mandelshell, ExposureMask, FALSE,  expose_mandel, NULL);

		manim_pixmap = XCreateBitmapFromData (XtDisplay (mandelshell),
						RootWindowOfScreen (XtScreen (mandelshell)),
						manim_bits,
						manim_width,
						manim_height);
		XtSetArg (av[0], XtNiconPixmap, manim_pixmap);
		XtSetValues (mandelshell, av, 1);

		XtRealizeWidget (mandelshell);
		manparm.window = XtWindow (mandelshell);
	}
	activeimage = &manparm;
	button_set (mandel_select, True);
	update_coordinates (&manparm, 0);
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
	struct imageparm *save;

	if (manparm.window == 0)
		return;
	juliaparm.lx = -1.5;
	juliaparm.ly = -1.5;
	juliaparm.ux = 1.5;
	juliaparm.uy = 1.5;

	if (juliashell == NULL)
	{	i = 0;
		XtSetArg (av[i], XmNwidth,   	mandel_initial_height);		i++;
		XtSetArg (av[i], XmNheight,  	mandel_initial_height);		i++;
		XtSetArg (av[i], XmNtitle,		"Julia");					i++;
		XtSetArg (av[i], XmNiconName,	"Julia");					i++;
		juliashell = XtCreateApplicationShell ("Juliashell", topLevelShellWidgetClass, av, i);
		XtAddEventHandler (juliashell, ExposureMask, FALSE,  expose_julia, NULL);
		XtAddCallback (juliashell, XmNdestroyCallback, do_remove , 0);

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
	juliaparm.slx[0] =  juliaparm.sly[0]  =  juliaparm.sux[0]  =  juliaparm.suy[0] = 0;
	juliaparm.window = XtWindow (juliashell);
	juliaparm.totrot = 0;
	save = activeimage;
	activeimage = &manparm;
	update_control (activeimage);
	button_set (mandel_select, True);
	start_julia (1, 1);
	if (save != activeimage)
	{	activeimage = save;
		update_control (activeimage);
		button_set (julia_select, True);
	}
}

/************************************************************************
*	do_quit():		button = quit,   leave xmandel.						*
************************************************************************/

static void do_quit (w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	exit (0);
}

/************************************************************************
*	do_select	button = mandel/julia		select default set			*
************************************************************************/

static void do_select (w, client_data, call_data)
Widget w;
caddr_t client_data;
XmToggleButtonCallbackStruct  *call_data;
{
	if (manparm.window == 0)
	{	button_set (NULL, False);
		return;
	}
	if (w == julia_select   &&  juliaparm.window == 0)
	{	button_set (mandel_select, True);
		activeimage = &manparm;
		return;
	}
	if (w == mandel_select)
	{	if (activeimage == &manparm)
			return;
		activeimage = &manparm;
	}
	else
	{	if (activeimage == &juliaparm)
			return;
		activeimage = &juliaparm;
	}

	upintval    ("cstep", activeimage->rotation);

	updoubleval ("flx", activeimage->lx);
	updoubleval ("fly", activeimage->ly);
	updoubleval ("fux", activeimage->ux);
	updoubleval ("fuy", activeimage->uy);
	updoubleval ("px",  activeimage->px);
	updoubleval ("py",  activeimage->py);

	XFlush (display);
}

static void button_set (w, how)
Widget w;
int how;
{	int		i;
	Arg		av[10];
	Widget	on, off;

	if (w == NULL)
	{	/*	set bot to on or off */
		i = 0;
		XtSetArg (av[i], XmNset, how);	i++;
		XtSetValues (mandel_select, av, i);
		XtSetValues (julia_select, av, i);
	}
	else
	{	/*	 set one to on, other to off */
	
		if (w == mandel_select)
		{	if (how == True)
			{	off = julia_select;
				on  = mandel_select;
			}
			else
			{	off = mandel_select;
				on  = julia_select;
			}
		}
		else
		{	if (how == True)
			{	on  = julia_select;
				off = mandel_select;
			}
			else
			{	on  = mandel_select;
				off = julia_select;
			}
		}
		i = 0;
		XtSetArg (av[i], XmNset, False);	i++;
		XtSetValues (off, av, i);
		i = 0;
		XtSetArg (av[i], XmNset, True);	i++;
		XtSetValues (on, av, i);
	}
}

/************************************************************************
*	set_zoom()		button = zoom: start the zoom						*
************************************************************************/

static void set_zoom (w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{	struct zoomd *zoomtmp;

	if (check_coord_change(activeimage))
	{	zoomtmp = (struct zoomd *) malloc (sizeof (struct zoomd));
		zoomtmp->zp = activeimage->zoom;		/* push onto stack */
		zoomtmp->ux = activeimage->ux;
		zoomtmp->lx = activeimage->lx;
		zoomtmp->ly = activeimage->ly;
		zoomtmp->uy = activeimage->uy;
		if (activeimage->zoom == NULL)
			zoomtmp->zp = NULL;
		activeimage->zoom = zoomtmp;			/* activeimage->zoom is current pointer */
		if (activeimage == &manparm)
			redo_mandel ();
		else
			julia (1, 0);
	}
	else
		do_zoom (w, client_data, call_data);	/*	normal zoom with mouse */
}

/************************************************************************
*	recalc()		button = recalc			recompute current image		*
************************************************************************/

static void recalc (w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{

	if (check_coord_change(activeimage))
	{	activeimage->zoom->ux = activeimage->ux;
		activeimage->zoom->lx = activeimage->lx;
		activeimage->zoom->ly = activeimage->ly;
		activeimage->zoom->uy = activeimage->uy;
	}

	if (activeimage == &manparm)
		redo_mandel();
	else
		julia (1, 0);
}


/************************************************************************
*	do_remove():		callback if julia window is closed.				*
************************************************************************/

static void do_remove (w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	if (julia_in_progress)
		return;
	juliaparm.window = 0;
	juliashell = NULL;
	if (activeimage == &juliaparm)
	{	activeimage = &manparm;
		button_set (mandel_select, True);
		update_control (activeimage);
	}
}

/************************************************************************
*	generate_gif ()		button = save	   write image to gif file		*
************************************************************************/

static void generate_gif (w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	char 	*s, *ss;
	Widget	answer;

	answer = XtNameToWidget(bboard, "fname");
	if (answer == NULL)
	{	printf ("Cannot find widget 'fname'\n");
		exit (1);
	}
	s = XmTextGetString (answer);
	if (s == NULL)
		return;	
	ss = tidy (s);
	if (*ss  && activeimage->pix)
	    write_gif_image (ss, activeimage);
	XtFree (s);
}

/************************************************************************
*	rotateplus			button = +colour	rotate colour positive		*
************************************************************************/

static void rotateplus (w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	activeimage->rotation = current_rotate();
	rotate (activeimage, activeimage->rotation);
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
	rotate (activeimage, -activeimage->rotation);
	if (activeimage == &manparm)
		redraw_mandel (NULL);
	else
		redraw_julia (NULL);
}

/************************************************************************
*	rotate()			rotate the image through colours by val offset	*
************************************************************************/

static void rotate (parm, val)
struct imageparm *parm;			/*	rotate this image		*/
int val;						/*	by this much.			*/
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


/***********	support routines	***********/

/************************************************************************
*	current_rotate()		get current colour rotate from dialogue		*
************************************************************************/

static int current_rotate ()
{	Widget 	w;
	char	*s;

	w = XtNameToWidget(bboard, "cstep");
	s = XmTextGetString (w);
	return (atoi(s));
}

/************************************************************************
*	save_coord()  save text of usr coords to detect changes				*
************************************************************************/

static void save_coord (image)
struct imageparm *image;
{	Widget 	w;
	char	*s;

	XFlush (display);
	if (w = XtNameToWidget(bboard, "flx"))
	{	if (s = XmTextGetString (w))
		{	strncpy (image->slx, s, SAVESIZE);
			image->slx[SAVESIZE-1] = 0;
			XtFree (s);
		}
	}
	
	if (w = XtNameToWidget(bboard, "fux"))
	{	if (s = XmTextGetString (w))
		{	strncpy (image->sux, s, SAVESIZE);
			image->sux[SAVESIZE-1] = 0;
			XtFree (s);
		}
	}

	if (w = XtNameToWidget(bboard, "fly"))
	{	if (s = XmTextGetString (w))
		{	strncpy (image->sly, s, SAVESIZE);
			image->sly[SAVESIZE-1] = 0;
			XtFree (s);
		}
	}

	if (w = XtNameToWidget(bboard, "fuy"))
	{	if (s = XmTextGetString (w))
		{	strncpy (image->suy, s, SAVESIZE);
			image->suy[SAVESIZE-1] = 0;
			XtFree (s);
		}
	}
}


/************************************************************************
*	check_coord_change()		did the user change the numbers.		*
************************************************************************/

#define TinyV 0.000000001

static int check_coord_change (image)
struct imageparm *image;
{	Widget 	w;
	char	*s, *ss;
	double	val, atof(), xu, yu, xl, yl;
	int		changed;
	
	changed = 0;

	if (image->slx[0] ||  image->sly[0]  ||  image->sux[0]  ||  image->suy[0])
	{
		w = XtNameToWidget(bboard, "flx");
		s = XmTextGetString (w);
		if (strcmp (image->slx, s))
		{	ss = anynumber (s);
			image->lx = atof (ss);
			changed = 1;
		}
		XtFree (s);
	
		w = XtNameToWidget(bboard, "fux");
		s = XmTextGetString (w);
		if (strcmp (image->sux, s))
		{	ss = anynumber (s);
			image->ux = atof (ss);
			changed = 1;
		}
		XtFree (s);

		w = XtNameToWidget(bboard, "fly");
		s = XmTextGetString (w);
		if (strcmp (image->sly, s))
		{	ss = anynumber (s);
			image->ly = atof (ss);
			changed = 1;
		}
		XtFree (s);

		w = XtNameToWidget(bboard, "fuy");
		s = XmTextGetString (w);
		if (strcmp (image->suy, s))
		{	ss = anynumber (s);
			image->uy = atof (ss);
			changed = 1;
		}
		XtFree (s);
		if (changed)
		{	/*	ensure user types something that won't cause failure */
			xu = MAX (image->ux, image->lx);
			xl = MIN (image->ux, image->lx);
			yu = MAX (image->uy, image->ly);
			yl = MIN (image->uy, image->ly);
			if ((xu - xl) < TinyV)
				xu = xl + TinyV;
			if ((yu - yl) < TinyV)
				yu = yl + TinyV;
			image->ux = xu;
			image->uy = yu;
			image->lx = xl;
			image->ly = yl;
		}
	}
	return (changed);
}

/************************************************************************
*	up*()				various routines to update data entry fields.	*
************************************************************************/

static void update_control (image)
struct imageparm *image;
{
	update_coordinates (image, 0);
	update_point (image);
}

void update_coordinates (image, lf)
struct imageparm *image;
int		lf;
{
	if (image == activeimage)
	{	updoubleval ("flx", image->lx);
		updoubleval ("fly", image->ly);
		updoubleval ("fux", image->ux);
		updoubleval ("fuy", image->uy);
		save_coord (image);
	}
}

void update_point (image)
struct imageparm *image;
{
	updoubleval ("px", image->px);
	updoubleval ("py", image->py);
}

static void updoubleval (name, x)
char *name;
double x;
{	int		i;
	Arg		av[10];
	char	s[100];
	Widget	w;

	
	sprintf (s, "%.8lf", x);
	w = XtNameToWidget(bboard, name);
	if (w == NULL)
	{	printf ("Cannot find set widget [%s] to %s\n", name, s);
		exit (1);
	}
	i = 0;
	XtSetArg (av[i], XmNvalue, s);  i++;
	XtSetValues (w, av, i);	
}

static void upintval (name, x)
char *name;
int x;
{	int		i;
	Arg		av[10];
	char	s[100];
	Widget	w;

	
	i = 0;
	XtSetArg (av[i], XmNvalue, s);  i++;
	w = cstep_value;		/*	only this widget uses integers */
	sprintf (s, "%d", x);
	if (w == NULL)
	{	printf ("Cannot find set widget [%s] to %s\n", name, s);
		exit (1);
	}
	XtSetValues (w, av, i);	
}


/************************************************************************
*	font_box_size()				get size of font box for default font	*
************************************************************************/

static void font_box_size (w, wide, high)
Widget w;
int *wide;			/*	returned font wdith		*/
int	*high;			/*	returned font height	*/
{	XFontStruct	*font;
	XID			font_id;
	int			screen_num;
	
	screen_num = DefaultScreen(display);
	font_id = XGContextFromGC (DefaultGC (display, screen_num));
	font = XQueryFont (display, font_id);

	*high = font->max_bounds.ascent + font->max_bounds.descent;	
	*wide = font->max_bounds.width;
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
*	anynumber()				get a numeric string from s					*
************************************************************************/

static char *anynumber (s)
char *s;
{
	static char numstring[100];
	char	*ns;
	
	ns = numstring;
	while (*s  &&  ((*s >= '0'  &&  *s <= '9')  
					||  *s == '+'  ||  *s == '-'  ||  *s == '.'))
	{	*ns++ = *s++;
	}
	*ns = 0;
	return (numstring);
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


