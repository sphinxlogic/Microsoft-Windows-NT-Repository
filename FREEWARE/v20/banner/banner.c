#module Banner "V1.0"

/*
**++
**  FACILITY:
**
**      Main entry point for the DECwindows Banner program. This module contains
**	all the initialiosation code for the Banner program, and the Main 
**	processing loop associated with DECwindows.
**
**  ABSTRACT:
**
**      [@tbs@]
**
**  AUTHORS:
**
**      [@tbs@]
**
**
**  CREATION DATE:     9th June 1988
**
**  MODIFICATION HISTORY:
**
**  Seperate the code that is init time only, from that is used at each sample
**  time.
**--
**/


/*
**
**  INCLUDE FILES
**
**/



#include "stdio.h"
#include "lnmdef.h"
#include "syidef.h"
#include <Mrm/MrmAppl.h>
#include <Xm/Shell.h>
#include <Xm/Protocols.h>
#include <Xt/vendor.h>

#include "Banner"


/*
**
**  MACRO DEFINITIONS
**
**/


/* Following are the forward, and external routine declarations used by
** this module */



extern void  BannerReadResource();
extern void  BannerSetDefaultResource();
extern void  BannerSoundInitialize();

extern void  BannerMenuCallback();
extern void  BannerSoundCallback();
extern void  BannerSoundInitCallback();
extern void  BannerSetupCallback();
extern void  BannerToggleChangeCallback();
extern void  BannerToggleInitCallback();

/* 
 * Definitions of other static items we use throughout the banner program
 * to reduce our setup times. 
 */

int	    BannerInitialPriority;
char	    BannerVmsNodeName[30];
char	    BannerVmsVersion[30];
char	    *BannerVmsDisplayName = NULL;
Bnr$_Blk    Bnr;
Bnr$_Blk    LBnr;
Clk$_Blk    Clk;
Lks$_Blk    Lks;
Lck$_Blk    Lck;
Sys$_Blk    Sys;
pem$_Blk    pem;
pes$_Blk    pes;
cwps$_Blk    cwps;
cwps$_Blk    cwpsold;
Mon$_Blk    Mon;
Link$_Blk    Link;
Clu$_Blk    Clu;
Qb$_Blk     Qb;
Qp$_Blk     Qp;
Tp$_Blk     Tp;
XWMHints    *wmhints;
GC	    BannerGC;
XGCValues   GcValues;
XEvent	     BannerEvent;
Widget menu_widget = 0;
char	     *BannerDayList[] = {
			    "Wednesday",
			    "Thursday",
			    "Friday",
			    "Saturday",
			    "Sunday",
			    "Monday",
			    "Tuesday",
			};

/* 
 * The names and address for the routine mappings we are going to 
 * Pass to the DRM processor
 */
static MrmRegisterArg namevec[] =
    {
	{"BANNER_SOUND_CALLBACK", (caddr_t)BannerSoundCallback},
	{"BANNER_SOUND_INIT_CALLBACK", (caddr_t)BannerSoundInitCallback},
	{"BANNER_MENU_CALLBACK", (caddr_t)BannerMenuCallback},
	{"BANNER_SETUP_CALLBACK", (caddr_t)BannerSetupCallback},
	{"TOGGLE_INIT_CALLBACK", (caddr_t)BannerToggleInitCallback},
	{"TOGGLE_CHANGE_CALLBACK", (caddr_t)BannerToggleChangeCallback}
    };

static MrmType *dummy_class;
static int     BannerProcessName[2] = {11, "DECW$BANNER"};
/*
 * The file specification to locate our UIL file via.
 */

static char *drm_filename_vec[] =
{
    "DECW$BANNER"
};   

typedef struct _vms_item_list {
	short	 buffer_length;
	short	 item_code;
	void	*buffer;
	int	*returned_buffer_length;
	} VMS_ItemList;

#define fill_width 17
#define fill_height 17
static char fill_bits[] = {
   0xcc, 0xcc, 0x00, 0x99, 0x99, 0x01, 0x33, 0x33, 0x01, 0x66, 0x66, 0x00,
   0xcc, 0xcc, 0x00, 0x99, 0x99, 0x01, 0x33, 0x33, 0x01, 0x66, 0x66, 0x00,
   0xcc, 0xcc, 0x00, 0x99, 0x99, 0x01, 0x33, 0x33, 0x01, 0x66, 0x66, 0x00,
   0xcc, 0xcc, 0x00, 0x99, 0x99, 0x01, 0x33, 0x33, 0x01, 0x66, 0x66, 0x00,
   0xcc, 0xcc, 0x00};

#define bell_width 17
#define bell_height 17
static char bell_bits[] = {
   0x80, 0x03, 0x00, 0x80, 0x03, 0x00, 0xe0, 0x0f, 0x00, 0x10, 0x10, 0x00,
   0x08, 0x20, 0x00, 0x08, 0x20, 0x00, 0x08, 0x20, 0x00, 0x08, 0x20, 0x00,
   0x08, 0x20, 0x00, 0x08, 0x20, 0x00, 0x08, 0x20, 0x00, 0x04, 0x40, 0x00,
   0x02, 0x80, 0x00, 0xff, 0xff, 0x01, 0x00, 0x01, 0x00, 0x00, 0x06, 0x00,
   0x00, 0x06, 0x00};

#define BannerIconWidth 32
#define BannerIconHeight 32
#define SmBannerIconWidth 17
#define SmBannerIconHeight 17

static char BannerIconBits[] = {
   0x06, 0x00, 0x00, 0x00, 0x0f, 0x7e, 0x00, 0x00, 0x8f, 0x81, 0x01, 0x00,
   0x66, 0x00, 0x06, 0xe0, 0x1e, 0x00, 0x18, 0x98, 0x06, 0x80, 0xe0, 0x87,
   0x46, 0x80, 0x00, 0x80, 0x46, 0x80, 0x01, 0x80, 0xc6, 0x80, 0x03, 0x80,
   0xc6, 0xc0, 0x83, 0x80, 0xc6, 0xc0, 0x83, 0x81, 0xc6, 0xc9, 0x87, 0x81,
   0xc6, 0xd9, 0x8f, 0x83, 0xc6, 0xdb, 0x8f, 0x87, 0xc6, 0xdf, 0xbf, 0x87,
   0xc6, 0xff, 0xff, 0x8f, 0xc6, 0xff, 0xff, 0x8f, 0xc6, 0xff, 0xff, 0x8f,
   0xc6, 0xff, 0xff, 0x8f, 0xc6, 0xff, 0xff, 0x8f, 0x06, 0x00, 0x00, 0x80,
   0x06, 0x00, 0x00, 0x80, 0x06, 0x7e, 0x00, 0x80, 0x86, 0x81, 0x01, 0x80,
   0x66, 0x00, 0x06, 0xe0, 0x1e, 0x00, 0x18, 0x18, 0x06, 0x00, 0xe0, 0x07,
   0x06, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00,
   0x06, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00
};

static char SmBannerIconBits[] = {
   0x07, 0x00, 0x00, 0xc7, 0x01, 0x00, 0x32, 0xe6, 0x01, 0x0e, 0x18, 0x01,
   0x12, 0x02, 0x01, 0x32, 0x06, 0x01, 0xb2, 0x16, 0x01, 0xb2, 0x16, 0x01,
   0xf2, 0x3f, 0x01, 0xf2, 0x3f, 0x01, 0x02, 0x00, 0x01, 0xc2, 0x01, 0x01,
   0x32, 0xe6, 0x01, 0x0e, 0x18, 0x00, 0x02, 0x00, 0x00, 0x02, 0x00, 0x00,
   0x02, 0x00, 0x00
};

Pixmap BannerPixmap, SmBannerPixmap, BannerFillPixmap, BannerBellPixmap;

char banner_font_name [50], banner_font_size [10];
char banner_font_wieght [50], banner_font_type [10];

static int main_used = 0;

/* 
 * The forward routine declarations 
 */

static void Main();
static void BannerCreateWindow();
static void BannerPlaceWindow();

static Pixmap MakePixmap();
       void BannerLoadFonts();
       void BannerSeperateWindows();
       void BannerOneWindow();
       void BannerSignalError();

/* The functions */

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	Main (argc, argv)
**
**
**      The main entry point for banner. This function performs the one time
**	setup, and then passes control to the event processing routines.
**
**  FORMAL PARAMETERS:
**
**      none
**
**  IMPLICIT INPUTS:
**
**      none
**
**  IMPLICIT OUTPUTS:
**
**      none
**
**  COMPLETION CODES:
**
**      none
**
**  SIDE EFFECTS:
**
**      none
**
**--
**/
main(argc, argv)
{
	int take_focus_atom;
	int class;
 	Arg	al[14];
 	int	ac = 0;
	int x, y;
        XColor screen_def,exact_def;
	int   colourmap;
	int node_len, version_len;
	char    nodedb[100];
	Widget	message;
	int	fid;
	int	prvs[2] = {-1, -1};
	char	bannername[512];
	VMS_ItemList sysitem[3] = {
	    8, SYI$_VERSION, &BannerVmsVersion, &version_len,
	    15, SYI$_NODENAME, &BannerVmsNodeName, &node_len, 0, 0, 0, 0}; 
/*
 *   Now perform all the toolkit and DRM initialisation, so we can get 
 *   access to our UI, and the X workstation.
 */
    sys$setprn (
        &BannerProcessName);
    sys$setprv (
        1, 
        &prvs, 
        0, 
        0);
    memset( &Bnr, 0, sizeof(Bnr));
    memset( &Link, 0, sizeof(Link));
    memset( &Mon, 0, sizeof(Mon));
    memset( &Clk, 0, sizeof(Clk));
    memset( &Lks, 0, sizeof(Lks));
    memset( &Lck, 0, sizeof(Lck));
    memset( &Sys, 0, sizeof(Sys));
    memset( &pes, 0, sizeof(pes));
    memset( &pem, 0, sizeof(pem));
    memset( &Tp, 0, sizeof(Tp));
    memset( &Qb, 0, sizeof(Qb));
    memset( &Qp, 0, sizeof(Qp));
    memset( &Clu, 0, sizeof(Clu));

    memset (&BannerVmsVersion, 0, sizeof (BannerVmsVersion));
    memset (&BannerVmsNodeName, 0, sizeof (BannerVmsNodeName));
/*
 * Find out our VMS node name, and Version for the display code!
 */


	SYS$GETSYI (NULL, NULL, NULL, &sysitem, NULL, NULL, NULL);
        if (node_len == 0 || strncmp (BannerVmsNodeName, 
		"  ", 2) == 0)
	    {
	    VMS_ItemList lnmitem[2] = {
		15, LNM$_STRING, &BannerVmsNodeName, &node_len, 0, 0, 0, 0}; 
	    char table_name_str[] = "LNM$FILE_DEV";
	    int  table_name[2] = {12, &table_name_str};
	    char sys_node_str[] = "SYS$NODE";
	    int  sys_node_name[2] = {8, &sys_node_str};
	    /*
	     * Then read it from sys$node!
	     */
	    SYS$TRNLNM (NULL, &table_name, &sys_node_name, NULL, 
		&lnmitem);
	    /*
	     * take off the trailing ::
	     */
	    while (BannerVmsNodeName[strlen(BannerVmsNodeName)-1] == ':')
		BannerVmsNodeName[strlen(BannerVmsNodeName)-1] = 0;
	    }

/* 
 * Now initialise decwindows
 */

    MrmInitialize ();			    /* Initialise DRM/UIL for our UI */
    DXmInitialize ();    
    
/*
 *>>>Mapping unknown: DwtSetDECApplication ("DECW$BANNER");
 */

    sprintf(bannername, "DECW$BANNER_%s", BannerVmsNodeName);

    Bnr.dp_id = XtInitialize (
			    bannername,		    /* Default title bar */
			    bannername,		    /* Our widget class */
			    NULL,		    /* No option list */
			    0,			    /* Number of options */
			    &argc,
			    argv);

    XrmInitialize ();

/* 
 * Set up the Banner default resources, and then read in our resource 
 * file, and initialise our in memory resource flags.
 */

    BannerSoundInitialize ();
    BannerSetDefaultResource (); 
    BannerReadResource (0);	/* Private database */

/*
 * Now set some of the default attributes our oru top level widget, 
 * including the X,Y coordinates, our title bar etc.
 */

    BannerSetArg (Bnr.dp_id, XmNallowShellResize, 1);
    BannerSetArg (Bnr.dp_id, XmNallowResize, 1);
    BannerSetArg (Bnr.dp_id, XmNnoResize, 0); 
    BannerSetArg (Bnr.dp_id, XmNresize, 1);
    BannerSetArg (Bnr.dp_id, XmNx, Bnr.clock_x); 
    BannerSetArg (Bnr.dp_id, XmNy, Bnr.clock_y); 
    BannerSetArg (Bnr.dp_id, XmNtitle, Bnr.title); 

/*
 * If we dont want a border redirect the top level shell, so window manager
 * wont give us all the nice window border features.
 */

    if (! Bnr.Param_Bits.border)
	{
	BannerSetArg (Bnr.dp_id, XmNoverrideRedirect, 1); 
	}

    if (! Bnr.Param_Bits.window_border)
	BannerSetArg (Bnr.dp_id, XmNborderWidth, 0); 
    else
	BannerSetArg (Bnr.dp_id, XmNborderWidth, 1); 
/*
 * Open up our UID file in SYS$LIBRARY, so we can get the DRM manager
 * to build our UI for us.
 */

    if (MrmOpenHierarchy (
			    1,			/* Number of files */
			    drm_filename_vec,	/* Filename Array */
			    NULL,		/* Default OS extension */
			    &Bnr.drm_prt
			    ) != MrmSUCCESS)
		BannerSignalError ("can't open hierarchy");


/*
 * Now define our callback routines to the DRM manager
 */

    MrmRegisterNames (
	    namevec,				/* Ascii names */
	    XtNumber (namevec)
	    );


/*
 * Get the desired font loaded.
 */

    BannerLoadFonts ();


    XtSetArg(al[ac], XmNx, Bnr.clock_x); ac++;
    XtSetArg(al[ac], XmNy, Bnr.clock_y); ac++;
    XtSetArg(al[ac], XmNwidth, 20*Bnr.font_width); ac++;
    XtSetArg(al[ac], XmNheight, 3*Bnr.font_height); ac++;

    Bnr.main_widget = XmCreateDrawingArea(Bnr.dp_id,
		    "banner",
		    al, ac);

    XtManageChild (Bnr.main_widget);   /* Get it managed */

    BannerFillPixmap = XCreateBitmapFromData (XtDisplay(Bnr.dp_id), 
		XDefaultRootWindow (XtDisplay(Bnr.dp_id)), fill_bits, 
		fill_width, fill_height);

    BannerBellPixmap = XCreateBitmapFromData (XtDisplay(Bnr.dp_id), 
		XDefaultRootWindow (XtDisplay(Bnr.dp_id)), bell_bits, 
		bell_width, bell_height);


    BannerPixmap = MakePixmap (XtDisplay(Bnr.dp_id), 
		XDefaultRootWindow (XtDisplay(Bnr.dp_id)),
		BannerIconBits, BannerIconWidth, BannerIconHeight);

    SmBannerPixmap = MakePixmap (XtDisplay(Bnr.dp_id), 
		XDefaultRootWindow (XtDisplay(Bnr.dp_id)),
		SmBannerIconBits, SmBannerIconWidth, SmBannerIconHeight);


    BannerSetArg (Bnr.dp_id, XtNiconPixmap, BannerPixmap);
/*    BannerSetArg (Bnr.dp_id, XtNiconifyPixmap, SmBannerPixmap);*/


    XtRealizeWidget (Bnr.dp_id);

    BannerCreateCursors (Bnr.dp_id);


    XSelectInput (XtDisplay(Bnr.main_widget), XtWindow(Bnr.main_widget), 
		VisibilityChangeMask |
		ExposureMask |
		StructureNotifyMask |
		SubstructureNotifyMask |
		PropertyChangeMask |
		ButtonPressMask |
		ButtonReleaseMask |
		NoEventMask);

    BannerGetArg (Bnr.main_widget, XmNforeground, &Bnr.foreground);
    BannerGetArg (Bnr.main_widget, XmNbackground, &Bnr.background);

/*
 * Get any colours we need
 */

    if (Bnr.hwmrk_colour != 0)
	{
	XColor color1, color2;
	colourmap = XDefaultColormap (XtDisplay(Bnr.main_widget), 
	    XDefaultScreen(XtDisplay(Bnr.main_widget)));
	XAllocNamedColor(XtDisplay(Bnr.main_widget), colourmap, Bnr.hwmrk_colour,
	    &color1, &color2);
	Bnr.hwmrk = color1.pixel;
	}

    if (Bnr.hwmrk == 0)
	Bnr.hwmrk = Bnr.foreground;

/*
 * now allocate the popup menu
 */
    MrmFetchWidget (Bnr.drm_prt,
	"BANNER_MENU",
	Bnr.main_widget,
	&menu_widget,
	&class);

/*
 * Now create all other windows
 */

    BannerCreateWindow(Bnr.clock_x,
	Bnr.clock_y,
	Bnr.clock_width,
	Bnr.clock_height,
	"Clock",
	&Bnr.clock_shell_widget,
	&Bnr.clock_widget,
	(Bnr.Param_Bits.analog_clock || Bnr.Param_Bits.digital_clock));

/*
 * the window for the Cube
 */

	
    BannerCreateWindow(Bnr.globe_x,
	Bnr.globe_y,
	Bnr.globe_width,
	Bnr.globe_height,
	"Toy",
	&Bnr.globe_shell_widget,
	&Bnr.globe_widget,
	(Bnr.Param_Bits.world));

	
    BannerCreateWindow(Bnr.hanoi_x,
	Bnr.hanoi_y,
	Bnr.hanoi_width,
	Bnr.hanoi_height,
	"Toy",
	&Bnr.hanoi_shell_widget,
	&Bnr.hanoi_widget,
	(Bnr.Param_Bits.hanoi));

	
    BannerCreateWindow(Bnr.cube_x,
	Bnr.cube_y,
	Bnr.cube_width,
	Bnr.cube_height,
	"Toy",
	&Bnr.cube_shell_widget,
	&Bnr.cube_widget,
	(Bnr.Param_Bits.cube));
/*
 * the window for the cpu
 */

    BannerCreateWindow(Bnr.cpu_x,
	Bnr.cpu_y,
	Bnr.cpu_width,
	Bnr.cpu_height,
	"Cpu",
	&Bnr.cpu_shell_widget,
	&Bnr.cpu_widget,
	(Bnr.Param_Bits.cpu_window));

/*
 * the window for the monitor
 */
	
    BannerCreateWindow(Bnr.mon_x,
	Bnr.mon_y,
	Bnr.mon_width,
	Bnr.mon_height,
	"Page IO",
	&Bnr.mon_shell_widget,
	&Bnr.mon_widget,
	(Bnr.Param_Bits.monitor));

    BannerCreateWindow(Bnr.link_x,
	Bnr.link_y,
	Bnr.link_width,
	Bnr.link_height,
	"Network Links",
	&Bnr.link_shell_widget,
	&Bnr.link_widget,
	(Bnr.Param_Bits.link_window));

/*
 * the window for the process monitor
 */
	
    BannerCreateWindow(Bnr.process_x,
	Bnr.process_y,
	Bnr.process_width,
	Bnr.process_height,
	"Processes",
	&Bnr.process_shell_widget,
	&Bnr.process_widget,
	(Bnr.Param_Bits.process_window));

/*
 * the window for the pe
 */

    BannerCreateWindow(Bnr.pe_x,
	Bnr.pe_y,
	Bnr.pe_width,
	Bnr.pe_height,
	"PE",
	&Bnr.pe_shell_widget,
	&Bnr.pe_widget,
	(Bnr.Param_Bits.pe_window));

/*
 * the window for the Locks
 */

    BannerCreateWindow(Bnr.lck_x,
	Bnr.lck_y,
	Bnr.lck_width,
	Bnr.lck_height,
	"Locks",
	&Bnr.lck_shell_widget,
	&Bnr.lck_widget,
	(Bnr.Param_Bits.lck_window));
/*
 * the window for the CWPS
 */

    BannerCreateWindow(Bnr.cwps_x,
	Bnr.cwps_y,
	Bnr.cwps_width,
	Bnr.cwps_height,
	"CWPS",
	&Bnr.cwps_shell_widget,
	&Bnr.cwps_widget,
	(Bnr.Param_Bits.cwps_window));

/*
 * the window for the sys display
 */

    BannerCreateWindow(Bnr.sys_x,
	Bnr.sys_y,
	Bnr.sys_width,
	Bnr.sys_height,
	"System",
	&Bnr.sys_shell_widget,
	&Bnr.sys_widget,
	(Bnr.Param_Bits.sys_window));

/*
 * the window for the tp display
 */

    BannerCreateWindow(Bnr.tp_x,
	Bnr.tp_y,
	Bnr.tp_width,
	Bnr.tp_height,
	"Trans Process",
	&Bnr.tp_shell_widget,
	&Bnr.tp_widget,
	(Bnr.Param_Bits.tp_window));

/*
 * the window for the qp display
 */

    BannerCreateWindow(Bnr.qp_x,
	Bnr.qp_y,
	Bnr.qp_width,
	Bnr.qp_height,
	"Print Queue",
	&Bnr.qp_shell_widget,
	&Bnr.qp_widget,
	(Bnr.Param_Bits.qp_window));

/*
 * the window for the qb display
 */

    BannerCreateWindow(Bnr.qb_x,
	Bnr.qb_y,
	Bnr.qb_width,
	Bnr.qb_height,
	"Batch Queue",
	&Bnr.qb_shell_widget,
	&Bnr.qb_widget,
	(Bnr.Param_Bits.qb_window));

/*
 * the window for the clu display
 */

    BannerCreateWindow(Bnr.clu_x,
	Bnr.clu_y,
	Bnr.clu_width,
	Bnr.clu_height,
	"Cluster",
	&Bnr.clu_shell_widget,
	&Bnr.clu_widget,
	(Bnr.Param_Bits.clu_window));


/*
 * Now create the message window, must always be last!
 */
	Bnr.message_shell_widget = XtCreatePopupShell ("Banner\nMessage",
			XtClass(Bnr.dp_id),
			Bnr.dp_id,
			NULL,
			0);

	BannerSetArg (Bnr.message_shell_widget, XmNx, Bnr.message_x); 
	BannerSetArg (Bnr.message_shell_widget, XmNy, Bnr.message_y); 

	BannerSetArg (Bnr.message_shell_widget, XtNiconPixmap, BannerPixmap);
/*	    BannerSetArg (Bnr.message_shell_widget, XtNiconifyPixmap, SmBannerPixmap);*/


	BannerSetArg (Bnr.message_shell_widget, XmNnoResize, 0); 
	BannerSetArg (Bnr.message_shell_widget, XtNallowShellResize, 1);
	BannerSetArg (Bnr.message_shell_widget, XtNresize, 1);


	MrmFetchWidget (Bnr.drm_prt,
	    "BANNER_MESSAGE_WIDGET",
	    Bnr.message_shell_widget,
	    &Bnr.message_widget,
	    &class);


	BannerSetArg (Bnr.message_widget, XmNforeground, Bnr.foreground);
	BannerSetArg (Bnr.message_widget, XmNbackground, Bnr.background);
    
	BannerSetArg (Bnr.message_widget, 
	    XmNwidth, Bnr.message_width);
	BannerSetArg (Bnr.message_widget, 
	    XmNheight, Bnr.message_height);

	if (! Bnr.Param_Bits.window_border)
	    BannerSetArg (Bnr.message_shell_widget, XmNborderWidth, 0); 
	else
	    BannerSetArg (Bnr.message_shell_widget, XmNborderWidth, 1); 
    
 	XtManageChild (Bnr.message_widget);
/*
 * Stop this anoying habit of taking Focus
 */

	take_focus_atom = XmInternAtom (
	    XtDisplay (Bnr.message_shell_widget), "WM_TAKE_FOCUS", FALSE);

/* Tell the window manager which ICCCM protocols we dont want supported. */

	XmDeactivateWMProtocol (
	    Bnr.message_shell_widget,           /* shell widget */
	    take_focus_atom);                      /* atom array */

	BannerSetArg (Bnr.message_shell_widget, XmNinput, 0); 


	XtRealizeWidget (Bnr.message_shell_widget);
/*
 * A nice firendly hack to get the popup menu working!
 */
	XGrabButton (XtDisplay(Bnr.message_widget), 
		     2,
		     AnyModifier,
		     XtWindow(Bnr.message_widget), 
		     TRUE,
		     NULL,
		     GrabModeAsync,
		     GrabModeAsync,
		     None,
		     None);	

    BannerSeperateWindows (0);

/*
 * See if we should fetch the startup message
 */

    sprintf (nodedb, "DECW$USER_DEFAULTS:DECW$BANNER_%s.DAT",
	BannerVmsNodeName);

    if ((fid = fopen(nodedb, "r")) == 0)
	{
	MrmFetchWidget (Bnr.drm_prt,
	    "BANNER_NEW_MESSAGE",
	    Bnr.dp_id,
	    &message,
	    &class);

	if (!Bnr.Param_Bits.border)
	    BannerSetArg (message->core.parent, 
		XmNoverrideRedirect, 1); 

	XtManageChild (message);
	XtRealizeWidget (message);
	}
    else
	fclose(fid);
/*
 * All initialisation has been done, now we are ready to fall into main loop,
 * and let the AST we setup to the rest of the work.
 */


    BannerMainLoop ();

/* We will never come out of this. */
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	BannerCreateWindow ()
**
**      This routine create a Banner root window
**
**  FORMAL PARAMETERS:
**
**      [@description_or_none@]
**
**  IMPLICIT INPUTS:
**
**      [@description_or_none@]
**
**  IMPLICIT OUTPUTS:
**
**      [@description_or_none@]
**
**  {@function_value_or_completion_codes@}
**
**      [@description_or_none@]
**
**  SIDE EFFECTS:
**
**      [@description_or_none@]
**
**--
**/
static int border = -1;
static int icon = -1;

void	
BannerCreateWindow(x,y,width,height,name,shell,widget, pop)
int x, y, width, height, pop;
char *name;
Widget *shell, *widget;
{
 	Arg	al[14];
 	int	ac = 0;
	int	class;

/*
 * Save this parameters, attempts to change it after windows have 
 * been created, will blow the window manager away
 */
    if (border == -1)
	{
	border = Bnr.Param_Bits.border;
	icon = Bnr.Param_Bits.icon_on;
	}
/*
 * We need a shell, so create it, and the window for this
 * display
 */

/*
 * Set the attributes for the images of the window, with or without 
 * title bars, with our without icon buttons, borders etc.
 */
    ac = 0;

    XtSetArg(al[ac], XmNx, x); ac++;
    XtSetArg(al[ac], XmNy, y); ac++;

    if (! border)
	{
	XtSetArg(al[ac], XmNoverrideRedirect, 1); ac++;
	}

    if (! Bnr.Param_Bits.window_border)
	{XtSetArg(al[ac], XmNborderWidth, 0); ac++;}
    else
	{XtSetArg(al[ac], XmNborderWidth, 1); ac++;}

    if (! icon && border)
	{
	XtSetArg(al[ac], XmNtransient, 1); ac++;
	}
    else
	{
	XtSetArg(al[ac], XmNtransient, 0); ac++;
	}

    XtSetArg(al[ac], XmNallowShellResize, 1); ac++;
    XtSetArg(al[ac], XmNresize, 1); ac++;
    XtSetArg(al[ac], XmNnoResize, 0); ac++;
    XtSetArg(al[ac], XmNallowResize, 1); ac++;


    *shell = XtCreatePopupShell (name,
		topLevelShellWidgetClass,
		Bnr.dp_id,
		al,
		ac);

/*
    *shell = XtAppCreateShell (name, "DECW$BANNER", 
	applicationShellWidgetClass,
	    XtDisplay(Bnr.dp_id),
	    al, ac);
*/

    ac = 0; 
    XtSetArg(al[ac], XmNx, x); ac++;
    XtSetArg(al[ac], XmNy, y); ac++;
    XtSetArg(al[ac], XmNwidth, width); ac++;
    XtSetArg(al[ac], XmNheight, height); ac++;

    XtSetArg(al[ac], XmNallowShellResize, 1); ac++;
    XtSetArg(al[ac], XmNresize, 1); ac++;
    XtSetArg(al[ac], XmNnoResize, 0); ac++;
    XtSetArg(al[ac], XmNallowResize, 1); ac++;


    *widget = XmCreateDrawingArea(*shell,
		    name,
		    al, ac);

    XtManageChild (*widget);

    BannerSetArg (*widget, XmNforeground, Bnr.foreground);
    BannerSetArg (*widget, XmNbackground, Bnr.background);

    BannerSetArg (*widget, XmNresize, 1);
    BannerSetArg (*widget, XmNresizePolicy, XmRESIZE_ANY);

    XtRealizeWidget (*shell);

/*
 * Set what events we are intersted in for this window
 */
    XSelectInput (XtDisplay(*shell), XtWindow(*shell), 
	VisibilityChangeMask |
	ExposureMask |
	StructureNotifyMask |
	SubstructureNotifyMask |
	PropertyChangeMask |
	ButtonPressMask |
	ButtonReleaseMask |
	NoEventMask);

    XSelectInput (XtDisplay(*widget), XtWindow(*widget), 
	VisibilityChangeMask |
	ExposureMask |
	StructureNotifyMask |
	SubstructureNotifyMask |
	PropertyChangeMask |
	ButtonPressMask |
	ButtonReleaseMask |
	NoEventMask);

/*
 * A nice firendly hack to get the popup menu working!
 */
    XGrabButton (XtDisplay(*widget), 
		 2,
		 AnyModifier,
		 XtWindow(*widget), 
		 TRUE,
		 NULL,
		 GrabModeAsync,
		 GrabModeAsync,
		 None,
		 None);	
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	BannerPlaceWindow ()
**
**      This routine create a Banner root window
**
**  FORMAL PARAMETERS:
**
**      [@description_or_none@]
**
**  IMPLICIT INPUTS:
**
**      [@description_or_none@]
**
**  IMPLICIT OUTPUTS:
**
**      [@description_or_none@]
**
**  {@function_value_or_completion_codes@}
**
**      [@description_or_none@]
**
**  SIDE EFFECTS:
**
**      [@description_or_none@]
**
**--
**/

void	
BannerPlaceWindow(x,y,width,height,shell,widget)
int x, y, width, height;
Widget shell, widget;
{
 	Arg	al[14];
 	int	ac = 0;

 
    XtSetArg(al[ac], XmNx, x); ac++;
    XtSetArg(al[ac], XmNy, y); ac++;
    XtSetArg(al[ac], XmNwidth, width); ac++;
    XtSetArg(al[ac], XmNheight, height); ac++;

    XtSetValues (shell, al, ac);

    XLowerWindow (XtDisplay(Bnr.dp_id), XtWindow (widget));
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	BannerSeperateWindows (force)
**
**
**      This routine creates or destroys the needed/uneeded shell windows for
**	seperate window mode.
**
**  FORMAL PARAMETERS:
**
**      none
**
**  IMPLICIT INPUTS:
**
**      none
**
**  IMPLICIT OUTPUTS:
**
**      none
**
**  COMPLETION CODES:
**
**      none
**
**  SIDE EFFECTS:
**
**      none
**
**--
**/
void	
BannerSeperateWindows(force)
int force;
{
int window_count;
int onewindow = 0;
/*
 * check to see if no windows need changing?
 */

	if (force == 0 &&
	    Bnr.Param_Bits.message_window == LBnr.Param_Bits.message_window &&
	    Bnr.Param_Bits.monitor == LBnr.Param_Bits.monitor &&
	    Bnr.Param_Bits.link_window == LBnr.Param_Bits.link_window &&
	    Bnr.Param_Bits.process_window == LBnr.Param_Bits.process_window &&
	    Bnr.Param_Bits.pe_window == LBnr.Param_Bits.pe_window &&
	    Bnr.Param_Bits.lck_window == LBnr.Param_Bits.lck_window &&
	    Bnr.Param_Bits.cwps_window == LBnr.Param_Bits.cwps_window &&
	    Bnr.Param_Bits.sys_window == LBnr.Param_Bits.sys_window &&
	    Bnr.Param_Bits.cpu_window == LBnr.Param_Bits.cpu_window &&
	    Bnr.Param_Bits.tp_window == LBnr.Param_Bits.tp_window &&
	    Bnr.Param_Bits.qb_window == LBnr.Param_Bits.qb_window &&
	    Bnr.Param_Bits.qp_window == LBnr.Param_Bits.qp_window &&
	    Bnr.Param_Bits.clu_window == LBnr.Param_Bits.clu_window &&
	    (Bnr.Param_Bits.analog_clock || Bnr.Param_Bits.digital_clock) ==
	    (LBnr.Param_Bits.analog_clock || LBnr.Param_Bits.digital_clock) &&
	    Bnr.Param_Bits.world == LBnr.Param_Bits.world &&
	    Bnr.Param_Bits.hanoi == LBnr.Param_Bits.hanoi &&
	    Bnr.Param_Bits.cube == LBnr.Param_Bits.cube
	     )
	     return;

	memcpy(&LBnr, &Bnr, sizeof(Bnr));

/*
 * Now popdown all widgets that arnt active
 */
	if ((Bnr.Param_Bits.analog_clock || Bnr.Param_Bits.digital_clock))
	    {
	    onewindow = 1;
	    BannerPlaceWindow(Bnr.clock_x,
		Bnr.clock_y,
		Bnr.clock_width,
		Bnr.clock_height,
		Bnr.clock_shell_widget,
		Bnr.clock_widget);
	    XtMapWidget (Bnr.clock_shell_widget);
	    }
	else
	    XtUnmapWidget (Bnr.clock_shell_widget);

	if (Bnr.Param_Bits.pe_window)
	    {
	    onewindow = 1;
	    BannerPlaceWindow(Bnr.pe_x,
		Bnr.pe_y,
		Bnr.pe_width,
		Bnr.pe_height,
		Bnr.pe_shell_widget,
		Bnr.pe_widget);
	    XtMapWidget (Bnr.pe_shell_widget);
	    }
	else
	    XtUnmapWidget (Bnr.pe_shell_widget);

	if (Bnr.Param_Bits.cwps_window)
	    {
	    onewindow = 1;
	    BannerPlaceWindow(Bnr.cwps_x,
		Bnr.cwps_y,
		Bnr.cwps_width,
		Bnr.cwps_height,
		Bnr.cwps_shell_widget,
		Bnr.cwps_widget);
	    XtMapWidget (Bnr.cwps_shell_widget);
	    }
	else
	    XtUnmapWidget (Bnr.cwps_shell_widget);

	if (Bnr.Param_Bits.sys_window)
	    {
	    onewindow = 1;
	    BannerPlaceWindow(Bnr.sys_x,
		Bnr.sys_y,
		Bnr.sys_width,
		Bnr.sys_height,
		Bnr.sys_shell_widget,
		Bnr.sys_widget);
	    XtMapWidget (Bnr.sys_shell_widget);
	    }
	else
	    XtUnmapWidget (Bnr.sys_shell_widget);

	if (Bnr.Param_Bits.tp_window)
	    {
	    onewindow = 1;
	    BannerPlaceWindow(Bnr.tp_x,
		Bnr.tp_y,
		Bnr.tp_width,
		Bnr.tp_height,
		Bnr.tp_shell_widget,
		Bnr.tp_widget);
	    XtMapWidget (Bnr.tp_shell_widget);
	    }
	else
	    XtUnmapWidget (Bnr.tp_shell_widget);

	if (Bnr.Param_Bits.qp_window)
	    {
	    onewindow = 1;
	    BannerPlaceWindow(Bnr.qp_x,
		Bnr.qp_y,
		Bnr.qp_width,
		Bnr.qp_height,
		Bnr.qp_shell_widget,
		Bnr.qp_widget);
	    XtMapWidget (Bnr.qp_shell_widget);
	    }
	else
	    XtUnmapWidget (Bnr.qp_shell_widget);

	if (Bnr.Param_Bits.qb_window)
	    {
	    onewindow = 1;
	    BannerPlaceWindow(Bnr.qb_x,
		Bnr.qb_y,
		Bnr.qb_width,
		Bnr.qb_height,
		Bnr.qb_shell_widget,
		Bnr.qb_widget);
	    XtMapWidget (Bnr.qb_shell_widget);
	    }
	else
	    XtUnmapWidget (Bnr.qb_shell_widget);

	if (Bnr.Param_Bits.clu_window)
	    {
	    onewindow = 1;
	    BannerPlaceWindow(Bnr.clu_x,
		Bnr.clu_y,
		Bnr.clu_width,
		Bnr.clu_height,
		Bnr.clu_shell_widget,
		Bnr.clu_widget);
	    XtMapWidget (Bnr.clu_shell_widget);
	    }
	else
	    XtUnmapWidget (Bnr.clu_shell_widget);

	if (Bnr.Param_Bits.lck_window)
	    {
	    onewindow = 1;
	    BannerPlaceWindow(Bnr.lck_x,
		Bnr.lck_y,
		Bnr.lck_width,
		Bnr.lck_height,
		Bnr.lck_shell_widget,
		Bnr.lck_widget);
	    XtMapWidget (Bnr.lck_shell_widget);
	    }
	else
	    XtUnmapWidget (Bnr.lck_shell_widget);

	if (Bnr.Param_Bits.cube)
	    {
	    onewindow = 1;
	    BannerPlaceWindow(Bnr.cube_x,
		Bnr.cube_y,
		Bnr.cube_width,
		Bnr.cube_height,
		Bnr.cube_shell_widget,
		Bnr.cube_widget);
	    XtMapWidget (Bnr.cube_shell_widget);
	    }
	else
	    XtUnmapWidget (Bnr.cube_shell_widget);

	if (Bnr.Param_Bits.world)
	    {
	    onewindow = 1;
	    BannerPlaceWindow(Bnr.globe_x,
		Bnr.globe_y,
		Bnr.globe_width,
		Bnr.globe_height,
		Bnr.globe_shell_widget,
		Bnr.globe_widget);
	    XtMapWidget (Bnr.globe_shell_widget);
	    }
	else
	    XtUnmapWidget (Bnr.globe_shell_widget);

	if (Bnr.Param_Bits.hanoi)
	    {
	    onewindow = 1;
	    BannerPlaceWindow(Bnr.hanoi_x,
		Bnr.hanoi_y,
		Bnr.hanoi_width,
		Bnr.hanoi_height,
		Bnr.hanoi_shell_widget,
		Bnr.hanoi_widget);
	    XtMapWidget (Bnr.hanoi_shell_widget);
	    }
	else
	    XtUnmapWidget (Bnr.hanoi_shell_widget);

	if (Bnr.Param_Bits.cpu_window)
	    {
	    onewindow = 1;
	    BannerPlaceWindow(Bnr.cpu_x,
		Bnr.cpu_y,
		Bnr.cpu_width,
		Bnr.cpu_height,
		Bnr.cpu_shell_widget,
		Bnr.cpu_widget);
	    XtMapWidget (Bnr.cpu_shell_widget);
	    }
	else
	    XtUnmapWidget (Bnr.cpu_shell_widget);

	if (Bnr.Param_Bits.monitor)
	    {
	    onewindow = 1;
	    BannerPlaceWindow(Bnr.mon_x,
		Bnr.mon_y,
		Bnr.mon_width,
		Bnr.mon_height,
		Bnr.mon_shell_widget,
		Bnr.mon_widget);
	    XtMapWidget (Bnr.mon_shell_widget);
	    }
	else
	    XtUnmapWidget (Bnr.mon_shell_widget);

	if (Bnr.Param_Bits.link_window)
	    {
	    onewindow = 1;
	    BannerPlaceWindow(Bnr.link_x,
		Bnr.link_y,
		Bnr.link_width,
		Bnr.link_height,
		Bnr.link_shell_widget,
		Bnr.link_widget);
	    XtMapWidget (Bnr.link_shell_widget);
	    }
	else
	    XtUnmapWidget (Bnr.link_shell_widget);

	if (Bnr.Param_Bits.process_window)
	    {
	    onewindow = 1;
	    BannerPlaceWindow(Bnr.process_x,
		Bnr.process_y,
		Bnr.process_width,
		Bnr.process_height,
		Bnr.process_shell_widget,
		Bnr.process_widget);
	    XtMapWidget (Bnr.process_shell_widget);
	    }
	else
	    XtUnmapWidget (Bnr.process_shell_widget);

/*
 * Now if there has been noone to use the main window, put up the dummy window.
 */

    if (!onewindow || Bnr.Param_Bits.border)
	{
	BannerPlaceWindow(Bnr.clock_x,
	    Bnr.clock_y,
	    20*Bnr.font_width,
	    3*Bnr.font_height,
	    Bnr.dp_id,
	    Bnr.main_widget);
	XtMapWidget (Bnr.dp_id);
	}
    else
	XtUnmapWidget (Bnr.dp_id);

/*
 * Unmap the message window
 */
    BannerMessageHide ();
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	BannerLoadFonts ()
**
**      This routine tells the server to load a desired font, and inquires
**	some of the font information, and loads that into our data structures
**	for future text sizing.
**
**  FORMAL PARAMETERS:
**
**	none
**
**  IMPLICIT INPUTS:
**
**      none
**
**  IMPLICIT OUTPUTS:
**
**      none
**
**  COMPLETION CODES:
**
**      none
**
**  SIDE EFFECTS:
**
**      none
**
**--
**/
void	
BannerLoadFonts()
{
    int font;
    int character_width;
/*
 * Store the pointer to our font info structure, and tell the server to 
 * load our desired font.
 */

    font = 0;
    font = XLoadQueryFont (XtDisplay(Bnr.dp_id), Bnr.fnt_name); 
    if (font == 0)
	{
	printf("\n");
	printf(Bnr.fnt_name);
	printf("\n");
	if (Bnr.font_ptr == 0)	
	    BannerSignalError ("Failed to load requested font\n", 0);
	else
	    printf ("Failed to load requested font\n");
	}

    if (font != 0)
	Bnr.font_ptr = font;
/*
 * store font name, size for future use.
 */
/*
 * Now calculate the width and height of a standard character, from the info
 * in the font.
 */
    character_width = XTextWidth (Bnr.font_ptr, "DD-MMM-YYY", 10);
    Bnr.font_width = (character_width/10);
    Bnr.font_height = (Bnr.font_ptr->ascent + 
				    Bnr.font_ptr->descent);
}

/*
**++
**  FUNCTIONAL DESCRIPTION: 
**
**	BannerSignalError (problem_string)
**
**      A Generic routine to signal any fatal errors encountered during running 
**	the banner program.
**
**  FORMAL PARAMETERS:
**
**      problem_string		Text string to be displayed
**
**  IMPLICIT INPUTS:
**
**      none
**
**  IMPLICIT OUTPUTS:
**
**      none
**
**  FUNCTION VALUE:
**
**      none
**
**  SIDE EFFECTS:
**
**      The program is exited.
**
**--
**/
void	
BannerSignalError(problem_string, exit_status)
    char	*problem_string;
    int		exit_status;
{

/*
 * Set our priority back to what it should be
 */
    SYS$SETPRI (NULL, NULL, BannerInitialPriority, NULL);
    printf ("%s\n", problem_string);
    exit (exit_status);
}

/* routine to make a pixmap stolen from session mgr */
static Pixmap MakePixmap(dpy, root, data, width, height) 
Display *dpy;
Drawable root;
short *data;
Dimension width, height;
{
    Pixmap pid;

    pid = XCreatePixmapFromBitmapData (dpy, root, data,
		(Dimension) width, (Dimension) height, 
		(unsigned long) BlackPixel (dpy, 
			XDefaultScreen(XtDisplay(Bnr.main_widget))),
		(unsigned long) WhitePixel (dpy, 
			XDefaultScreen(XtDisplay(Bnr.main_widget))),
		(unsigned int) DefaultDepth (dpy, 
			XDefaultScreen(XtDisplay(Bnr.main_widget))));
    return(pid);
}
