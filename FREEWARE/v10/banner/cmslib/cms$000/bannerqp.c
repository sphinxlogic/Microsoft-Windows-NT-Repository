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
 
 
*3I
 #include "quidef.h";
*3E
 #include "stdio.h"
 #include "MrmAppl.h"
 
 
 #include "Banner"
 
*3I
*4D
 static void GetQueueInfo ();
*4E
*4I
 static void BannerGetQueueInfo ();
*4E
*3E
 
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
*3I
 
*4D
 static char searchname[40];
 static char searchusername[40];
*4E
*4I
 static char BannerQueueSearchName[40];
 static int  BannerQueueSearchFlags;
 
*4E
 static char queuename[40];
*4D
 static int  queuenamesize;
*4E
 static char nodename[40];
*4D
 static int  nodenamesize;
*4E
*4I
 static char jobname[40];
 static char username[40];
*4E
*7I
 static int jobnumber;
*7E
 static int  status;
*4D
 static int  searchflags;
*4E
*4I
 static int  jobsearchflags = QUI$M_SEARCH_ALL_JOBS;
*4E
 static int  queueflags;
*4D
 
 static VMS_ItemList queueinfo[8] = {
     4,  QUI$_SEARCH_FLAGS, &searchflags, 0,
     31, QUI$_SEARCH_NAME, &searchname, 0,
     31, QUI$_SEARCH_USERNAME, &searchusername, 0,
*4E
*4I
 static int  pendingcount, holdingcount;
 static int  intcount, intblocks, size;
 
*7D
 static VMS_ItemList jobinfo[8] = {
*7E
*7I
 static VMS_ItemList jobinfo[9] = {
*7E
     4,  QUI$_SEARCH_FLAGS, &jobsearchflags, 0,
     40, QUI$_USERNAME, &username, 0, 
*7I
     4,  QUI$_ENTRY_NUMBER, &jobnumber, 0, 
*7E
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
*4E
     4,  QUI$_QUEUE_FLAGS, &queueflags, 0,
*4D
     31, QUI$_QUEUE_NAME, &queuename, &queuenamesize,
     31, QUI$_SCSNODE_NAME, &nodename, &nodenamesize,
*4E
*4I
     31, QUI$_QUEUE_NAME, &queuename, 0,
     31, QUI$_SCSNODE_NAME, &nodename, 0,
*4E
     4,  QUI$_QUEUE_STATUS, &status, 0, 
     0, 0, 0, 0};        
     
*3E
 
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
*2D
 	Qp_update_number = Bnr.qpupdate;
*2E
*2I
 	Qp_update_number = Bnr.qp_update;
*2E
 
*3I
     Qp.Lines[0].LastLine = 1;
 
*7D
     Qp.Lines[0].MaxNameSize = 14;
*7E
*7I
     Qp.Lines[0].MaxNameSize = 18;
*7E
     Qp.Lines[0].MaxNamePoints = 
*7D
 	XTextWidth(Bnr.font_ptr, " XXXXXXXXXXXX ", 14);
*7E
*7I
 	XTextWidth(Bnr.font_ptr, " XXXXXXXXXXXXXXXX ", 18);
*7E
     Qp.Lines[0].MaxTextSize = 20;
     Qp.Lines[0].MaxTextPoints =  
 	XTextWidth(Bnr.font_ptr, " XXXXXXXXXXXXXXXXXXXXXX", 20);
 
*6D
 /*
  * Setup search info
  */
*6E
*4D
     strcpy (searchusername, Bnr.queue_user);
     strcpy (searchname, Bnr.qp_name);
 
     if (Bnr.Param_Bits.all_users)
 	searchflags = QUI$M_SEARCH_PRINTER | QUI$M_SEARCH_ALL_JOBS;
     else	
 	searchflags = QUI$M_SEARCH_PRINTER;
 
     GetQueueInfo ();
*4E
*4I
*6D
 
     BannerGetQueueInfo (&Qp, Bnr.qp_name, QUI$M_SEARCH_PRINTER);
*6E
*4E
*6D
 
*6E
*3E
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
 
*3I
*6D
     BannerPaintTextLines (XtDisplay(Bnr.qp_widget), XtWindow(Bnr.qp_widget), 
 	Bnr.qp_width, Bnr.qp_height, 
 	&Qp.Lines[0], 1);
 
*6E
*6I
     if (Qp.Valid)
 	BannerPaintTextLines (XtDisplay(Bnr.qp_widget), XtWindow(Bnr.qp_widget), 
 	    Bnr.qp_width, Bnr.qp_height, 
 	    &Qp.Lines[0], 1);
*6E
*3E
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
*3I
 
*4D
     GetQueueInfo ();
*4E
*4I
*5D
     BannerGetQueueInfo (&Qp);
*5E
*5I
*6D
     BannerGetQueueInfo (&Qp, Bnr.qp_name, QUI$M_SEARCH_PRINTER);
*6E
*5E
*4E
*6D
 
     BannerPaintTextLines (XtDisplay(Bnr.qp_widget), XtWindow(Bnr.qp_widget), 
 	Bnr.qp_width, Bnr.qp_height, 
 	&Qp.Lines[0], 0);
 
*6E
*6I
     if (Qp.Valid)
 	{
 	BannerPaintTextLines (XtDisplay(Bnr.qp_widget), XtWindow(Bnr.qp_widget), 
 	    Bnr.qp_width, Bnr.qp_height, 
 	    &Qp.Lines[0], 0);
 	Qp.Updated = 0;
 	}
*6E
 }
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
*4D
 **      GetQueueInfo ()
*4E
*4I
 **      BannerGetQueueInfo ()
*4E
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
*4D
 static void	GetQueueInfo()
*4E
*4I
 void	BannerGetQueueInfo(Q$_Blk *Qblock, char *name, int flags)
*4E
 {
*4D
 int i;
*4E
*4I
 char buf[50], buf1[50], buf2[50];
*6D
 int i, oldhighlight;
*6E
*6I
 int oldhighlight;
 int sts;
 int iosb[2];
 int queueidle = 0;
*6E
 
*6D
 strcpy (BannerQueueSearchName, name);
 BannerQueueSearchFlags = flags;
*6E
*6I
     strcpy (BannerQueueSearchName, name);
     BannerQueueSearchFlags = flags;
     Qblock->Updated = Qblock->Valid = 0;
*6E
*4E
 
*6D
 /*
  * search all the queues
  */
*6E
*4D
     return;
*4E
*6D
     i = 0;
*6E
*4D
     while (SYS$GETQUIW (0, QUI$_DISPLAY_QUEUE, 0, &queueinfo, 0, 0, 0) == SS$_NORMAL)
*4E
*4I
*6D
     while (1)
*6E
*6I
     if (Qblock->DoingQueue)
*6E
*4E
 	{
*4I
*6D
 	int sts;
 	int savedi;
 	int iosb[2];
*6E
*4E
*6D
 	int queueidle = 0;
 	int queuealive = 1;
 
*6E
*4D
 	Qp.Lines[i].highlight = 0;
 	Qp.Lines[i].LastLine = 1;
 	Qp.Lines[i].text_changed = 1;
 
 	strcpy (Qp.Lines[i].Text, queuename);
*4E
*4I
*6D
 	sts = SYS$GETQUIW (0, QUI$_DISPLAY_QUEUE, 0, &queueinfo, &iosb, 0, 0);
 
 	if (!(sts & SS$_NORMAL) || !(iosb[0] & SS$_NORMAL))
 	    break;
 
 	oldhighlight = Qblock->Lines[i].highlight;
 	Qblock->Lines[i].highlight = 0;
 	Qblock->Lines[i].LastLine = 1;
 
 	strcpy (buf, "");
 	strcpy (buf1, "");
 	strcpy (buf2, "");
 	strcpy (buf, queuename);
*6E
*4E
*6D
 /*
  * Add node name if we need to
  */
*6E
*4D
 	if (!(queueflags & QUI$M_QUEUE_GENERIC))
 	    sprintf (Qp.Lines[i].Text1, " on %s ", nodename);
*4E
*4I
*6D
 	if (!(queueflags & QUI$M_QUEUE_GENERIC) &&
 	    strlen (nodename) != 0)
 		sprintf (buf1, "on %s", nodename);
*6E
*4E
*6D
 /*
  * check status
  */
 
 	if (status & QUI$M_QUEUE_CLOSED)
 	    {
*6E
*4D
 	    sprintf (Qp.Lines[i].Text1, "%s (Closed) ", Qp.Lines[i].Text1);
 	    Qp.Lines[i].highlight = 1;
*4E
*4I
*6D
 	    sprintf (buf1, "on %s (Closed)", nodename);
 	    Qblock->Lines[i].highlight = 1;
*6E
*4E
*6D
 	    queuealive = 0;
 	    }			
 
 	if (status & QUI$M_QUEUE_PAUSED)
 	    {
*6E
*4D
 	    sprintf (Qp.Lines[i].Text1, "%s (Paused) ", Qp.Lines[i].Text1);
 	    Qp.Lines[i].highlight = 1;
*4E
*4I
*6D
 	    sprintf (buf1, "on %s (Paused)", nodename);
 	    Qblock->Lines[i].highlight = 1;
*6E
*4E
*6D
 	    queuealive = 0;
 	    }			
 
 	if (status & QUI$M_QUEUE_PAUSING)
 	    {
*6E
*4D
 	    sprintf (Qp.Lines[i].Text1, "%s (Pausing) ", Qp.Lines[i].Text1);
 	    Qp.Lines[i].highlight = 1;
*4E
*4I
*6D
 	    sprintf (buf1, "on %s (Pausing)", nodename);
 	    Qblock->Lines[i].highlight = 1;
*6E
*4E
*6D
 	    queuealive = 0;
 	    }			
 
 	if (status & QUI$M_QUEUE_RESETTING)
 	    {
*6E
*4D
 	    sprintf (Qp.Lines[i].Text1, "%s (resetting) ", Qp.Lines[i].Text1);
 	    Qp.Lines[i].highlight = 1;
*4E
*4I
*6D
 	    sprintf (buf1, "on %s (Resetting)", nodename);
 	    Qblock->Lines[i].highlight = 1;
*6E
*4E
*6D
 	    queuealive = 0;
 	    }			
 
 	if (status & QUI$M_QUEUE_RESUMING)
 	    {
*6E
*4D
 	    sprintf (Qp.Lines[i].Text1, "%s (Resuming) ", Qp.Lines[i].Text1);
 	    Qp.Lines[i].highlight = 1;
*4E
*4I
*6D
 	    sprintf (buf1, "on %s (Resuming)", nodename);
 	    Qblock->Lines[i].highlight = 1;
*6E
*4E
*6D
 	    queuealive = 0;
 	    }			
 
 	if (status & QUI$M_QUEUE_STALLED)
 	    {
*6E
*4D
 	    sprintf (Qp.Lines[i].Text1, "%s (Stalled) ", Qp.Lines[i].Text1);
 	    Qp.Lines[i].highlight = 1;
*4E
*4I
*6D
 	    sprintf (buf1, "on %s (Stalled)", nodename);
 	    Qblock->Lines[i].highlight = 1;
*6E
*4E
*6D
 	    queuealive = 0;
 	    }			
 
 	if (status & QUI$M_QUEUE_STARTING)
 	    {
*6E
*4D
 	    sprintf (Qp.Lines[i].Text1, "%s (Starting) ", Qp.Lines[i].Text1);
 	    Qp.Lines[i].highlight = 1;
*4E
*4I
*6D
 	    sprintf (buf1, "on %s (Starting)", nodename);
 	    Qblock->Lines[i].highlight = 1;
*6E
*4E
*6D
 	    queuealive = 0;
 	    }			
 
 	if (status & QUI$M_QUEUE_STOPPED)
 	    {
*6E
*4D
 	    sprintf (Qp.Lines[i].Text1, "%s (Stopped) ", Qp.Lines[i].Text1);
 	    Qp.Lines[i].highlight = 1;
*4E
*4I
*6D
 	    sprintf (buf1, "on %s (Stopped)", nodename);
 	    Qblock->Lines[i].highlight = 1;
*6E
*4E
*6D
 	    queuealive = 0;
 	    }			
 
 	if (status & QUI$M_QUEUE_STOPPING)
 	    {
*6E
*4D
 	    sprintf (Qp.Lines[i].Text1, "%s (Stopping) ", Qp.Lines[i].Text1);
 	    Qp.Lines[i].highlight = 1;
*4E
*4I
*6D
 	    sprintf (buf1, "on %s (Stopping)", nodename);
 	    Qblock->Lines[i].highlight = 1;
*6E
*4E
*6D
 	    queuealive = 0;
 	    }			
 
 	if (status & QUI$M_QUEUE_UNAVAILABLE)
 	    {
*6E
*4D
 	    sprintf (Qp.Lines[i].Text1, "%s (UNAVAILABLE) ", Qp.Lines[i].Text1);
 	    Qp.Lines[i].highlight = 1;
*4E
*4I
*6D
 	    sprintf (buf1, "on %s (Unavaliable)", nodename);
 	    Qblock->Lines[i].highlight = 1;
*6E
*4E
*6D
 	    queuealive = 0;
 	    }			
 
*6E
*4D
 	if (status & QUI$M_QUEUE_IDLE)
 	    queueidle = 1;
*4E
*4I
*6D
 	if (oldhighlight != Qblock->Lines[i].highlight ||
 	    strcmp (buf, Qblock->Lines[i].Text) != 0   ||
 	    strcmp (buf1, Qblock->Lines[i].Text1) != 0)
 		Qblock->Lines[i].text_changed = 1;
 	strcpy (Qblock->Lines[i].Text, buf);
 	strcpy (Qblock->Lines[i].Text1, buf1);
 
*6E
*4E
*6D
 /*
  * Should we show this queue?
  */
*6E
*4D
 	if (!queueidle)
*4E
*4I
*6D
 	if (queuealive)
*6E
*6I
 	if (Qblock->QueueAlive)
*6E
*4E
 	    {
*4D
 	    Qp.Lines[i].LastLine = 1;
*4E
*4I
*6D
 	    int savedi;
 	    int backup;
 
 	    backup = 1;
 	    savedi = i;
 
 	    Qblock->Lines[i].LastLine = 1;
*6E
*4E
*6D
 	    if (i != 0)
*6E
*4D
 		Qp.Lines[i].LastLine = 0;	    
 /*
  * Should we show any jobs on this queue?
  */
 
 /*
  * Move to next line, for next queue
  */
*4E
*4I
*6D
 		Qblock->Lines[i-1].LastLine = 0;	    
 
*6E
*4E
*6D
 	    i++;
 	    if (i > clu_max_lines)
*6E
*6I
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
*8D
 
*8E
 		if (Qblock->BackupLine)
 		    {
 		    Qblock->CurrentLine = Qblock->SavedLine;
 		    Qblock->Lines[Qblock->CurrentLine].LastLine = 1;
 		    if (Qblock->CurrentLine != 0)
 			Qblock->Lines[Qblock->CurrentLine-1].LastLine = 0;	    
 		    }
*6E
 		return;
*4I
*6D
 /*
  * Should we show any jobs on this queue?
  */
 	    while (1)
*6E
*6I
 		}
     /*
      * Is it one of ours?
      */
 	    if (strncmp (username, Bnr.queue_user, 
*8D
 			strlen(Bnr.queue_user)) != 0 )
*8E
*8I
 			strlen(Bnr.queue_user)) != 0 &&
*9D
 		strcmp (username, "*" != 0))
*9E
*9I
 		strcmp (username, "*") != 0)
*9E
*8E
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
*8D
 			strlen(Bnr.queue_user)) == 0)
*8E
*8I
 			strlen(Bnr.queue_user)) == 0 ||
 		strcmp (username, "*") == 0)
*8E
 		showjob = 1;
 
 
 	    if (status & QUI$M_JOB_EXECUTING ||
 		(Bnr.Param_Bits.jobs_on_hold &&
 		Bnr.Param_Bits.all_users))
 		showjob = 1;
 
 	    if (showjob)
*6E
 		{
*6D
 		int sts;
 		int iosb[2];
 		int intjobcount = 0, intblockcount = 0;
 		int showjob;
 
 		sts = SYS$GETQUIW (0, QUI$_DISPLAY_JOB, 0, &jobinfo, &iosb, 0, 0);
 
 		if (!(sts & SS$_NORMAL) || !(iosb[0] & SS$_NORMAL))
 		    break;
 
 /*
  * Is it one of ours?
  */
 		if (strncmp (username, Bnr.queue_user, 
 			    strlen(Bnr.queue_user)) != 0 )
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
 			    strlen(Bnr.queue_user)) == 0)
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
 			backup = 0;
 			oldhighlight = Qblock->Lines[i].highlight;
 			Qblock->Lines[i].highlight = 0;
 			Qblock->Lines[i].LastLine = 1;
 
 			strcpy (buf, "");
 			strcpy (buf1, "");
 			strcpy (buf2, "");
 
 			sprintf (buf, "-    %d Int Job(s)", intjobcount);
 
 			if (flags == QUI$M_SEARCH_PRINTER)
 			    sprintf (buf1, "%d blocks", intblockcount);
 			    
 			if (oldhighlight != Qblock->Lines[i].highlight ||
 			    strcmp (buf, Qblock->Lines[i].Text) != 0   ||
 			    strcmp (buf1, Qblock->Lines[i].Text1) != 0)
 				Qblock->Lines[i].text_changed = 1;
 			strcpy (Qblock->Lines[i].Text, buf);
 			strcpy (Qblock->Lines[i].Text1, buf1);
 
 			intjobcount = intblockcount = 0;
 
 			Qblock->Lines[i].LastLine = 1;
 			if (i != 0)
 			    Qblock->Lines[i-1].LastLine = 0;	    
 			i++;
 			if (i > clu_max_lines)
 			    return;
 			}
 /*
  * Now display this entry
  */
 		    backup = 0;
 		    oldhighlight = Qblock->Lines[i].highlight;
 		    Qblock->Lines[i].highlight = 0;
 		    Qblock->Lines[i].LastLine = 1;
*6E
*6I
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
*6E
 
 		    strcpy (buf, "");
 		    strcpy (buf1, "");
 		    strcpy (buf2, "");
 
*6D
 		    sprintf (buf, "-    %s", username);
 		    strcpy (buf1, jobname);		
 /*
  * Add flags
  */
 		    if (status & QUI$M_JOB_ABORTING)
 			sprintf (buf1, "%s (Aborting)", buf1);
 		    if (status & QUI$M_JOB_EXECUTING)
 			sprintf (buf1, "%s (Executing)", buf1);
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
 
 		    if (oldhighlight != Qblock->Lines[i].highlight ||
 			strcmp (buf, Qblock->Lines[i].Text) != 0   ||
 			strcmp (buf1, Qblock->Lines[i].Text1) != 0)
 			    Qblock->Lines[i].text_changed = 1;
 		    strcpy (Qblock->Lines[i].Text, buf);
 		    strcpy (Qblock->Lines[i].Text1, buf1);
 
 		    Qblock->Lines[i].LastLine = 1;
 		    if (i != 0)
 			Qblock->Lines[i-1].LastLine = 0;	    
 		    i++;
 		    if (i > clu_max_lines)
 			return;
 
 		    }
 		}
 /*
  * See if we found anything to display
  */
 
 	    if (backup)
 		{
 		i = savedi;
 		Qblock->Lines[i].LastLine = 1;
 		if (i != 0)
 		    Qblock->Lines[i-1].LastLine = 0;	    
 		}
*6E
*6I
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
 
*7D
 		sprintf (buf, "-    %s", username);
*7E
*7I
 		sprintf (buf, "- %d  %s", jobnumber, username);
*7E
 		strcpy (buf1, jobname);		
     /*
      * Add flags
      */
 		if (status & QUI$M_JOB_ABORTING)
 		    sprintf (buf1, "%s (Aborting)", buf1);
 		if (status & QUI$M_JOB_EXECUTING)
*7D
 		    sprintf (buf1, "%s (Executing)", buf1);
*7E
*7I
 		    {
 		    if (flags == QUI$M_SEARCH_PRINTER)
 			sprintf (buf1, "%s (Printing)", buf1);
 		    else
 			sprintf (buf1, "%s (Executing)", buf1);
 		    }
*7E
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
*6E
*4E
 	    }
 	else
*4D
 	    if (i != 0)
 		Qp.Lines[i].LastLine = 1;	    
*4E
*4I
*6D
 	    if (!queuealive && Bnr.Param_Bits.stopped_queues)
*6E
*6I
 	    if (!Qblock->QueueAlive && Bnr.Param_Bits.stopped_queues)
*6E
 		{
*6D
 		Qblock->Lines[i].LastLine = 1;
 		if (i != 0)
 		    Qblock->Lines[i-1].LastLine = 0;	    
 		i++;
 		if (i > clu_max_lines)
 		    return;
 /*
  * This queue is dead, show how many jobs are held on it.
  */
*6E
*6I
     /*
      * This queue is dead, show how many jobs are held on it.
      */
*6E
 		if (holdingcount+pendingcount > 0)
 		    {
*6D
 /*
  * Create a line
  */
 		    oldhighlight = Qblock->Lines[i].highlight;
 		    Qblock->Lines[i].highlight = 1;
 		    Qblock->Lines[i].LastLine = 1;
*6E
*6I
     /*
      * Create a line
      */
 		    oldhighlight = Qblock->Lines[Qblock->CurrentLine].highlight;
 		    Qblock->Lines[Qblock->CurrentLine].highlight = 1;
 		    Qblock->Lines[Qblock->CurrentLine].LastLine = 1;
*6E
 
 		    sprintf (buf, "-    %d Job(s)", holdingcount+pendingcount);
 		    sprintf (buf1, "Holding/Pending");
 
*6D
 		    if (oldhighlight != Qblock->Lines[i].highlight ||
 			strcmp (buf, Qblock->Lines[i].Text) != 0   ||
 			strcmp (buf1, Qblock->Lines[i].Text1) != 0)
 			    Qblock->Lines[i].text_changed = 1;
 		    strcpy (Qblock->Lines[i].Text, buf);
 		    strcpy (Qblock->Lines[i].Text1, buf1);
*6E
*6I
 		    if (oldhighlight != Qblock->Lines[Qblock->CurrentLine].highlight ||
 			strcmp (buf, Qblock->Lines[Qblock->CurrentLine].Text) != 0   ||
 			strcmp (buf1, Qblock->Lines[Qblock->CurrentLine].Text1) != 0)
 			    Qblock->Lines[Qblock->CurrentLine].text_changed = 1;
 		    strcpy (Qblock->Lines[Qblock->CurrentLine].Text, buf);
 		    strcpy (Qblock->Lines[Qblock->CurrentLine].Text1, buf1);
*6E
 
*6D
 		    Qblock->Lines[i].LastLine = 1;
 		    if (i != 0)
 			Qblock->Lines[i-1].LastLine = 0;	    
 		    i++;
 		    if (i > clu_max_lines)
 			return;
 		    }
*6E
*6I
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
*6E
 		}
 	    else
*6D
 		if (!queuealive)
*6E
*6I
 		if (!Qblock->QueueAlive)
*6E
 		    {	
*6D
 		    int savedi, found;
 
 
 		    savedi = i;
 		    found = 0;
 
 		    Qblock->Lines[i].LastLine = 1;
 		    if (i != 0)
 			Qblock->Lines[i-1].LastLine = 0;	    
 		    i++;
 		    if (i > clu_max_lines)
*6E
*6I
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
 
*6E
 			return;
*6D
 /*
  * Scan the queue for our name, and if we find our jobs, display it anyway
  */
 		    while (1)
*6E
*6I
 			}
     /*
      * Is it one of ours?
      */
 		    if (strncmp (username, Bnr.queue_user, 
*8D
 				strlen(Bnr.queue_user)) == 0 )
*8E
*8I
 				strlen(Bnr.queue_user)) == 0 ||
 			strcmp (username, "*") == 0)
*8E
*6E
 			{
*6D
 			int sts;
 			int iosb[2];
 			int intjobcount = 0, intblockcount = 0;
 
 			sts = SYS$GETQUIW (0, QUI$_DISPLAY_JOB, 0, &jobinfo, &iosb, 0, 0);
 
 			if (!(sts & SS$_NORMAL) || !(iosb[0] & SS$_NORMAL))
 			    break;
 
 /*
  * Is it one of ours?
  */
 			if (strncmp (username, Bnr.queue_user, 
 				    strlen(Bnr.queue_user)) == 0 )
*6E
*6I
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
*6E
 			    {
*6D
 			    found = 1;
 			    oldhighlight = Qblock->Lines[i].highlight;
 			    Qblock->Lines[i].highlight = 1;
 			    Qblock->Lines[i].LastLine = 1;
 
 			    strcpy (buf, "");
 			    strcpy (buf1, "");
 			    strcpy (buf2, "");
 
 			    sprintf (buf, "-    %s", username);
 			    strcpy (buf1, jobname);		
 
 			    sprintf (buf1, "%s (Holding)", buf1);
 
 			    if (oldhighlight != Qblock->Lines[i].highlight ||
 				strcmp (buf, Qblock->Lines[i].Text) != 0   ||
 				strcmp (buf1, Qblock->Lines[i].Text1) != 0)
 				    Qblock->Lines[i].text_changed = 1;
 			    strcpy (Qblock->Lines[i].Text, buf);
 			    strcpy (Qblock->Lines[i].Text1, buf1);
 
 			    Qblock->Lines[i].LastLine = 1;
 			    if (i != 0)
 				Qblock->Lines[i-1].LastLine = 0;	    
 			    i++;
 			    if (i > clu_max_lines)
 				return;
 
*6E
*6I
 			    Qblock->Updated = Qblock->Valid = 1;
 			    Qblock->CurrentLine = 0;
 			    Qblock->DoingQueue = 0;
 			    return;			
*6E
 			    }
*6D
 			}
 /*
  * Didnt find any, so dont bother displaying queue
  */
 		    if (!found)
 			{
 			i = savedi;
 			Qblock->Lines[i].LastLine = 1;
 			if (i != 0)
 			    Qblock->Lines[i-1].LastLine = 0;	    
*6E
*6I
 
*6E
 			}
 		    }
*4E
*6I
 
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
*8D
     Qblock->BackupLine = 1;
*8E
*8I
 
     if (!Qblock->QueueAlive)
 	Qblock->BackupLine = 1;
 
*8E
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
*6E
 	}
*3E
*6I
 
*6E
 }
