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
 
*6D
 #include <stdio.h>
 #include <decw$include/DECwDwtWidgetProg.h>
*6E
*6I
 
 #include "stdio.h"
 #include "MrmAppl.h"
 
 
*6E
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
*2D
     PMS$GL_CWPS_PCNTRL_OUT,
     PMS$GL_CWPS_WAITS,
     PMS$GL_CWPS_WAITING;
*2E
*2I
     PMS$GL_CWPS_PCNTRL_OUT;
*3D
 /*    PMS$GL_CWPS_WAITS,*/
 /*    PMS$GL_CWPS_WAITING;*/
*3E
*3I
     PMS$GL_CWPS_RSRC_RECV,
     PMS$GL_CWPS_RSRC_SEND;
*3E
*2E
 
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
*3D
     tw = XTextWidth(Bnr.font_ptr, "CWPS Msg Out ", 13);
*3E
*3I
     tw = XTextWidth(Bnr.font_ptr, "GETJPI Out ", 10);
*3E
     w5w = XTextWidth(Bnr.font_ptr, "88888", 5);
     w8w = XTextWidth(Bnr.font_ptr, "88888888", 8);
 
     memset( &cwps, 0, sizeof(cwps));
*4I
     memset( &cwpsold, 0, sizeof(cwps));
*4E
 
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
*3D
 	    x_pos, y_pos, "CWPS Msg In", 1, 
*3E
*3I
 	    x_pos, y_pos, "Msg In", 1, 
*3E
 	    tw);
*4D
 	if (cwps.msgs_in != 0)
 	    {
 	    sprintf (string, "%d", cwps.msgs_in_rate); 
 	    if (cwps.msgs_in_rate != 0)
 		BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
 		    rx_pos, y_pos, string, 3, 
 		    w5w);
 	    sprintf (string, "%d", cwps.msgs_in); 
 	    BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
 		lx_pos, y_pos, string, 3, 
 		    w8w);
 	    }
 	else
 	    {
 	    BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
 		rx_pos, y_pos, "*****", 3, 
 		w5w);
 	    BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
 		lx_pos, y_pos, "********", 3, 
 		w8w);
 	    }
*4E
*4I
 	sprintf (string, "%d", cwpsold.msgs_in_rate); 
 	if (cwpsold.msgs_in_rate != 0)
 	    BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
 		rx_pos, y_pos, string, 3, 
 		w5w);
 	sprintf (string, "%d", cwpsold.msgs_in); 
 	BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
 	    lx_pos, y_pos, string, 3, 
 		w8w);
*4E
 	y_pos = y_pos + Bnr.font_height;
 	}
 
 	{
 	BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
*3D
 	    x_pos, y_pos, "CWPS Msg Out", 1, 
*3E
*3I
 	    x_pos, y_pos, "Msg Out", 1, 
*3E
 	    tw);
*4D
 	if (cwps.msgs_out != 0)
 	    {
 	    sprintf (string, "%d", cwps.msgs_out_rate); 
 	    if (cwps.msgs_out_rate != 0)
 		BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
 		    rx_pos, y_pos, string, 3, 
 		    w5w);
 	    sprintf (string, "%d", cwps.msgs_out); 
 	    BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
 		lx_pos, y_pos, string, 3, 
 		    w8w);
 	    }
 	else
 	    {
 	    BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
 		rx_pos, y_pos, "*****", 3, 
 		w5w);
 	    BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
 		lx_pos, y_pos, "********", 3, 
 		w8w);
 	    }
*4E
*4I
 	sprintf (string, "%d", cwpsold.msgs_out_rate); 
 	if (cwpsold.msgs_out_rate != 0)
 	    BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
 		rx_pos, y_pos, string, 3, 
 		w5w);
 	sprintf (string, "%d", cwpsold.msgs_out); 
 	BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
 	    lx_pos, y_pos, string, 3, 
 		w8w);
*4E
 	y_pos = y_pos + Bnr.font_height;
 	}
 
 	{
 	BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
 	    x_pos, y_pos, "Kb In", 1, 
 	    tw);
*4D
 	if (cwps.bytes_in != 0)
 	    {
 	    sprintf (string, "%d", cwps.bytes_in_rate); 
 	    if (cwps.bytes_in_rate != 0)
 		BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
 		    rx_pos, y_pos, string, 3, 
 		    w5w);
 	    sprintf (string, "%d", cwps.bytes_in); 
 	    BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
 		lx_pos, y_pos, string, 3, 
 		w8w);
 	    }
 	else
 	    {
 	    BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
 		rx_pos, y_pos, "*****", 3, 
 		w5w);
 	    BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
 		lx_pos, y_pos, "********", 3, 
 		w8w);
 	    }
*4E
*4I
 	sprintf (string, "%d", cwpsold.bytes_in_rate); 
 	if (cwpsold.bytes_in_rate != 0)
 	    BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
 		rx_pos, y_pos, string, 3, 
 		w5w);
 	sprintf (string, "%d", cwpsold.bytes_in); 
 	BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
 	    lx_pos, y_pos, string, 3, 
 	    w8w);
*4E
 	y_pos = y_pos + Bnr.font_height;
 	}
 
 	{
 	BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
 	    x_pos, y_pos, "Kb Out", 1,
 	    tw);
*4D
 	if (cwps.bytes_out != 0)
 	    {
 	    sprintf (string, "%d", cwps.bytes_out_rate); 
 	    if (cwps.bytes_out_rate != 0)
 		BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
 		    rx_pos, y_pos, string, 3, 
 		    w5w);
 	    sprintf (string, "%d", cwps.bytes_out); 
 	    BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
 		lx_pos, y_pos, string, 3,
 		w8w);
 	    }
 	else
 	    {
 	    BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
 		rx_pos, y_pos, "*****", 3, 
 		w5w);
 	    BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
 		lx_pos, y_pos, "********", 3, 
 		w8w);
 	    }
*4E
*4I
 	sprintf (string, "%d", cwpsold.bytes_out_rate); 
 	if (cwpsold.bytes_out_rate != 0)
 	    BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
 		rx_pos, y_pos, string, 3, 
 		w5w);
 	sprintf (string, "%d", cwpsold.bytes_out); 
 	BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
 	    lx_pos, y_pos, string, 3,
 	    w8w);
*4E
 	y_pos = y_pos + Bnr.font_height;
 	}
 
 	{
 	BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
 	    x_pos, y_pos, "GETJPI In", 1, 
 	    tw);
*4D
 	if (cwps.getjpi_in != 0)
 	    {
 	    sprintf (string, "%d", cwps.getjpi_in_rate); 
 	    if (cwps.getjpi_in_rate != 0)
 		BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
 		    rx_pos, y_pos, string, 3,
 		    w5w);
 	    sprintf (string, "%d", cwps.getjpi_in); 
 	    BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
 		lx_pos, y_pos, string, 3,
 		w8w);
 	    }
 	else
 	    {
 	    BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
 		rx_pos, y_pos, "*****", 3, 
 		w5w);
 	    BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
 		lx_pos, y_pos, "********", 3, 
 		w8w);
 	    }
*4E
*4I
 	sprintf (string, "%d", cwpsold.getjpi_in_rate); 
 	if (cwpsold.getjpi_in_rate != 0)
 	    BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
 		rx_pos, y_pos, string, 3,
 		w5w);
 	sprintf (string, "%d", cwpsold.getjpi_in); 
 	BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
 	    lx_pos, y_pos, string, 3,
 	    w8w);
*4E
 	y_pos = y_pos + Bnr.font_height;
 	}
 
 	{
 	BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
 	    x_pos, y_pos, "GETJPI Out", 1, 
 	    tw);
*4D
 	if (cwps.getjpi_out!= 0)
 	    {
 	    sprintf (string, "%d", cwps.getjpi_out_rate); 
 	    if (cwps.getjpi_out_rate != 0)
 		BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
 		    rx_pos, y_pos, string, 3,
 		    w5w);
 	    sprintf (string, "%d", cwps.getjpi_out); 
 	    BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
 		lx_pos, y_pos, string, 3, 
 		w8w);
 	    }
 	else
 	    {
 	    BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
 		rx_pos, y_pos, "*****", 3, 
 		w5w);
 	    BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
 		lx_pos, y_pos, "********", 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 
 	{
 	BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
*4E
*3D
 	    x_pos, y_pos, "Pcntrl I/O", 1, 
*3E
*3I
*4D
 	    x_pos, y_pos, "Pcntrl In", 1, 
*4E
*3E
*4D
 	    tw);
 	if (cwps.pcntrl_in != 0)
 	    {
 	    sprintf (string, "%d", cwps.pcntrl_in_rate); 
 	    if (cwps.pcntrl_in_rate != 0)
 		BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
 		    rx_pos, y_pos, string, 3, 
 		    w5w);
 	    sprintf (string, "%d", cwps.pcntrl_in); 
 	    BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
 		lx_pos, y_pos, string, 3,
 		w8w);
 	    }
 	else
 	    {
 	    BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
 		rx_pos, y_pos, "*****", 3, 
 		w5w);
 	    BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
 		lx_pos, y_pos, "********", 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 
 	{
 	BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
*4E
*3D
 	    x_pos, y_pos, "Waits", 1, 
*3E
*3I
*4D
 	    x_pos, y_pos, "Pcntrl Out", 1, 
 	    tw);
 	if (cwps.pcntrl_out != 0)
 	    {
 	    sprintf (string, "%d", cwps.pcntrl_out_rate); 
 	    if (cwps.pcntrl_out_rate != 0)
 		BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
 		    rx_pos, y_pos, string, 3, 
 		    w5w);
 	    sprintf (string, "%d", cwps.pcntrl_out); 
 	    BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
 		lx_pos, y_pos, string, 3,
 		w8w);
 	    }
 	else
 	    {
 	    BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
 		rx_pos, y_pos, "*****", 3, 
 		w5w);
 	    BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
 		lx_pos, y_pos, "********", 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 
 	{
 	BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
 	    x_pos, y_pos, "RSRC Recv", 1, 
 	    tw);
 	if (cwps.rsrc_recv != 0)
 	    {
 	    sprintf (string, "%d", cwps.rsrc_recv_rate); 
 	    if (cwps.rsrc_recv_rate != 0)
 		BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
 		    rx_pos, y_pos, string, 3, 
 		    w5w);
 	    sprintf (string, "%d", cwps.rsrc_recv); 
 	    BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
 		lx_pos, y_pos, string, 3,
 		w8w);
 	    }
 	else
 	    {
 	    BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
 		rx_pos, y_pos, "*****", 3, 
 		w5w);
 	    BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
 		lx_pos, y_pos, "********", 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 
 	{
*4E
*4I
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
*4E
 	BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
*4D
 	    x_pos, y_pos, "RSRC Send", 1, 
*4E
*3E
*4D
 	    tw);
*4E
*3D
 	if (cwps.waits != 0)
*3E
*3I
*4D
 	if (cwps.rsrc_send != 0)
*4E
*3E
*4D
 	    {
*4E
*3D
 	    sprintf (string, "%d", cwps.waits_rate); 
 	    if (cwps.waits_rate != 0)
*3E
*3I
*4D
 	    sprintf (string, "%d", cwps.rsrc_send_rate); 
 	    if (cwps.rsrc_send_rate != 0)
*4E
*3E
*4D
 		BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
 		    rx_pos, y_pos, string, 3, 
 		    w5w);
*4E
*3D
 	    sprintf (string, "%d", cwps.waits); 
*3E
*3I
*4D
 	    sprintf (string, "%d", cwps.rsrc_send); 
*4E
*3E
*4D
 	    BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
 		lx_pos, y_pos, string, 3,
 		w8w);
 	    }
 	else
 	    {
 	    BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
 		rx_pos, y_pos, "*****", 3, 
 		w5w);
 	    BannerWriteText (XtDisplay(Bnr.cwps_widget), XtWindow (Bnr.cwps_widget),
 		lx_pos, y_pos, "********", 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
*4E
*4I
 	    lx_pos, y_pos, string, 3,
 	    w8w);
 	y_pos = y_pos + Bnr.font_height;
 	}
 
*4E
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
 
*3D
     if (&PMS$GL_CWPS_MSGS_IN != 0)	/* Not there until VMS V5.1 */
*3E
*3I
     if (&PMS$GL_CWPS_MSGS_IN != 0)	/* Not there until VMS V5.2 */
*3E
 	{
 	cwps.msgs_in = PMS$GL_CWPS_MSGS_IN;
 	cwps.msgs_out = PMS$GL_CWPS_MSGS_OUT;
*3D
 	cwps.bytes_in = PMS$GL_CWPS_BYTES_IN;
 	cwps.bytes_out = PMS$GL_CWPS_BYTES_OUT;
*3E
*3I
 	cwps.bytes_in = PMS$GL_CWPS_BYTES_IN/1024;
 	cwps.bytes_out = PMS$GL_CWPS_BYTES_OUT/1024;
*3E
 	cwps.getjpi_in = PMS$GL_CWPS_GETJPI_IN;
 	cwps.getjpi_out = PMS$GL_CWPS_GETJPI_OUT;
*4D
 	cwps.pcntrl_in = PMS$GL_CWPS_PCNTRL_IN;
 	cwps.pcntrl_out = PMS$GL_CWPS_PCNTRL_OUT;
*4E
*2D
 	cwps.waits = PMS$GL_CWPS_WAITS;
 	cwps.waiting = PMS$GL_CWPS_WAITING;
*2E
*2I
*3D
 /*	cwps.waits = PMS$GL_CWPS_WAITS;*/
 /*	cwps.waiting = PMS$GL_CWPS_WAITING;*/
*3E
*3I
*4D
 	cwps.rsrc_recv = PMS$GL_CWPS_RSRC_RECV;
 	cwps.rsrc_send = PMS$GL_CWPS_RSRC_SEND;
*4E
*4I
 	cwps.pcntrl = PMS$GL_CWPS_PCNTRL_IN + PMS$GL_CWPS_PCNTRL_OUT;
 	cwps.rsrc = PMS$GL_CWPS_RSRC_RECV + PMS$GL_CWPS_RSRC_SEND;
*4E
*3E
*2E
 	}
 /*
*3D
  * First Lock's
*3E
*3I
  * Msgs In
*3E
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
*3D
  * Next Locks in 
*3E
*3I
  * Msgs Out
*3E
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
*3D
  * Next Locks out
*3E
*3I
  * Bytes In
*3E
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
*3D
  * Locks enq wait
*3E
*3I
  * Bytes out
*3E
  */
*2D
 	{
*2E
*2I
*3D
 /*	{
*3E
*3I
 	{
*3E
*2E
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
*2I
*3D
 */
*3E
*2E
 /*
*3D
  * Directory rate
*3E
*3I
  * GetJpi In
*3E
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
*3D
  *Locks search
*3E
*3I
  * GetJpi Out
*3E
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
*3D
  * Locks find rate 
*3E
*3I
*4D
  * Pcntrl In
*4E
*3E
*4D
  */
 	{
*4E
*3D
 	temp = cwps.pcntrl_in + cwps.pcntrl_out;
*3E
*3I
*4D
 	temp = cwps.pcntrl_in;
*4E
*3E
*4D
 	rate = temp - cwpsold.pcntrl_in;
 	if (rate != cwps.pcntrl_in_rate)
 	    {
 	    cwps.pcntrl_in_rate = rate;
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
 	if (temp != cwpsold.pcntrl_in)
 	    {
 	    cwpsold.pcntrl_in = temp;
 	    sprintf (string, "%d", temp);
 	    BannerWriteText (XtDisplay(Bnr.cwps_widget), 
 		XtWindow(Bnr.cwps_widget),
 		lx_pos, y_pos, string, 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 /*
*4E
*3D
  * Locks msgs 
*3E
*3I
*4D
  * Pcntrl Out
  */
 	{
 	temp = cwps.pcntrl_out;
 	rate = temp - cwpsold.pcntrl_out;
 	if (rate != cwps.pcntrl_out_rate)
 	    {
 	    cwps.pcntrl_out_rate = rate;
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
 	if (temp != cwpsold.pcntrl_out)
 	    {
 	    cwpsold.pcntrl_out = temp;
 	    sprintf (string, "%d", temp);
 	    BannerWriteText (XtDisplay(Bnr.cwps_widget), 
 		XtWindow(Bnr.cwps_widget),
 		lx_pos, y_pos, string, 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 /*
  * RSRC Recv
  */
 	{
 	temp = cwps.rsrc_recv;
 	rate = temp - cwpsold.rsrc_recv;
 	if (rate != cwps.rsrc_recv_rate)
 	    {
 	    cwps.rsrc_recv_rate = rate;
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
 	if (temp != cwpsold.rsrc_recv)
 	    {
 	    cwpsold.rsrc_recv = temp;
 	    sprintf (string, "%d", temp);
 	    BannerWriteText (XtDisplay(Bnr.cwps_widget), 
 		XtWindow(Bnr.cwps_widget),
 		lx_pos, y_pos, string, 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 
*4E
*4I
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
*4E
 /*
*4D
  * RSRC Send
*4E
*4I
  * RSRC Recv/Send
*4E
*3E
  */
*2D
 	{
*2E
*2I
*3D
 /*	{
*3E
*2E
*3D
 	temp = cwps.waits;
 	rate = temp - cwpsold.waits;
 	if (rate != cwps.waits_rate)
*3E
*3I
 	{
*4D
 	temp = cwps.rsrc_send;
 	rate = temp - cwpsold.rsrc_send;
 	if (rate != cwps.rsrc_send_rate)
*4E
*4I
 	temp = cwps.rsrc;
 	rate = temp - cwpsold.rsrc;
 	if (rate != cwps.rsrc_rate)
*4E
*3E
 	    {
*3D
 	    cwps.waits_rate = rate;
*3E
*3I
*4D
 	    cwps.rsrc_send_rate = rate;
*4E
*4I
 	    cwps.rsrc_rate = rate;
*4E
*3E
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
*3D
 	if (temp != cwpsold.waits)
*3E
*3I
*4D
 	if (temp != cwpsold.rsrc_send)
*4E
*4I
 	if (temp != cwpsold.rsrc)
*4E
*3E
 	    {
*3D
 	    cwpsold.waits = temp;
*3E
*3I
*4D
 	    cwpsold.rsrc_send = temp;
*4E
*4I
 	    cwpsold.rsrc = temp;
*4E
*3E
 	    sprintf (string, "%d", temp);
 	    BannerWriteText (XtDisplay(Bnr.cwps_widget), 
 		XtWindow(Bnr.cwps_widget),
 		lx_pos, y_pos, string, 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
*2I
*3D
 */
*3E
*3I
 
*3E
*2E
 }
