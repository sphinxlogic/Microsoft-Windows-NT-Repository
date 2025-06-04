/*

	YADMAN - Yet Another Damn MANdelbrot program

	Release Date: 9 April 1989 

	Author: Ken 'Dirk' Berkun

	Version: 1.0 

*/

/*
    ------------------------------------------------------------------
    Copyright (c) 1989 Digital Equipment Corporation.
    
    Permission to use, copy, modify and distribute this software for any
    purpose and without fee is hereby granted, provided the above copyright
    notice and this permission notice are retained, and the name of Digital
    Equipment Corporation not be used in advertising or publicity
    pertaining to this software without specific, written prior permission.
    This software is provided "as is" without express or implied warranty.
    
    Author: Ken Berkun 
    ------------------------------------------------------------------------
*/

/*****************************************************************************/
/*									     */ 
/*	ACKNOWLEDGEMENTS:						     */
/*									     */ 
/*     	Many thanks to:							     */
/*            								     */
/* 		Ander Samuelsson for providing the skeleton code  	     */
/*		Burns Fisher for the color code			  	     */
/*		DECburger without which this would not be possible	     */
/*		Xuidemo from which I stole all the help code		     */
/*	        The many many DEC employees who patiently answered my        */
/*		questions in the notesfiles				     */
/*		The Government of Malaysia for taking so long to process an  */
/*		export license, so that I had time to use a PMAX to develop  */
/*		this on.					  	     */
/*									     */
/*****************************************************************************/

/*

Contact Information:

	Ken Berkun
	hgovc::kenberkun
	Ken Berkun @HGO
	kenberkun%hgovc.dec@decwrl.dec.com

	Digital Equipment Far East Ltd.
	20/F Fleethouse
	38 Gloucester Rd.
	Wanchai Hong Kong
	5-861-4344
*/

#include <stdio.h>
#include <math.h>
#include <signal.h>

#ifdef VMS
#include <decw$include/DECwDwtApplProg.h>
#include <decw$cursor.h>
#include <decw$include/Vendor.h>
#define	 CURSORFONT "decw$cursor"
#else
#include <DECwDwtWidgetProg.h>
#include <DECwDwtApplProg.h>
#include <decwcursor.h>
#include <Vendor.h>
#define	 CURSORFONT "decwcursor"
#endif

#define	    MAX_WIDGETS			100

#define	    k_MainWindow	    	0
#define	    k_MenuBar	 	   	1
#define	    k_FileDown	    		2
#define	    k_MenuWidget    		3
#define	    k_OpenButton    		4
#define	    k_ExitButton    		5
#define	    k_FileBox	    		6
#define	    k_workwidget    		7
#define     k_SaveButton    		8
#define     k_NewButton                9
#define     k_xdim_label               10
#define     k_ydim_label               11
#define     k_length_label             12
#define     k_pixels_label             13
#define     k_iterations_label         14
#define     k_xdim                     15
#define     k_ydim                     16
#define	    k_length		       17
#define	    k_pixels                   18
#define     k_iterations               19    
#define	    k_DataEntryBox	       20
#define	    k_ok_data		       21
#define     k_reset_data	       22
#define     k_cancel_data              23
#define	    k_CommandsDown	    	24
#define	    k_CommandsWidget		25
#define     k_LaunchButton	     	26
#define	    k_ResizeButton		27
#define	    k_SaveAsButton		28
#define     k_CautionBox		32
#define		k_ColorBox		36
#define		k_apply_color		37
#define		k_reset_color		38
#define		k_dismiss_color		39
#define		k_red_mult_label	40
#define		k_green_mult_label	41
#define		k_blue_mult_label	42
#define		k_red_mult		43
#define		k_green_mult		44
#define		k_blue_mult		45
#define		color_box_label		46
#define		k_color_button_box	47
#define		k_OptionsDown		48
#define		k_OptionsWidget		49
#define		k_ColorsButton		50
#define		k_VerticalScroll	51
#define		k_HorizontalScroll	52
#define		k_MouseSizeButton	53
#define		k_AnimateInProgress	54
#define		k_LaunchInProgress	55
#define		k_AnimateButton		56
#define		k_BrainFadeButton	57
#define		k_launch_data		58
#define		k_shift			59
#define		k_mult			60
#define		k_effects_label		61
#define		k_progress_box		62
#define		k_red_up		63
#define		k_red_down		64
#define		k_green_up		65
#define		k_green_down		66
#define		k_blue_up		67
#define		k_blue_down		68
#define		k_inc_label		69
#define		k_1			70
#define		k_10			71
#define		k_HelpBox		72
#define		k_help_entry		73
#define		k_HelpDown		74
#define		k_HelpWidget		75
#define		k_HelpButton		76


#define		DEF_ACORNER		-2
#define		DEF_BCORNER		-2
#define		DEF_SIDE		4
#define		DEF_PIC			800       /* was 400 */
#define		DEF_IT			100
#define		MAX_PIXELS		256
#define		DEF_RED_MULT		1
#define		DEF_GREEN_MULT		2
#define		DEF_BLUE_MULT		4	
#define		INC_1			1
#define		INC_10			10
#define		MAX_SCROLL		100
#define		BAR_SIZE		17
#define		PROGRESS_SX		10	
#define		PROGRESS_SY		10	
#define		PROGRESS_WIDTH		100
#define		PROGRESS_LINE		16	
#define		X_MARGIN		20	
#define		Y_MARGIN		20		
#define		MAX_HEIGHT		864
#define		MAX_WIDTH		1024

/* bit maps for the icons */

#define iconify_width 16
#define iconify_height 16
static char iconify_bits[] = {
   0x00, 0x01, 0xe0, 0x0f, 0xf8, 0x3f, 0x1c, 0x7f, 0xc4, 0x7f, 0xe2, 0xf9,
   0xe8, 0xf3, 0xd2, 0xf3, 0xff, 0xe3, 0xd2, 0xf3, 0xe8, 0xf3, 0xe2, 0xf9,
   0xc4, 0x7f, 0x1c, 0x7f, 0xf8, 0x3f, 0xe0, 0x0f};

#define icon_width 16
#define icon_height 16
static char icon_bits[] = {
   0x00, 0x01, 0xe0, 0x0f, 0xf8, 0x3f, 0x1c, 0x7f, 0xc4, 0x7f, 0xe2, 0xf9,
   0xe8, 0xf3, 0xd2, 0xf3, 0xff, 0xe3, 0xd2, 0xf3, 0xe8, 0xf3, 0xe2, 0xf9,
   0xc4, 0x7f, 0x1c, 0x7f, 0xf8, 0x3f, 0xe0, 0x0f};

char * window_title = "YadMan - Mandelbrot";
int  x_pos = 0, y_pos = 0;      /* opening window position */
int demo_mode = 0;

Widget	toplevel, mainwidget, workwidget, WidgetID[MAX_WIDGETS];

char	*UID_database[] = { "yadman.uid" };
DRMHierarchy	Hierarchy;
DRMType	*dummy;

static void     prefetch ();
static void	error ();
static void	WidgetIDproc ();
static void     Activateproc ();
static void     newproc ();
static void	openproc ();
static void     saveproc ();
static void     saveasproc ();
static void	exitproc ();
static void	launchproc ();
static void	animateproc ();
static void	brainfadeproc ();
static void	colorsproc ();
static void	resizeproc ();
static void	VertScrollproc();
static void	HorScrollproc();
static void 	FileBoxproc ();
static void     CancelFileBoxproc ();
static void	CautionBoxproc ();
static void	ColorBoxproc ();
static void	MouseSizeproc ();
static void     CancelAnimateproc();
static void     CancelLaunchproc ();
static void	ColorIconproc();
static void	Effectsproc();
static void	Incproc();
static void 	Helpproc();
static void 	EndHelpproc();

static void	myreadheaderproc();
static void	mzoom();
static void 	ReInit();
static void	dispmand();
static void     MyExpoproc();
static void	makeimage();
static void 	makeroom();
static void 	readmand();
static void     MyWindowResize();
static void     RetrieveDataValues();
static void     ResetDataValues();
static void     RetrieveColorValues();
static void     ResetColorValues();
static void	DoFileInput();
static void     MyCloseFile();
static void     MyOpenFile();
static void     MySaveFile();
static void     ShouldISave();
static void     SetUpColorMap();
static void	LoadColorMap();
static void     UpdateColorMap();
static void	GetWindowSize();
static void     ResetScrollBars();
static void	MyVertScroll();
static void	MyHorScroll();
static void	InitRubberBox();
static void	ConvertCoords();
static void	DisplayMouseData();
static void 	Animate();
static void	Launcher();
static void	CreateIcon();
static void	GetLocation();

static	XtEventHandler Resizedproc();
static  XtEventHandler ButtonPressproc();

/* Declarations for colormaps, visuals, etc */


static	XVisualInfo 	*pVisualInfo,vInfoDummy;
static	int		nVis;
static	int		updateColor;
static	int		pixel;
static 	int		colorCount = 250;
static  XColor		*colors;
static  int		doprimary = 7;
static  int		pixels[MAX_PIXELS];
static  Colormap	map;
static  XtAppContext 	Context;
static  XEvent		work_event;

/* structure for each mandelbrot instance */

struct instance
	{
	char	filename[50];
	FILE	*fip;
	int 	x;
	int	y;
	double	real;
	double	imaginary;
	double	side;
	int	pic;
	int	it;
	XImage	*image;
	int	exists;
	};

/* the following variables are temporary holding areas used when gathering
   data from the data entry box */

static double temp_real,temp_imaginary, temp_side;
static int temp_pic, temp_it;

Arg argl[5];
static 	Font	current_font;
static  Cursor	wait_cursor;
static  XColor  current_fc,current_bc,current_dmy;
static  int	status;

/* file status flags */

static  char    new_image = FALSE;      /* an image has been generated */
static  char	cancel_operation = FALSE; /* cancel current operation */
static  char    old_file[50];	    /* hold old file name in case needed */
static  char	save_as = FALSE;    /* TRUE during save_as operation */
static  char	dirty = FALSE;	    /* you have made changes to data */
static  char    insaveascaution = FALSE;
static  char 	temp_file[50];
static  char  	exit_flag	= FALSE; /* in exit process */

/* display and scroll related flags and variables */

static 	char	auto_resize = FALSE; /* automatically resize window if true */
static	int	window_width;	     /* work window width */
static  int	window_height;	     /* work window height */
static  int	y_step;
static  int	y_scroll = 0;
static 	int	y_old_scroll = 0;
static	int	upper_image_from;
static	int	lower_image_from;
static	int	x_step;
static  int	x_scroll = 0;
static  int	x_old_scroll = 0;
static  int	left_image_from;
static  int	right_image_from;
static  char	kill_animation = FALSE; /* set when cancelling animation */
static  char	kill_launch = FALSE; /* set when cancelling launch */
static  float	total;	/* percent of completion of image generation */

static XGCValues progressGC_values;
static GC	 progressGC;

/* rubber box related flags and variables */

static XGCValues rubberGC_values;
static GC	 rubberGC;
static char	first_box = TRUE;

/* color multiplication factors */

static  int red_mult = DEF_RED_MULT;
static  int green_mult = DEF_GREEN_MULT;
static  int blue_mult = DEF_BLUE_MULT;
static  int old_red_mult = DEF_RED_MULT;
static  int old_green_mult = DEF_GREEN_MULT;
static  int old_blue_mult = DEF_BLUE_MULT;

static  int effect = 0; /* special effects flag */
static  int inc = INC_1; /* size of color increment */

struct 	instance	current;

static Pixmap icon_pixmap, iconify_pixmap;

char *hptr; 	/* pointer to holding area for creating the image */

char first = TRUE;  /* flag to indicate that the first image has been made */

struct mycallbackinfo
	{
	int		reason;
	XExposeEvent	*event;
	Window		wregion;
	};

unsigned int main( argc, argv)
    unsigned int   argc;
    char    *argv[];
{
    int i;
    static DRMCount	callback_num = 27;
    static DRMRegisterArg  callback_namevec[] = {  
			{"exitproc" ,(caddr_t)exitproc},
			{"newproc",(caddr_t)newproc},
			{"openproc" ,(caddr_t)openproc},
			{"saveproc" ,(caddr_t)saveproc},
			{"saveasproc",(caddr_t)saveasproc},
			{"launchproc",(caddr_t)launchproc},
			{"animateproc",(caddr_t)animateproc},
			{"brainfadeproc",(caddr_t)brainfadeproc},
			{"colorsproc",(caddr_t)colorsproc},
			{"resizeproc",(caddr_t)resizeproc},
			{"VertScrollproc",(caddr_t)VertScrollproc},
			{"HorScrollproc",(caddr_t)HorScrollproc},
			{"WidgetIDproc", (caddr_t)WidgetIDproc}, 
			{"Activateproc", (caddr_t)Activateproc},
			{"FileBoxproc", (caddr_t)FileBoxproc},
			{"CancelFileBoxproc", (caddr_t)CancelFileBoxproc},
			{"CautionBoxproc", (caddr_t)CautionBoxproc},
			{"ColorBoxproc", (caddr_t)ColorBoxproc},
			{"MouseSizeproc", (caddr_t)MouseSizeproc},
			{"CancelAnimateproc", (caddr_t)CancelAnimateproc},
			{"CancelLaunchproc", (caddr_t)CancelLaunchproc},
			{"ColorIconproc", (caddr_t)ColorIconproc},
			{"Effectsproc", (caddr_t)Effectsproc},
			{"Incproc", (caddr_t)Incproc},
			{"Helpproc", (caddr_t)Helpproc},
			{"EndHelpproc", (caddr_t)EndHelpproc},
			{"MyExpoproc", (caddr_t)MyExpoproc}
					 };

        for (i = 1; i < argc; i++) {
            if (strcmp(argv[i], "-x") == 0) {
                if (++i >= argc) usage(argv[0]);
                x_pos = atoi(argv[i]);
                }
            if (strcmp(argv[i], "-y") == 0) {
                if (++i >= argc) usage(argv[0]);
                y_pos = atoi(argv[i]);
                }
            if (strcmp(argv[i], "-title") == 0){
                if (++i >= argc) usage(argv[0]);
                window_title = argv[i];
                } 
             if (argv[i][0] == '-') usage(argv[0]);
        }
                                                    

    DwtInitializeDRM();

    toplevel = XtInitialize(	"Yadman",
				"TopLevel",
				NULL,
				0,
				&argc,
				argv );

    XtSetArg(argl[0], XtNallowShellResize,TRUE);
    XtSetArg(argl[1], DwtNtitle, window_title);
    XtSetArg(argl[2], DwtNx, x_pos);
    XtSetArg(argl[3], DwtNy, y_pos);    
    XtSetValues(toplevel,argl,4);
	

    if ( DwtOpenHierarchy(	1,
				UID_database,
				NULL,
				&Hierarchy ) != DRMSuccess )
				error( "Can't open hierarchy!" );

    DwtRegisterDRMNames(    callback_namevec, callback_num );


    if ( DwtFetchWidget(	Hierarchy,
				"MainWindow",
				toplevel,
				&mainwidget,
				&dummy ) != DRMSuccess )
				error( "Can't fetch main window!" );

    XtManageChild ( mainwidget);
    XtRealizeWidget( toplevel);

	/* Set up the color map */

    SetUpColorMap();
    LoadColorMap(&colorCount);
	
    prefetch();  	/* get all the widgets ahead of time for snappy
			   response */

    /* load the wait cursor and have it ready as needed */

    current_font = XLoadFont(XtDisplay(mainwidget), CURSORFONT);

    status = XAllocNamedColor(  XtDisplay(mainwidget),
				XDefaultColormap(XtDisplay(mainwidget),0),
				"White",
				&current_bc,
				&current_dmy);

    status = XAllocNamedColor(  XtDisplay(mainwidget),
				XDefaultColormap(XtDisplay(mainwidget),0),
				"Black",
				&current_fc,
				&current_dmy);

    wait_cursor = XCreateGlyphCursor(XtDisplay(mainwidget),
				     current_font,
				     current_font,
				     decw$c_wait_cursor,
				     decw$c_wait_cursor+1,
				     &current_fc,
				     &current_bc);

	
	/* initialize current to default */

	current.exists = 0;
	current.real = DEF_ACORNER;
	current.imaginary = DEF_BCORNER;
	current.side	 = DEF_SIDE;
	current.pic	 = DEF_PIC;
	current.it	 = DEF_IT;
	current.image   = NULL;

        XtSetSensitive(WidgetID[k_VerticalScroll],FALSE);
	XtSetSensitive(WidgetID[k_HorizontalScroll],FALSE);
	
	/* desensitize the save button until you have a file name */

	XtSetArg(argl[0],DwtNsensitive,FALSE);
	XtSetValues(WidgetID[k_SaveButton],argl,1);

	/* set up the graphics context for the
	   progress indicator in the launch in progress box.
	*/

	progressGC_values.function = GXcopy;

	progressGC = XCreateGC(XtDisplay(toplevel),
				XtWindow(toplevel),
				GCFunction,
				&progressGC_values);

	XSetLineAttributes(XtDisplay(WidgetID[k_LaunchInProgress]),
			   progressGC,
			   PROGRESS_LINE,
			   NULL,NULL,NULL);

	/* add an event handler to let me know when the user has resized
		the window 
	*/

	XtAddEventHandler(mainwidget, StructureNotifyMask, FALSE,
				Resizedproc, NULL);

	/* initialize the rubber box */

	InitRubberBox();
	
	/* Get the application context for use with work in progress */

	Context = XtWidgetToApplicationContext(toplevel);

	/* create the icons */         

	CreateIcon();
        resizeproc();
        XtAppMainLoop (Context);

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	Prefetch all the widgets activated by the menu items, so that they
**	will respond quickly
**
**--
*/

static void prefetch ()

{
if (DwtFetchWidget(Hierarchy, "FileBox",
	toplevel, &WidgetID[k_FileBox], &dummy) != DRMSuccess)
                    error("can't fetch file selection widget");

if (DwtFetchWidget(Hierarchy, "DataEntryBox",
	toplevel, &WidgetID[k_DataEntryBox], &dummy) != DRMSuccess)
		   error("Can't fetch data entry box");

if (DwtFetchWidget(Hierarchy, "CautionBox",
	toplevel, &WidgetID[k_CautionBox], &dummy) != DRMSuccess)
		   error("Can't fetch caution box");

if (DwtFetchWidget(Hierarchy, "ColorBox",
	toplevel, &WidgetID[k_ColorBox], &dummy) != DRMSuccess)
		   error("Can't fetch color box");

if (DwtFetchWidget(Hierarchy, "LaunchInProgress",
	toplevel, &WidgetID[k_LaunchInProgress], &dummy) != DRMSuccess)
		   error("Can't fetch launch in progress box");

if (DwtFetchWidget(Hierarchy, "AnimateInProgress",
	toplevel, &WidgetID[k_AnimateInProgress], &dummy) != DRMSuccess)
		   error("Can't fetch animate in progress box");

if (DwtFetchWidget(Hierarchy, "ProgressBox",
	WidgetID[k_LaunchInProgress], &WidgetID[k_progress_box],
							 &dummy) != DRMSuccess)
		   error("Can't fetch Progress box");
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This is just an error reporting procedure.
**
**--
*/

static void error(error_string )
char *error_string;
{
    printf( "%s\n", error_string );
	exit(1);
}



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	Callback routine for an exposure event
**	Determine which window was exposed and redraw that part of the 
**	image that was destroyed.
**
**
**--
*/


static void MyExpoproc(w,tag,expoevent)
Widget *w;
caddr_t tag;
struct mycallbackinfo *expoevent;

{
int tempx,tempy,tempwidth,tempheight;


	if (current.image == NULL) return;
	if (current.exists == 0) return;

	tempx = expoevent->event->x;
	tempy = expoevent->event->y;
 	tempwidth = expoevent->event->width;
	tempheight = expoevent->event->height;

	
	/* arbitrarilty if you resize the window the scroll bar goes back
	   to the beginning and the image is redisplayed from upper left
	   hand corner.
	*/

	if (current.pic > window_height)
		{
		XtSetSensitive(WidgetID[k_VerticalScroll],TRUE);
		}
	else
		{
                XtSetSensitive(WidgetID[k_VerticalScroll],FALSE);
		}

	if (current.pic > window_width)
		{
		XtSetSensitive(WidgetID[k_HorizontalScroll],TRUE);
		}
	else
		{
		XtSetSensitive(WidgetID[k_HorizontalScroll],FALSE);
		}

		XPutImage(	XtDisplay(WidgetID[k_workwidget]),
				XtWindow(WidgetID[k_workwidget]),
				DefaultGC(XtDisplay(WidgetID[k_workwidget]),0),
				current.image,
				tempx,
				tempy,
				tempx,
				tempy,
				tempwidth,
				tempheight);
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	Eventhandler for configure event - i.e. user has resize the window
**
**
**--
*/

static XtEventHandler Resizedproc(w, tag, event)

Widget w;
char *tag;
XConfigureEvent *event;

{
	if (event->type == ConfigureNotify)
		{
		ResetScrollBars();
		}
        return;
}
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This is the exitproc callback routine.
**
**       test for cleanliness then exit the program.
**--
*/

static void	exitproc( )
{
	if (new_image)
		{
		exit_flag = TRUE;
		ShouldISave();
		if (!exit_flag)return;
		}
	else
		exit(1);
	
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**
**  	saveasproc asks for a file name with the file selection widget
**      and saves your current work, with or without data.
**	If a file is already open it closes it first.
**
**
**--
*/

static void saveasproc ()

{
	/* set the save_as flag true so that the filebox routine knows
	   that we are doing a save as not just a save.
	*/

	save_as = TRUE;

	/* and change the title so the user knows what going on */

	XtSetArg(argl[0],DwtNtitle,"FileBox_save_title");

	DwtFetchSetValues(
		Hierarchy,
		WidgetID[k_FileBox],
		argl,
		1);

	XtManageChild(WidgetID[k_FileBox]);

}



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This is the newproc callback routine.
**
**--
*/

static void	newproc( )

{
	XtManageChild(WidgetID[k_DataEntryBox]);
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This is the openproc callback routine.
**
**--
*/

static void	openproc( )
{
	XtManageChild(WidgetID[k_FileBox]);
	
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	saveproc callback routine. saves a file when you have the
**	file name, else calls the saveasproc.
**
**
**--
*/

static void saveproc ()
{
	if (dirty)
		saveasproc(); /* give a chance to change file name */
	else
		MySaveFile();
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	launchproc - causes the mandelbrot to actually be computed
** 	for the values in current. Does not write a file.
**
**--
*/

static void launchproc(w, tag, reason)
	Widget	*w;
	int	*tag;
	unsigned long reason;
{

    int widget_num = *tag;              /* Convert tag to widget number. */
	
	switch (widget_num)
		{

		case k_LaunchButton:
			XtManageChild(WidgetID[k_LaunchInProgress]);
			XtManageChild(WidgetID[k_progress_box]);
			Launcher();
			break;

		case k_launch_data:
			current.exists = 0;
			RetrieveDataValues();
			dirty = TRUE;
			XtManageChild(WidgetID[k_LaunchInProgress]);
			XtManageChild(WidgetID[k_progress_box]);
			Launcher();
			break;
		
		default:
			printf("uh oh\n");
			break;
		}
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	Launcher - where the actual work gets done
**
**
**--
*/

static void Launcher()

{

	XDefineCursor(XtDisplay(mainwidget),XtWindow(mainwidget),wait_cursor);
	makeroom();
	mzoom();

	if (kill_launch)
		{
		XUndefineCursor(XtDisplay(mainwidget),XtWindow(mainwidget));
		kill_launch = FALSE; /* for next time */
		if (XtIsManaged(WidgetID[k_LaunchInProgress]))
			XtUnmanageChild(WidgetID[k_LaunchInProgress]);
		return;
		}

	makeimage();
	MyWindowResize();
	dispmand();

	if (XtIsManaged(WidgetID[k_LaunchInProgress]))
		XtUnmanageChild(WidgetID[k_LaunchInProgress]);
	XUndefineCursor(XtDisplay(mainwidget),XtWindow(mainwidget));
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	Color animation
**
**
**
**--
*/

static void animateproc ()

{
	GetLocation();
	XtManageChild(WidgetID[k_AnimateInProgress]);
	Animate(&colorCount,FALSE);

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	Brainfade mode - pretty
**
**
**
**--
*/

static void brainfadeproc ()

{
	GetLocation();
	XtManageChild(WidgetID[k_AnimateInProgress]);
	Animate(&colorCount,TRUE);
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	colorsproc callback routine handles the colors menu options
**
**
**--
*/

static void colorsproc()
{

	XtManageChild(WidgetID[k_ColorBox]);

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	resizeproc simply switches the autoresize flag and changes the
**	push button in the Options menu to no auto resize
**
**
**--
*/

static void resizeproc()
{


	if (auto_resize)
		{

		XtSetArg(argl[0],DwtNlabel,"AutoResize_label");

		DwtFetchSetValues(
			Hierarchy,
			WidgetID[k_ResizeButton],
			argl,
			1);
		auto_resize = FALSE;
		}
	else
		{
		XtSetArg(argl[0],DwtNlabel,"NoAutoResize_label");

		DwtFetchSetValues(
			Hierarchy,
			WidgetID[k_ResizeButton],
			argl,
			1);

		auto_resize = TRUE;
		MyWindowResize();
		}
			
}
	

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	VertScrollproc handles the input from the vertical scroll bar
**
**
**--
*/

static void VertScrollproc(w, tag, scroll)
    Widget w;
    int *tag;
    DwtScrollBarCallbackStruct *scroll;
{

	MyVertScroll(&scroll->value);
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	HorScrollproc handles the input from the Horizontal scroll bar
**
**
**--
*/

static void HorScrollproc(w, tag, scroll)
    Widget w;
    int *tag;
    DwtScrollBarCallbackStruct *scroll;
{

	MyHorScroll(&scroll->value);
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This is the WidgetIDproc callback routine.
**
**      Registers the widget ID's in an array when the widget is created.
**
**--
*/

static void WidgetIDproc(w, tag, reason)
    Widget w;
    int *tag;
    unsigned long *reason;
{
    int widget_num = *tag;

    WidgetID[widget_num] = w;
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	Activate procedure handles the pushbuttons in the work area
**	menu of the New data box.
**
**--
*/

static void Activateproc(w, tag, reason)
	Widget	*w;
	int	*tag;
	unsigned long reason;
{

    int widget_num = *tag;              /* Convert tag to widget number. */

	switch (widget_num)
		{

		case k_ok_data:
			current.exists = 0;
			RetrieveDataValues();
			dirty = TRUE;
			break;

		case k_reset_data:
			ResetDataValues();
			break;
	
		case k_cancel_data:
			XtUnmanageChild(WidgetID[k_DataEntryBox]);
			break;

		default:
			printf("uh oh\n");
			break;

		}

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	CautionBox procedure handles the input from the do you want
**	to save your file caution box.
**
**
**--
*/

static void CautionBoxproc(w, tag, reason)

	Widget	*w;
	int	*tag;
	unsigned long *reason;
{

    int widget_num = *tag;              /* Convert tag to widget number. */

	cancel_operation = FALSE;

	switch (*reason)
		{

		case DwtCRYes:
			XtUnmanageChild(WidgetID[k_CautionBox]);
			/* restore the old file name */
			strcpy(temp_file,current.filename);
			strcpy(current.filename,old_file);
			insaveascaution = TRUE;
			saveasproc();
			break;

		case DwtCRNo:
			XtUnmanageChild(WidgetID[k_CautionBox]);
			if (exit_flag) exit();
			DoFileInput();
			break;
	
		case DwtCRCancel:
			cancel_operation = TRUE;
			XtUnmanageChild(WidgetID[k_CautionBox]);
			if (exit_flag)
				{
				exit_flag = FALSE;
				return;
				} 
			DoFileInput();
			break;

		default:
			printf("uh oh\n");
			break;

		}
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	ColorBoxproc callback handles the input from the colors
**	dialog box
**
**
**--
*/

static void ColorBoxproc(w, tag, reason)
	Widget	*w;
	int	*tag;
	unsigned long *reason;
{

    int widget_num = *tag;              /* Convert tag to widget number. */

	switch (widget_num)
		{

		case k_apply_color:
			RetrieveColorValues();
			UpdateColorMap(&colorCount);
			break;

		case k_reset_color:
			ResetColorValues();
			break;
	
		case k_dismiss_color:
			old_red_mult = red_mult;
			old_green_mult = green_mult;
			old_blue_mult = blue_mult;
			XtUnmanageChild(WidgetID[k_ColorBox]);
			break;

		default:
			printf("uh oh\n");
			break;

		}

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**
**	MouseSizeproc - user is selecting an image area with the mouse
**
**
**
**--
*/

static void	MouseSizeproc( )

{
XtInputMask eventtype;

        XtAddEventHandler(WidgetID[k_workwidget], ButtonPressMask, FALSE,        
				ButtonPressproc, NULL);

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This is the Activate FileBox callback routine.
**
**	It processes the input from the File Selection Widget
**
**--
*/

static void FileBoxproc(w, tag, list)
    Widget w;
    int *tag;
    DwtListBoxCallbackStruct *list;

{
	
DwtRendMask	rend;
DwtCompStringContext 	dwContext;
char 		*text;
unsigned long	charset;
int 		dir;
unsigned long	lang;

	
	/* save the previous file name in case you need to save
	   the current contents
	*/

	strcpy(old_file,current.filename);

	/* Now read the value of the file name selected */

	/* extract the filename from the compound string */

	DwtInitGetSegment(&dwContext, list->item);
	DwtGetNextSegment(&dwContext,&text,&charset,
					&dir,&lang,&rend);
	


	strcpy(current.filename,text);
	XFree(text);

	/* And, unmanage the selection box */

	XtUnmanageChild(WidgetID[k_FileBox]);

	/* resensitize the Save button if file name not blank */

	if (strlen(current.filename) == 0)
		{
		/* received no file name don't do nothing but go
		   back to main routine
		*/
		printf("null name\n");
		return;
		}

	XtSetArg(argl[0],DwtNsensitive,TRUE);
	XtSetValues(WidgetID[k_SaveButton],argl,1);

	XDefineCursor(XtDisplay(mainwidget),
			      XtWindow(mainwidget),wait_cursor);

	XFlush(XtDisplay(toplevel));
	
	if (save_as)
		{	/* handle the save as case */
		MySaveFile();
	
		/* now reset the title and reset the flag */

		XtSetArg(argl[0],DwtNtitle,"FileBox_read_title");

		DwtFetchSetValues(
			Hierarchy,
			WidgetID[k_FileBox],
			argl,
			1);

		XUndefineCursor(XtDisplay(mainwidget),
				XtWindow(mainwidget));

		save_as = FALSE;
		if (exit_flag) exit(1);
		if (insaveascaution)
			{
			strcpy(current.filename,temp_file);
			DoFileInput();
			insaveascaution = FALSE;
			}
		}
	else
		{  	/* handle the read file case */

		/* if you've generated a new picture and haven't saved it, offer
	   	the option to do so before opening a newfile 
		*/
		
		if (new_image)
			{
			ShouldISave();
		
			XFlush(XtDisplay(toplevel));
			XtManageChild(WidgetID[k_CautionBox]);
			}
		else
			{
			DoFileInput();
			}
		}
		
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	DoFileInput handles all the work of actually reading in
**	the data, displaying, etc.
**
**	
**
**--
*/

static void DoFileInput()

{
		if (cancel_operation)
			{
			strcpy(current.filename,old_file);
			XUndefineCursor(XtDisplay(mainwidget),
					XtWindow(mainwidget));
			return;
			}

	MyOpenFile();


	/* now read the header of the file */

	myreadheaderproc();

	/* update the values in the data entry box */

	ResetDataValues();

	/* mallocate some space for the data */


	/* check and see whether the contents have been generated yet */ 


	if (current.exists == 1)
		{
		makeroom();
		readmand();  /* read the rest of the file */
		MyCloseFile();
		makeimage(); /* put it into image format */
		MyWindowResize(); /* set the window size */
		dispmand();  /* and then display it */
		XUndefineCursor(XtDisplay(mainwidget),XtWindow(mainwidget));
		return; 
		}
		

	/* if nothing there then blank the display */

	XClearWindow(	XtDisplay(WidgetID[k_workwidget]),
			XtWindow(WidgetID[k_workwidget]));
	MyCloseFile();

	XUndefineCursor(XtDisplay(mainwidget),XtWindow(mainwidget));
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This is the CancelFileBox callback routine.
**
**	It handles the case where the user presses the -cancel- button
**
**--
*/

static void CancelFileBoxproc()

{
	XtUnmanageChild(WidgetID[k_FileBox]);

	if (save_as)
		{	
		/* handle the save as case */

		/* now reset the title and reset the flag */

		XtSetArg(argl[0],DwtNtitle,"FileBox_read_title");

		DwtFetchSetValues(
			Hierarchy,
			WidgetID[k_FileBox],
			argl,
			1);

		XUndefineCursor(XtDisplay(mainwidget),
				XtWindow(mainwidget));

		save_as = FALSE;
		}

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This proc. reads the header of a mandelbrot file, passed in
**	the structure current.
**
**
**--
*/

static void myreadheaderproc()

{


fscanf(current.fip,"%d",&current.exists);
fscanf(current.fip,"%E",&current.real);
fscanf(current.fip,"%E",&current.imaginary);
fscanf(current.fip,"%E",&current.side);
fscanf(current.fip,"%d",&current.pic);
fscanf(current.fip,"%d",&current.it);

}




/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**  	Allocate enough memory to fit the image.  First time thru
**	malloc it, after that realloc.
**
**
**--
*/

static void makeroom()

{
int mysize;

	mysize = current.pic*current.pic*sizeof(*hptr);

	if (first)
		{
		hptr = (char *)XtMalloc(mysize);
		if (hptr == 0)
			 error("Can't Allocate Memory.\n");
		first = FALSE;
		return;
		};
	hptr = (char *)XtRealloc(hptr,mysize);
	if (hptr == 0)
		error("Can't Reallocate Memory.\n");

}



static void mzoom()
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	Routine to actual generate the Mandelbrot set.  At the end of 
**	the generation it is written to a file
**
**
**--
*/

{
int n,m,count,mcount,scount,numbytes;
double az,bz,azs,bzs,act,gap,ac,bc;
XtInputMask eventtype;
static int mycount;

	gap = current.side / (double)current.pic;

	while (eventtype = XtAppPending(Context) == XtIMXEvent)
		{
                XFlush(XtDisplay(toplevel));
		if ( XtAppPeekEvent(Context,&work_event) == XtIMXEvent)
			{
			XtAppProcessEvent(Context,XtIMXEvent);
			}
		}

	/* put up the status display in the WIP box.  First draw
	   the decorations then when appropriate update the progress.
	*/

	total = 0; /* reset the percentage counter */


mycount = 0;

for (m = 0; m < current.pic; m++)
	{
	/* once every outer loop, give a chance to cancel */

	if (kill_launch)
		{
		ReInit();
		return;
		}

	for (n = 0; n < current.pic; n++)
		{
		ac = current.real + ((double)n*gap);
		bc = current.imaginary + ((double)m*gap);
		az = 0;
		bz = 0;
		count = 0;
		azs = az * az;
		bzs = bz * bz;

	/* every hundred times peek ahead - there's a performance
	   hit for this, but it let's you see what's going on.
	*/

	if ( (n % 100) == 0)
		{
		while (eventtype = XtAppPending(Context) == XtIMXEvent)
			{
                        XFlush(XtDisplay(toplevel));
			if ( XtAppPeekEvent(Context,&work_event) == XtIMXEvent)
				{
				XtAppProcessEvent(Context,XtIMXEvent);
				}
			}
		}

		while (count++ < current.it && azs+bzs <= 4)
			{
			act = az;
			az = azs - bzs + ac;
			bz = (2.*act * bz) + bc;
			azs = az * az;
			bzs = bz * bz;
			}
		
		*(hptr+(mycount++)) = (char)count;

		}  /* end of n loop */

	/* update the status display */

	total = total + ((float)PROGRESS_WIDTH / (float)current.pic);

	XDrawLine ( 	XtDisplay(WidgetID[k_progress_box]),
			XtWindow(WidgetID[k_progress_box]),
			progressGC,
			PROGRESS_SX,
			PROGRESS_SY,
			PROGRESS_SX+(int)total,
			PROGRESS_SY);

	}  /* end of m loop */
	
	current.exists = 1;

	new_image = TRUE;

	if (XtIsManaged(WidgetID[k_LaunchInProgress]))
		XtUnmanageChild(WidgetID[k_LaunchInProgress]);
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	Re-init - set up the program variables indicating that
**	you have no image, but you might still have a filename and
**	some parameters.
**
**
**--
*/

static void ReInit()
{
        XtSetSensitive(WidgetID[k_VerticalScroll],FALSE);
	XtSetSensitive(WidgetID[k_HorizontalScroll],FALSE);
	first = TRUE;

	current.exists = 0;
	XtFree(hptr);
	
	XClearWindow(	XtDisplay(WidgetID[k_workwidget]),
			XtWindow(WidgetID[k_workwidget]));
	XtSetSensitive(WidgetID[k_AnimateButton],FALSE);
	XtSetSensitive(WidgetID[k_BrainFadeButton],FALSE);
	
}



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	Read the remaining contents of an image file, into the 
**	holding area.
**
**
**--
*/

static void readmand()

{

int m,numbytes;


	if (numbytes = fread(hptr
		,current.pic*sizeof(*hptr)
		,current.pic,current.fip) != EOF);


}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	Take the contents of the holdingarea and place it into an
**	image structure.  This is the first time after an image is
**	is generated, and then each time an image is read from disk.
**
**
**--
*/

static void makeimage()

{
	if (current.image != NULL)
		{
		/* destroy the previous image */
		/* don't worry about the image data, that's handled
		   elsewhere.
		*/
		XFree(current.image);
		}

	current.image = XCreateImage( 	
			XtDisplay(WidgetID[k_workwidget]),
			DefaultVisual(XtDisplay(WidgetID[k_workwidget]),0),
			DefaultDepth(XtDisplay(WidgetID[k_workwidget]),0),
			ZPixmap,
			0,
			hptr,
			current.pic,
			current.pic,
			8,
			0);
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	Display the Current Instance Mandelbrot image on the screen
**	Assume all the proper windows are made, but not managed
**
**
**--
*/

static void dispmand()
{

	/* first clear the window in case the new display is smaller
   	   than the old one.
	*/

	XClearWindow(	XtDisplay(WidgetID[k_workwidget]),
			XtWindow(WidgetID[k_workwidget]));

	/* now write the image into the window */

	XPutImage(	XtDisplay(WidgetID[k_workwidget]),
			XtWindow(WidgetID[k_workwidget]),
			DefaultGC(XtDisplay(WidgetID[k_workwidget]),0),
			current.image,
			0,
			0,
			0,
			0,
			MIN(current.pic,window_width),
			MIN(current.pic,window_height));
	
	/* Sensitize the mouse button, because there is now something to
	   select from.
	*/

	XtSetArg(argl[0],DwtNsensitive,TRUE);
	XtSetValues(WidgetID[k_MouseSizeButton],argl,1);
	/* and sensitize the animation buttons */

        XtSetSensitive(WidgetID[k_AnimateButton],TRUE);
        XtSetSensitive(WidgetID[k_BrainFadeButton],TRUE);

	/* reset the scroll bars */

	ResetScrollBars ();

}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	Resize the mainwindow and workwindow to display the entire image.
**	This is the default, then the user can resize from there.
**
**
**--
*/

static void MyWindowResize()
{
	
	GetWindowSize(); /* for the first time through */
	if (auto_resize)
		{
		XtSetArg(argl[0],DwtNwidth,current.pic);
		XtSetArg(argl[1],DwtNheight,current.pic);
		XtSetValues(WidgetID[k_workwidget],argl,2);
		GetWindowSize ();
		}

}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	Retrieve the data from the data entry box.
**
**
**--
*/

static void RetrieveDataValues()

{

char *input_text;

	input_text = DwtSTextGetString(WidgetID[k_xdim]);
	current.real = 0;
	sscanf(input_text, "%E", &current.real);
	input_text = DwtSTextGetString(WidgetID[k_ydim]);
	current.imaginary = 0;
	sscanf(input_text, "%E", &current.imaginary);
	input_text = DwtSTextGetString(WidgetID[k_length]);
	current.side = 0;
	sscanf(input_text, "%E", &current.side);
	input_text = DwtSTextGetString(WidgetID[k_pixels]);
	current.pic = 0;
	sscanf(input_text, "%d", &current.pic);
	input_text = DwtSTextGetString(WidgetID[k_iterations]);
	current.it = 0;
	sscanf(input_text, "%d", &current.it);

}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**
**	Reset the data entry box values
**
**--
*/

static void ResetDataValues()

{
	/* reset back to the current instance */

char temptext[20];


	sprintf(temptext,"%G",current.real);
	DwtSTextSetString(WidgetID[k_xdim],temptext);
	sprintf(temptext,"%G",current.imaginary);
	DwtSTextSetString(WidgetID[k_ydim],temptext);
	sprintf(temptext,"%G",current.side);
	DwtSTextSetString(WidgetID[k_length],temptext);
	sprintf(temptext,"%d",current.pic);
	DwtSTextSetString(WidgetID[k_pixels],temptext);
	sprintf(temptext,"%d",current.it);
	DwtSTextSetString(WidgetID[k_iterations],temptext);
	
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	Retrieve the data from the color box.
**
**
**--
*/

static void RetrieveColorValues()

{

char *input_text;

	input_text = DwtSTextGetString(WidgetID[k_red_mult]);
	sscanf(input_text, "%d", &red_mult);
	input_text = DwtSTextGetString(WidgetID[k_green_mult]);
	sscanf(input_text, "%d", &green_mult);
	input_text = DwtSTextGetString(WidgetID[k_blue_mult]);
	sscanf(input_text, "%d", &blue_mult);

}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**
**	Reset the values in the color box
**
**
**--
*/

static void ResetColorValues()

{
	/* reset back to the current instance */

char temptext[20];


	sprintf(temptext,"%d",old_red_mult);
	DwtSTextSetString(WidgetID[k_red_mult],temptext);
	sprintf(temptext,"%d",old_green_mult);
	DwtSTextSetString(WidgetID[k_green_mult],temptext);
	sprintf(temptext,"%d",old_blue_mult);
	DwtSTextSetString(WidgetID[k_blue_mult],temptext);
}


                                 
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**
**	MyOpenFile Opens a file.  
**
**
**--
*/

static void MyOpenFile()

{
	if ((int)
	(current.fip = fopen(current.filename,"r+")) == NULL)
		{
		/* open a new file if old file isn't there */
		if ((int)
			(current.fip = 
				fopen(current.filename,"w+")) == NULL)
        				printf("Can't open file, %s\n",	
						current.filename);
		}
	new_image = FALSE;
	dirty = FALSE;

}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**
**	MyCloseFile closes an existing file.  
**
**
**--
*/

static void MyCloseFile ()

{
	fclose(current.fip);
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This is the actual file save routine.  Assume that if you've gotten
**      here you have an open file, but the data is no good.  So reset
**	to the beginning of the file and save what you have.  Either
**	the parameters or the parameters and data if it exists.
**	The file is not closed here.
**
**
**--
*/

static void	MySaveFile( )
{
int numbytes;
	   	
	MyOpenFile();

	fprintf(current.fip,"%d\n",current.exists);
	fprintf(current.fip,"%G\n",current.real);
	fprintf(current.fip,"%G\n",current.imaginary);
	fprintf(current.fip,"%G\n",current.side);
	fprintf(current.fip,"%d\n",current.pic);
	fprintf(current.fip,"%d\n",current.it);

	fflush(current.fip);

	if (current.exists == 1)
		numbytes = fwrite(hptr,
				current.pic*sizeof(*hptr),
				current.pic,current.fip);
	new_image = FALSE;
	dirty = FALSE;
	MyCloseFile();
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**
**	ShouldISave asks the eternal question: do you want to save your
**	old data before messing with the new stuff?
**
**
**--
*/

static void ShouldISave()
	
{
	
	XtManageChild(WidgetID[k_CautionBox]);

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**
**	SetUpColorMap creates the colormap
**
**
**--
*/

static void SetUpColorMap ()

{

	/* Set up the colormap */

	vInfoDummy.visualid = 
			(DefaultVisual(XtDisplay(toplevel),0))->visualid;
	pVisualInfo = 
			XGetVisualInfo(XtDisplay(toplevel),
			VisualIDMask,&vInfoDummy,&nVis);
	pixel = WhitePixel(XtDisplay(toplevel),
			   DefaultScreen(XtDisplay(toplevel)));
	updateColor = 0;

	if (pVisualInfo->class != PseudoColor &&
	    pVisualInfo->class != DirectColor &&
	    pVisualInfo->class != GrayScale)
	    {

		error("Unable to use this visual class");
		exit(0);
	    }

	if (colorCount > pVisualInfo->colormap_size)
	    colorCount = pVisualInfo->colormap_size-2;
	
	map = XDefaultColormap(XtDisplay(toplevel),
				DefaultScreen(XtDisplay(toplevel)));

	XSetWindowColormap(XtDisplay(toplevel),XtWindow(toplevel),map);
	
	/* Set up a graphics context: */

	colors = (XColor *)XtMalloc(sizeof(*colors)*colorCount);
	
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**
**	And then load some colors into the color map
**
**
**--
*/


static void LoadColorMap(count)

int *count;
{

    unsigned int i,j,fullcount;
    double h,r,g,b;
    int rx,gx,bx;
    int dummy;

	while (!XAllocColorCells(XtDisplay(toplevel),
				map,0,&dummy,0,pixels,*count) && *count >= 1)
	    {
	    (*count)--;
	    }
	if (*count < 1)
	    {
	    error("Could not allocate enough colors");
	    exit(0);
	    }

	/* This loop find out if we are using WhitePixel and BlackPixel */

	fullcount = *count;

	for (j=0,i=0; i < fullcount; i++)
	    {
	    if (pixels[i]==BlackPixel(XtDisplay(toplevel),
				DefaultScreen(XtDisplay(toplevel))) || 
			pixels[i]==WhitePixel(XtDisplay(toplevel),
				DefaultScreen(XtDisplay(toplevel))))
		{
		if (XAllocColorCells(XtDisplay(toplevel),
					map,0,&dummy,0,&pixels[i],1))
		    i--; /*Got a replacement value; try it again*/
		else
		    (*count)--; /*No replacement; forget this one*/
		}
	    else
		{
		colors[j++].pixel = pixels[i];
		}
	    }

	if (*count < 1)
	    {
	    error("Could not allocate enough colors");
	    exit(0);
	    }

    /*This loop fills in the colors*/


    for (i=0; i<*count ; i++)
	{
	colors[i].flags=7;
	colors[i].red = 0;
	colors[i].green = 0;
	colors[i].blue = 0;
	}
    XStoreColors(XtDisplay(toplevel),map,colors,*count);

    UpdateColorMap(count);
}
    
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**
**	UpdateColorMap - actually sticks new values into color map
**
**
**
**--
*/


static void UpdateColorMap(count)

int *count;

{

int i;

    for (i=0; i<*count ; i++)
	{
	if (effect)
		{
		colors[i].flags=doprimary;
		colors[i].red = ((i*256)/(*count)) * 256 * red_mult;
		colors[i].green = ((i*256)/(*count))* 256 * green_mult;
		colors[i].blue = ((i*256)/(*count))*256 * blue_mult;
		}
	else
		{
		colors[i].flags=doprimary;
		colors[i].red = ((i*256)/(*count)) * 256 << red_mult;
		colors[i].green = ((i*256)/(*count))* 256 << green_mult;
		colors[i].blue = ((i*256)/(*count))*256 << blue_mult;
		}
	}

    XStoreColors(XtDisplay(toplevel),map,colors,*count);

}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	ColorIconproc handles the icon push buttons in the color
**	options dialog box.
**
**
**--
*/

static void ColorIconproc ( w, tag, reason)
	Widget	*w;
	int	*tag;
	unsigned long reason;
{

int widget_num = *tag;              /* Convert tag to widget number. */
char temptext[20];

	switch (widget_num)
		{
		case k_red_up:
			red_mult = red_mult + inc;
			sprintf(temptext,"%d",red_mult);
			DwtSTextSetString(WidgetID[k_red_mult],temptext);
			break;
		
		case k_red_down:
			red_mult = red_mult - inc;
			sprintf(temptext,"%d",red_mult);
			DwtSTextSetString(WidgetID[k_red_mult],temptext);
			break;

		case k_green_up:
			green_mult = green_mult + inc;
			sprintf(temptext,"%d",green_mult);
			DwtSTextSetString(WidgetID[k_green_mult],temptext);
			break;
		
		case k_green_down:
			green_mult = green_mult - inc;
			sprintf(temptext,"%d",green_mult);
			DwtSTextSetString(WidgetID[k_green_mult],temptext);
			break;

		case k_blue_up:
			blue_mult = blue_mult + inc;
			sprintf(temptext,"%d",blue_mult);
			DwtSTextSetString(WidgetID[k_blue_mult],temptext);
			break;
		
		case k_blue_down:
			blue_mult = blue_mult - inc;
			sprintf(temptext,"%d",blue_mult);
			DwtSTextSetString(WidgetID[k_blue_mult],temptext);
			break;
		
		default:
			printf("uh oh\n");
			break;
		}
		UpdateColorMap(&colorCount);

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**
**	Effectsproc - sets the global flag effect to 0 if the users
**	chooses the multiply effect and 1 if the user chooses the
**	shift effect.
**
**
**
**--
*/

static void Effectsproc ( w, tag, reason)
	Widget	*w;
	int	*tag;
	unsigned long reason;
{

    int widget_num = *tag;              /* Convert tag to widget number. */

	switch (widget_num)
		{
		case k_mult:
			effect = 0;
			break;
		case k_shift:
			effect = 1;
			break;
		default:
			error("uh oh");
		}

	UpdateColorMap(&colorCount);
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**
**	Incproc - determine the size of the color increment
**
**
**--
*/

static void Incproc ( w, tag, reason)
	Widget	*w;
	int	*tag;
	unsigned long reason;
{

    int widget_num = *tag;              /* Convert tag to widget number. */

	switch (widget_num)
		{
		case k_1:
			inc = INC_10;
			break;
		case k_10:
			inc = INC_1;
			break;
		default:
			error("uh oh");
		}
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**
**	GetWindowSize returns the X and Y window size into
**	window_width, window_height.
**	It also sets some values for the scroll bars.
**
**
**
**--
*/

static void GetWindowSize()

{
static 	int y_dist, x_dist; 	/* temp variables for scrolling */

	XtSetArg(argl[0],DwtNwidth,&window_width);
	XtSetArg(argl[1],DwtNheight,&window_height);
	XtGetValues(WidgetID[k_workwidget],argl,2);
	
	/* take into account the width of the scroll bars */

	window_height = window_height;
	window_width = window_width;

	/* Compute the number of pixels per step for the scroll bars */

	y_dist = current.pic - window_height;
	y_step = MAX(1,(int) y_dist / MAX_SCROLL);      

	upper_image_from = 0;
	lower_image_from = window_height;

	x_dist = current.pic - window_width;
	x_step = MAX(1,(int) x_dist / MAX_SCROLL);      

	left_image_from = 0;
	right_image_from = window_width;
	
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**
**	ResetScrollBars resets the sliders to the top and left and
**	sets the values to 0
**
**
**
**--
*/

static void ResetScrollBars()

{
static int shown;

	GetWindowSize();
	
	/* vertical scroll bar */

	y_scroll = 0;
	y_old_scroll = 0;
	upper_image_from = 0;
	lower_image_from = window_height;

	shown = (int) ((float)MAX_SCROLL * ((float)window_height / 
					(float)current.pic));

	DwtScrollBarSetSlider(WidgetID[k_VerticalScroll],
			0,
			shown,
			1,10,TRUE); /* rather arbitrary numbers */

	/* horizontal scroll bar */

	x_scroll = 0;
	x_old_scroll = 0;
	left_image_from = 0;
	right_image_from = window_width;


	shown = (int) ((float)MAX_SCROLL * ((float)window_width / 
					(float)current.pic));

	DwtScrollBarSetSlider(WidgetID[k_HorizontalScroll],
			0,
			shown,
			1,10,TRUE); /* rather arbitrary numbers */

}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**
**	MyVertScroll - perform a vertical scroll
**
**
**
**--
*/

static void MyVertScroll(y_value)
int *y_value;
{
static int y_delta;
static int area_height,area_from,area_to;
static int image_height,image_from,image_to;

	/* first compute the distance moved */


	y_old_scroll = y_scroll;
	y_scroll = *y_value * y_step;
	y_delta = y_scroll - y_old_scroll;

	/* next the area of good data on the screen to copy */

	if (y_delta > 0)
		{
		area_height = window_height - y_delta;
		area_from = y_delta;
		area_to = 0;
		}
	else
		{
		area_height = window_height + y_delta;
		area_from = 0;
		area_to = -y_delta;
		}
 
	/* finally the new area that must be brought in and displayed */

	if (y_delta > 0)
		{
		image_height = y_delta;
		image_from = lower_image_from;
		lower_image_from = lower_image_from + y_delta;
		if (lower_image_from > current.pic)
			lower_image_from = current.pic;
		upper_image_from = upper_image_from + y_delta;
		if (upper_image_from > current.pic) 
			upper_image_from = current.pic;
		image_to = window_height - y_delta;
		}	
	else
		{
		image_height = -y_delta;
		lower_image_from = lower_image_from + y_delta;
		upper_image_from = upper_image_from + y_delta;
		image_from = upper_image_from;
		image_to = 0;
		}

	XCopyArea(	XtDisplay(WidgetID[k_workwidget]),
			XtWindow(WidgetID[k_workwidget]),
			XtWindow(WidgetID[k_workwidget]),
			DefaultGC(XtDisplay(WidgetID[k_workwidget]),0),
			0,
			area_from,
			current.pic,
			area_height,
			0,
			area_to);

	XFlush(XtDisplay(toplevel));

	XPutImage(	XtDisplay(WidgetID[k_workwidget]),
			XtWindow(WidgetID[k_workwidget]),
			DefaultGC(XtDisplay(WidgetID[k_workwidget]),0),
			current.image,
			left_image_from,
			image_from,
			0,
			image_to,
			current.pic,
			image_height);
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**
**	MyHorzScroll - perform a Horizontal scroll
**
**
**
**--
*/

static void MyHorScroll(x_value)
int *x_value;
{
static int x_delta;
static int area_width,area_from,area_to;
static int image_width,image_from,image_to;

	/* first compute the distance moved */

	x_old_scroll = x_scroll;
	x_scroll = *x_value * x_step;
	x_delta = x_scroll - x_old_scroll;

	/* next the area of good data on the screen to copy */

	if (x_delta > 0)
		{
		area_width = window_width - x_delta;
		area_from = x_delta;
		area_to = 0;
		}
	else
		{
		area_width = window_width + x_delta;
		area_from = 0;
		area_to = -x_delta;
		}
 
	/* finally the new area that must be brought in and displayed */

	if (x_delta > 0)
		{
		image_width = x_delta;
		image_from = right_image_from;
		right_image_from = right_image_from + x_delta;
		if (right_image_from > current.pic)
			right_image_from = current.pic;
		left_image_from = left_image_from + x_delta;
		if (left_image_from > current.pic) 
			left_image_from = current.pic;
		image_to = window_width - x_delta;
		}	
	else
		{
		image_width = -x_delta;
		right_image_from = right_image_from + x_delta;
		left_image_from = left_image_from + x_delta;
		image_from = left_image_from;
		image_to = 0;
		}

	XCopyArea(	XtDisplay(WidgetID[k_workwidget]),
			XtWindow(WidgetID[k_workwidget]),
			XtWindow(WidgetID[k_workwidget]),
			DefaultGC(XtDisplay(WidgetID[k_workwidget]),0),
			area_from,
			0,
			area_width,
			current.pic,
			area_to,
			0);

	XFlush(XtDisplay(toplevel));

	XPutImage(	XtDisplay(WidgetID[k_workwidget]),
			XtWindow(WidgetID[k_workwidget]),
			DefaultGC(XtDisplay(WidgetID[k_workwidget]),0),
			current.image,
			image_from,
			upper_image_from,
			image_to,
			0,
			image_width,
			current.pic);
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**
**	ButtonPressproc an event handler called whenever the user presses
**	a mouse button
**
**
**
**--
*/


static XtEventHandler ButtonPressproc ( widget, tag, event)

Widget widget;
char *tag;
XButtonEvent *event;

{

Window root_win, child_win;   /* returns for query pointer */
int root_x, root_y;
int my_x, my_y;	/* the numbers I really want */
unsigned int my_buttons; /* status of the buttons at query time */

int org_x, org_y; 	/* starting origin of box */

int from_x, from_y, box_size; /* used to actually draw */

int old_x, old_y, old_size;   /* for erasing previous box */

double real_out, imaginary_out; /* converted coordinates in complex plane */
double side_out;		/* converted width */

XtInputMask	eventtype;

	org_x = event->x;
	org_y = event->y;
	
	old_size = 0;

	    /* raise the window to get the dialog box out of the way */

	    XRaiseWindow(XtDisplay(WidgetID[k_workwidget]),
			 XtWindow(WidgetID[k_workwidget]));


            do
              {

              XQueryPointer(XtDisplay(WidgetID[k_workwidget]),
				XtWindow(WidgetID[k_workwidget]), 
				&root_win, &child_win,
                		&root_x, &root_y, 
				&my_x, &my_y, &my_buttons);

	      if (my_buttons & Button3Mask) 
			{
			if (!first_box)
				XDrawRectangle(
                  		XtDisplay(WidgetID[k_workwidget]),
                  		XtWindow(WidgetID[k_workwidget]),
                  		rubberGC,
                  		old_x, old_y,
                  		old_size, old_size);

		            /* remove the event handler, not needed now */

			    XtRemoveEventHandler(WidgetID[k_workwidget], 
							ButtonPressMask, FALSE,
							ButtonPressproc, NULL);
			    return;
			}

	      
		/* rectangle only takes positive numbers, also force
		   to be square.
		*/

	box_size = 
		(int)sqrt((double)abs(((my_x - org_x) * (my_x-org_x)) + 
				((my_y - org_y) * (my_y - org_y))));

		if (my_x > org_x)
			{
			from_x = org_x;
			}
		else
			{
			from_x = org_x - box_size;
			}

		if (my_y > org_y)
			from_y = org_y;
		else
			from_y = org_y - box_size;

	    if ( box_size != old_size)
		{
		if (first_box)
			{
			XDrawRectangle(
				XtDisplay(WidgetID[k_workwidget]),
				XtWindow(WidgetID[k_workwidget]),
				rubberGC,
				from_x, from_y,
				box_size, box_size);
			first_box = FALSE;
			old_x = from_x;
			old_y = from_y;
			old_size = box_size;	
			}
		else
			{
			/* erase the old box then draw new one */

	                XDrawRectangle(
                                XtDisplay(WidgetID[k_workwidget]),
                                XtWindow(WidgetID[k_workwidget]),
                                rubberGC,
                                old_x, old_y,
                                old_size, old_size);
	
                         XDrawRectangle(
                                XtDisplay(WidgetID[k_workwidget]),
                                XtWindow(WidgetID[k_workwidget]),
                                rubberGC,
                                from_x, from_y,
                                box_size, box_size);
			}
		old_x = from_x;
		old_y = from_y;
		old_size = box_size;	
		}
              }
             while (my_buttons & Button1Mask);

	/* when you're all done set first_box true so it's set up
	   for the next time thru
	*/

	first_box = TRUE;

	/* and one final one to turn off the previous one */

	XDrawRectangle(
                  XtDisplay(WidgetID[k_workwidget]),
                  XtWindow(WidgetID[k_workwidget]),
                  rubberGC,
                  old_x, old_y,
                  old_size, old_size);

	/* remove the event handler, not needed now */

	XtRemoveEventHandler(WidgetID[k_workwidget], ButtonPressMask, FALSE,
				ButtonPressproc, NULL);

	/* convert the selected area to coordinates in the complex plane */

	ConvertCoords(from_x,from_y,box_size,&real_out,
					&imaginary_out,&side_out);

	/* and display them in the data entry box */

	DisplayMouseData(real_out,imaginary_out,side_out);

}

    
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**
**	Initialize variables for the rubber box
**
**
**
**--
*/

static void InitRubberBox()


{
	rubberGC_values.function = GXinvert;
	rubberGC_values.foreground = current_fc.pixel;
	rubberGC_values.background = current_bc.pixel;

        rubberGC = XtGetGC(WidgetID[k_workwidget],
          		GCForeground | GCBackground | GCFunction, 
			&rubberGC_values);

	first_box = TRUE;
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**
**	Convert the input parameters from screen coordinates to
**	coordinates in the complex plane
**
**
**
**--
*/

static void ConvertCoords(in_x,in_y,in_width,real,imaginary,side)

int in_x, in_y, in_width;

double *real, *imaginary, *side;
{
	/* first take into account any scrolling that may have
	   taken place.
	*/

	in_x = in_x + left_image_from;
	in_y = in_y + upper_image_from;

	*real = current.real + 
			(double)((current.side / 
				(double) current.pic) * 
							(double) in_x);

	*imaginary = current.imaginary +
			((current.side / 
				(double) current.pic) * 
							(double) in_y);

	*side = (current.side * (double) in_width) / 
				(double) current.pic;	

}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**
** 	Display the mouse data, but don't change the actuals, wait until
**	users hits the OK button.
**
**
**
**--
*/

static void DisplayMouseData(real, imaginary, side)

double real, imaginary, side;

{
char temptext[20];


	sprintf(temptext,"%G",real);
	DwtSTextSetString(WidgetID[k_xdim],temptext);
	sprintf(temptext,"%G",imaginary);
	DwtSTextSetString(WidgetID[k_ydim],temptext);
	sprintf(temptext,"%G",side);
	DwtSTextSetString(WidgetID[k_length],temptext);

}
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	Animate - flicker them colors
**
**
**
**--
*/


static void Animate(count,fade)

char fade; /* if true then go into brain fade mode */
int *count;
{

unsigned int tempcolor;
int i,j;

  	XDefineCursor(XtDisplay(mainwidget),XtWindow(mainwidget),wait_cursor);

	/* if brain fade mode then just cycle once for the number of
	   colors, else animate forever.
	*/

	for (j=0; fade ? j < *count : TRUE;j++)
	{
    	tempcolor = colors[0].pixel;
    	for (i=0; i<((*count-1)) ; i++)
		{
       		 colors[i].pixel = colors[i+1].pixel;
		}

		if (fade)
			colors[(*count)].pixel = tempcolor;
		else
			colors[(*count-1)].pixel = tempcolor;

	XStoreColors(XtDisplay(toplevel),map,colors,*count);

	if (XtAppPending(Context))
		{
		if ( XtAppPeekEvent(Context,&work_event) == XtIMXEvent)
			{
			XtAppProcessEvent(Context,XtIMXEvent);
			}
		}

	if (kill_animation) break;
	}
	
	    /* reset for next time */

	    kill_animation = FALSE;

       	    /* and restore to original */

	    for (i=0; i<*count ; i++)
		{
		colors[i].pixel = pixels[i];
		colors[i].flags=7;
		colors[i].red = 0;
		colors[i].green = 0;
		colors[i].blue = 0;
		}

	UpdateColorMap(count);

	/* if brain faded away then unmanage the wip box */

	if (XtIsManaged(WidgetID[k_AnimateInProgress]))
		XtUnmanageChild(WidgetID[k_AnimateInProgress]);

	XFlush(XtDisplay(toplevel));

	XUndefineCursor(XtDisplay(mainwidget),XtWindow(mainwidget));
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	
**	Cancel Launchproc - cancel the mandelbrot generation in progress
**
**
**
**--
*/

static void CancelLaunchproc()
{
	kill_launch = TRUE;
	XtUnmanageChild(WidgetID[k_LaunchInProgress]);
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	cancelanimateproc - cancel the animation and continue
**
**
**
**--
*/

static void CancelAnimateproc()

{
	kill_animation = TRUE;
	XtUnmanageChild(WidgetID[k_AnimateInProgress]);
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**
**	Create Icon - create a pixmap for the icon and create the ico
**
**
**--
*/

static void CreateIcon()

{


	icon_pixmap = XCreateBitmapFromData(XtDisplay(toplevel),
			      XtWindow(toplevel),
			      icon_bits,
			      icon_width,
			      icon_height);

	iconify_pixmap = XCreateBitmapFromData(XtDisplay(toplevel),
			      XtWindow(toplevel),
			      iconify_bits,
			      iconify_width,
			      iconify_height);

	XtSetArg(argl[0],XtNiconifyPixmap,iconify_pixmap);
	XtSetArg(argl[1],XtNiconPixmap,icon_pixmap);
	XtSetValues(toplevel,argl,2);

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	GetLocation - figures out the current size of the window and
**	computes where the work in progress boxes should go.
**
**
**
**--
*/

static void GetLocation()

{
static int caution_x, caution_y, caution_w;
static int wind_x, wind_y;

	/* place animate in progress box at upper right hand screen
	   just outside of the main window if room, else just inside
	   main window.
	*/

	/* get the width of the caution box */

	XtSetArg(argl[0],DwtNwidth,&caution_w);
	XtGetValues(WidgetID[k_AnimateInProgress],argl,1);
	
	/* get the location of the main window */

	XtSetArg(argl[0],DwtNx,&wind_x);
	XtSetArg(argl[1],DwtNy,&wind_y);
	XtGetValues(toplevel,argl,2);

	caution_x = window_width + wind_x + X_MARGIN;

	if (caution_x + caution_w > MAX_WIDTH)
			caution_x = MAX_WIDTH 
					- caution_w - X_MARGIN;

	caution_y =  (wind_y + Y_MARGIN) > MAX_HEIGHT ? Y_MARGIN : 
							wind_y + Y_MARGIN;


	XtSetArg(argl[0],DwtNx,caution_x);
	XtSetArg(argl[1],DwtNy,caution_y);
	XtSetValues(WidgetID[k_AnimateInProgress],argl,2);
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	Helpproc - handle help
**
**
**
**--
*/

static void Helpproc(widget, tag, reason)
    	Widget widget;
    	int *tag;
    	DwtAnyCallbackStruct *  reason;
{
    DwtCompString       firstTopic = (DwtCompString) tag;
    DRMType *           class;
    int                 status;
    Arg                 args[1];

    /*
     If this is the first time that the user has selected help, then
     fetch the help widget from the widget hierarchy.
    */

    if (WidgetID[k_HelpBox] == (Widget) NULL) 

		{
        	if (DwtFetchWidget(Hierarchy, "HelpBox",
                                toplevel, &WidgetID[k_HelpBox], &dummy) 
			!= DRMSuccess)
			error("can't fetch help widget");
        	}


    XtSetArg(args[0], DwtNfirstTopic, firstTopic);
    XtSetValues(WidgetID[k_HelpBox], args, 1);

    XtManageChild(WidgetID[k_HelpBox]);
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	End help proc - unmanage the help box
**
**
**
**--
*/

static void EndHelpproc()

{
	XtUnmanageChild(WidgetID[k_HelpBox]);
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      Command line usage
**
**
**
**--
*/
           
usage (program)
    char *program;
{
    fprintf(stderr,"\n");
    fprintf(stderr,"Usage: %s [ options ] \n",program);
    fprintf(stderr,"\n");
    fprintf(stderr,"     Where options are one or more of:\n");
    fprintf(stderr,"\n");
    fprintf(stderr,"     [-x #] [-y #]                  x and y window position\n");
    fprintf(stderr,"     [-title WindowTitle]           Title of YADMAN window\n");
    fprintf(stderr,"\n");
    exit(1);
}

/* End of Yadman.c */
    

