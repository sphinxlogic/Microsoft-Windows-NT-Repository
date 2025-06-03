 #module BannerSubs "V1.0"
 
 /*
 **++
 **  FACILITY:
 **
 **      DECwindows Banner program
 **
 **  ABSTRACT:
 **
 **      The sub-routines module, contains most of the odd sod routines 
 **	used in banner, that do not fit with the other generic module types.
 **
 **  AUTHORS:
 **
 **      Jim Saunders
 **
 **
 **  CREATION DATE:     10th June 1988
 **
 **  MODIFICATION HISTORY:
 **--
 **/
 
 
 /*
 **
 **  INCLUDE FILES
 **
 **/
 
*17D
 #include <stdio.h>
 #include <file.h>
 #include <decw$include/DECwDwtWidgetProg.h>
*17E
*17I
 
*28I
 #include <decw$cursor.h>
 
 
*28E
 #include "stdio.h"
 #include "file.h"
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
 
 
*17E
 #include "Banner"
 
 
 /*
 **
 **  MACRO DEFINITIONS
 **
 **/
 
 
 /*
  * External routine definitions
  */
 
 extern void BannerSetAreas ();
 extern void BannerSignalError ();
 extern void LIB$DAY ();
 extern void OTS$CVT_T_F ();
 
 /* 
  * Definitions of other static items we use throughout the banner program
  * to reduce our setup times. 
  */
 
*28I
 int	 cursors[10];   
 
*28E
 extern	 int	     BannerInitialPriority;
*33I
 extern	 Son$_Blk    Son;
*33E
 extern	 Bnr$_Blk    Bnr;
 extern	 Clk$_Blk    Clk;
*35I
*36D
 extern	 Link$_Blk   Link;
*36E
*35E
*34A1I
 extern	 Link$_Blk   Link;
*34A1E
*37I
*38D
 **************** Conflict 1     ************************************************
 ********************************************************************************
*38E
 extern	 Link$_Blk   Link;
*38D
 ************** End of Conflict 1     *******************************************
*38E
*37E
 extern	 Mon$_Blk    Mon;
 extern	 GC	     BannerGC;
 extern	 XEvent	     BannerEvent;
 extern	 char	     *BannerDayList[];
 extern   Widget	     menu_widget;
 
 extern char	     banner_font_name[];
 extern char	     banner_font_size[];
 extern char	     banner_font_wieght[];
 extern char	     banner_font_type[];
 
*24I
*41I
*42D
 extern   char	     *BannerVmsDisplayName[];
*42E
*42I
 extern   char	     *BannerVmsDisplayName;
*42E
*41E
 extern   char	     BannerVmsNodeName[];
 extern   char	     BannerVmsVersion[];
 
 
*24E
 /*
  * The forward routine declarations
  */
 
*28I
 void	BannerCreateCursors();
 void	BannerDisplayCursor();
 void	BannerUndisplayCursor();
 
*28E
 void BannerMenuPopup ();
 void BannerMenuPopupDestroy ();
 void BannerPositionManagePopup ();
 void BannerMenuPopup ();
 void BannerMenuDestroy ();
 void BannerMenuCallback ();
 void BannerSetupCallback ();
 void BannerToggleChangeCallback ();
 void BannerToggleInitCallback ();
*6D
 int  BannerWriteDatabase ();
*6E
 void BannerReadResource ();
 void BannerSetDefaultResource ();
*6D
 void BannerWriteText ();
 void BannerDigitalClock ();
 void BannerWriteDate ();
*6E
 static	    BannerGetresource ();
 static	    BannerStrToNumeric ();
 
 /* Define costants used by the UI, in the UID file */
  
 static Bnr$_Blk    New_Bnr;
 static Mon$_Blk    New_Mon;
*19D
 static Widget *toggle_vector[100];
*19E
*19I
 static Widget *toggle_vector[150];
*19E
 static int foreground, background;
 
 static int font_changed;
 
 
 static XEvent saveevent;
 static Widget *savewidget;
 
 #define menu_width  185
 #define menu_height 190
 #define setup_width 800
 #define setup_height 350
 #define other_width 505
 #define other_height 200
 
 #define banner_box_toggle		    1
 #define banner_world_toggle		    2
 #define banner_neither_toggle		    3
 #define banner_cpu_window_toggle	    6
*35D
*34A1D
 #define banner_mon_window_toggle	    7
*34A1E
*34A1I
 #define banner_link_window_toggle	    7
*34A1E
*35E
*35I
*36D
 #define banner_link_window_toggle	    7
*36E
*36I
*37I
*38D
 **************** Conflict 2     ************************************************
*38E
*37E
 #define banner_mon_window_toggle	    7
*36E
*35E
*37I
*38D
 ********************************************************************************
 #define banner_link_window_toggle	    7
 ************** End of Conflict 2     *******************************************
*38E
*37E
 #define banner_analog_window_toggle	    8
 #define banner_digital_window_toggle	    9
 #define banner_dua0_active		    10
 #define banner_dua1_active		    11
 #define banner_dua2_active		    12
 #define banner_page_io_active		    13
 #define banner_page_fault_active	    14
 #define banner_modified_pages_active	    15
 #define banner_free_pages_active	    16
 #define banner_border_active		    17
 #define banner_seconds_active		    18
 #define banner_tick_size		    19
 #define banner_base_priority		    20
 #define banner_hanoi_toggle		    21
 #define banner_modes_toggle		    22
 #define banner_message_toggle		    23
 #define banner_bell_on			    25
 #define banner_icon_active		    26
*38I
 #define banner_link_window_toggle	    27
*38E
 
 #define banner_hour_widget		    30
 #define banner_minute_widget		    31
 #define banner_alarmtext_widget		    32
 
 #define banner_process_window_toggle	    33
 
 #define banner_disk_0			    34
 #define banner_disk_0_name		    35
 #define banner_disk_1			    36
 #define banner_disk_1_name		    37
 #define banner_disk_2			    38
 #define banner_disk_2_name		    39
 
 #define banner_message_create		    40
 
 #define banner_pe_window		    41
 #define banner_lck_window		    42
 #define banner_cwps_window		    43
 
 #define banner_font_name_c		    44
 #define banner_font_size_c		    45
 
 #define banner_process_name		    46
 #define banner_process_user		    47
 #define banner_process_pid		    48
 #define banner_sysfaults_active		    49
 #define banner_ni_io_active		    50
 
 #define avantgarde_font			    51
 #define courier_font			    52
 #define helvetica_font			    53
 #define lubalingraph_font		    54
 #define ncsschoolbook_font		    55
 #define souvenir_font			    56
 #define times_font			    57
 #define a8_point			    58
 #define a10_point			    59
 #define a12_point			    60
 #define a14_point			    61
 #define a18_point			    62
 #define a24_point			    63
 
 #define banner_process_states		    64
 #define banner_multi_hist_toggle	    65
*19D
 #define banner_window_border_active		    66
*19E
*19I
 #define banner_window_border_active	    66
*19E
 
*3I
 #define banner_freeiocnt		    67
 #define banner_freeblkcnt		    68
 
*3E
*9I
 #define banner_sys_window_toggle	    69
 
*9E
*19I
 #define    banner_tp			70
 #define    banner_cluster		71
 #define    banner_print_queue		72
 #define    banner_batch_queue		73
 #define    banner_tp_tcomprate		74
 #define    banner_tp_1fcomprate 	75
 #define    banner_tp_startcomprate 	76
 #define    banner_tp_prepcomprate 	77
 #define    banner_tp_abortcomprate 	78
 #define    banner_tp_endcomprate 	79
 #define    banner_tp_remstartcomprate	80
 #define    banner_tp_remendcomprate 	81
 #define    banner_tp_comprate1		82
 #define    banner_tp_comprate2		83
 #define    banner_tp_comprate3		84
 #define    banner_tp_comprate4		85
 #define    banner_print_queue_name 	86
 #define    banner_batch_queue_name 	87
 #define    banner_queue_user 		88
 #define    banner_show_hold 		89
 #define    banner_show_all_users 	90
 #define    banner_show_stopped		91
 #define    banner_show_int 		92
 #define    banner_max_comp_rate 	93
 #define    banner_comp_rate1_start 	94
 #define    banner_comp_rate2_start 	95
 #define    banner_comp_rate3_start 	96
 #define    banner_comp_rate4_start 	97
 #define    banner_comp_rate1_end 	98
 #define    banner_comp_rate2_end 	99
 #define    banner_comp_rate3_end 	100
 
*19E
 /*The functions */
*16D
 
 
*16E
*16I
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **      BannerClearWindows
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
 **  FUNCTION VALUE:
 **
 **      none
 **
 **  SIDE EFFECTS:
 **
 **      none
 **
 **--
 **/
 void	BannerClearWindows()
 {
 /*
  * Now clear all the windows, so Banner will perform a reset.
  */
 if (Bnr.Param_Bits.analog_clock || Bnr.Param_Bits.digital_clock)
     XClearWindow (XtDisplay(Bnr.clock_widget), XtWindow (Bnr.clock_widget));
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
 
 if (Bnr.cube_widget != Bnr.dp_id)		
     {
     XClearWindow (XtDisplay(Bnr.cube_widget), 
 	XtWindow (Bnr.cube_widget));
     BannerCubeSetup ();
     BannerCubeRefresh ();
     }
 if (Bnr.hanoi_widget != Bnr.dp_id)		
     {
     XClearWindow (XtDisplay(Bnr.hanoi_widget), 
 	XtWindow (Bnr.hanoi_widget));
     BannerHanoiSetup ();
     BannerHanoiRefresh ();
     }
 if (Bnr.globe_widget != Bnr.dp_id)		
     {
     XClearWindow (XtDisplay(Bnr.globe_widget), 
 	XtWindow (Bnr.globe_widget));
     BannerGlobe (0, 0, Bnr.cube_width, 
 	Bnr.cube_height, 1);
     }
 if (Bnr.cpu_widget != Bnr.dp_id)		
     {
     XClearWindow (XtDisplay(Bnr.cpu_widget), 
 	XtWindow (Bnr.cpu_widget));
     BannerCpuSetup ();
     BannerCpuRefresh ();
     }
 if (Bnr.pe_widget != Bnr.dp_id)		
     {
     XClearWindow (XtDisplay(Bnr.pe_widget), 
 	XtWindow (Bnr.pe_widget));
     BannerPESetup ();
     BannerPERefresh ();
     }
 if (Bnr.lck_widget != Bnr.dp_id)		
     {
     XClearWindow (XtDisplay(Bnr.lck_widget), 
 	XtWindow (Bnr.lck_widget));
     BannerLCKSetup ();
     BannerLCKRefresh ();
     }
 if (Bnr.cwps_widget != Bnr.dp_id)		
     {
     XClearWindow (XtDisplay(Bnr.cwps_widget), 
 	XtWindow (Bnr.cwps_widget));
     BannerCWPSSetup ();
     BannerCWPSRefresh ();
     }
 if (Bnr.sys_widget != Bnr.dp_id)		
     {
     XClearWindow (XtDisplay(Bnr.sys_widget), 
 	XtWindow (Bnr.sys_widget));
     BannerSYSSetup ();
     BannerSYSRefresh ();
     }
*19I
 if (Bnr.tp_widget != Bnr.dp_id)		
     {
     XClearWindow (XtDisplay(Bnr.tp_widget), 
 	XtWindow (Bnr.tp_widget));
     BannerTPSetup ();
     BannerTPRefresh ();
     }
 if (Bnr.qp_widget != Bnr.dp_id)		
     {
     XClearWindow (XtDisplay(Bnr.qp_widget), 
 	XtWindow (Bnr.qp_widget));
     BannerQPSetup ();
     BannerQPRefresh ();
     }
 if (Bnr.qb_widget != Bnr.dp_id)		
     {
     XClearWindow (XtDisplay(Bnr.qb_widget), 
 	XtWindow (Bnr.qb_widget));
     BannerQBSetup ();
     BannerQBRefresh ();
     }
 if (Bnr.clu_widget != Bnr.dp_id)		
     {
     XClearWindow (XtDisplay(Bnr.clu_widget), 
 	XtWindow (Bnr.clu_widget));
     BannerCLUSetup ();
     BannerCLURefresh ();
     }
*19E
*35D
*34A1D
 if (Bnr.mon_widget != Bnr.dp_id)		
*34A1E
*34A1I
 if (Bnr.link_widget != Bnr.dp_id)		
*34A1E
*35E
*35I
*36D
 if (Bnr.link_widget != Bnr.dp_id)		
*36E
*36I
*37I
*38D
 **************** Conflict 3     ************************************************
*38E
*37E
 if (Bnr.mon_widget != Bnr.dp_id)		
*36E
*35E
*37D
     {
*37E
*37I
*38D
 ********************************************************************************
 if (Bnr.link_widget != Bnr.dp_id)		
 ************** End of Conflict 3     *******************************************
     {
 **************** Conflict 4     ************************************************
*38E
*38I
     {
*38E
*37E
*35D
*34A1D
     XClearWindow (XtDisplay(Bnr.mon_widget), 
 	XtWindow (Bnr.mon_widget));
     BannerMonSetup ();
     BannerMonRefresh ();
*34A1E
*34A1I
     XClearWindow (XtDisplay(Bnr.link_widget), 
 	XtWindow (Bnr.link_widget));
     BannerLinkSetup ();
     BannerLinkRefresh ();
*34A1E
*35E
*35I
*36D
     XClearWindow (XtDisplay(Bnr.link_widget), 
 	XtWindow (Bnr.link_widget));
     BannerLinkSetup ();
     BannerLinkRefresh ();
*36E
*36I
     XClearWindow (XtDisplay(Bnr.mon_widget), 
 	XtWindow (Bnr.mon_widget));
     BannerMonSetup ();
     BannerMonRefresh ();
*36E
*35E
*37I
*38D
 ********************************************************************************
*38E
*38I
     }
 if (Bnr.link_widget != Bnr.dp_id)		
     {
*38E
     XClearWindow (XtDisplay(Bnr.link_widget), 
 	XtWindow (Bnr.link_widget));
     BannerLinkSetup ();
     BannerLinkRefresh ();
*38D
 ************** End of Conflict 4     *******************************************
*38E
*37E
     }
 if (Bnr.process_widget != Bnr.dp_id)		
     {
     XClearWindow (XtDisplay(Bnr.process_widget), 
 	XtWindow (Bnr.process_widget));
     BannerProcessSetup ();
     BannerProcessRefresh ();
     }
 }
 
*16E
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
*6D
 **	BannerWriteText (display, window, xpos, ypos, str, position, fieldsize)
 **
 **      This routine writes the text string in the desired coordinates 
 **	in the specified window, using the current font.
 **
 **  FORMAL PARAMETERS:
 **
 **      display		    Current display
 **	window		    Current window
 **	xpos		    X position
 **	ypos		    Y position
 **	str		    The output string
 **	Position	    1 = left Justified
 **			    2 = center Justified
 **			    3 = right Justified
 **
 **  IMPLICIT INPUTS:
 **
 **      The Bnr block
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
 BannerWriteText(display, window, xpos, ypos, str, position, fieldlen)
 Display *display;
 Window *window;
 int xpos;
 int ypos;
 char *str;
 int position;
 int fieldlen;
 {
     int i, len, clypos, height, width;
     int size;
     char *str_size;
 /*
  * Set our font, as the current writting font, and strip the input string
  * so there are no leading spaces screwing it up.
  */
 
     while (*str == ' ') 
 	str++; 
 
     size = name_size (str) - 1;
 /*
  * Kill any trailing spaces as well
  */
 	while (size > 0 && str[size-1] == ' ')
 	    {
 	    str[size-1] = '\0';
 	    size--;
 	    }
 /*
  * now if the string is longer than the field length, cut the high order
  * off before we try to display it.
  */
 
     len = XTextWidth (Bnr.font_ptr, str, size); 
     while (fieldlen < len)
 	{
 	if (size == 0)
 	    break;
 	str++;
 	size--;
 	len = XTextWidth (Bnr.font_ptr, str, size); 
 	}
 /*
  * now find the number of Pixels we need to display the string, under the 
  * current font, and calculate the start X, Y coordinates.
  */
 
     clypos = ypos - Bnr.font_ptr->ascent; 
     height = Bnr.font_height; 
 
 /*
  * now select on the justification we need to do for the string.
  */
     switch (position)
     {
 
     case 1:	/* Left Justify */
 
 	{
 /*
  * Draw the string left justified, Draw the string, and then clear the area
  * in the field size, which is left over.
  */
 	    width = fieldlen - len; 
 	    XDrawImageString (display, window, BannerGC, 
 		    xpos, ypos, str, size);
 	    xpos = xpos + len; 
 	    XSetBackground (display, 
 		    BannerGC,
 		    Bnr.foreground);
 	    XSetForeground (display, 
 		    BannerGC,
 		    Bnr.background);
 	    XFillRectangle (display, window,
 		BannerGC, xpos, clypos, width, height);
 	    XSetBackground (display, 
 		    BannerGC,
 		    Bnr.background);
 	    XSetForeground (display, 
 		    BannerGC,
 		    Bnr.foreground);
 	    break;
 	}
     case 2:	/* Center justify */
 
 	{
 /*
  * Draw the string center justified, clear the area infront of the string, 
  * draw the string, then clear the area behind it.
  */
 	    width = (fieldlen - len) / 2; 
 	    XSetBackground (display, 
 		    BannerGC,
 		    Bnr.foreground);
 	    XSetForeground (display, 
 		    BannerGC,
 		    Bnr.background);
 	    XFillRectangle (display, window,
 		BannerGC, xpos, clypos, width, height);
 	    XSetBackground (display, 
 		    BannerGC,
 		    Bnr.background);
 	    XSetForeground (display, 
 		    BannerGC,
 		    Bnr.foreground);
 	    xpos = xpos + width;
 	    XDrawImageString (display, window, BannerGC, 
 		    xpos, ypos, str, size);
 	    xpos = xpos + len; 
 	    XSetBackground (display, 
 		    BannerGC,
 		    Bnr.foreground);
 	    XSetForeground (display, 
 		    BannerGC,
 		    Bnr.background);
 	    XFillRectangle (display, window,
 		BannerGC, xpos, clypos, width, height);
 	    XSetBackground (display, 
 		    BannerGC,
 		    Bnr.background);
 	    XSetForeground (display, 
 		    BannerGC,
 		    Bnr.foreground);
 	    break;
 	}
     case 3:	/* Right Justify */
 
 	{
 /*
  * draw the string right justified, clear the area infront, then draw 
  * the string.
  */
 	    width = fieldlen - len; 
 	    XSetBackground (display, 
 		    BannerGC,
 		    Bnr.foreground);
 	    XSetForeground (display, 
 		    BannerGC,
 		    Bnr.background);
 	    XFillRectangle (display, window,
 		BannerGC, xpos, clypos, width, height);
 	    XSetBackground (display, 
 		    BannerGC,
 		    Bnr.background);
 	    XSetForeground (display, 
 		    BannerGC,
 		    Bnr.foreground);
 	    xpos = xpos + width;
 	    XDrawImageString (display, window, BannerGC, 
 		    xpos, ypos, str, size);
 	    break;
 	}
     }
 }
 
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **	BannerDigitalClock (display, window )
 **
 **      This routine handles the display of the digital clock. It obtains the 
 **	current time stamp, in ASCII, and displays it at the coordinates for 
 **	the digital clock.
 **
 **  FORMAL PARAMETERS:
 **
 **      display		The current display
 **	window		The current window
 **
 **  IMPLICIT INPUTS:
 **
 **      The Bnr block, with the display coordinates
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
 BannerDigitalClock(display, window)
 Display *display;
 Window *window;
 {
 
     long timedesc[2];
     char timebuffer[] = "                               ";
 
 /*
  * Set up the Descriptor and call the VMS SYS$ASCTIM service.
  */
     timedesc[0] = 24;
     timedesc[1] = &timebuffer;
     SYS$ASCTIM (&timedesc, &timedesc, NULL, 1);
 /*
  * now make it pretty for the user
  */
     timebuffer[8] = '\0';
     if (timebuffer[0] == '0')
 	timebuffer[0] = ' ';
 /*
  * now write the time, removing the seconds if no seconds are wanted.
  */
    
     if (! Bnr.Param_Bits.seconds)
 	{
 	    timebuffer[5] = '\0';
 	    BannerWriteText (display, window, 
 		    Bnr.clock_height,
 		    (Bnr.clock_height / 2) - ((Bnr.font_height * 2) / 3),
 		    timebuffer, 2, XTextWidth(Bnr.font_ptr, "88-MMM-8888", 11));
 	}
     else
 	BannerWriteText (display, window, 
 		    Bnr.clock_height,
 		    (Bnr.clock_height / 2) - ((Bnr.font_height * 2) / 3),
 		    timebuffer, 2, XTextWidth(Bnr.font_ptr, "88-MMM-8888", 11));
 }
 
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **	BannerWriteDate (display, window)
 **
 **      The routine obains the current day, and date, and writes this to the 
 **	desired window.
 **
 **  FORMAL PARAMETERS:
 **
 **      display	    Current display
 **	window	    Current window
 **
 **  IMPLICIT INPUTS:
 **
 **      Bnr block, with the positioning information.
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
 BannerWriteDate(display, window)
 Display *display;
 Window *window;
 {
     int daynumber;
     float daydiv;
     long timedesc[2];
     char timebuffer[]    = "                              ";
 
 /*
  * Call the system service to find out the current day, and conver that to the
  * day of this week.
  */
 
     LIB$DAY (&daynumber, NULL, NULL);
     daynumber = daynumber - ((daynumber / 7) * 7); 
 /*
  * Set up the Descriptor and call the VMS SYS$ASCTIM service.
  */
     
     timedesc[0] = 24;
     timedesc[1] = &timebuffer;
     SYS$ASCTIM (&timedesc, &timedesc, NULL, NULL);
 /*
  * now write out the day, and the date.
  */
     
     timebuffer[11] = '\0';
     BannerWriteText (display, window, 
 	Bnr.clock_height,
 	(Bnr.clock_height / 2) - ((Bnr.font_height * 2) / 3) + Bnr.font_height, 
 	BannerDayList[daynumber], 2, XTextWidth(Bnr.font_ptr, "88-MMM-8888", 11));
 
     BannerWriteText (display, window, 
 	Bnr.clock_height,
 	(Bnr.clock_height / 2) - ((Bnr.font_height * 2) / 3) + 
 	    (2 * Bnr.font_height), 
 	timebuffer, 2, XTextWidth(Bnr.font_ptr, "88-MMM-8888", 11));
 
     Bnr.ws_purged = 0;
 }
 
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
*6E
 **	BannerCheckPopupPos (widget)
 **
 **      This routine check to make sure that the current popup is not about
 **	to be placed outside the current screen. If so we re-position it 
 **	within the display.
 **
 **  FORMAL PARAMETERS:
 **
 **      The widget
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
 BannerCheckPopupPos(widget, width, height)
 Widget widget;
 int width, height;
 {
 int  x, y;
 
 	x = saveevent.xbutton.x_root ;
 	y = saveevent.xbutton.y_root;
 
 	if (saveevent.xbutton.x_root + width > 
 			    DisplayWidth(XtDisplay (widget),0))
 	    x = saveevent.xbutton.x_root - width;
 
 	if (x < 0)
 	    x = 0;
 
 	if (saveevent.xbutton.y_root + height > 
 			DisplayHeight(XtDisplay (widget),0))
 	    y = saveevent.xbutton.y_root - height;
 
*17D
 	BannerSetArg (widget, DwtNy, y);
 	BannerSetArg (widget, DwtNx, x);
*17E
*17I
 	BannerSetArg (widget, XmNy, y);
 	BannerSetArg (widget, XmNx, x);
*17E
 }
 
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **	BannerGetResource (db resource, zero, resourcetype,
*29D
 **			   resourcevalue)
*29E
*29I
 **			   resourcevalue, flag)
*29E
 **
 **      This routine, reads an X resource from the system, or private database
 **	The private has peference over the system, for any resource.
 **
 **  FORMAL PARAMETERS:
 **
 **      db		    database
 **	resource	    Resource to read
 **	zero		    Addition param
 **	resourcetype	    returned resource type
 **	resourcevalue	    returned resource value
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
 static int
*29D
 BannerGetResource(db, resource, zero, resourcetype, resourcevalue)
*29E
*29I
 BannerGetResource(db, resource, zero, resourcetype, resourcevalue, flag)
*29E
 int db, zero;
 char *resource;
 XrmValue  *resourcevalue;
 XrmRepresentation *resourcetype;
*29I
 int flag;
*29E
 {
 int sts;
*29I
 char resname[512];
*29E
 /*
  * Read the open resource databases, first read the privdb, if that fails go
  * and try the appdb.
  */
*29D
 
     return XrmGetResource (db, resource, zero, resourcetype, resourcevalue);
*29E
*29I
     if (flag == 1)
 	sprintf (resname, "DECW$BANNER.%s", resource);
     else
 	sprintf (resname, "DECW$BANNER_%s.%s", BannerVmsNodeName, resource);
 
     return XrmGetResource (db, resname, zero, resourcetype, resourcevalue);
*29E
 }
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **	BannerReadResource ()
 **
 **      This routine reads all the Banner resource from the resource file,
 **	and loads them into ouir in memory Bnr data structure.
 **
 **  FORMAL PARAMETERS:
 **
 **      none
 **
 **  IMPLICIT INPUTS:
 **
 **      The Bnr data block.
 **
 **  IMPLICIT OUTPUTS:
 **
 **      The Bnr data block
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
 BannerReadResource(flag)
 int flag;
 {
     XrmValue	resourcevalue;
     XrmRepresentation resourcetype;
     int db;
 
 /*
  * First initialise the Resource manager, and try to open the resource databases
  * we are interested in.
  */
 
     if (flag == 1) 
*18D
 	db = XrmGetFileDatabase ("SYS$LIBRARY:DECW$BANNER.DAT");
*18E
*18I
 	db = XrmGetFileDatabase ("DECW$SYSTEM_DEFAULTS:DECW$BANNER.DAT");
*18E
*24D
     else
 	db = XtDatabase (XtDisplay (Bnr.dp_id));
*24E
*24I
     else 
*29D
 	if (flag == 2)
*29E
*25I
*29D
 	    db = XtDatabase (XtDisplay (Bnr.dp_id));
 	else
*29E
*25E
*29D
 	    {
 	    char    nodedb[100];
 
 	    sprintf (nodedb, "DECW$USER_DEFAULTS:DECW$BANNER_%s.DAT",
 		BannerVmsNodeName);
 	    db = XrmGetFileDatabase (nodedb);
 	    }
*29E
*29I
 	db = XtDatabase (XtDisplay (Bnr.dp_id));
 
*29E
*25D
 	else
 	    db = XtDatabase (XtDisplay (Bnr.dp_id));
*25E
*24E
 
 
 
     if (db == 0)
 	return;
 
 
 /*
  * Now start reading our resources, and see what we can get from the 
  * files.
  */
*29D
     if (BannerGetResource (db, "DECW$BANNER.font_template", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
*41I
     if (BannerGetResource (db, "node_display_name", 0, &resourcetype,
 	    &resourcevalue, flag))
 	{
 	BannerVmsDisplayName = (char *) XtMalloc (resourcevalue.size + 1);
 	strcpy(BannerVmsDisplayName, resourcevalue.addr);
 	}
 
*41E
     if (BannerGetResource (db, "font_template", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	{
*31D
 	Bnr.fnt_template = (char *) XtMalloc (resourcevalue.size);
*31E
*31I
 	Bnr.fnt_template = (char *) XtMalloc (resourcevalue.size + 1);
*31E
 	strcpy(Bnr.fnt_template, resourcevalue.addr);
 	Bnr.fnt_name = (char *) XtMalloc (100);
 	strcpy (banner_font_name, "Helvetica");
 	strcpy (banner_font_size, "10");
 	strcpy (banner_font_wieght, "medium");
 	strcpy (banner_font_type, "P");
 	sprintf (Bnr.fnt_name, Bnr.fnt_template, 
 	    banner_font_name, banner_font_wieght, banner_font_size,
 		banner_font_type);
 	}
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.font_family_name", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "font_family_name", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	{
 	strcpy (banner_font_wieght, "medium");
 	strcpy (banner_font_type, "P");
 	strcpy (banner_font_size, "10");
 	strcpy(banner_font_name, resourcevalue.addr);
 	if (strcmp(banner_font_name, "ITC Avant Garde Gothic") == 0 ||
 	    strcmp(banner_font_name, "ITC Lubalin Graph") == 0 ||
 	    strcmp(banner_font_name, "ITC Souvenir") == 0)
 		strcpy (banner_font_wieght, "demi");
 	if (strcmp(banner_font_name, "Courier") == 0)
 	    strcpy (banner_font_type, "M");
 
 	sprintf (Bnr.fnt_name, Bnr.fnt_template, 
 	    banner_font_name, banner_font_wieght, banner_font_size,
 		banner_font_type);
 	}
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.font_family_size", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "font_family_size", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	{
 	strcpy(banner_font_size, resourcevalue.addr);
 	sprintf (Bnr.fnt_name, Bnr.fnt_template, 
 	    banner_font_name, banner_font_wieght, banner_font_size,
 		banner_font_type);
 	}
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.title", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
*33I
     if (BannerGetResource (db, "volume", 0, &resourcetype,
 	    &resourcevalue, flag))
 	Son.Volume = BannerStrToNumeric (resourcevalue.addr);
 
     if (BannerGetResource (db, "time", 0, &resourcetype,
 	    &resourcevalue, flag))
 	Son.Time = BannerStrToNumeric (resourcevalue.addr);
 
     if (BannerGetResource (db, "mwait_sound", 0, &resourcetype,
 	    &resourcevalue, flag))
 	{
 	BannerSetSound (&Son.Mwait, resourcevalue.addr);
 	}
 
     if (BannerGetResource (db, "newprocesses_sound", 0, &resourcetype,
 	    &resourcevalue, flag))
 	{
 	BannerSetSound (&Son.NewProcesses, resourcevalue.addr);
 	}
 
     if (BannerGetResource (db, "lessprocesses_sound", 0, &resourcetype,
 	    &resourcevalue, flag))
 	{
 	BannerSetSound (&Son.LessProcesses, resourcevalue.addr);
 	}
 
     if (BannerGetResource (db, "disk1_sound", 0, &resourcetype,
 	    &resourcevalue, flag))
 	{
 	BannerSetSound (&Son.Disk1, resourcevalue.addr);
 	}
 
     if (BannerGetResource (db, "disk2_sound", 0, &resourcetype,
 	    &resourcevalue, flag))
 	{
 	BannerSetSound (&Son.Disk2, resourcevalue.addr);
 	}
 
     if (BannerGetResource (db, "disk3_sound", 0, &resourcetype,
 	    &resourcevalue, flag))
 	{
 	BannerSetSound (&Son.Disk3, resourcevalue.addr);
 	}
 
     if (BannerGetResource (db, "memory_sound", 0, &resourcetype,
 	    &resourcevalue, flag))
 	{
 	BannerSetSound (&Son.Memory, resourcevalue.addr);
 	}
 
     if (BannerGetResource (db, "pagefile_sound", 0, &resourcetype,
 	    &resourcevalue, flag))
 	{
 	BannerSetSound (&Son.PageFile, resourcevalue.addr);
 	}
 
     if (BannerGetResource (db, "pendingio_sound", 0, &resourcetype,
 	    &resourcevalue, flag))
 	{
 	BannerSetSound (&Son.PendingIO, resourcevalue.addr);
 	}
 
*33E
     if (BannerGetResource (db, "title", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	{
*31D
 	Bnr.title = (char *) XtMalloc (resourcevalue.size);
*31E
*31I
 	Bnr.title = (char *) XtMalloc (resourcevalue.size + 1);
*31E
 	strcpy(Bnr.title, resourcevalue.addr);
 	}
 
*8I
*29D
     if (BannerGetResource (db, "DECW$BANNER.highlight_colour", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "highlight_colour", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	{
*31D
 	Bnr.hwmrk_colour = (char *) XtMalloc (resourcevalue.size);
*31E
*31I
 	Bnr.hwmrk_colour = (char *) XtMalloc (resourcevalue.size + 1);
*31E
 	strcpy(Bnr.hwmrk_colour, resourcevalue.addr);
 	}
 
*8E
*29D
     if (BannerGetResource (db, "DECW$BANNER.foreground", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "foreground", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	{
*31D
 	Bnr.foreground_colour = (char *) XtMalloc (resourcevalue.size);
*31E
*31I
 	Bnr.foreground_colour = (char *) XtMalloc (resourcevalue.size + 1);
*31E
 	strcpy(Bnr.foreground_colour, resourcevalue.addr);
 	}
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.background", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "background", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	{
*31D
 	Bnr.background_colour = (char *) XtMalloc (resourcevalue.size);
*31E
*31I
 	Bnr.background_colour = (char *) XtMalloc (resourcevalue.size + 1);
*31E
 	strcpy(Bnr.background_colour, resourcevalue.addr);
 	}
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.title", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "title", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	{
*31D
 	Bnr.title = (char *) XtMalloc (resourcevalue.size);
*31E
*31I
 	Bnr.title = (char *) XtMalloc (resourcevalue.size + 1);
*31E
 	strcpy(Bnr.title, resourcevalue.addr);
 	}
 
*19I
*29D
     if (BannerGetResource (db, "DECW$BANNER.qp_name", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "qp_name", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	{
*31D
 	Bnr.qp_name = (char *) XtMalloc (resourcevalue.size);
*31E
*31I
 	Bnr.qp_name = (char *) XtMalloc (resourcevalue.size + 1);
*31E
 	strcpy(Bnr.qp_name, resourcevalue.addr);
 	}
     else
 	{
*20D
 	Bnr.qp_name = (char *) XtMaloc (6);
*20E
*20I
 	Bnr.qp_name = (char *) XtMalloc (6);
*20E
 	strcpy(Bnr.qp_name, "*LPS*");
 	}
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.qb_name", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "qb_name", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	{
*31D
 	Bnr.qb_name = (char *) XtMalloc (resourcevalue.size);
*31E
*31I
 	Bnr.qb_name = (char *) XtMalloc (resourcevalue.size + 1);
*31E
 	strcpy(Bnr.qb_name, resourcevalue.addr);
 	}
     else
 	{
*20D
 	Bnr.qb_name = (char *) XtMaloc (8);
*20E
*20I
 	Bnr.qb_name = (char *) XtMalloc (8);
*20E
 	strcpy(Bnr.qb_name, "*BATCH*");
 	}
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.queue_user", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "queue_user", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	{
*31D
 	Bnr.queue_user = (char *) XtMalloc (resourcevalue.size);
*31E
*31I
 	Bnr.queue_user = (char *) XtMalloc (resourcevalue.size + 1);
*31E
 	strcpy(Bnr.queue_user, resourcevalue.addr);
 	}
     else
 	{
*20D
 	Bnr.queue_user = (char *) XtMaloc (strlen (getenv("USER")));
*20E
*20I
*31D
 	Bnr.queue_user = (char *) XtMalloc (strlen (getenv("USER")));
*31E
*31I
 	Bnr.queue_user = (char *) XtMalloc (strlen (getenv("USER")) + 1);
*31E
*20E
 	strcpy(Bnr.queue_user, getenv("USER"));
 	}
 
*19E
*29D
     if (BannerGetResource (db, "DECW$BANNER.disk_0_name", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "disk_0_name", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	{
*31D
 	Bnr.disk_0_name = (char *) XtMalloc (resourcevalue.size);
*31E
*31I
 	Bnr.disk_0_name = (char *) XtMalloc (resourcevalue.size + 1);
*31E
 	strcpy(Bnr.disk_0_name, resourcevalue.addr);
 	}
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.disk_1_name", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "disk_1_name", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	{
*31D
 	Bnr.disk_1_name = (char *) XtMalloc (resourcevalue.size);
*31E
*31I
 	Bnr.disk_1_name = (char *) XtMalloc (resourcevalue.size + 1);
*31E
 	strcpy(Bnr.disk_1_name, resourcevalue.addr);
 	}
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.disk_2_name", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "disk_2_name", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	{
*31D
 	Bnr.disk_2_name = (char *) XtMalloc (resourcevalue.size);
*31E
*31I
 	Bnr.disk_2_name = (char *) XtMalloc (resourcevalue.size + 1);
*31E
 	strcpy(Bnr.disk_2_name, resourcevalue.addr);
 	}
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.disk_0", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "disk_0", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	{
*31D
 	Bnr.disk_0 = (char *) XtMalloc (resourcevalue.size);
*31E
*31I
 	Bnr.disk_0 = (char *) XtMalloc (resourcevalue.size + 1);
*31E
 	strcpy(Bnr.disk_0, resourcevalue.addr);
 	}
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.disk_1", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "disk_1", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	{
*31D
 	Bnr.disk_1 = (char *) XtMalloc (resourcevalue.size);
*31E
*31I
 	Bnr.disk_1 = (char *) XtMalloc (resourcevalue.size + 1);
*31E
 	strcpy(Bnr.disk_1, resourcevalue.addr);
 	}
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.disk_2", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "disk_2", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	{
*31D
 	Bnr.disk_2 = (char *) XtMalloc (resourcevalue.size);
*31E
*31I
 	Bnr.disk_2 = (char *) XtMalloc (resourcevalue.size + 1);
*31E
 	strcpy(Bnr.disk_2, resourcevalue.addr);
 	}
 
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.cpu_update", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "cpu_update", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.cpu_update = BannerStrToNumeric (resourcevalue.addr);
 
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.process_update", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "process_update", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.process_update = BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.monitor_update", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
*35D
*34A1D
     if (BannerGetResource (db, "monitor_update", 0, &resourcetype,
*34A1E
*34A1I
     if (BannerGetResource (db, "link_update", 0, &resourcetype,
*34A1E
*35E
*35I
*36D
     if (BannerGetResource (db, "link_update", 0, &resourcetype,
*36E
*36I
*37I
*38D
 **************** Conflict 5     ************************************************
*38E
*37E
     if (BannerGetResource (db, "monitor_update", 0, &resourcetype,
*36E
*35E
*37D
 	    &resourcevalue, flag))
*37E
*37I
*38D
 ********************************************************************************
*38E
*38I
 	    &resourcevalue, flag))
 	Bnr.monitor_update = BannerStrToNumeric (resourcevalue.addr);
 
 
*38E
     if (BannerGetResource (db, "link_update", 0, &resourcetype,
*38D
 ************** End of Conflict 5     *******************************************
 	    &resourcevalue, flag))
 **************** Conflict 6     ************************************************
*38E
*37E
*29E
*35D
*34A1D
 	Bnr.monitor_update = BannerStrToNumeric (resourcevalue.addr);
*34A1E
*34A1I
 	Bnr.link_update = BannerStrToNumeric (resourcevalue.addr);
*34A1E
*35E
*35I
*36D
 	Bnr.link_update = BannerStrToNumeric (resourcevalue.addr);
*36E
*36I
*38D
 	Bnr.monitor_update = BannerStrToNumeric (resourcevalue.addr);
*38E
*36E
*35E
*37I
*38D
 ********************************************************************************
*38E
*38I
 	    &resourcevalue, flag))
*38E
 	Bnr.link_update = BannerStrToNumeric (resourcevalue.addr);
*38D
 ************** End of Conflict 6     *******************************************
*38E
*37E
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.pe_update", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "pe_update", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.pe_update = BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.lck_update", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "lck_update", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.lck_update = BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.cwps_update", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "cwps_update", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.cwps_update = BannerStrToNumeric (resourcevalue.addr);
 
*7I
*8I
*29D
     if (BannerGetResource (db, "DECW$BANNER.sys_ioratemax", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "sys_ioratemax", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.sys_ioratemax = BannerStrToNumeric (resourcevalue.addr);
 
*12I
*29D
     if (BannerGetResource (db, "DECW$BANNER.sysrespgfile", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "sysrespgfile", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.sysrespgfile = BannerStrToNumeric (resourcevalue.addr);
 
*12E
*29D
     if (BannerGetResource (db, "DECW$BANNER.sys_faultratemax", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "sys_faultratemax", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
*9D
 	Bnr.sys_ioratemax = BannerStrToNumeric (resourcevalue.addr);
*9E
*9I
 	Bnr.sys_faultratemax = BannerStrToNumeric (resourcevalue.addr);
*9E
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.sys_highlight", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "sys_highlight", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.sys_highlight = BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.hwmrk_update", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "hwmrk_update", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.hwmrk_update = BannerStrToNumeric (resourcevalue.addr);
 
*8E
*29D
     if (BannerGetResource (db, "DECW$BANNER.sys_update", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "sys_update", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.sys_update = BannerStrToNumeric (resourcevalue.addr);
 
*7E
*19I
*29D
     if (BannerGetResource (db, "DECW$BANNER.tp_update", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "tp_update", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.tp_update = BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.qp_update", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "qp_update", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.qp_update = BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.qb_update", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "qb_update", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.qb_update = BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.clu_update", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "clu_update", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.clu_update = BannerStrToNumeric (resourcevalue.addr);
 
*19E
*29D
     if (BannerGetResource (db, "DECW$BANNER.cpu_count", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "cpu_count", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.cpu_count = BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.clock_x", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "clock_x", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.clock_x = BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.clock_y", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "clock_y", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.clock_y = BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.message_x", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "message_x", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.message_x = BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.message_y", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "message_y", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.message_y = BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.cube_x", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "cube_x", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.cube_x = BannerStrToNumeric (resourcevalue.addr);
*15D
 
*15E
*29D
     if (BannerGetResource (db, "DECW$BANNER.cube_y", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "cube_y", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.cube_y = BannerStrToNumeric (resourcevalue.addr);
 
*15I
*29D
     if (BannerGetResource (db, "DECW$BANNER.hanoi_x", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "hanoi_x", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.hanoi_x = BannerStrToNumeric (resourcevalue.addr);
*29D
     if (BannerGetResource (db, "DECW$BANNER.hanoi_y", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "hanoi_y", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.hanoi_y = BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.globe_x", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "globe_x", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.globe_x = BannerStrToNumeric (resourcevalue.addr);
*29D
     if (BannerGetResource (db, "DECW$BANNER.globe_y", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "globe_y", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.globe_y = BannerStrToNumeric (resourcevalue.addr);
 
*15E
*29D
     if (BannerGetResource (db, "DECW$BANNER.cpu_x", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "cpu_x", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.cpu_x = BannerStrToNumeric (resourcevalue.addr);
*15D
 
*15E
*29D
     if (BannerGetResource (db, "DECW$BANNER.cpu_y", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "cpu_y", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.cpu_y = BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.pe_x", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "pe_x", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.pe_x = BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.pe_y", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "pe_y", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.pe_y = BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.lck_x", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "lck_x", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.lck_x = BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.lck_y", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "lck_y", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.lck_y = BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.cwps_x", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "cwps_x", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.cwps_x = BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.cwps_y", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "cwps_y", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.cwps_y = BannerStrToNumeric (resourcevalue.addr);
 
*7I
*29D
     if (BannerGetResource (db, "DECW$BANNER.sys_x", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "sys_x", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.sys_x = BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.sys_y", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "sys_y", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.sys_y = BannerStrToNumeric (resourcevalue.addr);
 
*7E
*19I
*29D
     if (BannerGetResource (db, "DECW$BANNER.tp_x", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "tp_x", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.tp_x = BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.tp_y", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "tp_y", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.tp_y = BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.qp_x", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "qp_x", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.qp_x = BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.qp_y", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "qp_y", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.qp_y = BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.qb_x", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "qb_x", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.qb_x = BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.qb_y", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "qb_y", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.qb_y = BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.clu_x", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "clu_x", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.clu_x = BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.clu_y", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "clu_y", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.clu_y = BannerStrToNumeric (resourcevalue.addr);
 
*19E
*29D
     if (BannerGetResource (db, "DECW$BANNER.mon_x", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
*35D
*34A1D
     if (BannerGetResource (db, "mon_x", 0, &resourcetype,
*34A1E
*34A1I
     if (BannerGetResource (db, "link_x", 0, &resourcetype,
*34A1E
*35E
*35I
*36D
     if (BannerGetResource (db, "link_x", 0, &resourcetype,
*36E
*36I
*37I
*38D
 **************** Conflict 7     ************************************************
*38E
*37E
     if (BannerGetResource (db, "mon_x", 0, &resourcetype,
*36E
*35E
*37D
 	    &resourcevalue, flag))
*37E
*37I
*38D
 ********************************************************************************
     if (BannerGetResource (db, "link_x", 0, &resourcetype,
 ************** End of Conflict 7     *******************************************
 	    &resourcevalue, flag))
 **************** Conflict 8     ************************************************
*38E
*38I
 	    &resourcevalue, flag))
*38E
*37E
*29E
*35D
*34A1D
 	Bnr.mon_x = BannerStrToNumeric (resourcevalue.addr);
*34A1E
*34A1I
 	Bnr.link_x = BannerStrToNumeric (resourcevalue.addr);
*34A1E
*35E
*35I
*36D
 	Bnr.link_x = BannerStrToNumeric (resourcevalue.addr);
*36E
*36I
 	Bnr.mon_x = BannerStrToNumeric (resourcevalue.addr);
*36E
*35E
*37D
 
*37E
*37I
*38D
 ********************************************************************************
 	Bnr.link_x = BannerStrToNumeric (resourcevalue.addr);
 ************** End of Conflict 8     *******************************************
 
 **************** Conflict 9     ************************************************
*38E
*38I
 
*38E
*37E
*29D
     if (BannerGetResource (db, "DECW$BANNER.mon_y", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
*35D
*34A1D
     if (BannerGetResource (db, "mon_y", 0, &resourcetype,
*34A1E
*34A1I
     if (BannerGetResource (db, "link_y", 0, &resourcetype,
*34A1E
*35E
*35I
*36D
     if (BannerGetResource (db, "link_y", 0, &resourcetype,
*36E
*36I
     if (BannerGetResource (db, "mon_y", 0, &resourcetype,
*36E
*35E
*37D
 	    &resourcevalue, flag))
*37E
*37I
*38D
 ********************************************************************************
     if (BannerGetResource (db, "link_y", 0, &resourcetype,
 ************** End of Conflict 9     *******************************************
 	    &resourcevalue, flag))
 **************** Conflict 10    ************************************************
*38E
*38I
 	    &resourcevalue, flag))
*38E
*37E
*29E
*35D
*34A1D
 	Bnr.mon_y = BannerStrToNumeric (resourcevalue.addr);
*34A1E
*34A1I
 	Bnr.link_y = BannerStrToNumeric (resourcevalue.addr);
*34A1E
*35E
*35I
*36D
 	Bnr.link_y = BannerStrToNumeric (resourcevalue.addr);
*36E
*36I
 	Bnr.mon_y = BannerStrToNumeric (resourcevalue.addr);
*36E
*35E
*37I
*38D
 ********************************************************************************
*38E
*38I
 
     if (BannerGetResource (db, "link_x", 0, &resourcetype,
 	    &resourcevalue, flag))
 	Bnr.link_x = BannerStrToNumeric (resourcevalue.addr);
 
     if (BannerGetResource (db, "link_y", 0, &resourcetype,
 	    &resourcevalue, flag))
*38E
 	Bnr.link_y = BannerStrToNumeric (resourcevalue.addr);
*38D
 ************** End of Conflict 10    *******************************************
*38E
*37E
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.process_x", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "process_x", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.process_x = BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.process_y", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "process_y", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.process_y = BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.clock_height", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "clock_height", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.clock_height = BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.clock_width", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "clock_width", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.clock_width = BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.message_height", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "message_height", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.message_height = BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.message_width", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "message_width", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.message_width = BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.message_init_state", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "message_init_state", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.message_init_state = BannerStrToNumeric (resourcevalue.addr);
 
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.cube_height", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "cube_height", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.cube_height = BannerStrToNumeric (resourcevalue.addr);
*15D
 
*15E
*29D
     if (BannerGetResource (db, "DECW$BANNER.cube_width", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "cube_width", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.cube_width = BannerStrToNumeric (resourcevalue.addr);
 
*15I
*29D
     if (BannerGetResource (db, "DECW$BANNER.globe_height", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "globe_height", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.globe_height = BannerStrToNumeric (resourcevalue.addr);
*29D
     if (BannerGetResource (db, "DECW$BANNER.globe_width", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "globe_width", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.globe_width = BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.hanoi_height", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "hanoi_height", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.hanoi_height = BannerStrToNumeric (resourcevalue.addr);
*29D
     if (BannerGetResource (db, "DECW$BANNER.hanoi_width", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "hanoi_width", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.hanoi_width = BannerStrToNumeric (resourcevalue.addr);
 
*15E
*29D
     if (BannerGetResource (db, "DECW$BANNER.cpu_height", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "cpu_height", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.cpu_height = BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.cpu_width", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "cpu_width", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.cpu_width = BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.mon_height", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
*35D
*34A1D
     if (BannerGetResource (db, "mon_height", 0, &resourcetype,
*34A1E
*34A1I
     if (BannerGetResource (db, "link_height", 0, &resourcetype,
*34A1E
*35E
*35I
*36D
     if (BannerGetResource (db, "link_height", 0, &resourcetype,
*36E
*36I
*37I
*38D
 **************** Conflict 11    ************************************************
*38E
*37E
     if (BannerGetResource (db, "mon_height", 0, &resourcetype,
*36E
*35E
*37D
 	    &resourcevalue, flag))
*37E
*37I
*38D
 ********************************************************************************
     if (BannerGetResource (db, "link_height", 0, &resourcetype,
 ************** End of Conflict 11    *******************************************
 	    &resourcevalue, flag))
 **************** Conflict 12    ************************************************
*38E
*38I
 	    &resourcevalue, flag))
*38E
*37E
*29E
*35D
*34A1D
 	Bnr.mon_height = BannerStrToNumeric (resourcevalue.addr);
*34A1E
*34A1I
 	Bnr.link_height = BannerStrToNumeric (resourcevalue.addr);
*34A1E
*35E
*35I
*36D
 	Bnr.link_height = BannerStrToNumeric (resourcevalue.addr);
*36E
*36I
 	Bnr.mon_height = BannerStrToNumeric (resourcevalue.addr);
*36E
*35E
*37D
 
*37E
*37I
*38D
 ********************************************************************************
 	Bnr.link_height = BannerStrToNumeric (resourcevalue.addr);
 ************** End of Conflict 12    *******************************************
 
 **************** Conflict 13    ************************************************
*38E
*38I
 
*38E
*37E
*29D
     if (BannerGetResource (db, "DECW$BANNER.mon_width", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
*35D
*34A1D
     if (BannerGetResource (db, "mon_width", 0, &resourcetype,
*34A1E
*34A1I
     if (BannerGetResource (db, "link_width", 0, &resourcetype,
*34A1E
*35E
*35I
*36D
     if (BannerGetResource (db, "link_width", 0, &resourcetype,
*36E
*36I
     if (BannerGetResource (db, "mon_width", 0, &resourcetype,
*36E
*35E
*37D
 	    &resourcevalue, flag))
*37E
*37I
*38D
 ********************************************************************************
     if (BannerGetResource (db, "link_width", 0, &resourcetype,
 ************** End of Conflict 13    *******************************************
 	    &resourcevalue, flag))
 **************** Conflict 14    ************************************************
*38E
*38I
 	    &resourcevalue, flag))
*38E
*37E
*29E
*35D
*34A1D
 	Bnr.mon_width = BannerStrToNumeric (resourcevalue.addr);
*34A1E
*34A1I
 	Bnr.link_width = BannerStrToNumeric (resourcevalue.addr);
*34A1E
*35E
*35I
*36D
 	Bnr.link_width = BannerStrToNumeric (resourcevalue.addr);
*36E
*36I
 	Bnr.mon_width = BannerStrToNumeric (resourcevalue.addr);
*36E
*35E
*37I
*38D
 ********************************************************************************
*38E
*38I
 
     if (BannerGetResource (db, "link_height", 0, &resourcetype,
 	    &resourcevalue, flag))
 	Bnr.link_height = BannerStrToNumeric (resourcevalue.addr);
 
     if (BannerGetResource (db, "link_width", 0, &resourcetype,
 	    &resourcevalue, flag))
*38E
 	Bnr.link_width = BannerStrToNumeric (resourcevalue.addr);
*38D
 ************** End of Conflict 14    *******************************************
*38E
*37E
 
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.pe_height", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "pe_height", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.pe_height = BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.pe_width", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "pe_width", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.pe_width = BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.lck_height", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "lck_height", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.lck_height = BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.lck_width", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "lck_width", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.lck_width = BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.cwps_width", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "cwps_width", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.cwps_width = BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.cwps_height", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "cwps_height", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.cwps_height = BannerStrToNumeric (resourcevalue.addr);
 
*7I
*29D
     if (BannerGetResource (db, "DECW$BANNER.sys_width", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "sys_width", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.sys_width = BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.sys_height", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "sys_height", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.sys_height = BannerStrToNumeric (resourcevalue.addr);
 
*7E
*19I
*29D
     if (BannerGetResource (db, "DECW$BANNER.tp_width", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "tp_width", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.tp_width = BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.tp_height", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "tp_height", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.tp_height = BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.qp_width", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "qp_width", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.qp_width = BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.qp_height", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "qp_height", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.qp_height = BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.qb_width", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "qb_width", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.qb_width = BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.qb_height", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "qb_height", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.qb_height = BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.clu_width", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "clu_width", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.clu_width = BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.clu_height", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "clu_height", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.clu_height = BannerStrToNumeric (resourcevalue.addr);
 
*19E
*29D
     if (BannerGetResource (db, "DECW$BANNER.process_height", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "process_height", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.process_height = BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.process_width", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "process_width", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.process_width = BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.border", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "border", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.Param_Bits.border = BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.window_border", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "window_border", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.Param_Bits.window_border = BannerStrToNumeric (resourcevalue.addr);
 
*3I
*29D
     if (BannerGetResource (db, "DECW$BANNER.freeiocnt", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "freeiocnt", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.Param_Bits.freeiocnt = BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.freeblkcnt", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "freeblkcnt", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.Param_Bits.freeblkcnt = BannerStrToNumeric (resourcevalue.addr);
 
*3E
*29D
     if (BannerGetResource (db, "DECW$BANNER.cube", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "cube", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.Param_Bits.cube = BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.world", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "world", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.Param_Bits.world= BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.process_name", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "process_name", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.Param_Bits.process_name= BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.process_user", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "process_user", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.Param_Bits.process_user= BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.process_pid", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "process_pid", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.Param_Bits.process_pid= BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.bars", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "bars", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.Param_Bits.bars= BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.process_states", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "process_states", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.Param_Bits.process_states= BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.iocnt", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "iocnt", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.Param_Bits.iocnt= BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.monitor", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
*35D
*34A1D
     if (BannerGetResource (db, "monitor", 0, &resourcetype,
*34A1E
*34A1I
     if (BannerGetResource (db, "link_window", 0, &resourcetype,
*34A1E
*35E
*35I
*36D
     if (BannerGetResource (db, "link_window", 0, &resourcetype,
*36E
*36I
*37I
*38D
 **************** Conflict 15    ************************************************
*38E
*37E
     if (BannerGetResource (db, "monitor", 0, &resourcetype,
*36E
*35E
*37D
 	    &resourcevalue, flag))
*37E
*37I
*38D
 ********************************************************************************
*38E
*38I
 	    &resourcevalue, flag))
 	Bnr.Param_Bits.monitor= BannerStrToNumeric (resourcevalue.addr);
 
*38E
     if (BannerGetResource (db, "link_window", 0, &resourcetype,
*38D
 ************** End of Conflict 15    *******************************************
 	    &resourcevalue, flag))
 **************** Conflict 16    ************************************************
*38E
*37E
*29E
*35D
*34A1D
 	Bnr.Param_Bits.monitor= BannerStrToNumeric (resourcevalue.addr);
*34A1E
*34A1I
 	Bnr.Param_Bits.link_window = BannerStrToNumeric (resourcevalue.addr);
*34A1E
*35E
*35I
*36D
 	Bnr.Param_Bits.link_window = BannerStrToNumeric (resourcevalue.addr);
*36E
*36I
*38D
 	Bnr.Param_Bits.monitor= BannerStrToNumeric (resourcevalue.addr);
*38E
*36E
*35E
*37I
*38D
 ********************************************************************************
*38E
*38I
 	    &resourcevalue, flag))
*38E
 	Bnr.Param_Bits.link_window = BannerStrToNumeric (resourcevalue.addr);
*38D
 ************** End of Conflict 16    *******************************************
*38E
*37E
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.process_window", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "process_window", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.Param_Bits.process_window= BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.message_hide_timer", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "message_hide_timer", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 		BannerMessageSetHide (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.message_size", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "message_size", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 		BannerMessageSetSize (BannerStrToNumeric (resourcevalue.addr));
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.pe_window", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "pe_window", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.Param_Bits.pe_window= BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.lck_window", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "lck_window", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.Param_Bits.lck_window= BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.cwps_window", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "cwps_window", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.Param_Bits.cwps_window= BannerStrToNumeric (resourcevalue.addr);
 
*7I
*29D
     if (BannerGetResource (db, "DECW$BANNER.sys_window", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "sys_window", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.Param_Bits.sys_window= BannerStrToNumeric (resourcevalue.addr);
 
*7E
*19I
*29D
     if (BannerGetResource (db, "DECW$BANNER.tp_window", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "tp_window", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.Param_Bits.tp_window= BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.tp_max_rate", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "tp_max_rate", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.tp_max_rate= BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.tp_rate1_start", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "tp_rate1_start", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.tp_rate1_start= BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.tp_rate2_start", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "tp_rate2_start", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.tp_rate2_start= BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.tp_rate3_start", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "tp_rate3_start", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.tp_rate3_start= BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.tp_rate4_start", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "tp_rate4_start", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.tp_rate4_start= BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.tp_rate1_end", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "tp_rate1_end", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.tp_rate1_end= BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.tp_rate2_end", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "tp_rate2_end", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.tp_rate2_end= BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.tp_rate3_end", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "tp_rate3_end", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.tp_rate3_end= BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.total_com_rate", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "total_com_rate", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.Param_Bits.total_com_rate= BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.onef_comp_rate", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "onef_comp_rate", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.Param_Bits.onef_comp_rate= BannerStrToNumeric (resourcevalue.addr);
*29D
     if (BannerGetResource (db, "DECW$BANNER.start_rate", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "start_rate", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.Param_Bits.start_rate= BannerStrToNumeric (resourcevalue.addr);
*29D
     if (BannerGetResource (db, "DECW$BANNER.prep_rate", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "prep_rate", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.Param_Bits.prep_rate= BannerStrToNumeric (resourcevalue.addr);
*29D
     if (BannerGetResource (db, "DECW$BANNER.abort_rate", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "abort_rate", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.Param_Bits.abort_rate= BannerStrToNumeric (resourcevalue.addr);
*29D
     if (BannerGetResource (db, "DECW$BANNER.end_rate", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "end_rate", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.Param_Bits.end_rate= BannerStrToNumeric (resourcevalue.addr);
*29D
     if (BannerGetResource (db, "DECW$BANNER.rem_start_rate", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "rem_start_rate", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.Param_Bits.rem_start_rate= BannerStrToNumeric (resourcevalue.addr);
*29D
     if (BannerGetResource (db, "DECW$BANNER.rem_end_rate", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "rem_end_rate", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.Param_Bits.rem_end_rate= BannerStrToNumeric (resourcevalue.addr);
*29D
     if (BannerGetResource (db, "DECW$BANNER.comp_rate1", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "comp_rate1", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.Param_Bits.comp_rate1= BannerStrToNumeric (resourcevalue.addr);
*29D
     if (BannerGetResource (db, "DECW$BANNER.comp_rate2", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "comp_rate2", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.Param_Bits.comp_rate2= BannerStrToNumeric (resourcevalue.addr);
*29D
     if (BannerGetResource (db, "DECW$BANNER.comp_rate3", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "comp_rate3", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.Param_Bits.comp_rate3= BannerStrToNumeric (resourcevalue.addr);
*29D
     if (BannerGetResource (db, "DECW$BANNER.comp_rate4", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "comp_rate4", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.Param_Bits.comp_rate4= BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.qp_window", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "qp_window", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.Param_Bits.qp_window= BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.qb_window", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "qb_window", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.Param_Bits.qb_window= BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.clu_window", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "clu_window", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.Param_Bits.clu_window= BannerStrToNumeric (resourcevalue.addr);
 
*19E
*29D
     if (BannerGetResource (db, "DECW$BANNER.cpu", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "cpu", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.Param_Bits.cpu_window= BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.message_window", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "message_window", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.Param_Bits.message_window= BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.modes", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "modes", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.Param_Bits.modes_window= BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.multi_hist", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "multi_hist", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.Param_Bits.cpu_multi_hist = BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.modes", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "modes", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.Param_Bits.modes_window= BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.hanoi", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "hanoi", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.Param_Bits.hanoi= BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.seconds", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "seconds", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.Param_Bits.seconds= BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.digital_clock", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "digital_clock", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.Param_Bits.digital_clock= BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.analog_clock", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "analog_clock", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.Param_Bits.analog_clock= BannerStrToNumeric (resourcevalue.addr);
 
*19I
*29D
     if (BannerGetResource (db, "DECW$BANNER.all_users", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "all_users", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.Param_Bits.all_users= BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.jobs_on_hold", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "jobs_on_hold", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.Param_Bits.jobs_on_hold= BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.stopped_queues", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "stopped_queues", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.Param_Bits.stopped_queues= BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.int_jobs", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "int_jobs", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.Param_Bits.int_jobs= BannerStrToNumeric (resourcevalue.addr);
 
 
*19E
*29D
     if (BannerGetResource (db, "DECW$BANNER.dua0", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "dua0", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Mon.dua0= BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.dua1", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "dua1", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Mon.dua1= BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.dua2", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "dua2", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Mon.dua2= BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.pio", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "pio", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Mon.pio= BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.pf", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "pf", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Mon.pf= BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.sysflt", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "sysflt", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Mon.sysflt= BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.ni_io", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "ni_io", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Mon.ni_io= BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.mp", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "mp", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Mon.mp= BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.fp", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "fp", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Mon.fp= BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.bell", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "bell", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.Param_Bits.bell= BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.bell_volume", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "bell_volume", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.bell_volume = BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.icon_on", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "icon_on", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.Param_Bits.icon_on= BannerStrToNumeric (resourcevalue.addr);
 
*29D
     if (BannerGetResource (db, "DECW$BANNER.base_priority", 0, &resourcetype,
 	    &resourcevalue))
*29E
*29I
     if (BannerGetResource (db, "base_priority", 0, &resourcetype,
 	    &resourcevalue, flag))
*29E
 	Bnr.base_priority= BannerStrToNumeric (resourcevalue.addr);
 
 }
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **	BannerSetResource (string, database)
 **
 **      [@tbs@]
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
 static void	
 BannerSetResource(str, db, db1)
 int db, db1;
 char *str;
 {
     XrmPutLineResource (db, str);
     XrmPutLineResource (db1, str);
 }
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **  
 **	BannerWriteDatabase ();
 **
 **      This routine writes out the current setup options to the users 
 **	private database, DECW$BANNER.DAT.
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
 int
 BannerWriteDatabase()
 {
*29D
     char string[100];
*29E
*29I
     char string[250];
*29E
     int db, db1;
*24I
     char    nodedb[100];
*24E
 
 
 
     db1 = XtDatabase (XtDisplay(Bnr.dp_id));
 
*24D
     db = XrmGetFileDatabase ("DECW$USER_DEFAULTS:DECW$BANNER.DAT");
*24E
*24I
     sprintf (nodedb, "DECW$USER_DEFAULTS:DECW$BANNER_%s.DAT",
 	BannerVmsNodeName);
 
     db = XrmGetFileDatabase (nodedb);
*24E
 
 /*
  * Now start putting our resources, and see what we can get from the 
  * files.
  */
 
*29D
     sprintf (string, "DECW$BANNER.font_template: %s\n", Bnr.fnt_template);
*29E
*29I
*41I
     if (BannerVmsDisplayName != NULL)
 	sprintf (string, "DECW$BANNER_%s.node_display_name: %s\n", BannerVmsNodeName, 
 	    BannerVmsDisplayName);
     else
 	sprintf (string, "DECW$BANNER_%s.node_display_name: \n", BannerVmsNodeName);
     BannerSetResource(string, &db, &db1);
 
*41E
     sprintf (string, "DECW$BANNER_%s.font_template: %s\n", BannerVmsNodeName, Bnr.fnt_template);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.font_family_name: %s\n", banner_font_name);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.font_family_name: %s\n", BannerVmsNodeName, banner_font_name);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.font_family_size: %s\n", banner_font_size);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.font_family_size: %s\n", BannerVmsNodeName, banner_font_size);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.title: %s\n", Bnr.title);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.title: %s\n", BannerVmsNodeName, Bnr.title);
*29E
     BannerSetResource(string, &db, &db1);
 
*8D
 /*    if (Bnr.foreground_colour != 0)
*8E
*8I
     if (Bnr.hwmrk_colour != 0)
 	{    
*29D
 	sprintf (string, "DECW$BANNER.highlight_colour: %s\n", 
*29E
*29I
 	sprintf (string, "DECW$BANNER_%s.highlight_colour: %s\n", BannerVmsNodeName, 
*29E
 	    Bnr.hwmrk_colour);
 	BannerSetResource(string, &db, &db1);
 	}
 
*33I
     if (Son.Volume != 0)
 	{    
 	sprintf (string, "DECW$BANNER_%s.volume: %d\n", BannerVmsNodeName, 
 	    Son.Volume);
 	BannerSetResource(string, &db, &db1);
 	}
 
     if (Son.Time != 0)
 	{    
 	sprintf (string, "DECW$BANNER_%s.time: %d\n", BannerVmsNodeName, 
 	    Son.Time);
 	BannerSetResource(string, &db, &db1);
 	}
 
     if (Son.Mwait.DisplayName != NULL)
*34D
 	{    
*34E
 	sprintf (string, "DECW$BANNER_%s.mwait_sound: %s\n", BannerVmsNodeName, 
 	    Son.Mwait.DisplayName);
*34D
 	BannerSetResource(string, &db, &db1);
 	}
*34E
*34I
     else
 	sprintf (string, "DECW$BANNER_%s.mwait_sound: \n", BannerVmsNodeName);
     BannerSetResource(string, &db, &db1);
*34E
 
     if (Son.NewProcesses.DisplayName != NULL)
*34D
 	{    
*34E
 	sprintf (string, "DECW$BANNER_%s.newprocesses_sound: %s\n", BannerVmsNodeName, 
 	    Son.NewProcesses.DisplayName);
*34D
 	BannerSetResource(string, &db, &db1);
 	}
*34E
*34I
     else
 	sprintf (string, "DECW$BANNER_%s.newprocesses_sound: \n", BannerVmsNodeName);
     BannerSetResource(string, &db, &db1);
*34E
 
     if (Son.LessProcesses.DisplayName != NULL)
*34D
 	{    
*34E
 	sprintf (string, "DECW$BANNER_%s.lessprocesses_sound: %s\n", BannerVmsNodeName, 
 	    Son.LessProcesses.DisplayName);
*34D
 	BannerSetResource(string, &db, &db1);
 	}
*34E
*34I
     else
 	sprintf (string, "DECW$BANNER_%s.lessprocesses_sound: \n", BannerVmsNodeName);
     BannerSetResource(string, &db, &db1);
*34E
 
     if (Son.Disk1.DisplayName != NULL)
*34D
 	{    
*34E
 	sprintf (string, "DECW$BANNER_%s.disk1_sound: %s\n", BannerVmsNodeName, 
 	    Son.Disk1.DisplayName);
*34D
 	BannerSetResource(string, &db, &db1);
 	}
*34E
*34I
     else
 	sprintf (string, "DECW$BANNER_%s.disk1_sound: \n", BannerVmsNodeName);
     BannerSetResource(string, &db, &db1);
*34E
 
     if (Son.Disk2.DisplayName != NULL)
*34D
 	{    
*34E
 	sprintf (string, "DECW$BANNER_%s.disk2_sound: %s\n", BannerVmsNodeName, 
 	    Son.Disk2.DisplayName);
*34D
 	BannerSetResource(string, &db, &db1);
 	}
*34E
*34I
     else
 	sprintf (string, "DECW$BANNER_%s.disk2_sound: \n", BannerVmsNodeName);
     BannerSetResource(string, &db, &db1);
*34E
 
     if (Son.Disk3.DisplayName != NULL)
*34D
 	{    
*34E
 	sprintf (string, "DECW$BANNER_%s.disk3_sound: %s\n", BannerVmsNodeName, 
 	    Son.Disk3.DisplayName);
*34D
 	BannerSetResource(string, &db, &db1);
 	}
*34E
*34I
     else
 	sprintf (string, "DECW$BANNER_%s.disk3_sound: \n", BannerVmsNodeName);
     BannerSetResource(string, &db, &db1);
*34E
 
     if (Son.Memory.DisplayName != NULL)
*34D
 	{    
*34E
 	sprintf (string, "DECW$BANNER_%s.memory_sound: %s\n", BannerVmsNodeName, 
 	    Son.Memory.DisplayName);
*34D
 	BannerSetResource(string, &db, &db1);
 	}
*34E
*34I
     else
 	sprintf (string, "DECW$BANNER_%s.memory_sound: \n", BannerVmsNodeName);
     BannerSetResource(string, &db, &db1);
*34E
 
     if (Son.PageFile.DisplayName != NULL)
*34D
 	{    
*34E
 	sprintf (string, "DECW$BANNER_%s.pagefile_sound: %s\n", BannerVmsNodeName, 
 	    Son.PageFile.DisplayName);
*34D
 	BannerSetResource(string, &db, &db1);
 	}
*34E
*34I
     else
 	sprintf (string, "DECW$BANNER_%s.pagefile_sound: \n", BannerVmsNodeName);
     BannerSetResource(string, &db, &db1);
*34E
 
     if (Son.PendingIO.DisplayName != NULL)
*34D
 	{    
*34E
 	sprintf (string, "DECW$BANNER_%s.pendingio_sound: %s\n", BannerVmsNodeName, 
 	    Son.PendingIO.DisplayName);
*34D
 	BannerSetResource(string, &db, &db1);
 	}
*34E
*34I
     else
 	sprintf (string, "DECW$BANNER_%s.pendingio_sound: \n", BannerVmsNodeName);
     BannerSetResource(string, &db, &db1);
*34E
 
 
 
*33E
 /*    
     if (Bnr.foreground_colour != 0)
*8E
 	{    
*29D
 	sprintf (string, "DECW$BANNER*foreground: %s\n", 
*29E
*29I
 	sprintf (string, "DECW$BANNER*foreground: %s\n", BannerVmsNodeName, 
*29E
 	    Bnr.foreground_colour);
 	BannerSetResource(string, &db, &db1);
 	}
 
     if (Bnr.background_colour != 0)
 	{    
*29D
 	sprintf (string, "DECW$BANNER*background: %s\n", 
*29E
*29I
 	sprintf (string, "DECW$BANNER*background: %s\n", BannerVmsNodeName, 
*29E
 	    Bnr.background_colour);
 	BannerSetResource(string, &db, &db1);
 	}
 */
 
*19I
*29D
     sprintf (string, "DECW$BANNER.qp_name: %s\n", Bnr.qp_name);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.qp_name: %s\n", BannerVmsNodeName, Bnr.qp_name);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.qb_name: %s\n", Bnr.qb_name);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.qb_name: %s\n", BannerVmsNodeName, Bnr.qb_name);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.queue_user: %s\n", Bnr.queue_user);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.queue_user: %s\n", BannerVmsNodeName, Bnr.queue_user);
*29E
     BannerSetResource(string, &db, &db1);
 
*19E
*29D
     sprintf (string, "DECW$BANNER.disk_0_name: %s\n", Bnr.disk_0_name);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.disk_0_name: %s\n", BannerVmsNodeName, Bnr.disk_0_name);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.disk_1_name: %s\n", Bnr.disk_1_name);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.disk_1_name: %s\n", BannerVmsNodeName, Bnr.disk_1_name);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.disk_2_name: %s\n", Bnr.disk_2_name);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.disk_2_name: %s\n", BannerVmsNodeName, Bnr.disk_2_name);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.disk_0: %s\n", Bnr.disk_0);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.disk_0: %s\n", BannerVmsNodeName, Bnr.disk_0);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.disk_1: %s\n", Bnr.disk_1);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.disk_1: %s\n", BannerVmsNodeName, Bnr.disk_1);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.disk_2: %s\n", Bnr.disk_2);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.disk_2: %s\n", BannerVmsNodeName, Bnr.disk_2);
*29E
     BannerSetResource(string, &db, &db1);
 
 
*29D
     sprintf (string, "DECW$BANNER.cpu_update: %d\n", Bnr.cpu_update);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.cpu_update: %d\n", BannerVmsNodeName, Bnr.cpu_update);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.process_update: %d\n", Bnr.process_update);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.process_update: %d\n", BannerVmsNodeName, Bnr.process_update);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.monitor_update: %d\n", Bnr.monitor_update);
*29E
*29I
*35D
*34A1D
     sprintf (string, "DECW$BANNER_%s.monitor_update: %d\n", BannerVmsNodeName, Bnr.monitor_update);
*34A1E
*34A1I
     sprintf (string, "DECW$BANNER_%s.link_update: %d\n", BannerVmsNodeName, 
 	Bnr.link_update);
*34A1E
*35E
*35I
*36D
     sprintf (string, "DECW$BANNER_%s.link_update: %d\n", BannerVmsNodeName, 
 	Bnr.link_update);
*36E
*36I
*37I
*38D
 **************** Conflict 17    ************************************************
*38E
*37E
     sprintf (string, "DECW$BANNER_%s.monitor_update: %d\n", BannerVmsNodeName, Bnr.monitor_update);
*36E
*35E
*29E
*37I
*38D
 ********************************************************************************
*38E
*38I
     BannerSetResource(string, &db, &db1);
 
*38E
     sprintf (string, "DECW$BANNER_%s.link_update: %d\n", BannerVmsNodeName, 
 	Bnr.link_update);
*38D
 ************** End of Conflict 17    *******************************************
*38E
*37E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.pe_update: %d\n", Bnr.pe_update);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.pe_update: %d\n", BannerVmsNodeName, Bnr.pe_update);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.lck_update: %d\n", Bnr.lck_update);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.lck_update: %d\n", BannerVmsNodeName, Bnr.lck_update);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.cwps_update: %d\n", Bnr.cwps_update);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.cwps_update: %d\n", BannerVmsNodeName, Bnr.cwps_update);
*29E
     BannerSetResource(string, &db, &db1);
 
*7I
*8I
*29D
     sprintf (string, "DECW$BANNER.sys_ioratemax: %d\n", Bnr.sys_ioratemax);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.sys_ioratemax: %d\n", BannerVmsNodeName, Bnr.sys_ioratemax);
*29E
     BannerSetResource(string, &db, &db1);
 
*12I
*29D
     sprintf (string, "DECW$BANNER.sysrespgfile: %d\n", Bnr.sysrespgfile);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.sysrespgfile: %d\n", BannerVmsNodeName, Bnr.sysrespgfile);
*29E
     BannerSetResource(string, &db, &db1);
 
*12E
*29D
     sprintf (string, "DECW$BANNER.sys_faultratemax: %d\n", Bnr.sys_faultratemax);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.sys_faultratemax: %d\n", BannerVmsNodeName, Bnr.sys_faultratemax);
*29E
     BannerSetResource(string, &db, &db1);
 
*10D
     sprintf (string, "DECW$BANNER.sys_higlight: %d\n", Bnr.sys_highlight);
*10E
*10I
*29D
     sprintf (string, "DECW$BANNER.sys_highlight: %d\n", Bnr.sys_highlight);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.sys_highlight: %d\n", BannerVmsNodeName, Bnr.sys_highlight);
*29E
*10E
     BannerSetResource(string, &db, &db1);
 
*8E
*29D
     sprintf (string, "DECW$BANNER.sys_update: %d\n", Bnr.sys_update);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.sys_update: %d\n", BannerVmsNodeName, Bnr.sys_update);
*29E
     BannerSetResource(string, &db, &db1);
 
*7E
*8I
*19I
*29D
     sprintf (string, "DECW$BANNER.tp_update: %d\n", Bnr.tp_update);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.tp_update: %d\n", BannerVmsNodeName, Bnr.tp_update);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.qp_update: %d\n", Bnr.qp_update);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.qp_update: %d\n", BannerVmsNodeName, Bnr.qp_update);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.qb_update: %d\n", Bnr.qb_update);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.qb_update: %d\n", BannerVmsNodeName, Bnr.qb_update);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.clu_update: %d\n", Bnr.clu_update);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.clu_update: %d\n", BannerVmsNodeName, Bnr.clu_update);
*29E
     BannerSetResource(string, &db, &db1);
 
*19E
*29D
     sprintf (string, "DECW$BANNER.hwmrk_update: %d\n", Bnr.hwmrk_update);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.hwmrk_update: %d\n", BannerVmsNodeName, Bnr.hwmrk_update);
*29E
     BannerSetResource(string, &db, &db1);
 
*8E
*29D
     sprintf (string, "DECW$BANNER.cpu_count: %d\n", Bnr.cpu_count);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.cpu_count: %d\n", BannerVmsNodeName, Bnr.cpu_count);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.clock_x: %d\n", Bnr.clock_x);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.clock_x: %d\n", BannerVmsNodeName, Bnr.clock_x);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.clock_y: %d\n", Bnr.clock_y);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.clock_y: %d\n", BannerVmsNodeName, Bnr.clock_y);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.clock_width: %d\n", Bnr.clock_width);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.clock_width: %d\n", BannerVmsNodeName, Bnr.clock_width);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.clock_height: %d\n", Bnr.clock_height);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.clock_height: %d\n", BannerVmsNodeName, Bnr.clock_height);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.message_x: %d\n", Bnr.message_x);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.message_x: %d\n", BannerVmsNodeName, Bnr.message_x);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.message_y: %d\n", Bnr.message_y);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.message_y: %d\n", BannerVmsNodeName, Bnr.message_y);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.message_width: %d\n", Bnr.message_width);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.message_width: %d\n", BannerVmsNodeName, Bnr.message_width);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.message_height: %d\n", Bnr.message_height);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.message_height: %d\n", BannerVmsNodeName, Bnr.message_height);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.message_init_state: %d\n", Bnr.message_init_state);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.message_init_state: %d\n", BannerVmsNodeName, Bnr.message_init_state);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.cube_x: %d\n", Bnr.cube_x);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.cube_x: %d\n", BannerVmsNodeName, Bnr.cube_x);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.cube_y: %d\n", Bnr.cube_y);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.cube_y: %d\n", BannerVmsNodeName, Bnr.cube_y);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.cube_width: %d\n", Bnr.cube_width);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.cube_width: %d\n", BannerVmsNodeName, Bnr.cube_width);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.cube_height: %d\n", Bnr.cube_height);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.cube_height: %d\n", BannerVmsNodeName, Bnr.cube_height);
*29E
     BannerSetResource(string, &db, &db1);
 
*15I
*29D
     sprintf (string, "DECW$BANNER.hanoi_x: %d\n", Bnr.hanoi_x);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.hanoi_x: %d\n", BannerVmsNodeName, Bnr.hanoi_x);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.hanoi_y: %d\n", Bnr.hanoi_y);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.hanoi_y: %d\n", BannerVmsNodeName, Bnr.hanoi_y);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.hanoi_width: %d\n", Bnr.hanoi_width);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.hanoi_width: %d\n", BannerVmsNodeName, Bnr.hanoi_width);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.hanoi_height: %d\n", Bnr.hanoi_height);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.hanoi_height: %d\n", BannerVmsNodeName, Bnr.hanoi_height);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.globe_x: %d\n", Bnr.globe_x);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.globe_x: %d\n", BannerVmsNodeName, Bnr.globe_x);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.globe_y: %d\n", Bnr.globe_y);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.globe_y: %d\n", BannerVmsNodeName, Bnr.globe_y);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.globe_width: %d\n", Bnr.globe_width);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.globe_width: %d\n", BannerVmsNodeName, Bnr.globe_width);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.globe_height: %d\n", Bnr.globe_height);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.globe_height: %d\n", BannerVmsNodeName, Bnr.globe_height);
*29E
     BannerSetResource(string, &db, &db1);
 
*15E
*29D
     sprintf (string, "DECW$BANNER.cpu_x: %d\n", Bnr.cpu_x);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.cpu_x: %d\n", BannerVmsNodeName, Bnr.cpu_x);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.cpu_y: %d\n", Bnr.cpu_y);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.cpu_y: %d\n", BannerVmsNodeName, Bnr.cpu_y);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.cpu_width: %d\n", Bnr.cpu_width);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.cpu_width: %d\n", BannerVmsNodeName, Bnr.cpu_width);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.cpu_height: %d\n", Bnr.cpu_height);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.cpu_height: %d\n", BannerVmsNodeName, Bnr.cpu_height);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.mon_x: %d\n", Bnr.mon_x);
*29E
*29I
*35D
*34A1D
     sprintf (string, "DECW$BANNER_%s.mon_x: %d\n", BannerVmsNodeName, Bnr.mon_x);
*34A1E
*34A1I
     sprintf (string, "DECW$BANNER_%s.link_x: %d\n", BannerVmsNodeName, Bnr.link_x);
*34A1E
*35E
*35I
*36D
     sprintf (string, "DECW$BANNER_%s.link_x: %d\n", BannerVmsNodeName, Bnr.link_x);
*36E
*36I
*37I
*38D
 **************** Conflict 18    ************************************************
*38E
*37E
     sprintf (string, "DECW$BANNER_%s.mon_x: %d\n", BannerVmsNodeName, Bnr.mon_x);
*36E
*35E
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.mon_y: %d\n", Bnr.mon_y);
*29E
*29I
*35D
*34A1D
     sprintf (string, "DECW$BANNER_%s.mon_y: %d\n", BannerVmsNodeName, Bnr.mon_y);
*34A1E
*34A1I
     sprintf (string, "DECW$BANNER_%s.link_y: %d\n", BannerVmsNodeName, Bnr.link_y);
*34A1E
*35E
*35I
*36D
     sprintf (string, "DECW$BANNER_%s.link_y: %d\n", BannerVmsNodeName, Bnr.link_y);
*36E
*36I
     sprintf (string, "DECW$BANNER_%s.mon_y: %d\n", BannerVmsNodeName, Bnr.mon_y);
*36E
*35E
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.mon_width: %d\n", Bnr.mon_width);
*29E
*29I
*35D
*34A1D
     sprintf (string, "DECW$BANNER_%s.mon_width: %d\n", BannerVmsNodeName, Bnr.mon_width);
*34A1E
*34A1I
     sprintf (string, "DECW$BANNER_%s.link_width: %d\n", BannerVmsNodeName, Bnr.link_width);
*34A1E
*35E
*35I
*36D
     sprintf (string, "DECW$BANNER_%s.link_width: %d\n", BannerVmsNodeName, Bnr.link_width);
*36E
*36I
     sprintf (string, "DECW$BANNER_%s.mon_width: %d\n", BannerVmsNodeName, Bnr.mon_width);
*36E
*35E
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.mon_height: %d\n", Bnr.mon_height);
*29E
*29I
*35D
*34A1D
     sprintf (string, "DECW$BANNER_%s.mon_height: %d\n", BannerVmsNodeName, Bnr.mon_height);
*34A1E
*34A1I
     sprintf (string, "DECW$BANNER_%s.link_height: %d\n", BannerVmsNodeName, Bnr.link_height);
*34A1E
*35E
*35I
*36D
     sprintf (string, "DECW$BANNER_%s.link_height: %d\n", BannerVmsNodeName, Bnr.link_height);
*36E
*36I
     sprintf (string, "DECW$BANNER_%s.mon_height: %d\n", BannerVmsNodeName, Bnr.mon_height);
*36E
*35E
*29E
*37I
*38D
 ********************************************************************************
*38E
*38I
     BannerSetResource(string, &db, &db1);
 
*38E
     sprintf (string, "DECW$BANNER_%s.link_x: %d\n", BannerVmsNodeName, Bnr.link_x);
     BannerSetResource(string, &db, &db1);
     sprintf (string, "DECW$BANNER_%s.link_y: %d\n", BannerVmsNodeName, Bnr.link_y);
     BannerSetResource(string, &db, &db1);
     sprintf (string, "DECW$BANNER_%s.link_width: %d\n", BannerVmsNodeName, Bnr.link_width);
     BannerSetResource(string, &db, &db1);
     sprintf (string, "DECW$BANNER_%s.link_height: %d\n", BannerVmsNodeName, Bnr.link_height);
*38D
 ************** End of Conflict 18    *******************************************
*38E
*37E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.pe_x: %d\n", Bnr.pe_x);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.pe_x: %d\n", BannerVmsNodeName, Bnr.pe_x);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.pe_y: %d\n", Bnr.pe_y);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.pe_y: %d\n", BannerVmsNodeName, Bnr.pe_y);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.pe_width: %d\n", Bnr.pe_width);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.pe_width: %d\n", BannerVmsNodeName, Bnr.pe_width);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.pe_height: %d\n", Bnr.pe_height);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.pe_height: %d\n", BannerVmsNodeName, Bnr.pe_height);
*29E
     BannerSetResource(string, &db, &db1);
 
 
*29D
     sprintf (string, "DECW$BANNER.lck_x: %d\n", Bnr.lck_x);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.lck_x: %d\n", BannerVmsNodeName, Bnr.lck_x);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.lck_y: %d\n", Bnr.lck_y);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.lck_y: %d\n", BannerVmsNodeName, Bnr.lck_y);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.lck_width: %d\n", Bnr.lck_width);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.lck_width: %d\n", BannerVmsNodeName, Bnr.lck_width);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.lck_height: %d\n", Bnr.lck_height);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.lck_height: %d\n", BannerVmsNodeName, Bnr.lck_height);
*29E
     BannerSetResource(string, &db, &db1);
 
 
*29D
     sprintf (string, "DECW$BANNER.cwps_x: %d\n", Bnr.cwps_x);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.cwps_x: %d\n", BannerVmsNodeName, Bnr.cwps_x);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.cwps_y: %d\n", Bnr.cwps_y);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.cwps_y: %d\n", BannerVmsNodeName, Bnr.cwps_y);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.cwps_width: %d\n", Bnr.cwps_width);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.cwps_width: %d\n", BannerVmsNodeName, Bnr.cwps_width);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.cwps_height: %d\n", Bnr.cwps_height);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.cwps_height: %d\n", BannerVmsNodeName, Bnr.cwps_height);
*29E
     BannerSetResource(string, &db, &db1);
 
*7I
*29D
     sprintf (string, "DECW$BANNER.sys_x: %d\n", Bnr.sys_x);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.sys_x: %d\n", BannerVmsNodeName, Bnr.sys_x);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.sys_y: %d\n", Bnr.sys_y);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.sys_y: %d\n", BannerVmsNodeName, Bnr.sys_y);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.sys_width: %d\n", Bnr.sys_width);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.sys_width: %d\n", BannerVmsNodeName, Bnr.sys_width);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.sys_height: %d\n", Bnr.sys_height);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.sys_height: %d\n", BannerVmsNodeName, Bnr.sys_height);
*29E
     BannerSetResource(string, &db, &db1);
 
*7E
*19I
*29D
     sprintf (string, "DECW$BANNER.tp_x: %d\n", Bnr.tp_x);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.tp_x: %d\n", BannerVmsNodeName, Bnr.tp_x);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.tp_y: %d\n", Bnr.tp_y);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.tp_y: %d\n", BannerVmsNodeName, Bnr.tp_y);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.tp_width: %d\n", Bnr.tp_width);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.tp_width: %d\n", BannerVmsNodeName, Bnr.tp_width);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.tp_height: %d\n", Bnr.tp_height);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.tp_height: %d\n", BannerVmsNodeName, Bnr.tp_height);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.qp_x: %d\n", Bnr.qp_x);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.qp_x: %d\n", BannerVmsNodeName, Bnr.qp_x);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.qp_y: %d\n", Bnr.qp_y);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.qp_y: %d\n", BannerVmsNodeName, Bnr.qp_y);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.qp_width: %d\n", Bnr.qp_width);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.qp_width: %d\n", BannerVmsNodeName, Bnr.qp_width);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.qp_height: %d\n", Bnr.qp_height);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.qp_height: %d\n", BannerVmsNodeName, Bnr.qp_height);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.qb_x: %d\n", Bnr.qb_x);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.qb_x: %d\n", BannerVmsNodeName, Bnr.qb_x);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.qb_y: %d\n", Bnr.qb_y);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.qb_y: %d\n", BannerVmsNodeName, Bnr.qb_y);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.qb_width: %d\n", Bnr.qb_width);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.qb_width: %d\n", BannerVmsNodeName, Bnr.qb_width);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.qb_height: %d\n", Bnr.qb_height);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.qb_height: %d\n", BannerVmsNodeName, Bnr.qb_height);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.clu_x: %d\n", Bnr.clu_x);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.clu_x: %d\n", BannerVmsNodeName, Bnr.clu_x);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.clu_y: %d\n", Bnr.clu_y);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.clu_y: %d\n", BannerVmsNodeName, Bnr.clu_y);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.clu_width: %d\n", Bnr.clu_width);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.clu_width: %d\n", BannerVmsNodeName, Bnr.clu_width);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.clu_height: %d\n", Bnr.clu_height);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.clu_height: %d\n", BannerVmsNodeName, Bnr.clu_height);
*29E
     BannerSetResource(string, &db, &db1);
 
*19E
*29D
     sprintf (string, "DECW$BANNER.process_x: %d\n", Bnr.process_x);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.process_x: %d\n", BannerVmsNodeName, Bnr.process_x);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.process_y: %d\n", Bnr.process_y);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.process_y: %d\n", BannerVmsNodeName, Bnr.process_y);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.process_width: %d\n", Bnr.process_width);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.process_width: %d\n", BannerVmsNodeName, Bnr.process_width);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.process_height: %d\n", Bnr.process_height);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.process_height: %d\n", BannerVmsNodeName, Bnr.process_height);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.border: %d\n", Bnr.Param_Bits.border);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.border: %d\n", BannerVmsNodeName, Bnr.Param_Bits.border);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.window_border: %d\n", Bnr.Param_Bits.window_border);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.window_border: %d\n", BannerVmsNodeName, Bnr.Param_Bits.window_border);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.world: %d\n", Bnr.Param_Bits.world);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.world: %d\n", BannerVmsNodeName, Bnr.Param_Bits.world);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.process_name: %d\n", Bnr.Param_Bits.process_name);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.process_name: %d\n", BannerVmsNodeName, Bnr.Param_Bits.process_name);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.process_user: %d\n", Bnr.Param_Bits.process_user);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.process_user: %d\n", BannerVmsNodeName, Bnr.Param_Bits.process_user);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.process_pid: %d\n", Bnr.Param_Bits.process_pid);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.process_pid: %d\n", BannerVmsNodeName, Bnr.Param_Bits.process_pid);
*29E
     BannerSetResource(string, &db, &db1);
 
*3I
*29D
     sprintf (string, "DECW$BANNER.freeiocnt: %d\n", Bnr.Param_Bits.freeiocnt);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.freeiocnt: %d\n", BannerVmsNodeName, Bnr.Param_Bits.freeiocnt);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.freeblkcnt: %d\n", Bnr.Param_Bits.freeblkcnt);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.freeblkcnt: %d\n", BannerVmsNodeName, Bnr.Param_Bits.freeblkcnt);
*29E
     BannerSetResource(string, &db, &db1);
 
*3E
*29D
     sprintf (string, "DECW$BANNER.cube: %d\n", Bnr.Param_Bits.cube);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.cube: %d\n", BannerVmsNodeName, Bnr.Param_Bits.cube);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.bars: %d\n", Bnr.Param_Bits.bars);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.bars: %d\n", BannerVmsNodeName, Bnr.Param_Bits.bars);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.process_states: %d\n", Bnr.Param_Bits.process_states);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.process_states: %d\n", BannerVmsNodeName, Bnr.Param_Bits.process_states);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.iocnt: %d\n", Bnr.Param_Bits.iocnt);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.iocnt: %d\n", BannerVmsNodeName, Bnr.Param_Bits.iocnt);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.monitor : %d\n", Bnr.Param_Bits.monitor);
*29E
*29I
*35D
*34A1D
     sprintf (string, "DECW$BANNER_%s.monitor : %d\n", BannerVmsNodeName, Bnr.Param_Bits.monitor);
*34A1E
*34A1I
     sprintf (string, "DECW$BANNER_%s.link_window : %d\n", BannerVmsNodeName, 
 	Bnr.Param_Bits.link_window);
*34A1E
*35E
*35I
*36D
     sprintf (string, "DECW$BANNER_%s.link_window : %d\n", BannerVmsNodeName, 
 	Bnr.Param_Bits.link_window);
*36E
*36I
*37I
*38D
 **************** Conflict 19    ************************************************
*38E
*37E
     sprintf (string, "DECW$BANNER_%s.monitor : %d\n", BannerVmsNodeName, Bnr.Param_Bits.monitor);
*36E
*35E
*29E
*37I
*38D
 ********************************************************************************
*38E
*38I
     BannerSetResource(string, &db, &db1);
 
*38E
     sprintf (string, "DECW$BANNER_%s.link_window : %d\n", BannerVmsNodeName, 
 	Bnr.Param_Bits.link_window);
*38D
 ************** End of Conflict 19    *******************************************
*38E
*37E
     BannerSetResource(string, &db, &db1);
 
*9I
 
*9E
*29D
     sprintf (string, "DECW$BANNER.process_window : %d\n", Bnr.Param_Bits.process_window);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.process_window : %d\n", BannerVmsNodeName, Bnr.Param_Bits.process_window);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.pe_window: %d\n", Bnr.Param_Bits.pe_window);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.pe_window: %d\n", BannerVmsNodeName, Bnr.Param_Bits.pe_window);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.lck_window: %d\n", Bnr.Param_Bits.lck_window);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.lck_window: %d\n", BannerVmsNodeName, Bnr.Param_Bits.lck_window);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.cwps_window: %d\n", Bnr.Param_Bits.cwps_window);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.cwps_window: %d\n", BannerVmsNodeName, Bnr.Param_Bits.cwps_window);
*29E
     BannerSetResource(string, &db, &db1);
 
*7I
*29D
     sprintf (string, "DECW$BANNER.sys_window: %d\n", Bnr.Param_Bits.sys_window);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.sys_window: %d\n", BannerVmsNodeName, Bnr.Param_Bits.sys_window);
*29E
     BannerSetResource(string, &db, &db1);
 
*7E
*19I
*29D
     sprintf (string, "DECW$BANNER.tp_window: %d\n", Bnr.Param_Bits.tp_window);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.tp_window: %d\n", BannerVmsNodeName, Bnr.Param_Bits.tp_window);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.tp_max_rate: %d\n", Bnr.tp_max_rate);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.tp_max_rate: %d\n", BannerVmsNodeName, Bnr.tp_max_rate);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.tp_rate1_start: %d\n", Bnr.tp_rate1_start);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.tp_rate1_start: %d\n", BannerVmsNodeName, Bnr.tp_rate1_start);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.tp_rate2_start: %d\n", Bnr.tp_rate2_start);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.tp_rate2_start: %d\n", BannerVmsNodeName, Bnr.tp_rate2_start);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.tp_rate3_start: %d\n", Bnr.tp_rate3_start);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.tp_rate3_start: %d\n", BannerVmsNodeName, Bnr.tp_rate3_start);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.tp_rate4_start: %d\n", Bnr.tp_rate4_start);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.tp_rate4_start: %d\n", BannerVmsNodeName, Bnr.tp_rate4_start);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.tp_rate1_end: %d\n", Bnr.tp_rate1_end);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.tp_rate1_end: %d\n", BannerVmsNodeName, Bnr.tp_rate1_end);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.tp_rate2_end: %d\n", Bnr.tp_rate2_end);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.tp_rate2_end: %d\n", BannerVmsNodeName, Bnr.tp_rate2_end);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.tp_rate3_end: %d\n", Bnr.tp_rate3_end);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.tp_rate3_end: %d\n", BannerVmsNodeName, Bnr.tp_rate3_end);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.total_com_rate: %d\n", Bnr.Param_Bits.total_com_rate);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.total_com_rate: %d\n", BannerVmsNodeName, Bnr.Param_Bits.total_com_rate);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.onef_comp_rate: %d\n", Bnr.Param_Bits.onef_comp_rate);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.onef_comp_rate: %d\n", BannerVmsNodeName, Bnr.Param_Bits.onef_comp_rate);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.start_rate: %d\n", Bnr.Param_Bits.start_rate);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.start_rate: %d\n", BannerVmsNodeName, Bnr.Param_Bits.start_rate);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.prep_rate: %d\n", Bnr.Param_Bits.prep_rate);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.prep_rate: %d\n", BannerVmsNodeName, Bnr.Param_Bits.prep_rate);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.abort_rate: %d\n", Bnr.Param_Bits.abort_rate);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.abort_rate: %d\n", BannerVmsNodeName, Bnr.Param_Bits.abort_rate);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.end_rate: %d\n", Bnr.Param_Bits.end_rate);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.end_rate: %d\n", BannerVmsNodeName, Bnr.Param_Bits.end_rate);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.rem_start_rate: %d\n", Bnr.Param_Bits.rem_start_rate);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.rem_start_rate: %d\n", BannerVmsNodeName, Bnr.Param_Bits.rem_start_rate);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.rem_end_rate: %d\n", Bnr.Param_Bits.rem_end_rate);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.rem_end_rate: %d\n", BannerVmsNodeName, Bnr.Param_Bits.rem_end_rate);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.comp_rate1: %d\n", Bnr.Param_Bits.comp_rate1);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.comp_rate1: %d\n", BannerVmsNodeName, Bnr.Param_Bits.comp_rate1);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.comp_rate2: %d\n", Bnr.Param_Bits.comp_rate2);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.comp_rate2: %d\n", BannerVmsNodeName, Bnr.Param_Bits.comp_rate2);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.comp_rate3: %d\n", Bnr.Param_Bits.comp_rate3);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.comp_rate3: %d\n", BannerVmsNodeName, Bnr.Param_Bits.comp_rate3);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.comp_rate4: %d\n", Bnr.Param_Bits.comp_rate4);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.comp_rate4: %d\n", BannerVmsNodeName, Bnr.Param_Bits.comp_rate4);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.qp_window: %d\n", Bnr.Param_Bits.qp_window);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.qp_window: %d\n", BannerVmsNodeName, Bnr.Param_Bits.qp_window);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.qb_window: %d\n", Bnr.Param_Bits.qb_window);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.qb_window: %d\n", BannerVmsNodeName, Bnr.Param_Bits.qb_window);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.clu_window: %d\n", Bnr.Param_Bits.clu_window);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.clu_window: %d\n", BannerVmsNodeName, Bnr.Param_Bits.clu_window);
*29E
     BannerSetResource(string, &db, &db1);
 
*19E
*29D
     sprintf (string, "DECW$BANNER.cpu: %d\n", Bnr.Param_Bits.cpu_window);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.cpu: %d\n", BannerVmsNodeName, Bnr.Param_Bits.cpu_window);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.message_window: %d\n", Bnr.Param_Bits.message_window);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.message_window: %d\n", BannerVmsNodeName, Bnr.Param_Bits.message_window);
*29E
     BannerSetResource(string, &db, &db1);
*29D
     sprintf (string, "DECW$BANNER.message_size: %d\n", BannerMessageGetSize());
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.message_size: %d\n", BannerVmsNodeName, BannerMessageGetSize());
*29E
     BannerSetResource(string, &db, &db1);
     {
     extern int hide_timer_on;
     if (hide_timer_on)
 	{
*29D
 	sprintf (string, "DECW$BANNER.message_hide_timer: %s\n", BannerMessageGetHide());
*29E
*29I
 	sprintf (string, "DECW$BANNER_%s.message_hide_timer: %s\n", BannerVmsNodeName, BannerMessageGetHide());
*29E
 	BannerSetResource(string, &db, &db1);
 	}
     }
 
*29D
     sprintf (string, "DECW$BANNER.modes: %d\n", Bnr.Param_Bits.modes_window);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.modes: %d\n", BannerVmsNodeName, Bnr.Param_Bits.modes_window);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.multi_hist: %d\n", Bnr.Param_Bits.cpu_multi_hist);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.multi_hist: %d\n", BannerVmsNodeName, Bnr.Param_Bits.cpu_multi_hist);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.hanoi: %d\n", Bnr.Param_Bits.hanoi);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.hanoi: %d\n", BannerVmsNodeName, Bnr.Param_Bits.hanoi);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.seconds: %d\n", Bnr.Param_Bits.seconds);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.seconds: %d\n", BannerVmsNodeName, Bnr.Param_Bits.seconds);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.digital_clock: %d\n", Bnr.Param_Bits.digital_clock);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.digital_clock: %d\n", BannerVmsNodeName, Bnr.Param_Bits.digital_clock);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.analog_clock: %d\n", Bnr.Param_Bits.analog_clock);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.analog_clock: %d\n", BannerVmsNodeName, Bnr.Param_Bits.analog_clock);
*29E
     BannerSetResource(string, &db, &db1);
 
*19I
*29D
     sprintf (string, "DECW$BANNER.all_users: %d\n", Bnr.Param_Bits.all_users);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.all_users: %d\n", BannerVmsNodeName, Bnr.Param_Bits.all_users);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.jobs_on_hold: %d\n", Bnr.Param_Bits.jobs_on_hold);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.jobs_on_hold: %d\n", BannerVmsNodeName, Bnr.Param_Bits.jobs_on_hold);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.stopped_queues: %d\n", Bnr.Param_Bits.stopped_queues);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.stopped_queues: %d\n", BannerVmsNodeName, Bnr.Param_Bits.stopped_queues);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.int_jobs: %d\n", Bnr.Param_Bits.int_jobs);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.int_jobs: %d\n", BannerVmsNodeName, Bnr.Param_Bits.int_jobs);
*29E
     BannerSetResource(string, &db, &db1);
 
*19E
*29D
     sprintf (string, "DECW$BANNER.dua0: %d\n", Mon.dua0);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.dua0: %d\n", BannerVmsNodeName, Mon.dua0);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.dua1: %d\n", Mon.dua1);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.dua1: %d\n", BannerVmsNodeName, Mon.dua1);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.dua2: %d\n", Mon.dua2);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.dua2: %d\n", BannerVmsNodeName, Mon.dua2);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.pio: %d\n", Mon.pio);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.pio: %d\n", BannerVmsNodeName, Mon.pio);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.pf: %d\n", Mon.pf);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.pf: %d\n", BannerVmsNodeName, Mon.pf);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.sysflt: %d\n", Mon.sysflt);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.sysflt: %d\n", BannerVmsNodeName, Mon.sysflt);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.ni_io: %d\n", Mon.ni_io);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.ni_io: %d\n", BannerVmsNodeName, Mon.ni_io);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.mp: %d\n", Mon.mp);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.mp: %d\n", BannerVmsNodeName, Mon.mp);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.fp: %d\n", Mon.fp);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.fp: %d\n", BannerVmsNodeName, Mon.fp);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.bell: %d\n", Bnr.Param_Bits.bell);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.bell: %d\n", BannerVmsNodeName, Bnr.Param_Bits.bell);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.bell_volume: %d\n", Bnr.bell_volume);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.bell_volume: %d\n", BannerVmsNodeName, Bnr.bell_volume);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.icon_on: %d\n", Bnr.Param_Bits.icon_on);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.icon_on: %d\n", BannerVmsNodeName, Bnr.Param_Bits.icon_on);
*29E
     BannerSetResource(string, &db, &db1);
 
*29D
     sprintf (string, "DECW$BANNER.base_priority: %d\n", Bnr.base_priority);
*29E
*29I
     sprintf (string, "DECW$BANNER_%s.base_priority: %d\n", BannerVmsNodeName, Bnr.base_priority);
*29E
     BannerSetResource(string, &db, &db1);
 
 
 /*
  * now write it out
  */
*29D
 /*    fclose(&db);*/
*29E
*24D
     XrmPutFileDatabase (db, "DECW$USER_DEFAULTS:DECW$BANNER.DAT");
*24E
*24I
     XrmPutFileDatabase (db, nodedb);
*24E
     return 1;
 }
 
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **	BannerStrToNumeric (resource)
 **
 **      This routine copnvers a number in ascii form, to a numeric value.
 **
 **  FORMAL PARAMETERS:
 **
 **      resource	The resource string.
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
 **      The integer value of the resource
 **
 **  SIDE EFFECTS:
 **
 **      none
 **
 **--
 **/
 static int	
 BannerStrToNumeric(resource)
 char *resource;
 {
     int rslt;
 
 /*
  * loop through the resource, and add up the value of the string
  */
 
     rslt = 0;
 
     while (1)
 	{
 	if (*resource == '\0')
 	    return rslt;
 	rslt = rslt * 10 + ((*resource) - 48);
 	resource++;
 	}    
 }
 
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **	BannerSetDefaultResource()
 **
 **      This routine sets up the Brn block, with all the default resources.
 **
 **  FORMAL PARAMETERS:
 **
 **
 **  IMPLICIT INPUTS:
 **
 **
 **  IMPLICIT OUTPUTS:
 **
 **
 **  COMPLETION CODES:
 **
 **
 **  SIDE EFFECTS:
 **
 **
 **--
 **/
 void	
 BannerSetDefaultResource()
 {
 
 /*
  * first initialize all the string parameters.
  */
 
     Bnr.fnt_name = 0;
     Bnr.title = 0;
 
*41I
     BannerVmsDisplayName = NULL;
*41E
     BannerReadResource (1);	/* Application DB */;
 
     if (Bnr.fnt_name == 0)
*2D
 	BannerSignalError ("Failed to open Application Xdefault database SYS$SHARE:BANNER.DAT",
*2E
*2I
*18D
 	BannerSignalError ("Failed to open Application Xdefault database SYS$LIBRARY:DECW$BANNER.DAT",
*18E
*18I
 	BannerSignalError ("Failed to open Application Xdefault database DECW$SYSTEM_DEFAULTS:DECW$BANNER.DAT",
*18E
*2E
 	    0);
*24I
*29D
 
     BannerReadResource (2);	/* Node DB */;
 
*29E
*24E
 }
 
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **	BannerMenuPopup (event)
 **
 **      This routine is activated when the user presses MB2 on the mouse.
 **	It activates a popup menu, with options for the user to modify
 **	the activites of Banner.
 **
 **  FORMAL PARAMETERS:
 **
 **      event	    The X MB2 event.
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
 BannerMenuPopup(event)
 XEvent *event;
 {
     int class, height, width;
 /*
  * Call DRM to get our popup menu realised.
  */
 
     memcpy(&saveevent, event, sizeof(XEvent));
     BannerMenuPopupDestroy();
 
     savewidget = &Bnr.dp_id;
 /*
  * Now look at the event, X,Y coordinates, and if the menu would go off
  * the screen path the X,Y coordinates so it will stay on.
  */
 
     if (event->xbutton.x_root + menu_width/2 > DisplayWidth(XtDisplay (menu_widget),0))
 	{
 	event->xbutton.x_root = event->xbutton.x_root - menu_width/2;
 	}
     if (event->xbutton.x_root < menu_width/2)
 	{
 	event->xbutton.x_root = event->xbutton.x_root + menu_width/2;
 	}
     if (event->xbutton.y_root + menu_height*0.9 > DisplayHeight(XtDisplay(menu_widget),0))
 	{
 	event->xbutton.y_root = event->xbutton.y_root - menu_height*0.9;
 	}
     if (event->xbutton.y_root < menu_height*0.1)
 	{
 	event->xbutton.y_root = event->xbutton.y_root + menu_height*0.1;
 	}
 
 
  /*
  * now get it possitioned correctly
  */
 
*17D
     DwtMenuPosition (menu_widget, event);
*17E
*17I
     XmMenuPosition (menu_widget, event);
*17E
 /*
  * Now realise it and we are done.
  */
     XtManageChild (menu_widget);
     XtRealizeWidget (menu_widget);
     XtMapWidget (menu_widget);
 
 
     savewidget = &Bnr.dp_id;
 
 }
 
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **	BannerMenuPopupDestroy ()
 **
 **      This routine destroys the popup menu, when the user releases the 
 **	MB2 button or presses another button on the mouse.
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
 BannerMenuPopupDestroy()
 {
 /*    if (menu_widget != 0)
 	XtDestroyWidget(menu_widget);
 */
 /*    menu_widget = 0;*/
 }
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **	SaveWindowXY ()
 **
 **      This routine Save the X,Y coo-ordinates for all our windows.
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
 SaveWindowXY()
 {
 /*
  * if we have seperate windows find out our X,Y positions.
  */
     {
     if (Bnr.cpu_shell_widget != 0)
 	{
 	BannerGetArg(Bnr.cpu_shell_widget,
*17D
 	    DwtNx,
*17E
*17I
 	    XmNx,
*17E
 	    &Bnr.cpu_x);
 	BannerGetArg(Bnr.cpu_shell_widget,
*17D
 	    DwtNy,
*17E
*17I
 	    XmNy,
*17E
 	    &Bnr.cpu_y);
 	}
     if (Bnr.pe_shell_widget != 0)
 	{
 	BannerGetArg(Bnr.pe_shell_widget,
*17D
 	    DwtNx,
*17E
*17I
 	    XmNx,
*17E
 	    &Bnr.pe_x);
 	BannerGetArg(Bnr.pe_shell_widget,
*17D
 	    DwtNy,
*17E
*17I
 	    XmNy,
*17E
 	    &Bnr.pe_y);
 	}
     if (Bnr.lck_shell_widget != 0)
 	{
 	BannerGetArg(Bnr.lck_shell_widget,
*17D
 	    DwtNx,
*17E
*17I
 	    XmNx,
*17E
 	    &Bnr.lck_x);
 	BannerGetArg(Bnr.lck_shell_widget,
*17D
 	    DwtNy,
*17E
*17I
 	    XmNy,
*17E
 	    &Bnr.lck_y);
 	}
     if (Bnr.cwps_shell_widget != 0)
 	{
 	BannerGetArg(Bnr.cwps_shell_widget,
*17D
 	    DwtNx,
*17E
*17I
 	    XmNx,
*17E
 	    &Bnr.cwps_x);
 	BannerGetArg(Bnr.cwps_shell_widget,
*17D
 	    DwtNy,
*17E
*17I
 	    XmNy,
*17E
 	    &Bnr.cwps_y);
 	}
*7I
     if (Bnr.sys_shell_widget != 0)
 	{
 	BannerGetArg(Bnr.sys_shell_widget,
*17D
 	    DwtNx,
*17E
*17I
 	    XmNx,
*17E
 	    &Bnr.sys_x);
 	BannerGetArg(Bnr.sys_shell_widget,
*17D
 	    DwtNy,
*17E
*17I
 	    XmNy,
*17E
 	    &Bnr.sys_y);
 	}
*7E
*19I
     if (Bnr.tp_shell_widget != 0)
 	{
 	BannerGetArg(Bnr.tp_shell_widget,
 	    XmNx,
 	    &Bnr.tp_x);
 	BannerGetArg(Bnr.tp_shell_widget,
 	    XmNy,
 	    &Bnr.tp_y);
 	}
     if (Bnr.qp_shell_widget != 0)
 	{
 	BannerGetArg(Bnr.qp_shell_widget,
 	    XmNx,
 	    &Bnr.qp_x);
 	BannerGetArg(Bnr.qp_shell_widget,
 	    XmNy,
 	    &Bnr.qp_y);
 	}
     if (Bnr.qb_shell_widget != 0)
 	{
 	BannerGetArg(Bnr.qb_shell_widget,
 	    XmNx,
 	    &Bnr.qb_x);
 	BannerGetArg(Bnr.qb_shell_widget,
 	    XmNy,
 	    &Bnr.qb_y);
 	}
     if (Bnr.clu_shell_widget != 0)
 	{
 	BannerGetArg(Bnr.clu_shell_widget,
 	    XmNx,
 	    &Bnr.clu_x);
 	BannerGetArg(Bnr.clu_shell_widget,
 	    XmNy,
 	    &Bnr.clu_y);
 	}
*19E
*35D
*34A1D
     if (Bnr.mon_shell_widget != 0)
*34A1E
*34A1I
     if (Bnr.link_shell_widget != 0)
*34A1E
*35E
*35I
*36D
     if (Bnr.link_shell_widget != 0)
*36E
*36I
*37I
*38D
 **************** Conflict 20    ************************************************
*38E
*37E
     if (Bnr.mon_shell_widget != 0)
*36E
*35E
*37I
*38D
 ********************************************************************************
*38E
*38I
 	{
 	BannerGetArg(Bnr.mon_shell_widget,
 	    XmNx,
 	    &Bnr.mon_x);
 	BannerGetArg(Bnr.mon_shell_widget,
 	    XmNy,
 	    &Bnr.mon_y);
 	}
*38E
     if (Bnr.link_shell_widget != 0)
*38D
 ************** End of Conflict 20    *******************************************
*38E
*37E
 	{
*35D
*34A1D
 	BannerGetArg(Bnr.mon_shell_widget,
*34A1E
*34A1I
 	BannerGetArg(Bnr.link_shell_widget,
*34A1E
*35E
*35I
*36D
 	BannerGetArg(Bnr.link_shell_widget,
*36E
*36I
*37I
*38D
 **************** Conflict 21    ************************************************
*38E
*37E
*38D
 	BannerGetArg(Bnr.mon_shell_widget,
*38E
*36E
*35E
*17D
 	    DwtNx,
*17E
*17I
*37D
 	    XmNx,
*37E
*37I
*38D
 ********************************************************************************
 	BannerGetArg(Bnr.link_shell_widget,
 ************** End of Conflict 21    *******************************************
 	    XmNx,
 **************** Conflict 22    ************************************************
*38E
*37E
*17E
*35D
*34A1D
 	    &Bnr.mon_x);
 	BannerGetArg(Bnr.mon_shell_widget,
*34A1E
*34A1I
 	    &Bnr.link_x);
 	BannerGetArg(Bnr.link_shell_widget,
*34A1E
*35E
*35I
*36D
 	    &Bnr.link_x);
 	BannerGetArg(Bnr.link_shell_widget,
*36E
*36I
*38D
 	    &Bnr.mon_x);
 	BannerGetArg(Bnr.mon_shell_widget,
*38E
*36E
*35E
*17D
 	    DwtNy,
*17E
*17I
*37D
 	    XmNy,
*37E
*37I
*38D
 ********************************************************************************
*38E
*38I
 	BannerGetArg(Bnr.link_shell_widget,
 	    XmNx,
*38E
 	    &Bnr.link_x);
 	BannerGetArg(Bnr.link_shell_widget,
*38D
 ************** End of Conflict 22    *******************************************
 	    XmNy,
 **************** Conflict 23    ************************************************
*38E
*37E
*17E
*35D
*34A1D
 	    &Bnr.mon_y);
*34A1E
*34A1I
 	    &Bnr.link_y);
*34A1E
*35E
*35I
*36D
 	    &Bnr.link_y);
*36E
*36I
*38D
 	    &Bnr.mon_y);
*38E
*36E
*35E
*37I
*38D
 ********************************************************************************
*38E
*38I
 	    XmNy,
*38E
 	    &Bnr.link_y);
*38D
 ************** End of Conflict 23    *******************************************
*38E
*37E
 	}
     if (Bnr.process_shell_widget != 0)
 	{
 	BannerGetArg(Bnr.process_shell_widget,
*17D
 	    DwtNx,
*17E
*17I
 	    XmNx,
*17E
 	    &Bnr.process_x);
 	BannerGetArg(Bnr.process_shell_widget,
*17D
 	    DwtNy,
*17E
*17I
 	    XmNy,
*17E
 	    &Bnr.process_y);
 	}
     if (Bnr.message_shell_widget != 0)
 	{
 	BannerGetArg(Bnr.message_shell_widget,
*17D
 	    DwtNx,
*17E
*17I
 	    XmNx,
*17E
 	    &Bnr.message_x);
 	BannerGetArg(Bnr.message_shell_widget,
*17D
 	    DwtNy,
*17E
*17I
 	    XmNy,
*17E
 	    &Bnr.message_y);
 	BannerGetArg(Bnr.message_shell_widget,
*17D
 	    DwtNwidth,
*17E
*17I
 	    XmNwidth,
*17E
 	    &Bnr.message_width);
 	BannerGetArg(Bnr.message_shell_widget,
*17D
 	    DwtNheight,
*17E
*17I
 	    XmNheight,
*17E
 	    &Bnr.message_height);
 	}
     if (Bnr.cube_shell_widget != 0)
 	{
 	BannerGetArg(Bnr.cube_shell_widget,
*17D
 	    DwtNx,
*17E
*17I
 	    XmNx,
*17E
 	    &Bnr.cube_x);
 	BannerGetArg(Bnr.cube_shell_widget,
*17D
 	    DwtNy,
*17E
*17I
 	    XmNy,
*17E
 	    &Bnr.cube_y);
 	}
*15I
     if (Bnr.hanoi_shell_widget != 0)
 	{
 	BannerGetArg(Bnr.hanoi_shell_widget,
*17D
 	    DwtNx,
*17E
*17I
 	    XmNx,
*17E
 	    &Bnr.hanoi_x);
 	BannerGetArg(Bnr.hanoi_shell_widget,
*17D
 	    DwtNy,
*17E
*17I
 	    XmNy,
*17E
 	    &Bnr.hanoi_y);
 	}
     if (Bnr.globe_shell_widget != 0)
 	{
 	BannerGetArg(Bnr.globe_shell_widget,
*17D
 	    DwtNx,
*17E
*17I
 	    XmNx,
*17E
 	    &Bnr.globe_x);
 	BannerGetArg(Bnr.globe_shell_widget,
*17D
 	    DwtNy,
*17E
*17I
 	    XmNy,
*17E
 	    &Bnr.globe_y);
 	}
*15E
     if (Bnr.clock_shell_widget != 0)
 	{
 	BannerGetArg(Bnr.clock_shell_widget,
*17D
 	    DwtNx,
*17E
*17I
 	    XmNx,
*17E
 	    &Bnr.clock_x);
 	BannerGetArg(Bnr.clock_shell_widget,
*17D
 	    DwtNy,
*17E
*17I
 	    XmNy,
*17E
 	    &Bnr.clock_y);
 	}
     }
 }
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **	BannerMenuCallback (widget, tag);
 **
 **      This routine is the callback routine for when someone choises an
 **	option from the banner menu callback.
 **
 **  FORMAL PARAMETERS:
 **
 **      widget, tag
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
 BannerMenuCallback(widget, tag)
 Widget widget;
 int *tag;
 {
 
*28I
     BannerDisplayCursor (widget, 0);
 
*28E
     switch (*tag)
     {
 
     case 1:		/* Exit option */
 	{
 	/*
 	 * Set our priority back to what it should be
 	 */
 	SYS$SETPRI (NULL, NULL, BannerInitialPriority, NULL);
 	exit (1);
 	break;
 	}
 	
     case 2:		/* Setup Options */
 	{
 	int class;
 	
 	if (Bnr.setup_widget != 0)
 	    {
 	    XtDestroyWidget (Bnr.setup_widget);
 /*
  * Now syncronize with the Server
  */
 	    XSync (XtDisplay (Bnr.dp_id), 0);
 	    Bnr.setup_widget=0;
 	    }
 
 	SaveWindowXY ();
 
 	font_changed=0;
 
*17D
 	DwtFetchWidget (Bnr.drm_prt,
*17E
*17I
 	MrmFetchWidget (Bnr.drm_prt,
*17E
 	    "BANNER_SETUP_OPTIONS",
 	    *savewidget,
 	    &Bnr.setup_widget,
 	    &class);
 
*22I
*29D
 	if (!Bnr.Param_Bits.border)
 	    BannerSetArg (Bnr.setup_widget->core.parent, 
 		XmNoverrideRedirect, 1); 
*29E
*22E
 
 	BannerCheckPopupPos (Bnr.setup_widget, setup_width, setup_height);
 	XtManageChild (Bnr.setup_widget);
 	XtRealizeWidget (Bnr.setup_widget);
*23I
*29D
 	XtCallAcceptFocus (Bnr.setup_widget, NULL);
*29E
*23E
*26I
*29D
 	XSetInputFocus (XtDisplay(toggle_vector[banner_disk_0_name]), 
 	    XtWindow(toggle_vector[banner_disk_0_name]),
 	    RevertToParent, NULL);
*29E
*26E
 	break;
 	}    
 
     case 3:		    /* restore original defaults */
 	{
 	Widget message_widget;
 	int class, width, height;
 
 	if (Bnr.setup_widget  != 0)
 	    {
 	    XtDestroyWidget (Bnr.setup_widget);
 /*
  * Now syncronize with the Server
  */
 	    XSync (XtDisplay (Bnr.dp_id), 0);
 	    Bnr.setup_widget=0;
 	    }
 	/*
 	 * Read the system defaults file, to reset all the defaults.
 	 */
 	memcpy(&New_Bnr, &Bnr, sizeof(Bnr) );
 	BannerSetDefaultResource ();
 
 /*
  * Now set a new font if there is one.
  */
 	sprintf (Bnr.fnt_name, Bnr.fnt_template, 
 	    banner_font_name, banner_font_wieght, banner_font_size,
 		banner_font_type);
 	BannerLoadFonts ();
 	XSetFont (XtDisplay(Bnr.dp_id), BannerGC, Bnr.font_ptr->fid);
 /*
  * get the right windows up
  */
 	BannerSeperateWindows(1);
*16D
 	/*
 	 * Now clear all the windows, so Banner will perform a reset.
 	 */
 	if (Bnr.Param_Bits.analog_clock || Bnr.Param_Bits.digital_clock)
 	    XClearWindow (XtDisplay(Bnr.clock_widget), XtWindow (Bnr.clock_widget));
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
 	if (Bnr.cube_widget != Bnr.dp_id)		
 	    {
 	    XClearWindow (XtDisplay(Bnr.cube_widget), 
 		XtWindow (Bnr.cube_widget));
*16E
*15D
 	    if (Bnr.Param_Bits.cube)
 		{
 		BannerCubeSetup ();
 		BannerCubeRefresh ();
 		}
 	    if (Bnr.Param_Bits.hanoi)
 		{
 		BannerHanoiSetup ();
 		BannerHanoiRefresh ();
 		}
 	    if (Bnr.Param_Bits.world)
 		BannerGlobe (0, 0, Bnr.cube_width, 
 		    Bnr.cube_height, 1);
*15E
*15I
*16D
 	    BannerCubeSetup ();
 	    BannerCubeRefresh ();
 	    }
 	if (Bnr.globe_widget != Bnr.dp_id)		
 	    {
 	    XClearWindow (XtDisplay(Bnr.globe_widget), 
 		XtWindow (Bnr.globe_widget));
 	    BannerGlobe (0, 0, Bnr.cube_width, 
 		Bnr.cube_height, 1);
 	    }
 	if (Bnr.hanoi_widget != Bnr.dp_id)		
 	    {
 	    XClearWindow (XtDisplay(Bnr.hanoi_widget), 
 		XtWindow (Bnr.hanoi_widget));
 	    BannerHanoiSetup ();
 	    BannerHanoiRefresh ();
*16E
*15E
*16D
 	    }
 	if (Bnr.cpu_widget != Bnr.dp_id)		
 	    {
 	    XClearWindow (XtDisplay(Bnr.cpu_widget), 
 		XtWindow (Bnr.cpu_widget));
 	    BannerCpuSetup ();
 	    BannerCpuRefresh ();
 	    }
 	if (Bnr.mon_widget != Bnr.dp_id)		
 	    {
 	    XClearWindow (XtDisplay(Bnr.mon_widget), 
 		XtWindow (Bnr.mon_widget));
 	    BannerMonSetup ();
 	    BannerMonRefresh ();
 	    }
 	if (Bnr.lck_widget != Bnr.dp_id)		
 	    {
 	    XClearWindow (XtDisplay(Bnr.lck_widget), 
 		XtWindow (Bnr.lck_widget));
 	    BannerLCKSetup ();
 	    BannerLCKRefresh ();
 	    }
 	if (Bnr.pe_widget != Bnr.dp_id)		
 	    {
 	    XClearWindow (XtDisplay(Bnr.pe_widget), 
 		XtWindow (Bnr.pe_widget));
 	    BannerPESetup ();
 	    BannerPERefresh ();
 	    }
 	if (Bnr.cwps_widget != Bnr.dp_id)		
 	    {
 	    XClearWindow (XtDisplay(Bnr.cwps_widget), 
 		XtWindow (Bnr.cwps_widget));
 	    BannerCWPSSetup ();
 	    BannerCWPSRefresh ();
 	    }
*16E
*7I
*16D
 	if (Bnr.sys_widget != Bnr.dp_id)		
 	    {
 	    XClearWindow (XtDisplay(Bnr.sys_widget), 
 		XtWindow (Bnr.sys_widget));
 	    BannerSYSSetup ();
 	    BannerSYSRefresh ();
 	    }
*16E
*7E
*16D
 	if (Bnr.process_widget != Bnr.dp_id)		
 	    {
 	    XClearWindow (XtDisplay(Bnr.process_widget), 
 		XtWindow (Bnr.process_widget));
 	    BannerProcessSetup ();
 	    BannerProcessRefresh ();
 	    }
*16E
*16I
 	BannerClearWindows ();
*16E
 
*17D
 	DwtFetchWidget (Bnr.drm_prt,
*17E
*17I
 	MrmFetchWidget (Bnr.drm_prt,
*17E
 		"BANNER_RESTORE_MESSAGE",
 		*savewidget,
 		&message_widget,
 		&class);
 
*22I
*29D
 	if (!Bnr.Param_Bits.border)
 	    BannerSetArg (message_widget->core.parent, 
 		XmNoverrideRedirect, 1); 
*29E
*22E
 
 
 	BannerCheckPopupPos (message_widget, other_width, other_height);
 	XtManageChild (message_widget);
 	XtRealizeWidget (message_widget);
 
 /*
  * Now check the priority we should run at
  */
 	if (Bnr.Param_Bits.cube || Bnr.Param_Bits.hanoi)
 	    Bnr.execute_priority = 0;
 	else
 	    Bnr.execute_priority = Bnr.base_priority;
 
 	/*
 	 * Now check if anything has changed that we cant handle dynamicly.
 	 */
 	if (Bnr.Param_Bits.border != New_Bnr.Param_Bits.border ||
 	    Bnr.Param_Bits.icon_on != New_Bnr.Param_Bits.icon_on)
 	    {
*17D
 	    DwtFetchWidget (Bnr.drm_prt,
*17E
*17I
 	    MrmFetchWidget (Bnr.drm_prt,
*17E
 		"BANNER_MESSAGE",
 		*savewidget,
 		&message_widget,
 		&class);
 
*22I
*29D
 	    if (!Bnr.Param_Bits.border)
 		BannerSetArg (message_widget->core.parent, 
 		    XmNoverrideRedirect, 1); 
*29E
*22E
 
 	    BannerCheckPopupPos (message_widget, other_width, other_height);
 	    XtManageChild (message_widget);
 	    XtRealizeWidget (message_widget);
 	    }
 	break;
 	}
     
     case 4:		    /* Save current Settings option */
 	{
 	Widget message_widget;
 	int class, width, height;
 
 	if (Bnr.setup_widget != 0)
 	    {
 	    XtDestroyWidget (Bnr.setup_widget);
 /*
  * Now syncronize with the Server
  */
 	    XSync (XtDisplay (Bnr.dp_id), 0);
 	    Bnr.setup_widget=0;
 	    }
 
 	SaveWindowXY ();
 
 	if (BannerWriteDatabase () == 1)
*17D
 	    DwtFetchWidget (Bnr.drm_prt,
*17E
*17I
 	    MrmFetchWidget (Bnr.drm_prt,
*17E
 		"BANNER_SAVE_MESSAGE",
 		*savewidget,
 		&message_widget,
 		&class);
 	else
*17D
 	    DwtFetchWidget (Bnr.drm_prt,
*17E
*17I
 	    MrmFetchWidget (Bnr.drm_prt,
*17E
 		"BANNER_NOT_SAVE_MESSAGE",
 		*savewidget,
 		&message_widget,
 		&class);
 
*22I
*29D
 	if (!Bnr.Param_Bits.border)
 	    BannerSetArg (message_widget->core.parent, 
 		XmNoverrideRedirect, 1); 
*29E
 
*22E
 	BannerCheckPopupPos (message_widget, other_width, other_height);
 	XtManageChild (message_widget);
 	XtRealizeWidget (message_widget);
 	break;
 	}
 
     case 5:		    /* restore saved defaults */
 	{
 	Widget message_widget;
 	int class, width, height;
 
 	if (Bnr.setup_widget  != 0)
 	    {
 	    XtDestroyWidget (Bnr.setup_widget);
 /*
  * Now syncronize with the Server
  */
 	    XSync (XtDisplay (Bnr.dp_id), 0);
 	    Bnr.setup_widget=0;
 	    }
 	/*
 	 * Read the system defaults file, to reset all the defaults.
 	 */
 	memcpy(&New_Bnr, &Bnr, sizeof(Bnr) );
 	BannerSetDefaultResource ();
 	BannerReadResource (0);	/* Private database */
 
 /*
  * Now set a new font if there is one.
  */
 	sprintf (Bnr.fnt_name, Bnr.fnt_template, 
 	    banner_font_name, banner_font_wieght, banner_font_size,
 		banner_font_type);
 	BannerLoadFonts ();
 	XSetFont (XtDisplay(Bnr.dp_id), BannerGC, Bnr.font_ptr->fid);
 /*
  * get the right windows up
  */
 	BannerSeperateWindows(1);
*16D
 	/*
 	 * Now clear all the windows, so Banner will perform a reset.
 	 */
 	if (Bnr.Param_Bits.analog_clock || Bnr.Param_Bits.digital_clock)
 	    XClearWindow (XtDisplay(Bnr.clock_widget), XtWindow (Bnr.clock_widget));
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
 	if (Bnr.cube_widget != Bnr.dp_id)		
 	    {
 	    XClearWindow (XtDisplay(Bnr.cube_widget), 
 		XtWindow (Bnr.cube_widget));
*16E
*15D
 	    if (Bnr.Param_Bits.cube)
 		{
 		BannerCubeSetup ();
 		BannerCubeRefresh ();
 		}
 	    if (Bnr.Param_Bits.hanoi)
 		{
 		BannerHanoiSetup ();
 		BannerHanoiRefresh ();
 		}
 	    if (Bnr.Param_Bits.world)
 		BannerGlobe (0, 0, Bnr.cube_width, 
 		    Bnr.cube_height, 1);
*15E
*15I
*16D
 	    BannerCubeSetup ();
 	    BannerCubeRefresh ();
 	    }
 	if (Bnr.globe_widget != Bnr.dp_id)		
 	    {
 	    XClearWindow (XtDisplay(Bnr.globe_widget), 
 		XtWindow (Bnr.globe_widget));
 	    BannerGlobe (0, 0, Bnr.cube_width, 
 		Bnr.cube_height, 1);
 	    }
 	if (Bnr.hanoi_widget != Bnr.dp_id)		
 	    {
 	    XClearWindow (XtDisplay(Bnr.hanoi_widget), 
 		XtWindow (Bnr.hanoi_widget));
 	    BannerHanoiSetup ();
 	    BannerHanoiRefresh ();
*16E
*15E
*16D
 	    }
 	if (Bnr.cpu_widget != Bnr.dp_id)		
 	    {
 	    XClearWindow (XtDisplay(Bnr.cpu_widget), 
 		XtWindow (Bnr.cpu_widget));
 	    BannerCpuSetup ();
 	    BannerCpuRefresh ();
 	    }
 	if (Bnr.mon_widget != Bnr.dp_id)		
 	    {
 	    XClearWindow (XtDisplay(Bnr.mon_widget), 
 		XtWindow (Bnr.mon_widget));
 	    BannerMonSetup ();
 	    BannerMonRefresh ();
 	    }
 	if (Bnr.lck_widget != Bnr.dp_id)		
 	    {
 	    XClearWindow (XtDisplay(Bnr.lck_widget), 
 		XtWindow (Bnr.lck_widget));
 	    BannerLCKSetup ();
 	    BannerLCKRefresh ();
 	    }
 	if (Bnr.pe_widget != Bnr.dp_id)		
 	    {
 	    XClearWindow (XtDisplay(Bnr.pe_widget), 
 		XtWindow (Bnr.pe_widget));
 	    BannerPESetup ();
 	    BannerPERefresh ();
 	    }
 	if (Bnr.cwps_widget != Bnr.dp_id)		
 	    {
 	    XClearWindow (XtDisplay(Bnr.cwps_widget), 
 		XtWindow (Bnr.cwps_widget));
 	    BannerCWPSSetup ();
 	    BannerCWPSRefresh ();
 	    }
*16E
*7I
*16D
 	if (Bnr.sys_widget != Bnr.dp_id)		
 	    {
 	    XClearWindow (XtDisplay(Bnr.sys_widget), 
 		XtWindow (Bnr.sys_widget));
 	    BannerSYSSetup ();
 	    BannerSYSRefresh ();
 	    }
*16E
*7E
*16D
 	if (Bnr.process_widget != Bnr.dp_id)		
 	    {
 	    XClearWindow (XtDisplay(Bnr.process_widget), 
 		XtWindow (Bnr.process_widget));
 	    BannerProcessSetup ();
 	    BannerProcessRefresh ();
 	    }
*16E
*16I
 	BannerClearWindows();
*16E
 
*17D
 	DwtFetchWidget (Bnr.drm_prt,
*17E
*17I
 	MrmFetchWidget (Bnr.drm_prt,
*17E
 		"BANNER_RESTORE_MESSAGE",
 		*savewidget,
 		&message_widget,
 		&class);
 
*22I
*29D
 	if (!Bnr.Param_Bits.border)
 	    BannerSetArg (message_widget->core.parent, 
 		XmNoverrideRedirect, 1); 
*29E
*22E
 
 
 	BannerCheckPopupPos (message_widget, other_width, other_height);
 	XtManageChild (message_widget);
 	XtRealizeWidget (message_widget);
 
 /*
  * Now check the priority we should run at
  */
 	if (Bnr.Param_Bits.cube || Bnr.Param_Bits.hanoi)
 	    Bnr.execute_priority = 0;
 	else
 	    Bnr.execute_priority = Bnr.base_priority;
 
 	/*
 	 * Now check if anything has changed that we cant handle dynamicly.
 	 */
 	if (Bnr.Param_Bits.border != New_Bnr.Param_Bits.border ||
 	    Bnr.Param_Bits.icon_on != New_Bnr.Param_Bits.icon_on)
 	    {
*17D
 	    DwtFetchWidget (Bnr.drm_prt,
*17E
*17I
 	    MrmFetchWidget (Bnr.drm_prt,
*17E
 		"BANNER_MESSAGE",
 		*savewidget,
 		&message_widget,
 		&class);
 
*22I
*29D
 	    if (!Bnr.Param_Bits.border)
 		BannerSetArg (message_widget->core.parent, 
 		    XmNoverrideRedirect, 1); 
*29E
 
*22E
 
 	    BannerCheckPopupPos (message_widget, other_width, other_height);
 	    XtManageChild (message_widget);
 	    XtRealizeWidget (message_widget);
 	    }
 	break;
 	}
     
 
     case 6 :		/* hide message window */
 	{
*25D
 	if (Bnr.Param_Bits.message_window)
 	    BannerMessageHide ();
 	else
 	    {
 	    int class, width, height;
 	    Widget message_widget;
*25E
*17D
 	    DwtFetchWidget (Bnr.drm_prt,
*17E
*17I
*25D
 	    MrmFetchWidget (Bnr.drm_prt,
*25E
*17E
*25D
 		"BANNER_NO_MESSAGE",
 		*savewidget,
 		&message_widget,
 		&class);
 
 
*25E
*22I
*25D
 	    if (!Bnr.Param_Bits.border)
 		BannerSetArg (message_widget->core.parent, 
 		    XmNoverrideRedirect, 1); 
*25E
*22E
*25D
 
 	    BannerCheckPopupPos (message_widget, other_width, other_height);
 	    XtManageChild (message_widget);
 	    XtRealizeWidget (message_widget);
 	    }
*25E
*25I
 	BannerMessageHide ();
*25E
 	break;
 	}
     case 7 :		/* show message window */
 	{
*25D
 	if (Bnr.Param_Bits.message_window)
 	    XtMapWidget (Bnr.message_shell_widget);
 	else
 	    {
 	    int class, width, height;
 	    Widget message_widget;
*25E
*17D
 	    DwtFetchWidget (Bnr.drm_prt,
*17E
*17I
*25D
 	    MrmFetchWidget (Bnr.drm_prt,
*25E
*17E
*25D
 		"BANNER_NO_MESSAGE",
 		*savewidget,
 		&message_widget,
 		&class);
 
 
*25E
*22I
*25D
 	    if (!Bnr.Param_Bits.border)
 		BannerSetArg (message_widget->core.parent, 
 		    XmNoverrideRedirect, 1); 
*25E
*22E
*25D
 
 	    BannerCheckPopupPos (message_widget, other_width, other_height);
 	    XtManageChild (message_widget);
 	    XtRealizeWidget (message_widget);
 	    }
*25E
*25I
 	XtMapWidget (Bnr.message_shell_widget);
*25E
 	break;
 	}
     case 8 :		/* Erase message window */
 	{
*25D
 	if (Bnr.Param_Bits.message_window)
 	    BannerMessageErase ();
 	else
 	    {
 	    int class, width, height;
 	    Widget message_widget;
*25E
*17D
 	    DwtFetchWidget (Bnr.drm_prt,
*17E
*17I
*25D
 	    MrmFetchWidget (Bnr.drm_prt,
*25E
*17E
*25D
 		"BANNER_NO_MESSAGE",
 		*savewidget,
 		&message_widget,
 		&class);
 
*25E
*22I
*25D
 	    if (!Bnr.Param_Bits.border)
 		BannerSetArg (message_widget->core.parent, 
 		    XmNoverrideRedirect, 1); 
*25E
*22E
*25D
 
 
 	    BannerCheckPopupPos (message_widget, other_width, other_height);
 	    XtManageChild (message_widget);
 	    XtRealizeWidget (message_widget);
 	    }
*25E
*25I
 	BannerMessageErase ();
*25E
 	break;
 	}
     case 9:		    /* Alarm */
 	{
 	int class, width, height;
 	
 	if (Bnr.alarm_widget != 0)
 	    {
 	    XtDestroyWidget (Bnr.alarm_widget);
 /*
  * Now syncronize with the Server
  */
 	    XSync (XtDisplay (Bnr.dp_id), 0);
 	    Bnr.alarm_widget=0;
 	    }
 
*17D
 	DwtFetchWidget (Bnr.drm_prt,
*17E
*17I
 	MrmFetchWidget (Bnr.drm_prt,
*17E
 	    "BANNER_ALARM_OPTIONS",
 	    *savewidget,
 	    &Bnr.alarm_widget,
 	    &class);
 
*22I
*29D
 	    if (!Bnr.Param_Bits.border)
 		BannerSetArg (Bnr.alarm_widget->core.parent, 
 		    XmNoverrideRedirect, 1); 
*29E
*22E
 
 
 	BannerCheckPopupPos (Bnr.alarm_widget, other_width, other_height);
 	XtManageChild (Bnr.alarm_widget);
 	XtRealizeWidget (Bnr.alarm_widget);
 	break;
 	}
     case 10:	    /* Alarm O.K */
 	{
 	int hour, minute;
 	char *alarmtext;
 /*
  * Get the values from the sub widgets
  */
 	BannerGetArg(toggle_vector[banner_hour_widget],
*17D
 	    DwtNvalue,
*17E
*17I
 	    XmNvalue,
*17E
 	    &hour);	
 
 	BannerGetArg(toggle_vector[banner_minute_widget],
*17D
 	    DwtNvalue,
*17E
*17I
 	    XmNvalue,
*17E
 	    &minute);	
 
*17D
 	alarmtext = DwtSTextGetString (toggle_vector[banner_alarmtext_widget]);
*17E
*17I
 	alarmtext = XmTextGetString (toggle_vector[banner_alarmtext_widget]);
*17E
 
 /*
  * Call the routine to establish the alarm
  */
 
 	BannerEstablishAlarm (hour, minute, alarmtext);
 
 	if (Bnr.alarm_widget != 0)
 	    {
 	    XtDestroyWidget (Bnr.alarm_widget);
 /*
  * Now syncronize with the Server
  */
 	    XSync (XtDisplay (Bnr.dp_id), 0);
 	    Bnr.alarm_widget=0;
 	    }
 	break;
 	}
     case 11:	    /* Alarm cancel */
 	{
 	if (Bnr.alarm_widget != 0)
 	    {
 	    XtDestroyWidget (Bnr.alarm_widget);
 /*
  * Now syncronize with the Server
  */
 	    XSync (XtDisplay (Bnr.dp_id), 0);
 	    Bnr.alarm_widget=0;
 	    }
 	break;
 	};    
     case 12:		    /* Help */
 	{
 	int class, width, height;
 	Widget helpwidget;
 
*17D
 	DwtFetchWidget (Bnr.drm_prt,
*17E
*17I
*29I
 	helpwidget = NULL;
 
*29E
 	MrmFetchWidget (Bnr.drm_prt,
*17E
 	    "BANNER_HELP_WIDGET",
 	    Bnr.dp_id,
 	    &helpwidget,
 	    &class);
 
*27D
 
*27E
*27I
*29D
 /*
*29E
*27E
*22I
*29D
 	if (!Bnr.Param_Bits.border)
 	    BannerSetArg (helpwidget->core.parent, 
 		XmNoverrideRedirect, 1); 
*29E
*27D
 
*27E
*27I
*29D
 */
*29E
*27E
*22E
 
*29D
 	BannerCheckPopupPos (helpwidget, other_width, other_height);
 	XtManageChild (helpwidget);
 	XtRealizeWidget (helpwidget);
*29E
*29I
 	if (helpwidget != NULL)
 	    {
 	    BannerCheckPopupPos (helpwidget, other_width, other_height);
 	    XtManageChild (helpwidget);
 	    XtRealizeWidget (helpwidget);
 	    }
*29E
 	break;
 	}
*11I
 
     case 13:		/* Set min display */
 	{
 
 
 /*
  * get the right windows up
  */
 	Bnr.Param_Bits.message_window = Bnr.Param_Bits.modes_window =
 	Bnr.Param_Bits.hanoi = Bnr.Param_Bits.world = 
 	Bnr.Param_Bits.analog_clock = Bnr.Param_Bits.digital_clock =
 	Bnr.Param_Bits.sys_window = Bnr.Param_Bits.cwps_window =
*19I
 	Bnr.Param_Bits.qp_window = Bnr.Param_Bits.qb_window =
 	Bnr.Param_Bits.tp_window = Bnr.Param_Bits.clu_window =
*19E
 	Bnr.Param_Bits.lck_window = Bnr.Param_Bits.cpu_window =
*35D
*34A1D
 	Bnr.Param_Bits.process_window = Bnr.Param_Bits.monitor =
*34A1E
*34A1I
 	Bnr.Param_Bits.process_window = Bnr.Param_Bits.link_window =
*34A1E
*35E
*35I
*36D
 	Bnr.Param_Bits.process_window = Bnr.Param_Bits.link_window =
*36E
*36I
*37I
*38D
 **************** Conflict 24    ************************************************
*38E
*37E
 	Bnr.Param_Bits.process_window = Bnr.Param_Bits.monitor =
*36E
*35E
*13D
 	Bnr.Param_Bits.cube = 0;
*13E
*13I
*37I
*38D
 ********************************************************************************
*38E
 	Bnr.Param_Bits.process_window = Bnr.Param_Bits.link_window =
*38D
 ************** End of Conflict 24    *******************************************
*38E
*37E
 	Bnr.Param_Bits.cube = Bnr.Param_Bits.pe_window = 0;
*13E
 
 	BannerSeperateWindows(1);
 
 	break;
 	}    
*32I
     case 14:		/* Sound Setup */
 	{
 	BannerSoundSelect (*savewidget);
 	break;
 	}    
*32E
 
*11E
     }
*28I
 
     BannerUndisplayCursor ();
 
*28E
 }
 
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **	BannerSetupCallback (widget, tag)
 **
 **      This routine is a callback routine which is activated as each 
 **	part of the banner setup options is created.
 **
 **  FORMAL PARAMETERS:
 **
 **      widget, tag
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
 BannerSetupCallback(widget, tag)
 Widget widget;
 int *tag;
 {
     switch (*tag)
     {
 
     case 0:		    /* Cancel */
 	{
 	XtDestroyWidget (Bnr.setup_widget);
 /*
  * Now syncronize with the Server
  */
 	XSync (XtDisplay (Bnr.dp_id), 0);
 	Bnr.setup_widget = 0;
 	break;
 	}
     case 1:		/* O.K */
*16I
     case 3:		/* APPLY */
*16E
 	{
 	int state, width, height, border_changed;
*22I
 	int cant_change = 0;
*22E
 
*17D
 	New_Bnr.disk_0 = DwtSTextGetString (toggle_vector[banner_disk_0]);
 	New_Bnr.disk_0_name = DwtSTextGetString (toggle_vector[banner_disk_0_name]);
 	New_Bnr.disk_1 = DwtSTextGetString (toggle_vector[banner_disk_1]);
 	New_Bnr.disk_1_name = DwtSTextGetString (toggle_vector[banner_disk_1_name]);
 	New_Bnr.disk_2 = DwtSTextGetString (toggle_vector[banner_disk_2]);
 	New_Bnr.disk_2_name = DwtSTextGetString (toggle_vector[banner_disk_2_name]);
*17E
*17I
*19I
 	New_Bnr.qp_name = XmTextGetString (toggle_vector[banner_print_queue_name]);
 	New_Bnr.qb_name = XmTextGetString (toggle_vector[banner_batch_queue_name]);
 	New_Bnr.queue_user = XmTextGetString (toggle_vector[banner_queue_user]);
     
 	state = XmTextGetString (toggle_vector[banner_max_comp_rate]);
*21D
 	New_Bnr.tp_max_rate = strtol(state);
*21E
*21I
 	New_Bnr.tp_max_rate = BannerStrToNumeric(state);
*21E
 	XtFree (state);
 	state = XmTextGetString (toggle_vector[banner_comp_rate1_start]);
*21D
 	New_Bnr.tp_rate1_start = strtol(state);
*21E
*21I
 	New_Bnr.tp_rate1_start = BannerStrToNumeric(state);
*21E
 	XtFree (state);
 	state = XmTextGetString (toggle_vector[banner_comp_rate2_start]);
*21D
 	New_Bnr.tp_rate2_start = strtol(state);
*21E
*21I
 	New_Bnr.tp_rate2_start = BannerStrToNumeric(state);
*21E
 	XtFree (state);
 	state = XmTextGetString (toggle_vector[banner_comp_rate3_start]);
*21D
 	New_Bnr.tp_rate3_start = strtol(state);
*21E
*21I
 	New_Bnr.tp_rate3_start = BannerStrToNumeric(state);
*21E
 	XtFree (state);
 	state = XmTextGetString (toggle_vector[banner_comp_rate4_start]);
*21D
 	New_Bnr.tp_rate4_start = strtol(state);
*21E
*21I
 	New_Bnr.tp_rate4_start = BannerStrToNumeric(state);
*21E
 	XtFree (state);
 	state = XmTextGetString (toggle_vector[banner_comp_rate1_end]);
*21D
 	New_Bnr.tp_rate1_end = strtol(state);
*21E
*21I
 	New_Bnr.tp_rate1_end = BannerStrToNumeric(state);
*21E
 	XtFree (state);
 	state = XmTextGetString (toggle_vector[banner_comp_rate2_end]);
*21D
 	New_Bnr.tp_rate2_end = strtol(state);
*21E
*21I
 	New_Bnr.tp_rate2_end = BannerStrToNumeric(state);
*21E
 	XtFree (state);
 	state = XmTextGetString (toggle_vector[banner_comp_rate3_end]);
*21D
 	New_Bnr.tp_rate3_end = strtol(state);
*21E
*21I
 	New_Bnr.tp_rate3_end = BannerStrToNumeric(state);
*21E
 	XtFree (state);
 
*19E
 	New_Bnr.disk_0 = XmTextGetString (toggle_vector[banner_disk_0]);
 	New_Bnr.disk_0_name = XmTextGetString (toggle_vector[banner_disk_0_name]);
 	New_Bnr.disk_1 = XmTextGetString (toggle_vector[banner_disk_1]);
 	New_Bnr.disk_1_name = XmTextGetString (toggle_vector[banner_disk_1_name]);
 	New_Bnr.disk_2 = XmTextGetString (toggle_vector[banner_disk_2]);
 	New_Bnr.disk_2_name = XmTextGetString (toggle_vector[banner_disk_2_name]);
*17E
 
 	BannerGetArg(toggle_vector[banner_tick_size],
*17D
 	    DwtNvalue,
*17E
*17I
 	    XmNvalue,
*17E
 	    &state);	
 	New_Bnr.cpu_count = state;
 
 	BannerGetArg(toggle_vector[banner_base_priority],
*17D
 	    DwtNvalue,
*17E
*17I
 	    XmNvalue,
*17E
 	    &state);	
 	New_Bnr.base_priority = state;
 
 
*16I
 	New_Bnr.setup_widget = Bnr.setup_widget;
*16E
 
*16D
 	XtDestroyWidget (Bnr.setup_widget);
*16E
*16I
 	if (*tag == 1)	    /* Not apply */
 	    {
 	    XtDestroyWidget (Bnr.setup_widget);
*16E
 /*
  * Now syncronize with the Server
  */
*16D
 	XSync (XtDisplay (Bnr.dp_id), 0);
 	Bnr.setup_widget = New_Bnr.setup_widget = 0;
     
*16E
*16I
 	    XSync (XtDisplay (Bnr.dp_id), 0);
 	    Bnr.setup_widget = New_Bnr.setup_widget = 0;
 	    }    
*16E
 
 	/*
 	 * Now check if anything has changed that we cant handle dynamicly.
 	 */
 	if (Bnr.Param_Bits.border != New_Bnr.Param_Bits.border ||
 	    Bnr.Param_Bits.icon_on != New_Bnr.Param_Bits.icon_on)
*22D
 	    {
 	    Widget message_widget;
 	    int class;
 
*22E
*17D
 	    DwtFetchWidget (Bnr.drm_prt,
*17E
*17I
*22D
 	    MrmFetchWidget (Bnr.drm_prt,
*22E
*17E
*22D
 		"BANNER_MESSAGE",
 		*savewidget,
 		&message_widget,
 		&class);
 
 
 
 	    BannerCheckPopupPos (message_widget, other_width, other_height);
 	    XtManageChild (message_widget);
 	    XtRealizeWidget (message_widget);
 	    }
*22E
*22I
 	    cant_change = 1;
*22E
     
 	if (Bnr.Param_Bits.window_border != New_Bnr.Param_Bits.window_border)
 	    border_changed = 1;
 	else
 	    border_changed = 0;
 	memcpy(&Bnr, &New_Bnr, sizeof(Bnr) );
 	memcpy(&Mon, &New_Mon, sizeof(Mon));
 
 /*
  * Now set a new font if there is one.
  */
 	sprintf (Bnr.fnt_name, Bnr.fnt_template, 
 	    banner_font_name, banner_font_wieght, banner_font_size,
 		banner_font_type);
 	if ( font_changed == 1)
 	    {
 	    BannerLoadFonts ();
 	    XSetFont (XtDisplay(Bnr.dp_id), BannerGC, Bnr.font_ptr->fid);
 	    }
 /*
  * get the right windows up
  */
 	BannerSeperateWindows(border_changed);
*16D
 	/*
 	 * Now clear all the windows, so Banner will perform a reset.
 	 */
 	if (Bnr.Param_Bits.analog_clock || Bnr.Param_Bits.digital_clock)
 	    XClearWindow (XtDisplay(Bnr.clock_widget), XtWindow (Bnr.clock_widget));
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
 
 	if (Bnr.cube_widget != Bnr.dp_id)		
 	    {
 	    XClearWindow (XtDisplay(Bnr.cube_widget), 
 		XtWindow (Bnr.cube_widget));
*16E
*15D
 	    if (Bnr.Param_Bits.cube)
 		{
 		BannerCubeSetup ();
 		BannerCubeRefresh ();
 		}
 	    if (Bnr.Param_Bits.hanoi)
 		{
 		BannerHanoiSetup ();
 		BannerHanoiRefresh ();
 		}
 	    if (Bnr.Param_Bits.world)
 		BannerGlobe (0, 0, Bnr.cube_width, 
 		    Bnr.cube_height, 1);
*15E
*15I
*16D
 	    BannerCubeSetup ();
 	    BannerCubeRefresh ();
 	    }
 	if (Bnr.hanoi_widget != Bnr.dp_id)		
 	    {
 	    XClearWindow (XtDisplay(Bnr.hanoi_widget), 
 		XtWindow (Bnr.hanoi_widget));
 	    BannerHanoiSetup ();
 	    BannerHanoiRefresh ();
 	    }
 	if (Bnr.globe_widget != Bnr.dp_id)		
 	    {
 	    XClearWindow (XtDisplay(Bnr.globe_widget), 
 		XtWindow (Bnr.globe_widget));
 	    BannerGlobe (0, 0, Bnr.cube_width, 
 		Bnr.cube_height, 1);
*16E
*15E
*16D
 	    }
 	if (Bnr.cpu_widget != Bnr.dp_id)		
 	    {
 	    XClearWindow (XtDisplay(Bnr.cpu_widget), 
 		XtWindow (Bnr.cpu_widget));
 	    BannerCpuSetup ();
 	    BannerCpuRefresh ();
 	    }
 	if (Bnr.pe_widget != Bnr.dp_id)		
 	    {
 	    XClearWindow (XtDisplay(Bnr.pe_widget), 
 		XtWindow (Bnr.pe_widget));
 	    BannerPESetup ();
 	    BannerPERefresh ();
 	    }
 	if (Bnr.lck_widget != Bnr.dp_id)		
 	    {
 	    XClearWindow (XtDisplay(Bnr.lck_widget), 
 		XtWindow (Bnr.lck_widget));
 	    BannerLCKSetup ();
 	    BannerLCKRefresh ();
 	    }
 	if (Bnr.cwps_widget != Bnr.dp_id)		
 	    {
 	    XClearWindow (XtDisplay(Bnr.cwps_widget), 
 		XtWindow (Bnr.cwps_widget));
 	    BannerCWPSSetup ();
 	    BannerCWPSRefresh ();
 	    }
*16E
*7I
*16D
 	if (Bnr.sys_widget != Bnr.dp_id)		
 	    {
 	    XClearWindow (XtDisplay(Bnr.sys_widget), 
 		XtWindow (Bnr.sys_widget));
 	    BannerSYSSetup ();
 	    BannerSYSRefresh ();
 	    }
*16E
*7E
*16D
 	if (Bnr.mon_widget != Bnr.dp_id)		
 	    {
 	    XClearWindow (XtDisplay(Bnr.mon_widget), 
 		XtWindow (Bnr.mon_widget));
 	    BannerMonSetup ();
 	    BannerMonRefresh ();
 	    }
 	if (Bnr.process_widget != Bnr.dp_id)		
 	    {
 	    XClearWindow (XtDisplay(Bnr.process_widget), 
 		XtWindow (Bnr.process_widget));
 	    BannerProcessSetup ();
 	    BannerProcessRefresh ();
 	    }
 
*16E
*16I
 	BannerClearWindows ();
*16E
 /*
  * Now check the priority we should run at
  */
 	if (Bnr.Param_Bits.cube || Bnr.Param_Bits.hanoi)
 	    Bnr.execute_priority = 0;
 	else
 	    Bnr.execute_priority = Bnr.base_priority;
 
*16I
 	memcpy(&New_Bnr, &Bnr, sizeof(Bnr) );
 	memcpy(&New_Mon, &Mon, sizeof(Mon));
*22I
 /*
  * Do we need to say we cant change?
  */
 	if (cant_change)
 	    {
 	    Widget message_widget;
 	    int class;
 
 	    MrmFetchWidget (Bnr.drm_prt,
 		"BANNER_MESSAGE",
 		*savewidget,
 		&message_widget,
 		&class);
 
*29D
 	    if (!Bnr.Param_Bits.border)
 		BannerSetArg (message_widget->core.parent, 
 		    XmNoverrideRedirect, 1); 
*29E
 
 
 	    BannerCheckPopupPos (message_widget, other_width, other_height);
 	    XtManageChild (message_widget);
 	    XtRealizeWidget (message_widget);
 	    }
*22E
 
*16E
 	break;
 	}
     case 2 :			/* Initialise */
 	{
 	memcpy(&New_Bnr, &Bnr, sizeof(Bnr) );
 	memcpy(&New_Mon, &Mon, sizeof(Mon));
 	}
     }
 }
 
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **	BannerToggleInitCallback (widget, tag)
 **
 **      This routine is callback as each of the sub children in the setup
 **	options are created.
 **
 **  FORMAL PARAMETERS:
 **
 **      widget, tag
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
 BannerToggleInitCallback(widget, tag)
 Widget widget;
 int *tag;
 {
     int state;
*17I
     int valuetoset = XmNset;
*17E
 
     switch (*tag)
 	     {
 	     case banner_font_name_c:
 		{
 		toggle_vector[banner_font_name_c] = widget;
 		if (strcmp(banner_font_name, "ITC Avant Garde Gothic") == 0)
 		    BannerSetArg(toggle_vector[banner_font_name_c], 
*17D
 			DwtNmenuHistory, toggle_vector[avantgarde_font]);
*17E
*17I
 			XmNmenuHistory, toggle_vector[avantgarde_font]);
*17E
 		if (strcmp(banner_font_name, "Courier") == 0)
 		    BannerSetArg(toggle_vector[banner_font_name_c], 
*17D
 			DwtNmenuHistory, toggle_vector[courier_font]);
*17E
*17I
 			XmNmenuHistory, toggle_vector[courier_font]);
*17E
 		if (strcmp(banner_font_name, "Helvetica") == 0)
 		    BannerSetArg(toggle_vector[banner_font_name_c], 
*17D
 			DwtNmenuHistory, toggle_vector[helvetica_font]);
*17E
*17I
 			XmNmenuHistory, toggle_vector[helvetica_font]);
*17E
 		if (strcmp(banner_font_name, "ITC Lubalin Graph") == 0)
 		    BannerSetArg(toggle_vector[banner_font_name_c], 
*17D
 			DwtNmenuHistory, toggle_vector[lubalingraph_font]);
*17E
*17I
 			XmNmenuHistory, toggle_vector[lubalingraph_font]);
*17E
 		if (strcmp(banner_font_name, "New Century Schoolbook") == 0)
 		    BannerSetArg(toggle_vector[banner_font_name_c], 
*17D
 			DwtNmenuHistory, toggle_vector[ncsschoolbook_font]);
*17E
*17I
 			XmNmenuHistory, toggle_vector[ncsschoolbook_font]);
*17E
 		if (strcmp(banner_font_name, "ITC Souvenir") == 0)
 		    BannerSetArg(toggle_vector[banner_font_name_c], 
*17D
 			DwtNmenuHistory, toggle_vector[souvenir_font]);
*17E
*17I
 			XmNmenuHistory, toggle_vector[souvenir_font]);
*17E
 		if (strcmp(banner_font_name, "Times") == 0)
 		    BannerSetArg(toggle_vector[banner_font_name_c], 
*17D
 			DwtNmenuHistory, toggle_vector[times_font]);
*17E
*17I
 			XmNmenuHistory, toggle_vector[times_font]);
*17E
 		state=0;
 		break;
 		}
 	     case banner_font_size_c:
 		{
 		toggle_vector[banner_font_size_c] = widget;
 		if (strcmp(banner_font_size, "8") == 0)
 		    BannerSetArg(toggle_vector[banner_font_size_c], 
*17D
 			DwtNmenuHistory, toggle_vector[a8_point]);
*17E
*17I
 			XmNmenuHistory, toggle_vector[a8_point]);
*17E
 		if (strcmp(banner_font_size, "10") == 0)
 		    BannerSetArg(toggle_vector[banner_font_size_c], 
*17D
 			DwtNmenuHistory, toggle_vector[a10_point]);
*17E
*17I
 			XmNmenuHistory, toggle_vector[a10_point]);
*17E
 		if (strcmp(banner_font_size, "12") == 0)
 		    BannerSetArg(toggle_vector[banner_font_size_c], 
*17D
 			DwtNmenuHistory, toggle_vector[a12_point]);
*17E
*17I
 			XmNmenuHistory, toggle_vector[a12_point]);
*17E
 		if (strcmp(banner_font_size, "14") == 0)
 		    BannerSetArg(toggle_vector[banner_font_size_c], 
*17D
 			DwtNmenuHistory, toggle_vector[a14_point]);
*17E
*17I
 			XmNmenuHistory, toggle_vector[a14_point]);
*17E
 		if (strcmp(banner_font_size, "18") == 0)
 		    BannerSetArg(toggle_vector[banner_font_size_c], 
*17D
 			DwtNmenuHistory, toggle_vector[a18_point]);
*17E
*17I
 			XmNmenuHistory, toggle_vector[a18_point]);
*17E
 		if (strcmp(banner_font_size, "24") == 0)
 		    BannerSetArg(toggle_vector[banner_font_size_c], 
*17D
 			DwtNmenuHistory, toggle_vector[a24_point]);
*17E
*17I
 			XmNmenuHistory, toggle_vector[a24_point]);
*17E
 		state=0;
 		break;
 		}
 	     case avantgarde_font :
 		{
 		toggle_vector[avantgarde_font] = widget;
 		state=0;
 		break;
 		}
 	     case courier_font:
 		{
 		toggle_vector[courier_font] = widget;
 		state=0;
 		break;
 		}
 	     case helvetica_font:
 		{
 		toggle_vector[helvetica_font] = widget;
 		state=0;
 		break;
 		}
 	     case lubalingraph_font:
 		{
 		toggle_vector[lubalingraph_font] = widget;
 		state=0;
 		break;
 		}
 	     case ncsschoolbook_font:
 		{
 		toggle_vector[ncsschoolbook_font] = widget;
 		state=0;
 		break;
 		}
 	     case souvenir_font:
 		{
 		toggle_vector[souvenir_font] = widget;
 		state=0;
 		break;
 		}
 	     case times_font:
 		{
 		toggle_vector[times_font] = widget;
 		state=0;
 		break;
 		}
 	     case a8_point:
 		{
 		toggle_vector[a8_point] = widget;
 		state=0;
 		break;
 		}
 	     case a10_point:
 		{
 		toggle_vector[a10_point] = widget;
 		state=0;
 		break;
 		}
 	     case a12_point:
 		{
 		toggle_vector[a12_point] = widget;
 		state=0;
 		break;
 		}
 	     case a14_point:
 		{
 		toggle_vector[a14_point] = widget;
 		state=0;
 		break;
 		}
 	     case a18_point:
 		{
 		toggle_vector[a18_point] = widget;
 		state=0;
 		break;
 		}
 	     case a24_point:
 		{
 		toggle_vector[a24_point] = widget;
 		state=0;
 		break;
 		}
 	     case banner_message_create :
 		{
 		Bnr.message_text_widget = widget;
*17D
 		BannerSetArg (widget, DwtNforeground, Bnr.foreground);
 		BannerSetArg (widget, DwtNbackground, Bnr.background);
*17E
*17I
 		BannerSetArg (widget, XmNforeground, Bnr.foreground);
 		BannerSetArg (widget, XmNbackground, Bnr.background);
*17E
 		state=0;
 		break;
 		}
*19I
 	     case banner_print_queue_name :
 		{
 		if (!New_Bnr.Param_Bits.qp_window)
 		    BannerSetArg(widget, 
 			XmNsensitive,
 			0);
 		toggle_vector[banner_print_queue_name] = widget;
 		XmTextSetString (widget, Bnr.qp_name);
 		state=0;
 		break;
 		}
 	     case banner_batch_queue_name :
 		{
 		if (!New_Bnr.Param_Bits.qb_window)
 		    BannerSetArg(widget, 
 			XmNsensitive,
 			0);
 		toggle_vector[banner_batch_queue_name] = widget;
 		XmTextSetString (widget, Bnr.qb_name);
 		state=0;
 		break;
 		}
*22I
 	     case banner_batch_queue :
 		{
 		state = New_Bnr.Param_Bits.qb_window;
 		toggle_vector[banner_batch_queue] = widget;
 		break;
 		}
 	     case banner_print_queue :
 		{
 		state = New_Bnr.Param_Bits.qp_window;
 		toggle_vector[banner_print_queue] = widget;
 		break;
 		}
*22E
 	     case banner_queue_user :
 		{
 		if (!New_Bnr.Param_Bits.qp_window && !New_Bnr.Param_Bits.qb_window)
 		    BannerSetArg(widget, 
 			XmNsensitive,
 			0);
 		toggle_vector[banner_queue_user] = widget;
 		XmTextSetString (widget, Bnr.queue_user);
 		state=0;
 		break;
 		}
*19E
 	     case banner_disk_0 :
 		{
*4I
*9D
 		if (!New_Bnr.Param_Bits.monitor)
*9E
*9I
*35D
*34A1D
 		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
*34A1E
*34A1I
 		if (!New_Bnr.Param_Bits.sys_window)
*34A1E
*35E
*35I
*36D
 		if (!New_Bnr.Param_Bits.sys_window)
*36E
*36I
*37I
*38D
 **************** Conflict 25    ************************************************
*38E
*37E
*38D
 		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
*38E
*37I
*38D
 ********************************************************************************
 		if (!New_Bnr.Param_Bits.sys_window)
 ************** End of Conflict 25    *******************************************
*38E
*38I
 		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
*38E
*37E
*36E
*35E
*9E
 		    BannerSetArg(widget, 
*17D
 			DwtNsensitive,
*17E
*17I
 			XmNsensitive,
*17E
 			0);
*4E
 		toggle_vector[banner_disk_0] = widget;
*17D
 		DwtSTextSetString (widget, Bnr.disk_0);
*17E
*17I
 		XmTextSetString (widget, Bnr.disk_0);
*17E
 		state=0;
 		break;
 		}
 	     case banner_disk_0_name :
 		{
*4I
*9D
 		if (!New_Bnr.Param_Bits.monitor)
*9E
*9I
*35D
*34A1D
 		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
*34A1E
*34A1I
 		if (!New_Bnr.Param_Bits.sys_window)
*34A1E
*35E
*35I
*36D
 		if (!New_Bnr.Param_Bits.sys_window)
*36E
*36I
*37I
*38D
 **************** Conflict 26    ************************************************
*38E
*37E
*38D
 		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
*38E
*37I
*38D
 ********************************************************************************
 		if (!New_Bnr.Param_Bits.sys_window)
 ************** End of Conflict 26    *******************************************
*38E
*38I
 		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
*38E
*37E
*36E
*35E
*9E
 		    BannerSetArg(widget, 
*17D
 			DwtNsensitive,
*17E
*17I
 			XmNsensitive,
*17E
 			0);
*4E
 		toggle_vector[banner_disk_0_name] = widget;
*17D
 		DwtSTextSetString (widget, Bnr.disk_0_name);
*17E
*17I
 		XmTextSetString (widget, Bnr.disk_0_name);
*17E
 		state=0;
 		break;
 		}
 	     case banner_disk_1 :
 		{
*4I
*9D
 		if (!New_Bnr.Param_Bits.monitor)
*9E
*9I
*35D
*34A1D
 		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
*34A1E
*34A1I
 		if (!New_Bnr.Param_Bits.sys_window)
*34A1E
*35E
*35I
*36D
 		if (!New_Bnr.Param_Bits.sys_window)
*36E
*36I
*37I
*38D
 **************** Conflict 27    ************************************************
*38E
*37E
*38D
 		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
*38E
*37I
*38D
 ********************************************************************************
 		if (!New_Bnr.Param_Bits.sys_window)
 ************** End of Conflict 27    *******************************************
*38E
*38I
 		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
*38E
*37E
*36E
*35E
*9E
 		    BannerSetArg(widget, 
*17D
 			DwtNsensitive,
*17E
*17I
 			XmNsensitive,
*17E
 			0);
*4E
 		toggle_vector[banner_disk_1] = widget;
*17D
 		DwtSTextSetString (widget, Bnr.disk_1);
*17E
*17I
 		XmTextSetString (widget, Bnr.disk_1);
*17E
 		state=0;
 		break;
 		}
 	     case banner_disk_1_name :
 		{
*4I
*9D
 		if (!New_Bnr.Param_Bits.monitor)
*9E
*9I
*35D
*34A1D
 		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
*34A1E
*34A1I
 		if (!New_Bnr.Param_Bits.sys_window)
*34A1E
*35E
*35I
*36D
 		if (!New_Bnr.Param_Bits.sys_window)
*36E
*36I
*37I
*38D
 **************** Conflict 28    ************************************************
*38E
*37E
*38D
 		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
*38E
*37I
*38D
 ********************************************************************************
 		if (!New_Bnr.Param_Bits.sys_window)
 ************** End of Conflict 28    *******************************************
*38E
*38I
 		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
*38E
*37E
*36E
*35E
*9E
 		    BannerSetArg(widget, 
*17D
 			DwtNsensitive,
*17E
*17I
 			XmNsensitive,
*17E
 			0);
*4E
 		toggle_vector[banner_disk_1_name] = widget;
*17D
 		DwtSTextSetString (widget, Bnr.disk_1_name);
*17E
*17I
 		XmTextSetString (widget, Bnr.disk_1_name);
*17E
 		state=0;
 		break;
 		}
 	     case banner_disk_2 :
 		{
*4I
*9D
 		if (!New_Bnr.Param_Bits.monitor)
*9E
*9I
*35D
*34A1D
 		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
*34A1E
*34A1I
 		if (!New_Bnr.Param_Bits.sys_window)
*34A1E
*35E
*35I
*36D
 		if (!New_Bnr.Param_Bits.sys_window)
*36E
*36I
*37I
*38D
 **************** Conflict 29    ************************************************
*38E
*37E
*38D
 		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
*38E
*37I
*38D
 ********************************************************************************
 		if (!New_Bnr.Param_Bits.sys_window)
 ************** End of Conflict 29    *******************************************
*38E
*38I
 		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
*38E
*37E
*36E
*35E
*9E
 		    BannerSetArg(widget, 
*17D
 			DwtNsensitive,
*17E
*17I
 			XmNsensitive,
*17E
 			0);
*4E
 		toggle_vector[banner_disk_2] = widget;
*17D
 		DwtSTextSetString (widget, Bnr.disk_2);
*17E
*17I
 		XmTextSetString (widget, Bnr.disk_2);
*17E
 		state=0;
 		break;
 		}
 	     case banner_disk_2_name :
 		{
*4I
*9D
 		if (!New_Bnr.Param_Bits.monitor)
*9E
*9I
*35D
*34A1D
 		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
*34A1E
*34A1I
 		if (!New_Bnr.Param_Bits.sys_window)
*34A1E
*35E
*35I
*36D
 		if (!New_Bnr.Param_Bits.sys_window)
*36E
*36I
*37I
*38D
 **************** Conflict 30    ************************************************
*38E
*37E
*38D
 		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
*38E
*37I
*38D
 ********************************************************************************
 		if (!New_Bnr.Param_Bits.sys_window)
 ************** End of Conflict 30    *******************************************
*38E
*38I
 		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
*38E
*37E
*36E
*35E
*9E
 		    BannerSetArg(widget, 
*17D
 			DwtNsensitive,
*17E
*17I
 			XmNsensitive,
*17E
 			0);
*4E
 		toggle_vector[banner_disk_2_name] = widget;
*17D
 		DwtSTextSetString (widget, Bnr.disk_2_name);
*17E
*17I
 		XmTextSetString (widget, Bnr.disk_2_name);
*17E
 		state=0;
 		break;
 		}
 	     case banner_hour_widget :
 		{
 		toggle_vector[banner_hour_widget] = widget;
 		state=12;
*17I
 		valuetoset = XmNvalue;
*17E
 		break;
 		}
 	     case banner_minute_widget :
 		{
 		toggle_vector[banner_minute_widget] = widget;
 		state=0;
*17I
 		valuetoset = XmNvalue;
*17E
 		break;
 		}
 	     case banner_alarmtext_widget :
 		{
 		toggle_vector[banner_alarmtext_widget] = widget;
 		state="Alarm Message";
*17I
 		valuetoset = XmNvalue;
*17E
 		break;
 		}
 	     case banner_tick_size :
 		{
 		toggle_vector[banner_tick_size] = widget;
 		state = New_Bnr.cpu_update;
*17I
 		valuetoset = XmNvalue;
*17E
 		break;
 		}
 	     case banner_base_priority :
 		{
 		toggle_vector[banner_base_priority] = widget;
 		state = New_Bnr.base_priority;
*17I
 		valuetoset = XmNvalue;
*17E
 		break;
 		}
 	     case banner_border_active :
 		{
 		toggle_vector[banner_border_active] = widget;
 		state = New_Bnr.Param_Bits.border;
 		break;
 		}
 	     case banner_window_border_active :
 		{
 		toggle_vector[banner_window_border_active] = widget;
 		state = New_Bnr.Param_Bits.window_border;
 		break;
 		}
 	     case banner_icon_active :
 		{
 		toggle_vector[banner_icon_active] = widget;
 		state = New_Bnr.Param_Bits.icon_on;
 		BannerSetArg (toggle_vector[banner_icon_active],
*17D
 		    DwtNsensitive,
*17E
*17I
 		    XmNsensitive,
*17E
 		    New_Bnr.Param_Bits.border);
 		break;
 		}
 	     case banner_bell_on :
 		{
 		toggle_vector[banner_bell_on] = widget;
 		state = New_Bnr.Param_Bits.bell;
 		break;
 		}
 	     case banner_seconds_active :
 		{
 		toggle_vector[banner_seconds_active] = widget;
 		state = New_Bnr.Param_Bits.seconds;
 		BannerSetArg (toggle_vector[banner_seconds_active], 
*17D
 		    DwtNsensitive,
*17E
*17I
 		    XmNsensitive,
*17E
 		    New_Bnr.Param_Bits.digital_clock ||
 		    New_Bnr.Param_Bits.analog_clock);
 		break;
 		}
*3I
 	     case banner_freeiocnt :
 		{
*4I
*9D
 		if (!New_Bnr.Param_Bits.monitor)
*9E
*9I
*35D
*34A1D
 		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
*34A1E
*34A1I
 		if (!New_Bnr.Param_Bits.sys_window)
*34A1E
*35E
*35I
*36D
 		if (!New_Bnr.Param_Bits.sys_window)
*36E
*36I
*37I
*38D
 **************** Conflict 31    ************************************************
*38E
*37E
*38D
 		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
*38E
*37I
*38D
 ********************************************************************************
 		if (!New_Bnr.Param_Bits.sys_window)
 ************** End of Conflict 31    *******************************************
*38E
*38I
 		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
*38E
*37E
*36E
*35E
*9E
 		    BannerSetArg(widget, 
*17D
 			DwtNsensitive,
*17E
*17I
 			XmNsensitive,
*17E
 			0);
*4E
 		toggle_vector[banner_freeiocnt] = widget;
 		state = New_Bnr.Param_Bits.freeiocnt;
 		break;
 		}
 	     case banner_freeblkcnt :
 		{
*4I
*9D
 		if (!New_Bnr.Param_Bits.monitor)
*9E
*9I
*35D
*34A1D
 		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
*34A1E
*34A1I
 		if (!New_Bnr.Param_Bits.sys_window)
*34A1E
*35E
*35I
*36D
 		if (!New_Bnr.Param_Bits.sys_window)
*36E
*36I
*37I
*38D
 **************** Conflict 32    ************************************************
*38E
*37E
*38D
 		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
*38E
*37I
*38D
 ********************************************************************************
 		if (!New_Bnr.Param_Bits.sys_window)
 ************** End of Conflict 32    *******************************************
*38E
*38I
 		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
*38E
*37E
*36E
*35E
*9E
 		    BannerSetArg(widget, 
*17D
 			DwtNsensitive,
*17E
*17I
 			XmNsensitive,
*17E
 			0);
*4E
 		toggle_vector[banner_freeblkcnt] = widget;
 		state = New_Bnr.Param_Bits.freeblkcnt;
 		break;
 		}
*3E
 	     case banner_box_toggle :
 		{
 		toggle_vector[banner_box_toggle] = widget;
 		state = New_Bnr.Param_Bits.cube;
 		break;
 		}
 	     case banner_world_toggle :
 		{
 		toggle_vector[banner_world_toggle] = widget;
 		state = New_Bnr.Param_Bits.world;
 		break;
 		}
 	     case banner_hanoi_toggle :
 		{
 		toggle_vector[banner_hanoi_toggle] = widget;
 		state = New_Bnr.Param_Bits.hanoi;
 		break;
 		}
*15D
 	     case banner_neither_toggle :
 		{
 		toggle_vector[banner_world_toggle] = widget;
 		if (!New_Bnr.Param_Bits.cube &&
 		    !New_Bnr.Param_Bits.world &&
 		    !New_Bnr.Param_Bits.hanoi)
 		    state = 1;
 		else
 		    state = 0;		
 		break;
 		}
*15E
 	     case banner_cpu_window_toggle :
 		{
 		toggle_vector[banner_cpu_window_toggle] = widget;
 		state = New_Bnr.Param_Bits.cpu_window;
 		break;
 		}
 	     case banner_modes_toggle :
 		{
 		toggle_vector[banner_modes_toggle] = widget;
 		state = New_Bnr.Param_Bits.modes_window;
 		if (New_Bnr.Param_Bits.cpu_window == 0)
*17D
 		    BannerSetArg (widget, DwtNsensitive, 0);
*17E
*17I
 		    BannerSetArg (widget, XmNsensitive, 0);
*17E
 		break;
 		}
 	     case banner_multi_hist_toggle :
 		{
 		toggle_vector[banner_multi_hist_toggle] = widget;
 		state = New_Bnr.Param_Bits.cpu_multi_hist;
 		if (New_Bnr.Param_Bits.cpu_window == 0)
*17D
 		    BannerSetArg (widget, DwtNsensitive, 0);
*17E
*17I
 		    BannerSetArg (widget, XmNsensitive, 0);
*17E
 		break;
 		}
 	     case banner_message_toggle :
 		{
 		toggle_vector[banner_message_toggle] = widget;
 		state = New_Bnr.Param_Bits.message_window;
 		break;
 		}
*35D
*34A1D
 	     case banner_mon_window_toggle :
*34A1E
*34A1I
 	     case banner_link_window_toggle :
*34A1E
*35E
*35I
*36D
 	     case banner_link_window_toggle :
*36E
*36I
*37I
*38D
 **************** Conflict 33    ************************************************
*38E
*37E
 	     case banner_mon_window_toggle :
*36E
*35E
*37D
 		{
*37E
*37I
*38D
 ********************************************************************************
*38E
*38I
 		{
 		toggle_vector[banner_mon_window_toggle] = widget;
 		state = New_Bnr.Param_Bits.monitor;
 		break;
 		}
*38E
 	     case banner_link_window_toggle :
*38D
 ************** End of Conflict 33    *******************************************
 		{
 **************** Conflict 34    ************************************************
*38E
*37E
*35D
*34A1D
 		toggle_vector[banner_mon_window_toggle] = widget;
 		state = New_Bnr.Param_Bits.monitor;
*34A1E
*34A1I
 		toggle_vector[banner_link_window_toggle] = widget;
 		state = New_Bnr.Param_Bits.link_window;
*34A1E
*35E
*35I
*36D
 		toggle_vector[banner_link_window_toggle] = widget;
 		state = New_Bnr.Param_Bits.link_window;
*36E
*36I
*38D
 		toggle_vector[banner_mon_window_toggle] = widget;
 		state = New_Bnr.Param_Bits.monitor;
*38E
*36E
*35E
*37I
*38D
 ********************************************************************************
*38E
*38I
 		{
*38E
 		toggle_vector[banner_link_window_toggle] = widget;
 		state = New_Bnr.Param_Bits.link_window;
*38D
 ************** End of Conflict 34    *******************************************
*38E
*37E
 		break;
 		}
*9I
 	     case banner_sys_window_toggle :
 		{
 		toggle_vector[banner_sys_window_toggle] = widget;
 		state = New_Bnr.Param_Bits.sys_window;
 		break;
 		}
*9E
*19I
 	     case banner_cluster :
 		{
 		toggle_vector[banner_cluster] = widget;
 		state = New_Bnr.Param_Bits.clu_window;
 		break;
 		}
 
 	     case banner_tp_tcomprate:
 		{
 		toggle_vector[banner_tp_tcomprate] = widget;
 		state = New_Bnr.Param_Bits.total_com_rate;
*30I
 		if (New_Bnr.Param_Bits.tp_window == 0)
 		    BannerSetArg (widget, XmNsensitive, 0);
*30E
 		break;
 		}
 	     case banner_tp_1fcomprate :
 		{
 		toggle_vector[banner_tp_1fcomprate] = widget;
 		state = New_Bnr.Param_Bits.onef_comp_rate;
*30I
 		if (New_Bnr.Param_Bits.tp_window == 0)
 		    BannerSetArg (widget, XmNsensitive, 0);
*30E
 		break;
 		}
 	     case banner_tp_startcomprate :
 		{
 		toggle_vector[banner_tp_startcomprate] = widget;
 		state = New_Bnr.Param_Bits.start_rate;
*30I
 		if (New_Bnr.Param_Bits.tp_window == 0)
 		    BannerSetArg (widget, XmNsensitive, 0);
*30E
 		break;
 		}
 	     case banner_tp_prepcomprate :
 		{
 		toggle_vector[banner_tp_prepcomprate] = widget;
 		state = New_Bnr.Param_Bits.prep_rate;
*30I
 		if (New_Bnr.Param_Bits.tp_window == 0)
 		    BannerSetArg (widget, XmNsensitive, 0);
*30E
 		break;
 		}
 	     case banner_tp_abortcomprate :
 		{
 		toggle_vector[banner_tp_abortcomprate] = widget;
 		state = New_Bnr.Param_Bits.abort_rate;
*30I
 		if (New_Bnr.Param_Bits.tp_window == 0)
 		    BannerSetArg (widget, XmNsensitive, 0);
*30E
 		break;
 		}
 	     case banner_tp_endcomprate :
 		{
 		toggle_vector[banner_tp_endcomprate] = widget;
 		state = New_Bnr.Param_Bits.end_rate;
*30I
 		if (New_Bnr.Param_Bits.tp_window == 0)
 		    BannerSetArg (widget, XmNsensitive, 0);
*30E
 		break;
 		}
 	     case banner_tp_remstartcomprate :
 		{
 		toggle_vector[banner_tp_remstartcomprate] = widget;
 		state = New_Bnr.Param_Bits.rem_start_rate;
*30I
 		if (New_Bnr.Param_Bits.tp_window == 0)
 		    BannerSetArg (widget, XmNsensitive, 0);
*30E
 		break;
 		}
 	     case banner_tp_remendcomprate :
 		{
 		toggle_vector[banner_tp_remendcomprate] = widget;
 		state = New_Bnr.Param_Bits.rem_end_rate;
*30I
 		if (New_Bnr.Param_Bits.tp_window == 0)
 		    BannerSetArg (widget, XmNsensitive, 0);
*30E
 		break;
 		}
 	     case banner_tp_comprate1 :
 		{
 		toggle_vector[banner_tp_comprate1] = widget;
 		state = New_Bnr.Param_Bits.comp_rate1;
*30I
 		if (New_Bnr.Param_Bits.tp_window == 0)
 		    BannerSetArg (widget, XmNsensitive, 0);
*30E
 		break;
 		}
 	     case banner_tp_comprate2 :
 		{
 		toggle_vector[banner_tp_comprate2] = widget;
 		state = New_Bnr.Param_Bits.comp_rate2;
*30I
 		if (New_Bnr.Param_Bits.tp_window == 0)
 		    BannerSetArg (widget, XmNsensitive, 0);
*30E
 		break;
 		}
 	     case banner_tp_comprate3 :
 		{
 		toggle_vector[banner_tp_comprate3] = widget;
 		state = New_Bnr.Param_Bits.comp_rate3;
*30I
 		if (New_Bnr.Param_Bits.tp_window == 0)
 		    BannerSetArg (widget, XmNsensitive, 0);
*30E
 		break;
 		}
 	     case banner_tp_comprate4 :
 		{
 		toggle_vector[banner_tp_comprate4] = widget;
 		state = New_Bnr.Param_Bits.comp_rate4;
*30I
 		if (New_Bnr.Param_Bits.tp_window == 0)
 		    BannerSetArg (widget, XmNsensitive, 0);
*30E
 		break;
 		}
 
 	     case banner_max_comp_rate :
 		{
 		char	text[20];
 		sprintf(text, "%d", Bnr.tp_max_rate);
 		XmTextSetString (widget,text);
 		toggle_vector[banner_max_comp_rate] = widget;
 		state = 0;
*30I
 		if (New_Bnr.Param_Bits.tp_window == 0)
 		    BannerSetArg (widget, XmNsensitive, 0);
*30E
 		break;
 		}
 
 	     case banner_comp_rate1_start :
 		{
 		char	text[20];
 		sprintf(text, "%d", Bnr.tp_rate1_start);
 		XmTextSetString (widget,text);
 		toggle_vector[banner_comp_rate1_start] = widget;
 		state = 0;
*30I
 		if (New_Bnr.Param_Bits.tp_window == 0)
 		    BannerSetArg (widget, XmNsensitive, 0);
*30E
 		break;
 		}
 
 	     case banner_comp_rate2_start :
 		{
 		char	text[20];
 		sprintf(text, "%d", Bnr.tp_rate2_start);
 		XmTextSetString (widget,text);
 		toggle_vector[banner_comp_rate2_start] = widget;
 		state = 0;
*30I
 		if (New_Bnr.Param_Bits.tp_window == 0)
 		    BannerSetArg (widget, XmNsensitive, 0);
*30E
 		break;
 		}
 
 	     case banner_comp_rate3_start :
 		{
 		char	text[20];
 		sprintf(text, "%d", Bnr.tp_rate3_start);
 		XmTextSetString (widget,text);
 		toggle_vector[banner_comp_rate3_start] = widget;
 		state = 0;
*30I
 		if (New_Bnr.Param_Bits.tp_window == 0)
 		    BannerSetArg (widget, XmNsensitive, 0);
*30E
 		break;
 		}
 
 	     case banner_comp_rate4_start :
 		{
 		char	text[20];
 		sprintf(text, "%d", Bnr.tp_rate4_start);
 		XmTextSetString (widget,text);
 		toggle_vector[banner_comp_rate4_start] = widget;
 		state = 0;
*30I
 		if (New_Bnr.Param_Bits.tp_window == 0)
 		    BannerSetArg (widget, XmNsensitive, 0);
*30E
 		break;
 		}
 
 	     case banner_comp_rate1_end :
 		{
 		char	text[20];
 		sprintf(text, "%d", Bnr.tp_rate1_end);
 		XmTextSetString (widget,text);
 		toggle_vector[banner_comp_rate1_end] = widget;
 		state = 0;
*30I
 		if (New_Bnr.Param_Bits.tp_window == 0)
 		    BannerSetArg (widget, XmNsensitive, 0);
*30E
 		break;
 		}
 
 	     case banner_comp_rate2_end :
 		{
 		char	text[20];
 		sprintf(text, "%d", Bnr.tp_rate2_end);
 		XmTextSetString (widget,text);
 		toggle_vector[banner_comp_rate2_end] = widget;
 		state = 0;
*30I
 		if (New_Bnr.Param_Bits.tp_window == 0)
 		    BannerSetArg (widget, XmNsensitive, 0);
*30E
 		break;
 		}
 
 	     case banner_comp_rate3_end :
 		{
 		char	text[20];
 		sprintf(text, "%d", Bnr.tp_rate3_end);
 		XmTextSetString (widget,text);
 		toggle_vector[banner_comp_rate3_end] = widget;
 		state = 0;
*30I
 		if (New_Bnr.Param_Bits.tp_window == 0)
 		    BannerSetArg (widget, XmNsensitive, 0);
*30E
 		break;
 		}
 
*30D
 
*30E
 	     case banner_tp :
 		{
 		toggle_vector[banner_tp] = widget;
 		state = New_Bnr.Param_Bits.tp_window;
 		break;
 		}
*19E
 	     case banner_process_states :
 		{
 		toggle_vector[banner_process_states] = widget;
 		state = New_Bnr.Param_Bits.process_states;
 		if (New_Bnr.Param_Bits.process_window == 0)
*17D
 		    BannerSetArg (widget, DwtNsensitive, 0);
*17E
*17I
 		    BannerSetArg (widget, XmNsensitive, 0);
*17E
 		break;
 		}
 	     case banner_process_name :
 		{
 		toggle_vector[banner_process_name] = widget;
 		state = New_Bnr.Param_Bits.process_name;
 		if (New_Bnr.Param_Bits.process_window == 0)
*17D
 		    BannerSetArg (widget, DwtNsensitive, 0);
*17E
*17I
 		    BannerSetArg (widget, XmNsensitive, 0);
*17E
 		break;
 		}
 	     case banner_process_user :
 		{
 		toggle_vector[banner_process_user] = widget;
 		state = New_Bnr.Param_Bits.process_user;
 		if (New_Bnr.Param_Bits.process_window == 0)
*17D
 		    BannerSetArg (widget, DwtNsensitive, 0);
*17E
*17I
 		    BannerSetArg (widget, XmNsensitive, 0);
*17E
 		break;
 		}
 	     case banner_process_pid :
 		{
 		toggle_vector[banner_process_pid] = widget;
 		state = New_Bnr.Param_Bits.process_pid;
 		if (New_Bnr.Param_Bits.process_window == 0)
*17D
 		    BannerSetArg (widget, DwtNsensitive, 0);
*17E
*17I
 		    BannerSetArg (widget, XmNsensitive, 0);
*17E
 		break;
 		}
 	     case banner_pe_window :
 		{
 		toggle_vector[banner_pe_window] = widget;
 		state = New_Bnr.Param_Bits.pe_window;
 		break;
 		}
 	     case banner_lck_window :
 		{
 		toggle_vector[banner_lck_window] = widget;
 		state = New_Bnr.Param_Bits.lck_window;
 		break;
 		}
 	     case banner_cwps_window :
 		{
 		toggle_vector[banner_cwps_window] = widget;
 		state = New_Bnr.Param_Bits.cwps_window;
 		break;
 		}
 	     case banner_process_window_toggle :
 		{
 		toggle_vector[banner_process_window_toggle] = widget;
 		state = New_Bnr.Param_Bits.process_window;
 		break;
 		}
 	     case banner_analog_window_toggle :
 		{
 		toggle_vector[banner_analog_window_toggle] = widget;
 		state = New_Bnr.Param_Bits.analog_clock;		
 		break;
 		}
 	     case banner_digital_window_toggle :
 		{
 		toggle_vector[banner_digital_window_toggle] = widget;
 		state = New_Bnr.Param_Bits.digital_clock;		
 		break;
 		}
*19I
 
 	     case banner_show_all_users :
 		{
 		if (!New_Bnr.Param_Bits.qp_window && !New_Bnr.Param_Bits.qb_window)
 		    BannerSetArg(widget, 
 			XmNsensitive,
 			0);
 		toggle_vector[banner_show_all_users] = widget;
 		state = New_Bnr.Param_Bits.all_users;
 		break;
 		}
 
 	     case banner_show_hold :
 		{
 		if (!New_Bnr.Param_Bits.qp_window && !New_Bnr.Param_Bits.qb_window)
 		    BannerSetArg(widget, 
 			XmNsensitive,
 			0);
 		toggle_vector[banner_show_hold] = widget;
 		state = New_Bnr.Param_Bits.jobs_on_hold;
 		break;
 		}
 
 	     case banner_show_stopped :
 		{
 		if (!New_Bnr.Param_Bits.qp_window && !New_Bnr.Param_Bits.qb_window)
 		    BannerSetArg(widget, 
 			XmNsensitive,
 			0);
 		toggle_vector[banner_show_stopped] = widget;
 		state = New_Bnr.Param_Bits.stopped_queues;
 		break;
 		}
 
 	     case banner_show_int :
 		{
 		if (!New_Bnr.Param_Bits.qp_window && !New_Bnr.Param_Bits.qb_window)
 		    BannerSetArg(widget, 
 			XmNsensitive,
 			0);
 		toggle_vector[banner_show_int] = widget;
 		state = New_Bnr.Param_Bits.int_jobs;
 		break;
 		}
 
*19E
 	     case banner_dua0_active :
 		{
*9D
 		if (!New_Bnr.Param_Bits.monitor)
*9E
*9I
*35D
*34A1D
 		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
*34A1E
*34A1I
 		if (!New_Bnr.Param_Bits.sys_window)
*34A1E
*35E
*35I
*36D
 		if (!New_Bnr.Param_Bits.sys_window)
*36E
*36I
*37I
*38D
 **************** Conflict 35    ************************************************
*38E
*37E
*38D
 		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
*38E
*37I
*38D
 ********************************************************************************
 		if (!New_Bnr.Param_Bits.sys_window)
 ************** End of Conflict 35    *******************************************
*38E
*38I
 		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
*38E
*37E
*36E
*35E
*9E
 		    BannerSetArg(widget, 
*17D
 			DwtNsensitive,
*17E
*17I
 			XmNsensitive,
*17E
 			0);
 		toggle_vector[banner_dua0_active] = widget;
 		state = New_Mon.dua0;
 		break;
 		}
 	     case banner_dua1_active :
 		{
*9D
 		if (!New_Bnr.Param_Bits.monitor)
*9E
*9I
*35D
*34A1D
 		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
*34A1E
*34A1I
 		if (!New_Bnr.Param_Bits.sys_window)
*34A1E
*35E
*35I
*36D
 		if (!New_Bnr.Param_Bits.sys_window)
*36E
*36I
*37I
*38D
 **************** Conflict 36    ************************************************
*38E
*37E
*38D
 		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
*38E
*37I
*38D
 ********************************************************************************
 		if (!New_Bnr.Param_Bits.sys_window)
 ************** End of Conflict 36    *******************************************
*38E
*38I
 		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
*38E
*37E
*36E
*35E
*9E
 		    BannerSetArg(widget, 
*17D
 			DwtNsensitive,
*17E
*17I
 			XmNsensitive,
*17E
 			0);
 		toggle_vector[banner_dua1_active] = widget;
 		state = New_Mon.dua1;
 		break;
 		}
 	     case banner_dua2_active :
 		{
*9D
 		if (!New_Bnr.Param_Bits.monitor)
*9E
*9I
*35D
*34A1D
 		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
*34A1E
*34A1I
 		if (!New_Bnr.Param_Bits.sys_window)
*34A1E
*35E
*35I
*36D
 		if (!New_Bnr.Param_Bits.sys_window)
*36E
*36I
*37I
*38D
 **************** Conflict 37    ************************************************
*38E
*37E
*38D
 		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
*38E
*37I
*38D
 ********************************************************************************
 		if (!New_Bnr.Param_Bits.sys_window)
 ************** End of Conflict 37    *******************************************
*38E
*38I
 		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
*38E
*37E
*36E
*35E
*9E
 		    BannerSetArg(widget, 
*17D
 			DwtNsensitive,
*17E
*17I
 			XmNsensitive,
*17E
 			0);
 		toggle_vector[banner_dua2_active] = widget;
 		state = New_Mon.dua2;
 		break;
 		}
 	     case banner_page_io_active :
 		{
*9D
 		if (!New_Bnr.Param_Bits.monitor)
*9E
*9I
*35D
*34A1D
 		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
*34A1E
*34A1I
 		if (!New_Bnr.Param_Bits.sys_window)
*34A1E
*35E
*35I
*36D
 		if (!New_Bnr.Param_Bits.sys_window)
*36E
*36I
*37I
*38D
 **************** Conflict 38    ************************************************
*38E
*37E
*38D
 		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
*38E
*37I
*38D
 ********************************************************************************
 		if (!New_Bnr.Param_Bits.sys_window)
 ************** End of Conflict 38    *******************************************
*38E
*38I
 		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
*38E
*37E
*36E
*35E
*9E
 		    BannerSetArg(widget, 
*17D
 			DwtNsensitive,
*17E
*17I
 			XmNsensitive,
*17E
 			0);
 		toggle_vector[banner_page_io_active] = widget;
 		state = New_Mon.pio;
 		break;
 		}
 	     case banner_page_fault_active :
 		{
*9D
 		if (!New_Bnr.Param_Bits.monitor)
*9E
*9I
*35D
*34A1D
 		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
*34A1E
*34A1I
 		if (!New_Bnr.Param_Bits.sys_window)
*34A1E
*35E
*35I
*36D
 		if (!New_Bnr.Param_Bits.sys_window)
*36E
*36I
*37I
*38D
 **************** Conflict 39    ************************************************
*38E
*37E
*38D
 		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
*38E
*37I
*38D
 ********************************************************************************
 		if (!New_Bnr.Param_Bits.sys_window)
 ************** End of Conflict 39    *******************************************
*38E
*38I
 		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
*38E
*37E
*36E
*35E
*9E
 		    BannerSetArg(widget, 
*17D
 			DwtNsensitive,
*17E
*17I
 			XmNsensitive,
*17E
 			0);
 		toggle_vector[banner_page_fault_active] = widget;
 		state = New_Mon.pf;
 		break;
 		}
 	     case banner_sysfaults_active :
 		{
*9D
 		if (!New_Bnr.Param_Bits.monitor)
*9E
*9I
*35D
*34A1D
 		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
*34A1E
*34A1I
 		if (!New_Bnr.Param_Bits.sys_window)
*34A1E
*35E
*35I
*36D
 		if (!New_Bnr.Param_Bits.sys_window)
*36E
*36I
*37I
*38D
 **************** Conflict 40    ************************************************
*38E
*37E
*38D
 		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
*38E
*37I
*38D
 ********************************************************************************
 		if (!New_Bnr.Param_Bits.sys_window)
 ************** End of Conflict 40    *******************************************
*38E
*38I
 		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
*38E
*37E
*36E
*35E
*9E
 		    BannerSetArg(widget, 
*17D
 			DwtNsensitive,
*17E
*17I
 			XmNsensitive,
*17E
 			0);
 		toggle_vector[banner_sysfaults_active] = widget;
 		state = New_Mon.sysflt;
 		break;
 		}
 	     case banner_ni_io_active :
 		{
*9D
 		if (!New_Bnr.Param_Bits.monitor)
*9E
*9I
*35D
*34A1D
 		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
*34A1E
*34A1I
 		if (!New_Bnr.Param_Bits.sys_window)
*34A1E
*35E
*35I
*36D
 		if (!New_Bnr.Param_Bits.sys_window)
*36E
*36I
*37I
*38D
 **************** Conflict 41    ************************************************
*38E
*37E
*38D
 		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
*38E
*37I
*38D
 ********************************************************************************
 		if (!New_Bnr.Param_Bits.sys_window)
 ************** End of Conflict 41    *******************************************
*38E
*38I
 		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
*38E
*37E
*36E
*35E
*9E
 		    BannerSetArg(widget, 
*17D
 			DwtNsensitive,
*17E
*17I
 			XmNsensitive,
*17E
 			0);
 		toggle_vector[banner_ni_io_active] = widget;
 		state = New_Mon.ni_io;
 		break;
 		}
 	     case banner_modified_pages_active :
 		{
*9D
 		if (!New_Bnr.Param_Bits.monitor)
*9E
*9I
*35D
*34A1D
 		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
*34A1E
*34A1I
 		if (!New_Bnr.Param_Bits.sys_window)
*34A1E
*35E
*35I
*36D
 		if (!New_Bnr.Param_Bits.sys_window)
*36E
*36I
*37I
*38D
 **************** Conflict 42    ************************************************
*38E
*37E
*38D
 		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
*38E
*37I
*38D
 ********************************************************************************
 		if (!New_Bnr.Param_Bits.sys_window)
 ************** End of Conflict 42    *******************************************
*38E
*38I
 		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
*38E
*37E
*36E
*35E
*9E
 		    BannerSetArg(widget, 
*17D
 			DwtNsensitive,
*17E
*17I
 			XmNsensitive,
*17E
 			0);
 		toggle_vector[banner_modified_pages_active] = widget;
 		state = New_Mon.mp;
 		break;
 		}
 	     case banner_free_pages_active :
 		{
*9D
 		if (!New_Bnr.Param_Bits.monitor)
*9E
*9I
*35D
*34A1D
 		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
*34A1E
*34A1I
 		if (!New_Bnr.Param_Bits.sys_window)
*34A1E
*35E
*35I
*36D
 		if (!New_Bnr.Param_Bits.sys_window)
*36E
*36I
*37I
*38D
 **************** Conflict 43    ************************************************
*38E
*37E
 		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
*36E
*35E
*9E
*37I
*38D
 ********************************************************************************
 		if (!New_Bnr.Param_Bits.sys_window)
 ************** End of Conflict 43    *******************************************
*38E
*37E
 		    BannerSetArg(widget, 
*17D
 			DwtNsensitive,
*17E
*17I
 			XmNsensitive,
*17E
 			0);
 		toggle_vector[banner_free_pages_active] = widget;
 		state = New_Mon.fp;
 		break;
 		}
     	     }
*17D
     BannerSetArg(widget, DwtNvalue, state);
*17E
*17I
     BannerSetArg(widget, valuetoset, state);
*17E
 }
 
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **	BannerToggleChangeCallback (widget, tag)
 **
 **      This rouitne is called each time one of the setup sub items changes
 **
 **  FORMAL PARAMETERS:
 **
 **      widget, tag
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
 BannerToggleChangeCallback(widget, tag)
 Widget widget;
 int *tag;
 {
 int state;
     switch (*tag)
 	     {
 	     case avantgarde_font :
 		{
 		strcpy(banner_font_name, "ITC Avant Garde Gothic");
 		strcpy (banner_font_wieght, "demi");
 		strcpy (banner_font_type, "P");
 		font_changed=1;
 		break;
 		}
 	     case courier_font:
 		{
 		strcpy(banner_font_name, "Courier");
 		strcpy (banner_font_wieght, "medium");
 		strcpy (banner_font_type, "M");
 		font_changed=1;
 		break;
 		}
 	     case helvetica_font:
 		{
 		strcpy(banner_font_name, "Helvetica");
 		strcpy (banner_font_wieght, "medium");
 		strcpy (banner_font_type, "P");
 		font_changed=1;
 		break;
 		}
 	     case lubalingraph_font:
 		{
 		strcpy(banner_font_name, "ITC Lubalin Graph");
 		strcpy (banner_font_wieght, "demi");
 		strcpy (banner_font_type, "P");
 		font_changed=1;
 		break;
 		}
 	     case ncsschoolbook_font:
 		{
 		strcpy(banner_font_name, "New Century Schoolbook");
 		strcpy (banner_font_wieght, "medium");
 		strcpy (banner_font_type, "P");
 		font_changed=1;
 		break;
 		}
 	     case souvenir_font:
 		{
 		strcpy(banner_font_name, "ITC Souvenir");
 		strcpy (banner_font_wieght, "demi");
 		strcpy (banner_font_type, "P");
 		font_changed=1;
 		break;
 		}
 	     case times_font:
 		{
 		strcpy(banner_font_name, "Times");
 		strcpy (banner_font_wieght, "medium");
 		strcpy (banner_font_type, "P");
 		font_changed=1;
 		break;
 		}
 	     case a8_point:
 		{
 		strcpy(banner_font_size, "8");
 		font_changed=1;
 		break;
 		}
 	     case a10_point:
 		{
 		strcpy(banner_font_size, "10");
 		font_changed=1;
 		break;
 		}
 	     case a12_point:
 		{
 		strcpy(banner_font_size, "12");
 		font_changed=1;
 		break;
 		}
 	     case a14_point:
 		{
 		strcpy(banner_font_size, "14");
 		font_changed=1;
 		break;
 		}
 	     case a18_point:
 		{
 		strcpy(banner_font_size, "18");
 		font_changed=1;
 		break;
 		}
 	     case a24_point:
 		{
 		strcpy(banner_font_size, "24");
 		font_changed=1;
 		break;
 		}
 	     case banner_tick_size :
 		{
*17D
 		BannerGetArg (widget, DwtNvalue, &state);
*17E
*17I
 		BannerGetArg (widget, XmNvalue, &state);
*17E
 		New_Bnr.cpu_update = state;
 		break;
 		}
 	     case banner_base_priority :
 		{
*17D
 		BannerGetArg (widget, DwtNvalue, &state);
*17E
*17I
 		BannerGetArg (widget, XmNvalue, &state);
*17E
 		New_Bnr.base_priority = state;
 		break;
 		}
 	     case banner_border_active :
 		{
 		New_Bnr.Param_Bits.border = !New_Bnr.Param_Bits.border;
*5I
 		BannerSetArg (toggle_vector[banner_icon_active],
*17D
 		    DwtNsensitive,
*17E
*17I
 		    XmNsensitive,
*17E
 		    New_Bnr.Param_Bits.border);
*5E
 		break;
 		}
 	     case banner_window_border_active :
 		{
 		New_Bnr.Param_Bits.window_border = !New_Bnr.Param_Bits.window_border;
 		break;
 		}
 	     case banner_icon_active :
 		{
 		BannerSetArg (toggle_vector[banner_icon_active],
*17D
 		    DwtNsensitive,
*17E
*17I
 		    XmNsensitive,
*17E
 		    New_Bnr.Param_Bits.border);
 		New_Bnr.Param_Bits.icon_on = !New_Bnr.Param_Bits.icon_on;
 		break;
 		}
 	     case banner_bell_on :
 		{
 		New_Bnr.Param_Bits.bell = !New_Bnr.Param_Bits.bell;
 		break;
 		}
 	     case banner_seconds_active :
 		{
 		New_Bnr.Param_Bits.seconds = !New_Bnr.Param_Bits.seconds;
 		break;
 		}
*3I
 	     case banner_freeiocnt :
 		{
 		New_Bnr.Param_Bits.freeiocnt = !New_Bnr.Param_Bits.freeiocnt;
 		break;
 		}
 	     case banner_freeblkcnt :
 		{
 		New_Bnr.Param_Bits.freeblkcnt = !New_Bnr.Param_Bits.freeblkcnt;
 		break;
 		}
*3E
 	     case banner_box_toggle :
 		{
 		New_Bnr.Param_Bits.cube = !New_Bnr.Param_Bits.cube;
 		break;
 		}
 	     case banner_world_toggle :
 		{
 		New_Bnr.Param_Bits.world = !New_Bnr.Param_Bits.world;
 		break;
 		}
 	     case banner_hanoi_toggle :
 		{
 		New_Bnr.Param_Bits.hanoi = !New_Bnr.Param_Bits.hanoi;
 		break;
 		}
*15D
 	     case banner_neither_toggle :
 		{
 		break;
 		}
*15E
 	     case banner_cpu_window_toggle :
 		{
 		New_Bnr.Param_Bits.cpu_window = !New_Bnr.Param_Bits.cpu_window;
 		BannerSetArg (toggle_vector[banner_modes_toggle],
*17D
 		    DwtNsensitive,
*17E
*17I
 		    XmNsensitive,
*17E
 		    New_Bnr.Param_Bits.cpu_window);
 		BannerSetArg (toggle_vector[banner_multi_hist_toggle],
*17D
 		    DwtNsensitive,
*17E
*17I
 		    XmNsensitive,
*17E
 		    New_Bnr.Param_Bits.cpu_window);
 		break;
 		}
 	     case banner_modes_toggle :
 		{
 		New_Bnr.Param_Bits.modes_window = !New_Bnr.Param_Bits.modes_window;
 		break;
 		}
 	     case banner_multi_hist_toggle :
 		{
 		New_Bnr.Param_Bits.cpu_multi_hist = 
 		    !New_Bnr.Param_Bits.cpu_multi_hist;
 		break;
 		}
 	     case banner_message_toggle :
 		{
 		New_Bnr.Param_Bits.message_window = !New_Bnr.Param_Bits.message_window;
 
 		BannerSetArg (toggle_vector[banner_bell_on],
*17D
 			DwtNsensitive,
*17E
*17I
 			XmNsensitive,
*17E
 		        New_Bnr.Param_Bits.message_window);
 
 		break;
 		}
 	     case banner_process_window_toggle :
 		{
 		New_Bnr.Param_Bits.process_window = !New_Bnr.Param_Bits.process_window;
 		BannerSetArg (toggle_vector[banner_process_states], 
*17D
 		    DwtNsensitive, New_Bnr.Param_Bits.process_window);
*17E
*17I
 		    XmNsensitive, New_Bnr.Param_Bits.process_window);
*17E
 		BannerSetArg (toggle_vector[banner_process_name], 
*17D
 		    DwtNsensitive, New_Bnr.Param_Bits.process_window);
*17E
*17I
 		    XmNsensitive, New_Bnr.Param_Bits.process_window);
*17E
 		BannerSetArg (toggle_vector[banner_process_user], 
*17D
 		    DwtNsensitive, New_Bnr.Param_Bits.process_window);
*17E
*17I
 		    XmNsensitive, New_Bnr.Param_Bits.process_window);
*17E
 		BannerSetArg (toggle_vector[banner_process_pid], 
*17D
 		    DwtNsensitive, New_Bnr.Param_Bits.process_window);
*17E
*17I
 		    XmNsensitive, New_Bnr.Param_Bits.process_window);
*17E
 		break;
 		}
 	     case banner_process_states :
 		{
 		New_Bnr.Param_Bits.process_states = 
 			!New_Bnr.Param_Bits.process_states;
 		break;
 		}
 	     case banner_process_name :
 		{
 		New_Bnr.Param_Bits.process_name = 
 			!New_Bnr.Param_Bits.process_name;
 		break;
 		}
 	     case banner_process_user :
 		{
 		New_Bnr.Param_Bits.process_user = 
 			!New_Bnr.Param_Bits.process_user;
 		break;
 		}
 	     case banner_process_pid :
 		{
 		New_Bnr.Param_Bits.process_pid = 
 			!New_Bnr.Param_Bits.process_pid;
 		break;
 		}
 	     case banner_pe_window :
 		{
 		New_Bnr.Param_Bits.pe_window = !New_Bnr.Param_Bits.pe_window;
 		break;
 		}
 	     case banner_lck_window :
 		{
 		New_Bnr.Param_Bits.lck_window = !New_Bnr.Param_Bits.lck_window;
 		break;
 		}
 	     case banner_cwps_window :
 		{
 		New_Bnr.Param_Bits.cwps_window = !New_Bnr.Param_Bits.cwps_window;
 		break;
 		}
*19I
 
 	     case banner_cluster :
 		{
 		New_Bnr.Param_Bits.clu_window = !New_Bnr.Param_Bits.clu_window;
 		break;
 		}
 
 	     case banner_tp_tcomprate :
 		{
 		New_Bnr.Param_Bits.total_com_rate = !New_Bnr.Param_Bits.total_com_rate;
 		break;
 		}
 	     case banner_tp_1fcomprate :
 		{
 		New_Bnr.Param_Bits.onef_comp_rate = !New_Bnr.Param_Bits.onef_comp_rate;
 		break;
 		}
 	     case banner_tp_startcomprate :
 		{
 		New_Bnr.Param_Bits.start_rate = !New_Bnr.Param_Bits.start_rate;
 		break;
 		}
 	     case banner_tp_prepcomprate :
 		{
 		New_Bnr.Param_Bits.prep_rate = !New_Bnr.Param_Bits.prep_rate;
 		break;
 		}
 	     case banner_tp_abortcomprate :
 		{
 		New_Bnr.Param_Bits.abort_rate = !New_Bnr.Param_Bits.abort_rate;
 		break;
 		}
 	     case banner_tp_endcomprate :
 		{
 		New_Bnr.Param_Bits.end_rate = !New_Bnr.Param_Bits.end_rate;
 		break;
 		}
 	     case banner_tp_remstartcomprate :
 		{
 		New_Bnr.Param_Bits.rem_start_rate = 
 		    !New_Bnr.Param_Bits.rem_start_rate;
 		break;
 		}
 	     case banner_tp_remendcomprate :
 		{
 		New_Bnr.Param_Bits.rem_end_rate = 
 		    !New_Bnr.Param_Bits.rem_end_rate;
 		break;
 		}
 	     case banner_tp_comprate1 :
 		{
 		New_Bnr.Param_Bits.comp_rate1 = !New_Bnr.Param_Bits.comp_rate1;
 		break;
 		}
 	     case banner_tp_comprate2 :
 		{
 		New_Bnr.Param_Bits.comp_rate2 = !New_Bnr.Param_Bits.comp_rate2;
 		break;
 		}
 	     case banner_tp_comprate3 :
 		{
 		New_Bnr.Param_Bits.comp_rate3 = !New_Bnr.Param_Bits.comp_rate3;
 		break;
 		}
 	     case banner_tp_comprate4 :
 		{
 		New_Bnr.Param_Bits.comp_rate4 = !New_Bnr.Param_Bits.comp_rate4;
 		break;
 		}
 
 	     case banner_tp :
 		{
 		int state;
 		state = New_Bnr.Param_Bits.tp_window = !New_Bnr.Param_Bits.tp_window;
 		
 		BannerSetArg (toggle_vector[banner_tp_tcomprate], 
 		    XmNsensitive,
 		    state);
 		BannerSetArg (toggle_vector[banner_tp_1fcomprate], 
 		    XmNsensitive,
 		    state);
 		BannerSetArg (toggle_vector[banner_tp_startcomprate], 
 		    XmNsensitive,
 		    state);
 		BannerSetArg (toggle_vector[banner_tp_prepcomprate], 
 		    XmNsensitive,
 		    state);
 		BannerSetArg (toggle_vector[banner_tp_abortcomprate], 
 		    XmNsensitive,
 		    state);
 		BannerSetArg (toggle_vector[banner_tp_endcomprate], 
 		    XmNsensitive,
 		    state);
 		BannerSetArg (toggle_vector[banner_tp_remstartcomprate], 
 		    XmNsensitive,
 		    state);
 		BannerSetArg (toggle_vector[banner_tp_remendcomprate], 
 		    XmNsensitive,
 		    state);
 		BannerSetArg (toggle_vector[banner_tp_comprate1], 
 		    XmNsensitive,
 		    state);
 		BannerSetArg (toggle_vector[banner_tp_comprate2], 
 		    XmNsensitive,
 		    state);
 		BannerSetArg (toggle_vector[banner_tp_comprate3], 
 		    XmNsensitive,
 		    state);
 		BannerSetArg (toggle_vector[banner_tp_comprate4], 
 		    XmNsensitive,
 		    state);
 
 		BannerSetArg (toggle_vector[banner_max_comp_rate], 
 		    XmNsensitive,
 		    state);
 		BannerSetArg (toggle_vector[banner_comp_rate1_start], 
 		    XmNsensitive,
 		    state);
 		BannerSetArg (toggle_vector[banner_comp_rate2_start], 
 		    XmNsensitive,
 		    state);
 		BannerSetArg (toggle_vector[banner_comp_rate3_start], 
 		    XmNsensitive,
 		    state);
 		BannerSetArg (toggle_vector[banner_comp_rate4_start], 
 		    XmNsensitive,
 		    state);
 		BannerSetArg (toggle_vector[banner_comp_rate1_end], 
 		    XmNsensitive,
 		    state);
 		BannerSetArg (toggle_vector[banner_comp_rate2_end], 
 		    XmNsensitive,
 		    state);
 		BannerSetArg (toggle_vector[banner_comp_rate3_end], 
 		    XmNsensitive,
 		    state);
 		break;
 		}
 
*19E
*35D
*34A1D
     	     case banner_mon_window_toggle :
*34A1E
*34A1I
     	     case banner_link_window_toggle :
 		{
 		New_Bnr.Param_Bits.link_window = !New_Bnr.Param_Bits.link_window;
 		break;
 		}
 
*34A1E
*35E
*35I
*36D
     	     case banner_link_window_toggle :
 		{
 		New_Bnr.Param_Bits.link_window = !New_Bnr.Param_Bits.link_window;
 		break;
 		}
 
*36E
*36I
*37I
*38D
 **************** Conflict 44    ************************************************
*38E
*37E
*40I
     	     case banner_link_window_toggle :
 		{
 		New_Bnr.Param_Bits.link_window = !New_Bnr.Param_Bits.link_window;
 		break;
 		}
 
*40E
     	     case banner_mon_window_toggle :
*36E
*35E
*9I
*37I
*38D
 ********************************************************************************
*38E
*38I
*40D
 		{
 		New_Bnr.Param_Bits.monitor = !New_Bnr.Param_Bits.monitor;
 		break;
 		}
 
*40E
*38E
*40D
     	     case banner_link_window_toggle :
 		{
 		New_Bnr.Param_Bits.link_window = !New_Bnr.Param_Bits.link_window;
 		break;
 		}
 
*40E
*38D
 ************** End of Conflict 44    *******************************************
*38E
*37E
     	     case banner_sys_window_toggle :
*9E
 		{
*9D
 		New_Bnr.Param_Bits.monitor = !New_Bnr.Param_Bits.monitor;
*9E
*9I
 		int state;
 
*35D
*34A1D
 		if (*tag == banner_mon_window_toggle)  
 		    New_Bnr.Param_Bits.monitor = !New_Bnr.Param_Bits.monitor;
 		else
 		    New_Bnr.Param_Bits.sys_window = !New_Bnr.Param_Bits.sys_window;
 		state = New_Bnr.Param_Bits.monitor | New_Bnr.Param_Bits.sys_window;
*34A1E
*34A1I
 		New_Bnr.Param_Bits.sys_window = !New_Bnr.Param_Bits.sys_window;
 		state = New_Bnr.Param_Bits.sys_window;
*34A1E
*35E
*35I
*36D
 		New_Bnr.Param_Bits.sys_window = !New_Bnr.Param_Bits.sys_window;
 		state = New_Bnr.Param_Bits.sys_window;
*36E
*36I
*37I
*38D
 **************** Conflict 45    ************************************************
*38E
*37E
 		if (*tag == banner_mon_window_toggle)  
 		    New_Bnr.Param_Bits.monitor = !New_Bnr.Param_Bits.monitor;
 		else
 		    New_Bnr.Param_Bits.sys_window = !New_Bnr.Param_Bits.sys_window;
 		state = New_Bnr.Param_Bits.monitor | New_Bnr.Param_Bits.sys_window;
*36E
*35E
*37I
*38D
 ********************************************************************************
 		New_Bnr.Param_Bits.sys_window = !New_Bnr.Param_Bits.sys_window;
 		state = New_Bnr.Param_Bits.sys_window;
 ************** End of Conflict 45    *******************************************
*38E
*37E
 
*9E
 		BannerSetArg (toggle_vector[banner_dua0_active], 
*17D
 		    DwtNsensitive,
*17E
*17I
 		    XmNsensitive,
*17E
*9D
 		    New_Bnr.Param_Bits.monitor);
*9E
*9I
 		    state);
*9E
 		BannerSetArg (toggle_vector[banner_dua1_active], 
*17D
 		    DwtNsensitive,
*17E
*17I
 		    XmNsensitive,
*17E
*9D
 		    New_Bnr.Param_Bits.monitor);
*9E
*9I
 		    state);
*9E
 		BannerSetArg (toggle_vector[banner_dua2_active], 
*17D
 		    DwtNsensitive,
*17E
*17I
 		    XmNsensitive,
*17E
*9D
 		    New_Bnr.Param_Bits.monitor);
*9E
*9I
 		    state);
*9E
 		BannerSetArg (toggle_vector[banner_page_io_active], 
*17D
 		    DwtNsensitive,
*17E
*17I
 		    XmNsensitive,
*17E
*9D
 		    New_Bnr.Param_Bits.monitor);
*9E
*9I
 		    state);
*9E
 		BannerSetArg (toggle_vector[banner_page_fault_active], 
*17D
 		    DwtNsensitive,
*17E
*17I
 		    XmNsensitive,
*17E
*9D
 		    New_Bnr.Param_Bits.monitor);
*9E
*9I
 		    state);
*9E
 		BannerSetArg (toggle_vector[banner_sysfaults_active], 
*17D
 		    DwtNsensitive,
*17E
*17I
 		    XmNsensitive,
*17E
*9D
 		    New_Bnr.Param_Bits.monitor);
*9E
*9I
 		    state);
*9E
 		BannerSetArg (toggle_vector[banner_ni_io_active], 
*17D
 		    DwtNsensitive,
*17E
*17I
 		    XmNsensitive,
*17E
*9D
 		    New_Bnr.Param_Bits.monitor);
*9E
*9I
 		    state);
*9E
 		BannerSetArg (toggle_vector[banner_modified_pages_active], 
*17D
 		    DwtNsensitive,
*17E
*17I
 		    XmNsensitive,
*17E
*9D
 		    New_Bnr.Param_Bits.monitor);
*9E
*9I
 		    state);
*9E
 		BannerSetArg (toggle_vector[banner_free_pages_active], 
*17D
 		    DwtNsensitive,
*17E
*17I
 		    XmNsensitive,
*17E
*9D
 		    New_Bnr.Param_Bits.monitor);
*9E
*9I
 		    state);
*9E
*4I
 		BannerSetArg (toggle_vector[banner_freeiocnt], 
*17D
 		    DwtNsensitive,
*17E
*17I
 		    XmNsensitive,
*17E
*9D
 		    New_Bnr.Param_Bits.monitor);
*9E
*9I
 		    state);
*9E
 		BannerSetArg (toggle_vector[banner_freeblkcnt], 
*17D
 		    DwtNsensitive,
*17E
*17I
 		    XmNsensitive,
*17E
*9D
 		    New_Bnr.Param_Bits.monitor);
*9E
*9I
 		    state);
*9E
 
 		BannerSetArg (toggle_vector[banner_disk_0], 
*17D
 		    DwtNsensitive,
*17E
*17I
 		    XmNsensitive,
*17E
*9D
 		    New_Bnr.Param_Bits.monitor);
*9E
*9I
 		    state);
*9E
 		BannerSetArg (toggle_vector[banner_disk_0_name], 
*17D
 		    DwtNsensitive,
*17E
*17I
 		    XmNsensitive,
*17E
*9D
 		    New_Bnr.Param_Bits.monitor);
*9E
*9I
 		    state);
*9E
 		BannerSetArg (toggle_vector[banner_disk_1], 
*17D
 		    DwtNsensitive,
*17E
*17I
 		    XmNsensitive,
*17E
*9D
 		    New_Bnr.Param_Bits.monitor);
*9E
*9I
 		    state);
*9E
 		BannerSetArg (toggle_vector[banner_disk_1_name], 
*17D
 		    DwtNsensitive,
*17E
*17I
 		    XmNsensitive,
*17E
*9D
 		    New_Bnr.Param_Bits.monitor);
*9E
*9I
 		    state);
*9E
 		BannerSetArg (toggle_vector[banner_disk_2], 
*17D
 		    DwtNsensitive,
*17E
*17I
 		    XmNsensitive,
*17E
*9D
 		    New_Bnr.Param_Bits.monitor);
*9E
*9I
 		    state);
*9E
 		BannerSetArg (toggle_vector[banner_disk_2_name], 
*17D
 		    DwtNsensitive,
*17E
*17I
 		    XmNsensitive,
*17E
*9D
 		    New_Bnr.Param_Bits.monitor);
*9E
*9I
 		    state);
*9E
*4E
 		break;
 		}
*19I
 
 	     case banner_show_hold :
 		{
 		New_Bnr.Param_Bits.jobs_on_hold = !New_Bnr.Param_Bits.jobs_on_hold;
 		break;
 		}
 
 	     case banner_show_all_users :
 		{
 		New_Bnr.Param_Bits.all_users = !New_Bnr.Param_Bits.all_users;
 		break;
 		}
 	     case banner_show_stopped :
 		{
 		New_Bnr.Param_Bits.stopped_queues = !New_Bnr.Param_Bits.stopped_queues;
 		break;
 		}
 	     case banner_show_int :
 		{
 		New_Bnr.Param_Bits.int_jobs = !New_Bnr.Param_Bits.int_jobs;
 		break;
 		}
 
 
 
     	     case banner_batch_queue :
     	     case banner_print_queue :
 		{
 		int state;
 
 		if (*tag == banner_batch_queue)  
 		    {
 		    New_Bnr.Param_Bits.qb_window = !New_Bnr.Param_Bits.qb_window;
 		    BannerSetArg (toggle_vector[banner_batch_queue_name], 
 			XmNsensitive,
 			New_Bnr.Param_Bits.qb_window);
 		    }
 		else
 		    {
 		    New_Bnr.Param_Bits.qp_window = !New_Bnr.Param_Bits.qp_window;
 		    BannerSetArg (toggle_vector[banner_print_queue_name], 
 			XmNsensitive,
 			New_Bnr.Param_Bits.qp_window);
 		    }
 
 		state = New_Bnr.Param_Bits.qp_window | New_Bnr.Param_Bits.qb_window;
 
 		BannerSetArg (toggle_vector[banner_queue_user], 
 		    XmNsensitive,
 		    state);
 		BannerSetArg (toggle_vector[banner_show_hold], 
 		    XmNsensitive,
 		    state);
 		BannerSetArg (toggle_vector[banner_show_all_users], 
 		    XmNsensitive,
 		    state);
 		BannerSetArg (toggle_vector[banner_show_stopped], 
 		    XmNsensitive,
 		    state);
 		BannerSetArg (toggle_vector[banner_show_int], 
 		    XmNsensitive,
 		    state);
 		break;
 		}
*19E
 	     case banner_analog_window_toggle :
 		{
 		New_Bnr.Param_Bits.analog_clock = 
 		    !New_Bnr.Param_Bits.analog_clock;		
 		BannerSetArg (toggle_vector[banner_seconds_active], 
*17D
 		    DwtNsensitive,
*17E
*17I
 		    XmNsensitive,
*17E
 		    New_Bnr.Param_Bits.digital_clock ||
 		    New_Bnr.Param_Bits.analog_clock);
 		break;
 		}
 	     case banner_digital_window_toggle :
 		{
 		New_Bnr.Param_Bits.digital_clock = !New_Bnr.Param_Bits.digital_clock;		
 		BannerSetArg (toggle_vector[banner_seconds_active], 
*17D
 		    DwtNsensitive,
*17E
*17I
 		    XmNsensitive,
*17E
 		    New_Bnr.Param_Bits.digital_clock ||
 		    New_Bnr.Param_Bits.analog_clock);
 		break;
 		}
 	     case banner_dua0_active :
 		{
 		New_Mon.dua0 = !New_Mon.dua0;
 		break;
 		}
 	     case banner_dua1_active :
 		{
 		New_Mon.dua1 = !New_Mon.dua1;
 		break;
 		}
 	     case banner_dua2_active :
 		{
 		New_Mon.dua2 = !New_Mon.dua2;
 		break;
 		}
 	     case banner_page_io_active :
 		{
 		New_Mon.pio = !New_Mon.pio;
 		break;
 		}
 	     case banner_page_fault_active :
 		{
 		New_Mon.pf = !New_Mon.pf;
 		break;
 		}
 	     case banner_sysfaults_active :
 		{
 		New_Mon.sysflt = !New_Mon.sysflt;
 		break;
 		}
 	     case banner_ni_io_active :
 		{
 		New_Mon.ni_io = !New_Mon.ni_io;
 		break;
 		}
 	     case banner_modified_pages_active :
 		{
 		New_Mon.mp = !New_Mon.mp;
 		break;
 		}
 	     case banner_free_pages_active :
 		{
 		New_Mon.fp = !New_Mon.fp;
 		break;
 		}
     	     }
 }
*6D
 
 
 /*
  *++
  *  NAME:
  *	name_size
  *
  *  FUNCTIONAL DESCRIPTION:
  *
  *      [@tbs@]
  *
  *  FORMAL PARAMETERS:
  *
  *      [@description_or_none@]
  *
  *  IMPLICIT INPUTS:
  *
  *      None.
  *
  *  IMPLICIT OUTPUTS:
  *
  *      None.
  *
  *  RETURNS or COMPLETION CODES:
  *
  *      [@description_or_none@]
  *
  *  SIDE EFFECTS:
  *
  *      None.
  *
  *--
  */
 
 
 int name_size(char *in_string)
 {
 int i = 0;
 
 /*
  * count the size
  */
 	while (*in_string++ != '\0')
 		i++;
 
 	i++;
 
 /*
  * remove any trailing spaces
  */
 
 	in_string--;
 	while (*in_string == ' ' && i>0)
 	    {
 	    *in_string = '\0';
 	    in_string--;
 	    i--;
 	    }
 return i;
 }
 
*6E
*28I
 
 /*
  * Create a cursors
  */
 void
 BannerCreateCursors (w)
     Widget w;
 {
 Cursor cursor;
 Font font;
 Display	*display;
 Screen	*screen;
 XColor  cursor_colors[2];
 Pixel   fg, bg;
 
 display = XtDisplay(w);
 screen  = XtScreen(w);
 
 cursor_colors[0].pixel = BlackPixelOfScreen (screen);
 cursor_colors[1].pixel = WhitePixelOfScreen (screen);
 
 XQueryColors( display, DefaultColormapOfScreen(screen), cursor_colors, 2);
 
 font = XLoadFont(display, "DECw$Cursor");
 
 cursors[0] = XCreateGlyphCursor(display,
     font, font, decw$c_wait_cursor, (decw$c_wait_cursor+1) ,
     &cursor_colors[0], 
     &cursor_colors[1] );
 
 cursors[1] = XCreateGlyphCursor(display,
     font, font, decw$c_grabhand_cursor, (decw$c_grabhand_cursor+1) ,
     &cursor_colors[0], 
     &cursor_colors[1] );
 
 XUnloadFont(display, font);
 }
 
 /*
  * Create a wait cursor
  */
 void
 BannerDisplayCursor (w, cursor)
     Widget w;
     int cursor;
 {
 /*
  * Wait cursor we put on in all screens, other cursors only in current screen
  */
 
     if (cursor == 0)
 	{
 	XDefineCursor (XtDisplay (Bnr.main_widget), XtWindow(Bnr.main_widget), 
 	    cursors[cursor]);
 	XDefineCursor (XtDisplay (Bnr.clock_widget), XtWindow(Bnr.clock_widget), 
 	    cursors[cursor]);
 	XDefineCursor (XtDisplay (Bnr.globe_widget), XtWindow(Bnr.globe_widget), 
 	    cursors[cursor]);
 	XDefineCursor (XtDisplay (Bnr.hanoi_widget), XtWindow(Bnr.hanoi_widget), 
 	    cursors[cursor]);
 	XDefineCursor (XtDisplay (Bnr.cube_widget), XtWindow(Bnr.cube_widget), 
 	    cursors[cursor]);
 	XDefineCursor (XtDisplay (Bnr.globe_widget), XtWindow(Bnr.globe_widget), 
 	    cursors[cursor]);
 	XDefineCursor (XtDisplay (Bnr.cpu_widget), XtWindow(Bnr.cpu_widget), 
 	    cursors[cursor]);
*35D
*34A1D
 	XDefineCursor (XtDisplay (Bnr.mon_widget), XtWindow(Bnr.mon_widget), 
*34A1E
*34A1I
 	XDefineCursor (XtDisplay (Bnr.link_widget), XtWindow(Bnr.link_widget), 
*34A1E
*35E
*35I
*36D
 	XDefineCursor (XtDisplay (Bnr.link_widget), XtWindow(Bnr.link_widget), 
*36E
*36I
*37I
*38D
 **************** Conflict 46    ************************************************
*38E
*37E
 	XDefineCursor (XtDisplay (Bnr.mon_widget), XtWindow(Bnr.mon_widget), 
*36E
*35E
*37I
*38D
 ********************************************************************************
*38E
*39I
 	    cursors[cursor]);
*39E
 	XDefineCursor (XtDisplay (Bnr.link_widget), XtWindow(Bnr.link_widget), 
*38D
 ************** End of Conflict 46    *******************************************
*38E
*37E
 	    cursors[cursor]);
 	XDefineCursor (XtDisplay (Bnr.process_widget), XtWindow(Bnr.process_widget), 
 	    cursors[cursor]);
 	XDefineCursor (XtDisplay (Bnr.pe_widget), XtWindow(Bnr.pe_widget), 
 	    cursors[cursor]);
 	XDefineCursor (XtDisplay (Bnr.lck_widget), XtWindow(Bnr.lck_widget), 
 	    cursors[cursor]);
 	XDefineCursor (XtDisplay (Bnr.cwps_widget), XtWindow(Bnr.cwps_widget), 
 	    cursors[cursor]);
 	XDefineCursor (XtDisplay (Bnr.sys_widget), XtWindow(Bnr.sys_widget), 
 	    cursors[cursor]);
 	XDefineCursor (XtDisplay (Bnr.tp_widget), XtWindow(Bnr.tp_widget), 
 	    cursors[cursor]);
 	XDefineCursor (XtDisplay (Bnr.qp_widget), XtWindow(Bnr.qp_widget), 
 	    cursors[cursor]);
 	XDefineCursor (XtDisplay (Bnr.qb_widget), XtWindow(Bnr.qb_widget), 
 	    cursors[cursor]);
 	XDefineCursor (XtDisplay (Bnr.clu_widget), XtWindow(Bnr.clu_widget), 
 	    cursors[cursor]);
 	XDefineCursor (XtDisplay (Bnr.message_widget), XtWindow(Bnr.message_widget), 
 	    cursors[cursor]);
 	}
     else
 	XDefineCursor (XtDisplay (w), XtWindow(w), 
 	    cursors[cursor]);
 
     XFlush (XtDisplay(w));
 }
 
 /*
  * Create a wait cursor
  */
 void
 BannerUndisplayCursor ()
 {
 	
 	XUndefineCursor (XtDisplay (Bnr.main_widget), XtWindow(Bnr.main_widget));
 	XUndefineCursor (XtDisplay (Bnr.clock_widget), XtWindow(Bnr.clock_widget));
 	XUndefineCursor (XtDisplay (Bnr.globe_widget), XtWindow(Bnr.globe_widget));
 	XUndefineCursor (XtDisplay (Bnr.hanoi_widget), XtWindow(Bnr.hanoi_widget));
 	XUndefineCursor (XtDisplay (Bnr.cube_widget), XtWindow(Bnr.cube_widget));
 	XUndefineCursor (XtDisplay (Bnr.globe_widget), XtWindow(Bnr.globe_widget));
 	XUndefineCursor (XtDisplay (Bnr.cpu_widget), XtWindow(Bnr.cpu_widget));
*35D
*34A1D
 	XUndefineCursor (XtDisplay (Bnr.mon_widget), XtWindow(Bnr.mon_widget));
*34A1E
*34A1I
 	XUndefineCursor (XtDisplay (Bnr.link_widget), XtWindow(Bnr.link_widget));
*34A1E
*35E
*35I
*36D
 	XUndefineCursor (XtDisplay (Bnr.link_widget), XtWindow(Bnr.link_widget));
*36E
*36I
*37I
*38D
 **************** Conflict 47    ************************************************
*38E
*37E
 	XUndefineCursor (XtDisplay (Bnr.mon_widget), XtWindow(Bnr.mon_widget));
*36E
*35E
*37I
*38D
 ********************************************************************************
*38E
 	XUndefineCursor (XtDisplay (Bnr.link_widget), XtWindow(Bnr.link_widget));
*38D
 ************** End of Conflict 47    *******************************************
*38E
*37E
 	XUndefineCursor (XtDisplay (Bnr.process_widget), XtWindow(Bnr.process_widget));
 	XUndefineCursor (XtDisplay (Bnr.pe_widget), XtWindow(Bnr.pe_widget));
 	XUndefineCursor (XtDisplay (Bnr.lck_widget), XtWindow(Bnr.lck_widget));
 	XUndefineCursor (XtDisplay (Bnr.cwps_widget), XtWindow(Bnr.cwps_widget));
 	XUndefineCursor (XtDisplay (Bnr.sys_widget), XtWindow(Bnr.sys_widget));
 	XUndefineCursor (XtDisplay (Bnr.tp_widget), XtWindow(Bnr.tp_widget));
 	XUndefineCursor (XtDisplay (Bnr.qp_widget), XtWindow(Bnr.qp_widget));
 	XUndefineCursor (XtDisplay (Bnr.qb_widget), XtWindow(Bnr.qb_widget));
 	XUndefineCursor (XtDisplay (Bnr.clu_widget), XtWindow(Bnr.clu_widget));
 	XUndefineCursor (XtDisplay (Bnr.message_widget), XtWindow(Bnr.message_widget));
 }
*28E
