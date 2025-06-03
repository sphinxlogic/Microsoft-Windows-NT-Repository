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
*3I
 **
 **  Seperate the code that is init time only, from that is used at each sample
 **  time.
*3E
 **--
 **/
 
 
 /*
 **
 **  INCLUDE FILES
 **
 **/
 
*9D
 #include <stdio.h>
 #include <lnmdef.h>
 #include <syidef.h>
 #include <decw$include/DECwDwtWidgetProg.h>
 #include <decw$include/vendor.h>
*9E
*9I
 
 
 #include "stdio.h"
 #include "lnmdef.h"
 #include "syidef.h"
*13D
 #include "MrmAppl.h"
 #include "vendor.h"
*13E
*13I
 #include <Mrm/MrmAppl.h>
 #include <Xm/Shell.h>
*34I
 #include <Xm/Protocols.h>
*34E
 #include <Xt/vendor.h>
*13E
 
*9E
 #include "Banner"
 
 
 /*
 **
 **  MACRO DEFINITIONS
 **
 **/
 
 
 /* Following are the forward, and external routine declarations used by
 ** this module */
 
 
 
*3I
 extern void  BannerReadResource();
 extern void  BannerSetDefaultResource();
*21I
 extern void  BannerSoundInitialize();
*21E
 
*3E
*9D
 extern VoidProc  BannerMenuCallback();
 extern VoidProc  BannerSetupCallback();
 extern VoidProc  BannerToggleChangeCallback();
 extern VoidProc  BannerToggleInitCallback();
*9E
*9I
 extern void  BannerMenuCallback();
*21I
 extern void  BannerSoundCallback();
 extern void  BannerSoundInitCallback();
*21E
 extern void  BannerSetupCallback();
 extern void  BannerToggleChangeCallback();
 extern void  BannerToggleInitCallback();
*9E
*3D
 extern VoidProc	 BannerAst ();
 
 extern void  BannerMenuPopup();
 extern void  BannerMenuPopupDistroy();
 extern void  BannerReadResource();
 extern void  BannerSetDefaultResource();
 extern void  BannerMonitor();
 extern void  BannerClockAst();
 extern void  BannerWriteDate();
 extern void  BannerClock();
 extern void  BannerHanoiSetup();
 extern void  BannerCubeSetup();
 extern void  BannerCube();
 extern void  BannerUpdateWindows ();
*3E
 
 /* 
  * Definitions of other static items we use throughout the banner program
  * to reduce our setup times. 
  */
 
 int	    BannerInitialPriority;
 char	    BannerVmsNodeName[30];
 char	    BannerVmsVersion[30];
*35I
 char	    *BannerVmsDisplayName = NULL;
*35E
 Bnr$_Blk    Bnr;
 Bnr$_Blk    LBnr;
 Clk$_Blk    Clk;
 Lks$_Blk    Lks;
 Lck$_Blk    Lck;
*4I
 Sys$_Blk    Sys;
*4E
 pem$_Blk    pem;
 pes$_Blk    pes;
 cwps$_Blk    cwps;
 cwps$_Blk    cwpsold;
 Mon$_Blk    Mon;
*10I
*24I
*30D
 Link$_Blk    Link;
*30E
*24E
*23A1I
 Link$_Blk    Link;
*23A1E
*31I
 Link$_Blk    Link;
*31E
 Clu$_Blk    Clu;
 Qb$_Blk     Qb;
 Qp$_Blk     Qp;
 Tp$_Blk     Tp;
*10E
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
*9D
 static DRMRegisterArg namevec[] =
*9E
*9I
 static MrmRegisterArg namevec[] =
*9E
     {
*21I
 	{"BANNER_SOUND_CALLBACK", (caddr_t)BannerSoundCallback},
 	{"BANNER_SOUND_INIT_CALLBACK", (caddr_t)BannerSoundInitCallback},
*21E
 	{"BANNER_MENU_CALLBACK", (caddr_t)BannerMenuCallback},
 	{"BANNER_SETUP_CALLBACK", (caddr_t)BannerSetupCallback},
 	{"TOGGLE_INIT_CALLBACK", (caddr_t)BannerToggleInitCallback},
 	{"TOGGLE_CHANGE_CALLBACK", (caddr_t)BannerToggleChangeCallback}
     };
 
*9D
 static DRMType *dummy_class;
*9E
*9I
 static MrmType *dummy_class;
*9E
*27D
*23A1D
 
*23A1E
*23A1I
 static int     BannerProcessName[2] = {11, "DECW$BANNER"};
*23A1E
*27E
*27I
*30D
 static int     BannerProcessName[2] = {11, "DECW$BANNER"};
*30E
*30I
*31D
 
*31E
*31I
 static int     BannerProcessName[2] = {11, "DECW$BANNER"};
*31E
*30E
*27E
 /*
  * The file specification to locate our UIL file via.
  */
 
 static char *drm_filename_vec[] =
 {
*9D
     "SYS$LIBRARY:DECW$BANNER.UID"
*9E
*9I
     "DECW$BANNER"
*9E
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
*3D
 static void BannerMainLoop();
*3E
*12I
 static void BannerCreateWindow();
 static void BannerPlaceWindow();
 
*12E
 static Pixmap MakePixmap();
        void BannerLoadFonts();
        void BannerSeperateWindows();
        void BannerOneWindow();
        void BannerSignalError();
*3D
        void BannerSetArg();
        void BannerGetArg();
*3E
 
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
*12D
 	int x, y;
*12E
*32I
 	int take_focus_atom;
*32E
 	int class;
*12I
  	Arg	al[14];
  	int	ac = 0;
 	int x, y;
*12E
         XColor screen_def,exact_def;
*5I
 	int   colourmap;
*5E
 	int node_len, version_len;
*15I
 	char    nodedb[100];
 	Widget	message;
 	int	fid;
*15E
*16I
*28I
*30D
 	int	prvs[2] = {-1, -1};
*30E
*28E
*23A1I
 	int	prvs[2] = {-1, -1};
*23A1E
*31I
 	int	prvs[2] = {-1, -1};
*31E
 	char	bannername[512];
*16E
 	VMS_ItemList sysitem[3] = {
 	    8, SYI$_VERSION, &BannerVmsVersion, &version_len,
 	    15, SYI$_NODENAME, &BannerVmsNodeName, &node_len, 0, 0, 0, 0}; 
 /*
  *   Now perform all the toolkit and DRM initialisation, so we can get 
  *   access to our UI, and the X workstation.
  */
*27I
*30D
     sys$setprn (
*30E
*28D
         BannerProcessName);
*28E
*27E
*28D
*23A1D
 
*23A1E
*23A1I
     sys$setprn (
         &BannerProcessName);
     sys$setprv (
         1, 
         &prvs, 
         0, 
         0);
*23A1E
*28E
*28I
*30D
         &BannerProcessName);
     sys$setprv (
         1, 
         &prvs, 
         0, 
         0);
*30E
*30I
*31D
 
*31E
*31I
     sys$setprn (
         &BannerProcessName);
     sys$setprv (
         1, 
         &prvs, 
         0, 
         0);
*31E
*30E
*28E
     memset( &Bnr, 0, sizeof(Bnr));
*24I
*30D
     memset( &Link, 0, sizeof(Link));
*30E
*24E
*23A1I
     memset( &Link, 0, sizeof(Link));
*23A1E
*31I
     memset( &Link, 0, sizeof(Link));
*31E
     memset( &Mon, 0, sizeof(Mon));
     memset( &Clk, 0, sizeof(Clk));
     memset( &Lks, 0, sizeof(Lks));
     memset( &Lck, 0, sizeof(Lck));
*4I
     memset( &Sys, 0, sizeof(Sys));
*4E
     memset( &pes, 0, sizeof(pes));
     memset( &pem, 0, sizeof(pem));
*10I
     memset( &Tp, 0, sizeof(Tp));
     memset( &Qb, 0, sizeof(Qb));
     memset( &Qp, 0, sizeof(Qp));
     memset( &Clu, 0, sizeof(Clu));
*10E
 
     memset (&BannerVmsVersion, 0, sizeof (BannerVmsVersion));
     memset (&BannerVmsNodeName, 0, sizeof (BannerVmsNodeName));
 /*
  * Find out our VMS node name, and Version for the display code!
  */
 
 
 	SYS$GETSYI (NULL, NULL, NULL, &sysitem, NULL, NULL, NULL);
*26D
*23A1D
 	if (node_len == 0)
*23A1E
*23A1I
         if (node_len == 0 || strncmp (BannerVmsNodeName, 
 		"  ", 2) == 0)
*23A1E
*26E
*26I
*30D
         if (node_len == 0 || strncmp (BannerVmsNodeName, 
*30E
*29D
 		"  ", 2))
*29E
*29I
*30D
 		"  ", 2) == 0)
*30E
*30I
*31D
 	if (node_len == 0)
*31E
*31I
         if (node_len == 0 || strncmp (BannerVmsNodeName, 
 		"  ", 2) == 0)
*31E
*30E
*29E
*26E
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
*29I
*30D
 	    /*
 	     * take off the trailing ::
 	     */
 	    while (BannerVmsNodeName[strlen(BannerVmsNodeName)-1] == ':')
 		BannerVmsNodeName[strlen(BannerVmsNodeName)-1] = 0;
*30E
*29E
*23A1I
 	    /*
 	     * take off the trailing ::
 	     */
 	    while (BannerVmsNodeName[strlen(BannerVmsNodeName)-1] == ':')
 		BannerVmsNodeName[strlen(BannerVmsNodeName)-1] = 0;
*23A1E
*31I
 	    /*
 	     * take off the trailing ::
 	     */
 	    while (BannerVmsNodeName[strlen(BannerVmsNodeName)-1] == ':')
 		BannerVmsNodeName[strlen(BannerVmsNodeName)-1] = 0;
*31E
 	    }
 
 /* 
  * Now initialise decwindows
  */
 
*9D
     DwtInitializeDRM ();			    /* Initialise DRM/UIL for our UI */
*9E
*9I
     MrmInitialize ();			    /* Initialise DRM/UIL for our UI */
*23D
     
*23E
*23I
     DXmInitialize ();    
*23E
*9E
     
*9D
     DwtSetDECApplication ("DECW$BANNER");
*9E
*9I
 /*
  *>>>Mapping unknown: DwtSetDECApplication ("DECW$BANNER");
  */
*9E
 
*16I
     sprintf(bannername, "DECW$BANNER_%s", BannerVmsNodeName);
 
*16E
     Bnr.dp_id = XtInitialize (
*16D
 			    "Banner",		    /* Default title bar */
 			    "DECW$BANNER",	    /* Our widget class */
*16E
*16I
 			    bannername,		    /* Default title bar */
 			    bannername,		    /* Our widget class */
*16E
 			    NULL,		    /* No option list */
 			    0,			    /* Number of options */
 			    &argc,
 			    argv);
 
     XrmInitialize ();
 
 /* 
  * Set up the Banner default resources, and then read in our resource 
  * file, and initialise our in memory resource flags.
  */
 
*22I
     BannerSoundInitialize ();
*22E
     BannerSetDefaultResource (); 
     BannerReadResource (0);	/* Private database */
 
*21I
*22D
     BannerSoundInitialize ();
*22E
*21E
 /*
  * Now set some of the default attributes our oru top level widget, 
  * including the X,Y coordinates, our title bar etc.
  */
 
*16D
     BannerSetArg (Bnr.dp_id, XtNallowShellResize, 1);
*16E
*9D
     BannerSetArg (Bnr.dp_id, DwtNx, 0); 
     BannerSetArg (Bnr.dp_id, DwtNy, 0); 
     BannerSetArg (Bnr.dp_id, DwtNtitle, Bnr.title); 
*9E
*9I
*16D
     BannerSetArg (Bnr.dp_id, XtNresize, 1);
*16E
*16I
     BannerSetArg (Bnr.dp_id, XmNallowShellResize, 1);
     BannerSetArg (Bnr.dp_id, XmNallowResize, 1);
     BannerSetArg (Bnr.dp_id, XmNnoResize, 0); 
     BannerSetArg (Bnr.dp_id, XmNresize, 1);
*16E
*15D
     BannerSetArg (Bnr.dp_id, XmNx, 0); 
     BannerSetArg (Bnr.dp_id, XmNy, 0); 
*15E
*15I
     BannerSetArg (Bnr.dp_id, XmNx, Bnr.clock_x); 
     BannerSetArg (Bnr.dp_id, XmNy, Bnr.clock_y); 
*15E
     BannerSetArg (Bnr.dp_id, XmNtitle, Bnr.title); 
*9E
 
 /*
  * If we dont want a border redirect the top level shell, so window manager
  * wont give us all the nice window border features.
  */
 
     if (! Bnr.Param_Bits.border)
 	{
*9D
 	BannerSetArg (Bnr.dp_id, DwtNoverrideRedirect, 1); 
*9E
*9I
 	BannerSetArg (Bnr.dp_id, XmNoverrideRedirect, 1); 
*9E
 	}
 
     if (! Bnr.Param_Bits.window_border)
*18D
 	{
*18E
*9D
 	BannerSetArg (Bnr.dp_id, DwtNborderWidth, 0); 
*9E
*9I
 	BannerSetArg (Bnr.dp_id, XmNborderWidth, 0); 
*9E
*18D
 	}
 
*18E
*18I
     else
 	BannerSetArg (Bnr.dp_id, XmNborderWidth, 1); 
*18E
 /*
  * Open up our UID file in SYS$LIBRARY, so we can get the DRM manager
  * to build our UI for us.
  */
 
*9D
     if (DwtOpenHierarchy (
*9E
*9I
     if (MrmOpenHierarchy (
*9E
 			    1,			/* Number of files */
 			    drm_filename_vec,	/* Filename Array */
 			    NULL,		/* Default OS extension */
 			    &Bnr.drm_prt
*9D
 			    ) != DRMSuccess)
*9E
*9I
 			    ) != MrmSUCCESS)
*9E
 		BannerSignalError ("can't open hierarchy");
 
 
 /*
  * Now define our callback routines to the DRM manager
  */
 
*9D
     DwtRegisterDRMNames (
*9E
*9I
     MrmRegisterNames (
*9E
 	    namevec,				/* Ascii names */
 	    XtNumber (namevec)
 	    );
 
 
 /*
  * Get the desired font loaded.
  */
 
     BannerLoadFonts ();
 
*12D
 /*
  * Now if we are doing multiple windows, create the other shells.
  */
 
     Bnr.clock_widget = Bnr.dp_id;
     Bnr.message_widget = Bnr.dp_id;
     Bnr.top_widget = Bnr.dp_id;
     Bnr.cube_widget = Bnr.dp_id;
*12E
*8I
*12D
     Bnr.globe_widget = Bnr.dp_id;
     Bnr.hanoi_widget = Bnr.dp_id;
*12E
*8E
*12D
     Bnr.cpu_widget = Bnr.dp_id;
     Bnr.mon_widget = Bnr.dp_id;
     Bnr.process_widget = Bnr.dp_id;
     Bnr.pe_widget = Bnr.dp_id;
     Bnr.lck_widget = Bnr.dp_id;
     Bnr.cwps_widget = Bnr.dp_id;
*12E
*4I
*12D
     Bnr.sys_widget = Bnr.dp_id;
*12E
*4E
*10I
*12D
     Bnr.clu_widget = Bnr.dp_id;
     Bnr.qb_widget = Bnr.dp_id;
     Bnr.qp_widget = Bnr.dp_id;
     Bnr.tp_widget = Bnr.dp_id;
*12E
*10E
*12D
 
 
*12E
*9D
     Bnr.main_widget = DwtWindow (
 	Bnr.dp_id,				/* Top level Widget id */
 	"Banner",				/* Title */
 	0,					/* X coordinates */
 	0,					/* Y coordinates */
 	20*Bnr.font_width,			/* Width */
 	3*Bnr.font_height,			/* Height */
 	NULL);
*9E
*9I
*12D
 /*
  *    Bnr.main_widget = XmWindow (
  *  	Bnr.dp_id,				
  *  	"Banner",				
  *  	0,					
  *  	0,					
  *  	20*Bnr.font_width,			
  *  	3*Bnr.font_height,			
  *  	NULL);
  */
 
  {
  	Arg	al[14];
  	int	ac = 0;
  
  	XtSetArg(al[ac], XmNx, 0); ac++;
  	XtSetArg(al[ac], XmNy, 0); ac++;
  	XtSetArg(al[ac], XmNwidth, 20*Bnr.font_width); ac++;
  	XtSetArg(al[ac], XmNheight, 3*Bnr.font_height); ac++;
 
   	Bnr.main_widget = XmCreateDrawingArea(Bnr.dp_id,
 			"banner",
   			al, ac);
 
  
  };
*12E
*12I
 
*14D
     XtSetArg(al[ac], XmNx, 0); ac++;
     XtSetArg(al[ac], XmNy, 0); ac++;
*14E
*14I
     XtSetArg(al[ac], XmNx, Bnr.clock_x); ac++;
     XtSetArg(al[ac], XmNy, Bnr.clock_y); ac++;
*14E
     XtSetArg(al[ac], XmNwidth, 20*Bnr.font_width); ac++;
     XtSetArg(al[ac], XmNheight, 3*Bnr.font_height); ac++;
 
     Bnr.main_widget = XmCreateDrawingArea(Bnr.dp_id,
 		    "banner",
 		    al, ac);
*12E
*9E
 
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
*9D
     BannerSetArg (Bnr.dp_id, XtNiconifyPixmap, SmBannerPixmap);
*9E
*9I
 /*    BannerSetArg (Bnr.dp_id, XtNiconifyPixmap, SmBannerPixmap);*/
*9E
 
 
     XtRealizeWidget (Bnr.dp_id);
 
*15I
     BannerCreateCursors (Bnr.dp_id);
 
 
*15E
     XSelectInput (XtDisplay(Bnr.main_widget), XtWindow(Bnr.main_widget), 
 		VisibilityChangeMask |
 		ExposureMask |
 		StructureNotifyMask |
 		SubstructureNotifyMask |
 		PropertyChangeMask |
 		ButtonPressMask |
 		ButtonReleaseMask |
 		NoEventMask);
 
*9D
     BannerGetArg (Bnr.main_widget, DwtNforeground, &Bnr.foreground);
     BannerGetArg (Bnr.main_widget, DwtNbackground, &Bnr.background);
*9E
*9I
     BannerGetArg (Bnr.main_widget, XmNforeground, &Bnr.foreground);
     BannerGetArg (Bnr.main_widget, XmNbackground, &Bnr.background);
*9E
 
*5I
 /*
  * Get any colours we need
  */
 
     if (Bnr.hwmrk_colour != 0)
 	{
 	XColor color1, color2;
*7D
 	colourmap = XDefaultColormap (XtDisplay(Bnr.main_widget), 0);
*7E
*7I
 	colourmap = XDefaultColormap (XtDisplay(Bnr.main_widget), 
 	    XDefaultScreen(XtDisplay(Bnr.main_widget)));
*7E
 	XAllocNamedColor(XtDisplay(Bnr.main_widget), colourmap, Bnr.hwmrk_colour,
 	    &color1, &color2);
 	Bnr.hwmrk = color1.pixel;
 	}
 
     if (Bnr.hwmrk == 0)
 	Bnr.hwmrk = Bnr.foreground;
*5E
 
 /*
  * now allocate the popup menu
  */
*9D
     DwtFetchWidget (Bnr.drm_prt,
*9E
*9I
     MrmFetchWidget (Bnr.drm_prt,
*9E
 	"BANNER_MENU",
 	Bnr.main_widget,
 	&menu_widget,
 	&class);
 
*12I
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
 	
*24D
*23A1D
     BannerCreateWindow(Bnr.mon_x,
 	Bnr.mon_y,
 	Bnr.mon_width,
 	Bnr.mon_height,
*23A1E
*24E
*24I
*30D
     BannerCreateWindow(Bnr.link_x,
 	Bnr.link_y,
 	Bnr.link_width,
 	Bnr.link_height,
*30E
*24E
*25D
*23A1D
 	"Page IO",
*23A1E
*25E
*25I
*30D
 	"Network Links",
*30E
*25E
*24D
*23A1D
 	&Bnr.mon_shell_widget,
 	&Bnr.mon_widget,
 	(Bnr.Param_Bits.monitor));
*23A1E
*23A1I
     BannerCreateWindow(Bnr.link_x,
 	Bnr.link_y,
 	Bnr.link_width,
 	Bnr.link_height,
 	"Network Links",
 	&Bnr.link_shell_widget,
 	&Bnr.link_widget,
 	(Bnr.Param_Bits.link_window));
*23A1E
*24E
*24I
*30D
 	&Bnr.link_shell_widget,
 	&Bnr.link_widget,
 	(Bnr.Param_Bits.link_window));
*30E
*30I
     BannerCreateWindow(Bnr.mon_x,
 	Bnr.mon_y,
 	Bnr.mon_width,
 	Bnr.mon_height,
 	"Page IO",
 	&Bnr.mon_shell_widget,
 	&Bnr.mon_widget,
 	(Bnr.Param_Bits.monitor));
*30E
*24E
 
*31I
     BannerCreateWindow(Bnr.link_x,
 	Bnr.link_y,
 	Bnr.link_width,
 	Bnr.link_height,
 	"Network Links",
 	&Bnr.link_shell_widget,
 	&Bnr.link_widget,
 	(Bnr.Param_Bits.link_window));
 
*31E
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
 
*16D
 	if (! Bnr.Param_Bits.border)
 	    BannerSetArg (Bnr.message_shell_widget, XmNoverrideRedirect, 1); 
 
*16E
 
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
*18D
 	    {
*18E
 	    BannerSetArg (Bnr.message_shell_widget, XmNborderWidth, 0); 
*18D
 	    }
*18E
*18I
 	else
 	    BannerSetArg (Bnr.message_shell_widget, XmNborderWidth, 1); 
*18E
     
*15D
 	XtManageChild (Bnr.message_widget);
*15E
*15I
  	XtManageChild (Bnr.message_widget);
*15E
*32I
 /*
  * Stop this anoying habit of taking Focus
  */
 
 	take_focus_atom = XmInternAtom (
 	    XtDisplay (Bnr.message_shell_widget), "WM_TAKE_FOCUS", FALSE);
 
 /* Tell the window manager which ICCCM protocols we dont want supported. */
 
*33D
 	XmDeactivateWMProtocols (
*33E
*33I
 	XmDeactivateWMProtocol (
*33E
 	    Bnr.message_shell_widget,           /* shell widget */
 	    take_focus_atom);                      /* atom array */
 
 	BannerSetArg (Bnr.message_shell_widget, XmNinput, 0); 
 
*32E
 
 	XtRealizeWidget (Bnr.message_shell_widget);
*13D
 
 	XtPopup (Bnr.message_shell_widget, XtGrabNone);
*13E
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
*12E
 
     BannerSeperateWindows (0);
*15I
 
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
*15E
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
*12D
 BannerCreateWindow(x,y,width,height,name,shell,widget)
 int x, y, width, height;
*12E
*12I
 BannerCreateWindow(x,y,width,height,name,shell,widget, pop)
 int x, y, width, height, pop;
*12E
 char *name;
 Widget *shell, *widget;
 {
*12I
  	Arg	al[14];
  	int	ac = 0;
*12E
*13I
 	int	class;
*13E
 
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
*12D
  * Know check if we have to create a shell, or if we can use the root
  * window
  */
     if (main_used)
 	{
 	/*
 	 * We need a shell, so create it, and the window for this
 	 * display
 	 */
 	*shell = XtCreatePopupShell (name,
 		    XtClass(Bnr.dp_id),
 		    Bnr.dp_id,
 		    NULL,
 		    0);
*12E
*9D
 	*widget = DwtWindow (
 	    *shell,		/* Top level Widget id */
 	    name,		/* Title */
 	    x,			/* X coordinates */
 	    y,			/* Y coordinates */
 	    width,		/* Width */
 	    height,		/* Height */
 	    NULL);
*9E
*9I
*12D
 
 /*
  *
  *	*widget = XmWindow (
  *  	    *shell,		
  *  	    name,
  *  	    x,			X coordinates 
  *  	    y,			Y coordinates 
  *  	    width,		Width 
  *  	    height,		Height 
  *  	    NULL);
  */
 
  {
  	Arg	al[14];
  	int	ac = 0;
  
  	XtSetArg(al[ac], XmNx, x); ac++;
  	XtSetArg(al[ac], XmNy, y); ac++;
  	XtSetArg(al[ac], XmNwidth, width); ac++;
  	XtSetArg(al[ac], XmNheight, height); ac++;
 
 
   	*widget = XmCreateDrawingArea(*shell,
   			name,
   			al, ac);
 
  };
*12E
*9E
*12D
 
 	XtManageChild (*widget);
*12E
*9D
 	BannerSetArg (*shell, DwtNx, x); 
 	BannerSetArg (*shell, DwtNy, y); 
*9E
*9I
*12D
 	BannerSetArg (*shell, XmNx, x); 
 	BannerSetArg (*shell, XmNy, y); 
*12E
*9E
*12D
 	}
     else
 	{
 	/*
 	 * We can use the root, so dummy up the pointers to point
 	 * to the root
 	 */
 	*shell = Bnr.dp_id;
 	*widget = Bnr.main_widget;
*12E
*9D
 	BannerSetArg (*shell, DwtNx, x); 
 	BannerSetArg (*shell, DwtNy, y); 
 	BannerSetArg (Bnr.dp_id, DwtNwidth, width); 
 	BannerSetArg (Bnr.dp_id, DwtNheight, height); 
*9E
*9I
*12D
 	BannerSetArg (*shell, XmNx, x); 
 	BannerSetArg (*shell, XmNy, y); 
 	BannerSetArg (*shell, XmNwidth, width); 
 	BannerSetArg (*shell, XmNheight, height); 
 	BannerSetArg (*widget, XmNwidth, width); 
 	BannerSetArg (*widget, XmNheight, height); 
 	XClearWindow (XtDisplay(*shell), 
 	    XtWindow(*widget));	
*12E
*9E
*12D
 	}
*12E
*12I
  * We need a shell, so create it, and the window for this
  * display
  */
*13I
*15D
     class = applicationShellWidgetClass;
 
*15E
*13E
*15D
     *shell = XtCreatePopupShell (name,
*15E
*13D
 		XtClass(Bnr.dp_id),
*13E
*13I
*15D
 		class,
*15E
*13E
*15D
 		Bnr.dp_id,
 		NULL,
 		0);
 
*15E
 
*13D
  
     XtSetArg(al[ac], XmNx, x); ac++;
     XtSetArg(al[ac], XmNy, y); ac++;
     XtSetArg(al[ac], XmNwidth, width); ac++;
     XtSetArg(al[ac], XmNheight, height); ac++;
 
 
     *widget = XmCreateDrawingArea(*shell,
 		    name,
 		    al, ac);
 
     XtManageChild (*widget);
*13E
*13I
 /*
  * Set the attributes for the images of the window, with or without 
  * title bars, with our without icon buttons, borders etc.
  */
*13E
*15D
     BannerSetArg (*shell, XmNx, x); 
     BannerSetArg (*shell, XmNy, y); 
*15E
*15I
     ac = 0;
 
     XtSetArg(al[ac], XmNx, x); ac++;
     XtSetArg(al[ac], XmNy, y); ac++;
*15E
*12E
 
*9D
     BannerSetArg (*widget, DwtNforeground, Bnr.foreground);
     BannerSetArg (*widget, DwtNbackground, Bnr.background);
*9E
*9I
*13D
     BannerSetArg (*widget, XmNforeground, Bnr.foreground);
     BannerSetArg (*widget, XmNbackground, Bnr.background);
*13E
*9E
*13D
 
 /*
  * Set the attributes for the images of the window, with or without 
  * title bars, with our without icon buttons, borders etc.
  */
*13E
     if (! border)
 	{
*9D
 	BannerSetArg (*shell, DwtNoverrideRedirect, 1); 
*9E
*9I
*15D
 	BannerSetArg (*shell, XmNoverrideRedirect, 1); 
*15E
*15I
 	XtSetArg(al[ac], XmNoverrideRedirect, 1); ac++;
*15E
*9E
 	}
 
     if (! Bnr.Param_Bits.window_border)
*18D
 	{
*18E
*18I
*19D
 	XtSetArg(al[ac], XmNborderWidth, 0); ac++;
*19E
*19I
 	{XtSetArg(al[ac], XmNborderWidth, 0); ac++;}
*19E
     else
*18E
*9D
 	BannerSetArg (*shell, DwtNborderWidth, 0); 
*9E
*9I
*15D
 	BannerSetArg (*shell, XmNborderWidth, 0); 
*15E
*15I
*19D
 	XtSetArg(al[ac], XmNborderWidth, 0); ac++;
*19E
*19I
*20D
 	{XtSetArg(al[ac], XmNborderWidth, 0); ac++;}
*20E
*20I
 	{XtSetArg(al[ac], XmNborderWidth, 1); ac++;}
*20E
*19E
*15E
*9E
*18D
 	}
*18E
 
     if (! icon && border)
*13I
 	{
*13E
*15D
 	BannerSetArg (*shell, XtNtransient, 1); 
*15E
*13D
 
*13E
*9D
     BannerSetArg (*shell, DwtNnoResize, 0); 
*9E
*9I
*13D
     BannerSetArg (*shell, XmNnoResize, 0); 
*13E
*13I
*15D
 	BannerSetArg (*shell, XmNtransient, 1); 
*15E
*15I
 	XtSetArg(al[ac], XmNtransient, 1); ac++;
*15E
 	}
     else
 	{
*15D
 	BannerSetArg (*shell, XtNtransient, 0); 
 	BannerSetArg (*shell, XmNtransient, 0); 
*15E
*15I
 	XtSetArg(al[ac], XmNtransient, 0); ac++;
*15E
 	}
 
*13E
*15D
     BannerSetArg (*shell, XtNallowShellResize, 1);
     BannerSetArg (*shell, XtNresize, 1);
*15E
*9E
*15D
 
*15E
*12D
 /*
  * No realise and manage the window!
  */
     if (main_used)
 	{
 	XtRealizeWidget (*shell);
 	XtPopup (*shell, XtGrabNone);
 	}
     else
 	{
 	*shell = Bnr.dp_id;
 	main_used = 1;
 	}
*12E
*12I
*13I
*15D
     BannerSetArg (*shell, XmNnoResize, 0); 
     BannerSetArg (*shell, XmNallowShellResize, 1);
     BannerSetArg (*shell, XmNallowResize, 1);
     BannerSetArg (*shell, XmNresize, 1); 
 
 
  
     XtSetArg(al[ac], XmNx, x); ac++;
     XtSetArg(al[ac], XmNy, y); ac++;
     XtSetArg(al[ac], XmNwidth, width); ac++;
     XtSetArg(al[ac], XmNheight, height); ac++;
*15E
*15I
*16D
     XtSetArg(al[ac], XtNallowShellResize, 1); ac++;
     XtSetArg(al[ac], XtNresize, 1); ac++;
 
*16E
*16I
     XtSetArg(al[ac], XmNallowShellResize, 1); ac++;
     XtSetArg(al[ac], XmNresize, 1); ac++;
*16E
     XtSetArg(al[ac], XmNnoResize, 0); ac++;
*16D
     XtSetArg(al[ac], XmNallowShellResize, 1); ac++;
     XtSetArg(al[ac], XmNallowResize, 1); ac++;
     XtSetArg(al[ac], XmNresize, 1); ac++;
*16E
*16I
     XtSetArg(al[ac], XmNallowResize, 1); ac++;
*16E
 
 
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
*16I
 
     XtSetArg(al[ac], XmNallowShellResize, 1); ac++;
     XtSetArg(al[ac], XmNresize, 1); ac++;
*16E
     XtSetArg(al[ac], XmNnoResize, 0); ac++;
*15E
*16I
     XtSetArg(al[ac], XmNallowResize, 1); ac++;
*16E
 
 
     *widget = XmCreateDrawingArea(*shell,
 		    name,
 		    al, ac);
 
     XtManageChild (*widget);
 
     BannerSetArg (*widget, XmNforeground, Bnr.foreground);
     BannerSetArg (*widget, XmNbackground, Bnr.background);
 
     BannerSetArg (*widget, XmNresize, 1);
     BannerSetArg (*widget, XmNresizePolicy, XmRESIZE_ANY);
 
*13E
     XtRealizeWidget (*shell);
*12E
 
 /*
  * Set what events we are intersted in for this window
  */
*15I
     XSelectInput (XtDisplay(*shell), XtWindow(*shell), 
 	VisibilityChangeMask |
 	ExposureMask |
 	StructureNotifyMask |
 	SubstructureNotifyMask |
 	PropertyChangeMask |
 	ButtonPressMask |
 	ButtonReleaseMask |
 	NoEventMask);
 
*15E
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
*12I
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
*13I
 
     XLowerWindow (XtDisplay(Bnr.dp_id), XtWindow (widget));
*13E
 }
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
*12E
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
*12I
 int onewindow = 0;
*12E
 /*
  * check to see if no windows need changing?
  */
*12D
 	main_used = 0;
*12E
 
 	if (force == 0 &&
 	    Bnr.Param_Bits.message_window == LBnr.Param_Bits.message_window &&
*24D
*23A1D
 	    Bnr.Param_Bits.monitor == LBnr.Param_Bits.monitor &&
*23A1E
*23A1I
 	    Bnr.Param_Bits.link_window == LBnr.Param_Bits.link_window &&
*23A1E
*24E
*24I
*30D
 	    Bnr.Param_Bits.link_window == LBnr.Param_Bits.link_window &&
*30E
*30I
 	    Bnr.Param_Bits.monitor == LBnr.Param_Bits.monitor &&
*30E
*24E
*31I
 	    Bnr.Param_Bits.link_window == LBnr.Param_Bits.link_window &&
*31E
 	    Bnr.Param_Bits.process_window == LBnr.Param_Bits.process_window &&
 	    Bnr.Param_Bits.pe_window == LBnr.Param_Bits.pe_window &&
 	    Bnr.Param_Bits.lck_window == LBnr.Param_Bits.lck_window &&
 	    Bnr.Param_Bits.cwps_window == LBnr.Param_Bits.cwps_window &&
*4I
 	    Bnr.Param_Bits.sys_window == LBnr.Param_Bits.sys_window &&
*4E
 	    Bnr.Param_Bits.cpu_window == LBnr.Param_Bits.cpu_window &&
*10I
 	    Bnr.Param_Bits.tp_window == LBnr.Param_Bits.tp_window &&
 	    Bnr.Param_Bits.qb_window == LBnr.Param_Bits.qb_window &&
 	    Bnr.Param_Bits.qp_window == LBnr.Param_Bits.qp_window &&
 	    Bnr.Param_Bits.clu_window == LBnr.Param_Bits.clu_window &&
*10E
 	    (Bnr.Param_Bits.analog_clock || Bnr.Param_Bits.digital_clock) ==
 	    (LBnr.Param_Bits.analog_clock || LBnr.Param_Bits.digital_clock) &&
*8D
 	    (Bnr.Param_Bits.world || Bnr.Param_Bits.hanoi || 
 	     Bnr.Param_Bits.cube) ==
 	     (LBnr.Param_Bits.world || LBnr.Param_Bits.hanoi || 
 	     LBnr.Param_Bits.cube) )
*8E
*8I
 	    Bnr.Param_Bits.world == LBnr.Param_Bits.world &&
 	    Bnr.Param_Bits.hanoi == LBnr.Param_Bits.hanoi &&
 	    Bnr.Param_Bits.cube == LBnr.Param_Bits.cube
 	     )
*8E
 	     return;
 
*12D
 /*
  * we need to rebuild all the windows, so destroy them all, and recreate them
  */	     
*12E
 	memcpy(&LBnr, &Bnr, sizeof(Bnr));
*15I
 
*15E
 /*
*12D
  * Now destroy all the shell widgets, with the exception of the message 
  * widget, which is special
  */
 	if (Bnr.clock_shell_widget != 0 
 	    && Bnr.clock_shell_widget != Bnr.dp_id)
 	    {
 	    XtDestroyWidget (Bnr.clock_shell_widget);
 	    Bnr.clock_shell_widget = 0;
 	    Bnr.clock_widget = Bnr.dp_id;
 	    }
 	if (Bnr.pe_shell_widget != 0 
 	    && Bnr.pe_shell_widget != Bnr.dp_id)
 	    {
 	    XtDestroyWidget (Bnr.pe_shell_widget);
 	    Bnr.pe_shell_widget = 0;
 	    Bnr.pe_widget = Bnr.dp_id;
 	    }
 	if (Bnr.cwps_shell_widget != 0 
 	    && Bnr.cwps_shell_widget != Bnr.dp_id)
 	    {
 	    XtDestroyWidget (Bnr.cwps_shell_widget);
 	    Bnr.cwps_shell_widget = 0;
 	    Bnr.cwps_widget = Bnr.dp_id;
 	    }
*12E
*4I
*12D
 	if (Bnr.sys_shell_widget != 0 
 	    && Bnr.sys_shell_widget != Bnr.dp_id)
 	    {
 	    XtDestroyWidget (Bnr.sys_shell_widget);
 	    Bnr.sys_shell_widget = 0;
 	    Bnr.sys_widget = Bnr.dp_id;
 	    }
*12E
*4E
*10I
*12D
 	if (Bnr.tp_shell_widget != 0 
 	    && Bnr.tp_shell_widget != Bnr.dp_id)
 	    {
 	    XtDestroyWidget (Bnr.tp_shell_widget);
 	    Bnr.tp_shell_widget = 0;
 	    Bnr.tp_widget = Bnr.dp_id;
 	    }
 	if (Bnr.qb_shell_widget != 0 
 	    && Bnr.qb_shell_widget != Bnr.dp_id)
 	    {
 	    XtDestroyWidget (Bnr.qb_shell_widget);
 	    Bnr.qb_shell_widget = 0;
 	    Bnr.qb_widget = Bnr.dp_id;
 	    }
 	if (Bnr.qp_shell_widget != 0 
 	    && Bnr.qp_shell_widget != Bnr.dp_id)
 	    {
 	    XtDestroyWidget (Bnr.qp_shell_widget);
 	    Bnr.qp_shell_widget = 0;
 	    Bnr.qp_widget = Bnr.dp_id;
 	    }
 	if (Bnr.clu_shell_widget != 0 
 	    && Bnr.clu_shell_widget != Bnr.dp_id)
 	    {
 	    XtDestroyWidget (Bnr.clu_shell_widget);
 	    Bnr.clu_shell_widget = 0;
 	    Bnr.clu_widget = Bnr.dp_id;
 	    }
*12E
*10E
*12D
 	if (Bnr.lck_shell_widget != 0 
 	    && Bnr.lck_shell_widget != Bnr.dp_id)
 	    {
 	    XtDestroyWidget (Bnr.lck_shell_widget);
 	    Bnr.lck_shell_widget = 0;
 	    Bnr.lck_widget = Bnr.dp_id;
 	    }
 	if (Bnr.cube_shell_widget != 0
 	    && Bnr.cube_shell_widget != Bnr.dp_id)
 	    {
 	    XtDestroyWidget (Bnr.cube_shell_widget);
 	    Bnr.cube_shell_widget = 0;
 	    Bnr.cube_widget = Bnr.dp_id;
 	    }
*12E
*8I
*12D
 	if (Bnr.globe_shell_widget != 0
 	    && Bnr.globe_shell_widget != Bnr.dp_id)
 	    {
 	    XtDestroyWidget (Bnr.globe_shell_widget);
 	    Bnr.globe_shell_widget = 0;
 	    Bnr.globe_widget = Bnr.dp_id;
 	    }
 	if (Bnr.hanoi_shell_widget != 0
 	    && Bnr.hanoi_shell_widget != Bnr.dp_id)
 	    {
 	    XtDestroyWidget (Bnr.hanoi_shell_widget);
 	    Bnr.hanoi_shell_widget = 0;
 	    Bnr.hanoi_widget = Bnr.dp_id;
 	    }
*12E
*8E
*12D
 	if (Bnr.cpu_shell_widget != 0
 	    && Bnr.cpu_shell_widget != Bnr.dp_id)
 	    {
 	    XtDestroyWidget (Bnr.cpu_shell_widget);
 	    Bnr.cpu_shell_widget = 0;
 	    Bnr.cpu_widget = Bnr.dp_id;
 	    }
 	if (Bnr.mon_shell_widget != 0
 	    && Bnr.mon_shell_widget != Bnr.dp_id)
 	    {
 	    XtDestroyWidget (Bnr.mon_shell_widget);
 	    Bnr.mon_shell_widget = 0;
 	    Bnr.mon_widget = Bnr.dp_id;
 	    }
 	if (Bnr.process_shell_widget != 0
 	    && Bnr.process_shell_widget != Bnr.dp_id)
 	    {
 	    XtDestroyWidget (Bnr.process_shell_widget);
 	    Bnr.process_shell_widget = 0;
 	    Bnr.process_widget = Bnr.dp_id;
 	    }
 
 	Bnr.clock_widget = Bnr.dp_id;
 	Bnr.cube_widget = Bnr.dp_id;
*12E
*8I
*12D
 	Bnr.globe_widget = Bnr.dp_id;
 	Bnr.hanoi_widget = Bnr.dp_id;
*12E
*8E
*12D
 	Bnr.cpu_widget = Bnr.dp_id;
 	Bnr.mon_widget = Bnr.dp_id;
 	Bnr.pe_widget = Bnr.dp_id;
 	Bnr.lck_widget = Bnr.dp_id;
 	Bnr.cwps_widget = Bnr.dp_id;
*12E
*4I
*12D
 	Bnr.sys_widget = Bnr.dp_id;
*12E
*4E
*12D
 	Bnr.process_widget = Bnr.dp_id;
*12E
*10I
*12D
 	Bnr.tp_widget = Bnr.dp_id;
 	Bnr.qp_widget = Bnr.dp_id;
 	Bnr.qb_widget = Bnr.dp_id;
 	Bnr.clu_widget = Bnr.dp_id;
*12E
*10E
*12D
 
 
 /*
  * Now create the other widgets, and shells, not we will use the root
  * window, for the first object to be created
*12E
*12I
  * Now popdown all widgets that arnt active
*12E
  */
 	if ((Bnr.Param_Bits.analog_clock || Bnr.Param_Bits.digital_clock))
*12D
 	    BannerCreateWindow(Bnr.clock_x,
*12E
*12I
 	    {
 	    onewindow = 1;
 	    BannerPlaceWindow(Bnr.clock_x,
*12E
 		Bnr.clock_y,
 		Bnr.clock_width,
 		Bnr.clock_height,
*12D
 		"Clock",
 		&Bnr.clock_shell_widget,
 		&Bnr.clock_widget);
 
 /*
  * the window for the Cube
  */
 
 	
*12E
*8D
 	if (Bnr.Param_Bits.world || 
 	    Bnr.Param_Bits.hanoi || 
 	    Bnr.Param_Bits.cube)
*8E
*8I
*12D
 	if (Bnr.Param_Bits.world)
 	    BannerCreateWindow(Bnr.globe_x,
 		Bnr.globe_y,
 		Bnr.globe_width,
 		Bnr.globe_height,
 		"Toy",
 		&Bnr.globe_shell_widget,
 		&Bnr.globe_widget);
 
 	
 	if (Bnr.Param_Bits.hanoi)
 	    BannerCreateWindow(Bnr.hanoi_x,
 		Bnr.hanoi_y,
 		Bnr.hanoi_width,
 		Bnr.hanoi_height,
 		"Toy",
 		&Bnr.hanoi_shell_widget,
 		&Bnr.hanoi_widget);
 
 	
 	if (Bnr.Param_Bits.cube)
*12E
*8E
*12D
 	    BannerCreateWindow(Bnr.cube_x,
 		Bnr.cube_y,
 		Bnr.cube_width,
 		Bnr.cube_height,
 		"Toy",
 		&Bnr.cube_shell_widget,
 		&Bnr.cube_widget);
 /*
  * the window for the cpu
  */
 
 	if (Bnr.Param_Bits.cpu_window)
 	    BannerCreateWindow(Bnr.cpu_x,
 		Bnr.cpu_y,
 		Bnr.cpu_width,
 		Bnr.cpu_height,
 		"Cpu",
 		&Bnr.cpu_shell_widget,
 		&Bnr.cpu_widget);
 
 /*
  * the window for the monitor
  */
 	
 	if (Bnr.Param_Bits.monitor)
 	    BannerCreateWindow(Bnr.mon_x,
 		Bnr.mon_y,
 		Bnr.mon_width,
 		Bnr.mon_height,
 		"Page IO",
 		&Bnr.mon_shell_widget,
 		&Bnr.mon_widget);
 
 /*
  * the window for the process monitor
  */
 	
 	if (Bnr.Param_Bits.process_window)
 	    BannerCreateWindow(Bnr.process_x,
 		Bnr.process_y,
 		Bnr.process_width,
 		Bnr.process_height,
 		"Processes",
 		&Bnr.process_shell_widget,
 		&Bnr.process_widget);
 
 /*
  * the window for the pe
  */
*12E
*12I
 		Bnr.clock_shell_widget,
 		Bnr.clock_widget);
 	    XtMapWidget (Bnr.clock_shell_widget);
 	    }
 	else
 	    XtUnmapWidget (Bnr.clock_shell_widget);
*12E
 
 	if (Bnr.Param_Bits.pe_window)
*12D
 	    BannerCreateWindow(Bnr.pe_x,
*12E
*12I
 	    {
 	    onewindow = 1;
 	    BannerPlaceWindow(Bnr.pe_x,
*12E
 		Bnr.pe_y,
 		Bnr.pe_width,
 		Bnr.pe_height,
*12D
 		"PE",
 		&Bnr.pe_shell_widget,
 		&Bnr.pe_widget);
 
 /*
  * the window for the Locks
  */
 
 	if (Bnr.Param_Bits.lck_window)
 	    BannerCreateWindow(Bnr.lck_x,
 		Bnr.lck_y,
 		Bnr.lck_width,
 		Bnr.lck_height,
 		"Locks",
 		&Bnr.lck_shell_widget,
 		&Bnr.lck_widget);
 /*
  * the window for the CWPS
  */
*12E
*12I
 		Bnr.pe_shell_widget,
 		Bnr.pe_widget);
 	    XtMapWidget (Bnr.pe_shell_widget);
 	    }
 	else
 	    XtUnmapWidget (Bnr.pe_shell_widget);
*12E
 
 	if (Bnr.Param_Bits.cwps_window)
*12D
 	    BannerCreateWindow(Bnr.cwps_x,
*12E
*12I
 	    {
 	    onewindow = 1;
 	    BannerPlaceWindow(Bnr.cwps_x,
*12E
 		Bnr.cwps_y,
 		Bnr.cwps_width,
 		Bnr.cwps_height,
*12D
 		"CWPS",
 		&Bnr.cwps_shell_widget,
 		&Bnr.cwps_widget);
 
 /*
*12E
*4I
*12D
  * the window for the sys display
  */
*12E
*12I
 		Bnr.cwps_shell_widget,
 		Bnr.cwps_widget);
 	    XtMapWidget (Bnr.cwps_shell_widget);
 	    }
 	else
 	    XtUnmapWidget (Bnr.cwps_shell_widget);
*12E
 
 	if (Bnr.Param_Bits.sys_window)
*12D
 	    BannerCreateWindow(Bnr.sys_x,
*12E
*12I
 	    {
 	    onewindow = 1;
 	    BannerPlaceWindow(Bnr.sys_x,
*12E
 		Bnr.sys_y,
 		Bnr.sys_width,
 		Bnr.sys_height,
*12D
 		"System",
 		&Bnr.sys_shell_widget,
 		&Bnr.sys_widget);
 
 /*
*12E
*4E
*10I
*12D
  * the window for the tp display
  */
*12E
*12I
 		Bnr.sys_shell_widget,
 		Bnr.sys_widget);
 	    XtMapWidget (Bnr.sys_shell_widget);
 	    }
 	else
 	    XtUnmapWidget (Bnr.sys_shell_widget);
*12E
 
 	if (Bnr.Param_Bits.tp_window)
*12D
 	    BannerCreateWindow(Bnr.tp_x,
*12E
*12I
 	    {
 	    onewindow = 1;
 	    BannerPlaceWindow(Bnr.tp_x,
*12E
 		Bnr.tp_y,
 		Bnr.tp_width,
 		Bnr.tp_height,
*12D
 		"Trans Process",
 		&Bnr.tp_shell_widget,
 		&Bnr.tp_widget);
 
 /*
  * the window for the qp display
  */
*12E
*12I
 		Bnr.tp_shell_widget,
 		Bnr.tp_widget);
 	    XtMapWidget (Bnr.tp_shell_widget);
 	    }
 	else
 	    XtUnmapWidget (Bnr.tp_shell_widget);
*12E
 
 	if (Bnr.Param_Bits.qp_window)
*12D
 	    BannerCreateWindow(Bnr.qp_x,
*12E
*12I
 	    {
 	    onewindow = 1;
 	    BannerPlaceWindow(Bnr.qp_x,
*12E
 		Bnr.qp_y,
 		Bnr.qp_width,
 		Bnr.qp_height,
*12D
 		"Print Queue",
 		&Bnr.qp_shell_widget,
 		&Bnr.qp_widget);
 
 /*
  * the window for the qb display
  */
*12E
*12I
 		Bnr.qp_shell_widget,
 		Bnr.qp_widget);
*17D
 	    XtMapWidget (Bnr.qb_shell_widget);
*17E
*17I
 	    XtMapWidget (Bnr.qp_shell_widget);
*17E
 	    }
 	else
*17D
 	    XtUnmapWidget (Bnr.qb_shell_widget);
*17E
*17I
 	    XtUnmapWidget (Bnr.qp_shell_widget);
*17E
*12E
 
 	if (Bnr.Param_Bits.qb_window)
*12D
 	    BannerCreateWindow(Bnr.qb_x,
*12E
*12I
 	    {
 	    onewindow = 1;
 	    BannerPlaceWindow(Bnr.qb_x,
*12E
 		Bnr.qb_y,
 		Bnr.qb_width,
 		Bnr.qb_height,
*12D
 		"Batch Queue",
 		&Bnr.qb_shell_widget,
 		&Bnr.qb_widget);
 
 /*
  * the window for the clu display
  */
*12E
*12I
 		Bnr.qb_shell_widget,
 		Bnr.qb_widget);
*17D
 	    XtMapWidget (Bnr.qp_shell_widget);
*17E
*17I
 	    XtMapWidget (Bnr.qb_shell_widget);
*17E
 	    }
 	else
*17D
 	    XtUnmapWidget (Bnr.qp_shell_widget);
*17E
*17I
 	    XtUnmapWidget (Bnr.qb_shell_widget);
*17E
*12E
 
 	if (Bnr.Param_Bits.clu_window)
*12D
 	    BannerCreateWindow(Bnr.clu_x,
*12E
*12I
 	    {
 	    onewindow = 1;
 	    BannerPlaceWindow(Bnr.clu_x,
*12E
 		Bnr.clu_y,
 		Bnr.clu_width,
 		Bnr.clu_height,
*12D
 		"Cluster",
 		&Bnr.clu_shell_widget,
 		&Bnr.clu_widget);
 
 /*
*12E
*10E
*12D
  * Now if there has been noone to use the main window, put up the dummy window.
  */
     if (!main_used)
 	{
 
*12E
*9D
 	BannerSetArg (Bnr.dp_id, DwtNwidth, 20*Bnr.font_width); 
 	BannerSetArg (Bnr.dp_id, DwtNheight, 3*Bnr.font_height); 
 	BannerSetArg (Bnr.dp_id, DwtNx, 0); 
 	BannerSetArg (Bnr.dp_id, DwtNy, 0); 
*9E
*9I
*12D
 	BannerSetArg (Bnr.dp_id, XmNwidth, 20*Bnr.font_width); 
 	BannerSetArg (Bnr.dp_id, XmNheight, 3*Bnr.font_height); 
 	BannerSetArg (Bnr.dp_id, XmNx, 0); 
 	BannerSetArg (Bnr.dp_id, XmNy, 0); 
*12E
*9E
*12D
 	    
 	XSelectInput (XtDisplay(Bnr.main_widget), XtWindow(Bnr.main_widget), 
 		    VisibilityChangeMask |
 		    ExposureMask |
 		    StructureNotifyMask |
 		    SubstructureNotifyMask |
 		    PropertyChangeMask |
 		    ButtonPressMask |
 		    ButtonReleaseMask |
 		    NoEventMask);
 
 	}
 
 
 /*
  * Now create/destroy the message widget if we need to
  */
*12E
*11I
*12D
 	if (Bnr.message_shell_widget != 0)
 	    {
 	    XtDestroyWidget (Bnr.message_shell_widget);
 	    Bnr.message_shell_widget = 0;
 	    Bnr.message_widget = Bnr.dp_id;
 	    }
 
*12E
*11E
*12D
 	if (Bnr.Param_Bits.message_window &&
 	    Bnr.message_shell_widget == 0)
*12E
*12I
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
 
*24D
*23A1D
 	if (Bnr.Param_Bits.monitor)
*23A1E
*23A1I
 	if (Bnr.Param_Bits.link_window)
*23A1E
*24E
*24I
*30D
 	if (Bnr.Param_Bits.link_window)
*30E
*30I
 	if (Bnr.Param_Bits.monitor)
*30E
*24E
*12E
 	    {
*12D
 	    int class;
 	    Bnr.message_shell_widget = XtCreatePopupShell ("Banner\nMessage",
 			    XtClass(Bnr.dp_id),
 			    Bnr.dp_id,
 			    NULL,
 			    0);
 
*12E
*9D
 	    BannerSetArg (Bnr.message_shell_widget, DwtNx, Bnr.message_x); 
 	    BannerSetArg (Bnr.message_shell_widget, DwtNy, Bnr.message_y); 
*9E
*9I
*12D
 	    BannerSetArg (Bnr.message_shell_widget, XmNx, Bnr.message_x); 
 	    BannerSetArg (Bnr.message_shell_widget, XmNy, Bnr.message_y); 
*12E
*9E
*12D
 
 	    BannerSetArg (Bnr.message_shell_widget, XtNiconPixmap, BannerPixmap);
*12E
*9D
 	    BannerSetArg (Bnr.message_shell_widget, XtNiconifyPixmap, SmBannerPixmap);
*9E
*9I
*12D
 /*	    BannerSetArg (Bnr.message_shell_widget, XtNiconifyPixmap, SmBannerPixmap);*/
*12E
*9E
*12D
 
 	    if (! Bnr.Param_Bits.border)
*12E
*9D
 		BannerSetArg (Bnr.message_shell_widget, DwtNoverrideRedirect, 1); 
*9E
*9I
*12D
 		BannerSetArg (Bnr.message_shell_widget, XmNoverrideRedirect, 1); 
*12E
*9E
*12D
 
 
*12E
*9D
 	    BannerSetArg (Bnr.message_shell_widget, DwtNnoResize, 0); 
*9E
*9I
*12D
 	    BannerSetArg (Bnr.message_shell_widget, XmNnoResize, 0); 
 	    BannerSetArg (Bnr.message_shell_widget, XtNallowShellResize, 1);
 	    BannerSetArg (Bnr.message_shell_widget, XtNresize, 1);
*12E
*9E
*12D
 
 
*12E
*9D
 	    DwtFetchWidget (Bnr.drm_prt,
*9E
*9I
*12D
 	    MrmFetchWidget (Bnr.drm_prt,
*12E
*9E
*12D
 		"BANNER_MESSAGE_WIDGET",
 		Bnr.message_shell_widget,
 		&Bnr.message_widget,
 		&class);
 
 
*12E
*9D
 	    BannerSetArg (Bnr.message_widget, DwtNforeground, Bnr.foreground);
 	    BannerSetArg (Bnr.message_widget, DwtNbackground, Bnr.background);
*9E
*9I
*12D
 	    BannerSetArg (Bnr.message_widget, XmNforeground, Bnr.foreground);
 	    BannerSetArg (Bnr.message_widget, XmNbackground, Bnr.background);
*12E
*9E
*12D
 	
 	    BannerSetArg (Bnr.message_widget, 
*12E
*9D
 		DwtNwidth, Bnr.message_width);
*9E
*9I
*12D
 		XmNwidth, Bnr.message_width);
*12E
*9E
*12D
 	    BannerSetArg (Bnr.message_widget, 
*12E
*9D
 		DwtNheight, Bnr.message_height);
*9E
*9I
*12D
 		XmNheight, Bnr.message_height);
*12E
*9E
*12D
 
 	    if (! Bnr.Param_Bits.window_border)
 		{
*12E
*9D
 		BannerSetArg (Bnr.message_shell_widget, DwtNborderWidth, 0); 
*9E
*9I
*12D
 		BannerSetArg (Bnr.message_shell_widget, XmNborderWidth, 0); 
*12E
*9E
*12D
 		}
 	
 	    XtManageChild (Bnr.message_widget);
 
 	    XtRealizeWidget (Bnr.message_shell_widget);
 
 	    XtPopup (Bnr.message_shell_widget, XtGrabNone);
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
 	    }
 	else
*12E
*12I
 	    onewindow = 1;
*24D
*23A1D
 	    BannerPlaceWindow(Bnr.mon_x,
 		Bnr.mon_y,
 		Bnr.mon_width,
 		Bnr.mon_height,
 		Bnr.mon_shell_widget,
 		Bnr.mon_widget);
 	    XtMapWidget (Bnr.mon_shell_widget);
*23A1E
*23A1I
 	    BannerPlaceWindow(Bnr.link_x,
 		Bnr.link_y,
 		Bnr.link_width,
 		Bnr.link_height,
 		Bnr.link_shell_widget,
 		Bnr.link_widget);
 	    XtMapWidget (Bnr.link_shell_widget);
*23A1E
*24E
*24I
*30D
 	    BannerPlaceWindow(Bnr.link_x,
 		Bnr.link_y,
 		Bnr.link_width,
 		Bnr.link_height,
 		Bnr.link_shell_widget,
 		Bnr.link_widget);
 	    XtMapWidget (Bnr.link_shell_widget);
*30E
*30I
 	    BannerPlaceWindow(Bnr.mon_x,
 		Bnr.mon_y,
 		Bnr.mon_width,
 		Bnr.mon_height,
 		Bnr.mon_shell_widget,
 		Bnr.mon_widget);
 	    XtMapWidget (Bnr.mon_shell_widget);
*30E
*24E
 	    }
 	else
*24D
*23A1D
 	    XtUnmapWidget (Bnr.mon_shell_widget);
*23A1E
*23A1I
 	    XtUnmapWidget (Bnr.link_shell_widget);
*23A1E
*24E
*24I
*30D
 	    XtUnmapWidget (Bnr.link_shell_widget);
*30E
*30I
 	    XtUnmapWidget (Bnr.mon_shell_widget);
*30E
*24E
 
*31I
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
 
*31E
 	if (Bnr.Param_Bits.process_window)
*12E
 	    {
*12D
 	    if (Bnr.message_shell_widget != 0 &&
 		    ! Bnr.Param_Bits.window_border)
 		{
*12E
*9D
 		BannerSetArg (Bnr.message_shell_widget, DwtNborderWidth, 0); 
*9E
*9I
*12D
 		BannerSetArg (Bnr.message_shell_widget, XmNborderWidth, 0); 
*12E
*9E
*12D
 		}
 	    if (Bnr.message_shell_widget != 0 &&
 		!Bnr.Param_Bits.message_window)
 		{
 		XtDestroyWidget (Bnr.message_shell_widget);
 		Bnr.message_shell_widget = 0;
 		Bnr.message_widget = Bnr.dp_id;
 		}
*12E
*12I
 	    onewindow = 1;
 	    BannerPlaceWindow(Bnr.process_x,
 		Bnr.process_y,
 		Bnr.process_width,
 		Bnr.process_height,
 		Bnr.process_shell_widget,
 		Bnr.process_widget);
 	    XtMapWidget (Bnr.process_shell_widget);
*12E
 	    }
*12I
 	else
 	    XtUnmapWidget (Bnr.process_shell_widget);
 
 /*
  * Now if there has been noone to use the main window, put up the dummy window.
  */
*13D
     if (!onewindow)
*13E
*13I
 
     if (!onewindow || Bnr.Param_Bits.border)
*13E
 	{
*15I
 	BannerPlaceWindow(Bnr.clock_x,
 	    Bnr.clock_y,
 	    20*Bnr.font_width,
 	    3*Bnr.font_height,
 	    Bnr.dp_id,
 	    Bnr.main_widget);
*15E
 	XtMapWidget (Bnr.dp_id);
 	}
     else
 	XtUnmapWidget (Bnr.dp_id);
 
*12E
*13I
 /*
  * Unmap the message window
  */
     BannerMessageHide ();
*13E
 }
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
*3D
 **	BannerMainLoop ()
 **
 **      This is the main X event loop. Because Banner need continous control
 **	over the CPU we cannot use XtMainLoop, but have to process all events 
 **	ourselves, handling the ones we are interested in. 
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
 static void	
 BannerMainLoop()
 {
     int TRACE;
     int height, width;
     Window MoveWindow = 0;
     Widget MoveWidget;
     Widget MoveingWidget;
     XWindowChanges xswa;
     int MoveX, MoveY;
     int LastPointerX, LastPointerY;
 /*
  * First create our GC to work with.
  */
     GcValues.function = GXcopy;
     BannerGC = XCreateGC (XtDisplay(Bnr.dp_id), XtWindow(Bnr.main_widget), 
 	GCFunction, &GcValues);
     XSetForeground (XtDisplay(Bnr.dp_id), BannerGC, Bnr.foreground);
     XSetBackground (XtDisplay(Bnr.dp_id), BannerGC, Bnr.background);
 
     XSetFont (XtDisplay(Bnr.dp_id), BannerGC, Bnr.font_ptr->fid);
*3E
*2D
     XSetPlaneMask (XtDisplay(Bnr.dp_id), BannerGC, XAllPlanes());
*2E
*3D
 /*
  * Now syncronize with the Server
  */
     XSync (XtDisplay (Bnr.dp_id), 0);
 
     XSetStipple (XtDisplay(Bnr.dp_id), BannerGC, BannerFillPixmap);
 
     if (Bnr.message_init_state == 1 &&
 	Bnr.Param_Bits.message_window == 1)
 	    XtUnmapWidget (Bnr.message_shell_widget);
 
 /*
  * now set our priority, take the priority they Gave us, unless Hanoi, or Cube
  * is running, in which case force it to 0...
  */
 
     if (Bnr.Param_Bits.cube || Bnr.Param_Bits.hanoi)
 	Bnr.execute_priority = 0;
     else
 	Bnr.execute_priority = Bnr.base_priority;
 
     SYS$SETPRI (NULL, NULL, Bnr.base_priority, &BannerInitialPriority);
     Bnr.current_priority = Bnr.base_priority;
 
 /*
  * Now set our 1 second wakeup timer
  */
 
     BannerSecondTimerAst ();
     Bnr.ws_purged = 0;
 /*
  * Set up a loop that will service all the events that we recieve
  */
 
     while (1)
 	{
 
 /*
  * Now raise our priority if we need to, to get things done
  */
 
 	if (Bnr.current_priority != Bnr.base_priority)
 	    {
 	    SYS$SETPRI (NULL, NULL, Bnr.base_priority, NULL);
 	    Bnr.current_priority = Bnr.base_priority;
 	    }
 
 	XtNextEvent (&BannerEvent);
 	XtDispatchEvent (&BannerEvent);
 	if (TRACE == 1)
 	    printf ("Event: w=%x, event=%d\n", BannerEvent.xany.window, BannerEvent.xany.type);
 	if (BannerEvent.xany.window == RootWindow (XtDisplay(Bnr.dp_id),
 			    DefaultScreen(XtDisplay(Bnr.dp_id))) ||
 	     BannerEvent.xany.window == XtWindow(MoveWidget = Bnr.main_widget) ||
 	     BannerEvent.xany.window == XtWindow(MoveWidget = Bnr.dp_id) ||
 	     BannerEvent.xany.window == XtWindow(MoveWidget = Bnr.clock_widget) ||
 	     BannerEvent.xany.window == XtWindow(MoveWidget = Bnr.message_widget) ||
 	     BannerEvent.xany.window == XtWindow(MoveWidget = Bnr.top_widget) ||
 	     BannerEvent.xany.window == XtWindow(MoveWidget = Bnr.cpu_widget) ||
 	     BannerEvent.xany.window == XtWindow(MoveWidget = Bnr.pe_widget) ||
 	     BannerEvent.xany.window == XtWindow(MoveWidget = Bnr.lck_widget) ||
 	     BannerEvent.xany.window == XtWindow(MoveWidget = Bnr.cwps_widget) ||
 	     BannerEvent.xany.window == XtWindow(MoveWidget = Bnr.cube_widget) ||
 	     BannerEvent.xany.window == XtWindow(MoveWidget = Bnr.process_widget) ||
 	     BannerEvent.xany.window == XtWindow(MoveWidget = Bnr.mon_widget))
 	    switch (BannerEvent.xany.type)
 	    {
 	    case MotionNotify :
 		{
 /*
  * We have just recieved a motion event, we only ask for them on 
  * the move window, so reposition the window, to the new place
  */
 		if (MoveWindow != 0)
 		    {
 		    while (1)
 			{
 			MoveX = MoveX + ( BannerEvent.xmotion.x_root - LastPointerX);
 			MoveY = MoveY + ( BannerEvent.xmotion.y_root - LastPointerY);
 			LastPointerX = BannerEvent.xmotion.x_root;
 			LastPointerY = BannerEvent.xmotion.y_root;
 			if (XCheckTypedEvent (
 				XtDisplay (Bnr.dp_id), MotionNotify,
 				    &BannerEvent) == 0)
 				break;
 			}
 		    XMoveWindow (XtDisplay(Bnr.dp_id),
 			MoveWindow,
 			MoveX,
 			MoveY);
 		    }		
 		break;
 		}
 	    case ButtonPress :
 		{
 /*
  * If a move is in operation Can it!
  */
 		if (MoveWindow != 0 )
 		    {
 		    Widget widget;
 
 		    widget = MoveingWidget->core.parent;
 
 /*
  * Put us back to where we started
  */
 		    XMoveWindow (XtDisplay(Bnr.dp_id),
 			MoveWindow,
 			widget->core.x,
 			widget->core.y);
 
 /*
  * Turn borders off, 
  */
 
 		    if (Bnr.Param_Bits.window_border)
 			xswa.border_width = 1;
 		    else
 			xswa.border_width = 0;
 		    XConfigureWindow (XtDisplay(Bnr.dp_id),
 			MoveWindow, CWBorderWidth, &xswa);
 
 		    MoveWindow = 0;
 
 		    XSelectInput (XtDisplay(MoveingWidget), 
 				XtWindow(MoveingWidget), 
 				VisibilityChangeMask |
 				ExposureMask |
 				StructureNotifyMask |
 				SubstructureNotifyMask |
 				PropertyChangeMask |
 				ButtonPressMask |
 				ButtonReleaseMask |
 				NoEventMask);
 		    break;
 		    }
 /*
  * Someone asking for the popup?
  */
 		if (BannerEvent.xbutton.button == 2)
 		    BannerMenuPopup (&BannerEvent);
 		else
 		    BannerMenuPopupDestroy ();
 /*
  * If it a MB1 button, and we have no boarders, then go into
  * move window mode.
  */
 		if (BannerEvent.xbutton.button == 1 &&
 		    ! Bnr.Param_Bits.border)
 		    {
 		    Widget widget;
 
 		    widget = MoveWidget->core.parent;
 		    MoveingWidget = MoveWidget;
 
 		    LastPointerX = BannerEvent.xbutton.x_root;
 		    LastPointerY = BannerEvent.xbutton.y_root;
 				    
 		    MoveX = MoveY = 0;
 		    BannerGetArg (widget, DwtNx, 
 			&MoveX); 
 		    BannerGetArg (widget, DwtNy, 
 			&MoveY); 
 
 		    BannerGetArg (widget, DwtNwidth, 
 			&width); 
 		    BannerGetArg (widget, DwtNheight, 
 			&height); 
 		    
 		    MoveWindow = XtWindow (widget);
 
 /*
  * Turn borders on, 
  */
 
 		    xswa.border_width = 1;
 		    XConfigureWindow (XtDisplay(Bnr.dp_id),
 			MoveWindow, CWBorderWidth, &xswa);
 
 /*
  * Grab the pointer now, so we know where the window is 
  * being moved to
  */		    
 		    XSelectInput (XtDisplay(MoveingWidget), 
 				XtWindow(MoveingWidget), 
 				VisibilityChangeMask |
 				ExposureMask |
 				StructureNotifyMask |
 				SubstructureNotifyMask |
 				PropertyChangeMask |
 				ButtonPressMask |
 				ButtonReleaseMask |
 				PointerMotionMask | 
 				Button1MotionMask |
 				ButtonMotionMask |
 				NoEventMask);
 
 		    XChangeActivePointerGrab (
 				XtDisplay (Bnr.dp_id),
 				ButtonPressMask |
 				ButtonReleaseMask |
 				PointerMotionMask | 
 				Button1MotionMask |
 				ButtonMotionMask ,
 				NULL,
 				CurrentTime);
 				
 		    }
 		break;
 		}
 	    case ButtonRelease :
 		{
 		BannerMenuPopupDestroy ();
 /*
  * If we were in move window mode, stop it, and 
  * place the window at the new possition
  */
 		if (MoveWindow != 0 &&
 		    BannerEvent.xbutton.button == 1)
 		    {
 		    Widget widget;
 
 		    widget = MoveingWidget->core.parent;
 		    BannerGetArg (widget, DwtNwidth, 
 			&width); 
 		    BannerGetArg (widget, DwtNheight, 
 			&height); 
 
 /*
  * Turn borders off
  */
 
 		    if (Bnr.Param_Bits.window_border)
 			xswa.border_width = 1;
 		    else
 			xswa.border_width = 0;
 		    XConfigureWindow (XtDisplay(Bnr.dp_id),
 			MoveWindow, CWBorderWidth, &xswa);
 
 		    MoveWindow = 0;
 		    XSelectInput (XtDisplay(MoveingWidget), 
 				XtWindow(MoveingWidget), 
 				VisibilityChangeMask |
 				ExposureMask |
 				StructureNotifyMask |
 				SubstructureNotifyMask |
 				PropertyChangeMask |
 				ButtonPressMask |
 				ButtonReleaseMask |
 				NoEventMask);
 /*
  * Store our X,Y coordinates. Note: The cludge, where we set them
  * direct into the widget structure as well, this seems the only
  * way we can make it solide, with these types of windows?
  */
 		    BannerSetArg (widget, DwtNx, 
 			MoveX); 
 		    BannerSetArg (widget, DwtNy, 
 			MoveY); 
 		    widget->core.x = MoveX;
 		    widget->core.y = MoveY;
 		    }
 		break;
 		}
 	    case Expose :
 		{
 
 /*
  * If this is the last one of a sequence, then we must repaint the window,
  * so find which window it is, and call the action routine to do it.
  */
 	        if (BannerEvent.xexpose.count == 0)
 		    {		    
 		    if (BannerEvent.xany.window == XtWindow(Bnr.pe_widget) &&
 			Bnr.pe_widget != Bnr.dp_id)
 			{
 			    BannerPERefresh ();
 			    Bnr.ws_purged = 0;
 			break;
 			}
 		    if (BannerEvent.xany.window == XtWindow(Bnr.lck_widget) &&
 			Bnr.lck_widget != Bnr.dp_id)
 			{
 			    BannerLCKRefresh ();
 			    Bnr.ws_purged = 0;
 			break;
 			}
 		    if (BannerEvent.xany.window == XtWindow(Bnr.cwps_widget) &&
 			Bnr.cwps_widget != Bnr.dp_id)
 			{
 			    BannerCWPSRefresh ();
 			    Bnr.ws_purged = 0;
 			break;
 			}
 		    if (BannerEvent.xany.window == XtWindow(Bnr.cpu_widget) &&
 			Bnr.cpu_widget != Bnr.dp_id)
 			{
 			    BannerCpuRefresh ();
 			    Bnr.ws_purged = 0;
 			break;
 			}
 		    if (BannerEvent.xany.window == XtWindow(Bnr.mon_widget) &&
 			Bnr.mon_widget != Bnr.dp_id)
 			{
 			    BannerMonRefresh ();
 			    Bnr.ws_purged = 0;
 			break;
 			}
 		    if (BannerEvent.xany.window == XtWindow(Bnr.process_widget) &&
 			Bnr.process_widget != Bnr.dp_id)
 			{
 			    BannerProcessRefresh ();
 			    Bnr.ws_purged = 0;
 			break;
 			}
 		    if (BannerEvent.xany.window == XtWindow(Bnr.cube_widget) &&
 			Bnr.cube_widget != Bnr.dp_id)
 			{
 			if (Bnr.Param_Bits.world)
 			    BannerGlobe (0, 0, Bnr.cube_width, 
 				Bnr.cube_height, 1);
 			if (Bnr.Param_Bits.cube)
 			    BannerCubeRefresh ();
 			if (Bnr.Param_Bits.hanoi)
 			    BannerHanoiRefresh ();
 			Bnr.ws_purged = 0;
 			break;
 			}
 		    if (BannerEvent.xany.window == XtWindow(Bnr.clock_widget) &&
 			    Bnr.clock_widget != Bnr.dp_id)
 			{
 			if (Bnr.Param_Bits.analog_clock)
 			    BannerClock (
 				0,
 				0,
 				Bnr.clock_width,
 				Bnr.clock_height);
 /*
  * If a digital clock, then write the current day/date
  */
 			if (Bnr.Param_Bits.digital_clock)
 			    {
 			    BannerWriteDate (XtDisplay(Bnr.clock_widget), 
 				XtWindow(Bnr.clock_widget));
 			    BannerDigitalClock (XtDisplay(Bnr.clock_widget), 
 				XtWindow(Bnr.clock_widget));    	
 			    }
 			Bnr.ws_purged = 0;
 			break;
 			}
 		    if (BannerEvent.xany.window == XtWindow(Bnr.main_widget) &&
 			Bnr.main_widget != Bnr.dp_id &&
 			Bnr.top_widget == Bnr.dp_id)
 			{
 			BannerWriteText (XtDisplay (Bnr.main_widget),
 			    XtWindow (Bnr.main_widget),
 			    0,
 			    Bnr.font_height,
 			    "DECwindows Banner",
 			    2,
 			    Bnr.font_width*20);
 			BannerWriteText (XtDisplay (Bnr.main_widget),
 			    XtWindow (Bnr.main_widget),
 			    0,
 			    Bnr.font_height * 2,
 			    "Demo Program",
 			    2,
 			    Bnr.font_width*20);
 			}
 		    }
 		break;
 		}
 	    case VisibilityNotify :
 		{
 /*
  * if we have become visible, we had better repaint all the constant 
  * info on the screen.
  */
 		if (BannerEvent.xvisibility.state == VisibilityUnobscured)
 		    {
 		    if (BannerEvent.xany.window == XtWindow(Bnr.cpu_widget) &&
 			Bnr.cpu_widget != Bnr.dp_id)
 			{
 			if (Bnr.Param_Bits.cpu_window)
 			    BannerCpuRefresh ();
 			Bnr.ws_purged = 0;
 			break;
 			}
 		    if (BannerEvent.xany.window == XtWindow(Bnr.pe_widget) &&
 			Bnr.pe_widget != Bnr.dp_id)
 			{
 			if (Bnr.Param_Bits.pe_window)
 			    BannerPERefresh ();
 			Bnr.ws_purged = 0;
 			break;
 			}
 		    if (BannerEvent.xany.window == XtWindow(Bnr.lck_widget) &&
 			Bnr.lck_widget != Bnr.dp_id)
 			{
 			if (Bnr.Param_Bits.lck_window)
 			    BannerLCKRefresh ();
 			Bnr.ws_purged = 0;
 			break;
 			}
 		    if (BannerEvent.xany.window == XtWindow(Bnr.cwps_widget) &&
 			Bnr.cwps_widget != Bnr.dp_id)
 			{
 			if (Bnr.Param_Bits.cwps_window)
 			    BannerCWPSRefresh ();
 			Bnr.ws_purged = 0;
 			break;
 			}
 		    if (BannerEvent.xany.window == XtWindow(Bnr.mon_widget) &&
 			Bnr.mon_widget != Bnr.dp_id)
 			{
 			if (Bnr.Param_Bits.monitor)
 			    BannerMonRefresh ();
 			Bnr.ws_purged = 0;
 			break;
 			}
 		    if (BannerEvent.xany.window == XtWindow(Bnr.process_widget) &&
 			Bnr.process_widget != Bnr.dp_id)
 			{
 			if (Bnr.Param_Bits.process_window)
 			    BannerProcessRefresh ();
 			Bnr.ws_purged = 0;
 			break;
 			}
 		    if (BannerEvent.xany.window == XtWindow(Bnr.cube_widget) &&
 			Bnr.cube_widget != Bnr.dp_id)
 			{
 			if (Bnr.Param_Bits.world)
 			    BannerGlobe (0, 0, Bnr.cube_width, 
 				Bnr.cube_height, 1);
 
 			if (Bnr.Param_Bits.cube)
 			    BannerCubeRefresh ();
 
 			if (Bnr.Param_Bits.hanoi)
 			    BannerHanoiRefresh ();
 			Bnr.ws_purged = 0;
 			break;
 			}
 
 		    if (BannerEvent.xany.window == XtWindow(Bnr.clock_widget) &&
 			Bnr.clock_widget != Bnr.dp_id)
 			{
 			if (Bnr.Param_Bits.analog_clock)
 			    BannerClock (
 				0,
 				0,
 				Bnr.clock_width,
 				Bnr.clock_height);
 /*
  * If a digital clock, then write the current day/date
  */
 			if (Bnr.Param_Bits.digital_clock)
 			    {
 			    BannerWriteDate (XtDisplay(Bnr.clock_widget), 
 				XtWindow(Bnr.clock_widget));
 			    BannerDigitalClock (XtDisplay(Bnr.clock_widget), 
 				XtWindow(Bnr.clock_widget));    	
 			    }
 			Bnr.ws_purged = 0;
 			break;
 			}
 		    if (BannerEvent.xany.window == XtWindow(Bnr.main_widget) &&
 			Bnr.main_widget != Bnr.dp_id &&
 			Bnr.top_widget == Bnr.dp_id)
 			{
 			BannerWriteText (XtDisplay (Bnr.main_widget),
 			    XtWindow (Bnr.main_widget),
 			    0,
 			    Bnr.font_height,
 			    "DECwindows Banner",
 			    2,
 			    Bnr.font_width*20);
 			BannerWriteText (XtDisplay (Bnr.main_widget),
 			    XtWindow (Bnr.main_widget),
 			    0,
 			    Bnr.font_height * 2,
 			    "Demo Program",
 			    2,
 			    Bnr.font_width*20);
 			}
 		    }
 		break;
 		}
 	    case ConfigureNotify :
 		{
 
 /*
  * If we have been resized, then capture our new size, and issue a 
  * clear window. This will cause a full expose event, and we will repaint
  * with our new size.
  */
 		if (BannerEvent.xany.window == XtWindow(Bnr.cube_widget) &&
 		    Bnr.cube_widget != Bnr.dp_id)
 		    {
 		    Bnr.cube_width = BannerEvent.xconfigure.width;
 		    Bnr.cube_height = BannerEvent.xconfigure.height;
 		    if (Bnr.Param_Bits.cube)
 			BannerCubeSetup ();
 		    if (Bnr.Param_Bits.hanoi)
 			BannerHanoiSetup ();
 		    XClearWindow (BannerEvent.xany.display, 
 			BannerEvent.xany.window);
 		    Bnr.ws_purged = 0;
 		    break;
 		    }
 		if (BannerEvent.xany.window == XtWindow(Bnr.cpu_widget) &&
 		    Bnr.cpu_widget != Bnr.dp_id)
 		    {
 		    Bnr.cpu_width = BannerEvent.xconfigure.width;
 		    Bnr.cpu_height = BannerEvent.xconfigure.height;
 		    BannerCpuSetup ();
 		    XClearWindow (BannerEvent.xany.display, 
 			BannerEvent.xany.window);
 		    Bnr.ws_purged = 0;
 		    break;
 		    }
 		if (BannerEvent.xany.window == XtWindow(Bnr.pe_widget) &&
 		    Bnr.pe_widget != Bnr.dp_id)
 		    {
 		    Bnr.pe_width = BannerEvent.xconfigure.width;
 		    Bnr.pe_height = BannerEvent.xconfigure.height;
 		    BannerPESetup ();
 		    XClearWindow (BannerEvent.xany.display, 
 			BannerEvent.xany.window);
 		    Bnr.ws_purged = 0;
 		    break;
 		    }
 		if (BannerEvent.xany.window == XtWindow(Bnr.lck_widget) &&
 		    Bnr.lck_widget != Bnr.dp_id)
 		    {
 		    Bnr.lck_width = BannerEvent.xconfigure.width;
 		    Bnr.lck_height = BannerEvent.xconfigure.height;
 		    BannerLCKSetup ();
 
 		    XClearWindow (BannerEvent.xany.display, 
 			BannerEvent.xany.window);
 		    Bnr.ws_purged = 0;
 		    break;
 		    }
 		if (BannerEvent.xany.window == XtWindow(Bnr.cwps_widget) &&
 		    Bnr.cwps_widget != Bnr.dp_id)
 		    {
 		    Bnr.cwps_width = BannerEvent.xconfigure.width;
 		    Bnr.cwps_height = BannerEvent.xconfigure.height;
 		    BannerCWPSSetup ();
 		    XClearWindow (BannerEvent.xany.display, 
 			BannerEvent.xany.window);
 		    Bnr.ws_purged = 0;
 		    break;
 		    }
 		if (BannerEvent.xany.window == XtWindow(Bnr.mon_widget) &&
 		    Bnr.mon_widget != Bnr.dp_id)
 		    {
 		    Bnr.mon_width = BannerEvent.xconfigure.width;
 		    Bnr.mon_height = BannerEvent.xconfigure.height;
 		    BannerMonSetup ();
 		    XClearWindow (BannerEvent.xany.display, 
 			BannerEvent.xany.window);
 		    Bnr.ws_purged = 0;
 		    break;
 		    }
 		if (BannerEvent.xany.window == XtWindow(Bnr.process_widget) &&
 		    Bnr.process_widget != Bnr.dp_id)
 		    {
 		    Bnr.process_width = BannerEvent.xconfigure.width;
 		    Bnr.process_height = BannerEvent.xconfigure.height;
 		    BannerProcessSetup ();
 		    XClearWindow (BannerEvent.xany.display, 
 			BannerEvent.xany.window);
 		    Bnr.ws_purged = 0;
 		    break;
 		    }
 
 		if (BannerEvent.xany.window == XtWindow(Bnr.clock_widget) &&
 		    Bnr.clock_widget != Bnr.dp_id)
 		    {
 		    Bnr.clock_width = BannerEvent.xconfigure.width;
 		    Bnr.clock_height = BannerEvent.xconfigure.height;
 		    XClearWindow (BannerEvent.xany.display, 
 			BannerEvent.xany.window);
 		    Bnr.ws_purged = 0;
 		    break;
 		    }
 
 		if (BannerEvent.xany.window == XtWindow(Bnr.message_widget) &&
 		    Bnr.message_widget != Bnr.dp_id)
 		    {
 		    Bnr.message_width = BannerEvent.xconfigure.width;
 		    Bnr.message_height = BannerEvent.xconfigure.height;
 		    Bnr.ws_purged = 0;
 		    break;
 		    }
 
 		}
 	    }
 
 	/*
 	 * Now if there are no more events we can do our work.
 	 */
 	if (XtPending() == 0)
 	    {
 	    /*
 	     * This is our chance, so now we can call our routine
 	     * to write our window.
 	     */
 	    if (Bnr.current_priority != Bnr.execute_priority)
 		{
 		SYS$SETPRI (NULL, NULL, Bnr.execute_priority, NULL);
 		Bnr.current_priority = Bnr.execute_priority;
 		}
 /*
  * If Hanoi, or Cube, then goto a tight loop, otherwise just run our process
  * cycle for this second tick!
  */
 	    if (Bnr.Param_Bits.cube || Bnr.Param_Bits.hanoi)
 		while (XtPending() == 0)
 		    {
 		    BannerUpdateWindows();
 		    XFlush (XtDisplay(Bnr.dp_id));
 		    }
 	    else
 		BannerUpdateWindows();
 	    }
 	}
 }
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
*3E
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
*3D
 **  FUNCTIONAL DESCRIPTION:
 **
 **	BannerSetArg (w, resource, value)
 **
 **      A simplified set value routine. This allows use to set simple resources 
 **	values on any widget we desire.
 **
 **  FORMAL PARAMETERS:
 **
 **      w		    Widget to have value set on
 **	resource	    Widget Resource to set
 **	value		    value to set resource to
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
 BannerSetArg(w, resource, value)
     Widget w;
     char *resource;
     int value;
 {
     Arg al[10];
 
     XtSetArg (al[0], resource, value);
     XtSetValues (w, al, 1);
 }
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **	BannerGetArg (w, resource, value)
 **
 **      A simplified get value routine. This allows use to get simple resources 
 **	values on any widget we desire.
 **
 **  FORMAL PARAMETERS:
 **
 **      w		    Widget to have value set on
 **	resource	    Widget Resource to get
 **	value		    value to recieve
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
 BannerGetArg(w, resource, value)
     Widget w;
     char *resource;
     int value;
 {
     Arg al[10];
 
     XtSetArg (al[0], resource, value);
     XtGetValues (w, al, 1);
 }
 
 /*
 **++
*3E
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
*7D
 		(unsigned long) BlackPixel (dpy, 0),
 		(unsigned long) WhitePixel (dpy, 0),
 		(unsigned int) DefaultDepth (dpy, 0));
*7E
*7I
 		(unsigned long) BlackPixel (dpy, 
 			XDefaultScreen(XtDisplay(Bnr.main_widget))),
 		(unsigned long) WhitePixel (dpy, 
 			XDefaultScreen(XtDisplay(Bnr.main_widget))),
 		(unsigned int) DefaultDepth (dpy, 
 			XDefaultScreen(XtDisplay(Bnr.main_widget))));
*7E
     return(pid);
 }
