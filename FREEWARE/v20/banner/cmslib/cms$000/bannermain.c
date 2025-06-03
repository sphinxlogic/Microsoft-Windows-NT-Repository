 #module BannerMain "V1.0"
 
 /*
 **++
 **  FACILITY:
 **
 **      The DEcwindows Banner program
 **
 **  ABSTRACT:
 **
 **      This routine contains all the code conserned with the mian loop, and 
 **  processing of the window displays, not directly concerned with the contents
 **  of the windows them selves.
 **
 **  AUTHORS:
 **
 **      Jim Saunders
 **
 **
 **  CREATION DATE:     26th September 1989	Original version
 **
 **  MODIFICATION HISTORY:
 **--
 **/
 /*
 **
 **  INCLUDE FILES
 **
 **/
 
*8D
 #include <stdio.h>
 #include <decw$include/DECwDwtWidgetProg.h>
*8E
*8I
 
*12I
 #include "quidef.h"
*12E
 #include "stdio.h"
*15D
 #include "intrinsic.h"
 #include "Xlib.h"
 #include "Intrinsic.h"
 #include "StringDefs.h"
 #include "Shell.h"
 #include "IntrinsicP.h"
 #include "CoreP.h"
 #include "CompositeP.h"
 #include "ConstrainP.h"
 #include "ShellP.h"
 #include "VendorP.h"
 #include "MrmAppl.h"
*15E
*15I
 #include <Mrm/MrmAppl.h>
 #include <Xm/Shell.h>
 #include <Xt/vendor.h>
*15E
 
 
*8E
 #include "Banner"
 
 
 /*
 **
 **  MACRO DEFINITIONS
 **
 **/
 
 #define hoursize	3
 #define minsize		2
 #define	secsize		1
 
 /* 
  * Following are the forward, and external routine declarations used by
  * this module
  */
 
 void BannerUpdateWindows ();
*2D
 static void BannerMainLoop();
        void BannerSetArg();
        void BannerGetArg();
        void BannerSecondTimerAst();
*2E
*2I
 void BannerMainLoop();
 void BannerSetArg();
 void BannerGetArg();
 void BannerSecondTimerAst();
*2E
 
*12I
 extern void BannerGetQueueInfo();
 
*12E
 extern void BannerGlobe ();
 
 extern void  BannerMenuPopup();
 extern void  BannerMenuPopupDistroy();
*18D
*17A1D
 extern void  BannerMonitor();
*17A1E
*17A1I
 extern void  BannerLink();
*17A1E
*18E
*18I
*20D
 extern void  BannerLink();
*20E
*20I
 extern void  BannerMonitor();
*20E
*18E
*21I
 extern void  BannerLink();
*21E
 extern void  BannerClockAst();
 extern void  BannerClock();
 extern void  BannerHanoiSetup();
 extern void  BannerCubeSetup();
 extern void  BannerCube();
 
 /*
  * now our runtime data structures
  */
 
 
*12I
 extern	 Q$_Blk	     Qp;
 extern	 Q$_Blk	     Qb;
*12E
 extern	 Bnr$_Blk    Bnr;
 extern	 Clk$_Blk    Clk;
 extern	 GC	     BannerGC;
 extern	 XGCValues   GcValues;
 extern   Pixmap BannerPixmap, SmBannerPixmap, BannerFillPixmap, BannerBellPixmap;
 extern   int	    BannerInitialPriority;
 extern   XEvent	     BannerEvent;
 
*4I
 extern   char	     BannerVmsNodeName[];
 extern   char	     BannerVmsVersion[];
 
*4E
 
 static long purgadr[2] = {512, 2147483647};	/* Working set purge address */
 
 typedef struct numtim_struct	
 	{
 	    short int	year;
 	    short int	month;
 	    short int	day;
 	    short int	hour;
 	    short int	minute;
 	    short int	second;
 		  int	tenths;
 	} numtim_itemlist;
 
 static numtim_itemlist numtim;
 
 static long lasttime = -1;	/* Last time we came through the ast */
 
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
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
*2D
 static void	
 BannerMainLoop()
*2E
*2I
 void BannerMainLoop()
*2E
 {
     int TRACE;
     int height, width;
*10I
     Widget SetupWidget;
*10E
     Window MoveWindow = 0;
     Widget MoveWidget;
     Widget MoveingWidget;
     XWindowChanges xswa;
     int MoveX, MoveY;
*17I
     int OrgMoveX, OrgMoveY;
*17E
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
 
*10I
 	if (Bnr.setup_widget == NULL)
 	    SetupWidget = Bnr.dp_id;
 	else
 	    SetupWidget = Bnr.setup_widget;
 
*10E
 	XtNextEvent (&BannerEvent);
 	XtDispatchEvent (&BannerEvent);
 	if (TRACE == 1)
 	    printf ("Event: w=%x, event=%d\n", BannerEvent.xany.window, BannerEvent.xany.type);
 	if (BannerEvent.xany.window == RootWindow (XtDisplay(Bnr.dp_id),
 			    DefaultScreen(XtDisplay(Bnr.dp_id))) ||
*15D
 	     BannerEvent.xany.window == XtWindow(MoveWidget = Bnr.main_widget) ||
 	     BannerEvent.xany.window == XtWindow(MoveWidget = Bnr.dp_id) ||
*15E
*10I
*11D
 /*	     BannerEvent.xany.window == XtWindow(MoveWidget = SetupWidget) ||*/
*11E
*11I
*15D
 	     BannerEvent.xany.window == XtWindow(MoveWidget = SetupWidget) ||
*15E
*11E
*10E
*15D
 	     BannerEvent.xany.window == XtWindow(MoveWidget = Bnr.clock_widget) ||
 	     BannerEvent.xany.window == XtWindow(MoveWidget = Bnr.message_widget) ||
*15E
*13D
 	     BannerEvent.xany.window == XtWindow(MoveWidget = Bnr.top_widget) ||
*13E
*15D
 	     BannerEvent.xany.window == XtWindow(MoveWidget = Bnr.cpu_widget) ||
 	     BannerEvent.xany.window == XtWindow(MoveWidget = Bnr.pe_widget) ||
 	     BannerEvent.xany.window == XtWindow(MoveWidget = Bnr.lck_widget) ||
 	     BannerEvent.xany.window == XtWindow(MoveWidget = Bnr.cwps_widget) ||
*15E
*3I
*15D
 	     BannerEvent.xany.window == XtWindow(MoveWidget = Bnr.sys_widget) ||
*15E
*3E
*9I
*15D
 	     BannerEvent.xany.window == XtWindow(MoveWidget = Bnr.qp_widget) ||
 	     BannerEvent.xany.window == XtWindow(MoveWidget = Bnr.qb_widget) ||
 	     BannerEvent.xany.window == XtWindow(MoveWidget = Bnr.clu_widget) ||
 	     BannerEvent.xany.window == XtWindow(MoveWidget = Bnr.tp_widget) ||
*15E
*9E
*15D
 	     BannerEvent.xany.window == XtWindow(MoveWidget = Bnr.cube_widget) ||
*15E
*7I
*15D
 	     BannerEvent.xany.window == XtWindow(MoveWidget = Bnr.globe_widget) ||
 	     BannerEvent.xany.window == XtWindow(MoveWidget = Bnr.hanoi_widget) ||
*15E
*7E
*15D
 	     BannerEvent.xany.window == XtWindow(MoveWidget = Bnr.process_widget) ||
 	     BannerEvent.xany.window == XtWindow(MoveWidget = Bnr.mon_widget))
*15E
*15I
 	     BannerEvent.xany.window == XtWindow((MoveWidget = Bnr.main_widget)) ||
 	     BannerEvent.xany.window == XtWindow((MoveWidget = Bnr.dp_id)) ||
 	     BannerEvent.xany.window == XtWindow((MoveWidget = SetupWidget)) ||
 	     BannerEvent.xany.window == XtWindow((MoveWidget = Bnr.clock_widget)) ||
 	     BannerEvent.xany.window == XtWindow((MoveWidget = Bnr.message_widget)) ||
 	     BannerEvent.xany.window == XtWindow((MoveWidget = Bnr.cpu_widget)) ||
 	     BannerEvent.xany.window == XtWindow((MoveWidget = Bnr.pe_widget)) ||
 	     BannerEvent.xany.window == XtWindow((MoveWidget = Bnr.lck_widget)) ||
 	     BannerEvent.xany.window == XtWindow((MoveWidget = Bnr.cwps_widget)) ||
 	     BannerEvent.xany.window == XtWindow((MoveWidget = Bnr.sys_widget)) ||
 	     BannerEvent.xany.window == XtWindow((MoveWidget = Bnr.qp_widget)) ||
 	     BannerEvent.xany.window == XtWindow((MoveWidget = Bnr.qb_widget)) ||
 	     BannerEvent.xany.window == XtWindow((MoveWidget = Bnr.clu_widget)) ||
 	     BannerEvent.xany.window == XtWindow((MoveWidget = Bnr.tp_widget)) ||
 	     BannerEvent.xany.window == XtWindow((MoveWidget = Bnr.cube_widget)) ||
 	     BannerEvent.xany.window == XtWindow((MoveWidget = Bnr.globe_widget)) ||
 	     BannerEvent.xany.window == XtWindow((MoveWidget = Bnr.hanoi_widget)) ||
 	     BannerEvent.xany.window == XtWindow((MoveWidget = Bnr.process_widget)) ||
*18D
*17A1D
 	     BannerEvent.xany.window == XtWindow((MoveWidget = Bnr.mon_widget)))
*17A1E
*17A1I
 	     BannerEvent.xany.window == XtWindow((MoveWidget = Bnr.link_widget)))
*17A1E
*18E
*18I
*20D
 	     BannerEvent.xany.window == XtWindow((MoveWidget = Bnr.link_widget)))
*20E
*20I
*21I
 	     BannerEvent.xany.window == XtWindow((MoveWidget = Bnr.link_widget)) ||
*21E
 	     BannerEvent.xany.window == XtWindow((MoveWidget = Bnr.mon_widget)))
*20E
*18E
*15E
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
*17D
 			widget->core.x,
 			widget->core.y);
*17E
*17I
 			OrgMoveX,
 			OrgMoveY);
*17E
 
 /*
  * Turn borders off, 
  */
 
 		    if (Bnr.Param_Bits.window_border)
 			xswa.border_width = 1;
 		    else
 			xswa.border_width = 0;
*17I
 
*17E
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
*14I
 		    BannerUndisplayCursor ();
*14E
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
*10D
  * If it a MB1 button, and we have no boarders, then go into
*10E
*10I
  * If it a MB3 button, and we have no boarders, then go into
*10E
  * move window mode.
  */
*10D
 		if (BannerEvent.xbutton.button == 1 &&
*10E
*10I
 		if (BannerEvent.xbutton.button == 3 &&
*10E
 		    ! Bnr.Param_Bits.border)
 		    {
 		    Widget widget;
 
 		    widget = MoveWidget->core.parent;
 		    MoveingWidget = MoveWidget;
 
 		    LastPointerX = BannerEvent.xbutton.x_root;
 		    LastPointerY = BannerEvent.xbutton.y_root;
 				    
 		    MoveX = MoveY = 0;
*8D
 		    BannerGetArg (widget, DwtNx, 
*8E
*8I
 		    BannerGetArg (widget, XmNx, 
*8E
 			&MoveX); 
*8D
 		    BannerGetArg (widget, DwtNy, 
*8E
*8I
 		    BannerGetArg (widget, XmNy, 
*8E
 			&MoveY); 
*17I
 		    OrgMoveX = MoveX;
 		    OrgMoveY = MoveY;
*17E
 
*8D
 		    BannerGetArg (widget, DwtNwidth, 
*8E
*8I
 		    BannerGetArg (widget, XmNwidth, 
*8E
 			&width); 
*8D
 		    BannerGetArg (widget, DwtNheight, 
*8E
*8I
 		    BannerGetArg (widget, XmNheight, 
*8E
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
 				
*14I
 		    BannerDisplayCursor (MoveingWidget, 1);
*14E
 		    }
 		break;
 		}
 	    case ButtonRelease :
 		{
 		BannerMenuPopupDestroy ();
*14I
 		BannerUndisplayCursor ();
*14E
 /*
  * If we were in move window mode, stop it, and 
  * place the window at the new possition
  */
 		if (MoveWindow != 0 &&
*10D
 		    BannerEvent.xbutton.button == 1)
*10E
*10I
 		    BannerEvent.xbutton.button == 3)
*10E
 		    {
 		    Widget widget;
 
 		    widget = MoveingWidget->core.parent;
*8D
 		    BannerGetArg (widget, DwtNwidth, 
*8E
*8I
 		    BannerGetArg (widget, XmNwidth, 
*8E
 			&width); 
*8D
 		    BannerGetArg (widget, DwtNheight, 
*8E
*8I
 		    BannerGetArg (widget, XmNheight, 
*8E
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
*8D
 		    BannerSetArg (widget, DwtNx, 
*8E
*8I
 		    BannerSetArg (widget, XmNx, 
*8E
 			MoveX); 
*8D
 		    BannerSetArg (widget, DwtNy, 
*8E
*8I
 		    BannerSetArg (widget, XmNy, 
*8E
 			MoveY); 
 		    widget->core.x = MoveX;
 		    widget->core.y = MoveY;
 		    }
 		break;
 		}
*16I
 	    case VisibilityNotify :
 /*
  * if we have become visible, we had better repaint all the constant 
  * info on the screen.
  */
 		if (BannerEvent.xvisibility.state != VisibilityUnobscured)
 		    break;
 	    case ConfigureNotify :
*16E
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
*16D
 			    BannerPERefresh ();
 			    Bnr.ws_purged = 0;
*16E
*16I
 			if (Bnr.pe_widget->core.width != Bnr.pe_width ||
 			    Bnr.pe_widget->core.height != Bnr.pe_height)
 			    {
 			    Bnr.pe_width = Bnr.pe_widget->core.width;
 			    Bnr.pe_height = Bnr.pe_widget->core.height;
 			    BannerPESetup ();
 			    }
 			BannerPERefresh ();
 			Bnr.ws_purged = 0;
*16E
 			break;
 			}
 		    if (BannerEvent.xany.window == XtWindow(Bnr.lck_widget) &&
 			Bnr.lck_widget != Bnr.dp_id)
 			{
*16D
 			    BannerLCKRefresh ();
 			    Bnr.ws_purged = 0;
*16E
*16I
 			if (Bnr.lck_widget->core.width != Bnr.lck_width ||
 			    Bnr.lck_widget->core.height != Bnr.lck_height)
 			    {
 			    Bnr.lck_width = Bnr.lck_widget->core.width;
 			    Bnr.lck_height = Bnr.lck_widget->core.height;
 			    BannerLCKSetup ();
 			    }
 			BannerLCKRefresh ();
 			Bnr.ws_purged = 0;
*16E
 			break;
 			}
 		    if (BannerEvent.xany.window == XtWindow(Bnr.cwps_widget) &&
 			Bnr.cwps_widget != Bnr.dp_id)
 			{
*16D
 			    BannerCWPSRefresh ();
 			    Bnr.ws_purged = 0;
*16E
*16I
 			if (Bnr.cwps_widget->core.width != Bnr.cwps_width ||
 			    Bnr.cwps_widget->core.height != Bnr.cwps_height)
 			    {
 			    Bnr.cwps_width = Bnr.cwps_widget->core.width;
 			    Bnr.cwps_height = Bnr.cwps_widget->core.height;
 			    BannerCWPSSetup ();
 			    }
 			BannerCWPSRefresh ();
 			Bnr.ws_purged = 0;
*16E
 			break;
 			}
*3I
 		    if (BannerEvent.xany.window == XtWindow(Bnr.sys_widget) &&
 			Bnr.sys_widget != Bnr.dp_id)
 			{
*16D
 			    BannerSYSRefresh ();
 			    Bnr.ws_purged = 0;
*16E
*16I
 			if (Bnr.sys_widget->core.width != Bnr.sys_width ||
 			    Bnr.sys_widget->core.height != Bnr.sys_height)
 			    {
 			    Bnr.sys_width = Bnr.sys_widget->core.width;
 			    Bnr.sys_height = Bnr.sys_widget->core.height;
 			    BannerSYSSetup ();
 			    }
 			BannerSYSRefresh ();
 			Bnr.ws_purged = 0;
*16E
 			break;
 			}
*3E
*9I
 		    if (BannerEvent.xany.window == XtWindow(Bnr.tp_widget) &&
 			Bnr.tp_widget != Bnr.dp_id)
 			{
*16D
 			    BannerTPRefresh ();
 			    Bnr.ws_purged = 0;
*16E
*16I
 			if (Bnr.tp_widget->core.width != Bnr.tp_width ||
 			    Bnr.tp_widget->core.height != Bnr.tp_height)
 			    {
 			    Bnr.tp_width = Bnr.tp_widget->core.width;
 			    Bnr.tp_height = Bnr.tp_widget->core.height;
 			    BannerTPSetup ();
 			    }
 			BannerTPRefresh ();
 			Bnr.ws_purged = 0;
*16E
 			break;
 			}
 		    if (BannerEvent.xany.window == XtWindow(Bnr.qb_widget) &&
 			Bnr.qb_widget != Bnr.dp_id)
 			{
*16D
 			    BannerQBRefresh ();
 			    Bnr.ws_purged = 0;
*16E
*16I
 			if (Bnr.qb_widget->core.width != Bnr.qb_width ||
 			    Bnr.qb_widget->core.height != Bnr.qb_height)
 			    {
 			    Bnr.qb_width = Bnr.qb_widget->core.width;
 			    Bnr.qb_height = Bnr.qb_widget->core.height;
 			    BannerQBSetup ();
 			    }
 			BannerQBRefresh ();
 			Bnr.ws_purged = 0;
*16E
 			break;
 			}
 		    if (BannerEvent.xany.window == XtWindow(Bnr.qp_widget) &&
 			Bnr.qp_widget != Bnr.dp_id)
 			{
*16D
 			    BannerQPRefresh ();
 			    Bnr.ws_purged = 0;
*16E
*16I
 			if (Bnr.qp_widget->core.width != Bnr.qp_width ||
 			    Bnr.qp_widget->core.height != Bnr.qp_height)
 			    {
 			    Bnr.qp_width = Bnr.qp_widget->core.width;
 			    Bnr.qp_height = Bnr.qp_widget->core.height;
 			    BannerQPSetup ();
 			    }
 			BannerQPRefresh ();
 			Bnr.ws_purged = 0;
*16E
 			break;
 			}
 		    if (BannerEvent.xany.window == XtWindow(Bnr.clu_widget) &&
 			Bnr.clu_widget != Bnr.dp_id)
 			{
*16D
 			    BannerCLURefresh ();
 			    Bnr.ws_purged = 0;
 			break;
 			}
*16E
*9E
*16D
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
*16E
*7D
 			if (Bnr.Param_Bits.world)
 			    BannerGlobe (0, 0, Bnr.cube_width, 
 				Bnr.cube_height, 1);
 			if (Bnr.Param_Bits.cube)
 			    BannerCubeRefresh ();
 			if (Bnr.Param_Bits.hanoi)
 			    BannerHanoiRefresh ();
*7E
*7I
*16D
 			BannerCubeRefresh ();
 			Bnr.ws_purged = 0;
 			break;
 			}
 		    if (BannerEvent.xany.window == XtWindow(Bnr.hanoi_widget) &&
 			Bnr.hanoi_widget != Bnr.dp_id)
 			{
 			BannerHanoiRefresh ();
 			Bnr.ws_purged = 0;
 			break;
 			}
 		    if (BannerEvent.xany.window == XtWindow(Bnr.globe_widget) &&
 			Bnr.globe_widget != Bnr.dp_id)
 			{
 			BannerGlobe (0, 0, Bnr.cube_width, 
 				Bnr.cube_height, 1);
*16E
*7E
*16D
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
*16E
*13D
 		    if (BannerEvent.xany.window == XtWindow(Bnr.main_widget) &&
 			Bnr.main_widget != Bnr.dp_id &&
 			Bnr.top_widget == Bnr.dp_id)
*13E
*13I
*16D
 		    if (BannerEvent.xany.window == XtWindow(Bnr.main_widget))
*16E
*13E
*16D
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
*16E
*4I
*16D
 			XSetForeground (XtDisplay(Bnr.main_widget), 
 			    BannerGC, Bnr.hwmrk);
 			BannerWriteText (XtDisplay (Bnr.main_widget),
 			    XtWindow (Bnr.main_widget),
 			    0,
 			    Bnr.font_height * 3,
 			    BannerVmsNodeName,
 			    2,
 			    Bnr.font_width*20);
 			XSetForeground (XtDisplay(Bnr.main_widget), 
 			    BannerGC, Bnr.foreground);
*16E
*4E
*16D
 			}
 		    }
 		break;
 		}
*16E
*13I
*16D
 
 
*16E
*13E
*16D
 	    case VisibilityNotify :
 		{
 /*
  * if we have become visible, we had better repaint all the constant 
  * info on the screen.
  */
 		if (BannerEvent.xvisibility.state == VisibilityUnobscured)
 		    {
*16E
*16I
 			if (Bnr.clu_widget->core.width != Bnr.clu_width ||
 			    Bnr.clu_widget->core.height != Bnr.clu_height)
 			    {
 			    Bnr.clu_width = Bnr.clu_widget->core.width;
 			    Bnr.clu_height = Bnr.clu_widget->core.height;
 			    BannerCLUSetup ();
 			    }
 			BannerCLURefresh ();
 			Bnr.ws_purged = 0;
 			break;
 			}
*16E
 		    if (BannerEvent.xany.window == XtWindow(Bnr.cpu_widget) &&
 			Bnr.cpu_widget != Bnr.dp_id)
 			{
*16D
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
*16E
*3I
*16D
 		    if (BannerEvent.xany.window == XtWindow(Bnr.sys_widget) &&
 			Bnr.sys_widget != Bnr.dp_id)
 			{
 			if (Bnr.Param_Bits.sys_window)
 			    BannerSYSRefresh ();
 			Bnr.ws_purged = 0;
 			break;
 			}
*16E
*3E
*9I
*16D
 		    if (BannerEvent.xany.window == XtWindow(Bnr.tp_widget) &&
 			Bnr.tp_widget != Bnr.dp_id)
 			{
 			if (Bnr.Param_Bits.tp_window)
 			    BannerTPRefresh ();
 			Bnr.ws_purged = 0;
 			break;
 			}
 		    if (BannerEvent.xany.window == XtWindow(Bnr.qp_widget) &&
 			Bnr.qp_widget != Bnr.dp_id)
 			{
 			if (Bnr.Param_Bits.qp_window)
 			    BannerQPRefresh ();
 			Bnr.ws_purged = 0;
 			break;
 			}
 		    if (BannerEvent.xany.window == XtWindow(Bnr.qb_widget) &&
 			Bnr.qb_widget != Bnr.dp_id)
 			{
 			if (Bnr.Param_Bits.qb_window)
 			    BannerQBRefresh ();
 			Bnr.ws_purged = 0;
 			break;
 			}
 		    if (BannerEvent.xany.window == XtWindow(Bnr.clu_widget) &&
 			Bnr.clu_widget != Bnr.dp_id)
 			{
 			if (Bnr.Param_Bits.clu_window)
 			    BannerCLURefresh ();
*16E
*16I
 			if (Bnr.cpu_widget->core.width != Bnr.cpu_width ||
 			    Bnr.cpu_widget->core.height != Bnr.cpu_height)
 			    {
 			    Bnr.cpu_width = Bnr.cpu_widget->core.width;
 			    Bnr.cpu_height = Bnr.cpu_widget->core.height;
 			    BannerCpuSetup ();
 			    }
 			BannerCpuRefresh ();
*16E
 			Bnr.ws_purged = 0;
 			break;
 			}
*9E
*18D
*17A1D
 		    if (BannerEvent.xany.window == XtWindow(Bnr.mon_widget) &&
 			Bnr.mon_widget != Bnr.dp_id)
*17A1E
*17A1I
 		    if (BannerEvent.xany.window == XtWindow(Bnr.link_widget) &&
 			Bnr.link_widget != Bnr.dp_id)
*17A1E
*18E
*18I
*20D
 		    if (BannerEvent.xany.window == XtWindow(Bnr.link_widget) &&
 			Bnr.link_widget != Bnr.dp_id)
*20E
*20I
 		    if (BannerEvent.xany.window == XtWindow(Bnr.mon_widget) &&
 			Bnr.mon_widget != Bnr.dp_id)
*20E
*18E
 			{
*16D
 			if (Bnr.Param_Bits.monitor)
 			    BannerMonRefresh ();
*16E
*16I
*18D
*17A1D
 			if (Bnr.mon_widget->core.width != Bnr.mon_width ||
 			    Bnr.mon_widget->core.height != Bnr.mon_height)
*17A1E
*17A1I
 			if (Bnr.link_widget->core.width != Bnr.link_width ||
 			    Bnr.link_widget->core.height != Bnr.link_height)
*17A1E
*18E
*18I
*20D
 			if (Bnr.link_widget->core.width != Bnr.link_width ||
 			    Bnr.link_widget->core.height != Bnr.link_height)
*20E
*20I
 			if (Bnr.mon_widget->core.width != Bnr.mon_width ||
 			    Bnr.mon_widget->core.height != Bnr.mon_height)
*20E
*18E
 			    {
*18D
*17A1D
 			    Bnr.mon_width = Bnr.mon_widget->core.width;
 			    Bnr.mon_height = Bnr.mon_widget->core.height;
 			    BannerMonSetup ();
*17A1E
*17A1I
 			    Bnr.link_width = Bnr.link_widget->core.width;
 			    Bnr.link_height = Bnr.link_widget->core.height;
 			    BannerLinkSetup ();
*17A1E
*18E
*18I
*20D
 			    Bnr.link_width = Bnr.link_widget->core.width;
 			    Bnr.link_height = Bnr.link_widget->core.height;
 			    BannerLinkSetup ();
*20E
*20I
 			    Bnr.mon_width = Bnr.mon_widget->core.width;
 			    Bnr.mon_height = Bnr.mon_widget->core.height;
 			    BannerMonSetup ();
*20E
*18E
 			    }
*18D
*17A1D
 			BannerMonRefresh ();
*17A1E
*17A1I
 			BannerLinkRefresh ();
*17A1E
*18E
*18I
*20D
 			BannerLinkRefresh ();
*20E
*20I
 			BannerMonRefresh ();
*20E
*18E
*16E
 			Bnr.ws_purged = 0;
 			break;
 			}
*21I
 		    if (BannerEvent.xany.window == XtWindow(Bnr.link_widget) &&
 			Bnr.link_widget != Bnr.dp_id)
 			{
 			if (Bnr.link_widget->core.width != Bnr.link_width ||
 			    Bnr.link_widget->core.height != Bnr.link_height)
 			    {
 			    Bnr.link_width = Bnr.link_widget->core.width;
 			    Bnr.link_height = Bnr.link_widget->core.height;
 			    BannerLinkSetup ();
 			    }
 			BannerLinkRefresh ();
 			Bnr.ws_purged = 0;
 			break;
 			}
*21E
 		    if (BannerEvent.xany.window == XtWindow(Bnr.process_widget) &&
 			Bnr.process_widget != Bnr.dp_id)
 			{
*16D
 			if (Bnr.Param_Bits.process_window)
 			    BannerProcessRefresh ();
*16E
*16I
 			if (Bnr.process_widget->core.width != Bnr.process_width ||
 			    Bnr.process_widget->core.height != Bnr.process_height)
 			    {
 			    Bnr.process_width = Bnr.process_widget->core.width;
 			    Bnr.process_height = Bnr.process_widget->core.height;
 			    BannerProcessSetup ();
 			    }
 			BannerProcessRefresh ();
*16E
 			Bnr.ws_purged = 0;
 			break;
 			}
 		    if (BannerEvent.xany.window == XtWindow(Bnr.cube_widget) &&
 			Bnr.cube_widget != Bnr.dp_id)
 			{
*7D
 			if (Bnr.Param_Bits.world)
 			    BannerGlobe (0, 0, Bnr.cube_width, 
*7E
*7I
*16I
 			if (Bnr.cube_widget->core.width != Bnr.cube_width ||
 			    Bnr.cube_widget->core.height != Bnr.cube_height)
 			    {
 			    Bnr.cube_width = Bnr.cube_widget->core.width;
 			    Bnr.cube_height = Bnr.cube_widget->core.height;
 			    BannerCubeSetup ();
 			    }
*16E
 			BannerCubeRefresh ();
 			Bnr.ws_purged = 0;
 			break;
 			}
*16D
 		    if (BannerEvent.xany.window == XtWindow(Bnr.globe_widget) &&
 			Bnr.globe_widget != Bnr.dp_id)
 			{
 			BannerGlobe (0, 0, Bnr.cube_width, 
*16E
*7E
*16D
 				Bnr.cube_height, 1);
*16E
*7D
 
 			if (Bnr.Param_Bits.cube)
 			    BannerCubeRefresh ();
 
 			if (Bnr.Param_Bits.hanoi)
 			    BannerHanoiRefresh ();
*7E
*7I
*16D
 			Bnr.ws_purged = 0;
 			break;
 			}
*16E
 		    if (BannerEvent.xany.window == XtWindow(Bnr.hanoi_widget) &&
 			Bnr.hanoi_widget != Bnr.dp_id)
 			{
*16I
 			if (Bnr.hanoi_widget->core.width != Bnr.hanoi_width ||
 			    Bnr.hanoi_widget->core.height != Bnr.hanoi_height)
 			    {
 			    Bnr.hanoi_width = Bnr.hanoi_widget->core.width;
 			    Bnr.hanoi_height = Bnr.hanoi_widget->core.height;
 			    BannerHanoiSetup ();
 			    }
*16E
 			BannerHanoiRefresh ();
*7E
 			Bnr.ws_purged = 0;
 			break;
 			}
*16D
 
*16E
*16I
 		    if (BannerEvent.xany.window == XtWindow(Bnr.globe_widget) &&
 			Bnr.globe_widget != Bnr.dp_id)
 			{
 			if (Bnr.globe_widget->core.width != Bnr.globe_width ||
 			    Bnr.globe_widget->core.height != Bnr.globe_height)
 			    {
 			    Bnr.globe_width = Bnr.globe_widget->core.width;
 			    Bnr.globe_height = Bnr.globe_widget->core.height;
 			    }
 			BannerGlobe (0, 0, Bnr.cube_width, 
 				Bnr.cube_height, 1);
 			Bnr.ws_purged = 0;
 			break;
 			}
*16E
 		    if (BannerEvent.xany.window == XtWindow(Bnr.clock_widget) &&
*16D
 			Bnr.clock_widget != Bnr.dp_id)
*16E
*16I
 			    Bnr.clock_widget != Bnr.dp_id)
*16E
 			{
*16I
 			if (Bnr.clock_widget->core.width != Bnr.clock_width ||
 			    Bnr.clock_widget->core.height != Bnr.clock_height)
 			    {
 			    Bnr.clock_width = Bnr.clock_widget->core.width;
 			    Bnr.clock_height = Bnr.clock_widget->core.height;
 			    }
*16E
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
*13D
 		    if (BannerEvent.xany.window == XtWindow(Bnr.main_widget) &&
 			Bnr.main_widget != Bnr.dp_id &&
 			Bnr.top_widget == Bnr.dp_id)
*13E
*13I
 		    if (BannerEvent.xany.window == XtWindow(Bnr.main_widget))
*13E
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
*16I
 			XSetForeground (XtDisplay(Bnr.main_widget), 
 			    BannerGC, Bnr.hwmrk);
 			BannerWriteText (XtDisplay (Bnr.main_widget),
 			    XtWindow (Bnr.main_widget),
 			    0,
 			    Bnr.font_height * 3,
 			    BannerVmsNodeName,
 			    2,
 			    Bnr.font_width*20);
 			XSetForeground (XtDisplay(Bnr.main_widget), 
 			    BannerGC, Bnr.foreground);
*16E
 			}
 		    }
 		break;
 		}
*16D
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
*16E
*7D
 		    if (Bnr.Param_Bits.cube)
 			BannerCubeSetup ();
 		    if (Bnr.Param_Bits.hanoi)
 			BannerHanoiSetup ();
*7E
*7I
*16D
 		    BannerCubeSetup ();
 		    XClearWindow (BannerEvent.xany.display, 
 			BannerEvent.xany.window);
 		    Bnr.ws_purged = 0;
 		    break;
 		    }
 		if (BannerEvent.xany.window == XtWindow(Bnr.globe_widget) &&
 		    Bnr.globe_widget != Bnr.dp_id)
 		    {
 		    Bnr.globe_width = BannerEvent.xconfigure.width;
 		    Bnr.globe_height = BannerEvent.xconfigure.height;
 		    XClearWindow (BannerEvent.xany.display, 
 			BannerEvent.xany.window);
 		    Bnr.ws_purged = 0;
 		    break;
 		    }
 		if (BannerEvent.xany.window == XtWindow(Bnr.hanoi_widget) &&
 		    Bnr.hanoi_widget != Bnr.dp_id)
 		    {
 		    Bnr.hanoi_width = BannerEvent.xconfigure.width;
 		    Bnr.hanoi_height = BannerEvent.xconfigure.height;
 		    BannerHanoiSetup ();
*16E
*7E
*16D
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
*16E
*3I
*16D
 		if (BannerEvent.xany.window == XtWindow(Bnr.sys_widget) &&
 		    Bnr.sys_widget != Bnr.dp_id)
 		    {
 		    Bnr.sys_width = BannerEvent.xconfigure.width;
 		    Bnr.sys_height = BannerEvent.xconfigure.height;
 		    BannerSYSSetup ();
 		    XClearWindow (BannerEvent.xany.display, 
 			BannerEvent.xany.window);
 		    Bnr.ws_purged = 0;
 		    break;
 		    }
*16E
*3E
*9I
*16D
 		if (BannerEvent.xany.window == XtWindow(Bnr.tp_widget) &&
 		    Bnr.tp_widget != Bnr.dp_id)
 		    {
 		    Bnr.tp_width = BannerEvent.xconfigure.width;
 		    Bnr.tp_height = BannerEvent.xconfigure.height;
 		    BannerTPSetup ();
 		    XClearWindow (BannerEvent.xany.display, 
 			BannerEvent.xany.window);
 		    Bnr.ws_purged = 0;
 		    break;
 		    }
 		if (BannerEvent.xany.window == XtWindow(Bnr.qp_widget) &&
 		    Bnr.qp_widget != Bnr.dp_id)
 		    {
 		    Bnr.qp_width = BannerEvent.xconfigure.width;
 		    Bnr.qp_height = BannerEvent.xconfigure.height;
 		    BannerQPSetup ();
 		    XClearWindow (BannerEvent.xany.display, 
 			BannerEvent.xany.window);
 		    Bnr.ws_purged = 0;
 		    break;
 		    }
 		if (BannerEvent.xany.window == XtWindow(Bnr.qb_widget) &&
 		    Bnr.qb_widget != Bnr.dp_id)
 		    {
 		    Bnr.qb_width = BannerEvent.xconfigure.width;
 		    Bnr.qb_height = BannerEvent.xconfigure.height;
 		    BannerQBSetup ();
 		    XClearWindow (BannerEvent.xany.display, 
 			BannerEvent.xany.window);
 		    Bnr.ws_purged = 0;
 		    break;
 		    }
 		if (BannerEvent.xany.window == XtWindow(Bnr.clu_widget) &&
 		    Bnr.clu_widget != Bnr.dp_id)
 		    {
 		    Bnr.clu_width = BannerEvent.xconfigure.width;
 		    Bnr.clu_height = BannerEvent.xconfigure.height;
 		    BannerCLUSetup ();
 		    XClearWindow (BannerEvent.xany.display, 
 			BannerEvent.xany.window);
 		    Bnr.ws_purged = 0;
 		    break;
 		    }
*16E
*9E
*16D
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
*16E
*16I
 
*16E
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
*12D
  * cycle for this second tick!
*12E
*12I
  * cycle for this second tick! Also if one of the queue windows is specified, 
  * then we will loop here, because the JOB_CONTROLLER is so slow.
*12E
  */
*12D
 	    if (Bnr.Param_Bits.cube || Bnr.Param_Bits.hanoi)
*12E
*12I
 	    if (Bnr.Param_Bits.cube || Bnr.Param_Bits.hanoi ||
 		((Bnr.Param_Bits.qb_window && !Qb.Updated) || 
 		 (Bnr.Param_Bits.qp_window && !Qp.Updated)))
*12E
 		while (XtPending() == 0)
 		    {
*12I
 		    static int qpdone = 0, qbdone = 0;
 
*12E
 		    BannerUpdateWindows();
*12I
*19D
*17A1D
 		    if (!qpdone && !Qp.Updated)
*17A1E
*17A1I
 		    if (!qpdone && !Qp.Updated && Bnr.Param_Bits.qp_window)
*17A1E
*19E
*19I
*20D
 		    if (!qpdone && !Qp.Updated && Bnr.Param_Bits.qp_window)
*20E
*20I
*21D
 		    if (!qpdone && !Qp.Updated)
*21E
*21I
 		    if (!qpdone && !Qp.Updated && Bnr.Param_Bits.qp_window)
*21E
*20E
*19E
 			{
 			BannerGetQueueInfo (&Qp, Bnr.qp_name, 
 			    QUI$M_SEARCH_PRINTER);
 			if (Qp.Updated)
 			    qpdone = 1;
 			}
 		    else
*19D
*17A1D
 			if (!qbdone && !Qb.Updated)
*17A1E
*17A1I
 			if (!qbdone && !Qb.Updated && Bnr.Param_Bits.qb_window)
*17A1E
*19E
*19I
*20D
 			if (!qbdone && !Qb.Updated && Bnr.Param_Bits.qb_window)
*20E
*20I
*21D
 			if (!qbdone && !Qb.Updated)
*21E
*21I
 			if (!qbdone && !Qb.Updated && Bnr.Param_Bits.qb_window)
*21E
*20E
*19E
 			    {
 			    BannerGetQueueInfo (&Qb, Bnr.qb_name, 
 				QUI$M_SEARCH_BATCH);
 			    if (Qb.Updated)
 				qpdone = qbdone = 0;
 			    }
*12E
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
 **  FUNCTIONAL DESCRIPTION:
 **
 **	BannerUpdateWindows()
 **
 **      This routine is out client message event routine. When a client message
 **	is recieveed this routine is called, to handle the update of the 
 **	screen.
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
 BannerUpdateWindows()
 {
     float newhours, newminutes, newseconds;
     int digitaldone = 0;
     int drawhands = 0;
 
 
 /*
  * check if we should purge our working set
  */
     if (!Bnr.ws_purged)
 	{
 	Bnr.ws_purged = 1;
 	SYS$PURGWS (&purgadr);
 	}
 /*
  * do the things we must always do
  */
 
     if (Bnr.Param_Bits.cube)
 	{
*6D
 	BannerRotationSwitch();
*6E
 	BannerCube();
 	}
 /*
  * first check out if a second has really passed since the last time, if 
  * not we will do nothing.
  */
 
     SYS$NUMTIM (&numtim, NULL);
 
 /*
  * If a real second hasnt passed then forget it.
  */
     if (numtim.second == lasttime)
 	return;
 
     lasttime = numtim.second;
 
 /*
  * first see if a total refresh is need, if so call the appropriate 
  * action routines.
  */
 
     if (numtim.day != Bnr.current_day)
 	{
 	if (Bnr.Param_Bits.digital_clock)
 	    BannerWriteDate (XtDisplay(Bnr.clock_widget), 
 		XtWindow(Bnr.clock_widget));
 	drawhands = 1;
 	Bnr.ws_purged = 0;
 	Bnr.current_day = numtim.day;
 	}
 
 /*
  * check if we have any alarms outstanding
  */
     if (Bnr.alarm_count != 0)
 	BannerExecuteAlarm ();
 
 /*
  * Now rotate the world if we want it, by 1 click.
  */
 
*6I
     if (Bnr.Param_Bits.cube)
 	BannerRotationSwitch();
 
*6E
     if (Bnr.Param_Bits.world )
 	BannerGlobe (0, 0, Bnr.cube_width, Bnr.cube_height, 0);
 
     if (Bnr.Param_Bits.hanoi)
 	BannerHanoi();
 
     if (Bnr.Param_Bits.message_window)
 	BannerMessage();
 
 /*
  * First see if we need to do the rest this cycle
  */
 
     if (Bnr.Param_Bits.lck_window )
 	BannerLCK ();
 
     if (Bnr.Param_Bits.cwps_window )
 	BannerCWPS ();
 
*3I
     if (Bnr.Param_Bits.sys_window )
 	BannerSYS ();
 
*3E
*9I
     if (Bnr.Param_Bits.tp_window )
 	BannerTP ();
 
     if (Bnr.Param_Bits.qp_window )
 	BannerQP ();
 
     if (Bnr.Param_Bits.qb_window )
 	BannerQB ();
 
     if (Bnr.Param_Bits.clu_window )
 	BannerCLU ();
 
*9E
     if (Bnr.Param_Bits.pe_window )
 	BannerPE ();
 
*18D
*17A1D
     if (Bnr.Param_Bits.monitor )
 	BannerMon ();
*17A1E
*17A1I
     if (Bnr.Param_Bits.link_window )
 	BannerLink ();
*17A1E
*18E
*18I
*20D
     if (Bnr.Param_Bits.link_window )
 	BannerLink ();
*20E
*20I
     if (Bnr.Param_Bits.monitor )
 	BannerMon ();
*20E
*18E
 
*21I
     if (Bnr.Param_Bits.link_window )
 	BannerLink ();
 
*21E
     if (Bnr.Param_Bits.cpu_window )
 	BannerCpu ();
 
     if (Bnr.Param_Bits.process_window )
 	BannerProcess ();
 
     newhours = numtim.hour + (numtim.minute / 60.0);
     if (newhours > 12.0)
 	newhours = newhours - 12.0;
 
     newminutes = numtim.minute;
     newseconds = numtim.second;
 
 
 /* 
  * now refresh the clock hands.
  */
 
     if (Clk.minutes != newminutes)
     {
 	if (Bnr.Param_Bits.analog_clock)
 	{
 	    BannerDrawHand (Clk.hours, 12.0, Clk.hour_hand, hoursize, 1);
 	    BannerDrawHand (Clk.minutes, 60.0, Clk.minute_hand, minsize, 1);
 	    drawhands =1;
 	}
 	Clk.hours = newhours;
 	Clk.minutes = newminutes;
 	if (Bnr.Param_Bits.digital_clock)
 	    {
 	    BannerDigitalClock (XtDisplay(Bnr.clock_widget), 
 		XtWindow(Bnr.clock_widget));
 	    digitaldone=1;
 	    }
     }
 
     if (Bnr.Param_Bits.seconds)
     {
 	if (Bnr.Param_Bits.analog_clock)
 	    {
 	    BannerDrawHand (Clk.seconds, 60.0, Clk.second_hand, secsize, 1);
 	    Clk.seconds = newseconds;
 	    BannerDrawHand (Clk.seconds, 60.0, Clk.second_hand, secsize, 0);
 	    drawhands = 1;
 	    }
 	if (Bnr.Param_Bits.digital_clock && digitaldone == 0)
 	    BannerDigitalClock (XtDisplay(Bnr.clock_widget), 
 		XtWindow(Bnr.clock_widget));    	
     }
 
 /*
  * Now paint new hands for hours minutes etc.
  */    
 	if (drawhands)
 	{
 	    BannerDrawHand (Clk.hours, 12.0, Clk.hour_hand, hoursize, 0);
 	    BannerDrawHand (Clk.minutes, 60.0, Clk.minute_hand, minsize, 0);
 	}
 
 }
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **	BannerSecondTimerAst ();
 **
 **	This routine sets up an AST which calls itself, on a 1 second interval.
 **	For each call it sends a client message to wake up the main processing 
 **	loop.
 **
 **  FORMAL PARAMETERS:
 **
 **	None
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
 
 static XClientMessageEvent clientmessage;
 
 void	
 BannerSecondTimerAst()
 {
 
 
 /* 
  * Priority up!
  */
 
     if (Bnr.current_priority != Bnr.base_priority)
 	SYS$SETPRI (NULL, NULL, Bnr.base_priority, NULL);
 /*
  * now send the client message to our selves!
  */
 
     clientmessage.type = ClientMessage;
     clientmessage.display = XtDisplay (Bnr.main_widget);
     clientmessage.window = XtWindow (Bnr.main_widget);
     clientmessage.format = 32;
 
     XSendEvent (XtDisplay (Bnr.main_widget),
 	XtWindow (Bnr.main_widget),
 	True,
 	(unsigned long) 0,
 	&clientmessage);
 
 /*
  * now reset an AST for 1 seconds time
  */
 
 /*    SYS$SETIMR (NULL, &clocktimer, BannerSecondTimerAst, NULL); */
 
       XtAddTimeOut (1000, BannerSecondTimerAst, 0);
 
 /* 
  * Priority down!
  */
 
     if (Bnr.current_priority != Bnr.base_priority)
 	SYS$SETPRI (NULL, NULL, Bnr.current_priority, NULL);
 
 }
 
