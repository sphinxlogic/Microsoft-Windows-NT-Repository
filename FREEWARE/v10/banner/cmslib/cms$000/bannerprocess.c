 #module BannerProcess "V1.0"
 
 /*
 **++
 **  FACILITY:
 **
 **      The DECwindow Banner program
 **
 **  ABSTRACT:
 **
 **      This rouitne controls the display of the Top Cpu processes.
 **
 **  AUTHORS:
 **
 **      Jim Saunders
 **
 **
 **  CREATION DATE:     1st Aug 1988
 **
 **  MODIFICATION HISTORY:
 **--
 **/
 
*11D
 #include <stdio.h>
 #include <decw$include/DECwDwtWidgetProg.h>
*11E
*11I
 
 #include "stdio.h"
 #include "MrmAppl.h"
 
*11E
 #include "Banner"
*11D
 #include <jpidef.h>
 #include <syidef.h>
 #include <ssdef.h>
*11E
*11I
 
 #include "jpidef.h"
 #include "syidef.h"
 #include "ssdef.h"
 
*11E
 
 
 #define SCH$C_COLPG  1                /* COLLIDED PAGE WAIT */
 #define SCH$C_MWAIT  2                /* MUTEX AND MISCELLANEOUS RESOURCE WAIT */
 #define SCH$C_CEF  3                  /* COMMON EVENT FLAG WAIT STATE */
 #define SCH$C_PFW  4                  /* PAGE FAULT WAIT */
 #define SCH$C_LEF  5                  /* LOCAL EVENT FLAG WAIT */
 #define SCH$C_LEFO  6                 /* LOCAL EVENT FLAG WAIT OUT OF BALANCE SET */
 #define SCH$C_HIB  7                  /* HIBERNATE WAIT */
 #define SCH$C_HIBO  8                 /* HIBERNATE WAIT OUT OF BALANCE SET */
 #define SCH$C_SUSP  9                 /* SUSPENDED */
 #define SCH$C_SUSPO  10               /* SUSPENDED OUT OF THE BALANCE SET */
 #define SCH$C_FPG  11                 /* FREEPAGE WAIT */
 #define SCH$C_COM  12                 /* COMPUTE, IN BALANCE SET STATE */
 #define SCH$C_COMO  13                /* COMPUTE, OUT OF BALANCE SET STATE */ 
 #define SCH$C_CUR  14                 /* CURRENT PROCESS STATE */
 
 
 /*
 **
 **  MACRO DEFINITIONS
 **
 **/
 
 
 
 typedef struct _vms_item_list {
 	short	 buffer_length;
 	short	 item_code;
 	void	*buffer;
 	int	*returned_buffer_length;
 	} VMS_ItemList;
 
 
 typedef struct {
     char *name;
     char *user;
     int  resident;
     int  pid;
     int  mode;
     int  seen;
*5D
     int  name_changed;
*5E
*5I
     short int  name_changed;
     short int  highlighted;
*5E
     float  display_size;
     int  percent;
     float  bufcnt;
     float  diff_bufcnt;
     float  old_bufcnt;
     float  diocnt;
     float  diff_diocnt;
     float  old_diocnt;
     float  cputim;
     float  diff_cputim;
     float  old_cputim;
     float  pageflts;
     float  diff_pageflts;
     float  old_pageflts;
     } Bnr$Process;
 
 static void PaintDisplay ();
 static void GetNewInfo ();
 /*
  * now our runtime data structures
  */
 
*12I
 extern	 Son$_Blk    Son;
*12E
 extern	 Bnr$_Blk    Bnr;
 extern	 Clk$_Blk    Clk;
 extern	 GC	     BannerGC;
 extern	 XGCValues   GcValues;
*13I
*14D
 extern   char	     *BannerVmsDisplayName[];
*14E
*14I
 extern   char	     *BannerVmsDisplayName;
*14E
*13E
 extern   char	     BannerVmsNodeName[];
 extern   char	     BannerVmsVersion[];
 
 static int lef, hib, com, pfw, mwait, lefo, hibo, como, cef, colpg, fpg, others;
 static int old_lef, old_hib, old_com, old_pfw, old_mwait, old_lefo, old_hibo, 
 	   old_como, old_others, old_cef, old_colpg, old_fpg;
*5I
 static int old_lef1, old_hib1, old_com1, old_pfw1, old_mwait1, old_lefo1, 
 	   old_hibo1, old_como1, old_others1, old_cef1, old_colpg1, old_fpg1;
*5E
 
 static int total_states_width, states_col_1, states_name_2, states_col_2,
 	states_num_width;
 
 static int old_net, old_batch, old_interactive, old_oswap, old_total;
 
*5I
 static int mwait_x, mwait_y;
*5E
 static int percent;
 static int name_width;
 static int pid_width;
 static int user_width;
 static int our_pid = 0;
 static int null_pid = 0;
 static int num_oswap = 0;
 static int num_process = 0;
 static int num_interactive = 0;
 static int num_batch = 0;
 static int num_sub = 0;
 static int num_net = 0;
 static int num_total = 0;
 static int process_display = 0;
 static int (*processes)[] = NULL;
 static int (*top_processes)[] = NULL;
 static int (*oldtop_processes)[] = NULL;
 static Bnr$Process *process;
 
 static int process_init = 0;
 static int process_update;
 static int process_update_count;
 
 static float total_cpu;
 static int display_size;
 
 static char name[20];
 static char user[20];
 static int  pid, pageflts, diocnt, bufcnt, cputim, mode, sts, state; 
 static VMS_ItemList modeitemlist[7] = {
     4,  JPI$_PID, &pid, 0,
     4,  JPI$_MODE, &mode, 0,
     4,  JPI$_STATE, &state, 0,
     4,  JPI$_STS, &sts, 0,
     20, JPI$_PRCNAM, &name, 0,
     20, JPI$_USERNAME, &user, 0,
     0,  0, 0, 0};
 static VMS_ItemList itemlist[5] = {
     4,  JPI$_PAGEFLTS, &pageflts, 0,
     4,	JPI$_DIOCNT, &diocnt, 0,
     4,  JPI$_BIOCNT, &bufcnt, 0,
     4,	JPI$_CPUTIM, &cputim, 0,
     0,  0, 0, 0};
 
 
 typedef struct _cpudata {
 char fill_1[632];
 int cpu$l_kernel;
 int cpu$l_executive;
 int cpu$l_super;
 int cpu$l_user;
 int cpu$l_interrupt;
 int cpu$l_compatibility;
 int cpu$l_spinlock;
 int cpu$l_nullcpu;
 } cpu$data;
 
 static int BannerActiveCpuCount;
 static int BannerLastActiveCpuCount;
     
 globalref cpu$data *SMP$GL_CPU_DATA[];	    /* CPU data pointer array */
 globalref int SMP$GL_ACTIVE_CPUS;	    /* Active cpu mask */
 
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **	BannerProcessSetup ()
 **
 **      This routine does all the one only setup for the Processes display, 
 **	it is called at initilialisation time, and once every time the display
 **	changes size.
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
 BannerProcessSetup()
 {
 int status, i, old_size;
 /*
  * First get our PID, so we can reconise ourself
  */
 
     if ( (status = SYS$GETJPIW (NULL, NULL, NULL, &itemlist, NULL, NULL, NULL))
 	 != 1)
 	BannerSignalError ("Failed to get the time for SYS$GETJPI service",
 	    status);
 
     our_pid = pid;
 
 
     percent = XTextWidth (Bnr.font_ptr, " 999%", 5); 
     pid_width = XTextWidth (Bnr.font_ptr, "9999XXXX", 8); 
 
     if (Bnr.Param_Bits.process_states)
 	{
 	states_num_width = XTextWidth (Bnr.font_ptr, "99", 2);
 	states_col_1 = XTextWidth (Bnr.font_ptr, "MWAIT  ", 7);
 	states_name_2 = states_col_1 + states_num_width +
 	    Bnr.font_width;
 	states_col_2 = states_name_2 + XTextWidth (Bnr.font_ptr, "Others  ", 8);
 	total_states_width =  states_col_2 + states_num_width + Bnr.font_width;
 	}
     else
 	total_states_width = 0;
 /*
  * now register the top process
  */
 
     old_size = process_display;
     process_display = (Bnr.process_height/Bnr.font_height) - 2;
 
     if (process_display <= 0)
 	process_display = 1;
 	    
     if (old_size == 0)
 	{
 	top_processes = XtMalloc (sizeof(int) * process_display);
 	oldtop_processes = XtMalloc (sizeof(int) * process_display);
 	}
     else
 	{
 	XtFree (top_processes);
 	top_processes = XtMalloc (sizeof(int) * process_display);   
 	XtFree (oldtop_processes);
 	oldtop_processes = XtMalloc (sizeof(int) * process_display);   
 	}
 
     for (i=0; i<process_display; i++)
 	(*top_processes)[i] = NULL;    
 
 /*
  * clear all fields in the current pocess count
  */
     user_width = 0;
     name_width = 0;
     for (i=0; i<num_process; i++)
 	{
 	process = (*processes)[i];
 	process->pid = -1;
 	}
 /*
  * Now make an initial call to get all the process.
  */
     GetNewInfo ();
 
*12D
     old_total = 0;
*12E
*12I
     old_interactive = 0;
*12E
 
     process_update = 1;
     if (Bnr.process_update == 0)
 	process_update_count = Bnr.cpu_update;
     else
 	process_update_count = Bnr.process_update;
 
     process_init = 1;
 }
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **	BannerProcessRefresh ()
 **
 **      This routine refreshes all the static information about the current
 **	processes display. It is called each time the screen needs repainting.
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
 BannerProcessRefresh()
 {
 int x, y, i, name_width;
 
 
 /*
  * check we are initialised
  */
     if (!process_init)
 	BannerProcessSetup ();
 /*
*10D
  * First display our node name etc.
  */
 	BannerWriteText (XtDisplay(Bnr.process_widget), XtWindow(Bnr.process_widget),
 	    0, Bnr.font_height, BannerVmsNodeName, 2, 
 	    Bnr.process_width);
 
 /*
*10E
  * clear memory so we will repaint all the display.
  */
     for (i=0; i<process_display; i++)
 	(*oldtop_processes)[i] = 0;
 
 /*
  *Paint the process_states
  */
     if (Bnr.Param_Bits.process_states)
 	{
         old_lef = old_hib = old_com = old_pfw = old_mwait = old_lefo = 
 	    old_hibo = old_como = old_others = old_cef = old_colpg =
 	    old_fpg = -1;
 
 	y = 3*Bnr.font_height + 2;
 	x = Bnr.process_width - total_states_width;
 
 	BannerWriteText (XtDisplay(Bnr.process_widget), XtWindow(Bnr.process_widget),
 	    x, y, "LEF", 1, 
 	    states_col_1);
 	BannerWriteText (XtDisplay(Bnr.process_widget), XtWindow(Bnr.process_widget),
 	    x + states_name_2, y, "LEFO", 1, 
 	    states_col_2 - states_name_2);
 	y = y + Bnr.font_height;
 
 	BannerWriteText (XtDisplay(Bnr.process_widget), XtWindow(Bnr.process_widget),
 	    x, y, "HIB", 1, 
 	    states_col_1);
 	BannerWriteText (XtDisplay(Bnr.process_widget), XtWindow(Bnr.process_widget),
 	    x + states_name_2, y, "HIBO", 1, 
 	    states_col_2 - states_name_2);
 	y = y + Bnr.font_height;
 
 	BannerWriteText (XtDisplay(Bnr.process_widget), XtWindow(Bnr.process_widget),
 	    x, y, "COM", 1, 
 	    states_col_1);
 	BannerWriteText (XtDisplay(Bnr.process_widget), XtWindow(Bnr.process_widget),
 	    x + states_name_2, y, "COMO", 1, 
 	    states_col_2 - states_name_2);
 	y = y + Bnr.font_height;
 
 	BannerWriteText (XtDisplay(Bnr.process_widget), XtWindow(Bnr.process_widget),
 	    x, y, "PFW", 1, 
 	    states_col_1);
 	BannerWriteText (XtDisplay(Bnr.process_widget), XtWindow(Bnr.process_widget),
 	    x + states_name_2, y, "CEF", 1, 
 	    states_col_2 - states_name_2);
 	y = y + Bnr.font_height;
 
 	BannerWriteText (XtDisplay(Bnr.process_widget), XtWindow(Bnr.process_widget),
 	    x, y, "COLPG", 1, 
 	    states_col_1);
 	BannerWriteText (XtDisplay(Bnr.process_widget), XtWindow(Bnr.process_widget),
 	    x + states_name_2, y, "FPG", 1, 
 	    states_col_2 - states_name_2);
 	y = y + Bnr.font_height;
 
*5I
 	mwait_x = x;
 	mwait_y = y;
 
*5E
 	BannerWriteText (XtDisplay(Bnr.process_widget), XtWindow(Bnr.process_widget),
 	    x, y, "MWAIT", 1, 
 	    states_col_1);
*5I
 
*5E
 	BannerWriteText (XtDisplay(Bnr.process_widget), XtWindow(Bnr.process_widget),
 	    x + states_name_2, y, "Others", 1, 
 	    states_col_2 - states_name_2);
 	y = y + Bnr.font_height;
 	
 	}
 /*
  * paint the rest of the screen
  */
*12D
     old_total = 0;
*12E
*12I
     old_interactive = 0;
*12E
 
     PaintDisplay ();
 }
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **	PaintDisplay ()
 **
 **      This routine paints the display for the TOP processes
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
 static int hist;
 static void	
 PaintDisplay()
 {
 int x, x1, y, i;
*2D
 char buffer[] = "                                         "; 
*2E
*2I
 char buffer[] = "                                                             "; 
*2E
*8I
*10I
 int uptime[2];
 int curtime[2];
 int uplen;
 char upstr [15];
 int updesc[2] = {13, upstr};
 char nodebuf[50];
*10E
 static title_highlighted = 0;
*8E
*10I
 VMS_ItemList sysitem[2] = {
     8, SYI$_BOOTTIME, &uptime, &uplen, 0, 0, 0, 0}; 
 
 
 /*
  * First display our node name, and system uptime.
  */
 	SYS$GETSYI (NULL, NULL, NULL, &sysitem, NULL, NULL, NULL);
 	memset (upstr, 0, sizeof(upstr));
 	SYS$GETTIM (&curtime);
 	LIB$SUBX(&uptime, &curtime, &uptime, &2); 
 	SYS$ASCTIM (0, &updesc, &uptime, 0);
 	sprintf (nodebuf, "%s  (Uptime =%s)", BannerVmsNodeName, upstr);
  
*13D
 	BannerWriteText (XtDisplay(Bnr.process_widget), XtWindow(Bnr.process_widget),
 	    0, Bnr.font_height, nodebuf, 2, 
 	    Bnr.process_width);
*13E
*10E
*13D
 
*13E
*13I
 	if (BannerVmsDisplayName == NULL)
 	    {
 	    BannerWriteText (XtDisplay(Bnr.process_widget), XtWindow(Bnr.process_widget),
 		0, Bnr.font_height, nodebuf, 2, 
 		Bnr.process_width);
 	    }
 	else
 	    {
 	    BannerWriteText (XtDisplay(Bnr.process_widget), XtWindow(Bnr.process_widget),
 		0, Bnr.font_height, BannerVmsDisplayName, 2, 
 		Bnr.process_width);
 	    }
*13E
 /*
  * First paint the dtotal line
  */
 
*8D
 	if (old_net != num_net ||
*8E
*8I
 	if (title_highlighted ||
 	    old_net != num_net ||
*8E
 	    old_batch != num_batch ||
 	    old_interactive != num_interactive ||
 	    old_oswap != num_oswap ||
 	    BannerLastActiveCpuCount != BannerActiveCpuCount ||
 	    old_total != num_total)
 	    {
*8I
*10I
 	    char sep[5];
*10E
 
 	    if (old_total != num_total ||
 		BannerLastActiveCpuCount != BannerActiveCpuCount)
 		title_highlighted = 1;
 	    else
 		title_highlighted = 0;
*10D
 		
*10E
*10I
 
 	    memset (buffer, 0, sizeof(buffer));
 	    memset (sep, 0, sizeof(sep));
 
 	    if (num_net > 0)
 		{
 		sprintf (buffer, "%s%sNet = %d",
 		buffer, sep, num_net);				
 		strcpy(sep, ", ");
 		}
 
 	    if (num_batch > 0)
 		{
 		sprintf (buffer, "%s%sBat = %d",
 		buffer, sep, num_batch);				
 		strcpy(sep, ", ");
 		}
 
 	    if (num_interactive > 0)
 		{
 		sprintf (buffer, "%s%sInt = %d",
 		buffer, sep, num_interactive);				
 		strcpy(sep, ", ");
 		}
 
 	    if (num_oswap > 0)
 		{
 		sprintf (buffer, "%s%sOSwap = %d",
 		buffer, sep, num_oswap);				
 		strcpy(sep, ", ");
 		}
 
*10E
*8E
 	    if (BannerActiveCpuCount > 1)
*10D
 		sprintf (buffer, "Net = %d, Bat = %d, Int = %d, OSwap = %d, Cpu's = %d, Total = %d",
 		num_net, num_batch, num_interactive, num_oswap, 
 		BannerActiveCpuCount, num_total);
 	    else
 		sprintf (buffer, "Net = %d, Bat = %d, Int = %d, OSwap = %d, Total = %d",
 		num_net, num_batch, num_interactive, num_oswap, num_total);
*10E
*10I
 		{
 		sprintf (buffer, "%s%sCpu = %d",
 		buffer, sep, BannerActiveCpuCount);				
 		strcpy(sep, ", ");
 		}
 
 	    sprintf (buffer, "%s%sTotal = %d",
 	    buffer, sep, num_total);				
 
*10E
 
*8I
 	    if (title_highlighted)
 		XSetForeground (XtDisplay(Bnr.process_widget), BannerGC, Bnr.hwmrk);
 
*8E
 	    BannerWriteText (XtDisplay(Bnr.process_widget), XtWindow(Bnr.process_widget),
 		0, 2*Bnr.font_height, buffer, 2, 
 		Bnr.process_width);
*8I
 
 	    XSetForeground (XtDisplay(Bnr.process_widget), BannerGC, Bnr.foreground);
 
*8E
 	    old_net = num_net;
 	    old_batch = num_batch;
 	    old_interactive = num_interactive;
 	    old_oswap = num_oswap;
*12I
 	    if (old_total > num_total)
 		BannerSound (&Son.LessProcesses, Son.Volume, Son.Time);
 	    if (old_total < num_total)
 		BannerSound (&Son.NewProcesses, Son.Volume, Son.Time);
*12E
 	    old_total = num_total;
 	    }
 
 /*
  * Now check out if the process States should be displayed
  */
     if (Bnr.Param_Bits.process_states)
 	{
 	char buffer[4];
 
 	y = 3*Bnr.font_height + 2;
 	x = Bnr.process_width - total_states_width + states_col_1;
 
 
*5D
 	if (lef != old_lef)
*5E
*5I
 	if (lef != old_lef || lef != old_lef1)
*5E
 	    {
*5I
 	    if (lef != old_lef)
 		XSetForeground (XtDisplay(Bnr.process_widget), BannerGC, Bnr.hwmrk);
 
*5E
 	    sprintf (buffer, "%d", lef);
*5I
 	    old_lef1 = old_lef;
*5E
 	    old_lef = lef;
 	    BannerWriteText (XtDisplay(Bnr.process_widget), XtWindow(Bnr.process_widget),
 		x, y, buffer, 3, 
 		states_num_width);
*5I
 	    XSetForeground (XtDisplay(Bnr.process_widget), BannerGC, Bnr.foreground);
*5E
 	    }
*5D
 	if (old_lefo != lefo)
*5E
*5I
 	if (old_lefo != lefo || old_lefo1 != lefo)
*5E
 	    {
*5I
 	    if (lefo != old_lefo)
 		XSetForeground (XtDisplay(Bnr.process_widget), BannerGC, Bnr.hwmrk);
*5E
 	    sprintf (buffer, "%d", lefo);
*5I
 	    old_lefo1 = old_lefo;
*5E
 	    old_lefo = lefo;
 	    BannerWriteText (XtDisplay(Bnr.process_widget), XtWindow(Bnr.process_widget),
 	    x + states_col_2 - states_col_1, y, buffer, 3, 
 	    states_num_width);
*5I
 	    XSetForeground (XtDisplay(Bnr.process_widget), BannerGC, Bnr.foreground);
*5E
 	    }
 
 	y = y + Bnr.font_height;
 
*5D
 	if (old_hib != hib)
*5E
*5I
 	if (old_hib != hib || old_hib1 != hib)
*5E
 	    {
*5I
 	    if (hib != old_hib)
 		XSetForeground (XtDisplay(Bnr.process_widget), BannerGC, Bnr.hwmrk);
*5E
 	    sprintf (buffer, "%d", hib);
*5I
 	    old_hib = old_hib1;
*5E
 	    old_hib = hib;
 	    BannerWriteText (XtDisplay(Bnr.process_widget), XtWindow(Bnr.process_widget),
 		x, y, buffer, 3, 
 		states_num_width);
*5I
 	    XSetForeground (XtDisplay(Bnr.process_widget), BannerGC, Bnr.foreground);
*5E
 	    }
*5D
 	if (old_hibo != hibo)
*5E
*5I
 	if (old_hibo != hibo || old_hibo1 != hibo)
*5E
 	    {
*5I
 	    if (hibo != old_hibo)
 		XSetForeground (XtDisplay(Bnr.process_widget), BannerGC, Bnr.hwmrk);
*5E
 	    sprintf (buffer, "%d", hibo);
*5I
 	    old_hibo1 = old_hibo;
*5E
 	    old_hibo = hibo;
 	    BannerWriteText (XtDisplay(Bnr.process_widget), XtWindow(Bnr.process_widget),
 	    x + states_col_2 - states_col_1, y, buffer, 3, 
 	    states_num_width);
*5I
 	    XSetForeground (XtDisplay(Bnr.process_widget), BannerGC, Bnr.foreground);
*5E
 	    }
 
 	y = y + Bnr.font_height;
 
*5D
 	if (old_com != com)
*5E
*5I
 	if (old_com != com || old_com1 != com)
*5E
 	    {
*5I
 	    if (com != old_com)
 		XSetForeground (XtDisplay(Bnr.process_widget), BannerGC, Bnr.hwmrk);
*5E
 	    sprintf (buffer, "%d", com);
*5I
 	    old_com1 = old_com;
*5E
 	    old_com = com;
 	    BannerWriteText (XtDisplay(Bnr.process_widget), XtWindow(Bnr.process_widget),
 		x, y, buffer, 3, 
 		states_num_width);
*5I
 	    XSetForeground (XtDisplay(Bnr.process_widget), BannerGC, Bnr.foreground);
*5E
 	    }
 
*5D
 	if (old_como != como)
*5E
*5I
 	if (old_como != como || old_como1 != como)
*5E
 	    {
*5I
 	    if (como != old_como)
 		XSetForeground (XtDisplay(Bnr.process_widget), BannerGC, Bnr.hwmrk);
*5E
 	    sprintf (buffer, "%d", como);
*5I
 	    old_como1 = old_como;
*5E
 	    old_como = como;
 	    BannerWriteText (XtDisplay(Bnr.process_widget), XtWindow(Bnr.process_widget),
 	    x + states_col_2 - states_col_1, y, buffer, 3, 
 	    states_num_width);
*5I
 	    XSetForeground (XtDisplay(Bnr.process_widget), BannerGC, Bnr.foreground);
*5E
 	    }
 
 	y = y + Bnr.font_height;
 
*5D
 	if (old_pfw != pfw)
*5E
*5I
 	if (old_pfw != pfw || old_pfw1 != pfw)
*5E
 	    {
*5I
 	    if (pfw != old_pfw)
 		XSetForeground (XtDisplay(Bnr.process_widget), BannerGC, Bnr.hwmrk);
*5E
 	    sprintf (buffer, "%d", pfw);
*5I
 	    old_pfw1 = old_pfw;
*5E
 	    old_pfw = pfw;
 	    BannerWriteText (XtDisplay(Bnr.process_widget), XtWindow(Bnr.process_widget),
 		x, y, buffer, 3, 
 		states_num_width);
*5I
 	    XSetForeground (XtDisplay(Bnr.process_widget), BannerGC, Bnr.foreground);
*5E
 	    }
*5D
 	if (old_cef != cef)
*5E
*5I
 	if (old_cef != cef || old_cef1 != cef)
*5E
 	    {
*5I
 	    if (cef != old_cef)
 		XSetForeground (XtDisplay(Bnr.process_widget), BannerGC, Bnr.hwmrk);
*5E
 	    sprintf (buffer, "%d", cef);
*5D
 	    old_others = cef;
*5E
*5I
 	    old_cef1 = old_cef;
 	    old_cef = cef;
*5E
 	    BannerWriteText (XtDisplay(Bnr.process_widget), XtWindow(Bnr.process_widget),
 	    x + states_col_2 - states_col_1, y, buffer, 3, 
 	    states_num_width);
*5I
 	    XSetForeground (XtDisplay(Bnr.process_widget), BannerGC, Bnr.foreground);
*5E
 	    }
 
 	y = y + Bnr.font_height;
 
*5D
 	if (old_colpg != colpg)
*5E
*5I
 	if (old_colpg != colpg || old_colpg1 != colpg)
*5E
 	    {
*5I
 	    if (colpg != old_colpg)
 		XSetForeground (XtDisplay(Bnr.process_widget), BannerGC, Bnr.hwmrk);
*5E
 	    sprintf (buffer, "%d", colpg);
*3D
 	    old_pfw = colpg;
*3E
*3I
*5I
 	    old_colpg1 = old_colpg;
*5E
 	    old_colpg = colpg;
*3E
 	    BannerWriteText (XtDisplay(Bnr.process_widget), XtWindow(Bnr.process_widget),
 		x, y, buffer, 3, 
 		states_num_width);
*5I
 	    XSetForeground (XtDisplay(Bnr.process_widget), BannerGC, Bnr.foreground);
*5E
 	    }
*5D
 	if (old_fpg != fpg)
*5E
*5I
 	if (old_fpg != fpg || old_fpg1 != fpg)
*5E
 	    {
*5I
 	    if (fpg != old_fpg)
 		XSetForeground (XtDisplay(Bnr.process_widget), BannerGC, Bnr.hwmrk);
*5E
 	    sprintf (buffer, "%d", fpg);
*5D
 	    old_others = fpg;
*5E
*5I
 	    old_fpg1 = old_fpg;
 	    old_fpg = fpg;
*5E
 	    BannerWriteText (XtDisplay(Bnr.process_widget), XtWindow(Bnr.process_widget),
 	    x + states_col_2 - states_col_1, y, buffer, 3, 
 	    states_num_width);
*5I
 	    XSetForeground (XtDisplay(Bnr.process_widget), BannerGC, Bnr.foreground);
*5E
 	    }
 
 	y = y + Bnr.font_height;
 
 	if (old_mwait != mwait)
 	    {
*5I
*12D
 
*12E
 	    if (mwait > 0)
*12I
 		{
*12E
 		XSetForeground (XtDisplay(Bnr.process_widget), BannerGC, Bnr.hwmrk);
*12I
 		BannerSound (&Son.Mwait, Son.Volume, Son.Time);
 		}
*12E
 
 	    BannerWriteText (XtDisplay(Bnr.process_widget), XtWindow(Bnr.process_widget),
 		mwait_x, mwait_y, "MWAIT", 1, 
 		states_col_1);
 
*5E
*7I
 	    if (mwait > 0)
 		XSetForeground (XtDisplay(Bnr.process_widget), BannerGC, Bnr.hwmrk);
 
*7E
 	    sprintf (buffer, "%d", mwait);
 	    old_mwait = mwait;
 	    BannerWriteText (XtDisplay(Bnr.process_widget), XtWindow(Bnr.process_widget),
 		x, y, buffer, 3, 
 		states_num_width);
*5I
 	    XSetForeground (XtDisplay(Bnr.process_widget), BannerGC, Bnr.foreground);
*5E
 	    }
*5D
 	if (old_others != others)
*5E
*5I
 	if (old_others != others || old_others1 != others)
*5E
 	    {
*5I
 	    if (others != old_others)
 		XSetForeground (XtDisplay(Bnr.process_widget), BannerGC, Bnr.hwmrk);
*5E
 	    sprintf (buffer, "%d", others);
*5I
 	    old_others1 = old_others;
*5E
 	    old_others = others;
 	    BannerWriteText (XtDisplay(Bnr.process_widget), XtWindow(Bnr.process_widget),
 	    x + states_col_2 - states_col_1, y, buffer, 3, 
 	    states_num_width);
*5I
 	    XSetForeground (XtDisplay(Bnr.process_widget), BannerGC, Bnr.foreground);
*5E
 	    }
 	}
 /*
  * Now go through the top process count, and display it all
  */
 
     y = 2*Bnr.font_height + 2;
     x = Bnr.font_width/2;
     for (i=0; i<process_display; i++)
 	{
 	process = (*top_processes)[i];
 	if (process != NULL &&
 	    process->diff_cputim > 0)
 	    {
 	    int width, clearwidth;
 	    int hist_x;
 
 	    if ((*top_processes)[i] != (*oldtop_processes)[i] 
*5D
 	       || process->name_changed)
*5E
*5I
 	       || process->name_changed || process->highlighted)
*5E
 		{
 		char buffer[50];
*6I
 		int forg;
 
 		forg = Bnr.foreground;
*6E
 		
*5I
 		if (i == 0 && !process->highlighted)
 		    {
*6D
 		    XSetForeground (XtDisplay(Bnr.process_widget), BannerGC, Bnr.hwmrk);
*6E
*6I
 		    forg = Bnr.hwmrk;
*6E
 		    process->highlighted = 1;
 		    }
 		else
 		    process->highlighted = 0;
 
*5E
 		x1 = x;
 		buffer[0] = 0;	
 		if (Bnr.Param_Bits.process_pid)	
 		    {
 		    sprintf (buffer, "%08X", process->pid);
*6I
 		    XSetForeground (XtDisplay(Bnr.process_widget), BannerGC, forg);
*6E
 		    BannerWriteText (XtDisplay(Bnr.process_widget), XtWindow(Bnr.process_widget),
 			x1, y + Bnr.font_height, buffer, 1, 
 			pid_width);
 		    x1 = x1 + pid_width + Bnr.font_width;
 		    }
 		if (Bnr.Param_Bits.process_user)	
 		    {
*6I
 		    XSetForeground (XtDisplay(Bnr.process_widget), BannerGC, forg);
*6E
 		    BannerWriteText (XtDisplay(Bnr.process_widget), XtWindow(Bnr.process_widget),
 			x1, y + Bnr.font_height, process->user, 1, 
 			user_width);
 		    x1 = x1 + user_width;
 		    }
 		if (Bnr.Param_Bits.process_name)	
 		    {
*6I
 		    XSetForeground (XtDisplay(Bnr.process_widget), BannerGC, forg);
*6E
 		    BannerWriteText (XtDisplay(Bnr.process_widget), XtWindow(Bnr.process_widget),
 			x1, y + Bnr.font_height, process->name, 1, 
 			name_width);
 		    x1 = x1 + name_width;
 		    }
 		hist = x1;
 		process->name_changed=0;
*5I
 		XSetForeground (XtDisplay(Bnr.process_widget), BannerGC, Bnr.foreground);
*5E
 		}	
 	    (*oldtop_processes)[i] = (*top_processes)[i];
 
 	    x1 = hist;
 
 	    sprintf (buffer, "%d%%", process->percent);
*5D
 
*5E
*5I
     
*5E
 	    if (process->percent > 0)
 		BannerWriteText (XtDisplay(Bnr.process_widget), XtWindow(Bnr.process_widget),
 		    x1, y + Bnr.font_height, buffer, 3, 
 		    percent);
 
 	    x1 = x1 + percent + Bnr.font_width/2;
 
 	    display_size = (Bnr.process_width - total_states_width) - x1;
 
 	    width = process->display_size * display_size;
 	    if (width > display_size)
 		width = display_size;
 	    clearwidth = display_size - width;
 
 	    if (width > 0)
*5D
 		XFillRectangle (XtDisplay(Bnr.process_widget), XtWindow (Bnr.process_widget),
 		    BannerGC, x1, 
 		    y + 3, 
 		    width,
 		    Bnr.font_height - 3);
 	    
*5E
*5I
 		{
 		int width1, hlwidth;
 
 		hlwidth = (display_size*Bnr.sys_highlight)/100;
 
 		if (width > hlwidth)
 		    {
 		    XFillRectangle (XtDisplay(Bnr.process_widget), XtWindow (Bnr.process_widget),
 			BannerGC, x1, 
 			y + 3, 
 			hlwidth,
 			Bnr.font_height - 3);
 		    XSetForeground (XtDisplay(Bnr.process_widget), BannerGC, Bnr.hwmrk);
 		    XFillRectangle (XtDisplay(Bnr.process_widget), XtWindow (Bnr.process_widget),
 			BannerGC, x1 + hlwidth, 
 			y + 3, 
 			width - hlwidth,
 			Bnr.font_height - 3);
 		    }
 		else
 		    XFillRectangle (XtDisplay(Bnr.process_widget), XtWindow (Bnr.process_widget),
 			BannerGC, x1, 
 			y + 3, 
 			width,
 			Bnr.font_height - 3);
 		}	    
*5E
 
 	    XSetBackground (XtDisplay(Bnr.process_widget), BannerGC,
 		Bnr.foreground);
 	    XSetForeground (XtDisplay(Bnr.process_widget), BannerGC,
 		Bnr.background);
 
 
 	    if (clearwidth > 0)
 		XFillRectangle (XtDisplay(Bnr.process_widget), XtWindow (Bnr.process_widget),
 		    BannerGC, x1 + width, 
 			y, 
 			clearwidth,
 			Bnr.font_height);
 
 	    XSetForeground (XtDisplay(Bnr.process_widget), BannerGC,
 		Bnr.foreground);
 	    XSetBackground (XtDisplay(Bnr.process_widget), BannerGC,
 		Bnr.background);
 
 	    y = y + Bnr.font_height;
 	    }
 	else
 	    {
 	    (*oldtop_processes)[i] = 0;
 	    XSetBackground (XtDisplay(Bnr.process_widget), BannerGC,
 		Bnr.foreground);
 	    XSetForeground (XtDisplay(Bnr.process_widget), BannerGC,
 		Bnr.background);
 
 	    XFillRectangle (XtDisplay(Bnr.process_widget), XtWindow (Bnr.process_widget),
 		BannerGC, 0, 
 		    y + Bnr.font_height - Bnr.font_ptr->ascent, 
 		    Bnr.process_width - total_states_width,
 		    Bnr.font_height);
 
 	    XSetForeground (XtDisplay(Bnr.process_widget), BannerGC,
 		Bnr.foreground);
 	    XSetBackground (XtDisplay(Bnr.process_widget), BannerGC,
 		Bnr.background);
 	    y = y + Bnr.font_height;
 	    }
 	}
 }
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **	BannerProcess ()
 **
 **      This is the main work routine for the processes window, it is called
 **	once a second, and dispays all the info about the current top process.
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
 BannerProcess()
 {
 int i, y, current_lowest;
 int top_cnt = 0;
 /*
  * first check we are all initialised
  */
 
     if (!process_init)
 	BannerProcessRefresh();
 
     process_update --;
     if (process_update > 0)
 	return;
     process_update = process_update_count;
 
 /*
  * Now call the colection routine
  */
 
     total_cpu = 0;
     GetNewInfo ();
 
 
 /*
  * Clear out the old data
  */
     memset (top_processes, 0, sizeof(int)*process_display);
 /*
  * Now place it in its order of cpu usage
  */
     current_lowest = 0;
     for (i=0;  i<num_process;  i++)
 	{
 	Bnr$Process *topprocess;
 	process = (*processes)[i];
 	if (process->pid != null_pid &&
 	    process->seen == 1 &&
 	    process->diff_cputim > current_lowest)
 	    for (y=0; y<process_display; y++)
 		{
 		topprocess = (*top_processes)[y];
 		if (topprocess == NULL ||
 		    process->diff_cputim > 
 		    topprocess->diff_cputim)
 		    {
 		    memcpy (&((*top_processes)[y+1]), 
 			&((*top_processes)[y]),
 			sizeof(int) * (process_display - 1 - y));
 		    (*top_processes)[y] = (*processes)[i];		
 		    topprocess = (*top_processes)[y];
 		    if (topprocess->diff_cputim > 0.0 &&
 			(total_cpu/topprocess->diff_cputim) > 0.0)
 			{
 			topprocess->percent = (100.0*BannerActiveCpuCount)/
 			    (total_cpu/topprocess->diff_cputim);
 			if (topprocess->percent > 100)
 			    topprocess->percent=100;
 			}
 		    else
 			topprocess->percent=0;
 		    topprocess->display_size = 
 			topprocess->diff_cputim / 
 			    (total_cpu/BannerActiveCpuCount);			
 /*
  * Remeber the lowest CPU user we have in our display, to reduce the 
  * sort time
  */
 		    topprocess = (*top_processes)[process_display-1];
 		    if (topprocess != NULL)
 			{
 			current_lowest = topprocess->diff_cputim;
 			}
 		    break;
 		    }
 		}		
 
 	    }
 /*
  * Now call the display routine
  */ 
 
     PaintDisplay ();       
 }
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **  GetNewInfo ()
 **
 **      This routine gets the new process info.
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
 
 static     int size;
 static     int biggest_size = 0;
 
 static void	
 GetNewInfo()
 {
     int i;
     int index = -1;
     int pos;
     int free_pos;
     int number = 0;
     int status;
 /*
  * Clear the seen count, so we can find processes that dont exist any longer.
  */
 
     for (i=0; i<num_process; i++)
 	{
 	process = (*processes)[i];
 	if (process->seen == 0)
 	    process->pid = -1;
 /*
  * tag all processes as non resident for the time being
  */
 	process->seen = 0;
 	process->resident = 0;
 	};    
 
     num_interactive = num_batch = num_sub = num_net = num_total = num_oswap = 0;
 
     lef = lefo = hib = hibo = com = como = pfw = others = mwait = cef =
     colpg = fpg = 0;
 
     while (1)
 	{
 
 /*
  * Get the processes mode, if it is COMO, HIBO, LEFO, or Suspended, then 
  * skip it. If this is the end of the list, then get out.
  */
 	status = SYS$GETJPIW (NULL, &index, NULL, &modeitemlist, 
 	    NULL, NULL, NULL);
 
 	if (status == SS$_NOMOREPROC)
 	    break;
 
 /*
  * Null process will be the first we see, so remeber its pid
  */
 	if (null_pid == 0)
 	    null_pid = pid;
 	
 
 /*
  * remember the type of process this one was
  */
 	if (pid != null_pid)
 	{
 	    switch (mode)
 	    {
 
 	    case JPI$K_NETWORK : num_net ++; break;
 
 	    case JPI$K_BATCH : num_batch++; break;
 
 	    case JPI$K_INTERACTIVE : num_interactive++; break;
 	    }
 	    num_total++;
 /*
  * Is it a resident procerss, or Oswapped?
  */
 	    if (!(sts & 1))
 		num_oswap++;
 
 /*
  * Add up the states
  */
 	    switch (state)
 	    {
 	    case SCH$C_COM : com++; break;
 	    case SCH$C_COMO : como++; break;
 	    case SCH$C_LEF : lef++; break;
 	    case SCH$C_LEFO : lefo++; break;
 	    case SCH$C_HIB : hib++; break;
 	    case SCH$C_HIBO : hibo++; break;
 	    case SCH$C_PFW : pfw++; break;
 	    case SCH$C_MWAIT : mwait++; break;
 	    case SCH$C_CEF : cef++; break;
 	    case SCH$C_COLPG : colpg++; break;
 	    case SCH$C_FPG : fpg++; break;
 	    default: others++;break;
 	    }
 	}
 	
 
 	
 /*
  * find this process in our array
  */
 	number ++;
 	pos = -1;
 	free_pos = -1;
 	for (i=0; i<num_process; i++)
 	    {
 	    process = (*processes)[i];
 /*
  * Is this our process?
  */
 	    if (pid == process->pid)
 		{
 		pos = i;
 		process->seen = 1;
 		break;
 		}	
 /*
  * Could we use this as a free place?
  */
 	    if (process->pid == -1)
 		free_pos = i;
 	    }
 	if (pos == -1)
 	    {
 	    int newsize;
 /*
 * this is a new process so check that its name isn't bigger than we have 
 * seen before.
 */
 	    newsize = XTextWidth (Bnr.font_ptr, name, name_size(name) - 1);
 	    if (newsize > name_width)
 		name_width = newsize;
 
 	    newsize = XTextWidth (Bnr.font_ptr, user, name_size(user) - 1);
 	    if (newsize > user_width)
 		user_width = newsize;
 /*
 * Now find a slot to store this process
 */
 	    if (processes == NULL)
 		{
 		processes = XtMalloc(sizeof(int));
 		process = (*processes)[0] = XtMalloc(sizeof(Bnr$Process));
 		process->name = XtMalloc(30);
 		process->user = XtMalloc(30);
 		pos = num_process;
 		num_process++;
 		}
 	    else
 		{
 		int new;
 /* 
 * look for a place that we haven't seen for quit a while.
 */
 
 		pos = free_pos;
 /*
 * now use it if we found one, else, create a new slot
 */
 		if (pos == -1)
 		    {
 		    new = XtMalloc((num_process + 1)*sizeof(int));
 		    memcpy (new, processes, num_process*sizeof(int));
 		    XtFree(processes);
 		    processes=new;
 		    process = (*processes)[num_process]= XtMalloc(sizeof(Bnr$Process));
 		    process->name = XtMalloc(30);
 		    process->user = XtMalloc(30);
 		    pos = num_process;
 		    num_process++;
 		    }
 		else
 		    process = (*processes)[pos];
 		}
 /* 
 * dummy up the counts on this record, so all counts will seem to be 0
 */
 	    process->old_bufcnt = 0;
 	    process->old_diocnt = 0;
 	    process->old_cputim = 0;
 	    process->old_pageflts = 0;
 	    process->name_changed = 0;
 	    memcpy(process->name,name,name_size(name) );
 	    memcpy(process->user,user,name_size(user) );
 	    }
 
 	process = (*processes)[pos];
 	if (sts & 1)	    /* Is this a Resident process? */
 	    process->resident = 1;
 	process->seen = 1;
 	process->pid = pid;
 	process->mode = mode;
 	process->diff_cputim = 0;
 /*
  * Check that the process name hasn't changed since the last time we looked.
  */
*8D
 	if (strcmp(process->user, user) != 0)
*8E
*8I
 	if (strncmp(process->user, user, name_size(user)-1) != 0)
*8E
 	    {
 	    int newsize;
 	    process->name_changed=1;
 	    memcpy(process->user,user,name_size(user) );
 	    newsize = XTextWidth (Bnr.font_ptr, user, name_size(user) - 1);
 	    if (newsize > user_width)
 		user_width = newsize;
 	    }
*8I
 
*8E
 	if (strcmp(process->name, name) != 0)
 	    {
 	    int newsize;
 	    process->name_changed=1;
 	    memcpy(process->name,name,name_size(name) );
 	    newsize = XTextWidth (Bnr.font_ptr, name, name_size(name) - 1);
 	    if (newsize > name_width)
 		name_width = newsize;
 	    }
 	}
 
     for (i=0; i<num_process; i++)
 	{
 	process = (*processes)[i];
 	if ((process->seen == 1 && process->resident == 1) 
 	    || process->pid == null_pid)
 	    {
 /*
  * Now store all the information for this process
  */
 	    if (process->pid != null_pid)
 		status = SYS$GETJPIW (NULL, &process->pid, NULL, &itemlist, 
 		    NULL, NULL, NULL);
 	    process->bufcnt = bufcnt;
 	    process->diff_bufcnt = bufcnt - process->old_bufcnt;
 	    process->old_bufcnt = bufcnt;
 	    process->diocnt = diocnt;
 	    process->diff_diocnt = diocnt - process->old_diocnt;
 	    process->old_diocnt = diocnt;
 /*
  * The null process does not show any CPU time, even though the CPU(s) may
  * have spent some time in NULL mode. We will have to read the CPU data to
  * get the time in NULL mode, and populate the Null mode
*4I
  *
  * We total NULL times, and Interupt time together, because Interupt time
  * is a system overhead as well, and should not be blaimed on any particular
  * process.
*4E
  */
 	    if (process->pid != null_pid)
 		{
 		process->cputim = cputim;
 		if (process->old_cputim > 0)
 		    process->diff_cputim = cputim - process->old_cputim;
 		process->old_cputim = cputim;
 		}
 	    else
 		{
 		float null_mode = 0;
 		int i,j;
 
 		j = 0;
 		BannerLastActiveCpuCount = BannerActiveCpuCount;
 		BannerActiveCpuCount = 0;
 		for (i=1;  i<=SMP$GL_ACTIVE_CPUS;  i=i*2)
 		    {
 		    if ((i & SMP$GL_ACTIVE_CPUS) > 0)
 			{
*4D
 			null_mode = null_mode + SMP$GL_CPU_DATA[j]->cpu$l_nullcpu;
*4E
*4I
 			null_mode = null_mode + 
 				SMP$GL_CPU_DATA[j]->cpu$l_nullcpu;
*8D
 			null_mode = null_mode + 
*8E
*8I
 
 			if (SMP$GL_CPU_DATA[j]->cpu$l_interrupt >=
 			    SMP$GL_CPU_DATA[j]->cpu$l_nullcpu)
 			    null_mode = null_mode + 
 				SMP$GL_CPU_DATA[j]->cpu$l_interrupt -
 				SMP$GL_CPU_DATA[j]->cpu$l_nullcpu;
 			else
 			    null_mode = null_mode + 
*8E
 				SMP$GL_CPU_DATA[j]->cpu$l_interrupt;
*4E
*8I
 
*8E
 			BannerActiveCpuCount++;
 			}
 		    j++;
 		    }
 
 		process->cputim = null_mode;
 		if (process->old_cputim != 0)
 		    process->diff_cputim = null_mode - process->old_cputim;
 		else
 		    process->diff_cputim = 0;
 		process->old_cputim = null_mode;
 		}
 	    process->pageflts = pageflts;
 	    process->diff_pageflts = pageflts - process->old_pageflts;
 	    process->old_pageflts = pageflts;
 /*
  * Now work out which are the top cpu users.
  */
 	    if (process->diff_cputim > 0 &&
 		process->seen == 1)
 		total_cpu = total_cpu + process->diff_cputim;
 	    }        
 	}
 }
