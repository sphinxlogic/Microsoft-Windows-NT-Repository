#module BannerCpu "V1.0"

/*
**++
**  FACILITY:
**
**      The DECwindows Banner program
**
**  ABSTRACT:
**
**      This module contains all the code, to run the CPU histogram.
**
**  AUTHORS:
**
**      JIM SAUNDERS
**
**
**  CREATION DATE:     20th June 1988
**
**  MODIFICATION HISTORY:
**--
**/


/*
**
**  INCLUDE FILES
**
**/


#include "stdio.h"
#include "jpidef.h"
#include "syidef.h"
#include "MrmAppl.h"

#include "Banner"


/*
**
**  MACRO DEFINITIONS
**
**/

#define SS$_NORMAL 1


typedef struct _vms_item_list {
	short	 buffer_length;
	short	 item_code;
	void	*buffer;
	int	*returned_buffer_length;
	} VMS_ItemList;


/*
 * now our runtime data structures
 */

extern	 Bnr$_Blk    Bnr;
extern	 Clk$_Blk    Clk;
extern	 GC	     BannerGC;
extern	 XGCValues   GcValues;

extern   char	     *BannerVmsDisplayName;
extern   char	     BannerVmsNodeName[];
extern   char	     BannerVmsVersion[];


extern void BannerSignalError();

void BannerCpu ();
void BannerCpuRefresh ();
void BannerCpuSetup ();


static int cpu_init = 0;
static int cpu_update;

static int last_time[2];
static int curr_time[2];
static int quadtime[2];
static int update_count;
static int min_tick;
static int p_size;

static XPoint cpu_hist[4][1004];

static int min_height = 100;
static int height[4];

static int bar_wid, widel, tickl_x, tach_x, bar_x, bar_x1, tach_x1,
	    tickr_x, wider, bar_height, seg_height, low_y, high_y;

static int cpu_modes = 0;

static float interrupt_mode = 0; 
static float kernel_mode = 0; 
static float exec_mode = 0; 
static float super_mode = 0; 
static float user_mode = 0; 
static float compatibility_mode = 0;
static float spinlock_mode = 0;
static float null_mode = 0;
static float null_cpumode[4] = {0, 0, 0, 0};
static float last_total = 0;
static float last_cputotal[4] = {0, 0, 0, 0};

static int k_start;
static int e_start;
static int i_start;
static int s_start;
static int u_start;
static int n_start;

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
    
globalref cpu$data *SMP$GL_CPU_DATA[];	    /* CPU data pointer array */
globalref int SMP$GL_ACTIVE_CPUS;	    /* Active cpu mask */

static int BannerActiveCpuCount = 1;
static int BannerLastActiveCpuCount = 1;


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	BannerCpuSetup ()
**
**      This routine is the setup routine for the CPu histogram. It initalises
**	all the data structures, and then calls the refresh routine, to get
**	all the sdtatic info displayed.
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
BannerCpuSetup()
{
    int ticks, i, j; 
    int status, inc, delta;
    short len;


/*
 * first initialise our structures
 */
    update_count = Bnr.cpu_update;
    
    if (Bnr.cpu_width > 160 &&
	Bnr.Param_Bits.modes_window &&
	Bnr.cpu_height > 5 * Bnr.font_height)
	{
	cpu_modes = 1;
	Bnr.cpu_count = Bnr.cpu_width - (11*Bnr.font_width) - 20;

	interrupt_mode = 0;
	kernel_mode = 0;
	exec_mode = 0;
	user_mode = 0;
	super_mode = 0;
	compatibility_mode = 0;
	spinlock_mode = 0;
	null_mode = 0;
	null_cpumode[0] = null_cpumode[1] = null_cpumode[2] =
	    null_cpumode[3] = 0;

	j = 0;
	BannerLastActiveCpuCount = BannerActiveCpuCount;
	BannerActiveCpuCount = 0;
	for (i=1;  i<=SMP$GL_ACTIVE_CPUS;  i=i*2)
	    {
	    if ((i & SMP$GL_ACTIVE_CPUS) > 0)
		{
		BannerActiveCpuCount++;
		kernel_mode = kernel_mode + SMP$GL_CPU_DATA[j]->cpu$l_kernel;
		last_cputotal[BannerActiveCpuCount-1] = 
		    last_cputotal[BannerActiveCpuCount-1] + 
		    SMP$GL_CPU_DATA[j]->cpu$l_kernel;
		exec_mode = exec_mode + SMP$GL_CPU_DATA[j]->cpu$l_executive;
		last_cputotal[BannerActiveCpuCount-1] = 
		    last_cputotal[BannerActiveCpuCount-1] + 
		    SMP$GL_CPU_DATA[j]->cpu$l_executive;
		user_mode =  user_mode + SMP$GL_CPU_DATA[j]->cpu$l_user;
		last_cputotal[BannerActiveCpuCount-1] =  
		    last_cputotal[BannerActiveCpuCount-1] + 
		    SMP$GL_CPU_DATA[j]->cpu$l_user;
		super_mode = super_mode + SMP$GL_CPU_DATA[j]->cpu$l_super;
		last_cputotal[BannerActiveCpuCount-1] = 
		    last_cputotal[BannerActiveCpuCount-1] + 
		    SMP$GL_CPU_DATA[j]->cpu$l_super;
		compatibility_mode = compatibility_mode + SMP$GL_CPU_DATA[j]->cpu$l_compatibility;
		last_cputotal[BannerActiveCpuCount-1] = 
		    last_cputotal[BannerActiveCpuCount-1] + 
		    SMP$GL_CPU_DATA[j]->cpu$l_compatibility;
		spinlock_mode = spinlock_mode + SMP$GL_CPU_DATA[j]->cpu$l_spinlock;
		last_cputotal[BannerActiveCpuCount-1] = 
		    last_cputotal[BannerActiveCpuCount-1] + 
		    SMP$GL_CPU_DATA[j]->cpu$l_spinlock;
		null_mode = null_mode + SMP$GL_CPU_DATA[j]->cpu$l_nullcpu;
		null_cpumode[BannerActiveCpuCount-1] = 
		    SMP$GL_CPU_DATA[j]->cpu$l_nullcpu;
		last_cputotal[BannerActiveCpuCount-1] = 
		    last_cputotal[BannerActiveCpuCount-1] + 
		    SMP$GL_CPU_DATA[j]->cpu$l_nullcpu;
/*
 * Interrupt time should include NULL time, so subtract NULL time 
 * from Interrupt time, if that is the case.
 */
		if (SMP$GL_CPU_DATA[j]->cpu$l_interrupt >=
		    SMP$GL_CPU_DATA[j]->cpu$l_nullcpu)
		    {
		    interrupt_mode = interrupt_mode + 
			SMP$GL_CPU_DATA[j]->cpu$l_interrupt -
			SMP$GL_CPU_DATA[j]->cpu$l_nullcpu;
		    last_cputotal[BannerActiveCpuCount-1] = 
			last_cputotal[BannerActiveCpuCount-1] + 
			SMP$GL_CPU_DATA[j]->cpu$l_interrupt -
			SMP$GL_CPU_DATA[j]->cpu$l_nullcpu;
		    }
		else
		    {
		    interrupt_mode = interrupt_mode + 
			SMP$GL_CPU_DATA[j]->cpu$l_interrupt;
		    last_cputotal[BannerActiveCpuCount-1] = 
			last_cputotal[BannerActiveCpuCount-1] + 
			SMP$GL_CPU_DATA[j]->cpu$l_interrupt;
		    }
		}
	    j++;
	    }
	last_total = interrupt_mode + kernel_mode + exec_mode + user_mode +
	    super_mode + compatibility_mode + spinlock_mode + null_mode;
	}
    else
	{
	cpu_modes = 0;
	Bnr.cpu_count = Bnr.cpu_width - 20; 
	}   

    if (BannerActiveCpuCount>4)
	BannerActiveCpuCount = 4;

    if (Bnr.cpu_count > 1000)
	Bnr.cpu_count = 1000;

    p_size = Bnr.cpu_count + 4;

/*
 * Now initialise our current time marks.
 */
    if ( (status = SYS$GETTIM (&curr_time)) != SS$_NORMAL)
	BannerSignalError ("Failed to get the time for SYS$GETTIM service",
	    status);


   
    last_time[0] = curr_time[0];
    last_time[1] = curr_time[1];

    min_tick = 4000000;

/*
 * Now initialise all the variables to redisplay the static infor with.
 */

/*
 * If we want just a single histogram 
 */
	if (Bnr.Param_Bits.cpu_multi_hist)
	    seg_height = bar_height = Bnr.cpu_height/BannerActiveCpuCount;
	else
	    seg_height = bar_height = Bnr.cpu_height;

    inc = 10;

    while (seg_height/inc < 1 && inc > 1)
	inc = inc/2;

    delta = seg_height/inc;
    bar_height = inc * delta;

    bar_wid = Bnr.cpu_count;
    widel = 2;
    tickl_x = widel + 2;
    tach_x = tickl_x + 3;
    bar_x = tach_x + 2;
    bar_x1 = bar_x + bar_wid;
    tach_x1 = bar_x1 + 2;
    tickr_x = tach_x1 + 3;
    wider = tickr_x + 2;
/*
 * Now initialise our memory
 */
    for (j=0; j <BannerActiveCpuCount; j++)
	{
	for (i=2;  i <= Bnr.cpu_count + 2;  i++)
	    {
	    cpu_hist[j][i].x = bar_x1;
	    cpu_hist[j][i].y = seg_height * j + bar_height;
	    }
	}
    cpu_update = 1;   
    cpu_init = 1;
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	BannerCpuRefresh ();
**
**      This routine repaints all the constant infor on the screen.
**
**  FORMAL PARAMETERS:
**
**      display
**	window
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
BannerCpuRefresh()
{
    int height, delta, i, j, inc;

/*
 * If we are not initialised then do it.
 */

    if (! cpu_init)
	BannerCpuSetup ();

    inc = 10;

    while (seg_height/inc < 1 && inc > 1)
	inc = inc/2;

    delta = seg_height/inc;
    bar_height = inc * delta;
/*
 * Now draw the ticks on the side of the window.
 */

    for (j=0; j < BannerActiveCpuCount; j++)
	{
/*
 * If we want just a single histogram
 */
	if (Bnr.Param_Bits.cpu_multi_hist)
	    height = (BannerActiveCpuCount-j-1) * seg_height; 
	else
	    height = 0; 
	for (i=0;  i <= inc;  i++)
	    {
	    XDrawLine (XtDisplay (Bnr.cpu_widget), XtWindow (Bnr.cpu_widget), 
		BannerGC, tickr_x, height, wider, height);

	    if (i == 0 || i == 5 || i == inc)
		XDrawLine (XtDisplay (Bnr.cpu_widget), XtWindow (Bnr.cpu_widget), 
		    BannerGC, tach_x1, height, wider, height);

	    XDrawLine (XtDisplay (Bnr.cpu_widget), XtWindow (Bnr.cpu_widget), 
		BannerGC, widel, height, tickl_x, height);

	    if (i == 0 || i == 5 || i == inc)
		XDrawLine (XtDisplay (Bnr.cpu_widget), XtWindow (Bnr.cpu_widget), 
		    BannerGC, widel, height, tach_x, height);

	    height = delta + height;
	    }

/*
 * If we want just a single histogram then exit at 0
 */
	if (!Bnr.Param_Bits.cpu_multi_hist)
	    break;
	}
/* 
 * Now repaint the area we had filled in the histogram
 */
    
    for (j=0; j < BannerActiveCpuCount; j++)
    {
	cpu_hist[j][0].x = bar_x;
	cpu_hist[j][0].y = seg_height * j;
	cpu_hist[j][1].x = bar_x;
	cpu_hist[j][1].y = seg_height * j + bar_height;
	cpu_hist[j][Bnr.cpu_count + 2].x = bar_x1;
	cpu_hist[j][Bnr.cpu_count + 2].y = seg_height * j + bar_height;
	cpu_hist[j][Bnr.cpu_count + 3].x = bar_x1;
	cpu_hist[j][Bnr.cpu_count + 3].y = seg_height * j;

	XSetBackground (XtDisplay(Bnr.cpu_widget), BannerGC,
	    Bnr.foreground);
	XSetForeground (XtDisplay(Bnr.cpu_widget), BannerGC,
	    Bnr.background);

	XFillPolygon (XtDisplay(Bnr.cpu_widget), XtWindow(Bnr.cpu_widget),
	    BannerGC, &cpu_hist[j][0], p_size, Complex, CoordModeOrigin);
	
	XSetForeground (XtDisplay(Bnr.cpu_widget), BannerGC,
	    Bnr.foreground);
	XSetBackground (XtDisplay(Bnr.cpu_widget), BannerGC,
	    Bnr.background);


	cpu_hist[j][0].x = bar_x;
	cpu_hist[j][0].y = seg_height * j + bar_height;
	cpu_hist[j][1].x = bar_x;
	cpu_hist[j][1].y = seg_height * j + bar_height;
	cpu_hist[j][Bnr.cpu_count + 2].x = bar_x1;
	cpu_hist[j][Bnr.cpu_count + 2].y = seg_height * j + bar_height;
	cpu_hist[j][Bnr.cpu_count + 3].x = bar_x1;
	cpu_hist[j][Bnr.cpu_count + 3].y = seg_height * j + bar_height;

	XFillPolygon (XtDisplay(Bnr.cpu_widget), XtWindow(Bnr.cpu_widget),
	    BannerGC, &cpu_hist[j][0], p_size, Complex, CoordModeOrigin);

/*
 * If we want just a single histogram then exit at 0
 */
	if (!Bnr.Param_Bits.cpu_multi_hist)
	    break;

    }

    if (cpu_modes)
	{
	int x, y;

	if (BannerVmsDisplayName == NULL)
	    {
	    BannerWriteText (XtDisplay(Bnr.cpu_widget), XtWindow(Bnr.cpu_widget),
		Bnr.cpu_count + 20, Bnr.font_height, BannerVmsNodeName, 2, 
		XTextWidth(Bnr.font_ptr, "XXXXXXXXXXX", 11));

	    BannerWriteText (XtDisplay(Bnr.cpu_widget), XtWindow(Bnr.cpu_widget),
		Bnr.cpu_count + 20, 2*Bnr.font_height, BannerVmsVersion, 2, 
		XTextWidth(Bnr.font_ptr, "XXXXXXXXXXX", 11));
	    }
	else
	    {
	    BannerWriteText (XtDisplay(Bnr.cpu_widget), XtWindow(Bnr.cpu_widget),
		Bnr.cpu_count + 20, Bnr.font_height, BannerVmsDisplayName, 2, 
		XTextWidth(Bnr.font_ptr, "XXXXXXXXXXX", 11));
	    }

	n_start = x = Bnr.cpu_count + 20 + Bnr.font_width + Bnr.font_width/4;
	y = Bnr.cpu_height - 1;
	BannerWriteText (XtDisplay(Bnr.cpu_widget), XtWindow(Bnr.cpu_widget),
	    x, y, "N", 2, 
	    XTextWidth(Bnr.font_ptr, "N", 1));
	u_start = x = x + Bnr.font_width + Bnr.font_width / 2;
	BannerWriteText (XtDisplay(Bnr.cpu_widget), XtWindow(Bnr.cpu_widget),
	    x, y, "U", 2, 
	    XTextWidth(Bnr.font_ptr, "U", 1));
	s_start = x = x + Bnr.font_width + Bnr.font_width / 2;
	BannerWriteText (XtDisplay(Bnr.cpu_widget), XtWindow(Bnr.cpu_widget),
	    x, y, "S", 2, 
	    XTextWidth(Bnr.font_ptr, "S", 1));
	e_start = x = x + Bnr.font_width + Bnr.font_width / 2;
	BannerWriteText (XtDisplay(Bnr.cpu_widget), XtWindow(Bnr.cpu_widget),
	    x, y, "E", 2, 
	    XTextWidth(Bnr.font_ptr, "E", 1));
	k_start = x = x + Bnr.font_width + Bnr.font_width / 2;
	BannerWriteText (XtDisplay(Bnr.cpu_widget), XtWindow(Bnr.cpu_widget),
	    x, y, "K", 2, 
	    XTextWidth(Bnr.font_ptr, "K", 1));
	i_start = x = x + Bnr.font_width + Bnr.font_width / 2;
	BannerWriteText (XtDisplay(Bnr.cpu_widget), XtWindow(Bnr.cpu_widget),
	    x, y, "I", 2, 
	    XTextWidth(Bnr.font_ptr, "X", 1));

	y = Bnr.cpu_height - Bnr.font_height;

	for (i=0;  i <= 5;  i++)
	    {
	    XDrawLine (XtDisplay (Bnr.cpu_widget), XtWindow (Bnr.cpu_widget), 
		BannerGC, Bnr.cpu_count + 20 + 2, y, 
		Bnr.cpu_count + 20 + Bnr.font_width - 2, y);

	    XDrawLine (XtDisplay (Bnr.cpu_widget), XtWindow (Bnr.cpu_widget), 
		BannerGC, Bnr.cpu_width - Bnr.font_width + 2, y, 
		Bnr.cpu_width - 2, y);

	    y = y - (Bnr.cpu_height - 3*Bnr.font_height)/5;
	    }
	}
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      BannerDrawCpuHist ()
**
**  This routine draws a CPU histogram line for the modes display.
**
**  FORMAL PARAMETERS:
**
**      See above
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
void	BannerDrawCpuHist(display, window, gc, x, y, width, height,
	    max_height, fill, highlight)
{
    int hwmrk;

    hwmrk = (max_height*Bnr.sys_highlight)/100;

    XSetFillStyle (display, gc, fill);

    if (highlight && height > hwmrk)
	{
	XSetForeground (display, gc, Bnr.hwmrk);
	XFillRectangle (display, window,
		    gc, x, y, width, height - hwmrk); 
	XSetForeground (display, gc, Bnr.foreground);
	XFillRectangle (display, window,
		    gc, x, y + (height - hwmrk), width, hwmrk); 
	}
    else
	XFillRectangle (display, window,
		    gc, x, y, width, height); 

    XSetFillStyle (display, gc, FillSolid);

    XSetBackground (display, gc,
	Bnr.foreground);
    XSetForeground (display, gc,
	Bnr.background);

    XFillRectangle (display, window,
		    gc, x, 
		    (Bnr.font_height * 2), 
		    Bnr.font_width, 
		    Bnr.cpu_height - 3*Bnr.font_height - height);

    XSetForeground (display, gc,
	Bnr.foreground);
    XSetBackground (display, gc,
	Bnr.background);
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	BannerCpu ()
**
**      Thsi routine handles the drawing of the banner histogram.
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
BannerCpu()
{
    int ticks, i, j, cpu, modes_height, number_cpus, status;
    short len;
    float total_time, total_cputime[4], cpu_ticks;
    int mode_size, null_size, null_cpusize[4];	
    float total_user, total_kernel, total_interrupt, total_super, total_exec,
	total_null, total_cpunull[4], ftemp;

    float realtime, fbar_height;

/*
 * Check we are initialised
 */

    if (! cpu_init)
	BannerCpuRefresh ();

    cpu_update --;
    if (cpu_update > 0)
	return;
    cpu_update = Bnr.cpu_update;
/*
 * Now initialise our current time marks.
 */
    if ( (status = SYS$GETTIM (&curr_time)) != SS$_NORMAL)
	BannerSignalError ("Failed to get the time for SYS$GETTIM service",
	    status);


   
/*
 * Now compuet opur CPU time versa percent of real time
 */

    lib$subx (
	&curr_time, 
	&last_time, 
	&quadtime, 
	NULL );

    lib$ediv (
	&100000, 
	&quadtime, 
	&quadtime[0], 
	&quadtime[1]);


/*
 * check we have not been called twice in 1 second
 */

    if (quadtime[0] < 64)
	return;



/*
 * calculate time in each mode for each CPU
 */
    total_time = 0;
    total_null = 0;
    total_user = 0;
    total_kernel = 0;
    total_super = 0;
    total_exec = 0;
    total_interrupt = 0;

    total_cputime[0] = total_cputime[1] = total_cputime[2] = 
	total_cputime[3] = 0; 
    total_cpunull[0] = total_cpunull[1] = total_cpunull[2] = 
	total_cpunull[3] = 0; 
    null_cpusize[0] = null_cpusize[1] = null_cpusize[2] = null_cpusize[3] = 0;
    height[0] = height[1] = height[2] = height[3] = 0;
    j = 0;
    BannerLastActiveCpuCount = BannerActiveCpuCount;
    BannerActiveCpuCount = 0;
    for (i=1;  i<=SMP$GL_ACTIVE_CPUS;  i=i*2)
	{
	if ((i & SMP$GL_ACTIVE_CPUS) > 0)
	    {
	    BannerActiveCpuCount++;
	    total_time = total_time + SMP$GL_CPU_DATA[j]->cpu$l_kernel;
	    total_cputime[BannerActiveCpuCount-1] = 
		total_cputime[BannerActiveCpuCount-1] + 
		SMP$GL_CPU_DATA[j]->cpu$l_kernel;
	    total_kernel = total_kernel + SMP$GL_CPU_DATA[j]->cpu$l_kernel;
	    total_time = total_time + SMP$GL_CPU_DATA[j]->cpu$l_executive;
	    total_cputime[BannerActiveCpuCount-1] =
		total_cputime[BannerActiveCpuCount-1] + 
		SMP$GL_CPU_DATA[j]->cpu$l_executive;
	    total_exec = total_exec + SMP$GL_CPU_DATA[j]->cpu$l_executive;
	    total_time = total_time + SMP$GL_CPU_DATA[j]->cpu$l_user;
	    total_cputime[BannerActiveCpuCount-1] = 
		total_cputime[BannerActiveCpuCount-1] + 
		SMP$GL_CPU_DATA[j]->cpu$l_user;
	    total_user = total_user + SMP$GL_CPU_DATA[j]->cpu$l_user;
	    total_time = total_time + SMP$GL_CPU_DATA[j]->cpu$l_super;
	    total_cputime[BannerActiveCpuCount-1] = 
		total_cputime[BannerActiveCpuCount-1] + 
		SMP$GL_CPU_DATA[j]->cpu$l_super;
	    total_super = total_super + SMP$GL_CPU_DATA[j]->cpu$l_super;
	    total_time = total_time + SMP$GL_CPU_DATA[j]->cpu$l_compatibility;
	    total_cputime[BannerActiveCpuCount-1] = 
		total_cputime[BannerActiveCpuCount-1] + 
		SMP$GL_CPU_DATA[j]->cpu$l_compatibility;
	    total_time = total_time + SMP$GL_CPU_DATA[j]->cpu$l_spinlock;
	    total_cputime[BannerActiveCpuCount-1] = 
		total_cputime[BannerActiveCpuCount-1] + 
		SMP$GL_CPU_DATA[j]->cpu$l_spinlock;
	    total_time = total_time + SMP$GL_CPU_DATA[j]->cpu$l_nullcpu;
	    total_cputime[BannerActiveCpuCount-1] = 
		total_cputime[BannerActiveCpuCount-1] + 
		SMP$GL_CPU_DATA[j]->cpu$l_nullcpu;
	    total_null = total_null + SMP$GL_CPU_DATA[j]->cpu$l_nullcpu;
	    total_cpunull[BannerActiveCpuCount-1] = 
		SMP$GL_CPU_DATA[j]->cpu$l_nullcpu;
/*
 * Interrupt time should include NULL time, so subtract NULL time 
 * from Interrupt time, if that is the case.
 */
		if (SMP$GL_CPU_DATA[j]->cpu$l_interrupt >=
		    SMP$GL_CPU_DATA[j]->cpu$l_nullcpu)
		    {
		    total_interrupt = total_interrupt + 
			SMP$GL_CPU_DATA[j]->cpu$l_interrupt -
			SMP$GL_CPU_DATA[j]->cpu$l_nullcpu;
		    total_time = total_time + 
			SMP$GL_CPU_DATA[j]->cpu$l_interrupt -
			SMP$GL_CPU_DATA[j]->cpu$l_nullcpu;
		    total_cputime[BannerActiveCpuCount-1] = 
			total_cputime[BannerActiveCpuCount-1] + 
			SMP$GL_CPU_DATA[j]->cpu$l_interrupt -
			SMP$GL_CPU_DATA[j]->cpu$l_nullcpu;
		    }
		else
		    {
		    total_interrupt = total_interrupt + 
			SMP$GL_CPU_DATA[j]->cpu$l_interrupt;
		    total_time = total_time + 
			SMP$GL_CPU_DATA[j]->cpu$l_interrupt;
		    total_cputime[BannerActiveCpuCount-1] = 
			total_cputime[BannerActiveCpuCount-1] + 
			SMP$GL_CPU_DATA[j]->cpu$l_interrupt;
		    }
	    }
	j++;
	}


    if (BannerActiveCpuCount>4)
	BannerActiveCpuCount = 4;

    if (BannerLastActiveCpuCount != BannerActiveCpuCount)
	{
	XClearWindow (XtDisplay(Bnr.cpu_widget), XtWindow(Bnr.cpu_widget));
	BannerCpuSetup();
	BannerCpuRefresh();	
	return;
	}
/*
* subtract the count we got last time
*/
    ftemp = total_time;
    total_time = total_time - last_total;

    if (total_time == 0)
	return;


/*
 * make sure we cant get a devide by 0!
 */
    null_size = total_null - null_mode;
    if (null_size < 1)
	null_size = 1;

    fbar_height = bar_height;
    if (Bnr.Param_Bits.cpu_multi_hist)
	for (i=0;  i<BannerActiveCpuCount;  i++)
	    {
	    float null_size, ftemp;
	    ftemp = total_cputime[i];
	    total_cputime[i] = total_cputime[i] - last_cputotal[i];
	    if (total_cputime[i] < 1)
		total_cputime[i] = 1;
	    null_size = total_cpunull[i] - null_cpumode[i];
	    if (null_size < 1)
		null_size = 1;
	    null_cpumode[i] =  total_cpunull[i];
	    last_cputotal[i] = ftemp;
	    if (total_cputime[i] > 0)
		height[i] = fbar_height / 
		    (total_cputime[i]/(null_size));
	    if (height[i] < 0)
		height[i] = 0;
	    }
	else
	    {
	    height[0] = fbar_height / (total_time/(null_size));
	    }
    fbar_height = Bnr.cpu_height - 3*Bnr.font_height;
    modes_height = fbar_height / (total_time/(null_size));
    if (modes_height >= fbar_height)
	modes_height = fbar_height - 1;
    if (modes_height < 0)
	modes_height = 0;

/*
 * Now update the modes display if we have open.
 */
    if (cpu_modes)
	{
/*
 * first null time
 */

	BannerDrawCpuHist (XtDisplay(Bnr.cpu_widget), XtWindow (Bnr.cpu_widget),
		    BannerGC, 
		    n_start, 
		    Bnr.cpu_height - Bnr.font_height - modes_height, 
		    Bnr.font_width, 
		    modes_height, 
		    Bnr.cpu_height - 3*Bnr.font_height,
		    FillOpaqueStippled,
		    0);


	null_size = modes_height;

	

	
/*
 * User mode time time, for the time being the invers of our CPU time.
 */
	cpu_ticks = 0;
	if (total_user - user_mode > 0)
	    cpu_ticks = total_time/(total_user - user_mode);
	mode_size = 0;
	if (cpu_ticks > 0)
	    mode_size = fbar_height/cpu_ticks;

	modes_height = mode_size;
	if (modes_height >= fbar_height)
	    modes_height = fbar_height - 1;

	BannerDrawCpuHist (XtDisplay(Bnr.cpu_widget), XtWindow (Bnr.cpu_widget),
	    BannerGC, u_start, 
		Bnr.cpu_height - Bnr.font_height - modes_height, 
		    Bnr.font_width, 
		    modes_height,
		    Bnr.cpu_height - 3*Bnr.font_height,
		    FillSolid,
		    1);

/*
 * Super mode time time, for the time being the invers of our CPU time.
 */
	cpu_ticks = 0;
	if (total_super - super_mode > 0)
	    cpu_ticks = total_time/(total_super - super_mode);
	mode_size = 0;
	if (cpu_ticks > 0)
	    mode_size = fbar_height/cpu_ticks;

	modes_height = mode_size;
	if (modes_height >= fbar_height)
	    modes_height = fbar_height - 1;

	BannerDrawCpuHist (XtDisplay(Bnr.cpu_widget), XtWindow (Bnr.cpu_widget),
	    BannerGC, s_start, 
		Bnr.cpu_height - Bnr.font_height - modes_height, 
		    Bnr.font_width, 
		    modes_height,
		    Bnr.cpu_height - 3*Bnr.font_height,
		    FillSolid,
		    1);


/*
 * Interrupt mode time time, for the time being the invers of our CPU time.
 */
	cpu_ticks = 0;
	if (total_interrupt - interrupt_mode > 0)
	    cpu_ticks = total_time/(total_interrupt - interrupt_mode);
	mode_size = 0;
	if (cpu_ticks > 0)
	    mode_size = fbar_height/cpu_ticks;

	modes_height = mode_size;
	if (modes_height >= fbar_height)
	    modes_height = fbar_height - 1;

	BannerDrawCpuHist (XtDisplay(Bnr.cpu_widget), XtWindow (Bnr.cpu_widget),
	    BannerGC, i_start, 
		Bnr.cpu_height - Bnr.font_height - modes_height, 
		    Bnr.font_width, 
		    modes_height,
		    Bnr.cpu_height - 3*Bnr.font_height,
		    FillSolid,
		    1);



/*
 * Executive mode time time, for the time being the invers of our CPU time.
 */
	cpu_ticks = 0;
	if (total_exec - exec_mode > 0)
	    cpu_ticks = total_time/(total_exec - exec_mode);
	mode_size = 0;
	if (cpu_ticks > 0)
	    mode_size = fbar_height/cpu_ticks;

	modes_height = mode_size;
	if (modes_height >= fbar_height)
	    modes_height = fbar_height - 1;

	BannerDrawCpuHist (XtDisplay(Bnr.cpu_widget), XtWindow (Bnr.cpu_widget),
	    BannerGC, e_start, 
		Bnr.cpu_height - Bnr.font_height - modes_height, 
		    Bnr.font_width, 
		    modes_height,
		    Bnr.cpu_height - 3*Bnr.font_height,
		    FillSolid,
		    1);


/*
 * kernel mode time time, for the time being the invers of our CPU time.
 */
	cpu_ticks = 0;
	if (total_kernel - kernel_mode > 0)
	    cpu_ticks = total_time/(total_kernel - kernel_mode);
	mode_size = 0;
	if (cpu_ticks > 0)
	    mode_size = fbar_height/cpu_ticks;

	modes_height = mode_size;
	if (modes_height >= fbar_height)
	    modes_height = fbar_height - 1;

	BannerDrawCpuHist (XtDisplay(Bnr.cpu_widget), XtWindow (Bnr.cpu_widget),
	    BannerGC, k_start, 
		Bnr.cpu_height - Bnr.font_height - modes_height, 
		    Bnr.font_width, 
		    modes_height,
		    fbar_height,
		    FillSolid,
		    1);


	}

/*
 * Now remember the current values, accumilated fro each CPU
 */
    last_total = ftemp;

    interrupt_mode = total_interrupt;
    kernel_mode = total_kernel;
    exec_mode = total_exec;
    user_mode = total_user;
    super_mode = total_super;
    null_mode = total_null;

/*
 * remember the tick size
 */
    if (quadtime[0] < min_tick)
	min_tick = quadtime[0];

    if (min_tick < 100*Bnr.cpu_update)
	min_tick = 100*Bnr.cpu_update;

    ticks = quadtime[0] / min_tick;
    if (ticks < 1)
	ticks = 1;

/*
 * Now remember the current values
 */


    last_time[0] = curr_time[0];
    last_time[1] = curr_time[1];


/*
 * now move the window to the right by the number of ticks
 */
    {
    int width, x_pos;

    if (ticks > Bnr.cpu_count)
	ticks = Bnr.cpu_count;

    for (j=0;  j<BannerActiveCpuCount;  j++)
	{
	if (height[j] < 0 )
	    height[j] = 1;
	if (height[j] > bar_height)
	    height[j] = bar_height;

	memcpy(&cpu_hist[j][2], &cpu_hist[j][ticks+2], 
	    (Bnr.cpu_count - ticks) * sizeof (XPoint) );
	x_pos = bar_x + ticks;
	width = bar_x1 - x_pos;
	XCopyArea (XtDisplay(Bnr.cpu_widget), XtWindow(Bnr.cpu_widget),
	    XtWindow(Bnr.cpu_widget), BannerGC, x_pos, seg_height * j, 
	    width, seg_height, bar_x, seg_height * j);
/*
 * Now update the array with our new possitions.
 */

	for (i=2 + Bnr.cpu_count - ticks;  i <= 2 + Bnr.cpu_count;  i++)
	    cpu_hist[j][i].y = seg_height*j +  height[j];
/*
 * now update the X positions.
 */

	for (i = 2;  i <= Bnr.cpu_count+1;  i++)
	    {
	    int x_pos;

	    x_pos = bar_x + (i - 2);
	    cpu_hist[j][i].x = x_pos;
	    }
/*
 * If we want just a single histogram then exit at 0
 */
	if (!Bnr.Param_Bits.cpu_multi_hist)
	    break;
	}
    }
/*
 * O.K now we can paint our screen
 */
/* 
 * Now repaint the area we had filled in the histogram
 */
    for (j=0; j < BannerActiveCpuCount; j++)
    {
	cpu_hist[j][0].x = bar_x;
	cpu_hist[j][0].y = seg_height * j;
	cpu_hist[j][1].x = bar_x;
	cpu_hist[j][1].y = seg_height * j + bar_height;
	cpu_hist[j][Bnr.cpu_count + 2].x = bar_x1;
	cpu_hist[j][Bnr.cpu_count + 2].y = seg_height * j + bar_height;
	cpu_hist[j][Bnr.cpu_count + 3].x = bar_x1;
	cpu_hist[j][Bnr.cpu_count + 3].y = seg_height * j;

	XSetBackground (XtDisplay(Bnr.cpu_widget), BannerGC,
	    Bnr.foreground);
	XSetForeground (XtDisplay(Bnr.cpu_widget), BannerGC,
	    Bnr.background);

	XFillPolygon (XtDisplay(Bnr.cpu_widget), XtWindow(Bnr.cpu_widget),
	    BannerGC, &cpu_hist[j][0], p_size, Complex, CoordModeOrigin);
	
	XSetForeground (XtDisplay(Bnr.cpu_widget), BannerGC,
	    Bnr.foreground);
	XSetBackground (XtDisplay(Bnr.cpu_widget), BannerGC,
	    Bnr.background);


	cpu_hist[j][0].x = bar_x;
	cpu_hist[j][0].y = seg_height * j + bar_height;
	cpu_hist[j][1].x = bar_x;
	cpu_hist[j][1].y = seg_height * j + bar_height;
	cpu_hist[j][Bnr.cpu_count + 2].x = bar_x1;
	cpu_hist[j][Bnr.cpu_count + 2].y = seg_height * j + bar_height;
	cpu_hist[j][Bnr.cpu_count + 3].x = bar_x1;
	cpu_hist[j][Bnr.cpu_count + 3].y = seg_height * j + bar_height;

	XFillPolygon (XtDisplay(Bnr.cpu_widget), XtWindow(Bnr.cpu_widget),
	    BannerGC, &cpu_hist[j][0], p_size, Complex, CoordModeOrigin);

/*
 * If we want just a single histogram then exit at 0
 */
	if (!Bnr.Param_Bits.cpu_multi_hist)
	    break;

    }
    
}
