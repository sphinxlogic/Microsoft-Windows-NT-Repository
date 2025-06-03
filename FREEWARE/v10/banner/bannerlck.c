#module BannerLCK "V1.0"

/*
**++
**  FACILITY:
**
**      The DECwindows Banner program
**
**  ABSTRACT:
**
**      This module contains all the code, to run the Locks monitor.
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
extern   Lck$_Blk    Lck;
extern   Lks$_Blk    Lks;
extern	 GC	     BannerGC;
extern	 XGCValues   GcValues;

globalref int 
    PMS$GL_ENQNEW_LOC,
    PMS$GL_ENQNEW_IN,
    PMS$GL_ENQNEW_OUT,
    PMS$GL_ENQCVT_IN,
    PMS$GL_ENQCVT_OUT,
    PMS$GL_ENQCVT_LOC,
    PMS$GL_DEQ_LOC,
    PMS$GL_DEQ_IN,
    PMS$GL_DEQ_OUT,
    PMS$GL_ENQWAIT,
    PMS$GL_ENQNOTQD,
    PMS$GL_BLK_LOC,
    PMS$GL_BLK_IN,
    PMS$GL_BLK_OUT,
    PMS$GL_DIR_IN,
    PMS$GL_DIR_OUT,
    PMS$GL_DLCKMSGS_IN,
    PMS$GL_DLCKMSGS_OUT,
    PMS$GL_DLCKSRCH,
    PMS$GL_DLCKFND;

extern void BannerSignalError();

static int tw;
static int w5w;
static int w8w;

static int lck_update_count, lck_update_number;
static int Lck_init = 0;

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	BannerLCKSetup ()
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
BannerLCKSetup()
{
int nmsize;
/* 
 * setup standard text widths
 */
    tw = XTextWidth(Bnr.font_ptr, "Wait/NotQd ", 11);
    w5w = XTextWidth(Bnr.font_ptr, "88888", 5);
    w8w = XTextWidth(Bnr.font_ptr, "88888888", 8);


    lck_update_count = 1;
    if (Bnr.lck_update == 0)
	lck_update_number = Bnr.cpu_update;
    else
	lck_update_number = Bnr.lck_update;

    Lck_init = 1;
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	BannerLCKRefresh ()
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
BannerLCKRefresh()
{
    int lx_pos, rx_pos, x_pos, y_pos;
    char string[] = "                ";

    if (!Lck_init)
	BannerLCKSetup ();

    x_pos = Bnr.font_height/2;
    y_pos = Bnr.font_height;

    rx_pos = tw + Bnr.font_height/2;
    lx_pos = rx_pos + w5w + Bnr.font_height/2;

	{
	BannerWriteText (XtDisplay(Bnr.lck_widget), XtWindow (Bnr.lck_widget),
	    x_pos, y_pos, "Lock Lcl", 1, 
	    tw);
	if (Lck.loc != 0)
	    {
	    sprintf (string, "%d", Lck.loc_rate); 
	    if (Lck.loc_rate != 0)
		BannerWriteText (XtDisplay(Bnr.lck_widget), XtWindow (Bnr.lck_widget),
		    rx_pos, y_pos, string, 3, 
		    w5w);
	    sprintf (string, "%d", Lck.loc); 
	    BannerWriteText (XtDisplay(Bnr.lck_widget), XtWindow (Bnr.lck_widget),
		lx_pos, y_pos, string, 3, 
		    w8w);
	    }
	else
	    {
	    BannerWriteText (XtDisplay(Bnr.lck_widget), XtWindow (Bnr.lck_widget),
		rx_pos, y_pos, "*****", 3, 
		w5w);
	    BannerWriteText (XtDisplay(Bnr.lck_widget), XtWindow (Bnr.lck_widget),
		lx_pos, y_pos, "********", 3, 
		w8w);
	    }
	y_pos = y_pos + Bnr.font_height;
	}

	{
	BannerWriteText (XtDisplay(Bnr.lck_widget), XtWindow (Bnr.lck_widget),
	    x_pos, y_pos, "Lock In", 1, 
	    tw);
	if (Lck.in != 0)
	    {
	    sprintf (string, "%d", Lck.in_rate); 
	    if (Lck.in_rate != 0)
		BannerWriteText (XtDisplay(Bnr.lck_widget), XtWindow (Bnr.lck_widget),
		    rx_pos, y_pos, string, 3, 
		    w5w);
	    sprintf (string, "%d", Lck.in); 
	    BannerWriteText (XtDisplay(Bnr.lck_widget), XtWindow (Bnr.lck_widget),
		lx_pos, y_pos, string, 3, 
		    w8w);
	    }
	else
	    {
	    BannerWriteText (XtDisplay(Bnr.lck_widget), XtWindow (Bnr.lck_widget),
		rx_pos, y_pos, "*****", 3, 
		w5w);
	    BannerWriteText (XtDisplay(Bnr.lck_widget), XtWindow (Bnr.lck_widget),
		lx_pos, y_pos, "********", 3, 
		w8w);
	    }
	y_pos = y_pos + Bnr.font_height;
	}

	{
	BannerWriteText (XtDisplay(Bnr.lck_widget), XtWindow (Bnr.lck_widget),
	    x_pos, y_pos, "Lock Out", 1, 
	    tw);
	if (Lck.out != 0)
	    {
	    sprintf (string, "%d", Lck.out_rate); 
	    if (Lck.out_rate != 0)
		BannerWriteText (XtDisplay(Bnr.lck_widget), XtWindow (Bnr.lck_widget),
		    rx_pos, y_pos, string, 3, 
		    w5w);
	    sprintf (string, "%d", Lck.out); 
	    BannerWriteText (XtDisplay(Bnr.lck_widget), XtWindow (Bnr.lck_widget),
		lx_pos, y_pos, string, 3, 
		w8w);
	    }
	else
	    {
	    BannerWriteText (XtDisplay(Bnr.lck_widget), XtWindow (Bnr.lck_widget),
		rx_pos, y_pos, "*****", 3, 
		w5w);
	    BannerWriteText (XtDisplay(Bnr.lck_widget), XtWindow (Bnr.lck_widget),
		lx_pos, y_pos, "********", 3, 
		w8w);
	    }
	y_pos = y_pos + Bnr.font_height;
	}

	{
	BannerWriteText (XtDisplay(Bnr.lck_widget), XtWindow (Bnr.lck_widget),
	    x_pos, y_pos, "Wait/NotQd", 1,
	    tw);
	if (Lck.wtnotq != 0)
	    {
	    sprintf (string, "%d", Lck.wtnotq_rate); 
	    if (Lck.wtnotq_rate != 0)
		BannerWriteText (XtDisplay(Bnr.lck_widget), XtWindow (Bnr.lck_widget),
		    rx_pos, y_pos, string, 3, 
		    w5w);
	    sprintf (string, "%d", Lck.wtnotq); 
	    BannerWriteText (XtDisplay(Bnr.lck_widget), XtWindow (Bnr.lck_widget),
		lx_pos, y_pos, string, 3,
		w8w);
	    }
	else
	    {
	    BannerWriteText (XtDisplay(Bnr.lck_widget), XtWindow (Bnr.lck_widget),
		rx_pos, y_pos, "*****", 3, 
		w5w);
	    BannerWriteText (XtDisplay(Bnr.lck_widget), XtWindow (Bnr.lck_widget),
		lx_pos, y_pos, "********", 3, 
		w8w);
	    }
	y_pos = y_pos + Bnr.font_height;
	}

	{
	BannerWriteText (XtDisplay(Bnr.lck_widget), XtWindow (Bnr.lck_widget),
	    x_pos, y_pos, "Directory", 1, 
	    tw);
	if (Lck.dir != 0)
	    {
	    sprintf (string, "%d", Lck.dir_rate); 
	    if (Lck.dir_rate != 0)
		BannerWriteText (XtDisplay(Bnr.lck_widget), XtWindow (Bnr.lck_widget),
		    rx_pos, y_pos, string, 3,
		    w5w);
	    sprintf (string, "%d", Lck.dir); 
	    BannerWriteText (XtDisplay(Bnr.lck_widget), XtWindow (Bnr.lck_widget),
		lx_pos, y_pos, string, 3,
		w8w);
	    }
	else
	    {
	    BannerWriteText (XtDisplay(Bnr.lck_widget), XtWindow (Bnr.lck_widget),
		rx_pos, y_pos, "*****", 3, 
		w5w);
	    BannerWriteText (XtDisplay(Bnr.lck_widget), XtWindow (Bnr.lck_widget),
		lx_pos, y_pos, "********", 3, 
		w8w);
	    }
	y_pos = y_pos + Bnr.font_height;
	}

	{
	BannerWriteText (XtDisplay(Bnr.lck_widget), XtWindow (Bnr.lck_widget),
	    x_pos, y_pos, "DLck Srch", 1, 
	    tw);
	if (Lck.dlcksrch!= 0)
	    {
	    sprintf (string, "%d", Lck.dlcksrch_rate); 
	    if (Lck.dlcksrch_rate != 0)
		BannerWriteText (XtDisplay(Bnr.lck_widget), XtWindow (Bnr.lck_widget),
		    rx_pos, y_pos, string, 3,
		    w5w);
	    sprintf (string, "%d", Lck.dlcksrch); 
	    BannerWriteText (XtDisplay(Bnr.lck_widget), XtWindow (Bnr.lck_widget),
		lx_pos, y_pos, string, 3, 
		w8w);
	    }
	else
	    {
	    BannerWriteText (XtDisplay(Bnr.lck_widget), XtWindow (Bnr.lck_widget),
		rx_pos, y_pos, "*****", 3, 
		w5w);
	    BannerWriteText (XtDisplay(Bnr.lck_widget), XtWindow (Bnr.lck_widget),
		lx_pos, y_pos, "********", 3, 
		w8w);
	    }
	y_pos = y_pos + Bnr.font_height;
	}

	{
	BannerWriteText (XtDisplay(Bnr.lck_widget), XtWindow (Bnr.lck_widget),
	    x_pos, y_pos, "DLck Find", 1, 
	    tw);
	if (Lck.dlckfind != 0)
	    {
	    sprintf (string, "%d", Lck.dlckfind_rate); 
	    if (Lck.dlckfind_rate != 0)
		BannerWriteText (XtDisplay(Bnr.lck_widget), XtWindow (Bnr.lck_widget),
		    rx_pos, y_pos, string, 3, 
		    w5w);
	    sprintf (string, "%d", Lck.dlckfind); 
	    BannerWriteText (XtDisplay(Bnr.lck_widget), XtWindow (Bnr.lck_widget),
		lx_pos, y_pos, string, 3,
		w8w);
	    }
	else
	    {
	    BannerWriteText (XtDisplay(Bnr.lck_widget), XtWindow (Bnr.lck_widget),
		rx_pos, y_pos, "*****", 3, 
		w5w);
	    BannerWriteText (XtDisplay(Bnr.lck_widget), XtWindow (Bnr.lck_widget),
		lx_pos, y_pos, "********", 3, 
		w8w);
	    }
	y_pos = y_pos + Bnr.font_height;
	}

	{
	BannerWriteText (XtDisplay(Bnr.lck_widget), XtWindow (Bnr.lck_widget),
	    x_pos, y_pos, "DLck Msgs", 1, 
	    tw);
	if (Lck.dlckmsg != 0)
	    {
	    sprintf (string, "%d", Lck.dlckmsg_rate); 
	    if (Lck.dlckmsg_rate != 0)
		BannerWriteText (XtDisplay(Bnr.lck_widget), XtWindow (Bnr.lck_widget),
		    rx_pos, y_pos, string, 3, 
		    w5w);
	    sprintf (string, "%d", Lck.dlckmsg); 
	    BannerWriteText (XtDisplay(Bnr.lck_widget), XtWindow (Bnr.lck_widget),
		lx_pos, y_pos, string, 3,
		w8w);
	    }
	else
	    {
	    BannerWriteText (XtDisplay(Bnr.lck_widget), XtWindow (Bnr.lck_widget),
		rx_pos, y_pos, "*****", 3, 
		w5w);
	    BannerWriteText (XtDisplay(Bnr.lck_widget), XtWindow (Bnr.lck_widget),
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
**	BannerLCK ()
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
BannerLCK()
{
    int rate, temp, x_pos, y_pos, lx_pos;
    char string[] = "                ";
    int status, iocnt, len; 

    if (!Lck_init)
	BannerLCKRefresh ();

    lck_update_count --;
    if (lck_update_count > 0)
	return;
	
    lck_update_count = lck_update_number;

    x_pos = tw + Bnr.font_height/2;
    lx_pos = x_pos + w5w + Bnr.font_height/2;
    y_pos = Bnr.font_height;

    Lks.enqnew_loc = PMS$GL_ENQNEW_LOC;
    Lks.enqnew_in = PMS$GL_ENQNEW_IN;
    Lks.enqnew_out = PMS$GL_ENQNEW_OUT;
    Lks.enqcvt_in = PMS$GL_ENQCVT_IN;
    Lks.enqcvt_out = PMS$GL_ENQCVT_OUT;
    Lks.enqcvt_loc = PMS$GL_ENQCVT_LOC;
    Lks.deq_loc = PMS$GL_DEQ_LOC;
    Lks.deq_in = PMS$GL_DEQ_IN;
    Lks.deq_out = PMS$GL_DEQ_OUT;
    Lks.enqwait = PMS$GL_ENQWAIT;
    Lks.enqnotqd = PMS$GL_ENQNOTQD;
    Lks.blk_loc = PMS$GL_BLK_LOC;
    Lks.blk_in = PMS$GL_BLK_IN;
    Lks.blk_out = PMS$GL_BLK_OUT;
    Lks.dir_in = PMS$GL_DIR_IN;
    Lks.dir_out = PMS$GL_DIR_OUT;
    Lks.dlckmsgs_in = PMS$GL_DLCKMSGS_IN;
    Lks.dlckmsgs_out = PMS$GL_DLCKMSGS_OUT;
    Lks.dlcksrch = PMS$GL_DLCKSRCH;
    Lks.dlckfnd = PMS$GL_DLCKFND;

/*
 * First Lock's
 */
	{
	temp = Lks.enqnew_loc + Lks.enqcvt_loc + Lks.deq_loc;
	rate = temp - Lck.loc;
	if (rate != Lck.loc_rate)
	    {
	    Lck.loc_rate = rate;
	    sprintf (string, "%d", rate); 
	    if (rate == 0)
		BannerWriteText (XtDisplay(Bnr.lck_widget), 
		    XtWindow(Bnr.lck_widget),
		    x_pos, y_pos, "     ", 3, 
		    w5w);
	    else 	    
		BannerWriteText (XtDisplay(Bnr.lck_widget), 
		    XtWindow(Bnr.lck_widget),
		    x_pos, y_pos, string, 3, 
		    w5w);
	    }
	if (temp != Lck.loc)
	    {
	    Lck.loc = temp;
	    sprintf (string, "%d", temp);
	    BannerWriteText (XtDisplay(Bnr.lck_widget), 
		XtWindow(Bnr.lck_widget),
		lx_pos, y_pos, string, 3, 
		w8w);
	    }
	y_pos = y_pos + Bnr.font_height;
	}
/*
 * Next Locks in 
 */
	{
	temp = Lks.enqnew_in + Lks.enqcvt_in + Lks.deq_in;
	rate = temp - Lck.in;
	if (rate != Lck.in_rate)
	    {
	    Lck.in_rate = rate;
	    sprintf (string, "%d", rate); 
	    if (rate == 0)
		BannerWriteText (XtDisplay(Bnr.lck_widget), 
		    XtWindow(Bnr.lck_widget),
		    x_pos, y_pos, "     ", 3, 
		    w5w);
	    else 	    
		BannerWriteText (XtDisplay(Bnr.lck_widget), 
		    XtWindow(Bnr.lck_widget),
		    x_pos, y_pos, string, 3, 
		    w5w);
	    }
	if (temp != Lck.in)
	    {
	    Lck.in = temp;
	    sprintf (string, "%d", temp);
	    BannerWriteText (XtDisplay(Bnr.lck_widget), 
		XtWindow(Bnr.lck_widget),
		lx_pos, y_pos, string, 3, 
		w8w);
	    }
	y_pos = y_pos + Bnr.font_height;
	}
/*
 * Next Locks out
 */
	{
	temp = Lks.enqnew_out + Lks.enqcvt_out + Lks.deq_out;
	rate = temp - Lck.out;
	if (rate != Lck.out_rate)
	    {
	    Lck.out_rate = rate;
	    sprintf (string, "%d", rate); 
	    if (rate == 0)
		BannerWriteText (XtDisplay(Bnr.lck_widget), 
		    XtWindow(Bnr.lck_widget),
		    x_pos, y_pos, "     ", 3, 
		    w5w);
	    else 	    
		BannerWriteText (XtDisplay(Bnr.lck_widget), 
		    XtWindow(Bnr.lck_widget),
		    x_pos, y_pos, string, 3, 
		    w5w);
	    }
	if (temp != Lck.out)
	    {
	    Lck.out = temp;
	    sprintf (string, "%d", temp);
	    BannerWriteText (XtDisplay(Bnr.lck_widget), 
		XtWindow(Bnr.lck_widget),
		lx_pos, y_pos, string, 3, 
		w8w);
	    }
	y_pos = y_pos + Bnr.font_height;
	}
/*
 * Locks enq wait
 */
	{
	temp = Lks.enqwait + Lks.enqnotqd;
	rate = temp - Lck.wtnotq;
	if (rate != Lck.wtnotq_rate)
	    {
	    Lck.wtnotq_rate = rate;
	    sprintf (string, "%d", rate); 
	    if (rate == 0)
		BannerWriteText (XtDisplay(Bnr.lck_widget), 
		    XtWindow(Bnr.lck_widget),
		    x_pos, y_pos, "     ", 3, 
		    w5w);
	    else 	    
		BannerWriteText (XtDisplay(Bnr.lck_widget), 
		    XtWindow(Bnr.lck_widget),
		    x_pos, y_pos, string, 3, 
		    w5w);
	    }
	if (temp != Lck.wtnotq)
	    {
	    Lck.wtnotq = temp;
	    sprintf (string, "%d", temp);
	    BannerWriteText (XtDisplay(Bnr.lck_widget), 
		XtWindow(Bnr.lck_widget),
		lx_pos, y_pos, string, 3, 
		w8w);
	    }
	y_pos = y_pos + Bnr.font_height;
	}
/*
 * Directory rate
 */
	{
	temp = Lks.dir_in + Lks.dir_out;
	rate = temp - Lck.dir;
	if (rate != Lck.dir_rate)
	    {
	    Lck.dir_rate = rate;
	    sprintf (string, "%d", rate); 
	    if (rate == 0)
		BannerWriteText (XtDisplay(Bnr.lck_widget), 
		    XtWindow(Bnr.lck_widget),
		    x_pos, y_pos, "     ", 3, 
		    w5w);
	    else 	    
		BannerWriteText (XtDisplay(Bnr.lck_widget), 
		    XtWindow(Bnr.lck_widget),
		    x_pos, y_pos, string, 3, 
		    w5w);
	    }
	if (temp != Lck.dir)
	    {
	    Lck.dir = temp;
	    sprintf (string, "%d", temp);
	    BannerWriteText (XtDisplay(Bnr.lck_widget), 
		XtWindow(Bnr.lck_widget),
		lx_pos, y_pos, string, 3, 
		w8w);
	    }
	y_pos = y_pos + Bnr.font_height;
	}
/*
 *Locks search
 */
	{
	temp = Lks.dlcksrch;
	rate = temp - Lck.dlcksrch;
	if (rate != Lck.dlcksrch_rate)
	    {
	    Lck.dlcksrch_rate = rate;
	    sprintf (string, "%d", rate); 
	    if (rate == 0)
		BannerWriteText (XtDisplay(Bnr.lck_widget), 
		    XtWindow(Bnr.lck_widget),
		    x_pos, y_pos, "     ", 3, 
		    w5w);
	    else 	    
		BannerWriteText (XtDisplay(Bnr.lck_widget), 
		    XtWindow(Bnr.lck_widget),
		    x_pos, y_pos, string, 3, 
		    w5w);
	    }
	if (temp != Lck.dlcksrch)
	    {
	    Lck.dlcksrch = temp;
	    sprintf (string, "%d", temp);
	    BannerWriteText (XtDisplay(Bnr.lck_widget), 
		XtWindow(Bnr.lck_widget),
		lx_pos, y_pos, string, 3, 
		w8w);
	    }
	y_pos = y_pos + Bnr.font_height;
	}
/*
 * Locks find rate 
 */
	{
	temp = Lks.dlckfnd;
	rate = temp - Lck.dlckfind;
	if (rate != Lck.dlckfind_rate)
	    {
	    Lck.dlckfind_rate = rate;
	    sprintf (string, "%d", rate); 
	    if (rate == 0)
		BannerWriteText (XtDisplay(Bnr.lck_widget), 
		    XtWindow(Bnr.lck_widget),
		    x_pos, y_pos, "     ", 3, 
		    w5w);
	    else 	    
		BannerWriteText (XtDisplay(Bnr.lck_widget), 
		    XtWindow(Bnr.lck_widget),
		    x_pos, y_pos, string, 3, 
		    w5w);
	    }
	if (temp != Lck.dlckfind)
	    {
	    Lck.dlckfind = temp;
	    sprintf (string, "%d", temp);
	    BannerWriteText (XtDisplay(Bnr.lck_widget), 
		XtWindow(Bnr.lck_widget),
		lx_pos, y_pos, string, 3, 
		w8w);
	    }
	y_pos = y_pos + Bnr.font_height;
	}
/*
 * Locks msgs 
 */
	{
	temp = Lks.dlckmsgs_in + Lks.dlckmsgs_out;
	rate = temp - Lck.dlckmsg;
	if (rate != Lck.dlckmsg_rate)
	    {
	    Lck.dlckmsg_rate = rate;
	    sprintf (string, "%d", rate); 
	    if (rate == 0)
		BannerWriteText (XtDisplay(Bnr.lck_widget), 
		    XtWindow(Bnr.lck_widget),
		    x_pos, y_pos, "     ", 3, 
		    w5w);
	    else 	    
		BannerWriteText (XtDisplay(Bnr.lck_widget), 
		    XtWindow(Bnr.lck_widget),
		    x_pos, y_pos, string, 3, 
		    w5w);
	    }
	if (temp != Lck.dlckmsg)
	    {
	    Lck.dlckmsg = temp;
	    sprintf (string, "%d", temp);
	    BannerWriteText (XtDisplay(Bnr.lck_widget), 
		XtWindow(Bnr.lck_widget),
		lx_pos, y_pos, string, 3, 
		w8w);
	    }
	y_pos = y_pos + Bnr.font_height;
	}
}
