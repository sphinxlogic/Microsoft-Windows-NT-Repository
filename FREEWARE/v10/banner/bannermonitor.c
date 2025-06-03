#module BannerMonitor "V1.0"

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
#include "dvidef.h"
#include "MrmAppl.h"


#include "Banner"


/*
**
**  MACRO DEFINITIONS
**
**/

int BannerDeviceCount();

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
extern   Mon$_Blk    Mon;
extern	 GC	     BannerGC;
extern	 XGCValues   GcValues;

globalref int MON_V_NI_MNTVERIP, MON_V_NI_BUSY, MON_V_NI_SYS_MV;

static int OLD_MON_V_NI_MNTVERIP, OLD_MON_V_NI_BUSY, OLD_MON_V_NI_SYS_MV;

globalref int PMS$GL_PREADIO, PMS$GL_PWRITIO, PMS$GL_FAULTS, 
	   SCH$GL_MFYCNT, SCH$GL_FREECNT;

extern void BannerSignalError();

static int device_iocnt[3] = {0, 0, 0};
static int device_rate[3] = {0, 0, 0};
static int device_lnblnk[3] = {0, 0, 0};

#define lnb_size (12 + 64)	/* Header + 64 bytes names
				 *      ----------------------
				 * (0)  !   forward link     !
				 *      ----------------------
				 * (1)  !   Device unit count!
				 *      ----------------------
				 * (2)  !   64-byte name     !
				 *      !                    !
				 *      !                    !
				 *      !                    !
				 *      ----------------------
				 */

static int lnb_pool[lnb_size * 100];

static char dua2_string[] = "DUA2:";
static int dua2[2] = {5, &dua2_string};
static char dua1_string[] = "DUA1:";
static int dua1[2] = {5, &dua1_string};
static char dua0_string[] = "DUA0:";
static int dua0[2] = {5, &dua0_string};


static int tw;
static int w5w;
static int w8w;
static int smp_size;
static int old_smp_size = 0;
static int remoteio_size;

static int Mon_update_count, Mon_update_number;
static int Mon_init = 0;

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	BannerMonSetup ()
**
**      This is the setup routine for the Banner monitor, it initialises all
**	the data structurtes for the monitor section of Banner.
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
BannerMonSetup()
{
int nmsize;
/* 
 * setup standard text widths
 */
    tw = XTextWidth(Bnr.font_ptr, "Free Pages ", 11);
    w5w = XTextWidth(Bnr.font_ptr, "88888", 5);
    w8w = XTextWidth(Bnr.font_ptr, "88888888", 8);
    remoteio_size = XTextWidth(Bnr.font_ptr, "RemoteIO ", 9);

/*
 * set up disk names
 */

    if (Bnr.disk_0 != NULL)
	{
	dua0[0] = name_size (Bnr.disk_0) - 1;
	dua0[1] = Bnr.disk_0;
	nmsize = XTextWidth(Bnr.font_ptr, Bnr.disk_0_name, 
		name_size(Bnr.disk_0_name) - 1);
	if (nmsize > tw) tw = nmsize;
	}
    else
	{
	Bnr.disk_0 = dua0_string;
	Bnr.disk_0_name = dua0_string;
	}

    if (Bnr.disk_1 != NULL)
	{
	dua1[0] = name_size (Bnr.disk_1) - 1; 
	dua1[1] = Bnr.disk_1;
	nmsize = XTextWidth(Bnr.font_ptr, Bnr.disk_1_name, 
		name_size(Bnr.disk_1_name) - 1);
	if (nmsize > tw) tw = nmsize;
	}
    else
	{
	Bnr.disk_1_name = dua1_string;
	Bnr.disk_1 = dua1_string;
	}

    if (Bnr.disk_2 != NULL)
	{
	dua2[0] = name_size (Bnr.disk_2) - 1;
	dua2[1] = Bnr.disk_2;
	nmsize = XTextWidth(Bnr.font_ptr, Bnr.disk_2_name, 
		name_size(Bnr.disk_2_name) - 1);
	if (nmsize > tw) tw = nmsize;
	}
    else
	{
	Bnr.disk_2_name = dua2_string;
	Bnr.disk_2 = dua2_string;
	}


    Mon_update_count = 1;
    if (Bnr.monitor_update == 0)
	Mon_update_number = Bnr.cpu_update;
    else
	Mon_update_number = Bnr.monitor_update;

    device_logicals ();

    Mon_init = 1;
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	BannerMonRefresh ()
**
**      This routine rewrites all the static information display by the 
**	Monitor section of Banner.
**
**  FORMAL PARAMETERS:
**
**
**  IMPLICIT INPUTS:
**
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
BannerMonRefresh()
{
    int lx_pos, rx_pos, x_pos, y_pos;
    char string[] = "                ";

    if (!Mon_init)
	BannerMonSetup ();

    x_pos = Bnr.font_height/2;
    y_pos = Bnr.font_height;

    rx_pos = tw + Bnr.font_height/2;
    lx_pos = rx_pos + w5w + Bnr.font_height/2;

    if (Mon.dua2)
	{
	BannerWriteText (XtDisplay(Bnr.mon_widget), XtWindow (Bnr.mon_widget),
	    x_pos, y_pos, Bnr.disk_2_name, 1, 
	    tw);
	if (device_iocnt[2] != 0)
	    {
	    sprintf (string, "%d", device_rate[2]); 
	    if (device_rate[2] != 0)
		BannerWriteText (XtDisplay(Bnr.mon_widget), XtWindow (Bnr.mon_widget),
		    rx_pos, y_pos, string, 3, 
		    w5w);
	    sprintf (string, "%d", device_iocnt[2]); 
	    BannerWriteText (XtDisplay(Bnr.mon_widget), XtWindow (Bnr.mon_widget),
		lx_pos, y_pos, string, 3, 
		w8w);
	    }
	else
	    {
	    BannerWriteText (XtDisplay(Bnr.mon_widget), XtWindow (Bnr.mon_widget),
		rx_pos, y_pos, "*****", 3, 
		w5w);
	    BannerWriteText (XtDisplay(Bnr.mon_widget), XtWindow (Bnr.mon_widget),
		lx_pos, y_pos, "********", 3, 
		w8w);
	    }
	y_pos = y_pos + Bnr.font_height;
	}

    if (Mon.dua1)
	{
	BannerWriteText (XtDisplay(Bnr.mon_widget), XtWindow (Bnr.mon_widget),
	    x_pos, y_pos, Bnr.disk_1_name, 1, 
	    tw);
	if (device_iocnt[1] != 0)
	    {
	    sprintf (string, "%d", device_rate[1]); 
	    if (device_rate[1] != 0)
		BannerWriteText (XtDisplay(Bnr.mon_widget), XtWindow (Bnr.mon_widget),
		    rx_pos, y_pos, string, 3, 
		    w5w);
	    sprintf (string, "%d", device_iocnt[1]); 
	    BannerWriteText (XtDisplay(Bnr.mon_widget), XtWindow (Bnr.mon_widget),
		lx_pos, y_pos, string, 3, 
		    w8w);
	    }
	else
	    {
	    BannerWriteText (XtDisplay(Bnr.mon_widget), XtWindow (Bnr.mon_widget),
		rx_pos, y_pos, "*****", 3, 
		w5w);
	    BannerWriteText (XtDisplay(Bnr.mon_widget), XtWindow (Bnr.mon_widget),
		lx_pos, y_pos, "********", 3, 
		w8w);
	    }
	y_pos = y_pos + Bnr.font_height;
	}
    if (Mon.dua0)
	{
	BannerWriteText (XtDisplay(Bnr.mon_widget), XtWindow (Bnr.mon_widget),
	    x_pos, y_pos, Bnr.disk_0_name, 1, 
	    tw);
	if (device_iocnt[0] != 0)
	    {
	    sprintf (string, "%d", device_rate[0]); 
	    if (device_rate[0] != 0)
		BannerWriteText (XtDisplay(Bnr.mon_widget), XtWindow (Bnr.mon_widget),
		    rx_pos, y_pos, string, 3, 
		    w5w);
	    sprintf (string, "%d", device_iocnt[0]); 
	    BannerWriteText (XtDisplay(Bnr.mon_widget), XtWindow (Bnr.mon_widget),
		lx_pos, y_pos, string, 3, 
		w8w);
	    }
	else
	    {
	    BannerWriteText (XtDisplay(Bnr.mon_widget), XtWindow (Bnr.mon_widget),
		rx_pos, y_pos, "*****", 3, 
		w5w);
	    BannerWriteText (XtDisplay(Bnr.mon_widget), XtWindow (Bnr.mon_widget),
		lx_pos, y_pos, "********", 3, 
		w8w);
	    }
	y_pos = y_pos + Bnr.font_height;
	}

    if (Mon.ni_io)
	{
	BannerWriteText (XtDisplay(Bnr.mon_widget), XtWindow (Bnr.mon_widget),
	    x_pos, y_pos, "RemoteIO", 1, 
	    tw);
	if (Mon.ni_io_value != 0)
	    {
	    sprintf (string, "%d", Mon.ni_io_rate); 
	    if (Mon.ni_io_rate != 0)
		BannerWriteText (XtDisplay(Bnr.mon_widget), XtWindow (Bnr.mon_widget),
		    rx_pos, y_pos, string, 3, 
		    w5w);
	    sprintf (string, "%d", Mon.ni_io_value); 
	    BannerWriteText (XtDisplay(Bnr.mon_widget), XtWindow (Bnr.mon_widget),
		lx_pos, y_pos, string, 3, 
		w8w);
	    }
	else
	    {
	    BannerWriteText (XtDisplay(Bnr.mon_widget), XtWindow (Bnr.mon_widget),
		rx_pos, y_pos, "*****", 3, 
		w5w);
	    BannerWriteText (XtDisplay(Bnr.mon_widget), XtWindow (Bnr.mon_widget),
		lx_pos, y_pos, "********", 3, 
		w8w);
	    }
	y_pos = y_pos + Bnr.font_height;
	}

    if (Mon.pio)
	{
	BannerWriteText (XtDisplay(Bnr.mon_widget), XtWindow (Bnr.mon_widget),
	    x_pos, y_pos, "Paging IO", 1,
	    tw);
	if (Mon.pageio != 0)
	    {
	    sprintf (string, "%d", Mon.pageio_rate); 
	    if (Mon.pageio_rate != 0)
		BannerWriteText (XtDisplay(Bnr.mon_widget), XtWindow (Bnr.mon_widget),
		    rx_pos, y_pos, string, 3, 
		    w5w);
	    sprintf (string, "%d", Mon.pageio); 
	    BannerWriteText (XtDisplay(Bnr.mon_widget), XtWindow (Bnr.mon_widget),
		lx_pos, y_pos, string, 3,
		w8w);
	    }
	else
	    {
	    BannerWriteText (XtDisplay(Bnr.mon_widget), XtWindow (Bnr.mon_widget),
		rx_pos, y_pos, "*****", 3, 
		w5w);
	    BannerWriteText (XtDisplay(Bnr.mon_widget), XtWindow (Bnr.mon_widget),
		lx_pos, y_pos, "********", 3, 
		w8w);
	    }
	y_pos = y_pos + Bnr.font_height;
	}

    if (Mon.sysflt)
	{
	BannerWriteText (XtDisplay(Bnr.mon_widget), XtWindow (Bnr.mon_widget),
	    x_pos, y_pos, "Sys Faults", 1, 
	    tw);
	if (Mon.sysfault != 0)
	    {
	    sprintf (string, "%d", Mon.sysfault_rate); 
	    if (Mon.sysfault_rate != 0)
		BannerWriteText (XtDisplay(Bnr.mon_widget), XtWindow (Bnr.mon_widget),
		    rx_pos, y_pos, string, 3,
		    w5w);
	    sprintf (string, "%d", Mon.sysfault); 
	    BannerWriteText (XtDisplay(Bnr.mon_widget), XtWindow (Bnr.mon_widget),
		lx_pos, y_pos, string, 3,
		w8w);
	    }
	else
	    {
	    BannerWriteText (XtDisplay(Bnr.mon_widget), XtWindow (Bnr.mon_widget),
		rx_pos, y_pos, "*****", 3, 
		w5w);
	    BannerWriteText (XtDisplay(Bnr.mon_widget), XtWindow (Bnr.mon_widget),
		lx_pos, y_pos, "********", 3, 
		w8w);
	    }
	y_pos = y_pos + Bnr.font_height;
	}

    if (Mon.pf)
	{
	BannerWriteText (XtDisplay(Bnr.mon_widget), XtWindow (Bnr.mon_widget),
	    x_pos, y_pos, "Faults", 1, 
	    tw);
	if (Mon.fault != 0)
	    {
	    sprintf (string, "%d", Mon.fault_rate); 
	    if (Mon.fault_rate != 0)
		BannerWriteText (XtDisplay(Bnr.mon_widget), XtWindow (Bnr.mon_widget),
		    rx_pos, y_pos, string, 3,
		    w5w);
	    sprintf (string, "%d", Mon.fault); 
	    BannerWriteText (XtDisplay(Bnr.mon_widget), XtWindow (Bnr.mon_widget),
		lx_pos, y_pos, string, 3,
		w8w);
	    }
	else
	    {
	    BannerWriteText (XtDisplay(Bnr.mon_widget), XtWindow (Bnr.mon_widget),
		rx_pos, y_pos, "*****", 3, 
		w5w);
	    BannerWriteText (XtDisplay(Bnr.mon_widget), XtWindow (Bnr.mon_widget),
		lx_pos, y_pos, "********", 3, 
		w8w);
	    }
	y_pos = y_pos + Bnr.font_height;
	}

    if (Mon.mp)
	{
	BannerWriteText (XtDisplay(Bnr.mon_widget), XtWindow (Bnr.mon_widget),
	    x_pos, y_pos, "Modified", 1, 
	    tw);
	if (Mon.mfycnt != 0)
	    {
	    sprintf (string, "%d", Mon.mfycnt_rate); 
	    if (Mon.mfycnt_rate != 0)
		BannerWriteText (XtDisplay(Bnr.mon_widget), XtWindow (Bnr.mon_widget),
		    rx_pos, y_pos, string, 3,
		    w5w);
	    sprintf (string, "%d", Mon.mfycnt); 
	    BannerWriteText (XtDisplay(Bnr.mon_widget), XtWindow (Bnr.mon_widget),
		lx_pos, y_pos, string, 3, 
		w8w);
	    }
	else
	    {
	    BannerWriteText (XtDisplay(Bnr.mon_widget), XtWindow (Bnr.mon_widget),
		rx_pos, y_pos, "*****", 3, 
		w5w);
	    BannerWriteText (XtDisplay(Bnr.mon_widget), XtWindow (Bnr.mon_widget),
		lx_pos, y_pos, "********", 3, 
		w8w);
	    }
	y_pos = y_pos + Bnr.font_height;
	}

    if (Mon.fp)
	{
	BannerWriteText (XtDisplay(Bnr.mon_widget), XtWindow (Bnr.mon_widget),
	    x_pos, y_pos, "Free Pages", 1, 
	    tw);
	if (Mon.freecnt != 0)
	    {
	    sprintf (string, "%d", Mon.freecnt_rate); 
	    if (Mon.freecnt_rate != 0)
		BannerWriteText (XtDisplay(Bnr.mon_widget), XtWindow (Bnr.mon_widget),
		    rx_pos, y_pos, string, 3, 
		    w5w);
	    sprintf (string, "%d", Mon.freecnt); 
	    BannerWriteText (XtDisplay(Bnr.mon_widget), XtWindow (Bnr.mon_widget),
		lx_pos, y_pos, string, 3,
		w8w);
	    }
	else
	    {
	    BannerWriteText (XtDisplay(Bnr.mon_widget), XtWindow (Bnr.mon_widget),
		rx_pos, y_pos, "*****", 3, 
		w5w);
	    BannerWriteText (XtDisplay(Bnr.mon_widget), XtWindow (Bnr.mon_widget),
		lx_pos, y_pos, "********", 3, 
		w8w);
	    }
	y_pos = y_pos + Bnr.font_height;
	}
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	device_display
**
**      Counts the number of IO's for one particular device specification list.
**
**  FORMAL PARAMETERS:
**
**      idx	0:2
**	lx_pos
**	x_pos
**	y_pos
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
device_display(idx, lx_pos, x_pos, y_pos)
int idx, lx_pos, x_pos, y_pos;
{
    int iocnt, rate, temp, len;
    char string[] = "                ";

    iocnt = BannerDeviceCount (idx, Bnr.Param_Bits.freeiocnt, NULL);
    rate = iocnt - device_iocnt[idx];
    if (rate != device_rate[idx])
	{
	device_rate[idx] = rate;
	sprintf (string, "%d", rate); 
	if (rate == 0)
	    BannerWriteText (XtDisplay(Bnr.mon_widget), 
		XtWindow(Bnr.mon_widget),
		x_pos, y_pos, "     ", 3, 
		w5w);
	else 	    
	    BannerWriteText (XtDisplay(Bnr.mon_widget), 
		XtWindow(Bnr.mon_widget),
		x_pos, y_pos, string, 3, 
		w5w);
	}
    if (iocnt != device_iocnt[idx])
	{
	device_iocnt[idx] = iocnt;
	sprintf (string, "%d", iocnt);
	BannerWriteText (XtDisplay(Bnr.mon_widget), 
	    XtWindow(Bnr.mon_widget),
	    lx_pos, y_pos, string, 3, 
	    w8w);
	}
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	BannerMon ()
**
**      This routine runs the Monitor display of banner.
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
BannerMon()
{
    int rate, temp, x_pos, y_pos, lx_pos, iocnt;
    char string[] = "                ";

    if (!Mon_init)
	BannerMonRefresh ();

    Mon_update_count --;
    if (Mon_update_count > 0)
	return;
	
    Mon_update_count = Mon_update_number;


    x_pos = tw + Bnr.font_height/2;
    lx_pos = x_pos + w5w + Bnr.font_height/2;
    y_pos = Bnr.font_height;

    iocnt = 0;
/*
 * First Dua2:
 */
    if (Mon.dua2)
	{
	device_display (2, lx_pos, x_pos, y_pos);
	y_pos = y_pos + Bnr.font_height;
	}
/*
 * Next Dua1:
 */
    if (Mon.dua1)
	{
	device_display (1, lx_pos, x_pos, y_pos);
	y_pos = y_pos + Bnr.font_height;
	}
/*
 * Next Dua0
 */
    if (Mon.dua0)
	{
	device_display (0, lx_pos, x_pos, y_pos);
	y_pos = y_pos + Bnr.font_height;
	}
/*
 * Next Remote IO
 */
    if (Mon.ni_io)
	{
	iocnt = REMOTE_IO_COUNT ();
	rate = iocnt - Mon.ni_io_value;
	if (rate != Mon.ni_io_rate)
	    {
	    Mon.ni_io_rate = rate;
	    sprintf (string, "%d", rate); 
	    if (rate == 0)
		BannerWriteText (XtDisplay(Bnr.mon_widget), 
		    XtWindow(Bnr.mon_widget),
		    x_pos, y_pos, "     ", 3, 
		    w5w);
	    else 	    
		BannerWriteText (XtDisplay(Bnr.mon_widget), 
		    XtWindow(Bnr.mon_widget),
		    x_pos, y_pos, string, 3, 
		    w5w);
/*
 * force any flag bits to be repainted as well
 */
	    OLD_MON_V_NI_SYS_MV = -1;
	    }
	if (iocnt != Mon.ni_io_value)
	    {
	    Mon.ni_io_value = iocnt;
	    sprintf (string, "%d", iocnt);
	    BannerWriteText (XtDisplay(Bnr.mon_widget), 
		XtWindow(Bnr.mon_widget),
		lx_pos, y_pos, string, 3, 
		w8w);
	    }
	if (OLD_MON_V_NI_SYS_MV != MON_V_NI_SYS_MV
	||  OLD_MON_V_NI_MNTVERIP != MON_V_NI_MNTVERIP
	||  OLD_MON_V_NI_BUSY != MON_V_NI_BUSY)
	    {
	    char foo[10] = "";
	    int size;

	    if ( MON_V_NI_SYS_MV != 0 ||
		 MON_V_NI_MNTVERIP != 0 ||
		 MON_V_NI_BUSY != 0)
		 sprintf (foo, "%s%s", foo, "(");
		 
	    if (MON_V_NI_SYS_MV != 0)
		sprintf (foo, "%s%s", foo, "S");
	    if (MON_V_NI_MNTVERIP != 0)
		sprintf (foo, "%s%s", foo, "M");
	    if (MON_V_NI_BUSY != 0)
		sprintf (foo, "%s%s", foo, "P");

	    if ( MON_V_NI_SYS_MV != 0 ||
		 MON_V_NI_MNTVERIP != 0 ||
		 MON_V_NI_BUSY != 0)
		 sprintf (foo, "%s%s", foo, ")");

	    smp_size = XTextWidth(Bnr.font_ptr, foo, name_size(foo) - 1);
    
	    size = smp_size;
	    if (old_smp_size > size)
		size=old_smp_size;

	    BannerWriteText (XtDisplay(Bnr.mon_widget), 
		XtWindow(Bnr.mon_widget),
		remoteio_size + Bnr.font_height/2, y_pos, foo, 1, 
		size);

	    old_smp_size = smp_size;
	    OLD_MON_V_NI_SYS_MV = MON_V_NI_SYS_MV;
	    OLD_MON_V_NI_MNTVERIP = MON_V_NI_MNTVERIP;
	    OLD_MON_V_NI_BUSY = MON_V_NI_BUSY;
	    }
	y_pos = y_pos + Bnr.font_height;
	}
/*
 * pio
 */

    if (Mon.pio)
	{
	temp = PMS$GL_PREADIO + PMS$GL_PWRITIO;
	rate = temp - Mon.pageio;
	if (rate != Mon.pageio_rate)
	    {
	    Mon.pageio_rate = rate;
	    sprintf (string, "%d", rate); 
	    if (rate == 0)
		BannerWriteText (XtDisplay(Bnr.mon_widget), 
		    XtWindow(Bnr.mon_widget),
		    x_pos, y_pos, "     ", 3, 
		    w5w);
	    else 	    
		BannerWriteText (XtDisplay(Bnr.mon_widget), 
		    XtWindow(Bnr.mon_widget),
		    x_pos, y_pos, string, 3, 
		    w5w);
	    }
	if (temp != Mon.pageio)
	    {
	    Mon.pageio = temp;
	    sprintf (string, "%d", temp);
	    BannerWriteText (XtDisplay(Bnr.mon_widget), 
		XtWindow(Bnr.mon_widget),
		lx_pos, y_pos, string, 3, 
		w8w);
	    }
	y_pos = y_pos + Bnr.font_height;
	}
/*
 * sys pf
 */
    if (Mon.sysflt)
	{
	temp = BANNER_KERNEL_SYSFLTS ();
	rate = temp - Mon.sysfault;
	if (rate != Mon.sysfault_rate)
	    {
	    Mon.sysfault_rate = rate;
	    sprintf (string, "%d", rate); 
	    if (rate == 0)
		BannerWriteText (XtDisplay(Bnr.mon_widget), 
		    XtWindow(Bnr.mon_widget),
		    x_pos, y_pos, "     ", 3, 
		    w5w);
	    else 	    
		BannerWriteText (XtDisplay(Bnr.mon_widget), 
		    XtWindow(Bnr.mon_widget),
		    x_pos, y_pos, string, 3, 
		    w5w);
	    }
	if (temp != Mon.sysfault)
	    {
	    Mon.sysfault = temp;
	    sprintf (string, "%d", temp);
	    BannerWriteText (XtDisplay(Bnr.mon_widget), 
		XtWindow(Bnr.mon_widget),
		lx_pos, y_pos, string, 3, 
		w8w);
	    }
	y_pos = y_pos + Bnr.font_height;
	}
/*
 * pf
 */
    if (Mon.pf)
	{
	rate = PMS$GL_FAULTS - Mon.fault;
	if (rate != Mon.fault_rate)
	    {
	    Mon.fault_rate = rate;
	    sprintf (string, "%d", rate); 
	    if (rate == 0)
		BannerWriteText (XtDisplay(Bnr.mon_widget), 
		    XtWindow(Bnr.mon_widget),
		    x_pos, y_pos, "     ", 3, 
		    w5w);
	    else 	    
		BannerWriteText (XtDisplay(Bnr.mon_widget), 
		    XtWindow(Bnr.mon_widget),
		    x_pos, y_pos, string, 3, 
		    w5w);
	    }
	if (PMS$GL_FAULTS != Mon.fault)
	    {
	    Mon.fault = PMS$GL_FAULTS;
	    sprintf (string, "%d", PMS$GL_FAULTS);
	    BannerWriteText (XtDisplay(Bnr.mon_widget), 
		XtWindow(Bnr.mon_widget),
		lx_pos, y_pos, string, 3, 
		w8w);
	    }
	y_pos = y_pos + Bnr.font_height;
	}
/*
 *mp
 */
    if (Mon.mp)
	{
	rate = SCH$GL_MFYCNT - Mon.mfycnt;
	if (rate != Mon.mfycnt_rate)
	    {
	    Mon.mfycnt_rate = rate;
	    sprintf (string, "%d", rate); 
	    if (rate == 0)
		BannerWriteText (XtDisplay(Bnr.mon_widget), 
		    XtWindow(Bnr.mon_widget),
		    x_pos, y_pos, "     ", 3, 
		    w5w);
	    else 	    
		BannerWriteText (XtDisplay(Bnr.mon_widget), 
		    XtWindow(Bnr.mon_widget),
		    x_pos, y_pos, string, 3, 
		    w5w);
	    }
	if (SCH$GL_MFYCNT != Mon.mfycnt)
	    {
	    Mon.mfycnt = SCH$GL_MFYCNT;
	    sprintf (string, "%d", SCH$GL_MFYCNT);
	    BannerWriteText (XtDisplay(Bnr.mon_widget), 
		XtWindow(Bnr.mon_widget),
		lx_pos, y_pos, string, 3, 
		w8w);
	    }
	y_pos = y_pos + Bnr.font_height;
	}
/*
 * fp
 */
    if (Mon.fp)
	{
	rate = SCH$GL_FREECNT - Mon.freecnt;
	if (rate != Mon.freecnt_rate)
	    {
	    Mon.freecnt_rate = rate;
	    sprintf (string, "%d", rate); 
	    if (rate == 0)
		BannerWriteText (XtDisplay(Bnr.mon_widget), 
		    XtWindow(Bnr.mon_widget),
		    x_pos, y_pos, "     ", 3, 
		    w5w);
	    else 	    
		BannerWriteText (XtDisplay(Bnr.mon_widget), 
		    XtWindow(Bnr.mon_widget),
		    x_pos, y_pos, string, 3, 
		    w5w);
	    }
	if (SCH$GL_FREECNT != Mon.freecnt)
	    {
	    Mon.freecnt = SCH$GL_FREECNT;
	    sprintf (string, "%d", SCH$GL_FREECNT);
	    BannerWriteText (XtDisplay(Bnr.mon_widget), 
		XtWindow(Bnr.mon_widget),
		lx_pos, y_pos, string, 3, 
		w8w);
	    }
	y_pos = y_pos + Bnr.font_height;
	}
}
