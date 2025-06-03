*5D
 #module BannerPE "V1.0"
*5E
*5I
 #module BannerPE "V2.1"
*5E
 
 /*
 **++
 **  FACILITY:
 **
 **      The DECwindows Banner program
 **
 **  ABSTRACT:
 **
 **      This module contains all the code, to run the Lavc monitor.
 **
 **  AUTHORS:
 **
 **      JIM SAUNDERS
 **
 **
 **  CREATION DATE:     20th June 1988
 **
 **  MODIFICATION HISTORY:
*5D
 **--
*5E
*5I
 **
 **      V02-1   LPL0001        Lee Leahy                14 Oct 1991
 **              Changed the display to show LAN adapter utilization and
 **              errors, and display virtual circuit utilization and errors.
 **              The PE counters are no longer displayed.
 **
 **              LAN Adapter Utilization
 **                  Error count - The number of transmit errors using this
 **                      LAN adapter.  Highlighted if error rate exceeds a 
 **                      threashold value.
 **                  Adapter name - LAN adapter name.  Highlighted if the
 **                      LAN adapter is unavailable for NISCA use.
 **                  % of LAN - NISCA protocol utilization (transmit and
 **                      receive) of this LAN adapter as a percentage
 **                      of the maximum LAN bandwidth.
 **                  Adapter utilization graph - Transmit utilization as a
 **                      percentage of the total NISCA traffic for this node.
 **                      Receive utilization (highlighted) as a percentage of
 **                      the total NISCA traffic for this node.  Combination
 **                      for all adapters will be less than 100% since the
 **                      local loopback adapter (BUS) is not displayed.
 **
 **              Virtual Circuit Utilization
 **                  Retransmit error count - Number of messages retransmitted 
 **                      to this remote node.  This error count is highlighted
 **                      if the combined (retransmit + re-receive) error rate 
 **                      exceeds a threashold value.
 **                  Re-receive error count - Number of messages received again
 **                      from this remote node.  This error count is highlighted
 **                      if the combined (retransmit + re-receive) error rate 
 **                      exceeds a threashold value.
 **                  SCS node name - Node name of the remote system.
 **                  % of LAN - NISCA protocol utilization (transmit and
 **                      receive) of this virtual circuit as a percentage
 **                      of the maximum LAN bandwidth available to this 
 **                      node (total bandwidth of all LAN adapters).
 **                  Virtual circuit traffic graph - Transmit utilization as a
 **                      percentage of the total NISCA traffic for this node.
 **                      Receive utilization (highlighted) as a percentage of
 **                      the total NISCA traffic for this node.  Combination
 **                      for all nodes will be less than 100% since total NISCA
 **                      traffic includes channel control messages.
 **
 **      V02-0   TEC200         T Coughlan		23-Aug-1991
 **              Update the PE counters to match the current driver.  
 **              (Modifications to file obtained from: BANNER003.A.)
 **
*5E
 **/
 
 
 /*
 **
 **  INCLUDE FILES
 **
 **/
 
*4D
 #include <stdio.h>
 #include <syidef.h>
 #include <decw$include/DECwDwtWidgetProg.h>
*4E
*4I
*5D
 
 #include "stdio.h"
 #include "syidef.h"
 #include "MrmAppl.h"
 
 
*5E
*5I
 #include <stdio.h>
 #include <syidef.h>
 #include <decw$include/DECwDwtWidgetProg.h>
*5E
*4E
 #include "Banner"
*5D
 
*5E
*5I
 #include "BANNER_PE"
 #include "PEM_DEF"
 #include "LANUDEF"
*5E
 
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
 extern   pem$_Blk    pem;
 extern   pes$_Blk    pes;
 extern	 GC	     BannerGC;
 extern	 XGCValues   GcValues;
 
 globalref   pes$_Blk    new_pes;
 
*5I
 globaldef struct PE_BUS	    BUS_ARRAY [ PE_BUS$C_BUS_COUNT ];
 globaldef struct PE_VC	    VC_ARRAY [ PE_VC$C_VC_COUNT ];
*5E
 
 extern void BannerSignalError();
 
 static int tw;
*5I
 static int w2w;
*5E
 static int w5w;
 static int w8w;
*5I
 static int bus_error_width;
 static int bus_name_width;
 static int bus_percent_width;
 static int bus_graph_0_width, bus_graph_100_width, bus_graph_min_width;
 static int vc_rexmt_width;
 static int vc_rercv_width;
 static int vc_name_width;
 static int vc_percent_width;
 static int vc_graph_0_width, vc_graph_100_width, vc_graph_min_width;
 static int tx_width, rx_width;
 
 static int vc_error_rate = 0;
*5E
 
 static int pe_update_count, pe_update_number;
 static int pe_init = 0;
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **	BannerPESetup ()
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
 BannerPESetup()
 {
 int nmsize;
 /* 
  * setup standard text widths
  */
*5D
     tw = XTextWidth(Bnr.font_ptr, "Node Count ", 11);
*5E
*5I
     tw = XTextWidth(Bnr.font_ptr, "NewPathNoticeDlvrd ", 19);
     w2w = XTextWidth(Bnr.font_ptr, "88", 2);
*5E
     w5w = XTextWidth(Bnr.font_ptr, "88888", 5);
     w8w = XTextWidth(Bnr.font_ptr, "88888888", 8);
 
*5I
     bus_error_width = XTextWidth ( Bnr.font_ptr, "888888", 6 );
     vc_rexmt_width = bus_error_width;
     vc_rercv_width = bus_error_width;
     bus_name_width = XTextWidth ( Bnr.font_ptr, "Name", 4 );
     vc_name_width = XTextWidth ( Bnr.font_ptr, "SCS Node", 8 );
     bus_percent_width = XTextWidth ( Bnr.font_ptr, "% of LAN", 8 );
     vc_percent_width = bus_percent_width;
     bus_graph_0_width = XTextWidth ( Bnr.font_ptr, "0", 1 );
     vc_graph_0_width = bus_graph_0_width;
     bus_graph_100_width = XTextWidth ( Bnr.font_ptr, "100", 3 );
     vc_graph_100_width = bus_graph_100_width;
     tx_width = XTextWidth ( Bnr.font_ptr, "Tx", 2 );
     rx_width = XTextWidth ( Bnr.font_ptr, "Rx", 2 );
     bus_graph_min_width = bus_graph_0_width + tx_width + w2w + rx_width + bus_graph_100_width;
     vc_graph_min_width = bus_graph_min_width;
*5E
 
     pe_update_count = 1;
     if (Bnr.pe_update == 0)
 	pe_update_number = Bnr.cpu_update;
     else
 	pe_update_number = Bnr.pe_update;
 
     pe_init = 1;
 }
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **	BannerPERefresh ()
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
 BannerPERefresh()
 {
     int lx_pos, rx_pos, x_pos, y_pos;
     char string[] = "                ";
 
     if (!pe_init)
 	BannerPESetup ();
 
*5I
 /*
 
*5E
     x_pos = Bnr.font_height/2;
     y_pos = Bnr.font_height;
 
     rx_pos = tw + Bnr.font_height/2;
     lx_pos = rx_pos + w5w + Bnr.font_height/2;
 
 	{
 	BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
*5D
 	    x_pos, y_pos, "PE Xmt", 1, 
 	    tw);
 	if (pem.xmt != 0)
 	    {
 	    sprintf (string, "%d", pem.xmt_rate); 
 	    if (pem.xmt_rate != 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		    rx_pos, y_pos, string, 3, 
 		    w5w);
 	    sprintf (string, "%d", pem.xmt); 
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		    w8w);
 	    }
 	else
 	    {
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		rx_pos, y_pos, " ", 3, 
 		w5w);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, "0", 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 
 	{
 	BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 	    x_pos, y_pos, "PE Rcv", 1, 
 	    tw);
 	if (pem.rcv != 0)
 	    {
 	    sprintf (string, "%d", pem.rcv_rate); 
 	    if (pem.rcv_rate != 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		    rx_pos, y_pos, string, 3, 
 		    w5w);
 	    sprintf (string, "%d", pem.rcv); 
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		    w8w);
 	    }
 	else
 	    {
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		rx_pos, y_pos, " ", 3, 
 		w5w);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, "0", 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 
 	{
 	BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 	    x_pos, y_pos, "Kbytes", 1, 
 	    tw);
 	if (pem.kbyte != 0)
 	    {
 	    sprintf (string, "%d", pem.kbyte_rate); 
 	    if (pem.kbyte_rate != 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		    rx_pos, y_pos, string, 3, 
 		    w5w);
 	    sprintf (string, "%d", pem.kbyte); 
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		w8w);
 	    }
 	else
 	    {
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		rx_pos, y_pos, "", 3, 
 		w5w);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, "0", 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 
 	{
 	BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 	    x_pos, y_pos, "Retries", 1,
 	    tw);
 	if (pem.retry != 0)
 	    {
 	    sprintf (string, "%d", pem.retry_rate); 
 	    if (pem.retry_rate != 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		    rx_pos, y_pos, string, 3, 
 		    w5w);
 	    sprintf (string, "%d", pem.retry); 
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, string, 3,
 		w8w);
 	    }
 	else
 	    {
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		rx_pos, y_pos, " ", 3, 
 		w5w);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, "0", 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 
 	{
 	BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 	    x_pos, y_pos, "PipeFull", 1, 
 	    tw);
 	if (pem.pipefull != 0)
 	    {
 	    sprintf (string, "%d", pem.pipefull_rate); 
 	    if (pem.pipefull_rate != 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		    rx_pos, y_pos, string, 3,
 		    w5w);
 	    sprintf (string, "%d", pem.pipefull); 
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, string, 3,
 		w8w);
 	    }
 	else
 	    {
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		rx_pos, y_pos, " ", 3, 
 		w5w);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, "0", 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 
 	{
 	BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 	    x_pos, y_pos, "Timeouts", 1, 
 	    tw);
 	if (pem.timeout != 0)
 	    {
 	    sprintf (string, "%d", pem.timeout_rate ); 
 	    if (pem.timeout_rate  != 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		    rx_pos, y_pos, string, 3,
 		    w5w);
 	    sprintf (string, "%d", pem.timeout); 
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		w8w);
 	    }
 	else
 	    {
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		rx_pos, y_pos, " ", 3, 
 		w5w);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, "0", 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 
 	{
 	BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 	    x_pos, y_pos, "Misc Event", 1, 
 	    tw);
 	if (pem.miscerr != 0)
 	    {
 	    sprintf (string, "%d", pem.miscerr_rate); 
 	    if (pem.miscerr_rate != 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		    rx_pos, y_pos, string, 3, 
 		    w5w);
 	    sprintf (string, "%d", pem.miscerr); 
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, string, 3,
 		w8w);
 	    }
 	else
 	    {
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		rx_pos, y_pos, " ", 3, 
 		w5w);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, "0", 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 
 	{
 	BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 	    x_pos, y_pos, "Node Count", 1, 
 	    tw);
 	sprintf (string, "%d", pem.badchk_rate); 
*5E
*5I
 	    x_pos, y_pos, "PathCount", 1, 
 	    tw);
 	if (pem.vc_cnt != 0)
 	    {
 	    sprintf (string, "%d", pem.vc_cnt_rate); 
 	    if (pem.vc_cnt_rate != 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		    rx_pos, y_pos, string, 3, 
 		    w5w);
 	    sprintf (string, "%d", pem.vc_cnt); 
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		    w8w);
 	    }
 	else
 	    {
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		rx_pos, y_pos, " ", 3, 
 		w5w);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, "0", 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 
 	{
 	BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 	    x_pos, y_pos, "NewPathFormed", 1, 
 	    tw);
 	if (pem.ret_id_attempts != 0)
 	    {
 	    sprintf (string, "%d", pem.ret_id_attempts_rate); 
 	    if (pem.ret_id_attempts_rate != 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		    rx_pos, y_pos, string, 3, 
 		    w5w);
 	    sprintf (string, "%d", pem.ret_id_attempts); 
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		    w8w);
 	    }
 	else
 	    {
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		rx_pos, y_pos, " ", 3, 
 		w5w);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, "0", 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 
 	{
 	BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 	    x_pos, y_pos, "NewPathNoticeDlvrd", 1, 
 	    tw);
 	if (pem.ret_ids_sent != 0)
 	    {
 	    sprintf (string, "%d", pem.ret_ids_sent_rate); 
 	    if (pem.ret_ids_sent_rate != 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		    rx_pos, y_pos, string, 3, 
 		    w5w);
 	    sprintf (string, "%d", pem.ret_ids_sent); 
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		    w8w);
 	    }
 	else
 	    {
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		rx_pos, y_pos, " ", 3, 
 		w5w);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, "0", 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 
 	{
 	BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 	    x_pos, y_pos, "KbytesSent", 1, 
 	    tw);
 	if (pem.xmt_bytes != 0)
 	    {
 	    sprintf (string, "%d", pem.xmt_bytes_rate); 
 	    if (pem.xmt_bytes_rate != 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		    rx_pos, y_pos, string, 3, 
 		    w5w);
 	    sprintf (string, "%d", pem.xmt_bytes); 
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		    w8w);
 	    }
 	else
 	    {
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		rx_pos, y_pos, " ", 3, 
 		w5w);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, "0", 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 
 	{
 	BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 	    x_pos, y_pos, "PktsSent", 1, 
 	    tw);
 	if (pem.xmt_msg != 0)
 	    {
 	    sprintf (string, "%d", pem.xmt_msg_rate); 
 	    if (pem.xmt_msg_rate != 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		    rx_pos, y_pos, string, 3, 
 		    w5w);
 	    sprintf (string, "%d", pem.xmt_msg); 
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		    w8w);
 	    }
 	else
 	    {
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		rx_pos, y_pos, " ", 3, 
 		w5w);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, "0", 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 
 	{
 	BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 	    x_pos + w2w, y_pos, "DGpktsSent", 1, 
 	    tw);
 	if (pem.xmt_unseq != 0)
 	    {
 	    sprintf (string, "%d", pem.xmt_unseq_rate); 
 	    if (pem.xmt_unseq_rate != 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		    rx_pos, y_pos, string, 3, 
 		    w5w);
 	    sprintf (string, "%d", pem.xmt_unseq); 
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		    w8w);
 	    }
 	else
 	    {
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		rx_pos, y_pos, " ", 3, 
 		w5w);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, "0", 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 
 	{
 	BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 	    x_pos + w2w, y_pos, "VCpktsSent", 1, 
 	    tw);
 	if (pem.xmt_seq != 0)
 	    {
 	    sprintf (string, "%d", pem.xmt_seq_rate); 
 	    if (pem.xmt_seq_rate != 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		    rx_pos, y_pos, string, 3, 
 		    w5w);
 	    sprintf (string, "%d", pem.xmt_seq); 
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		    w8w);
 	    }
 	else
 	    {
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		rx_pos, y_pos, " ", 3, 
 		w5w);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, "0", 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 
 	{
 	BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 	    x_pos + w2w, y_pos, "LoneACKsSent", 1, 
 	    tw);
 	if (pem.xmt_ack != 0)
 	    {
 	    sprintf (string, "%d", pem.xmt_ack_rate); 
 	    if (pem.xmt_ack_rate != 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		    rx_pos, y_pos, string, 3, 
 		    w5w);
 	    sprintf (string, "%d", pem.xmt_ack); 
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		    w8w);
 	    }
 	else
 	    {
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		rx_pos, y_pos, " ", 3, 
 		w5w);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, "0", 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 
 	{
 	BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 	    x_pos + w2w, y_pos, "Retransmits", 1, 
 	    tw);
 	if (pem.xmt_rexmt != 0)
 	    {
 	    sprintf (string, "%d", pem.xmt_rexmt_rate); 
 	    if (pem.xmt_rexmt_rate != 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		    rx_pos, y_pos, string, 3, 
 		    w5w);
 	    sprintf (string, "%d", pem.xmt_rexmt); 
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		    w8w);
 	    }
 	else
 	    {
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		rx_pos, y_pos, " ", 3, 
 		w5w);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, "0", 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 
 	{
 	BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 	    x_pos, y_pos, "KbytesRcvd,VCopen", 1, 
 	    tw);
 	if (pem.rcv_bytes != 0)
 	    {
 	    sprintf (string, "%d", pem.rcv_bytes_rate); 
 	    if (pem.rcv_bytes_rate != 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		    rx_pos, y_pos, string, 3, 
 		    w5w);
 	    sprintf (string, "%d", pem.rcv_bytes); 
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		    w8w);
 	    }
 	else
 	    {
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		rx_pos, y_pos, " ", 3, 
 		w5w);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, "0", 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 
 	{
 	BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 	    x_pos, y_pos, "PktsRcvd,VCopen", 1, 
 	    tw);
 	if (pem.rcv_msg != 0)
 	    {
 	    sprintf (string, "%d", pem.rcv_msg_rate); 
 	    if (pem.rcv_msg_rate != 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		    rx_pos, y_pos, string, 3, 
 		    w5w);
 	    sprintf (string, "%d", pem.rcv_msg); 
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		    w8w);
 	    }
 	else
 	    {
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		rx_pos, y_pos, " ", 3, 
 		w5w);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, "0", 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 
 	{
 	BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 	    x_pos, y_pos, "DGpktsRcvd", 1, 
 	    tw);
 	if (pem.rcv_unseq != 0)
 	    {
 	    sprintf (string, "%d", pem.rcv_unseq_rate); 
 	    if (pem.rcv_unseq_rate != 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		    rx_pos, y_pos, string, 3, 
 		    w5w);
 	    sprintf (string, "%d", pem.rcv_unseq); 
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		    w8w);
 	    }
 	else
 	    {
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		rx_pos, y_pos, " ", 3, 
 		w5w);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, "0", 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 
 	{
 	BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 	    x_pos, y_pos, "VCpktsRcvdInOrder", 1, 
 	    tw);
 	if (pem.rcv_seq != 0)
 	    {
 	    sprintf (string, "%d", pem.rcv_seq_rate); 
 	    if (pem.rcv_seq_rate != 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		    rx_pos, y_pos, string, 3, 
 		    w5w);
 	    sprintf (string, "%d", pem.rcv_seq); 
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		    w8w);
 	    }
 	else
 	    {
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		rx_pos, y_pos, " ", 3, 
 		w5w);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, "0", 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 
 	{
 	BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 	    x_pos, y_pos, "VCpktsRcvdEarly", 1, 
 	    tw);
 	if (pem.rcv_cache != 0)
 	    {
 	    sprintf (string, "%d", pem.rcv_cache_rate); 
 	    if (pem.rcv_cache_rate != 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		    rx_pos, y_pos, string, 3, 
 		    w5w);
 	    sprintf (string, "%d", pem.rcv_cache); 
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		    w8w);
 	    }
 	else
 	    {
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		rx_pos, y_pos, " ", 3, 
 		w5w);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, "0", 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 
 	{
 	BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 	    x_pos, y_pos, "DuplicatesRcvd", 1, 
 	    tw);
 	if (pem.rcv_rercv != 0)
 	    {
 	    sprintf (string, "%d", pem.rcv_rercv_rate); 
 	    if (pem.rcv_rercv_rate != 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		    rx_pos, y_pos, string, 3, 
 		    w5w);
 	    sprintf (string, "%d", pem.rcv_rercv); 
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		    w8w);
 	    }
 	else
 	    {
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		rx_pos, y_pos, " ", 3, 
 		w5w);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, "0", 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 
 	{
 	BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 	    x_pos, y_pos, "LoneACKsRcvd", 1, 
 	    tw);
 	if (pem.rcv_ack != 0)
 	    {
 	    sprintf (string, "%d", pem.rcv_ack_rate); 
 	    if (pem.rcv_ack_rate != 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		    rx_pos, y_pos, string, 3, 
 		    w5w);
 	    sprintf (string, "%d", pem.rcv_ack); 
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		    w8w);
 	    }
 	else
 	    {
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		rx_pos, y_pos, " ", 3, 
 		w5w);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, "0", 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 
 	{
 	BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 	    x_pos, y_pos, "VCbuf>ChanBuf", 1, 
 	    tw);
 	if (pem.topology_change != 0)
 	    {
 	    sprintf (string, "%d", pem.topology_change_rate); 
 	    if (pem.topology_change_rate != 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		    rx_pos, y_pos, string, 3, 
 		    w5w);
 	    sprintf (string, "%d", pem.topology_change); 
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		    w8w);
 	    }
 	else
 	    {
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		rx_pos, y_pos, " ", 3, 
 		w5w);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, "0", 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 
 	{
 	BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 	    x_pos, y_pos, "NoNpageDynForTx", 1, 
 	    tw);
 	if (pem.npagedyn_low != 0)
 	    {
 	    sprintf (string, "%d", pem.npagedyn_low_rate); 
 	    if (pem.npagedyn_low_rate != 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		    rx_pos, y_pos, string, 3, 
 		    w5w);
 	    sprintf (string, "%d", pem.npagedyn_low); 
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		    w8w);
 	    }
 	else
 	    {
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		rx_pos, y_pos, " ", 3, 
 		w5w);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, "0", 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 
 	{
 	BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 	    x_pos, y_pos, "NoChanForTX", 1, 
 	    tw);
 	if (pem.xmt_noxch != 0)
 	    {
 	    sprintf (string, "%d", pem.xmt_noxch_rate); 
 	    if (pem.xmt_noxch_rate != 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		    rx_pos, y_pos, string, 3, 
 		    w5w);
 	    sprintf (string, "%d", pem.xmt_noxch); 
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		    w8w);
 	    }
 	else
 	    {
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		rx_pos, y_pos, " ", 3, 
 		w5w);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, "0", 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 
 	{
 	BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 	    x_pos, y_pos, "TXstall,FullWindow", 1, 
 	    tw);
 	if (pem.tr_pipe_quota != 0)
 	    {
 	    sprintf (string, "%d", pem.tr_pipe_quota_rate); 
 	    if (pem.tr_pipe_quota_rate != 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		    rx_pos, y_pos, string, 3, 
 		    w5w);
 	    sprintf (string, "%d", pem.tr_pipe_quota); 
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		    w8w);
 	    }
 	else
 	    {
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		rx_pos, y_pos, " ", 3, 
 		w5w);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, "0", 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 
 	{
 	BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 	    x_pos, y_pos, "ShortTRpktRcvd", 1, 
 	    tw);
 	if (pem.rcv_tr_short != 0)
 	    {
 	    sprintf (string, "%d", pem.rcv_tr_short_rate); 
 	    if (pem.rcv_tr_short_rate != 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		    rx_pos, y_pos, string, 3, 
 		    w5w);
 	    sprintf (string, "%d", pem.rcv_tr_short); 
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		    w8w);
 	    }
 	else
 	    {
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		rx_pos, y_pos, " ", 3, 
 		w5w);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, "0", 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 
 	{
 	BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 	    x_pos, y_pos, "IllegalACKsRcvd", 1, 
 	    tw);
 	if (pem.rcv_ill_ack != 0)
 	    {
 	    sprintf (string, "%d", pem.rcv_ill_ack_rate); 
 	    if (pem.rcv_ill_ack_rate != 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		    rx_pos, y_pos, string, 3, 
 		    w5w);
 	    sprintf (string, "%d", pem.rcv_ill_ack); 
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		    w8w);
 	    }
 	else
 	    {
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		rx_pos, y_pos, " ", 3, 
 		w5w);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, "0", 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 
 	{
 	BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 	    x_pos, y_pos, "DscrdVCpkt,VCclsd", 1, 
 	    tw);
 	if (pem.rcv_ill_seq != 0)
 	    {
 	    sprintf (string, "%d", pem.rcv_ill_seq_rate); 
 	    if (pem.rcv_ill_seq_rate != 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		    rx_pos, y_pos, string, 3, 
 		    w5w);
 	    sprintf (string, "%d", pem.rcv_ill_seq); 
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		    w8w);
 	    }
 	else
 	    {
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		rx_pos, y_pos, " ", 3, 
 		w5w);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, "0", 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 
 	{
 	BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 	    x_pos, y_pos, "RcvdBadTRchkSum", 1, 
 	    tw);
 	if (pem.rcv_bad_cksum != 0)
 	    {
 	    sprintf (string, "%d", pem.rcv_bad_cksum_rate); 
 	    if (pem.rcv_bad_cksum_rate != 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		    rx_pos, y_pos, string, 3, 
 		    w5w);
 	    sprintf (string, "%d", pem.rcv_bad_cksum); 
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		    w8w);
 	    }
 	else
 	    {
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		rx_pos, y_pos, " ", 3, 
 		w5w);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, "0", 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 
 	{
 	BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 	    x_pos, y_pos, "RetriesExceeded", 1, 
 	    tw);
 	if (pem.xmt_seq_tmo != 0)
 	    {
 	    sprintf (string, "%d", pem.xmt_seq_tmo_rate); 
 	    if (pem.xmt_seq_tmo_rate != 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		    rx_pos, y_pos, string, 3, 
 		    w5w);
 	    sprintf (string, "%d", pem.xmt_seq_tmo); 
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		    w8w);
 	    }
 	else
 	    {
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		rx_pos, y_pos, " ", 3, 
 		w5w);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, "0", 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 
 	{
*5E
 	BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
*5D
 	    lx_pos, y_pos, string, 3, 
 	    w8w);
 	sprintf (string, "Bad %d", pem.badchk); 
 	if (pem.badchk != 0)
*5E
*5I
 	    x_pos, y_pos, "FreeQueEmpty", 1, 
 	    tw);
 	if (pem.fq_empty != 0)
 	    {
 	    sprintf (string, "%d", pem.fq_empty_rate); 
 	    if (pem.fq_empty_rate != 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		    rx_pos, y_pos, string, 3, 
 		    w5w);
 	    sprintf (string, "%d", pem.fq_empty); 
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		    w8w);
 	    }
 	else
 	    {
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
 		rx_pos, y_pos, " ", 3, 
 		w5w);
*5E
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), XtWindow (Bnr.pe_widget),
*5D
 		rx_pos, y_pos, string, 3,
 		w5w);
*5E
*5I
 		lx_pos, y_pos, "0", 3, 
 		w8w);
 	    }
*5E
 	y_pos = y_pos + Bnr.font_height;
 	}
*5I
 
  */
 
*5E
 }
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **	BannerPE ()
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
 BannerPE()
 {
     int rate, newkb, x_pos, y_pos, lx_pos;
*5I
     int width, x, y, x1, y1, y_max;
     int bus_error_pos, bus_name_pos, bus_percent_pos, bus_graph_pos, bus_graph_width;
     int vc_rexmt_pos, vc_rercv_pos, vc_name_pos, vc_percent_pos, vc_graph_pos, vc_graph_width;
     int vcs_displayed;
*5E
     char string[] = "                ";
     int status, iocnt, len; 
*5I
     long int bus_index;
     long int vc_index;
     struct PE_BUS *bus;
     struct PE_VC *vc;
     unsigned long int total_lan_bandwidth;
     unsigned long int lan_utilization;
     struct PE_VC *vc_display_array [ 100 + 1 ];
     long int hash_index;
*5E
 
     if (!pe_init)
 	BannerPERefresh ();
 
     pe_update_count --;
     if (pe_update_count > 0)
 	return;
 	
     pe_update_count = pe_update_number;
 
*5D
     x_pos = tw + Bnr.font_height/2;
     lx_pos = x_pos + w5w + Bnr.font_height/2;
     y_pos = Bnr.font_height;
 
 
*5E
     status = REMOTE_PE_COUNT ();
     if (status != 1)
 	{
*5D
 	printf("\nNi access routine rreturned bad status!\n");
*5E
*5I
*7D
 	printf("\nNi access routine returned bad status!\n");
*7E
*7I
 	printf("\nPE access routine returned bad status.\n");
 	printf("PE Monitor was built for VMS 5.5\n");
 	printf("No PE statistics can be displayed\n");
*7E
*5E
 	Bnr.Param_Bits.pe_window = 0;	
 	return;
 	}
*5I
 
 /*
  * For each BUS, update the statistics for the display.
  */
 
     lan_utilization = 0;
     for ( bus_index = 0; bus_index < PE_BUS$C_BUS_COUNT; bus_index++ )
       {
 
         bus = & BUS_ARRAY [ bus_index ];
         if ( bus -> PE_BUS$T_NAME [ 0 ] != 0 )
           {
 
 			/*  Highlight the BUS if it is offline.  */
 
             if (( bus -> PE_BUS$L_FLAGS & BUS$M_ONLINE ) == 0 )
               bus -> PE_BUS$L_HIGHLIGHT_BUS = 1;
             else
               bus -> PE_BUS$L_HIGHLIGHT_BUS = 0;
 
 			/*  Compute the BUS utilization.  */
 
             if ( bus -> PE_BUS$L_XMT_LAST != 0 )
               {
                 bus -> PE_BUS$L_XMT_PERCENT = bus -> PE_BUS$L_XMT_BYTES - bus -> PE_BUS$L_XMT_LAST;
                 bus -> PE_BUS$L_XMT_LAST = bus -> PE_BUS$L_XMT_BYTES;
                 bus -> PE_BUS$L_RCV_PERCENT = bus -> PE_BUS$L_RCV_BYTES - bus -> PE_BUS$L_RCV_LAST;
                 bus -> PE_BUS$L_RCV_LAST = bus -> PE_BUS$L_RCV_BYTES;
 
 			/*  Compute the LAN utilization.  */
 
                 if (( bus -> PE_BUS$L_FLAGS & BUS$M_LDL ) == 0 )
                   lan_utilization = lan_utilization + bus -> PE_BUS$L_XMT_PERCENT + bus -> PE_BUS$L_RCV_PERCENT;
 
               }
             else
               {
                 bus -> PE_BUS$L_XMT_LAST = bus -> PE_BUS$L_XMT_BYTES;
                 bus -> PE_BUS$L_XMT_PERCENT = 0;
                 bus -> PE_BUS$L_RCV_LAST = bus -> PE_BUS$L_RCV_BYTES;
                 bus -> PE_BUS$L_RCV_PERCENT = 0;
               }
 
             bus -> PE_BUS$L_LAN_UTILIZATION = 0;
 
 			/*  Highlight the error count if the error rate exceeds 1 per hour.  */
 
             bus -> PE_BUS$L_HIGHLIGHT_ERRORS = bus -> PE_BUS$L_HIGHLIGHT_ERRORS + 
               ((( bus -> PE_BUS$L_XMT_ERRORS - bus  -> PE_BUS$L_XMT_ERRORS_LAST ) * 60. * 60. ) / pe_update_number );
             bus  -> PE_BUS$L_XMT_ERRORS_LAST = bus -> PE_BUS$L_XMT_ERRORS;
 
           }
       }
 
 		/*  Compute the utilization of each BUS.  */
 
     total_lan_bandwidth = 0;
     if ( lan_utilization != 0 )
       for ( bus_index = 0; bus_index < PE_BUS$C_BUS_COUNT; bus_index++ )
         {
 
           bus = & BUS_ARRAY [ bus_index ];
           if ( bus -> PE_BUS$T_NAME [ 0 ] != 0 )
             {
 
 			/*  LAN segment utilization associated with this LAN adapter.  */
 
               bus -> PE_BUS$L_LAN_UTILIZATION = (( bus -> PE_BUS$L_XMT_PERCENT + bus -> PE_BUS$L_RCV_PERCENT ) 
                 * 8 * 100 ) / pe_update_number;
 	      if ( bus -> PE_BUS$L_LAN_TYPE == VCIB$K_DLL_CSMACD )
                 {
                   bus -> PE_BUS$L_LAN_UTILIZATION = ( bus -> PE_BUS$L_LAN_UTILIZATION / 10000000. );
                   total_lan_bandwidth = total_lan_bandwidth + 10000000;
                 }
               else
                 if ( bus -> PE_BUS$L_LAN_TYPE == VCIB$K_DLL_FDDI )
                   {
                     bus -> PE_BUS$L_LAN_UTILIZATION = ( bus -> PE_BUS$L_LAN_UTILIZATION / 100000000. );
                     total_lan_bandwidth = total_lan_bandwidth + 100000000;
                   }
 
 			/*  Node load supported by this transmitter.  */
 
               bus -> PE_BUS$L_XMT_PERCENT = ( 100. * bus -> PE_BUS$L_XMT_PERCENT ) / lan_utilization;
 
 			/*  Node load supported by this receiver.  */
 
               bus -> PE_BUS$L_RCV_PERCENT = ( 100. * bus -> PE_BUS$L_RCV_PERCENT ) / lan_utilization;
 
             }
         }
 
 /*
  * Clear the Virtual Circuit display array.
  */
 
     for ( vc_index = 0; vc_index <= 100; vc_index++ )
       vc_display_array [ vc_index ] = 0;
 
 /*
  * Update the error rate counter.
  */
 
     if ( --vc_error_rate < 0 )
       vc_error_rate = ( 60 * 60 / pe_update_number ) - 1;
 
 /*
  * For each VC, update the statistics for the display.
  */
 
     for ( vc_index = PE_VC$C_VC_COUNT - 1; vc_index >= 0; vc_index-- )
       {
         vc = & VC_ARRAY [ vc_index ];
         if ( vc -> PE_VC$T_NODENAME [ 0 ] != 0 )
           {
 
 			/*  Highlight the VC if it is closed.  */
 
             if (( vc -> PE_VC$L_FLAGS & VC$M_OPEN ) == 0 )
               {
                 vc -> PE_VC$L_HIGHLIGHT_VC = 1;
                 if ( vc -> PE_VC$L_CLOSED_COUNT > 0 )
                   vc -> PE_VC$L_CLOSED_COUNT = vc -> PE_VC$L_CLOSED_COUNT - 1;
               }
             else
               {
                 vc -> PE_VC$L_HIGHLIGHT_VC = 0;
 
 			/*  Display closed VCs for up to 10 minutes.  */
 
                 vc -> PE_VC$L_CLOSED_COUNT = 60 * 10 / pe_update_number;
               }
 
 			/*  Compute the VC utilization.  */
 
             if ( vc -> PE_VC$L_XMT_LAST != 0 )
               {
                 vc -> PE_VC$L_XMT_PERCENT = vc -> PE_VC$L_XMT_BYTES - vc -> PE_VC$L_XMT_LAST;
                 vc -> PE_VC$L_XMT_LAST = vc -> PE_VC$L_XMT_BYTES;
                 vc -> PE_VC$L_RCV_PERCENT = vc -> PE_VC$L_RCV_BYTES - vc -> PE_VC$L_RCV_LAST;
                 vc -> PE_VC$L_RCV_LAST = vc -> PE_VC$L_RCV_BYTES;
 
 			/*  Compute the LAN utilization.  */
 
                 vc -> PE_VC$L_LAN_UTILIZATION = (( vc -> PE_VC$L_XMT_PERCENT + vc -> PE_VC$L_RCV_PERCENT ) * 8
                   * 100 / total_lan_bandwidth ) / pe_update_number;
                 vc -> PE_VC$L_XMT_PERCENT = 100 * vc -> PE_VC$L_XMT_PERCENT / lan_utilization;
                 vc -> PE_VC$L_RCV_PERCENT = 100 * vc -> PE_VC$L_RCV_PERCENT / lan_utilization;
 
 			/*  Update the error rates.  */
 
                 if (( vc_error_rate == 0 ) && ( vc -> PE_VC$L_REXMT_RATE != 0 ))
                   vc -> PE_VC$L_REXMT_RATE = vc -> PE_VC$L_REXMT_RATE - 1;
                 if (( vc_error_rate == 0 ) && ( vc -> PE_VC$L_RERCV_RATE != 0 ))
                   vc -> PE_VC$L_RERCV_RATE = vc -> PE_VC$L_RERCV_RATE - 1;
 
 			/*  Compute the retransmit error rate.  */
 
                 vc -> PE_VC$L_REXMT_LAST = vc -> PE_VC$L_REXMT_MSGS - vc -> PE_VC$L_REXMT_LAST;
                 if ( vc -> PE_VC$L_PIPE_QUOTA == 0 )
                   vc -> PE_VC$L_REXMT_RATE = vc -> PE_VC$L_REXMT_RATE + vc -> PE_VC$L_REXMT_LAST;
                 else
                   vc -> PE_VC$L_REXMT_RATE = vc -> PE_VC$L_REXMT_RATE + ( vc -> PE_VC$L_REXMT_LAST / vc -> PE_VC$L_PIPE_QUOTA );
                 vc -> PE_VC$L_REXMT_LAST = vc -> PE_VC$L_REXMT_MSGS;
 
 			/*  Compute the re-receive error rate.  */
 
                 vc -> PE_VC$L_RERCV_LAST = vc -> PE_VC$L_RERCV_MSGS - vc -> PE_VC$L_RERCV_LAST;
                 if ( vc -> PE_VC$L_PIPE_QUOTA == 0 )
                   vc -> PE_VC$L_RERCV_RATE = vc -> PE_VC$L_RERCV_RATE + vc -> PE_VC$L_RERCV_LAST;
                 else
                   vc -> PE_VC$L_RERCV_RATE = vc -> PE_VC$L_RERCV_RATE + ( vc -> PE_VC$L_RERCV_LAST / vc -> PE_VC$L_PIPE_QUOTA );
 		vc -> PE_VC$L_RERCV_LAST = vc -> PE_VC$L_RERCV_MSGS;
 
 			/*  Compute the VC hash index.  */
 
                 if (( vc -> PE_VC$L_FLAGS & VC$M_OPEN ) != 0 )
                   hash_index = vc -> PE_VC$L_XMT_PERCENT + vc -> PE_VC$L_RCV_PERCENT;
                 else
                   hash_index = vc -> PE_VC$L_CLOSED_COUNT / 6;
                 if ( hash_index > 100 )
                   hash_index = 100;
 
 			/*  Highlight the error rates if they exceed the error threshold of 2 per hour.  */
 
                 vc -> PE_VC$L_HIGHLIGHT_REXMT = 0;
                 vc -> PE_VC$L_HIGHLIGHT_RERCV = 0;
                 if (( vc -> PE_VC$L_REXMT_RATE + vc -> PE_VC$L_RERCV_RATE ) > 2 )
                   {
                     hash_index = 100;
                     if ( vc -> PE_VC$L_REXMT_RATE != 0 )
                       vc -> PE_VC$L_HIGHLIGHT_REXMT = 1;
                     if ( vc -> PE_VC$L_RERCV_RATE != 0 )
                       vc -> PE_VC$L_HIGHLIGHT_RERCV = 1;
                   }
 
 			/*  Place this virtual circuit into the display array.  */
 
                 vc -> PE_VC$A_VC_DISPLAY_LINK = vc_display_array [ hash_index ];
                 vc_display_array [ hash_index ] = vc;
 
               }
             else
               {
                 vc -> PE_VC$L_XMT_PERCENT = 0;
                 vc -> PE_VC$L_XMT_LAST = vc -> PE_VC$L_XMT_BYTES;
                 vc -> PE_VC$L_RCV_PERCENT = 0;
                 vc -> PE_VC$L_RCV_LAST = vc -> PE_VC$L_RCV_BYTES;
                 vc -> PE_VC$L_LAN_UTILIZATION = 0;
                 vc -> PE_VC$L_REXMT_RATE = 0;
                 vc -> PE_VC$L_REXMT_LAST = vc -> PE_VC$L_REXMT_MSGS;
 		vc -> PE_VC$L_HIGHLIGHT_REXMT = 0;
                 vc -> PE_VC$L_RERCV_RATE = 0;
 		vc -> PE_VC$L_RERCV_LAST = vc -> PE_VC$L_RERCV_MSGS;
 		vc -> PE_VC$L_HIGHLIGHT_RERCV = 0;
 
 			    /*  This will display closed virtual circuits for up to two minutes.  */
 
                 vc -> PE_VC$L_CLOSED_COUNT = 60 * 2 / pe_update_number;
               }
           }
       }
 
 /*
  * Display the BUS utilization header.
  */
 
     x_pos = Bnr.font_height / 2;
     x = x_pos;
     y = Bnr.font_height;
 
     XSetForeground ( XtDisplay ( Bnr.pe_widget ), BannerGC, Bnr.foreground );
     BannerWriteText ( XtDisplay ( Bnr.pe_widget ), XtWindow ( Bnr.pe_widget ),
       x, y, "LAN Adapter Utilization", 2, Bnr.pe_width );
 
     y = y + Bnr.font_height;
     bus_error_pos = x;
     BannerWriteText ( XtDisplay ( Bnr.pe_widget ), XtWindow ( Bnr.pe_widget ),
       bus_error_pos, y, "Errors", 2, bus_error_width );
 
     x = x + bus_error_width + w2w;
     bus_name_pos = x;
     BannerWriteText ( XtDisplay ( Bnr.pe_widget ), XtWindow ( Bnr.pe_widget ),
       bus_name_pos, y, "Name", 1, bus_name_width );
 
     x = x + bus_name_width + w2w;
     bus_percent_pos = x;
     BannerWriteText ( XtDisplay ( Bnr.pe_widget ), XtWindow ( Bnr.pe_widget ),
       bus_percent_pos, y, "% of LAN", 1, bus_percent_width );
 
     x = x + bus_percent_width + w2w;
     bus_graph_pos = x;
     bus_graph_width = 0;
     if (( bus_graph_pos + bus_graph_min_width ) <= Bnr.pe_width )
       {
         bus_graph_width = Bnr.pe_width - x;
         BannerWriteText ( XtDisplay ( Bnr.pe_widget ), XtWindow ( Bnr.pe_widget ),
           bus_graph_pos, y, "0", 1, bus_graph_0_width );
         x = x + bus_graph_0_width;
         x1 = ( bus_graph_width - bus_graph_0_width - bus_graph_100_width ) / 2;
         BannerWriteText ( XtDisplay ( Bnr.pe_widget ), XtWindow ( Bnr.pe_widget ),
           x, y, "Tx", 2, x1 );
         x = x + x1;
         XSetForeground ( XtDisplay ( Bnr.pe_widget ), BannerGC, Bnr.hwmrk );
         BannerWriteText ( XtDisplay ( Bnr.pe_widget ), XtWindow ( Bnr.pe_widget ),
           x, y, "Rx", 2, x1 );
         XSetForeground ( XtDisplay ( Bnr.pe_widget ), BannerGC, Bnr.foreground );
         x = Bnr.pe_width - bus_graph_100_width;
         BannerWriteText ( XtDisplay ( Bnr.pe_widget ), XtWindow ( Bnr.pe_widget ),
           x, y, "100", 1, bus_graph_100_width );
       }
 
 /*
  * Display the BUS utilization.
  */
 
     y = y + Bnr.font_height;
     for ( bus_index = 0; bus_index < PE_BUS$C_DISPLAY_BUS; bus_index++ )
       {
         bus = & BUS_ARRAY [ bus_index ];
         if (( bus -> PE_BUS$T_NAME [ 0 ] != 0 ) && (( bus -> PE_BUS$L_FLAGS & BUS$M_LDL ) == 0 ))
           {
 
 				/*  Display the BUS error count.  */
 
             sprintf ( string, "%d", bus -> PE_BUS$L_XMT_ERRORS );
             if ( bus -> PE_BUS$L_HIGHLIGHT_ERRORS != 0 )
               XSetForeground ( XtDisplay ( Bnr.pe_widget ), BannerGC, Bnr.hwmrk );
             BannerWriteText ( XtDisplay ( Bnr.pe_widget ), XtWindow ( Bnr.pe_widget ),
               bus_error_pos, y, string, 3, bus_error_width );
             if ( bus -> PE_BUS$L_HIGHLIGHT_ERRORS != 0 )
               XSetForeground ( XtDisplay ( Bnr.pe_widget ), BannerGC, Bnr.foreground );
 
 				/*  Display the BUS name.  */
 
             if ( bus -> PE_BUS$L_HIGHLIGHT_BUS != 0 )
               XSetForeground ( XtDisplay ( Bnr.pe_widget ), BannerGC, Bnr.hwmrk );
             BannerWriteText ( XtDisplay ( Bnr.pe_widget ), XtWindow ( Bnr.pe_widget ),
               bus_name_pos, y, &bus -> PE_BUS$T_NAME [ 1 ], 1, bus_name_width );
             if ( bus -> PE_BUS$L_HIGHLIGHT_BUS != 0 )
               XSetForeground ( XtDisplay ( Bnr.pe_widget ), BannerGC, Bnr.foreground );
 
 				/*  Display the LAN utilization.  */
 
             sprintf ( string, "%d", bus -> PE_BUS$L_LAN_UTILIZATION );
             BannerWriteText ( XtDisplay ( Bnr.pe_widget ), XtWindow ( Bnr.pe_widget ),
               bus_percent_pos, y, string, 3, bus_percent_width );
 
 				/*  Display the transmit utilization.  */
 
             if ( bus_graph_width > 0 )
               {
                 x1 = bus_graph_pos + ( bus_graph_width * bus -> PE_BUS$L_XMT_PERCENT / 100 );
                 if ( x1 > Bnr.pe_width )
                   x1 = Bnr.pe_width;
                 y1 = y - Bnr.font_height + 3;
                 if (( x1 - bus_graph_pos ) > 0 )
                   XFillRectangle ( XtDisplay ( Bnr.pe_widget ), XtWindow ( Bnr.pe_widget ), BannerGC, bus_graph_pos, y1, x1, y );
 
 				/*  Display the receive utilization.  */
 
                 x = x1;
                 x1 = x + ( bus_graph_width * bus -> PE_BUS$L_RCV_PERCENT / 100 );
                 if ( x1 > Bnr.pe_width )
                   x1 = Bnr.pe_width;
                 if (( x1 - x ) > 0 )
                   {
                     XSetForeground ( XtDisplay ( Bnr.pe_widget ), BannerGC, Bnr.hwmrk );
                     XFillRectangle ( XtDisplay ( Bnr.pe_widget ), XtWindow ( Bnr.pe_widget ), BannerGC, x, y1, x1, y );
                   }
 
 				/*  Clear the rest of the line.  */
 
                 x = x1;
                 x1 = Bnr.pe_width;
                 if (( x1 - x ) >= 0 )
                   {
                     XSetForeground ( XtDisplay ( Bnr.pe_widget ), BannerGC, Bnr.background );
                     XFillRectangle ( XtDisplay ( Bnr.pe_widget ), XtWindow ( Bnr.pe_widget ), BannerGC, x, y1, x1, y );
                   }
                 XSetForeground ( XtDisplay ( Bnr.pe_widget ), BannerGC, Bnr.foreground );
               }
 
 				/*  Set the next line.  */
 
             x = x_pos;
             y = y + Bnr.font_height;
 
           }
       }
 
 			/*  Clear the line after the BUS utilization display.  */
 
     y1 = y - Bnr.font_height;
     XSetForeground ( XtDisplay ( Bnr.pe_widget ), BannerGC, Bnr.background );
     XFillRectangle ( XtDisplay ( Bnr.pe_widget ), XtWindow ( Bnr.pe_widget ), BannerGC, 0, y1, Bnr.pe_width, y );
     XSetForeground ( XtDisplay ( Bnr.pe_widget ), BannerGC, Bnr.foreground );
 
 /*
  * Display the Virtual Circuit utilization header.
  */
 
     x = x_pos;
     y = y + Bnr.font_height;
     XSetForeground ( XtDisplay ( Bnr.pe_widget ), BannerGC, Bnr.foreground );
     BannerWriteText ( XtDisplay ( Bnr.pe_widget ), XtWindow ( Bnr.pe_widget ),
       x, y, "Virtual Circuit Utilization", 2, Bnr.pe_width );
 
     y = y + Bnr.font_height;
     vc_rexmt_pos = x;
     BannerWriteText ( XtDisplay ( Bnr.pe_widget ), XtWindow ( Bnr.pe_widget ),
       vc_rexmt_pos, y, "ReXmt", 2, vc_rexmt_width );
 
     x = x + vc_rexmt_width + w2w;
     vc_rercv_pos = x;
     BannerWriteText ( XtDisplay ( Bnr.pe_widget ), XtWindow ( Bnr.pe_widget ),
       vc_rercv_pos, y, "ReRcv", 2, vc_rercv_width );
 
     x = x + vc_rercv_width + w2w;
     vc_name_pos = x;
     BannerWriteText ( XtDisplay ( Bnr.pe_widget ), XtWindow ( Bnr.pe_widget ),
       vc_name_pos, y, "SCS Node", 1, vc_name_width );
 
     x = x + vc_name_width + w2w;
     vc_percent_pos = x;
     BannerWriteText ( XtDisplay ( Bnr.pe_widget ), XtWindow ( Bnr.pe_widget ),
       vc_percent_pos, y, "% of LAN", 1, vc_percent_width );
 
     x = x + vc_percent_width + w2w;
     vc_graph_pos = x;
     vc_graph_width = 0;
     if (( vc_graph_pos + vc_graph_min_width ) <= Bnr.pe_width )
       {
         vc_graph_width = Bnr.pe_width - x;
         BannerWriteText ( XtDisplay ( Bnr.pe_widget ), XtWindow ( Bnr.pe_widget ),
           vc_graph_pos, y, "0", 1, vc_graph_0_width );
         x = x + vc_graph_0_width;
         x1 = ( vc_graph_width - vc_graph_0_width - vc_graph_100_width ) / 2;
         BannerWriteText ( XtDisplay ( Bnr.pe_widget ), XtWindow ( Bnr.pe_widget ),
           x, y, "Tx", 2, x1 );
         x = x + x1;
         XSetForeground ( XtDisplay ( Bnr.pe_widget ), BannerGC, Bnr.hwmrk );
         BannerWriteText ( XtDisplay ( Bnr.pe_widget ), XtWindow ( Bnr.pe_widget ),
           x, y, "Rx", 2, x1 );
         XSetForeground ( XtDisplay ( Bnr.pe_widget ), BannerGC, Bnr.foreground );
         x = Bnr.pe_width - vc_graph_100_width;
         BannerWriteText ( XtDisplay ( Bnr.pe_widget ), XtWindow ( Bnr.pe_widget ),
           x, y, "100", 1, vc_graph_100_width );
       }
 
 /*
  * Display the Virtual Circuit utilization.
  */
 
     y = y + Bnr.font_height;
     y_max = y + ( Bnr.font_height * PE_VC$C_DISPLAY_VC );
     vcs_displayed = 0;
     for ( vc_index = 100; vc_index > 0; vc_index-- )
       {
         if ( vcs_displayed < PE_VC$C_DISPLAY_VC )
           {
             vc = vc_display_array [ vc_index ];
             while ( vc != 0 )
               {
                 if ( vcs_displayed < PE_VC$C_DISPLAY_VC )
                   {
                     vcs_displayed = vcs_displayed + 1;
 
 				/*  Display the retransmit error count.  */
 
                     sprintf ( string, "%d", vc -> PE_VC$L_REXMT_MSGS );
                     if ( vc -> PE_VC$L_HIGHLIGHT_REXMT != 0 )
                       XSetForeground ( XtDisplay ( Bnr.pe_widget ), BannerGC, Bnr.hwmrk );
                     BannerWriteText ( XtDisplay ( Bnr.pe_widget ), XtWindow ( Bnr.pe_widget ),
                       vc_rexmt_pos, y, string, 3, vc_rexmt_width );
                     if ( vc -> PE_VC$L_HIGHLIGHT_REXMT != 0 )
                       XSetForeground ( XtDisplay ( Bnr.pe_widget ), BannerGC, Bnr.foreground );
 
 				/*  Display the re-receive error count.  */
 
                     sprintf ( string, "%d", vc -> PE_VC$L_RERCV_MSGS );
                     if ( vc -> PE_VC$L_HIGHLIGHT_RERCV != 0 )
                       XSetForeground ( XtDisplay ( Bnr.pe_widget ), BannerGC, Bnr.hwmrk );
                     BannerWriteText ( XtDisplay ( Bnr.pe_widget ), XtWindow ( Bnr.pe_widget ),
                       vc_rercv_pos, y, string, 3, vc_rercv_width );
                     if ( vc -> PE_VC$L_HIGHLIGHT_RERCV != 0 )
                       XSetForeground ( XtDisplay ( Bnr.pe_widget ), BannerGC, Bnr.foreground );
 
 				/*  Display the SCS node name.  */
 
                     if ( vc -> PE_VC$L_HIGHLIGHT_VC != 0 )
                       XSetForeground ( XtDisplay ( Bnr.pe_widget ), BannerGC, Bnr.hwmrk );
                     BannerWriteText ( XtDisplay ( Bnr.pe_widget ), XtWindow ( Bnr.pe_widget ),
                       vc_name_pos, y, &vc -> PE_VC$T_NODENAME [ 1 ], 1, vc_name_width );
                     if ( vc -> PE_VC$L_HIGHLIGHT_VC != 0 )
                       XSetForeground ( XtDisplay ( Bnr.pe_widget ), BannerGC, Bnr.foreground );
 
 				/*  Display the LAN utilization.  */
 
                     sprintf ( string, "%d", vc -> PE_VC$L_LAN_UTILIZATION );
                     BannerWriteText ( XtDisplay ( Bnr.pe_widget ), XtWindow ( Bnr.pe_widget ),
                       vc_percent_pos, y, string, 3, vc_percent_width );
 
 				/*  Display the transmit utilization.  */
 
                     if ( vc_graph_width > 0 )
                       {
                         x1 = vc_graph_pos + ( vc_graph_width * vc -> PE_VC$L_XMT_PERCENT / 100 );
                         if ( x1 > Bnr.pe_width )
                           x1 = Bnr.pe_width;
                         y1 = y - Bnr.font_height + 3;
                         if (( x1 - vc_graph_pos ) > 0 )
                           XFillRectangle ( XtDisplay ( Bnr.pe_widget ), XtWindow ( Bnr.pe_widget ), BannerGC, vc_graph_pos, y1, x1, y );
 
 				/*  Display the receive utilization.  */
 
                         x = x1;
                         x1 = x + ( vc_graph_width * vc -> PE_VC$L_RCV_PERCENT / 100 );
                         if ( x1 > Bnr.pe_width )
                           x1 = Bnr.pe_width;
                         if (( x1 - x ) > 0 )
                           {
                             XSetForeground ( XtDisplay ( Bnr.pe_widget ), BannerGC, Bnr.hwmrk );
                             XFillRectangle ( XtDisplay ( Bnr.pe_widget ), XtWindow ( Bnr.pe_widget ), BannerGC, x, y1, x1, y );
                           }
 
 				/*  Clear the rest of the line.  */
 
                         x = x1;
                         x1 = Bnr.pe_width;
                         if (( x1 - x ) >= 0 )
                           {
                             XSetForeground ( XtDisplay ( Bnr.pe_widget ), BannerGC, Bnr.background );
                             XFillRectangle ( XtDisplay ( Bnr.pe_widget ), XtWindow ( Bnr.pe_widget ), BannerGC, x, y1, x1, y );
                           }
                         XSetForeground ( XtDisplay ( Bnr.pe_widget ), BannerGC, Bnr.foreground );
                       }
 
 				/*  Set the next line.  */
 
                     x = x_pos;
                     y = y + Bnr.font_height;
 
                   }
 
 			/*  Locate the next VC for display.  */
 
                 vc = vc -> PE_VC$A_VC_DISPLAY_LINK;
 
               }
           }
       }
 
 			/*  Clear the rest of the virtual circuit utilization display.  */
 
     if ( y < y_max )
       {
         y1 = y - Bnr.font_height;
         XSetForeground ( XtDisplay ( Bnr.pe_widget ), BannerGC, Bnr.background );
         XFillRectangle ( XtDisplay ( Bnr.pe_widget ), XtWindow ( Bnr.pe_widget ), BannerGC, 0, y1, Bnr.pe_width, y_max );
         XSetForeground ( XtDisplay ( Bnr.pe_widget ), BannerGC, Bnr.foreground );
       }
 
*5E
 /*
  * First Lock's
  */
*5I
 
 /*
 
     x_pos = tw + ( Bnr.font_height / 2 );
     lx_pos = x_pos + w5w + ( Bnr.font_height / 2 );
     y_pos = y + Bnr.font_height;
 
 	{
 	rate = new_pes.vc_cnt - pes.vc_cnt;
 	if (rate != pem.vc_cnt_rate)
 	    {
 	    pem.vc_cnt_rate = rate;
 	    sprintf (string, "%d", rate); 
 	    if (rate == 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, "     ", 3, 
 		    w5w);
 	    else 	    
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, string, 3, 
 		    w5w);
 	    }
 	if (new_pes.vc_cnt != pem.vc_cnt)
 	    {
 	    pem.vc_cnt = new_pes.vc_cnt;
 	    sprintf (string, "%d", pem.vc_cnt);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		XtWindow(Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 	{
 	rate = new_pes.ret_id_attempts - pes.ret_id_attempts;
 	if (rate != pem.ret_id_attempts_rate)
 	    {
 	    pem.ret_id_attempts_rate = rate;
 	    sprintf (string, "%d", rate); 
 	    if (rate == 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, "     ", 3, 
 		    w5w);
 	    else 	    
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, string, 3, 
 		    w5w);
 	    }
 	if (new_pes.ret_id_attempts != pem.ret_id_attempts)
 	    {
 	    pem.ret_id_attempts = new_pes.ret_id_attempts;
 	    sprintf (string, "%d", pem.ret_id_attempts);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		XtWindow(Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 	{
 	rate = new_pes.ret_ids_sent - pes.ret_ids_sent;
 	if (rate != pem.ret_ids_sent_rate)
 	    {
 	    pem.ret_ids_sent_rate = rate;
 	    sprintf (string, "%d", rate); 
 	    if (rate == 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, "     ", 3, 
 		    w5w);
 	    else 	    
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, string, 3, 
 		    w5w);
 	    }
 	if (new_pes.ret_ids_sent != pem.ret_ids_sent)
 	    {
 	    pem.ret_ids_sent = new_pes.ret_ids_sent;
 	    sprintf (string, "%d", pem.ret_ids_sent);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		XtWindow(Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 	{
 	rate = new_pes.xmt_bytes/4 - pem.xmt_bytes;
 	if (rate != pem.xmt_bytes_rate)
 	    {
 	    pem.xmt_bytes_rate = rate;
 	    sprintf (string, "%d", rate); 
 	    if (rate == 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, "     ", 3, 
 		    w5w);
 	    else 	    
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, string, 3, 
 		    w5w);
 	    }
 	if (new_pes.xmt_bytes/4 != pem.xmt_bytes)
 	    {
 	    pem.xmt_bytes = new_pes.xmt_bytes/4;
 	    sprintf (string, "%d", pem.xmt_bytes);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		XtWindow(Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
*5E
 	{
 	rate = new_pes.xmt_msg - pes.xmt_msg;
*5D
 	if (rate != pem.xmt_rate)
 	    {
 	    pem.xmt_rate = rate;
 	    sprintf (string, "%d", rate); 
 	    if (rate == 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, "     ", 3, 
 		    w5w);
 	    else 	    
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, string, 3, 
 		    w5w);
 	    }
 	if (new_pes.xmt_msg != pem.xmt)
 	    {
 	    pem.xmt = new_pes.xmt_msg;
 	    sprintf (string, "%d", pem.xmt);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		XtWindow(Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 /*
  * Next Locks in 
  */
*5E
*5I
 	if (rate != pem.xmt_msg_rate)
 	    {
 	    pem.xmt_msg_rate = rate;
 	    sprintf (string, "%d", rate); 
 	    if (rate == 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, "     ", 3, 
 		    w5w);
 	    else 	    
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, string, 3, 
 		    w5w);
 	    }
 	if (new_pes.xmt_msg != pem.xmt_msg)
 	    {
 	    pem.xmt_msg = new_pes.xmt_msg;
 	    sprintf (string, "%d", pem.xmt_msg);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		XtWindow(Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 	{
 	rate = new_pes.xmt_unseq - pes.xmt_unseq;
 	if (rate != pem.xmt_unseq_rate)
 	    {
 	    pem.xmt_unseq_rate = rate;
 	    sprintf (string, "%d", rate); 
 	    if (rate == 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, "     ", 3, 
 		    w5w);
 	    else 	    
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, string, 3, 
 		    w5w);
 	    }
 	if (new_pes.xmt_unseq != pem.xmt_unseq)
 	    {
 	    pem.xmt_unseq = new_pes.xmt_unseq;
 	    sprintf (string, "%d", pem.xmt_unseq);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		XtWindow(Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 	{
 	rate = new_pes.xmt_seq - pes.xmt_seq;
 	if (rate != pem.xmt_seq_rate)
 	    {
 	    pem.xmt_seq_rate = rate;
 	    sprintf (string, "%d", rate); 
 	    if (rate == 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, "     ", 3, 
 		    w5w);
 	    else 	    
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, string, 3, 
 		    w5w);
 	    }
 	if (new_pes.xmt_seq != pem.xmt_seq)
 	    {
 	    pem.xmt_seq = new_pes.xmt_seq;
 	    sprintf (string, "%d", pem.xmt_seq);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		XtWindow(Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 	{
 	rate = new_pes.xmt_ack - pes.xmt_ack;
 	if (rate != pem.xmt_ack_rate)
 	    {
 	    pem.xmt_ack_rate = rate;
 	    sprintf (string, "%d", rate); 
 	    if (rate == 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, "     ", 3, 
 		    w5w);
 	    else 	    
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, string, 3, 
 		    w5w);
 	    }
 	if (new_pes.xmt_ack != pem.xmt_ack)
 	    {
 	    pem.xmt_ack = new_pes.xmt_ack;
 	    sprintf (string, "%d", pem.xmt_ack);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		XtWindow(Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 	{
 	rate = new_pes.xmt_rexmt - pes.xmt_rexmt;
 	if (rate != pem.xmt_rexmt_rate)
 	    {
 	    pem.xmt_rexmt_rate = rate;
 	    sprintf (string, "%d", rate); 
 	    if (rate == 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, "     ", 3, 
 		    w5w);
 	    else 	    
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, string, 3, 
 		    w5w);
 	    }
 	if (new_pes.xmt_rexmt != pem.xmt_rexmt)
 	    {
 	    pem.xmt_rexmt = new_pes.xmt_rexmt;
 	    sprintf (string, "%d", pem.xmt_rexmt);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		XtWindow(Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 	{
 	rate = new_pes.rcv_bytes/4 - pem.rcv_bytes;
 	if (rate != pem.rcv_bytes_rate)
 	    {
 	    pem.rcv_bytes_rate = rate;
 	    sprintf (string, "%d", rate); 
 	    if (rate == 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, "     ", 3, 
 		    w5w);
 	    else 	    
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, string, 3, 
 		    w5w);
 	    }
 	if (new_pes.rcv_bytes/4 != pem.rcv_bytes)
 	    {
 	    pem.rcv_bytes = new_pes.rcv_bytes/4;
 	    sprintf (string, "%d", pem.rcv_bytes);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		XtWindow(Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
*5E
 	{
 	rate = new_pes.rcv_msg - pes.rcv_msg;
*5D
 	if (rate != pem.rcv_rate)
 	    {
 	    pem.rcv_rate = rate;
 	    sprintf (string, "%d", rate); 
 	    if (rate == 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, "     ", 3, 
 		    w5w);
 	    else 	    
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, string, 3, 
 		    w5w);
 	    }
 	if (new_pes.rcv_msg != pem.rcv)
 	    {
 	    pem.rcv = new_pes.rcv_msg;
 	    sprintf (string, "%d", pem.rcv);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		XtWindow(Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 /*
  * Next Locks out
  */
 	{
 	newkb = (new_pes.xmt_bytes/4) + (new_pes.rcv_bytes/4);
 	rate = newkb - pem.kbyte;
 	if (rate != pem.kbyte_rate)
 	    {
 	    pem.kbyte_rate = rate;
 	    sprintf (string, "%d", rate); 
 	    if (rate == 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, "     ", 3, 
 		    w5w);
 	    else 	    
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, string, 3, 
 		    w5w);
 	    }
 	if (newkb != pem.kbyte)
 	    {
 	    pem.kbyte = newkb;
 	    sprintf (string, "%d", pem.kbyte);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		XtWindow(Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 /*
  * Locks enq wait
  */
*5E
*5I
 	if (rate != pem.rcv_msg_rate)
 	    {
 	    pem.rcv_msg_rate = rate;
 	    sprintf (string, "%d", rate); 
 	    if (rate == 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, "     ", 3, 
 		    w5w);
 	    else 	    
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, string, 3, 
 		    w5w);
 	    }
 	if (new_pes.rcv_msg != pem.rcv_msg)
 	    {
 	    pem.rcv_msg = new_pes.rcv_msg;
 	    sprintf (string, "%d", pem.rcv_msg);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		XtWindow(Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 	{
 	rate = new_pes.rcv_unseq - pes.rcv_unseq;
 	if (rate != pem.rcv_unseq_rate)
 	    {
 	    pem.rcv_unseq_rate = rate;
 	    sprintf (string, "%d", rate); 
 	    if (rate == 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, "     ", 3, 
 		    w5w);
 	    else 	    
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, string, 3, 
 		    w5w);
 	    }
 	if (new_pes.rcv_unseq != pem.rcv_unseq)
 	    {
 	    pem.rcv_unseq = new_pes.rcv_unseq;
 	    sprintf (string, "%d", pem.rcv_unseq);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		XtWindow(Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 	{
 	rate = new_pes.rcv_seq - pes.rcv_seq;
 	if (rate != pem.rcv_seq_rate)
 	    {
 	    pem.rcv_seq_rate = rate;
 	    sprintf (string, "%d", rate); 
 	    if (rate == 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, "     ", 3, 
 		    w5w);
 	    else 	    
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, string, 3, 
 		    w5w);
 	    }
 	if (new_pes.rcv_seq != pem.rcv_seq)
 	    {
 	    pem.rcv_seq = new_pes.rcv_seq;
 	    sprintf (string, "%d", pem.rcv_seq);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		XtWindow(Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 	{
 	rate = new_pes.rcv_cache - pes.rcv_cache;
 	if (rate != pem.rcv_cache_rate)
 	    {
 	    pem.rcv_cache_rate = rate;
 	    sprintf (string, "%d", rate); 
 	    if (rate == 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, "     ", 3, 
 		    w5w);
 	    else 	    
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, string, 3, 
 		    w5w);
 	    }
 	if (new_pes.rcv_cache != pem.rcv_cache)
 	    {
 	    pem.rcv_cache = new_pes.rcv_cache;
 	    sprintf (string, "%d", pem.rcv_cache);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		XtWindow(Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
*5E
 	{
 	rate = new_pes.rcv_rercv - pes.rcv_rercv;
*5D
 	rate = rate + new_pes.xmt_rexmt - pes.xmt_rexmt;
 	if (rate != pem.retry_rate)
 	    {
 	    pem.retry_rate = rate;
 	    sprintf (string, "%d", rate); 
 	    if (rate == 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, "     ", 3, 
 		    w5w);
 	    else 	    
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, string, 3, 
 		    w5w);
 	    }
 	rate = new_pes.rcv_rercv + new_pes.xmt_rexmt;
 	if (rate != pem.retry)
 	    {
 	    pem.retry = rate;
 	    sprintf (string, "%d", pem.retry);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		XtWindow(Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 /*
  * Directory rate
  */
*5E
*5I
 	if (rate != pem.rcv_rercv_rate)
 	    {
 	    pem.rcv_rercv_rate = rate;
 	    sprintf (string, "%d", rate); 
 	    if (rate == 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, "     ", 3, 
 		    w5w);
 	    else 	    
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, string, 3, 
 		    w5w);
 	    }
 	if (new_pes.rcv_rercv != pem.rcv_rercv)
 	    {
 	    pem.rcv_rercv = new_pes.rcv_rercv;
 	    sprintf (string, "%d", pem.rcv_rercv);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		XtWindow(Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 	{
 	rate = new_pes.rcv_ack - pes.rcv_ack;
 	if (rate != pem.rcv_ack_rate)
 	    {
 	    pem.rcv_ack_rate = rate;
 	    sprintf (string, "%d", rate); 
 	    if (rate == 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, "     ", 3, 
 		    w5w);
 	    else 	    
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, string, 3, 
 		    w5w);
 	    }
 	if (new_pes.rcv_ack != pem.rcv_ack)
 	    {
 	    pem.rcv_ack = new_pes.rcv_ack;
 	    sprintf (string, "%d", pem.rcv_ack);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		XtWindow(Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 	{
 	rate = new_pes.topology_change - pes.topology_change;
 	if (rate != pem.topology_change_rate)
 	    {
 	    pem.topology_change_rate = rate;
 	    sprintf (string, "%d", rate); 
 	    if (rate == 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, "     ", 3, 
 		    w5w);
 	    else 	    
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, string, 3, 
 		    w5w);
 	    }
 	if (new_pes.topology_change != pem.topology_change)
 	    {
 	    pem.topology_change = new_pes.topology_change;
 	    sprintf (string, "%d", pem.topology_change);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		XtWindow(Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 	{
 	rate = new_pes.npagedyn_low - pes.npagedyn_low;
 	if (rate != pem.npagedyn_low_rate)
 	    {
 	    pem.npagedyn_low_rate = rate;
 	    sprintf (string, "%d", rate); 
 	    if (rate == 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, "     ", 3, 
 		    w5w);
 	    else 	    
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, string, 3, 
 		    w5w);
 	    }
 	if (new_pes.npagedyn_low != pem.npagedyn_low)
 	    {
 	    pem.npagedyn_low = new_pes.npagedyn_low;
 	    sprintf (string, "%d", pem.npagedyn_low);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		XtWindow(Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 	{
 	rate = new_pes.xmt_noxch - pes.xmt_noxch;
 	if (rate != pem.xmt_noxch_rate)
 	    {
 	    pem.xmt_noxch_rate = rate;
 	    sprintf (string, "%d", rate); 
 	    if (rate == 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, "     ", 3, 
 		    w5w);
 	    else 	    
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, string, 3, 
 		    w5w);
 	    }
 	if (new_pes.xmt_noxch != pem.xmt_noxch)
 	    {
 	    pem.xmt_noxch = new_pes.xmt_noxch;
 	    sprintf (string, "%d", pem.xmt_noxch);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		XtWindow(Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
*5E
 	{
 	rate = new_pes.tr_pipe_quota - pes.tr_pipe_quota;
*5D
 	if (rate != pem.pipefull_rate)
 	    {
 	    pem.pipefull_rate = rate;
 	    sprintf (string, "%d", rate); 
 	    if (rate == 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, "     ", 3, 
 		    w5w);
 	    else 	    
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, string, 3, 
 		    w5w);
 	    }
 	if (new_pes.tr_pipe_quota != pem.pipefull)
 	    {
 	    pem.pipefull = new_pes.tr_pipe_quota;
 	    sprintf (string, "%d", pem.pipefull);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		XtWindow(Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 /*
  *Locks search
  */
 	{
 	int temp;
 	temp = new_pes.hs_tmo + new_pes.xmt_seq_tmo + new_pes.rcv_listen_tmo;
 	rate = temp - pem.timeout;
 	if (rate != pem.timeout_rate)
 	    {
 	    pem.timeout_rate = rate;
 	    sprintf (string, "%d", rate); 
 	    if (rate == 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, "     ", 3, 
 		    w5w);
 	    else 	    
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, string, 3, 
 		    w5w);
 	    }
 	if (temp != pem.timeout)
 	    {
 	    pem.timeout = temp;
 	    sprintf (string, "%d", pem.timeout);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		XtWindow(Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 /*
  * Locks find rate 
  */
 	{
 	int temp;
*5E
*2I
*5D
 /*
*5E
*2E
*5D
 	temp = new_pes.xmt_noxch + new_pes.xmt_rcv_norch + new_pes.rcv_tr_short
 	    + new_pes.rcv_cc_short + new_pes.rcv_ill_ack + new_pes.rcv_ill_seq
 	    + new_pes.rcv_cc_bad_eco + new_pes.rcv_cc_authorize 
 	    + new_pes.tr_dfq_empty + new_pes.tr_mfq_empty 
 	    + new_pes.cc_dfq_empty + new_pes.cc_mfq_empty;
*5E
*2I
*5D
 */
 	temp = new_pes.rcv_tr_short
 	    + new_pes.rcv_cc_short + new_pes.rcv_ill_ack + new_pes.rcv_ill_seq
 	    + new_pes.rcv_cc_bad_eco + new_pes.rcv_cc_authorize 
 	    + new_pes.tr_dfq_empty + new_pes.tr_mfq_empty 
 	    + new_pes.cc_dfq_empty + new_pes.cc_mfq_empty;
 
*5E
*2E
*5D
 	rate = temp - pem.miscerr;
 	if (rate != pem.miscerr_rate)
 	    {
 	    pem.miscerr_rate = rate;
 	    sprintf (string, "%d", rate); 
 	    if (rate == 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, "     ", 3, 
 		    w5w);
 	    else 	    
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, string, 3, 
 		    w5w);
 	    }
 	if (temp != pem.miscerr)
 	    {
 	    pem.miscerr = temp;
 	    sprintf (string, "%d", pem.miscerr);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		XtWindow(Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 /*
  * Locks msgs 
  */
*5E
*5I
 	if (rate != pem.tr_pipe_quota_rate)
 	    {
 	    pem.tr_pipe_quota_rate = rate;
 	    sprintf (string, "%d", rate); 
 	    if (rate == 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, "     ", 3, 
 		    w5w);
 	    else 	    
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, string, 3, 
 		    w5w);
 	    }
 	if (new_pes.tr_pipe_quota != pem.tr_pipe_quota)
 	    {
 	    pem.tr_pipe_quota = new_pes.tr_pipe_quota;
 	    sprintf (string, "%d", pem.tr_pipe_quota);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		XtWindow(Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 	{
 	rate = new_pes.rcv_tr_short - pes.rcv_tr_short;
 	if (rate != pem.rcv_tr_short_rate)
 	    {
 	    pem.rcv_tr_short_rate = rate;
 	    sprintf (string, "%d", rate); 
 	    if (rate == 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, "     ", 3, 
 		    w5w);
 	    else 	    
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, string, 3, 
 		    w5w);
 	    }
 	if (new_pes.rcv_tr_short != pem.rcv_tr_short)
 	    {
 	    pem.rcv_tr_short = new_pes.rcv_tr_short;
 	    sprintf (string, "%d", pem.rcv_tr_short);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		XtWindow(Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 	{
 	rate = new_pes.rcv_ill_ack - pes.rcv_ill_ack;
 	if (rate != pem.rcv_ill_ack_rate)
 	    {
 	    pem.rcv_ill_ack_rate = rate;
 	    sprintf (string, "%d", rate); 
 	    if (rate == 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, "     ", 3, 
 		    w5w);
 	    else 	    
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, string, 3, 
 		    w5w);
 	    }
 	if (new_pes.rcv_ill_ack != pem.rcv_ill_ack)
 	    {
 	    pem.rcv_ill_ack = new_pes.rcv_ill_ack;
 	    sprintf (string, "%d", pem.rcv_ill_ack);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		XtWindow(Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 	{
 	rate = new_pes.rcv_ill_seq - pes.rcv_ill_seq;
 	if (rate != pem.rcv_ill_seq_rate)
 	    {
 	    pem.rcv_ill_seq_rate = rate;
 	    sprintf (string, "%d", rate); 
 	    if (rate == 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, "     ", 3, 
 		    w5w);
 	    else 	    
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, string, 3, 
 		    w5w);
 	    }
 	if (new_pes.rcv_ill_seq != pem.rcv_ill_seq)
 	    {
 	    pem.rcv_ill_seq = new_pes.rcv_ill_seq;
 	    sprintf (string, "%d", pem.rcv_ill_seq);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		XtWindow(Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 	{
 	rate = new_pes.rcv_bad_cksum - pes.rcv_bad_cksum;
 	if (rate != pem.rcv_bad_cksum_rate)
 	    {
 	    pem.rcv_bad_cksum_rate = rate;
 	    sprintf (string, "%d", rate); 
 	    if (rate == 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, "     ", 3, 
 		    w5w);
 	    else 	    
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, string, 3, 
 		    w5w);
 	    }
 	if (new_pes.rcv_bad_cksum != pem.rcv_bad_cksum)
 	    {
 	    pem.rcv_bad_cksum = new_pes.rcv_bad_cksum;
 	    sprintf (string, "%d", pem.rcv_bad_cksum);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		XtWindow(Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
 	{
 	rate = new_pes.xmt_seq_tmo - pes.xmt_seq_tmo;
 	if (rate != pem.xmt_seq_tmo_rate)
 	    {
 	    pem.xmt_seq_tmo_rate = rate;
 	    sprintf (string, "%d", rate); 
 	    if (rate == 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, "     ", 3, 
 		    w5w);
 	    else 	    
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, string, 3, 
 		    w5w);
 	    }
 	if (new_pes.xmt_seq_tmo != pem.xmt_seq_tmo)
 	    {
 	    pem.xmt_seq_tmo = new_pes.xmt_seq_tmo;
 	    sprintf (string, "%d", pem.xmt_seq_tmo);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		XtWindow(Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		w8w);
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
*5E
 	{
*5D
 	int node_count;
 	VMS_ItemList sys_item[2] = {
 	    4, SYI$_CLUSTER_NODES, &node_count, 0, 0, 0, 0, 0};
 
 	
 	SYS$GETSYI (NULL, NULL, NULL, &sys_item, NULL, NULL, NULL);
 
 	if (new_pes.rcv_bad_chksum != pem.badchk)
 	    {
 	    pem.badchk = new_pes.rcv_bad_chksum;
 	    sprintf (string, "Bad %d", pem.badchk);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		XtWindow(Bnr.pe_widget),
 		x_pos, y_pos, string, 3, 
 		w5w);
 	    }
 	if (node_count != pem.badchk_rate)
*5E
*5I
 	rate = (new_pes.tr_dfq_empty +
 	        new_pes.tr_mfq_empty +
 	        new_pes.cc_dfq_empty +
 	        new_pes.cc_mfq_empty) -
 	       (pes.tr_dfq_empty +
 	        pes.tr_mfq_empty +
 	        pes.cc_dfq_empty +
 	        pes.cc_mfq_empty);
 	if (rate != pem.fq_empty_rate)
*5E
 	    {
*5D
 	    pem.badchk_rate = node_count;
 	    sprintf (string, "%d", node_count); 
*5E
*5I
 	    pem.fq_empty_rate = rate;
 	    sprintf (string, "%d", rate); 
 	    if (rate == 0)
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
 		    x_pos, y_pos, "     ", 3, 
 		    w5w);
 	    else 	    
*5E
 		BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		    XtWindow(Bnr.pe_widget),
*5D
 		    lx_pos, y_pos, string, 3, 
 		    w8w);
*5E
*5I
 		    x_pos, y_pos, string, 3, 
 		    w5w);
 	    }
 	if ((new_pes.tr_dfq_empty +
 	    new_pes.tr_mfq_empty +
 	    new_pes.cc_dfq_empty +
 	    new_pes.cc_mfq_empty) != pem.fq_empty)
 	    {
 	    pem.fq_empty = new_pes.tr_dfq_empty +
 	    		   new_pes.tr_mfq_empty +
 	                   new_pes.cc_dfq_empty +
 	                   new_pes.cc_mfq_empty;
 	    sprintf (string, "%d", pem.fq_empty);
 	    BannerWriteText (XtDisplay(Bnr.pe_widget), 
 		XtWindow(Bnr.pe_widget),
 		lx_pos, y_pos, string, 3, 
 		w8w);
*5E
 	    }
 	y_pos = y_pos + Bnr.font_height;
 	}
*5D
 memcpy(&pes, &new_pes, sizeof(pes));
*5E
*5I
 
       memcpy(&pes, &new_pes, sizeof(pes));
 
  */
 
*5E
 }
