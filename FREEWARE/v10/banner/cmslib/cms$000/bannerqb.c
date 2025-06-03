 #module BannerQb "V1.0"
 
 /*
 **++
 **  FACILITY:
 **
 **      The DECwindows Banner program
 **
 **  ABSTRACT:
 **
 **      This module contains all the code, to run the Batch Queue monitor.
 **
 **  AUTHORS:
 **
 **      JIM SAUNDERS
 **
 **
 **  CREATION DATE:     20th May 1990
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
*2I
 #include "quidef.h"
*2E
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
*2D
 extern   Qb$_Blk    Qb;
*2E
*2I
 extern   Qb$_Blk     Qb;
*2E
 extern	 GC	     BannerGC;
 extern	 XGCValues   GcValues;
 
 extern void BannerSignalError();
*2I
 extern void BannerGetQueueInfo();
*2E
 
 static int Qb_update_count, Qb_update_number;
 static int Qb_init = 0;
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **	BannerQbSetup ()
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
 BannerQbSetup()
 {
 
 
     Qb_update_count = 1;
     if (Bnr.qb_update == 0)
 	Qb_update_number = Bnr.cpu_update;
     else
 	Qb_update_number = Bnr.qb_update;
 
*2I
     Qb.Lines[0].LastLine = 1;
 
*5D
     Qb.Lines[0].MaxNameSize = 14;
*5E
*5I
     Qb.Lines[0].MaxNameSize = 18;
*5E
     Qb.Lines[0].MaxNamePoints = 
*5D
 	XTextWidth(Bnr.font_ptr, " XXXXXXXXXXXX ", 14);
*5E
*5I
 	XTextWidth(Bnr.font_ptr, " XXXXXXXXXXXXXXXX ", 18);
*5E
     Qb.Lines[0].MaxTextSize = 20;
     Qb.Lines[0].MaxTextPoints =  
 	XTextWidth(Bnr.font_ptr, " XXXXXXXXXXXXXXXXXXXXXX", 20);
 
*4D
 /*
  * Setup search info
  */
 
     BannerGetQueueInfo (&Qb, Bnr.qb_name, QUI$M_SEARCH_BATCH);
*4E
 
*2E
     Qb_init = 1;
 }
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **	BannerQbRefresh ()
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
 BannerQbRefresh()
 {
 
     if (!Qb_init)
 	BannerQbSetup ();
 
*2I
 
*4D
     BannerPaintTextLines (XtDisplay(Bnr.qb_widget), XtWindow(Bnr.qb_widget), 
 	Bnr.qb_width, Bnr.qb_height, 
 	&Qb.Lines[0], 1);
 
*4E
*4I
     if (Qb.Valid)
 	BannerPaintTextLines (XtDisplay(Bnr.qb_widget), XtWindow(Bnr.qb_widget), 
 	    Bnr.qb_width, Bnr.qb_height, 
 	    &Qb.Lines[0], 1);
*4E
*2E
 }
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **	BannerQb ()
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
 BannerQb()
 {
 
     if (!Qb_init)
 	BannerQbRefresh ();
 
     Qb_update_count --;
     if (Qb_update_count > 0)
 	return;
 	
     Qb_update_count = Qb_update_number;
*2I
 
*3D
     BannerGetQueueInfo (&Qb);
*3E
*3I
*4D
     BannerGetQueueInfo (&Qb, Bnr.qb_name, QUI$M_SEARCH_BATCH);
*4E
*3E
*4D
 
     BannerPaintTextLines (XtDisplay(Bnr.qb_widget), XtWindow(Bnr.qb_widget), 
 	Bnr.qb_width, Bnr.qb_height, 
 	&Qb.Lines[0], 0);
 
*4E
*4I
     if (Qb.Valid)
 	{
 	BannerPaintTextLines (XtDisplay(Bnr.qb_widget), XtWindow(Bnr.qb_widget), 
 	    Bnr.qb_width, Bnr.qb_height, 
 	    &Qb.Lines[0], 0);
 	Qb.Updated = 0;
 	}
*4E
*2E
 }
