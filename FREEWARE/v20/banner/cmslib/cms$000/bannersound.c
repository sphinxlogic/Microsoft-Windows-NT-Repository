 #module BannerSound "V1.0"
 
 /*
 **++
 **  FACILITY:  
 **  
 **  This module contains all the interface support for Sound generation within
 **  the DECwindows Banner program. Sound Support is currently used for state
 **  excepts, such as processes going into MWAIT, and varous system resources
 **  reaching critical levels.
 **
 **  MODULE DESCRIPTION:
 **
 **      As Above
 **
 **  AUTHORS:
 **
 **      Jim Saunders
 **
 **  CREATION DATE:  28th December 1990
 **
 **  DESIGN ISSUES:
 **
 **      None
 **
 **
 **  MODIFICATION HISTORY:
 **
 **      
 **	Initial Version
 **--
 */
 #include "stdio.h"
 #include "lnmdef.h"
 #include <Mrm/MrmAppl.h>
*3I
 #include <Xt/vendor.h>
*3E
 #include <Xm/Shell.h>
*3D
 #include <Xt/vendor.h>
*3E
 
 #include "Banner"
 
 
*3D
 
*3E
*3I
 void BannerSoundInitialize ();
 void BannerSoundSelect ();
 void BannerSoundCallback ();
 void BannerSoundInitCallback ();
 void BannerSound ();
 void BannerSetSound ();
 static void BuildSoundMenus ();
 static void WidgetToSound ();
*3E
 /*
 **
 **  MACRO DEFINITIONS
 **
 */
 
 extern	 int	     BannerInitialPriority;
 extern	 Bnr$_Blk    Bnr;
 extern	 Clk$_Blk    Clk;
 extern	 Mon$_Blk    Mon;
 extern	 GC	     BannerGC;
 extern	 XEvent	     BannerEvent;
 
*3D
 #define BANNER_SOUND_MENU_CREATE    1
 
*3E
*3I
 	 Son$_Blk    Son;
 	 Son$_Blk    NewSon;
 
 static	 Sound$_Item  Test;
 static	 Sound$_Item  *TestSound = &Test;
 
 #define BANNER_SOUND_MENU_CREATE    1001
 
 #define BANNER_SOUND_OK_BUTTON		1
 #define BANNER_SOUND_CANCEL_BUTTON	2
 #define BANNER_SOUND_VOLUME		3
 #define BANNER_SOUND_TIME		4
 #define BANNER_SOUND_MWAIT		5
 #define BANNER_SOUND_MWAIT_PUSH		6
 #define BANNER_SOUND_NEW_PROCESSES	7
 #define BANNER_SOUND_NEW_PROCESSES_PUSH 8
 #define BANNER_LESS_PROCESSES		9
 #define BANNER_LESS_PROCESSES_PUSH	10
 #define BANNER_SOUND_DISK_1		11
 #define BANNER_SOUND_DISK_1_PUSH	12
 #define BANNER_SOUND_DISK_2		13
 #define BANNER_SOUND_DISK_2_PUSH	14
 #define BANNER_SOUND_DISK_3		15
 #define BANNER_SOUND_DISK_3_PUSH	16
 #define BANNER_SOUND_MEMORY		17
 #define BANNER_SOUND_MEMORY_PUSH	18
 #define BANNER_SOUND_PAGE_FILE		19
 #define BANNER_SOUND_PAGE_FILE_PUSH	20
 #define BANNER_SOUND_PENDING_IOS	21
 #define BANNER_SOUND_PENDING_IOS_PUSH	22
 
 #define BANNER_SOUND_MAXWIDGETS 23
 
 static int  SoundWidgets[BANNER_SOUND_MAXWIDGETS];
 static int  *AvailableSounds = NULL;
 static int  *AvailableSoundsSpecs = NULL;
*3E
*2D
 static int  BannerSoundAvailable = 0;
*2E
*2I
*5D
 static int  BannerSoundAvailable = 1;
*5E
*5I
 static int  BannerSoundAvailable;
*5E
*2E
*3D
 
*3E
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **      BannerSoundInitialize ()
 **
 **	Performs all once only initialization for the sound subsystem.
 **
 **  FORMAL PARAMETERS:
 **
 **      None
 **
 **  RETURN VALUE:
 **
 **      None
 **
 **  SIDE EFFECTS:
 **
 **      None
 **
 **  DESIGN:
 **
 **      {@description or none@}
 **
 **
 **--
 */
 void BannerSoundInitialize ()
 {
*3D
 }
 
*3E
*3I
 /*
  * Get a list of all the sounds available
  */
     memset (&Son, 0, sizeof (Son));
     memset (&NewSon, 0, sizeof (NewSon));
 
     AvailableSounds = XtMalloc (20*sizeof(int));
     AvailableSoundsSpecs = XtMalloc (20*sizeof(int));
     AvailableSounds[0] = "Mwait Sound";
     AvailableSoundsSpecs[0] = NULL;
     Test.On = 1;
     Test.DisplayName = AvailableSounds[0];
     Test.Sound = AvailableSoundsSpecs[0];
     AvailableSounds[1] = "New Processes Sound";
     AvailableSoundsSpecs[1] = NULL;
     AvailableSounds[2] = "Less Processes Sound";
     AvailableSoundsSpecs[2] = NULL;
     AvailableSounds[3] = "Disk1 Sound";
     AvailableSoundsSpecs[3] = NULL;
     AvailableSounds[4] = "Disk2 Sound";
     AvailableSoundsSpecs[4] = NULL;
     AvailableSounds[5] = "Disk3 Sound";
     AvailableSoundsSpecs[5] = NULL;
     AvailableSounds[6] = "Memory Sound";
     AvailableSoundsSpecs[6] = NULL;
     AvailableSounds[7] = "Penidnhg IO Sound";
     AvailableSoundsSpecs[7] = NULL;
     AvailableSounds[8] = "PageFile Sound";
     AvailableSoundsSpecs[8] = NULL;
     AvailableSounds[9] = NULL;
     AvailableSoundsSpecs[9] = NULL;
 /*
  * Find out if sound can be used on this machine
  */
*5D
     BannerSoundAvailable = 1;
*5E
*5I
     BannerSoundAvailable = 0;
*5E
 }
*3E
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
*2D
 **      BannerSoundSelect ()
*2E
*2I
 **      BannerSoundSelect (savewidget)
*2E
 **
 **	This routine creates, and initializes the dialog box for sound 
 **  selection. The dialog box allows the user select specific sounds for 
 **  specific functions, and establish which sounds should be active.
 **
 **  FORMAL PARAMETERS:
 **
 **      None
 **
 **  RETURN VALUE:
 **
 **      None
 **
 **  SIDE EFFECTS:
 **
 **      None
 **
 **  DESIGN:
 **
 **      {@description or none@}
 **
 **
 **--
 */
*2D
 void BannerSoundSelect ()
*2E
*2I
 void BannerSoundSelect (savewidget)
 Widget savewidget;
*2E
 {
*2D
 }
 
*2E
*2I
 int class, width, height;
 	
     if (Bnr.sound_widget != 0)
 	{
 	XtDestroyWidget (Bnr.sound_widget);
 /*
  * Now syncronize with the Server
  */
 	XSync (XtDisplay (Bnr.dp_id), 0);
 	Bnr.sound_widget=0;
 	}
 
     MrmFetchWidget (Bnr.drm_prt,
 	"BANNER_SOUND_OPTIONS",
 	savewidget,
 	&Bnr.sound_widget,
 	&class);
 
     XtManageChild (Bnr.sound_widget);
     XtRealizeWidget (Bnr.sound_widget);    
 }
*2E
*3D
 
*3E
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **      BannerSoundCallback (widget, tag)
 **
 **  The callback routine for the varous functions for the sound interface.
 **
 **  FORMAL PARAMETERS:
 **
 **      As Above
 **
 **  RETURN VALUE:
 **
 **      None
 **
 **  SIDE EFFECTS:
 **
 **      None
 **
 **  DESIGN:
 **
 **      {@description or none@}
 **
 **
 **--
 */
 void BannerSoundCallback (widget, tag)
 Widget widget;
 int *tag;
 {
*3I
 Arg		al[10];
 
*3E
     switch (*tag)
     {
*2I
*3D
     }
 }
 
 
*3E
*3I
     case BANNER_SOUND_OK_BUTTON:
 	{
 	memcpy (&Son, &NewSon, sizeof(Son));
 	TestSound = &Test;
 	break;
 	}
     case BANNER_SOUND_CANCEL_BUTTON :
 	{
 	memcpy (&NewSon, &Son, sizeof(Son));
 	break;
 	}
     case BANNER_SOUND_VOLUME :
 	{
 	XtSetArg (al[0], XmNvalue, &NewSon.Volume);
 	XtGetValues (widget, al, 1);
 	BannerSound (TestSound, NewSon.Volume, NewSon.Time);
 	break;
 	}
     case BANNER_SOUND_TIME :
 	{
 	XtSetArg (al[0], XmNvalue, &NewSon.Time);
 	XtGetValues (widget, al, 1);
 	BannerSound (TestSound, NewSon.Volume, NewSon.Time);
 	break;
 	}
     case BANNER_SOUND_MWAIT :
 	{
 	WidgetToSound (widget, &NewSon.Mwait);
 	BannerSound (&NewSon.Mwait, NewSon.Volume, NewSon.Time);
 	TestSound = &NewSon.Mwait;
 	break;
 	}
     case BANNER_SOUND_NEW_PROCESSES :
 	{
 	WidgetToSound (widget, &NewSon.NewProcesses);
 	BannerSound (&NewSon.NewProcesses, NewSon.Volume, NewSon.Time);
 	TestSound = &NewSon.NewProcesses;	
 	break;
 	}
     case BANNER_LESS_PROCESSES :
 	{
 	WidgetToSound (widget, &NewSon.LessProcesses);
 	BannerSound (&NewSon.LessProcesses, NewSon.Volume, NewSon.Time);
 	TestSound = &NewSon.LessProcesses;
         break;
         }
     case BANNER_SOUND_DISK_1 :
 	{
 	WidgetToSound (widget, &NewSon.Disk1);
 	BannerSound (&NewSon.Disk1, NewSon.Volume, NewSon.Time);
 	TestSound = &NewSon.Disk1;
         break;
         }
     case BANNER_SOUND_DISK_2 :
 	{
 	WidgetToSound (widget, &NewSon.Disk2);
 	BannerSound (&NewSon.Disk2, NewSon.Volume, NewSon.Time);
 	TestSound = &NewSon.Disk2;
         break;
         }
     case BANNER_SOUND_DISK_3 :
 	{
 	WidgetToSound (widget, &NewSon.Disk3);
 	BannerSound (&NewSon.Disk3, NewSon.Volume, NewSon.Time);
 	TestSound = &NewSon.Disk3;
         break;
         }
     case BANNER_SOUND_MEMORY :
 	{
 	WidgetToSound (widget, &NewSon.Memory);
 	BannerSound (&NewSon.Memory, NewSon.Volume, NewSon.Time);
 	TestSound = &NewSon.Memory;
 	break;
 	}
     case BANNER_SOUND_PAGE_FILE :
 	{
 	WidgetToSound (widget, &NewSon.PageFile);
 	BannerSound (&NewSon.PageFile, NewSon.Volume, NewSon.Time);
 	TestSound = &NewSon.PageFile;
 	break;
 	}
     case BANNER_SOUND_PENDING_IOS :
 	{
 	WidgetToSound (widget, &NewSon.PendingIO);
 	BannerSound (&NewSon.PendingIO, NewSon.Volume, NewSon.Time);
 	TestSound = &NewSon.PendingIO;
 	break;
 	}
     }
 }
*3E
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **      BannerSoundInitCallback (widget, tag)
 **
 **  The callback routine for the varous functions for the sound interface.
 **
 **  FORMAL PARAMETERS:
 **
 **      As Above
 **
 **  RETURN VALUE:
 **
 **      None
 **
 **  SIDE EFFECTS:
 **
 **      None
 **
 **  DESIGN:
 **
 **      {@description or none@}
 **
 **
 **--
 */
 void BannerSoundInitCallback (widget, tag)
 Widget widget;
 int *tag;
 {
*3I
 Arg		al[10];
 
*3E
     switch (*tag)
     {
*2E
     case BANNER_SOUND_MENU_CREATE:
 	{
 /*
  * Set the menu sensitivity according to if sound is supported or not 
  * on this machine
  */
 	BannerSetArg(widget, 
 	    XmNsensitive,
 	    BannerSoundAvailable);
 	break;
 	}
*3I
     case BANNER_SOUND_OK_BUTTON:
 	{
 	SoundWidgets[BANNER_SOUND_OK_BUTTON] = widget;
 	memcpy (&NewSon, &Son, sizeof(Son));
 	break;
 	}
     case BANNER_SOUND_CANCEL_BUTTON :
 	{
 	SoundWidgets[BANNER_SOUND_CANCEL_BUTTON] = widget;
 	break;
 	}
     case BANNER_SOUND_VOLUME :
 	{
 	SoundWidgets[BANNER_SOUND_VOLUME] = widget;
 	XtSetArg (al[0], XmNvalue, NewSon.Volume);
 	XtSetValues (widget, al, 1);
 	break;
 	}
     case BANNER_SOUND_TIME :
 	{
 	SoundWidgets[BANNER_SOUND_TIME] = widget;
 	XtSetArg (al[0], XmNvalue, NewSon.Time);
 	XtSetValues (widget, al, 1);
 	break;
 	}
     case BANNER_SOUND_MWAIT_PUSH :
 	{
 	SoundWidgets[BANNER_SOUND_MWAIT_PUSH] = widget;
 	break;
 	}
     case BANNER_SOUND_MWAIT :
 	{
 	SoundWidgets[BANNER_SOUND_MWAIT] = widget;
 	BuildSoundMenus (widget,
 	    SoundWidgets[BANNER_SOUND_MWAIT_PUSH], 
 	    BANNER_SOUND_MWAIT,
 	    &NewSon.Mwait);
 	if (Bnr.Param_Bits.process_window == 0)
 	    BannerSetArg (widget, XmNsensitive, 0);
 	break;
 	}
     case BANNER_SOUND_NEW_PROCESSES_PUSH :
 	{
 	SoundWidgets[BANNER_SOUND_NEW_PROCESSES_PUSH] = widget;
 	break;
 	}
     case BANNER_SOUND_NEW_PROCESSES :
 	{
 	SoundWidgets[BANNER_SOUND_NEW_PROCESSES] = widget;
 	BuildSoundMenus (widget,
 	    SoundWidgets[BANNER_SOUND_NEW_PROCESSES_PUSH],
 	    BANNER_SOUND_NEW_PROCESSES,
 	    &NewSon.NewProcesses);
 	if (Bnr.Param_Bits.process_window == 0)
 	    BannerSetArg (widget, XmNsensitive, 0);
 	break;
 	}
     case BANNER_LESS_PROCESSES_PUSH :
 	{
 	SoundWidgets[BANNER_LESS_PROCESSES_PUSH] = widget;
 	break;
 	}
     case BANNER_LESS_PROCESSES :
 	{
 	SoundWidgets[BANNER_LESS_PROCESSES] = widget;
 	BuildSoundMenus (widget,
 	    SoundWidgets[BANNER_LESS_PROCESSES_PUSH],
 	    BANNER_LESS_PROCESSES,
 	    &NewSon.LessProcesses);
 	if (Bnr.Param_Bits.process_window == 0)
 	    BannerSetArg (widget, XmNsensitive, 0);
 	break;
 	}
     case BANNER_SOUND_DISK_1_PUSH :
 	{
 	SoundWidgets[BANNER_SOUND_DISK_1_PUSH] = widget;
 	break;
 	}
     case BANNER_SOUND_DISK_1 :
 	{
 	SoundWidgets[BANNER_SOUND_DISK_1] = widget;
 	BuildSoundMenus (widget,
 	    SoundWidgets[BANNER_SOUND_DISK_1_PUSH],
 	    BANNER_SOUND_DISK_1,
 	    &NewSon.Disk1);
 	if (!Bnr.Param_Bits.sys_window)
 	    BannerSetArg(widget, 
 		XmNsensitive,
 		0);
 	break;
 	}
     case BANNER_SOUND_DISK_2_PUSH :
 	{
 	SoundWidgets[BANNER_SOUND_DISK_2_PUSH] = widget;
 	break;
 	}
     case BANNER_SOUND_DISK_2 :
 	{
 	SoundWidgets[BANNER_SOUND_DISK_2] = widget;
 	BuildSoundMenus (widget,
 	    SoundWidgets[BANNER_SOUND_DISK_2_PUSH],
 	    BANNER_SOUND_DISK_2,
 	    &NewSon.Disk2);
 	if (!Bnr.Param_Bits.sys_window)
 	    BannerSetArg(widget, 
 		XmNsensitive,
 		0);
 	break;
 	}
     case BANNER_SOUND_DISK_3_PUSH :
 	{
 	SoundWidgets[BANNER_SOUND_DISK_3_PUSH] = widget;
 	break;
 	}
     case BANNER_SOUND_DISK_3 :
 	{
 	SoundWidgets[BANNER_SOUND_DISK_3] = widget;
 	BuildSoundMenus (widget,
 	    SoundWidgets[BANNER_SOUND_DISK_3_PUSH],
 	    BANNER_SOUND_DISK_3,
 	    &NewSon.Disk3);
 	if ( !Bnr.Param_Bits.sys_window)
 	    BannerSetArg(widget, 
 		XmNsensitive,
 		0);
 	break;
 	}
     case BANNER_SOUND_MEMORY_PUSH :
 	{
 	SoundWidgets[BANNER_SOUND_MEMORY_PUSH] = widget;
 	break;
 	}
     case BANNER_SOUND_MEMORY :
 	{
 	SoundWidgets[BANNER_SOUND_MEMORY] = widget;
 	BuildSoundMenus (widget,
 	    SoundWidgets[BANNER_SOUND_MEMORY_PUSH],
 	    BANNER_SOUND_MEMORY,
 	    &NewSon.Memory);
 	if ( !Bnr.Param_Bits.sys_window)
 	    BannerSetArg(widget, 
 		XmNsensitive,
 		0);
 	break;
 	}
     case BANNER_SOUND_PAGE_FILE_PUSH :
 	{
 	SoundWidgets[BANNER_SOUND_PAGE_FILE_PUSH] = widget;
 	break;
 	}
     case BANNER_SOUND_PAGE_FILE :
 	{
 	SoundWidgets[BANNER_SOUND_PAGE_FILE] = widget;
 	BuildSoundMenus (widget,
 	    SoundWidgets[BANNER_SOUND_PAGE_FILE_PUSH],
 	    BANNER_SOUND_PAGE_FILE,
 	    &NewSon.PageFile);
 	if ( !Bnr.Param_Bits.sys_window)
 	    BannerSetArg(widget, 
 		XmNsensitive,
 		0);
 	break;
 	}
     case BANNER_SOUND_PENDING_IOS_PUSH :
 	{
 	SoundWidgets[BANNER_SOUND_PENDING_IOS_PUSH] = widget;
 	break;
 	}
     case BANNER_SOUND_PENDING_IOS :
 	{
 	SoundWidgets[BANNER_SOUND_PENDING_IOS] = widget;
 	BuildSoundMenus (widget,
 	    SoundWidgets[BANNER_SOUND_PENDING_IOS_PUSH],
 	    BANNER_SOUND_PENDING_IOS,
 	    &NewSon.PendingIO);
 	if ( !Bnr.Param_Bits.sys_window)
 	    BannerSetArg(widget, 
 		XmNsensitive,
 		0);
 	break;
 	}
*3E
     }
 }
 
*2I
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
*3D
 **      BannerSound (SoundCode)
*3E
*3I
 **      WidgetToSound (widget, soundblock)
 **
 **  This routine maps the the name of the current pushbutton, into a 
 **  sound block.
 **
 **  FORMAL PARAMETERS:
 **
 **      {@subtags@}
 **
 **  RETURN VALUE:
 **
 **      {@description or none@}
 **
 **  SIDE EFFECTS:
 **
 **      {@description or none@}
 **
 **  DESIGN:
 **
 **      {@description or none@}
 **
 **  [@logical properties@]...
 **
 **  [@optional function tags@]...
 **
 **--
 */
 static void WidgetToSound (widget, soundblock)
 Widget widget;
 Sound$_Item *soundblock;
 {
 Arg		al[10];
 char *name;
 
     XtSetArg (al[0], XmNuserData, &name); 
     XtGetValues (widget, al, 1);
     BannerSetSound (soundblock, name);
 }
 
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **      BannerSetSound (soundblock, soundname)
 **
 **  This routine maps a sound name, into a specific sound block.
 **
 **  FORMAL PARAMETERS:
 **
 **      {@subtags@}
 **
 **  RETURN VALUE:
 **
 **      {@description or none@}
 **
 **  SIDE EFFECTS:
 **
 **      {@description or none@}
 **
 **  DESIGN:
 **
 **      {@description or none@}
 **
 **  [@logical properties@]...
 **
 **  [@optional function tags@]...
 **
 **--
 */
 void BannerSetSound (soundblock, soundname)
 Sound$_Item *soundblock;
 char	    *soundname;
 {
 
     soundblock->On = 0;
     soundblock->DisplayName = NULL;
     soundblock->Sound = NULL;
 
     if (soundname != NULL)
 	{
 	int i;
 
 	i = 0;
 	while (AvailableSounds != NULL && AvailableSounds[i] != NULL)
 	    {
 	    if (strcmp (soundname, AvailableSounds[i]) == 0)
 		{
 		soundblock->On = 1;
 		soundblock->DisplayName = AvailableSounds[i];
 		soundblock->Sound = AvailableSoundsSpecs[i];		
 		}
 	    i++;
 	    }
 	}
 }
 
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **      BuildSoundMenus	(widget, pushbutton, menucode, soundblock)
 **
 **	Build the dynamic sound otpion menu's
 **
 **  FORMAL PARAMETERS:
 **
 **      {@subtags@}
 **
 **  RETURN VALUE:
 **
 **      {@description or none@}
 **
 **  SIDE EFFECTS:
 **
 **      {@description or none@}
 **
 **  DESIGN:
 **
 **      {@description or none@}
 **
 **  [@logical properties@]...
 **
 **  [@optional function tags@]...
 **
 **--
 */
 static void BuildSoundMenus (menu, PushButton, menucode, soundblock)
 CompositeWidget	menu;
 Widget	PushButton;
 int	menucode;
 Sound$_Item	*soundblock;
 {
 int		i, ac;
 int		*tag;
 Arg		al[10];
 Widget		Push, MenuHistory;
 WidgetList	childlist;
 XtCallbackRec	activatecallback[2];
 /*
  * Setup this push button as the 'No sound' button
  */
 
     ac = 0;
     XtSetArg (al[ac], XmNlabelString, XmStringCreate ("Sound Off", 
 	XmSTRING_DEFAULT_CHARSET)); ac++;
     XtSetArg (al[ac], XmNuserData, NULL); ac++;
     activatecallback[0].callback = BannerSoundCallback;
     tag = XtMalloc (sizeof(int));
     *tag = menucode;
     activatecallback[0].closure = tag;
     activatecallback[1].callback = NULL;
     activatecallback[1].closure = 0;
     XtSetArg (al[ac], XmNactivateCallback, activatecallback); ac++;
     XtSetValues (PushButton, al, ac);
     MenuHistory = PushButton;
 /*
  * Now add the addition buttons, necessary for the different sounds
  */
     i = 0;
     while (AvailableSounds != NULL && AvailableSounds[i] != NULL)
 	{
 	/*
 	 * Create a push button
 	 */
 	ac = 0;
 	XtSetArg (al[ac], XmNlabelString, XmStringCreate (AvailableSounds[i], 
 	    XmSTRING_DEFAULT_CHARSET)); ac++;
 	XtSetArg (al[ac], XmNuserData, AvailableSounds[i]); ac++;
 	activatecallback[0].callback = BannerSoundCallback;
 	tag = XtMalloc (sizeof(int));
 	*tag = menucode;
 	activatecallback[0].closure = tag;
 	activatecallback[1].callback = NULL;
 	activatecallback[1].closure = 0;
 	XtSetArg (al[ac], XmNactivateCallback, activatecallback); ac++;
 	Push = XmCreatePushButton (XtParent(PushButton), 
 	    "SoundPushbutton", al, ac);
 	XtManageChild (Push);
 	if (soundblock->DisplayName != NULL &&
 	    strcmp (soundblock->DisplayName, AvailableSounds[i]) == 0)
 	    MenuHistory = Push;
 
 	i++;
 	}
 
 /*
  * Set the menu History to show the current selected sound
  */
     ac = 0;
     XtSetArg (al[ac], XmNmenuHistory, 
 	MenuHistory); ac++;
 
     XtSetValues (menu, al, ac);
 }
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **      BannerSound (SoundCode, Volume, Time)
*3E
 **
 **	This routine generates the actual sound, according to a specific
 **  sound code. The sound code is used as an index into the active sound 
 **  tables, and if the specified sound is available, it is generated.
 **
 **  FORMAL PARAMETERS:
 **
 **      As Above
 **
 **  RETURN VALUE:
 **
 **      None
 **
 **  SIDE EFFECTS:
 **
 **      None
 **
 **  DESIGN:
 **
 **      {@description or none@}
 **
 **
 **--
 */
*3D
 void BannerSound (SoundCode)
 int SoundCode;
*3E
*3I
 void BannerSound (SoundCode, Volume, Lenth)
 Sound$_Item	*SoundCode;
 int		Volume;
 int		Lenth;
*3E
 {
*3I
 if (SoundCode->On)
     printf ("Sounding '%s', Time - %d, Volume - %d\n", SoundCode->DisplayName,
 	    Lenth, Volume);
*4D
 else
     printf ("Sound Off\n");
*4E
*3E
 } 
*2E
