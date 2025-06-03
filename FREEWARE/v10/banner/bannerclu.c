#module BannerClu "V1.0"

/*
**++
**  FACILITY:
**
**      The DECwindows Banner program
**
**  ABSTRACT:
**
**      This module contains all the code, to run the Cluster Node monitor.
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
#include "syidef.h"
#include "MrmAppl.h"


#include "Banner"


static void GetNodeInfo ();
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
extern   Clu$_Blk    Clu;
extern	 GC	     BannerGC;
extern	 XGCValues   GcValues;

extern void BannerSignalError();

static int Clu_update_count, Clu_update_number;
static int Clu_init = 0;

static char nodename[31];
static int  nodenamelen;
static char hwname[31];
static int  hwnamelen;
static int  cpus;
static int  acpus;
static int  member;
static char hwtype[10];
static char vmsversion[10];

static VMS_ItemList nodeitemlist[8] = {
	31, SYI$_NODENAME, &nodename, &nodenamelen,
	31, SYI$_HW_NAME, &hwname, &hwnamelen,
	4,  SYI$_NODE_SWTYPE, &hwtype, 0,
	4,  SYI$_NODE_SWVERS,  &vmsversion, 0,
	4,  SYI$_CLUSTER_MEMBER, &member, 0,
	4,  SYI$_ACTIVECPU_CNT, &acpus, 0,
	4,  SYI$_AVAILCPU_CNT, &cpus, 0,
	0, 0, 0, 0};	



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	BannerCluSetup ()
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
BannerCluSetup()
{


    Clu_update_count = 1;
    if (Bnr.clu_update == 0)
	Clu_update_number = Bnr.cpu_update;
    else
	Clu_update_number = Bnr.clu_update;

    Clu.Lines[0].LastLine = 1;

    Clu.Lines[0].MaxNameSize = 8;
    Clu.Lines[0].MaxNamePoints = 
	XTextWidth(Bnr.font_ptr, " XXXXXX ", 8);
    Clu.Lines[0].MaxTextSize = 8;
    Clu.Lines[0].MaxTextPoints = 
	XTextWidth(Bnr.font_ptr, "- V8.8XX", 8);

    Clu_init = 1;
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	BannerCluRefresh ()
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
BannerCluRefresh()
{

    if (!Clu_init)
	BannerCluSetup ();

    GetNodeInfo ();

    BannerPaintTextLines (XtDisplay(Bnr.clu_widget), XtWindow(Bnr.clu_widget), 
	Bnr.clu_width, Bnr.clu_height, 
	&Clu.Lines[0], 1);
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	BannerClu ()
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
BannerClu()
{

    if (!Clu_init)
	BannerCluRefresh ();

    Clu_update_count --;
    if (Clu_update_count > 0)
	return;
	
    Clu_update_count = Clu_update_number;

    GetNodeInfo ();

    BannerPaintTextLines (XtDisplay(Bnr.clu_widget), XtWindow(Bnr.clu_widget), 
	Bnr.clu_width, Bnr.clu_height, 
	&Clu.Lines[0], 0);

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      GetNodeInfo ()
**
**  Gets info about the VMS nodes in the cluster
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
static void	GetNodeInfo()
{
int i=0;
int csid = -1;
int size;
int maxsize, maxpoints;
/*
 * All nodes in the cluster
 */
    
    maxsize = 0;
    maxpoints = 0;
/*
 * Mark all nodes as unseen
 */

    i = 0;
    while (1)
	{
	Clu.Lines[i].seen = 0;
	Clu.Lines[i].text_changed = 0;
	if (Clu.Lines[i].LastLine == 1)
	    break;
	i++;
	}
/*
 * Get this node
 */
    if (SYS$GETSYIW (0, &0, 0, &nodeitemlist, 0, 0, 0) == SS$_NORMAL)
	{
	char buf[40];
	int oldhighlight;
	int found = 0;

	nodename[nodenamelen]=0;
	hwname[hwnamelen] = 0;
	vmsversion[4] = 0;
	hwtype[4] = 0;
/*
 * Find this node in the list
 */
	i = 0;
	sprintf (buf, "%s", nodename);
	while (1)
	    {
	    if (strcmp (Clu.Lines[i].Text, buf) == 0)
		{
		found = 1;
		break;
		}
	    if (Clu.Lines[i].LastLine == 1)
		break;
	    i++;
	    }

/*
 * New node?
 */	
	if (!found)
	    {

	    Clu.Lines[i].LastLine = 0;
/*
 * If the last line was used, and create a new line
 */
	    if (strlen (Clu.Lines[i].Text) != 0)
		i++;
	    Clu.Lines[i].text_changed = 1;
	    Clu.Lines[i].LastLine = 1;
	    }
	    
	oldhighlight = Clu.Lines[i].highlight;
	Clu.Lines[i].seen = 1;
	Clu.Lines[i].highlight = 0;

	sprintf (Clu.Lines[i].Text, "%s", nodename);
	if (strncmp (hwtype, "VMS", 3) == 0)
	    sprintf (Clu.Lines[i].Text1, "- %s", vmsversion);
	else
	    sprintf (Clu.Lines[i].Text1, "- %s", hwtype);
/*
 * Dead CPU's?
 */    
	if (cpus != acpus && !Clu.Lines[i].highlight)
	    {
	    sprintf (Clu.Lines[i].Text1, "- %d/%d", acpus, cpus);
	    Clu.Lines[i].highlight = 1;
	    Clu.Lines[i].text_changed = 1;
	    }
/*
 * If it was prevously highlighted
 * then the text has changed.
 */
	if (Clu.Lines[i].highlight != oldhighlight)
	    Clu.Lines[i].text_changed = 1;

	}

/*
 * Now get cluster members
 */
    while (SYS$GETSYIW (0, &csid, 0, &nodeitemlist, 0, 0, 0) == SS$_NORMAL)
	{
	char buf[40];
	int oldhighlight;
	int found = 0;

	nodename[nodenamelen]=0;
	hwname[hwnamelen] = 0;
	vmsversion[4] = 0;
	hwtype[4] = 0;
/*
 * Find this node in the list
 */
	i = 0;
	sprintf (buf, "%s", nodename);
	while (1)
	    {
	    if (strcmp (Clu.Lines[i].Text, buf) == 0)
		{
		found = 1;
		break;
		}
	    if (Clu.Lines[i].LastLine == 1)
		break;
	    i++;
	    }

/*
 * New node?
 */	
	if (!found)
	    {
	    if (i+1 >= clu_max_lines)
		break;

	    Clu.Lines[i].LastLine = 0;
/*
 * If the last line was used, and create a new line
 */
	    if (strlen (Clu.Lines[i].Text) != 0)
		i++;
	    Clu.Lines[i].text_changed = 1;
	    Clu.Lines[i].LastLine = 1;
	    }
	    
	oldhighlight = Clu.Lines[i].highlight;
	Clu.Lines[i].seen = 1;
	Clu.Lines[i].highlight = 0;

	sprintf (Clu.Lines[i].Text, "%s", nodename);
	if (strncmp (hwtype, "VMS", 3) == 0)
	    sprintf (Clu.Lines[i].Text1, "- %s", vmsversion);
	else
	    sprintf (Clu.Lines[i].Text1, "- %s", hwtype);
/*
 * Dead CPU's?
 */    
	if (cpus != acpus && !Clu.Lines[i].highlight)
	    {
	    sprintf (Clu.Lines[i].Text1, "- %d/%d", acpus, cpus);
	    Clu.Lines[i].highlight = 1;
	    Clu.Lines[i].text_changed = 1;
	    }
/*
 * Not a member?
 */
	if ((member & 1) != 1  && !Clu.Lines[i].highlight)
	    {
	    strcpy (Clu.Lines[i].Text1, "- BRK");
	    Clu.Lines[i].highlight = 1;	    
	    Clu.Lines[i].text_changed = 1;
	    }	    
/*
 * If it was prevously highlighted
 * then the text has changed.
 */
	if (Clu.Lines[i].highlight != oldhighlight)
	    Clu.Lines[i].text_changed = 1;

	}
/*
 * Now see if any nodes went away?
 */
i=0;
while (1)
    {
    if (!Clu.Lines[i].seen && !Clu.Lines[i].highlight)
	{
	strcpy (Clu.Lines[i].Text1, "- BRK");
	Clu.Lines[i].text_changed = 1;
	Clu.Lines[i].highlight = 1;	
	}

    if (Clu.Lines[i].LastLine == 1)
	break;
    i++;
    }
}
