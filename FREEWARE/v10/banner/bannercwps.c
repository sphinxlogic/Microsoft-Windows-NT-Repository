#module BannerCWPS "V1.0"

/*
**++
**  FACILITY:
**
**      The DECwindows Banner program
**
**  ABSTRACT:
**
**      This module contains all the code, to run the cwps monitor.
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
#include "MrmAppl.h"


#include "Banner"


/*
**
**  MACRO DEFINITIONS
**
**/

/*
 * now our runtime data structures
 */

extern	 Bnr$_Blk    Bnr;
extern   cwps$_Blk    cwps;
extern   cwps$_Blk    cwpsold;
extern	 GC	     BannerGC;
extern	 XGCValues   GcValues;

globalref int 
    PMS$GL_CWPS_MSGS_IN,
    PMS$GL_CWPS_MSGS_OUT,
    PMS$GL_CWPS_BYTES_IN,
    PMS$GL_CWPS_BYTES_OUT,
    PMS$GL_CWPS_GETJPI_IN,
    PMS$GL_CWPS_GETJPI_OUT,
    PMS$GL_CWPS_PCNTRL_IN,
    PMS$GL_CWPS_PCNTRL_OUT;
    PMS$GL_CWPS_RSRC_RECV,
    PMS$GL_CWPS_RSRC_SEND;

extern void BannerSignalError();

static int tw;
static int w5w;
static int w8w;

static int cwps_update_count, cwps_update_number;
static int cwps_init = 0;

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	BannerCWPSSetup ()
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
BannerCWPSSetup()
{
int nmsize;
/* 
 * setup standard text widths
 */
    tw = XTextWidth(Bnr.font_ptr, "GETJPI Out ", 10);
    w5w = XTextWidth(Bnr.font_ptr, "88888", 5);
    w8w = XTextWidth(Bnr.font_ptr, "88888888", 8);

    memset( &cwps, 0, sizeof(cwps));
    memset( &cwpsold, 0, sizeof(cwps));

    cwps_update_count = 1;
    if (Bnr.cwps_update == 0)
	cwps_update_number = Bnr.cpu_update;
    else
	cwps_update_number = Bnr.cwps_update;

    cwps_init = 1;
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	BannerCWPSRefresh ()
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
BannerCWPSRefresh()
{
    int lx_pos, rx_pos, x_pos, y_pos;
    char string[] = "                ";

    if (!cwps_init)
	BannerCWPSSetup ();

    x_pos = Bnr.font_height/2;
    y_pos = Bnr.font_height;

    rx_pos = tw + Bnr.font_height/2;
    lx_pos = rx_pos + w5w + Bnr.font_height/2;

	{
	BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
	    x_pos, y_pos, "Msg In", 1, 
	    tw);
	sprintf (string, "%d", cwpsold.msgs_in_rate); 
	if (cwpsold.msgs_in_rate != 0)
	    BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
		rx_pos, y_pos, string, 3, 
		w5w);
	sprintf (string, "%d", cwpsold.msgs_in); 
	BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
	    lx_pos, y_pos, string, 3, 
		w8w);
	y_pos = y_pos + Bnr.font_height;
	}

	{
	BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
	    x_pos, y_pos, "Msg Out", 1, 
	    tw);
	sprintf (string, "%d", cwpsold.msgs_out_rate); 
	if (cwpsold.msgs_out_rate != 0)
	    BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
		rx_pos, y_pos, string, 3, 
		w5w);
	sprintf (string, "%d", cwpsold.msgs_out); 
	BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
	    lx_pos, y_pos, string, 3, 
		w8w);
	y_pos = y_pos + Bnr.font_height;
	}

	{
	BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
	    x_pos, y_pos, "Kb In", 1, 
	    tw);
	sprintf (string, "%d", cwpsold.bytes_in_rate); 
	if (cwpsold.bytes_in_rate != 0)
	    BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
		rx_pos, y_pos, string, 3, 
		w5w);
	sprintf (string, "%d", cwpsold.bytes_in); 
	BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
	    lx_pos, y_pos, string, 3, 
	    w8w);
	y_pos = y_pos + Bnr.font_height;
	}

	{
	BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
	    x_pos, y_pos, "Kb Out", 1,
	    tw);
	sprintf (string, "%d", cwpsold.bytes_out_rate); 
	if (cwpsold.bytes_out_rate != 0)
	    BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
		rx_pos, y_pos, string, 3, 
		w5w);
	sprintf (string, "%d", cwpsold.bytes_out); 
	BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
	    lx_pos, y_pos, string, 3,
	    w8w);
	y_pos = y_pos + Bnr.font_height;
	}

	{
	BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
	    x_pos, y_pos, "GETJPI In", 1, 
	    tw);
	sprintf (string, "%d", cwpsold.getjpi_in_rate); 
	if (cwpsold.getjpi_in_rate != 0)
	    BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
		rx_pos, y_pos, string, 3,
		w5w);
	sprintf (string, "%d", cwpsold.getjpi_in); 
	BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
	    lx_pos, y_pos, string, 3,
	    w8w);
	y_pos = y_pos + Bnr.font_height;
	}

	{
	BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
	    x_pos, y_pos, "GETJPI Out", 1, 
	    tw);
	sprintf (string, "%d", cwpsold.getjpi_out_rate); 
	if (cwpsold.getjpi_out_rate != 0)
	    BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
		rx_pos, y_pos, string, 3,
		w5w);
	sprintf (string, "%d", cwpsold.getjpi_out); 
	BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
	    lx_pos, y_pos, string, 3, 
	    w8w);
	y_pos = y_pos + Bnr.font_height;
	}

	{
	BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
	    x_pos, y_pos, "Pcntrl I/O", 1, 
	    tw);
	sprintf (string, "%d", cwpsold.pcntrl_rate); 
	if (cwpsold.pcntrl_rate != 0)
	    BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
		rx_pos, y_pos, string, 3, 
		w5w);
	sprintf (string, "%d", cwpsold.pcntrl); 
	BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
	    lx_pos, y_pos, string, 3,
	    w8w);
	y_pos = y_pos + Bnr.font_height;
	}

	{
	BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
	    x_pos, y_pos, "RSRC R/S", 1, 
	    tw);
	sprintf (string, "%d", cwpsold.rsrc_rate); 
	if (cwpsold.rsrc_rate != 0)
	    BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
		rx_pos, y_pos, string, 3, 
		w5w);
	sprintf (string, "%d", cwpsold.rsrc); 
	BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
	    lx_pos, y_pos, string, 3,
	    w8w);
	y_pos = y_pos + Bnr.font_height;
	}

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	BannerCWPS ()
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
BannerCWPS()
{
    int rate, temp, x_pos, y_pos, lx_pos;
    char string[] = "                ";
    int status, iocnt, len; 

    if (!cwps_init)
	BannerCWPSRefresh ();

    cwps_update_count --;
    if (cwps_update_count > 0)
	return;
	
    cwps_update_count = cwps_update_number;

    x_pos = tw + Bnr.font_height/2;
    lx_pos = x_pos + w5w + Bnr.font_height/2;
    y_pos = Bnr.font_height;

    if (&PMS$GL_CWPS_MSGS_IN != 0)	/* Not there until VMS V5.2 */
	{
	cwps.msgs_in = PMS$GL_CWPS_MSGS_IN;
	cwps.msgs_out = PMS$GL_CWPS_MSGS_OUT;
	cwps.bytes_in = PMS$GL_CWPS_BYTES_IN/1024;
	cwps.bytes_out = PMS$GL_CWPS_BYTES_OUT/1024;
	cwps.getjpi_in = PMS$GL_CWPS_GETJPI_IN;
	cwps.getjpi_out = PMS$GL_CWPS_GETJPI_OUT;
	cwps.pcntrl = PMS$GL_CWPS_PCNTRL_IN + PMS$GL_CWPS_PCNTRL_OUT;
	cwps.rsrc = PMS$GL_CWPS_RSRC_RECV + PMS$GL_CWPS_RSRC_SEND;
	}
/*
 * Msgs In
 */
	{
	temp = cwps.msgs_in;
	rate = temp - cwpsold.msgs_in;
	if (rate != cwps.msgs_in_rate)
	    {
	    cwps.msgs_in_rate = rate;
	    sprintf (string, "%d", rate); 
	    if (rate == 0)
		BannerWriteText (XtDisplay(Bnr.cwps_widget), 
		    XtWindow(Bnr.cwps_widget),
		    x_pos, y_pos, "     ", 3, 
		    w5w);
	    else 	    
		BannerWriteText (XtDisplay(Bnr.cwps_widget), 
		    XtWindow(Bnr.cwps_widget),
		    x_pos, y_pos, string, 3, 
		    w5w);
	    }
	if (temp != cwpsold.msgs_in)
	    {
	    cwpsold.msgs_in = temp;
	    sprintf (string, "%d", temp);
	    BannerWriteText (XtDisplay(Bnr.cwps_widget), 
		XtWindow(Bnr.cwps_widget),
		lx_pos, y_pos, string, 3, 
		w8w);
	    }
	y_pos = y_pos + Bnr.font_height;
	}
/*
 * Msgs Out
 */
	{
	temp = cwps.msgs_out;
	rate = temp - cwpsold.msgs_out;
	if (rate != cwps.msgs_out_rate)
	    {
	    cwps.msgs_out_rate = rate;
	    sprintf (string, "%d", rate); 
	    if (rate == 0)
		BannerWriteText (XtDisplay(Bnr.cwps_widget), 
		    XtWindow(Bnr.cwps_widget),
		    x_pos, y_pos, "     ", 3, 
		    w5w);
	    else 	    
		BannerWriteText (XtDisplay(Bnr.cwps_widget), 
		    XtWindow(Bnr.cwps_widget),
		    x_pos, y_pos, string, 3, 
		    w5w);
	    }
	if (temp != cwpsold.msgs_out)
	    {
	    cwpsold.msgs_out = temp;
	    sprintf (string, "%d", temp);
	    BannerWriteText (XtDisplay(Bnr.cwps_widget), 
		XtWindow(Bnr.cwps_widget),
		lx_pos, y_pos, string, 3, 
		w8w);
	    }
	y_pos = y_pos + Bnr.font_height;
	}
/*
 * Bytes In
 */
	{
	temp = cwps.bytes_in;
	rate = temp - cwpsold.bytes_in;
	if (rate != cwps.bytes_in_rate)
	    {
	    cwps.bytes_in_rate = rate;
	    sprintf (string, "%d", rate); 
	    if (rate == 0)
		BannerWriteText (XtDisplay(Bnr.cwps_widget), 
		    XtWindow(Bnr.cwps_widget),
		    x_pos, y_pos, "     ", 3, 
		    w5w);
	    else 	    
		BannerWriteText (XtDisplay(Bnr.cwps_widget), 
		    XtWindow(Bnr.cwps_widget),
		    x_pos, y_pos, string, 3, 
		    w5w);
	    }
	if (temp != cwpsold.bytes_in)
	    {
	    cwpsold.bytes_in = temp;
	    sprintf (string, "%d", temp);
	    BannerWriteText (XtDisplay(Bnr.cwps_widget), 
		XtWindow(Bnr.cwps_widget),
		lx_pos, y_pos, string, 3, 
		w8w);
	    }
	y_pos = y_pos + Bnr.font_height;
	}
/*
 * Bytes out
 */
	{
	temp = cwps.bytes_out;
	rate = temp - cwpsold.bytes_out;
	if (rate != cwps.bytes_out_rate)
	    {
	    cwps.bytes_out_rate = rate;
	    sprintf (string, "%d", rate); 
	    if (rate == 0)
		BannerWriteText (XtDisplay(Bnr.cwps_widget), 
		    XtWindow(Bnr.cwps_widget),
		    x_pos, y_pos, "     ", 3, 
		    w5w);
	    else 	    
		BannerWriteText (XtDisplay(Bnr.cwps_widget), 
		    XtWindow(Bnr.cwps_widget),
		    x_pos, y_pos, string, 3, 
		    w5w);
	    }
	if (temp != cwpsold.bytes_out)
	    {
	    cwpsold.bytes_out = temp;
	    sprintf (string, "%d", temp);
	    BannerWriteText (XtDisplay(Bnr.cwps_widget), 
		XtWindow(Bnr.cwps_widget),
		lx_pos, y_pos, string, 3, 
		w8w);
	    }
	y_pos = y_pos + Bnr.font_height;
	}
/*
 * GetJpi In
 */
	{
	temp = cwps.getjpi_in;
	rate = temp - cwpsold.getjpi_in;
	if (rate != cwps.getjpi_in_rate)
	    {
	    cwps.getjpi_in_rate = rate;
	    sprintf (string, "%d", rate); 
	    if (rate == 0)
		BannerWriteText (XtDisplay(Bnr.cwps_widget), 
		    XtWindow(Bnr.cwps_widget),
		    x_pos, y_pos, "     ", 3, 
		    w5w);
	    else 	    
		BannerWriteText (XtDisplay(Bnr.cwps_widget), 
		    XtWindow(Bnr.cwps_widget),
		    x_pos, y_pos, string, 3, 
		    w5w);
	    }
	if (temp != cwpsold.getjpi_in)
	    {
	    cwpsold.getjpi_in = temp;
	    sprintf (string, "%d", temp);
	    BannerWriteText (XtDisplay(Bnr.cwps_widget), 
		XtWindow(Bnr.cwps_widget),
		lx_pos, y_pos, string, 3, 
		w8w);
	    }
	y_pos = y_pos + Bnr.font_height;
	}
/*
 * GetJpi Out
 */
	{
	temp = cwps.getjpi_out;
	rate = temp - cwpsold.getjpi_out;
	if (rate != cwps.getjpi_out_rate)
	    {
	    cwps.getjpi_out_rate = rate;
	    sprintf (string, "%d", rate); 
	    if (rate == 0)
		BannerWriteText (XtDisplay(Bnr.cwps_widget), 
		    XtWindow(Bnr.cwps_widget),
		    x_pos, y_pos, "     ", 3, 
		    w5w);
	    else 	    
		BannerWriteText (XtDisplay(Bnr.cwps_widget), 
		    XtWindow(Bnr.cwps_widget),
		    x_pos, y_pos, string, 3, 
		    w5w);
	    }
	if (temp != cwpsold.getjpi_out)
	    {
	    cwpsold.getjpi_out = temp;
	    sprintf (string, "%d", temp);
	    BannerWriteText (XtDisplay(Bnr.cwps_widget), 
		XtWindow(Bnr.cwps_widget),
		lx_pos, y_pos, string, 3, 
		w8w);
	    }
	y_pos = y_pos + Bnr.font_height;
	}
/*
 * Pcntrl In/Out
 */
	{
	temp = cwps.pcntrl;
	rate = temp - cwpsold.pcntrl;
	if (rate != cwps.pcntrl_rate)
	    {
	    cwps.pcntrl_rate = rate;
	    sprintf (string, "%d", rate); 
	    if (rate == 0)
		BannerWriteText (XtDisplay(Bnr.cwps_widget), 
		    XtWindow(Bnr.cwps_widget),
		    x_pos, y_pos, "     ", 3, 
		    w5w);
	    else 	    
		BannerWriteText (XtDisplay(Bnr.cwps_widget), 
		    XtWindow(Bnr.cwps_widget),
		    x_pos, y_pos, string, 3, 
		    w5w);
	    }
	if (temp != cwpsold.pcntrl)
	    {
	    cwpsold.pcntrl = temp;
	    sprintf (string, "%d", temp);
	    BannerWriteText (XtDisplay(Bnr.cwps_widget), 
		XtWindow(Bnr.cwps_widget),
		lx_pos, y_pos, string, 3, 
		w8w);
	    }
	y_pos = y_pos + Bnr.font_height;
	}
/*
 * RSRC Recv/Send
 */
	{
	temp = cwps.rsrc;
	rate = temp - cwpsold.rsrc;
	if (rate != cwps.rsrc_rate)
	    {
	    cwps.rsrc_rate = rate;
	    sprintf (string, "%d", rate); 
	    if (rate == 0)
		BannerWriteText (XtDisplay(Bnr.cwps_widget), 
		    XtWindow(Bnr.cwps_widget),
		    x_pos, y_pos, "     ", 3, 
		    w5w);
	    else 	    
		BannerWriteText (XtDisplay(Bnr.cwps_widget), 
		    XtWindow(Bnr.cwps_widget),
		    x_pos, y_pos, string, 3, 
		    w5w);
	    }
	if (temp != cwpsold.rsrc)
	    {
	    cwpsold.rsrc = temp;
	    sprintf (string, "%d", temp);
	    BannerWriteText (XtDisplay(Bnr.cwps_widget), 
		XtWindow(Bnr.cwps_widget),
		lx_pos, y_pos, string, 3, 
		w8w);
	    }
	y_pos = y_pos + Bnr.font_height;
	}

}
