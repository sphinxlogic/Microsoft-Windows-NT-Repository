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


#include <decw$cursor.h>


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

int	 cursors[10];   

extern	 int	     BannerInitialPriority;
extern	 Son$_Blk    Son;
extern	 Bnr$_Blk    Bnr;
extern	 Clk$_Blk    Clk;
extern	 Link$_Blk   Link;
extern	 Mon$_Blk    Mon;
extern	 GC	     BannerGC;
extern	 XEvent	     BannerEvent;
extern	 char	     *BannerDayList[];
extern   Widget	     menu_widget;

extern char	     banner_font_name[];
extern char	     banner_font_size[];
extern char	     banner_font_wieght[];
extern char	     banner_font_type[];

extern   char	     *BannerVmsDisplayName;
extern   char	     BannerVmsNodeName[];
extern   char	     BannerVmsVersion[];


/*
 * The forward routine declarations
 */

void	BannerCreateCursors();
void	BannerDisplayCursor();
void	BannerUndisplayCursor();

void BannerMenuPopup ();
void BannerMenuPopupDestroy ();
void BannerPositionManagePopup ();
void BannerMenuPopup ();
void BannerMenuDestroy ();
void BannerMenuCallback ();
void BannerSetupCallback ();
void BannerToggleChangeCallback ();
void BannerToggleInitCallback ();
void BannerReadResource ();
void BannerSetDefaultResource ();
static	    BannerGetresource ();
static	    BannerStrToNumeric ();

/* Define costants used by the UI, in the UID file */
 
static Bnr$_Blk    New_Bnr;
static Mon$_Blk    New_Mon;
static Widget *toggle_vector[150];
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
#define banner_mon_window_toggle	    7
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
#define banner_link_window_toggle	    27

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
#define banner_window_border_active	    66

#define banner_freeiocnt		    67
#define banner_freeblkcnt		    68

#define banner_sys_window_toggle	    69

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

/*The functions */

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
if (Bnr.mon_widget != Bnr.dp_id)		
    {
    XClearWindow (XtDisplay(Bnr.mon_widget), 
	XtWindow (Bnr.mon_widget));
    BannerMonSetup ();
    BannerMonRefresh ();
    }
if (Bnr.link_widget != Bnr.dp_id)		
    {
    XClearWindow (XtDisplay(Bnr.link_widget), 
	XtWindow (Bnr.link_widget));
    BannerLinkSetup ();
    BannerLinkRefresh ();
    }
if (Bnr.process_widget != Bnr.dp_id)		
    {
    XClearWindow (XtDisplay(Bnr.process_widget), 
	XtWindow (Bnr.process_widget));
    BannerProcessSetup ();
    BannerProcessRefresh ();
    }
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
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

	BannerSetArg (widget, XmNy, y);
	BannerSetArg (widget, XmNx, x);
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	BannerGetResource (db resource, zero, resourcetype,
**			   resourcevalue, flag)
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
BannerGetResource(db, resource, zero, resourcetype, resourcevalue, flag)
int db, zero;
char *resource;
XrmValue  *resourcevalue;
XrmRepresentation *resourcetype;
int flag;
{
int sts;
char resname[512];
/*
 * Read the open resource databases, first read the privdb, if that fails go
 * and try the appdb.
 */
    if (flag == 1)
	sprintf (resname, "DECW$BANNER.%s", resource);
    else
	sprintf (resname, "DECW$BANNER_%s.%s", BannerVmsNodeName, resource);

    return XrmGetResource (db, resname, zero, resourcetype, resourcevalue);
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
	db = XrmGetFileDatabase ("DECW$SYSTEM_DEFAULTS:DECW$BANNER.DAT");
    else 
	db = XtDatabase (XtDisplay (Bnr.dp_id));




    if (db == 0)
	return;


/*
 * Now start reading our resources, and see what we can get from the 
 * files.
 */
    if (BannerGetResource (db, "node_display_name", 0, &resourcetype,
	    &resourcevalue, flag))
	{
	BannerVmsDisplayName = (char *) XtMalloc (resourcevalue.size + 1);
	strcpy(BannerVmsDisplayName, resourcevalue.addr);
	}

    if (BannerGetResource (db, "font_template", 0, &resourcetype,
	    &resourcevalue, flag))
	{
	Bnr.fnt_template = (char *) XtMalloc (resourcevalue.size + 1);
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

    if (BannerGetResource (db, "font_family_name", 0, &resourcetype,
	    &resourcevalue, flag))
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

    if (BannerGetResource (db, "font_family_size", 0, &resourcetype,
	    &resourcevalue, flag))
	{
	strcpy(banner_font_size, resourcevalue.addr);
	sprintf (Bnr.fnt_name, Bnr.fnt_template, 
	    banner_font_name, banner_font_wieght, banner_font_size,
		banner_font_type);
	}

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

    if (BannerGetResource (db, "title", 0, &resourcetype,
	    &resourcevalue, flag))
	{
	Bnr.title = (char *) XtMalloc (resourcevalue.size + 1);
	strcpy(Bnr.title, resourcevalue.addr);
	}

    if (BannerGetResource (db, "highlight_colour", 0, &resourcetype,
	    &resourcevalue, flag))
	{
	Bnr.hwmrk_colour = (char *) XtMalloc (resourcevalue.size + 1);
	strcpy(Bnr.hwmrk_colour, resourcevalue.addr);
	}

    if (BannerGetResource (db, "foreground", 0, &resourcetype,
	    &resourcevalue, flag))
	{
	Bnr.foreground_colour = (char *) XtMalloc (resourcevalue.size + 1);
	strcpy(Bnr.foreground_colour, resourcevalue.addr);
	}

    if (BannerGetResource (db, "background", 0, &resourcetype,
	    &resourcevalue, flag))
	{
	Bnr.background_colour = (char *) XtMalloc (resourcevalue.size + 1);
	strcpy(Bnr.background_colour, resourcevalue.addr);
	}

    if (BannerGetResource (db, "title", 0, &resourcetype,
	    &resourcevalue, flag))
	{
	Bnr.title = (char *) XtMalloc (resourcevalue.size + 1);
	strcpy(Bnr.title, resourcevalue.addr);
	}

    if (BannerGetResource (db, "qp_name", 0, &resourcetype,
	    &resourcevalue, flag))
	{
	Bnr.qp_name = (char *) XtMalloc (resourcevalue.size + 1);
	strcpy(Bnr.qp_name, resourcevalue.addr);
	}
    else
	{
	Bnr.qp_name = (char *) XtMalloc (6);
	strcpy(Bnr.qp_name, "*LPS*");
	}

    if (BannerGetResource (db, "qb_name", 0, &resourcetype,
	    &resourcevalue, flag))
	{
	Bnr.qb_name = (char *) XtMalloc (resourcevalue.size + 1);
	strcpy(Bnr.qb_name, resourcevalue.addr);
	}
    else
	{
	Bnr.qb_name = (char *) XtMalloc (8);
	strcpy(Bnr.qb_name, "*BATCH*");
	}

    if (BannerGetResource (db, "queue_user", 0, &resourcetype,
	    &resourcevalue, flag))
	{
	Bnr.queue_user = (char *) XtMalloc (resourcevalue.size + 1);
	strcpy(Bnr.queue_user, resourcevalue.addr);
	}
    else
	{
	Bnr.queue_user = (char *) XtMalloc (strlen (getenv("USER")) + 1);
	strcpy(Bnr.queue_user, getenv("USER"));
	}

    if (BannerGetResource (db, "disk_0_name", 0, &resourcetype,
	    &resourcevalue, flag))
	{
	Bnr.disk_0_name = (char *) XtMalloc (resourcevalue.size + 1);
	strcpy(Bnr.disk_0_name, resourcevalue.addr);
	}

    if (BannerGetResource (db, "disk_1_name", 0, &resourcetype,
	    &resourcevalue, flag))
	{
	Bnr.disk_1_name = (char *) XtMalloc (resourcevalue.size + 1);
	strcpy(Bnr.disk_1_name, resourcevalue.addr);
	}

    if (BannerGetResource (db, "disk_2_name", 0, &resourcetype,
	    &resourcevalue, flag))
	{
	Bnr.disk_2_name = (char *) XtMalloc (resourcevalue.size + 1);
	strcpy(Bnr.disk_2_name, resourcevalue.addr);
	}

    if (BannerGetResource (db, "disk_0", 0, &resourcetype,
	    &resourcevalue, flag))
	{
	Bnr.disk_0 = (char *) XtMalloc (resourcevalue.size + 1);
	strcpy(Bnr.disk_0, resourcevalue.addr);
	}

    if (BannerGetResource (db, "disk_1", 0, &resourcetype,
	    &resourcevalue, flag))
	{
	Bnr.disk_1 = (char *) XtMalloc (resourcevalue.size + 1);
	strcpy(Bnr.disk_1, resourcevalue.addr);
	}

    if (BannerGetResource (db, "disk_2", 0, &resourcetype,
	    &resourcevalue, flag))
	{
	Bnr.disk_2 = (char *) XtMalloc (resourcevalue.size + 1);
	strcpy(Bnr.disk_2, resourcevalue.addr);
	}


    if (BannerGetResource (db, "cpu_update", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.cpu_update = BannerStrToNumeric (resourcevalue.addr);


    if (BannerGetResource (db, "process_update", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.process_update = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "monitor_update", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.monitor_update = BannerStrToNumeric (resourcevalue.addr);


    if (BannerGetResource (db, "link_update", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.link_update = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "pe_update", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.pe_update = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "lck_update", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.lck_update = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "cwps_update", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.cwps_update = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "sys_ioratemax", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.sys_ioratemax = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "sysrespgfile", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.sysrespgfile = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "sys_faultratemax", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.sys_faultratemax = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "sys_highlight", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.sys_highlight = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "hwmrk_update", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.hwmrk_update = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "sys_update", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.sys_update = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "tp_update", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.tp_update = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "qp_update", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.qp_update = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "qb_update", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.qb_update = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "clu_update", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.clu_update = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "cpu_count", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.cpu_count = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "clock_x", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.clock_x = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "clock_y", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.clock_y = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "message_x", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.message_x = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "message_y", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.message_y = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "cube_x", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.cube_x = BannerStrToNumeric (resourcevalue.addr);
    if (BannerGetResource (db, "cube_y", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.cube_y = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "hanoi_x", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.hanoi_x = BannerStrToNumeric (resourcevalue.addr);
    if (BannerGetResource (db, "hanoi_y", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.hanoi_y = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "globe_x", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.globe_x = BannerStrToNumeric (resourcevalue.addr);
    if (BannerGetResource (db, "globe_y", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.globe_y = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "cpu_x", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.cpu_x = BannerStrToNumeric (resourcevalue.addr);
    if (BannerGetResource (db, "cpu_y", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.cpu_y = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "pe_x", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.pe_x = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "pe_y", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.pe_y = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "lck_x", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.lck_x = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "lck_y", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.lck_y = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "cwps_x", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.cwps_x = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "cwps_y", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.cwps_y = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "sys_x", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.sys_x = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "sys_y", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.sys_y = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "tp_x", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.tp_x = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "tp_y", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.tp_y = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "qp_x", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.qp_x = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "qp_y", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.qp_y = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "qb_x", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.qb_x = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "qb_y", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.qb_y = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "clu_x", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.clu_x = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "clu_y", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.clu_y = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "mon_x", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.mon_x = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "mon_y", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.mon_y = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "link_x", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.link_x = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "link_y", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.link_y = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "process_x", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.process_x = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "process_y", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.process_y = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "clock_height", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.clock_height = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "clock_width", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.clock_width = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "message_height", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.message_height = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "message_width", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.message_width = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "message_init_state", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.message_init_state = BannerStrToNumeric (resourcevalue.addr);


    if (BannerGetResource (db, "cube_height", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.cube_height = BannerStrToNumeric (resourcevalue.addr);
    if (BannerGetResource (db, "cube_width", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.cube_width = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "globe_height", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.globe_height = BannerStrToNumeric (resourcevalue.addr);
    if (BannerGetResource (db, "globe_width", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.globe_width = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "hanoi_height", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.hanoi_height = BannerStrToNumeric (resourcevalue.addr);
    if (BannerGetResource (db, "hanoi_width", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.hanoi_width = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "cpu_height", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.cpu_height = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "cpu_width", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.cpu_width = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "mon_height", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.mon_height = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "mon_width", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.mon_width = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "link_height", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.link_height = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "link_width", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.link_width = BannerStrToNumeric (resourcevalue.addr);


    if (BannerGetResource (db, "pe_height", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.pe_height = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "pe_width", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.pe_width = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "lck_height", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.lck_height = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "lck_width", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.lck_width = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "cwps_width", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.cwps_width = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "cwps_height", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.cwps_height = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "sys_width", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.sys_width = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "sys_height", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.sys_height = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "tp_width", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.tp_width = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "tp_height", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.tp_height = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "qp_width", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.qp_width = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "qp_height", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.qp_height = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "qb_width", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.qb_width = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "qb_height", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.qb_height = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "clu_width", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.clu_width = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "clu_height", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.clu_height = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "process_height", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.process_height = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "process_width", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.process_width = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "border", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.Param_Bits.border = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "window_border", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.Param_Bits.window_border = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "freeiocnt", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.Param_Bits.freeiocnt = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "freeblkcnt", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.Param_Bits.freeblkcnt = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "cube", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.Param_Bits.cube = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "world", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.Param_Bits.world= BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "process_name", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.Param_Bits.process_name= BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "process_user", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.Param_Bits.process_user= BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "process_pid", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.Param_Bits.process_pid= BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "bars", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.Param_Bits.bars= BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "process_states", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.Param_Bits.process_states= BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "iocnt", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.Param_Bits.iocnt= BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "monitor", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.Param_Bits.monitor= BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "link_window", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.Param_Bits.link_window = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "process_window", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.Param_Bits.process_window= BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "message_hide_timer", 0, &resourcetype,
	    &resourcevalue, flag))
		BannerMessageSetHide (resourcevalue.addr);

    if (BannerGetResource (db, "message_size", 0, &resourcetype,
	    &resourcevalue, flag))
		BannerMessageSetSize (BannerStrToNumeric (resourcevalue.addr));

    if (BannerGetResource (db, "pe_window", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.Param_Bits.pe_window= BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "lck_window", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.Param_Bits.lck_window= BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "cwps_window", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.Param_Bits.cwps_window= BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "sys_window", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.Param_Bits.sys_window= BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "tp_window", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.Param_Bits.tp_window= BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "tp_max_rate", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.tp_max_rate= BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "tp_rate1_start", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.tp_rate1_start= BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "tp_rate2_start", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.tp_rate2_start= BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "tp_rate3_start", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.tp_rate3_start= BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "tp_rate4_start", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.tp_rate4_start= BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "tp_rate1_end", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.tp_rate1_end= BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "tp_rate2_end", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.tp_rate2_end= BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "tp_rate3_end", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.tp_rate3_end= BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "total_com_rate", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.Param_Bits.total_com_rate= BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "onef_comp_rate", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.Param_Bits.onef_comp_rate= BannerStrToNumeric (resourcevalue.addr);
    if (BannerGetResource (db, "start_rate", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.Param_Bits.start_rate= BannerStrToNumeric (resourcevalue.addr);
    if (BannerGetResource (db, "prep_rate", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.Param_Bits.prep_rate= BannerStrToNumeric (resourcevalue.addr);
    if (BannerGetResource (db, "abort_rate", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.Param_Bits.abort_rate= BannerStrToNumeric (resourcevalue.addr);
    if (BannerGetResource (db, "end_rate", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.Param_Bits.end_rate= BannerStrToNumeric (resourcevalue.addr);
    if (BannerGetResource (db, "rem_start_rate", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.Param_Bits.rem_start_rate= BannerStrToNumeric (resourcevalue.addr);
    if (BannerGetResource (db, "rem_end_rate", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.Param_Bits.rem_end_rate= BannerStrToNumeric (resourcevalue.addr);
    if (BannerGetResource (db, "comp_rate1", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.Param_Bits.comp_rate1= BannerStrToNumeric (resourcevalue.addr);
    if (BannerGetResource (db, "comp_rate2", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.Param_Bits.comp_rate2= BannerStrToNumeric (resourcevalue.addr);
    if (BannerGetResource (db, "comp_rate3", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.Param_Bits.comp_rate3= BannerStrToNumeric (resourcevalue.addr);
    if (BannerGetResource (db, "comp_rate4", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.Param_Bits.comp_rate4= BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "qp_window", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.Param_Bits.qp_window= BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "qb_window", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.Param_Bits.qb_window= BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "clu_window", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.Param_Bits.clu_window= BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "cpu", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.Param_Bits.cpu_window= BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "message_window", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.Param_Bits.message_window= BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "modes", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.Param_Bits.modes_window= BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "multi_hist", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.Param_Bits.cpu_multi_hist = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "modes", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.Param_Bits.modes_window= BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "hanoi", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.Param_Bits.hanoi= BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "seconds", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.Param_Bits.seconds= BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "digital_clock", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.Param_Bits.digital_clock= BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "analog_clock", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.Param_Bits.analog_clock= BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "all_users", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.Param_Bits.all_users= BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "jobs_on_hold", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.Param_Bits.jobs_on_hold= BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "stopped_queues", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.Param_Bits.stopped_queues= BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "int_jobs", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.Param_Bits.int_jobs= BannerStrToNumeric (resourcevalue.addr);


    if (BannerGetResource (db, "dua0", 0, &resourcetype,
	    &resourcevalue, flag))
	Mon.dua0= BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "dua1", 0, &resourcetype,
	    &resourcevalue, flag))
	Mon.dua1= BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "dua2", 0, &resourcetype,
	    &resourcevalue, flag))
	Mon.dua2= BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "pio", 0, &resourcetype,
	    &resourcevalue, flag))
	Mon.pio= BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "pf", 0, &resourcetype,
	    &resourcevalue, flag))
	Mon.pf= BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "sysflt", 0, &resourcetype,
	    &resourcevalue, flag))
	Mon.sysflt= BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "ni_io", 0, &resourcetype,
	    &resourcevalue, flag))
	Mon.ni_io= BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "mp", 0, &resourcetype,
	    &resourcevalue, flag))
	Mon.mp= BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "fp", 0, &resourcetype,
	    &resourcevalue, flag))
	Mon.fp= BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "bell", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.Param_Bits.bell= BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "bell_volume", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.bell_volume = BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "icon_on", 0, &resourcetype,
	    &resourcevalue, flag))
	Bnr.Param_Bits.icon_on= BannerStrToNumeric (resourcevalue.addr);

    if (BannerGetResource (db, "base_priority", 0, &resourcetype,
	    &resourcevalue, flag))
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
    char string[250];
    int db, db1;
    char    nodedb[100];



    db1 = XtDatabase (XtDisplay(Bnr.dp_id));

    sprintf (nodedb, "DECW$USER_DEFAULTS:DECW$BANNER_%s.DAT",
	BannerVmsNodeName);

    db = XrmGetFileDatabase (nodedb);

/*
 * Now start putting our resources, and see what we can get from the 
 * files.
 */

    if (BannerVmsDisplayName != NULL)
	sprintf (string, "DECW$BANNER_%s.node_display_name: %s\n", BannerVmsNodeName, 
	    BannerVmsDisplayName);
    else
	sprintf (string, "DECW$BANNER_%s.node_display_name: \n", BannerVmsNodeName);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.font_template: %s\n", BannerVmsNodeName, Bnr.fnt_template);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.font_family_name: %s\n", BannerVmsNodeName, banner_font_name);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.font_family_size: %s\n", BannerVmsNodeName, banner_font_size);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.title: %s\n", BannerVmsNodeName, Bnr.title);
    BannerSetResource(string, &db, &db1);

    if (Bnr.hwmrk_colour != 0)
	{    
	sprintf (string, "DECW$BANNER_%s.highlight_colour: %s\n", BannerVmsNodeName, 
	    Bnr.hwmrk_colour);
	BannerSetResource(string, &db, &db1);
	}

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
	sprintf (string, "DECW$BANNER_%s.mwait_sound: %s\n", BannerVmsNodeName, 
	    Son.Mwait.DisplayName);
    else
	sprintf (string, "DECW$BANNER_%s.mwait_sound: \n", BannerVmsNodeName);
    BannerSetResource(string, &db, &db1);

    if (Son.NewProcesses.DisplayName != NULL)
	sprintf (string, "DECW$BANNER_%s.newprocesses_sound: %s\n", BannerVmsNodeName, 
	    Son.NewProcesses.DisplayName);
    else
	sprintf (string, "DECW$BANNER_%s.newprocesses_sound: \n", BannerVmsNodeName);
    BannerSetResource(string, &db, &db1);

    if (Son.LessProcesses.DisplayName != NULL)
	sprintf (string, "DECW$BANNER_%s.lessprocesses_sound: %s\n", BannerVmsNodeName, 
	    Son.LessProcesses.DisplayName);
    else
	sprintf (string, "DECW$BANNER_%s.lessprocesses_sound: \n", BannerVmsNodeName);
    BannerSetResource(string, &db, &db1);

    if (Son.Disk1.DisplayName != NULL)
	sprintf (string, "DECW$BANNER_%s.disk1_sound: %s\n", BannerVmsNodeName, 
	    Son.Disk1.DisplayName);
    else
	sprintf (string, "DECW$BANNER_%s.disk1_sound: \n", BannerVmsNodeName);
    BannerSetResource(string, &db, &db1);

    if (Son.Disk2.DisplayName != NULL)
	sprintf (string, "DECW$BANNER_%s.disk2_sound: %s\n", BannerVmsNodeName, 
	    Son.Disk2.DisplayName);
    else
	sprintf (string, "DECW$BANNER_%s.disk2_sound: \n", BannerVmsNodeName);
    BannerSetResource(string, &db, &db1);

    if (Son.Disk3.DisplayName != NULL)
	sprintf (string, "DECW$BANNER_%s.disk3_sound: %s\n", BannerVmsNodeName, 
	    Son.Disk3.DisplayName);
    else
	sprintf (string, "DECW$BANNER_%s.disk3_sound: \n", BannerVmsNodeName);
    BannerSetResource(string, &db, &db1);

    if (Son.Memory.DisplayName != NULL)
	sprintf (string, "DECW$BANNER_%s.memory_sound: %s\n", BannerVmsNodeName, 
	    Son.Memory.DisplayName);
    else
	sprintf (string, "DECW$BANNER_%s.memory_sound: \n", BannerVmsNodeName);
    BannerSetResource(string, &db, &db1);

    if (Son.PageFile.DisplayName != NULL)
	sprintf (string, "DECW$BANNER_%s.pagefile_sound: %s\n", BannerVmsNodeName, 
	    Son.PageFile.DisplayName);
    else
	sprintf (string, "DECW$BANNER_%s.pagefile_sound: \n", BannerVmsNodeName);
    BannerSetResource(string, &db, &db1);

    if (Son.PendingIO.DisplayName != NULL)
	sprintf (string, "DECW$BANNER_%s.pendingio_sound: %s\n", BannerVmsNodeName, 
	    Son.PendingIO.DisplayName);
    else
	sprintf (string, "DECW$BANNER_%s.pendingio_sound: \n", BannerVmsNodeName);
    BannerSetResource(string, &db, &db1);



/*    
    if (Bnr.foreground_colour != 0)
	{    
	sprintf (string, "DECW$BANNER*foreground: %s\n", BannerVmsNodeName, 
	    Bnr.foreground_colour);
	BannerSetResource(string, &db, &db1);
	}

    if (Bnr.background_colour != 0)
	{    
	sprintf (string, "DECW$BANNER*background: %s\n", BannerVmsNodeName, 
	    Bnr.background_colour);
	BannerSetResource(string, &db, &db1);
	}
*/

    sprintf (string, "DECW$BANNER_%s.qp_name: %s\n", BannerVmsNodeName, Bnr.qp_name);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.qb_name: %s\n", BannerVmsNodeName, Bnr.qb_name);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.queue_user: %s\n", BannerVmsNodeName, Bnr.queue_user);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.disk_0_name: %s\n", BannerVmsNodeName, Bnr.disk_0_name);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.disk_1_name: %s\n", BannerVmsNodeName, Bnr.disk_1_name);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.disk_2_name: %s\n", BannerVmsNodeName, Bnr.disk_2_name);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.disk_0: %s\n", BannerVmsNodeName, Bnr.disk_0);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.disk_1: %s\n", BannerVmsNodeName, Bnr.disk_1);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.disk_2: %s\n", BannerVmsNodeName, Bnr.disk_2);
    BannerSetResource(string, &db, &db1);


    sprintf (string, "DECW$BANNER_%s.cpu_update: %d\n", BannerVmsNodeName, Bnr.cpu_update);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.process_update: %d\n", BannerVmsNodeName, Bnr.process_update);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.monitor_update: %d\n", BannerVmsNodeName, Bnr.monitor_update);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.link_update: %d\n", BannerVmsNodeName, 
	Bnr.link_update);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.pe_update: %d\n", BannerVmsNodeName, Bnr.pe_update);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.lck_update: %d\n", BannerVmsNodeName, Bnr.lck_update);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.cwps_update: %d\n", BannerVmsNodeName, Bnr.cwps_update);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.sys_ioratemax: %d\n", BannerVmsNodeName, Bnr.sys_ioratemax);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.sysrespgfile: %d\n", BannerVmsNodeName, Bnr.sysrespgfile);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.sys_faultratemax: %d\n", BannerVmsNodeName, Bnr.sys_faultratemax);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.sys_highlight: %d\n", BannerVmsNodeName, Bnr.sys_highlight);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.sys_update: %d\n", BannerVmsNodeName, Bnr.sys_update);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.tp_update: %d\n", BannerVmsNodeName, Bnr.tp_update);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.qp_update: %d\n", BannerVmsNodeName, Bnr.qp_update);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.qb_update: %d\n", BannerVmsNodeName, Bnr.qb_update);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.clu_update: %d\n", BannerVmsNodeName, Bnr.clu_update);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.hwmrk_update: %d\n", BannerVmsNodeName, Bnr.hwmrk_update);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.cpu_count: %d\n", BannerVmsNodeName, Bnr.cpu_count);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.clock_x: %d\n", BannerVmsNodeName, Bnr.clock_x);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.clock_y: %d\n", BannerVmsNodeName, Bnr.clock_y);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.clock_width: %d\n", BannerVmsNodeName, Bnr.clock_width);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.clock_height: %d\n", BannerVmsNodeName, Bnr.clock_height);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.message_x: %d\n", BannerVmsNodeName, Bnr.message_x);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.message_y: %d\n", BannerVmsNodeName, Bnr.message_y);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.message_width: %d\n", BannerVmsNodeName, Bnr.message_width);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.message_height: %d\n", BannerVmsNodeName, Bnr.message_height);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.message_init_state: %d\n", BannerVmsNodeName, Bnr.message_init_state);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.cube_x: %d\n", BannerVmsNodeName, Bnr.cube_x);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.cube_y: %d\n", BannerVmsNodeName, Bnr.cube_y);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.cube_width: %d\n", BannerVmsNodeName, Bnr.cube_width);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.cube_height: %d\n", BannerVmsNodeName, Bnr.cube_height);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.hanoi_x: %d\n", BannerVmsNodeName, Bnr.hanoi_x);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.hanoi_y: %d\n", BannerVmsNodeName, Bnr.hanoi_y);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.hanoi_width: %d\n", BannerVmsNodeName, Bnr.hanoi_width);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.hanoi_height: %d\n", BannerVmsNodeName, Bnr.hanoi_height);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.globe_x: %d\n", BannerVmsNodeName, Bnr.globe_x);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.globe_y: %d\n", BannerVmsNodeName, Bnr.globe_y);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.globe_width: %d\n", BannerVmsNodeName, Bnr.globe_width);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.globe_height: %d\n", BannerVmsNodeName, Bnr.globe_height);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.cpu_x: %d\n", BannerVmsNodeName, Bnr.cpu_x);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.cpu_y: %d\n", BannerVmsNodeName, Bnr.cpu_y);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.cpu_width: %d\n", BannerVmsNodeName, Bnr.cpu_width);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.cpu_height: %d\n", BannerVmsNodeName, Bnr.cpu_height);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.mon_x: %d\n", BannerVmsNodeName, Bnr.mon_x);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.mon_y: %d\n", BannerVmsNodeName, Bnr.mon_y);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.mon_width: %d\n", BannerVmsNodeName, Bnr.mon_width);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.mon_height: %d\n", BannerVmsNodeName, Bnr.mon_height);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.link_x: %d\n", BannerVmsNodeName, Bnr.link_x);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.link_y: %d\n", BannerVmsNodeName, Bnr.link_y);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.link_width: %d\n", BannerVmsNodeName, Bnr.link_width);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.link_height: %d\n", BannerVmsNodeName, Bnr.link_height);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.pe_x: %d\n", BannerVmsNodeName, Bnr.pe_x);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.pe_y: %d\n", BannerVmsNodeName, Bnr.pe_y);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.pe_width: %d\n", BannerVmsNodeName, Bnr.pe_width);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.pe_height: %d\n", BannerVmsNodeName, Bnr.pe_height);
    BannerSetResource(string, &db, &db1);


    sprintf (string, "DECW$BANNER_%s.lck_x: %d\n", BannerVmsNodeName, Bnr.lck_x);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.lck_y: %d\n", BannerVmsNodeName, Bnr.lck_y);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.lck_width: %d\n", BannerVmsNodeName, Bnr.lck_width);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.lck_height: %d\n", BannerVmsNodeName, Bnr.lck_height);
    BannerSetResource(string, &db, &db1);


    sprintf (string, "DECW$BANNER_%s.cwps_x: %d\n", BannerVmsNodeName, Bnr.cwps_x);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.cwps_y: %d\n", BannerVmsNodeName, Bnr.cwps_y);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.cwps_width: %d\n", BannerVmsNodeName, Bnr.cwps_width);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.cwps_height: %d\n", BannerVmsNodeName, Bnr.cwps_height);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.sys_x: %d\n", BannerVmsNodeName, Bnr.sys_x);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.sys_y: %d\n", BannerVmsNodeName, Bnr.sys_y);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.sys_width: %d\n", BannerVmsNodeName, Bnr.sys_width);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.sys_height: %d\n", BannerVmsNodeName, Bnr.sys_height);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.tp_x: %d\n", BannerVmsNodeName, Bnr.tp_x);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.tp_y: %d\n", BannerVmsNodeName, Bnr.tp_y);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.tp_width: %d\n", BannerVmsNodeName, Bnr.tp_width);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.tp_height: %d\n", BannerVmsNodeName, Bnr.tp_height);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.qp_x: %d\n", BannerVmsNodeName, Bnr.qp_x);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.qp_y: %d\n", BannerVmsNodeName, Bnr.qp_y);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.qp_width: %d\n", BannerVmsNodeName, Bnr.qp_width);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.qp_height: %d\n", BannerVmsNodeName, Bnr.qp_height);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.qb_x: %d\n", BannerVmsNodeName, Bnr.qb_x);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.qb_y: %d\n", BannerVmsNodeName, Bnr.qb_y);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.qb_width: %d\n", BannerVmsNodeName, Bnr.qb_width);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.qb_height: %d\n", BannerVmsNodeName, Bnr.qb_height);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.clu_x: %d\n", BannerVmsNodeName, Bnr.clu_x);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.clu_y: %d\n", BannerVmsNodeName, Bnr.clu_y);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.clu_width: %d\n", BannerVmsNodeName, Bnr.clu_width);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.clu_height: %d\n", BannerVmsNodeName, Bnr.clu_height);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.process_x: %d\n", BannerVmsNodeName, Bnr.process_x);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.process_y: %d\n", BannerVmsNodeName, Bnr.process_y);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.process_width: %d\n", BannerVmsNodeName, Bnr.process_width);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.process_height: %d\n", BannerVmsNodeName, Bnr.process_height);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.border: %d\n", BannerVmsNodeName, Bnr.Param_Bits.border);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.window_border: %d\n", BannerVmsNodeName, Bnr.Param_Bits.window_border);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.world: %d\n", BannerVmsNodeName, Bnr.Param_Bits.world);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.process_name: %d\n", BannerVmsNodeName, Bnr.Param_Bits.process_name);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.process_user: %d\n", BannerVmsNodeName, Bnr.Param_Bits.process_user);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.process_pid: %d\n", BannerVmsNodeName, Bnr.Param_Bits.process_pid);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.freeiocnt: %d\n", BannerVmsNodeName, Bnr.Param_Bits.freeiocnt);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.freeblkcnt: %d\n", BannerVmsNodeName, Bnr.Param_Bits.freeblkcnt);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.cube: %d\n", BannerVmsNodeName, Bnr.Param_Bits.cube);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.bars: %d\n", BannerVmsNodeName, Bnr.Param_Bits.bars);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.process_states: %d\n", BannerVmsNodeName, Bnr.Param_Bits.process_states);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.iocnt: %d\n", BannerVmsNodeName, Bnr.Param_Bits.iocnt);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.monitor : %d\n", BannerVmsNodeName, Bnr.Param_Bits.monitor);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.link_window : %d\n", BannerVmsNodeName, 
	Bnr.Param_Bits.link_window);
    BannerSetResource(string, &db, &db1);


    sprintf (string, "DECW$BANNER_%s.process_window : %d\n", BannerVmsNodeName, Bnr.Param_Bits.process_window);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.pe_window: %d\n", BannerVmsNodeName, Bnr.Param_Bits.pe_window);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.lck_window: %d\n", BannerVmsNodeName, Bnr.Param_Bits.lck_window);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.cwps_window: %d\n", BannerVmsNodeName, Bnr.Param_Bits.cwps_window);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.sys_window: %d\n", BannerVmsNodeName, Bnr.Param_Bits.sys_window);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.tp_window: %d\n", BannerVmsNodeName, Bnr.Param_Bits.tp_window);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.tp_max_rate: %d\n", BannerVmsNodeName, Bnr.tp_max_rate);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.tp_rate1_start: %d\n", BannerVmsNodeName, Bnr.tp_rate1_start);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.tp_rate2_start: %d\n", BannerVmsNodeName, Bnr.tp_rate2_start);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.tp_rate3_start: %d\n", BannerVmsNodeName, Bnr.tp_rate3_start);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.tp_rate4_start: %d\n", BannerVmsNodeName, Bnr.tp_rate4_start);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.tp_rate1_end: %d\n", BannerVmsNodeName, Bnr.tp_rate1_end);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.tp_rate2_end: %d\n", BannerVmsNodeName, Bnr.tp_rate2_end);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.tp_rate3_end: %d\n", BannerVmsNodeName, Bnr.tp_rate3_end);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.total_com_rate: %d\n", BannerVmsNodeName, Bnr.Param_Bits.total_com_rate);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.onef_comp_rate: %d\n", BannerVmsNodeName, Bnr.Param_Bits.onef_comp_rate);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.start_rate: %d\n", BannerVmsNodeName, Bnr.Param_Bits.start_rate);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.prep_rate: %d\n", BannerVmsNodeName, Bnr.Param_Bits.prep_rate);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.abort_rate: %d\n", BannerVmsNodeName, Bnr.Param_Bits.abort_rate);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.end_rate: %d\n", BannerVmsNodeName, Bnr.Param_Bits.end_rate);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.rem_start_rate: %d\n", BannerVmsNodeName, Bnr.Param_Bits.rem_start_rate);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.rem_end_rate: %d\n", BannerVmsNodeName, Bnr.Param_Bits.rem_end_rate);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.comp_rate1: %d\n", BannerVmsNodeName, Bnr.Param_Bits.comp_rate1);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.comp_rate2: %d\n", BannerVmsNodeName, Bnr.Param_Bits.comp_rate2);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.comp_rate3: %d\n", BannerVmsNodeName, Bnr.Param_Bits.comp_rate3);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.comp_rate4: %d\n", BannerVmsNodeName, Bnr.Param_Bits.comp_rate4);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.qp_window: %d\n", BannerVmsNodeName, Bnr.Param_Bits.qp_window);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.qb_window: %d\n", BannerVmsNodeName, Bnr.Param_Bits.qb_window);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.clu_window: %d\n", BannerVmsNodeName, Bnr.Param_Bits.clu_window);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.cpu: %d\n", BannerVmsNodeName, Bnr.Param_Bits.cpu_window);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.message_window: %d\n", BannerVmsNodeName, Bnr.Param_Bits.message_window);
    BannerSetResource(string, &db, &db1);
    sprintf (string, "DECW$BANNER_%s.message_size: %d\n", BannerVmsNodeName, BannerMessageGetSize());
    BannerSetResource(string, &db, &db1);
    {
    extern int hide_timer_on;
    if (hide_timer_on)
	{
	sprintf (string, "DECW$BANNER_%s.message_hide_timer: %s\n", BannerVmsNodeName, BannerMessageGetHide());
	BannerSetResource(string, &db, &db1);
	}
    }

    sprintf (string, "DECW$BANNER_%s.modes: %d\n", BannerVmsNodeName, Bnr.Param_Bits.modes_window);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.multi_hist: %d\n", BannerVmsNodeName, Bnr.Param_Bits.cpu_multi_hist);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.hanoi: %d\n", BannerVmsNodeName, Bnr.Param_Bits.hanoi);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.seconds: %d\n", BannerVmsNodeName, Bnr.Param_Bits.seconds);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.digital_clock: %d\n", BannerVmsNodeName, Bnr.Param_Bits.digital_clock);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.analog_clock: %d\n", BannerVmsNodeName, Bnr.Param_Bits.analog_clock);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.all_users: %d\n", BannerVmsNodeName, Bnr.Param_Bits.all_users);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.jobs_on_hold: %d\n", BannerVmsNodeName, Bnr.Param_Bits.jobs_on_hold);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.stopped_queues: %d\n", BannerVmsNodeName, Bnr.Param_Bits.stopped_queues);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.int_jobs: %d\n", BannerVmsNodeName, Bnr.Param_Bits.int_jobs);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.dua0: %d\n", BannerVmsNodeName, Mon.dua0);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.dua1: %d\n", BannerVmsNodeName, Mon.dua1);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.dua2: %d\n", BannerVmsNodeName, Mon.dua2);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.pio: %d\n", BannerVmsNodeName, Mon.pio);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.pf: %d\n", BannerVmsNodeName, Mon.pf);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.sysflt: %d\n", BannerVmsNodeName, Mon.sysflt);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.ni_io: %d\n", BannerVmsNodeName, Mon.ni_io);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.mp: %d\n", BannerVmsNodeName, Mon.mp);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.fp: %d\n", BannerVmsNodeName, Mon.fp);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.bell: %d\n", BannerVmsNodeName, Bnr.Param_Bits.bell);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.bell_volume: %d\n", BannerVmsNodeName, Bnr.bell_volume);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.icon_on: %d\n", BannerVmsNodeName, Bnr.Param_Bits.icon_on);
    BannerSetResource(string, &db, &db1);

    sprintf (string, "DECW$BANNER_%s.base_priority: %d\n", BannerVmsNodeName, Bnr.base_priority);
    BannerSetResource(string, &db, &db1);


/*
 * now write it out
 */
    XrmPutFileDatabase (db, nodedb);
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

    BannerVmsDisplayName = NULL;
    BannerReadResource (1);	/* Application DB */;

    if (Bnr.fnt_name == 0)
	BannerSignalError ("Failed to open Application Xdefault database DECW$SYSTEM_DEFAULTS:DECW$BANNER.DAT",
	    0);
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

    XmMenuPosition (menu_widget, event);
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
	    XmNx,
	    &Bnr.cpu_x);
	BannerGetArg(Bnr.cpu_shell_widget,
	    XmNy,
	    &Bnr.cpu_y);
	}
    if (Bnr.pe_shell_widget != 0)
	{
	BannerGetArg(Bnr.pe_shell_widget,
	    XmNx,
	    &Bnr.pe_x);
	BannerGetArg(Bnr.pe_shell_widget,
	    XmNy,
	    &Bnr.pe_y);
	}
    if (Bnr.lck_shell_widget != 0)
	{
	BannerGetArg(Bnr.lck_shell_widget,
	    XmNx,
	    &Bnr.lck_x);
	BannerGetArg(Bnr.lck_shell_widget,
	    XmNy,
	    &Bnr.lck_y);
	}
    if (Bnr.cwps_shell_widget != 0)
	{
	BannerGetArg(Bnr.cwps_shell_widget,
	    XmNx,
	    &Bnr.cwps_x);
	BannerGetArg(Bnr.cwps_shell_widget,
	    XmNy,
	    &Bnr.cwps_y);
	}
    if (Bnr.sys_shell_widget != 0)
	{
	BannerGetArg(Bnr.sys_shell_widget,
	    XmNx,
	    &Bnr.sys_x);
	BannerGetArg(Bnr.sys_shell_widget,
	    XmNy,
	    &Bnr.sys_y);
	}
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
    if (Bnr.mon_shell_widget != 0)
	{
	BannerGetArg(Bnr.mon_shell_widget,
	    XmNx,
	    &Bnr.mon_x);
	BannerGetArg(Bnr.mon_shell_widget,
	    XmNy,
	    &Bnr.mon_y);
	}
    if (Bnr.link_shell_widget != 0)
	{
	BannerGetArg(Bnr.link_shell_widget,
	    XmNx,
	    &Bnr.link_x);
	BannerGetArg(Bnr.link_shell_widget,
	    XmNy,
	    &Bnr.link_y);
	}
    if (Bnr.process_shell_widget != 0)
	{
	BannerGetArg(Bnr.process_shell_widget,
	    XmNx,
	    &Bnr.process_x);
	BannerGetArg(Bnr.process_shell_widget,
	    XmNy,
	    &Bnr.process_y);
	}
    if (Bnr.message_shell_widget != 0)
	{
	BannerGetArg(Bnr.message_shell_widget,
	    XmNx,
	    &Bnr.message_x);
	BannerGetArg(Bnr.message_shell_widget,
	    XmNy,
	    &Bnr.message_y);
	BannerGetArg(Bnr.message_shell_widget,
	    XmNwidth,
	    &Bnr.message_width);
	BannerGetArg(Bnr.message_shell_widget,
	    XmNheight,
	    &Bnr.message_height);
	}
    if (Bnr.cube_shell_widget != 0)
	{
	BannerGetArg(Bnr.cube_shell_widget,
	    XmNx,
	    &Bnr.cube_x);
	BannerGetArg(Bnr.cube_shell_widget,
	    XmNy,
	    &Bnr.cube_y);
	}
    if (Bnr.hanoi_shell_widget != 0)
	{
	BannerGetArg(Bnr.hanoi_shell_widget,
	    XmNx,
	    &Bnr.hanoi_x);
	BannerGetArg(Bnr.hanoi_shell_widget,
	    XmNy,
	    &Bnr.hanoi_y);
	}
    if (Bnr.globe_shell_widget != 0)
	{
	BannerGetArg(Bnr.globe_shell_widget,
	    XmNx,
	    &Bnr.globe_x);
	BannerGetArg(Bnr.globe_shell_widget,
	    XmNy,
	    &Bnr.globe_y);
	}
    if (Bnr.clock_shell_widget != 0)
	{
	BannerGetArg(Bnr.clock_shell_widget,
	    XmNx,
	    &Bnr.clock_x);
	BannerGetArg(Bnr.clock_shell_widget,
	    XmNy,
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

    BannerDisplayCursor (widget, 0);

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

	MrmFetchWidget (Bnr.drm_prt,
	    "BANNER_SETUP_OPTIONS",
	    *savewidget,
	    &Bnr.setup_widget,
	    &class);


	BannerCheckPopupPos (Bnr.setup_widget, setup_width, setup_height);
	XtManageChild (Bnr.setup_widget);
	XtRealizeWidget (Bnr.setup_widget);
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
	BannerClearWindows ();

	MrmFetchWidget (Bnr.drm_prt,
		"BANNER_RESTORE_MESSAGE",
		*savewidget,
		&message_widget,
		&class);



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
	    MrmFetchWidget (Bnr.drm_prt,
		"BANNER_MESSAGE",
		*savewidget,
		&message_widget,
		&class);


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
	    MrmFetchWidget (Bnr.drm_prt,
		"BANNER_SAVE_MESSAGE",
		*savewidget,
		&message_widget,
		&class);
	else
	    MrmFetchWidget (Bnr.drm_prt,
		"BANNER_NOT_SAVE_MESSAGE",
		*savewidget,
		&message_widget,
		&class);


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
	BannerClearWindows();

	MrmFetchWidget (Bnr.drm_prt,
		"BANNER_RESTORE_MESSAGE",
		*savewidget,
		&message_widget,
		&class);



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
	    MrmFetchWidget (Bnr.drm_prt,
		"BANNER_MESSAGE",
		*savewidget,
		&message_widget,
		&class);



	    BannerCheckPopupPos (message_widget, other_width, other_height);
	    XtManageChild (message_widget);
	    XtRealizeWidget (message_widget);
	    }
	break;
	}
    

    case 6 :		/* hide message window */
	{
	BannerMessageHide ();
	break;
	}
    case 7 :		/* show message window */
	{
	XtMapWidget (Bnr.message_shell_widget);
	break;
	}
    case 8 :		/* Erase message window */
	{
	BannerMessageErase ();
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

	MrmFetchWidget (Bnr.drm_prt,
	    "BANNER_ALARM_OPTIONS",
	    *savewidget,
	    &Bnr.alarm_widget,
	    &class);



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
	    XmNvalue,
	    &hour);	

	BannerGetArg(toggle_vector[banner_minute_widget],
	    XmNvalue,
	    &minute);	

	alarmtext = XmTextGetString (toggle_vector[banner_alarmtext_widget]);

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

	helpwidget = NULL;

	MrmFetchWidget (Bnr.drm_prt,
	    "BANNER_HELP_WIDGET",
	    Bnr.dp_id,
	    &helpwidget,
	    &class);


	if (helpwidget != NULL)
	    {
	    BannerCheckPopupPos (helpwidget, other_width, other_height);
	    XtManageChild (helpwidget);
	    XtRealizeWidget (helpwidget);
	    }
	break;
	}

    case 13:		/* Set min display */
	{


/*
 * get the right windows up
 */
	Bnr.Param_Bits.message_window = Bnr.Param_Bits.modes_window =
	Bnr.Param_Bits.hanoi = Bnr.Param_Bits.world = 
	Bnr.Param_Bits.analog_clock = Bnr.Param_Bits.digital_clock =
	Bnr.Param_Bits.sys_window = Bnr.Param_Bits.cwps_window =
	Bnr.Param_Bits.qp_window = Bnr.Param_Bits.qb_window =
	Bnr.Param_Bits.tp_window = Bnr.Param_Bits.clu_window =
	Bnr.Param_Bits.lck_window = Bnr.Param_Bits.cpu_window =
	Bnr.Param_Bits.process_window = Bnr.Param_Bits.monitor =
	Bnr.Param_Bits.process_window = Bnr.Param_Bits.link_window =
	Bnr.Param_Bits.cube = Bnr.Param_Bits.pe_window = 0;

	BannerSeperateWindows(1);

	break;
	}    
    case 14:		/* Sound Setup */
	{
	BannerSoundSelect (*savewidget);
	break;
	}    

    }

    BannerUndisplayCursor ();

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
    case 3:		/* APPLY */
	{
	int state, width, height, border_changed;
	int cant_change = 0;

	New_Bnr.qp_name = XmTextGetString (toggle_vector[banner_print_queue_name]);
	New_Bnr.qb_name = XmTextGetString (toggle_vector[banner_batch_queue_name]);
	New_Bnr.queue_user = XmTextGetString (toggle_vector[banner_queue_user]);
    
	state = XmTextGetString (toggle_vector[banner_max_comp_rate]);
	New_Bnr.tp_max_rate = BannerStrToNumeric(state);
	XtFree (state);
	state = XmTextGetString (toggle_vector[banner_comp_rate1_start]);
	New_Bnr.tp_rate1_start = BannerStrToNumeric(state);
	XtFree (state);
	state = XmTextGetString (toggle_vector[banner_comp_rate2_start]);
	New_Bnr.tp_rate2_start = BannerStrToNumeric(state);
	XtFree (state);
	state = XmTextGetString (toggle_vector[banner_comp_rate3_start]);
	New_Bnr.tp_rate3_start = BannerStrToNumeric(state);
	XtFree (state);
	state = XmTextGetString (toggle_vector[banner_comp_rate4_start]);
	New_Bnr.tp_rate4_start = BannerStrToNumeric(state);
	XtFree (state);
	state = XmTextGetString (toggle_vector[banner_comp_rate1_end]);
	New_Bnr.tp_rate1_end = BannerStrToNumeric(state);
	XtFree (state);
	state = XmTextGetString (toggle_vector[banner_comp_rate2_end]);
	New_Bnr.tp_rate2_end = BannerStrToNumeric(state);
	XtFree (state);
	state = XmTextGetString (toggle_vector[banner_comp_rate3_end]);
	New_Bnr.tp_rate3_end = BannerStrToNumeric(state);
	XtFree (state);

	New_Bnr.disk_0 = XmTextGetString (toggle_vector[banner_disk_0]);
	New_Bnr.disk_0_name = XmTextGetString (toggle_vector[banner_disk_0_name]);
	New_Bnr.disk_1 = XmTextGetString (toggle_vector[banner_disk_1]);
	New_Bnr.disk_1_name = XmTextGetString (toggle_vector[banner_disk_1_name]);
	New_Bnr.disk_2 = XmTextGetString (toggle_vector[banner_disk_2]);
	New_Bnr.disk_2_name = XmTextGetString (toggle_vector[banner_disk_2_name]);

	BannerGetArg(toggle_vector[banner_tick_size],
	    XmNvalue,
	    &state);	
	New_Bnr.cpu_count = state;

	BannerGetArg(toggle_vector[banner_base_priority],
	    XmNvalue,
	    &state);	
	New_Bnr.base_priority = state;


	New_Bnr.setup_widget = Bnr.setup_widget;

	if (*tag == 1)	    /* Not apply */
	    {
	    XtDestroyWidget (Bnr.setup_widget);
/*
 * Now syncronize with the Server
 */
	    XSync (XtDisplay (Bnr.dp_id), 0);
	    Bnr.setup_widget = New_Bnr.setup_widget = 0;
	    }    

	/*
	 * Now check if anything has changed that we cant handle dynamicly.
	 */
	if (Bnr.Param_Bits.border != New_Bnr.Param_Bits.border ||
	    Bnr.Param_Bits.icon_on != New_Bnr.Param_Bits.icon_on)
	    cant_change = 1;
    
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
	BannerClearWindows ();
/*
 * Now check the priority we should run at
 */
	if (Bnr.Param_Bits.cube || Bnr.Param_Bits.hanoi)
	    Bnr.execute_priority = 0;
	else
	    Bnr.execute_priority = Bnr.base_priority;

	memcpy(&New_Bnr, &Bnr, sizeof(Bnr) );
	memcpy(&New_Mon, &Mon, sizeof(Mon));
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



	    BannerCheckPopupPos (message_widget, other_width, other_height);
	    XtManageChild (message_widget);
	    XtRealizeWidget (message_widget);
	    }

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
    int valuetoset = XmNset;

    switch (*tag)
	     {
	     case banner_font_name_c:
		{
		toggle_vector[banner_font_name_c] = widget;
		if (strcmp(banner_font_name, "ITC Avant Garde Gothic") == 0)
		    BannerSetArg(toggle_vector[banner_font_name_c], 
			XmNmenuHistory, toggle_vector[avantgarde_font]);
		if (strcmp(banner_font_name, "Courier") == 0)
		    BannerSetArg(toggle_vector[banner_font_name_c], 
			XmNmenuHistory, toggle_vector[courier_font]);
		if (strcmp(banner_font_name, "Helvetica") == 0)
		    BannerSetArg(toggle_vector[banner_font_name_c], 
			XmNmenuHistory, toggle_vector[helvetica_font]);
		if (strcmp(banner_font_name, "ITC Lubalin Graph") == 0)
		    BannerSetArg(toggle_vector[banner_font_name_c], 
			XmNmenuHistory, toggle_vector[lubalingraph_font]);
		if (strcmp(banner_font_name, "New Century Schoolbook") == 0)
		    BannerSetArg(toggle_vector[banner_font_name_c], 
			XmNmenuHistory, toggle_vector[ncsschoolbook_font]);
		if (strcmp(banner_font_name, "ITC Souvenir") == 0)
		    BannerSetArg(toggle_vector[banner_font_name_c], 
			XmNmenuHistory, toggle_vector[souvenir_font]);
		if (strcmp(banner_font_name, "Times") == 0)
		    BannerSetArg(toggle_vector[banner_font_name_c], 
			XmNmenuHistory, toggle_vector[times_font]);
		state=0;
		break;
		}
	     case banner_font_size_c:
		{
		toggle_vector[banner_font_size_c] = widget;
		if (strcmp(banner_font_size, "8") == 0)
		    BannerSetArg(toggle_vector[banner_font_size_c], 
			XmNmenuHistory, toggle_vector[a8_point]);
		if (strcmp(banner_font_size, "10") == 0)
		    BannerSetArg(toggle_vector[banner_font_size_c], 
			XmNmenuHistory, toggle_vector[a10_point]);
		if (strcmp(banner_font_size, "12") == 0)
		    BannerSetArg(toggle_vector[banner_font_size_c], 
			XmNmenuHistory, toggle_vector[a12_point]);
		if (strcmp(banner_font_size, "14") == 0)
		    BannerSetArg(toggle_vector[banner_font_size_c], 
			XmNmenuHistory, toggle_vector[a14_point]);
		if (strcmp(banner_font_size, "18") == 0)
		    BannerSetArg(toggle_vector[banner_font_size_c], 
			XmNmenuHistory, toggle_vector[a18_point]);
		if (strcmp(banner_font_size, "24") == 0)
		    BannerSetArg(toggle_vector[banner_font_size_c], 
			XmNmenuHistory, toggle_vector[a24_point]);
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
		BannerSetArg (widget, XmNforeground, Bnr.foreground);
		BannerSetArg (widget, XmNbackground, Bnr.background);
		state=0;
		break;
		}
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
	     case banner_disk_0 :
		{
		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
		    BannerSetArg(widget, 
			XmNsensitive,
			0);
		toggle_vector[banner_disk_0] = widget;
		XmTextSetString (widget, Bnr.disk_0);
		state=0;
		break;
		}
	     case banner_disk_0_name :
		{
		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
		    BannerSetArg(widget, 
			XmNsensitive,
			0);
		toggle_vector[banner_disk_0_name] = widget;
		XmTextSetString (widget, Bnr.disk_0_name);
		state=0;
		break;
		}
	     case banner_disk_1 :
		{
		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
		    BannerSetArg(widget, 
			XmNsensitive,
			0);
		toggle_vector[banner_disk_1] = widget;
		XmTextSetString (widget, Bnr.disk_1);
		state=0;
		break;
		}
	     case banner_disk_1_name :
		{
		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
		    BannerSetArg(widget, 
			XmNsensitive,
			0);
		toggle_vector[banner_disk_1_name] = widget;
		XmTextSetString (widget, Bnr.disk_1_name);
		state=0;
		break;
		}
	     case banner_disk_2 :
		{
		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
		    BannerSetArg(widget, 
			XmNsensitive,
			0);
		toggle_vector[banner_disk_2] = widget;
		XmTextSetString (widget, Bnr.disk_2);
		state=0;
		break;
		}
	     case banner_disk_2_name :
		{
		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
		    BannerSetArg(widget, 
			XmNsensitive,
			0);
		toggle_vector[banner_disk_2_name] = widget;
		XmTextSetString (widget, Bnr.disk_2_name);
		state=0;
		break;
		}
	     case banner_hour_widget :
		{
		toggle_vector[banner_hour_widget] = widget;
		state=12;
		valuetoset = XmNvalue;
		break;
		}
	     case banner_minute_widget :
		{
		toggle_vector[banner_minute_widget] = widget;
		state=0;
		valuetoset = XmNvalue;
		break;
		}
	     case banner_alarmtext_widget :
		{
		toggle_vector[banner_alarmtext_widget] = widget;
		state="Alarm Message";
		valuetoset = XmNvalue;
		break;
		}
	     case banner_tick_size :
		{
		toggle_vector[banner_tick_size] = widget;
		state = New_Bnr.cpu_update;
		valuetoset = XmNvalue;
		break;
		}
	     case banner_base_priority :
		{
		toggle_vector[banner_base_priority] = widget;
		state = New_Bnr.base_priority;
		valuetoset = XmNvalue;
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
		    XmNsensitive,
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
		    XmNsensitive,
		    New_Bnr.Param_Bits.digital_clock ||
		    New_Bnr.Param_Bits.analog_clock);
		break;
		}
	     case banner_freeiocnt :
		{
		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
		    BannerSetArg(widget, 
			XmNsensitive,
			0);
		toggle_vector[banner_freeiocnt] = widget;
		state = New_Bnr.Param_Bits.freeiocnt;
		break;
		}
	     case banner_freeblkcnt :
		{
		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
		    BannerSetArg(widget, 
			XmNsensitive,
			0);
		toggle_vector[banner_freeblkcnt] = widget;
		state = New_Bnr.Param_Bits.freeblkcnt;
		break;
		}
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
		    BannerSetArg (widget, XmNsensitive, 0);
		break;
		}
	     case banner_multi_hist_toggle :
		{
		toggle_vector[banner_multi_hist_toggle] = widget;
		state = New_Bnr.Param_Bits.cpu_multi_hist;
		if (New_Bnr.Param_Bits.cpu_window == 0)
		    BannerSetArg (widget, XmNsensitive, 0);
		break;
		}
	     case banner_message_toggle :
		{
		toggle_vector[banner_message_toggle] = widget;
		state = New_Bnr.Param_Bits.message_window;
		break;
		}
	     case banner_mon_window_toggle :
		{
		toggle_vector[banner_mon_window_toggle] = widget;
		state = New_Bnr.Param_Bits.monitor;
		break;
		}
	     case banner_link_window_toggle :
		{
		toggle_vector[banner_link_window_toggle] = widget;
		state = New_Bnr.Param_Bits.link_window;
		break;
		}
	     case banner_sys_window_toggle :
		{
		toggle_vector[banner_sys_window_toggle] = widget;
		state = New_Bnr.Param_Bits.sys_window;
		break;
		}
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
		if (New_Bnr.Param_Bits.tp_window == 0)
		    BannerSetArg (widget, XmNsensitive, 0);
		break;
		}
	     case banner_tp_1fcomprate :
		{
		toggle_vector[banner_tp_1fcomprate] = widget;
		state = New_Bnr.Param_Bits.onef_comp_rate;
		if (New_Bnr.Param_Bits.tp_window == 0)
		    BannerSetArg (widget, XmNsensitive, 0);
		break;
		}
	     case banner_tp_startcomprate :
		{
		toggle_vector[banner_tp_startcomprate] = widget;
		state = New_Bnr.Param_Bits.start_rate;
		if (New_Bnr.Param_Bits.tp_window == 0)
		    BannerSetArg (widget, XmNsensitive, 0);
		break;
		}
	     case banner_tp_prepcomprate :
		{
		toggle_vector[banner_tp_prepcomprate] = widget;
		state = New_Bnr.Param_Bits.prep_rate;
		if (New_Bnr.Param_Bits.tp_window == 0)
		    BannerSetArg (widget, XmNsensitive, 0);
		break;
		}
	     case banner_tp_abortcomprate :
		{
		toggle_vector[banner_tp_abortcomprate] = widget;
		state = New_Bnr.Param_Bits.abort_rate;
		if (New_Bnr.Param_Bits.tp_window == 0)
		    BannerSetArg (widget, XmNsensitive, 0);
		break;
		}
	     case banner_tp_endcomprate :
		{
		toggle_vector[banner_tp_endcomprate] = widget;
		state = New_Bnr.Param_Bits.end_rate;
		if (New_Bnr.Param_Bits.tp_window == 0)
		    BannerSetArg (widget, XmNsensitive, 0);
		break;
		}
	     case banner_tp_remstartcomprate :
		{
		toggle_vector[banner_tp_remstartcomprate] = widget;
		state = New_Bnr.Param_Bits.rem_start_rate;
		if (New_Bnr.Param_Bits.tp_window == 0)
		    BannerSetArg (widget, XmNsensitive, 0);
		break;
		}
	     case banner_tp_remendcomprate :
		{
		toggle_vector[banner_tp_remendcomprate] = widget;
		state = New_Bnr.Param_Bits.rem_end_rate;
		if (New_Bnr.Param_Bits.tp_window == 0)
		    BannerSetArg (widget, XmNsensitive, 0);
		break;
		}
	     case banner_tp_comprate1 :
		{
		toggle_vector[banner_tp_comprate1] = widget;
		state = New_Bnr.Param_Bits.comp_rate1;
		if (New_Bnr.Param_Bits.tp_window == 0)
		    BannerSetArg (widget, XmNsensitive, 0);
		break;
		}
	     case banner_tp_comprate2 :
		{
		toggle_vector[banner_tp_comprate2] = widget;
		state = New_Bnr.Param_Bits.comp_rate2;
		if (New_Bnr.Param_Bits.tp_window == 0)
		    BannerSetArg (widget, XmNsensitive, 0);
		break;
		}
	     case banner_tp_comprate3 :
		{
		toggle_vector[banner_tp_comprate3] = widget;
		state = New_Bnr.Param_Bits.comp_rate3;
		if (New_Bnr.Param_Bits.tp_window == 0)
		    BannerSetArg (widget, XmNsensitive, 0);
		break;
		}
	     case banner_tp_comprate4 :
		{
		toggle_vector[banner_tp_comprate4] = widget;
		state = New_Bnr.Param_Bits.comp_rate4;
		if (New_Bnr.Param_Bits.tp_window == 0)
		    BannerSetArg (widget, XmNsensitive, 0);
		break;
		}

	     case banner_max_comp_rate :
		{
		char	text[20];
		sprintf(text, "%d", Bnr.tp_max_rate);
		XmTextSetString (widget,text);
		toggle_vector[banner_max_comp_rate] = widget;
		state = 0;
		if (New_Bnr.Param_Bits.tp_window == 0)
		    BannerSetArg (widget, XmNsensitive, 0);
		break;
		}

	     case banner_comp_rate1_start :
		{
		char	text[20];
		sprintf(text, "%d", Bnr.tp_rate1_start);
		XmTextSetString (widget,text);
		toggle_vector[banner_comp_rate1_start] = widget;
		state = 0;
		if (New_Bnr.Param_Bits.tp_window == 0)
		    BannerSetArg (widget, XmNsensitive, 0);
		break;
		}

	     case banner_comp_rate2_start :
		{
		char	text[20];
		sprintf(text, "%d", Bnr.tp_rate2_start);
		XmTextSetString (widget,text);
		toggle_vector[banner_comp_rate2_start] = widget;
		state = 0;
		if (New_Bnr.Param_Bits.tp_window == 0)
		    BannerSetArg (widget, XmNsensitive, 0);
		break;
		}

	     case banner_comp_rate3_start :
		{
		char	text[20];
		sprintf(text, "%d", Bnr.tp_rate3_start);
		XmTextSetString (widget,text);
		toggle_vector[banner_comp_rate3_start] = widget;
		state = 0;
		if (New_Bnr.Param_Bits.tp_window == 0)
		    BannerSetArg (widget, XmNsensitive, 0);
		break;
		}

	     case banner_comp_rate4_start :
		{
		char	text[20];
		sprintf(text, "%d", Bnr.tp_rate4_start);
		XmTextSetString (widget,text);
		toggle_vector[banner_comp_rate4_start] = widget;
		state = 0;
		if (New_Bnr.Param_Bits.tp_window == 0)
		    BannerSetArg (widget, XmNsensitive, 0);
		break;
		}

	     case banner_comp_rate1_end :
		{
		char	text[20];
		sprintf(text, "%d", Bnr.tp_rate1_end);
		XmTextSetString (widget,text);
		toggle_vector[banner_comp_rate1_end] = widget;
		state = 0;
		if (New_Bnr.Param_Bits.tp_window == 0)
		    BannerSetArg (widget, XmNsensitive, 0);
		break;
		}

	     case banner_comp_rate2_end :
		{
		char	text[20];
		sprintf(text, "%d", Bnr.tp_rate2_end);
		XmTextSetString (widget,text);
		toggle_vector[banner_comp_rate2_end] = widget;
		state = 0;
		if (New_Bnr.Param_Bits.tp_window == 0)
		    BannerSetArg (widget, XmNsensitive, 0);
		break;
		}

	     case banner_comp_rate3_end :
		{
		char	text[20];
		sprintf(text, "%d", Bnr.tp_rate3_end);
		XmTextSetString (widget,text);
		toggle_vector[banner_comp_rate3_end] = widget;
		state = 0;
		if (New_Bnr.Param_Bits.tp_window == 0)
		    BannerSetArg (widget, XmNsensitive, 0);
		break;
		}

	     case banner_tp :
		{
		toggle_vector[banner_tp] = widget;
		state = New_Bnr.Param_Bits.tp_window;
		break;
		}
	     case banner_process_states :
		{
		toggle_vector[banner_process_states] = widget;
		state = New_Bnr.Param_Bits.process_states;
		if (New_Bnr.Param_Bits.process_window == 0)
		    BannerSetArg (widget, XmNsensitive, 0);
		break;
		}
	     case banner_process_name :
		{
		toggle_vector[banner_process_name] = widget;
		state = New_Bnr.Param_Bits.process_name;
		if (New_Bnr.Param_Bits.process_window == 0)
		    BannerSetArg (widget, XmNsensitive, 0);
		break;
		}
	     case banner_process_user :
		{
		toggle_vector[banner_process_user] = widget;
		state = New_Bnr.Param_Bits.process_user;
		if (New_Bnr.Param_Bits.process_window == 0)
		    BannerSetArg (widget, XmNsensitive, 0);
		break;
		}
	     case banner_process_pid :
		{
		toggle_vector[banner_process_pid] = widget;
		state = New_Bnr.Param_Bits.process_pid;
		if (New_Bnr.Param_Bits.process_window == 0)
		    BannerSetArg (widget, XmNsensitive, 0);
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

	     case banner_dua0_active :
		{
		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
		    BannerSetArg(widget, 
			XmNsensitive,
			0);
		toggle_vector[banner_dua0_active] = widget;
		state = New_Mon.dua0;
		break;
		}
	     case banner_dua1_active :
		{
		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
		    BannerSetArg(widget, 
			XmNsensitive,
			0);
		toggle_vector[banner_dua1_active] = widget;
		state = New_Mon.dua1;
		break;
		}
	     case banner_dua2_active :
		{
		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
		    BannerSetArg(widget, 
			XmNsensitive,
			0);
		toggle_vector[banner_dua2_active] = widget;
		state = New_Mon.dua2;
		break;
		}
	     case banner_page_io_active :
		{
		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
		    BannerSetArg(widget, 
			XmNsensitive,
			0);
		toggle_vector[banner_page_io_active] = widget;
		state = New_Mon.pio;
		break;
		}
	     case banner_page_fault_active :
		{
		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
		    BannerSetArg(widget, 
			XmNsensitive,
			0);
		toggle_vector[banner_page_fault_active] = widget;
		state = New_Mon.pf;
		break;
		}
	     case banner_sysfaults_active :
		{
		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
		    BannerSetArg(widget, 
			XmNsensitive,
			0);
		toggle_vector[banner_sysfaults_active] = widget;
		state = New_Mon.sysflt;
		break;
		}
	     case banner_ni_io_active :
		{
		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
		    BannerSetArg(widget, 
			XmNsensitive,
			0);
		toggle_vector[banner_ni_io_active] = widget;
		state = New_Mon.ni_io;
		break;
		}
	     case banner_modified_pages_active :
		{
		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
		    BannerSetArg(widget, 
			XmNsensitive,
			0);
		toggle_vector[banner_modified_pages_active] = widget;
		state = New_Mon.mp;
		break;
		}
	     case banner_free_pages_active :
		{
		if (!New_Bnr.Param_Bits.monitor && !New_Bnr.Param_Bits.sys_window)
		    BannerSetArg(widget, 
			XmNsensitive,
			0);
		toggle_vector[banner_free_pages_active] = widget;
		state = New_Mon.fp;
		break;
		}
    	     }
    BannerSetArg(widget, valuetoset, state);
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
		BannerGetArg (widget, XmNvalue, &state);
		New_Bnr.cpu_update = state;
		break;
		}
	     case banner_base_priority :
		{
		BannerGetArg (widget, XmNvalue, &state);
		New_Bnr.base_priority = state;
		break;
		}
	     case banner_border_active :
		{
		New_Bnr.Param_Bits.border = !New_Bnr.Param_Bits.border;
		BannerSetArg (toggle_vector[banner_icon_active],
		    XmNsensitive,
		    New_Bnr.Param_Bits.border);
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
		    XmNsensitive,
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
	     case banner_cpu_window_toggle :
		{
		New_Bnr.Param_Bits.cpu_window = !New_Bnr.Param_Bits.cpu_window;
		BannerSetArg (toggle_vector[banner_modes_toggle],
		    XmNsensitive,
		    New_Bnr.Param_Bits.cpu_window);
		BannerSetArg (toggle_vector[banner_multi_hist_toggle],
		    XmNsensitive,
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
			XmNsensitive,
		        New_Bnr.Param_Bits.message_window);

		break;
		}
	     case banner_process_window_toggle :
		{
		New_Bnr.Param_Bits.process_window = !New_Bnr.Param_Bits.process_window;
		BannerSetArg (toggle_vector[banner_process_states], 
		    XmNsensitive, New_Bnr.Param_Bits.process_window);
		BannerSetArg (toggle_vector[banner_process_name], 
		    XmNsensitive, New_Bnr.Param_Bits.process_window);
		BannerSetArg (toggle_vector[banner_process_user], 
		    XmNsensitive, New_Bnr.Param_Bits.process_window);
		BannerSetArg (toggle_vector[banner_process_pid], 
		    XmNsensitive, New_Bnr.Param_Bits.process_window);
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

    	     case banner_link_window_toggle :
		{
		New_Bnr.Param_Bits.link_window = !New_Bnr.Param_Bits.link_window;
		break;
		}

    	     case banner_mon_window_toggle :
    	     case banner_sys_window_toggle :
		{
		int state;

		if (*tag == banner_mon_window_toggle)  
		    New_Bnr.Param_Bits.monitor = !New_Bnr.Param_Bits.monitor;
		else
		    New_Bnr.Param_Bits.sys_window = !New_Bnr.Param_Bits.sys_window;
		state = New_Bnr.Param_Bits.monitor | New_Bnr.Param_Bits.sys_window;

		BannerSetArg (toggle_vector[banner_dua0_active], 
		    XmNsensitive,
		    state);
		BannerSetArg (toggle_vector[banner_dua1_active], 
		    XmNsensitive,
		    state);
		BannerSetArg (toggle_vector[banner_dua2_active], 
		    XmNsensitive,
		    state);
		BannerSetArg (toggle_vector[banner_page_io_active], 
		    XmNsensitive,
		    state);
		BannerSetArg (toggle_vector[banner_page_fault_active], 
		    XmNsensitive,
		    state);
		BannerSetArg (toggle_vector[banner_sysfaults_active], 
		    XmNsensitive,
		    state);
		BannerSetArg (toggle_vector[banner_ni_io_active], 
		    XmNsensitive,
		    state);
		BannerSetArg (toggle_vector[banner_modified_pages_active], 
		    XmNsensitive,
		    state);
		BannerSetArg (toggle_vector[banner_free_pages_active], 
		    XmNsensitive,
		    state);
		BannerSetArg (toggle_vector[banner_freeiocnt], 
		    XmNsensitive,
		    state);
		BannerSetArg (toggle_vector[banner_freeblkcnt], 
		    XmNsensitive,
		    state);

		BannerSetArg (toggle_vector[banner_disk_0], 
		    XmNsensitive,
		    state);
		BannerSetArg (toggle_vector[banner_disk_0_name], 
		    XmNsensitive,
		    state);
		BannerSetArg (toggle_vector[banner_disk_1], 
		    XmNsensitive,
		    state);
		BannerSetArg (toggle_vector[banner_disk_1_name], 
		    XmNsensitive,
		    state);
		BannerSetArg (toggle_vector[banner_disk_2], 
		    XmNsensitive,
		    state);
		BannerSetArg (toggle_vector[banner_disk_2_name], 
		    XmNsensitive,
		    state);
		break;
		}

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
	     case banner_analog_window_toggle :
		{
		New_Bnr.Param_Bits.analog_clock = 
		    !New_Bnr.Param_Bits.analog_clock;		
		BannerSetArg (toggle_vector[banner_seconds_active], 
		    XmNsensitive,
		    New_Bnr.Param_Bits.digital_clock ||
		    New_Bnr.Param_Bits.analog_clock);
		break;
		}
	     case banner_digital_window_toggle :
		{
		New_Bnr.Param_Bits.digital_clock = !New_Bnr.Param_Bits.digital_clock;		
		BannerSetArg (toggle_vector[banner_seconds_active], 
		    XmNsensitive,
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
	XDefineCursor (XtDisplay (Bnr.mon_widget), XtWindow(Bnr.mon_widget), 
	    cursors[cursor]);
	XDefineCursor (XtDisplay (Bnr.link_widget), XtWindow(Bnr.link_widget), 
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
	XUndefineCursor (XtDisplay (Bnr.mon_widget), XtWindow(Bnr.mon_widget));
	XUndefineCursor (XtDisplay (Bnr.link_widget), XtWindow(Bnr.link_widget));
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
