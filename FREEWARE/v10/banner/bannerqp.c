#module BannerQp "V1.0"

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


#include "quidef.h";
#include "stdio.h"
#include "MrmAppl.h"


#include "Banner"

static void BannerGetQueueInfo ();

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
extern   Qp$_Blk    Qp;
extern	 GC	     BannerGC;
extern	 XGCValues   GcValues;

extern void BannerSignalError();

static int Qp_update_count, Qp_update_number;
static int Qp_init = 0;

static char BannerQueueSearchName[40];
static int  BannerQueueSearchFlags;

static char queuename[40];
static char nodename[40];
static char jobname[40];
static char username[40];
static int jobnumber;
static int  status;
static int  jobsearchflags = QUI$M_SEARCH_ALL_JOBS;
static int  queueflags;
static int  pendingcount, holdingcount;
static int  intcount, intblocks, size;

static VMS_ItemList jobinfo[9] = {
    4,  QUI$_SEARCH_FLAGS, &jobsearchflags, 0,
    40, QUI$_USERNAME, &username, 0, 
    4,  QUI$_ENTRY_NUMBER, &jobnumber, 0, 
    40, QUI$_JOB_NAME, &jobname, 0, 
    4,  QUI$_JOB_STATUS, &status, 0, 
    4,	QUI$_JOB_SIZE, &size, 0,
    4,  QUI$_INTERVENING_JOBS, &intcount, 0,
    4,  QUI$_INTERVENING_BLOCKS, &intblocks, 0,
    0, 0, 0, 0};        

static VMS_ItemList queueinfo[9] = {
    20, QUI$_SEARCH_NAME, &BannerQueueSearchName, 0,
    4,  QUI$_PENDING_JOB_COUNT, &pendingcount, 0,
    4,  QUI$_HOLDING_JOB_COUNT, &holdingcount, 0,
    4,  QUI$_SEARCH_FLAGS, &BannerQueueSearchFlags, 0,
    4,  QUI$_QUEUE_FLAGS, &queueflags, 0,
    31, QUI$_QUEUE_NAME, &queuename, 0,
    31, QUI$_SCSNODE_NAME, &nodename, 0,
    4,  QUI$_QUEUE_STATUS, &status, 0, 
    0, 0, 0, 0};        
    

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	BannerQpSetup ()
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
BannerQpSetup()
{


    Qp_update_count = 1;
    if (Bnr.qp_update == 0)
	Qp_update_number = Bnr.cpu_update;
    else
	Qp_update_number = Bnr.qp_update;

    Qp.Lines[0].LastLine = 1;

    Qp.Lines[0].MaxNameSize = 18;
    Qp.Lines[0].MaxNamePoints = 
	XTextWidth(Bnr.font_ptr, " XXXXXXXXXXXXXXXX ", 18);
    Qp.Lines[0].MaxTextSize = 20;
    Qp.Lines[0].MaxTextPoints =  
	XTextWidth(Bnr.font_ptr, " XXXXXXXXXXXXXXXXXXXXXX", 20);

    Qp_init = 1;
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	BannerQpRefresh ()
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
BannerQpRefresh()
{

    if (!Qp_init)
	BannerQpSetup ();

    if (Qp.Valid)
	BannerPaintTextLines (XtDisplay(Bnr.qp_widget), XtWindow(Bnr.qp_widget), 
	    Bnr.qp_width, Bnr.qp_height, 
	    &Qp.Lines[0], 1);
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	BannerQp ()
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
BannerQp()
{

    if (!Qp_init)
	BannerQpRefresh ();

    Qp_update_count --;
    if (Qp_update_count > 0)
	return;
	
    Qp_update_count = Qp_update_number;

    if (Qp.Valid)
	{
	BannerPaintTextLines (XtDisplay(Bnr.qp_widget), XtWindow(Bnr.qp_widget), 
	    Bnr.qp_width, Bnr.qp_height, 
	    &Qp.Lines[0], 0);
	Qp.Updated = 0;
	}
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      BannerGetQueueInfo ()
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
void	BannerGetQueueInfo(Q$_Blk *Qblock, char *name, int flags)
{
char buf[50], buf1[50], buf2[50];
int oldhighlight;
int sts;
int iosb[2];
int queueidle = 0;

    strcpy (BannerQueueSearchName, name);
    BannerQueueSearchFlags = flags;
    Qblock->Updated = Qblock->Valid = 0;

    if (Qblock->DoingQueue)
	{
	if (Qblock->QueueAlive)
	    {
	    int sts;
	    int iosb[2];
	    int intjobcount = 0, intblockcount = 0;
	    int showjob;
    /*
     * Should we show any jobs on this queue?
     */

	    sts = SYS$GETQUIW (0, QUI$_DISPLAY_JOB, 0, &jobinfo, &iosb, 0, 0);

	    if (!(sts & SS$_NORMAL) || !(iosb[0] & SS$_NORMAL))
		{
		Qblock->DoingQueue = 0;
    /*
     * See if we found anything to display
     */
		if (Qblock->BackupLine)
		    {
		    Qblock->CurrentLine = Qblock->SavedLine;
		    Qblock->Lines[Qblock->CurrentLine].LastLine = 1;
		    if (Qblock->CurrentLine != 0)
			Qblock->Lines[Qblock->CurrentLine-1].LastLine = 0;	    
		    }
		return;
		}
    /*
     * Is it one of ours?
     */
	    if (strncmp (username, Bnr.queue_user, 
			strlen(Bnr.queue_user)) != 0 &&
		strcmp (username, "*") != 0)
		{
		intjobcount++;
		intblockcount += size;
		}
    /*
     * Display this job, if it fits our user, and if its not on hold,
     * or holding jobs are requested
     */		
	    showjob = 0;

	    if (strncmp (username, Bnr.queue_user, 
			strlen(Bnr.queue_user)) == 0 ||
		strcmp (username, "*") == 0)
		showjob = 1;


	    if (status & QUI$M_JOB_EXECUTING ||
		(Bnr.Param_Bits.jobs_on_hold &&
		Bnr.Param_Bits.all_users))
		showjob = 1;

	    if (showjob)
		{
    /*
     * If he/she didnt ask for all jobs, but did ask about intervening info, 
     * then if there are intervening jobs show a line
     */

		if (!Bnr.Param_Bits.all_users && 
		    !Bnr.Param_Bits.int_jobs &&
		    !(status & QUI$M_JOB_EXECUTING) &&
		    intjobcount > 0)
		    {
    /*
     * Create a line
     */
		    Qblock->BackupLine = 0;
		    oldhighlight = Qblock->Lines[Qblock->CurrentLine].highlight;
		    Qblock->Lines[Qblock->CurrentLine].highlight = 0;
		    Qblock->Lines[Qblock->CurrentLine].LastLine = 1;

		    strcpy (buf, "");
		    strcpy (buf1, "");
		    strcpy (buf2, "");

		    sprintf (buf, "-    %d Int Job(s)", intjobcount);

		    if (flags == QUI$M_SEARCH_PRINTER)
			sprintf (buf1, "%d blocks", intblockcount);
			
		    if (oldhighlight != Qblock->Lines[Qblock->CurrentLine].highlight ||
			strcmp (buf, Qblock->Lines[Qblock->CurrentLine].Text) != 0   ||
			strcmp (buf1, Qblock->Lines[Qblock->CurrentLine].Text1) != 0)
			    Qblock->Lines[Qblock->CurrentLine].text_changed = 1;
		    strcpy (Qblock->Lines[Qblock->CurrentLine].Text, buf);
		    strcpy (Qblock->Lines[Qblock->CurrentLine].Text1, buf1);

		    intjobcount = intblockcount = 0;

		    Qblock->Lines[Qblock->CurrentLine].LastLine = 1;
		    if (Qblock->CurrentLine != 0)
			Qblock->Lines[Qblock->CurrentLine-1].LastLine = 0;	    
		    Qblock->CurrentLine++;
		    if (Qblock->CurrentLine > clu_max_lines)
			{
			Qblock->Updated = Qblock->Valid = 1;
			Qblock->CurrentLine = 0;
			Qblock->DoingQueue = 0;
			return;			
			}
		    }
    /*
     * Now display this entry
     */
		Qblock->BackupLine = 0;
		oldhighlight = Qblock->Lines[Qblock->CurrentLine].highlight;
		Qblock->Lines[Qblock->CurrentLine].highlight = 0;
		Qblock->Lines[Qblock->CurrentLine].LastLine = 1;

		strcpy (buf, "");
		strcpy (buf1, "");
		strcpy (buf2, "");

		sprintf (buf, "- %d  %s", jobnumber, username);
		strcpy (buf1, jobname);		
    /*
     * Add flags
     */
		if (status & QUI$M_JOB_ABORTING)
		    sprintf (buf1, "%s (Aborting)", buf1);
		if (status & QUI$M_JOB_EXECUTING)
		    {
		    if (flags == QUI$M_SEARCH_PRINTER)
			sprintf (buf1, "%s (Printing)", buf1);
		    else
			sprintf (buf1, "%s (Executing)", buf1);
		    }
		if (status & QUI$M_JOB_HOLDING ||
		    status & QUI$M_JOB_TIMED_RELEASE)
		    sprintf (buf1, "%s (Holding)", buf1);
		if (status & QUI$M_JOB_PENDING)
		    sprintf (buf1, "%s (Pending)", buf1);
		if (status & QUI$M_JOB_REFUSED)
		    sprintf (buf1, "%s (Refused)", buf1);
		if (status & QUI$M_JOB_RETAINED)
		    sprintf (buf1, "%s (Retained)", buf1);
		if (status & QUI$M_JOB_STARTING)
		    sprintf (buf1, "%s (Starting)", buf1);
		if (status & QUI$M_JOB_SUSPENDED)
		    sprintf (buf1, "%s (Suspended)", buf1);

		if (oldhighlight != Qblock->Lines[Qblock->CurrentLine].highlight ||
		    strcmp (buf, Qblock->Lines[Qblock->CurrentLine].Text) != 0   ||
		    strcmp (buf1, Qblock->Lines[Qblock->CurrentLine].Text1) != 0)
			Qblock->Lines[Qblock->CurrentLine].text_changed = 1;
		strcpy (Qblock->Lines[Qblock->CurrentLine].Text, buf);
		strcpy (Qblock->Lines[Qblock->CurrentLine].Text1, buf1);

		Qblock->Lines[Qblock->CurrentLine].LastLine = 1;
		if (Qblock->CurrentLine != 0)
		    Qblock->Lines[Qblock->CurrentLine-1].LastLine = 0;	    
		Qblock->CurrentLine++;
		if (Qblock->CurrentLine > clu_max_lines)
		    {
		    Qblock->Updated = Qblock->Valid = 1;
		    Qblock->CurrentLine = 0;
		    Qblock->DoingQueue = 0;
		    return;			
		    }

		}
	    return;
	    }
	else
	    if (!Qblock->QueueAlive && Bnr.Param_Bits.stopped_queues)
		{
    /*
     * This queue is dead, show how many jobs are held on it.
     */
		if (holdingcount+pendingcount > 0)
		    {
    /*
     * Create a line
     */
		    oldhighlight = Qblock->Lines[Qblock->CurrentLine].highlight;
		    Qblock->Lines[Qblock->CurrentLine].highlight = 1;
		    Qblock->Lines[Qblock->CurrentLine].LastLine = 1;

		    sprintf (buf, "-    %d Job(s)", holdingcount+pendingcount);
		    sprintf (buf1, "Holding/Pending");

		    if (oldhighlight != Qblock->Lines[Qblock->CurrentLine].highlight ||
			strcmp (buf, Qblock->Lines[Qblock->CurrentLine].Text) != 0   ||
			strcmp (buf1, Qblock->Lines[Qblock->CurrentLine].Text1) != 0)
			    Qblock->Lines[Qblock->CurrentLine].text_changed = 1;
		    strcpy (Qblock->Lines[Qblock->CurrentLine].Text, buf);
		    strcpy (Qblock->Lines[Qblock->CurrentLine].Text1, buf1);

		    Qblock->Lines[Qblock->CurrentLine].LastLine = 1;
		    if (Qblock->CurrentLine != 0)
			Qblock->Lines[Qblock->CurrentLine-1].LastLine = 0;	    
		    Qblock->CurrentLine++;
		    if (Qblock->CurrentLine > clu_max_lines)
			{
			Qblock->Updated = Qblock->Valid = 1;
			Qblock->CurrentLine = 0;
			Qblock->DoingQueue = 0;
			return;			
			}
		    }
		Qblock->DoingQueue = 0;
		return;
		}
	    else
		if (!Qblock->QueueAlive)
		    {	
		    int sts;
		    int iosb[2];
		    int intjobcount = 0, intblockcount = 0;


    /*
     * Scan the queue for our name, and if we find our jobs, display it anyway
     */

		    sts = SYS$GETQUIW (0, QUI$_DISPLAY_JOB, 0, &jobinfo, &iosb, 0, 0);

		    if (!(sts & SS$_NORMAL) || !(iosb[0] & SS$_NORMAL))
			{
			Qblock->DoingQueue = 0;
	    /*
	     * Didnt find any, so dont bother displaying queue
	     */
			if (Qblock->BackupLine)
			    {
			    Qblock->CurrentLine = Qblock->SavedLine;
			    Qblock->Lines[Qblock->CurrentLine].LastLine = 1;
			    if (Qblock->CurrentLine != 0)
				Qblock->Lines[Qblock->CurrentLine-1].LastLine = 0;	    
			    }

			return;
			}
    /*
     * Is it one of ours?
     */
		    if (strncmp (username, Bnr.queue_user, 
				strlen(Bnr.queue_user)) == 0 ||
			strcmp (username, "*") == 0)
			{
			Qblock->BackupLine = 0;
			oldhighlight = Qblock->Lines[Qblock->CurrentLine].highlight;
			Qblock->Lines[Qblock->CurrentLine].highlight = 1;
			Qblock->Lines[Qblock->CurrentLine].LastLine = 1;

			strcpy (buf, "");
			strcpy (buf1, "");
			strcpy (buf2, "");

			sprintf (buf, "-    %s", username);
			strcpy (buf1, jobname);		

			sprintf (buf1, "%s (Holding)", buf1);

			if (oldhighlight != Qblock->Lines[Qblock->CurrentLine].highlight ||
			    strcmp (buf, Qblock->Lines[Qblock->CurrentLine].Text) != 0   ||
			    strcmp (buf1, Qblock->Lines[Qblock->CurrentLine].Text1) != 0)
				Qblock->Lines[Qblock->CurrentLine].text_changed = 1;
			strcpy (Qblock->Lines[Qblock->CurrentLine].Text, buf);
			strcpy (Qblock->Lines[Qblock->CurrentLine].Text1, buf1);

			Qblock->Lines[Qblock->CurrentLine].LastLine = 1;
			if (Qblock->CurrentLine != 0)
			    Qblock->Lines[Qblock->CurrentLine-1].LastLine = 0;	    
			Qblock->CurrentLine++;
			if (Qblock->CurrentLine > clu_max_lines)
			    {
			    Qblock->Updated = Qblock->Valid = 1;
			    Qblock->CurrentLine = 0;
			    Qblock->DoingQueue = 0;
			    return;			
			    }

			}
		    }

	return;	
	}

/*
 * search all the queues
 */
    sts = SYS$GETQUIW (0, QUI$_DISPLAY_QUEUE, 0, &queueinfo, &iosb, 0, 0);

    if (!(sts & SS$_NORMAL) || !(iosb[0] & SS$_NORMAL))
	{
	Qblock->Updated = Qblock->Valid = 1;
	Qblock->CurrentLine = 0;
	Qblock->DoingQueue = 0;
	return;
	}


    oldhighlight = Qblock->Lines[Qblock->CurrentLine].highlight;
    Qblock->Lines[Qblock->CurrentLine].highlight = 0;
    Qblock->Lines[Qblock->CurrentLine].LastLine = 1;

    Qblock->QueueAlive = 1;

    strcpy (buf, "");
    strcpy (buf1, "");
    strcpy (buf2, "");
    strcpy (buf, queuename);
/*
 * Add node name if we need to
 */
    if (!(queueflags & QUI$M_QUEUE_GENERIC) &&
	strlen (nodename) != 0)
	    sprintf (buf1, "on %s", nodename);
/*
 * check status
 */

    if (status & QUI$M_QUEUE_CLOSED)
	{
	sprintf (buf1, "on %s (Closed)", nodename);
	Qblock->Lines[Qblock->CurrentLine].highlight = 1;
	Qblock->QueueAlive = 0;
	}			

    if (status & QUI$M_QUEUE_PAUSED)
	{
	sprintf (buf1, "on %s (Paused)", nodename);
	Qblock->Lines[Qblock->CurrentLine].highlight = 1;
	Qblock->QueueAlive = 0;
	}			

    if (status & QUI$M_QUEUE_PAUSING)
	{
	sprintf (buf1, "on %s (Pausing)", nodename);
	Qblock->Lines[Qblock->CurrentLine].highlight = 1;
	Qblock->QueueAlive = 0;
	}			

    if (status & QUI$M_QUEUE_RESETTING)
	{
	sprintf (buf1, "on %s (Resetting)", nodename);
	Qblock->Lines[Qblock->CurrentLine].highlight = 1;
	Qblock->QueueAlive = 0;
	}			

    if (status & QUI$M_QUEUE_RESUMING)
	{
	sprintf (buf1, "on %s (Resuming)", nodename);
	Qblock->Lines[Qblock->CurrentLine].highlight = 1;
	Qblock->QueueAlive = 0;
	}			

    if (status & QUI$M_QUEUE_STALLED)
	{
	sprintf (buf1, "on %s (Stalled)", nodename);
	Qblock->Lines[Qblock->CurrentLine].highlight = 1;
	Qblock->QueueAlive = 0;
	}			

    if (status & QUI$M_QUEUE_STARTING)
	{
	sprintf (buf1, "on %s (Starting)", nodename);
	Qblock->Lines[Qblock->CurrentLine].highlight = 1;
	Qblock->QueueAlive = 0;
	}			

    if (status & QUI$M_QUEUE_STOPPED)
	{
	sprintf (buf1, "on %s (Stopped)", nodename);
	Qblock->Lines[Qblock->CurrentLine].highlight = 1;
	Qblock->QueueAlive = 0;
	}			

    if (status & QUI$M_QUEUE_STOPPING)
	{
	sprintf (buf1, "on %s (Stopping)", nodename);
	Qblock->Lines[Qblock->CurrentLine].highlight = 1;
	Qblock->QueueAlive = 0;
	}			

    if (status & QUI$M_QUEUE_UNAVAILABLE)
	{
	sprintf (buf1, "on %s (Unavaliable)", nodename);
	Qblock->Lines[Qblock->CurrentLine].highlight = 1;
	Qblock->QueueAlive = 0;
	}			

    if (oldhighlight != Qblock->Lines[Qblock->CurrentLine].highlight ||
	strcmp (buf, Qblock->Lines[Qblock->CurrentLine].Text) != 0   ||
	strcmp (buf1, Qblock->Lines[Qblock->CurrentLine].Text1) != 0)
	    Qblock->Lines[Qblock->CurrentLine].text_changed = 1;
    strcpy (Qblock->Lines[Qblock->CurrentLine].Text, buf);
    strcpy (Qblock->Lines[Qblock->CurrentLine].Text1, buf1);

/*
 * Should we show this queue?
 */

    Qblock->DoingQueue = 1;

    if (!Qblock->QueueAlive)
	Qblock->BackupLine = 1;

    Qblock->SavedLine = Qblock->CurrentLine;

    Qblock->Lines[Qblock->CurrentLine].LastLine = 1;
    if (Qblock->CurrentLine != 0)
	Qblock->Lines[Qblock->CurrentLine-1].LastLine = 0;	    

    Qblock->CurrentLine++;
    if (Qblock->CurrentLine > clu_max_lines)
	{
	Qblock->Updated = Qblock->Valid = 1;
	Qblock->CurrentLine = 0;
	Qblock->DoingQueue = 0;
	return;			
	}

}
