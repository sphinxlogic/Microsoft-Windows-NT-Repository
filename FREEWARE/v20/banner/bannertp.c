#module BannerTP "V1.0"

/*
**++
**  FACILITY:
**
**      The DECwindows Banner program
**
**  ABSTRACT:
**
**      This module contains all the code concerned with the TP monitor
**  window in the banner program.
**
**  AUTHORS:
**
**      Jim Saunders
**
**
**  CREATION DATE:     20th May 1990
**
**  MODIFICATION HISTORY:
**--
**/


#include "stdio.h"
#include "MrmAppl.h"


#include "Banner"


/*
**
**  MACRO DEFINITIONS
**
**/


void BannerTPSetup ();
void BannerTPRefresh ();
void BannerTP ();

/* 
 * Definitions of other static items we use throughout the banner program
 * to reduce our setup times. 
 */

extern	 Bnr$_Blk    Bnr;
extern	 Tp$_Blk     Tp;
extern   Mon$_Blk    Mon;
extern	 GC	     BannerGC;

extern   char	     *BannerVmsDisplayName;
extern   char	     BannerVmsNodeName[];
extern   char	     BannerVmsVersion[];

typedef struct _ddtm_data {
	char	fill [148];
	int	DDTM_STARTS;
	int	DDTM_PREPARES;
	int	DDTM_COMMITS;
	int	DDTM_ABORTS;
	int	DDTM_ENDS;
	int	DDTM_ONE_PHASE;
	int	DDTM_BRANCHS;
	int	DDTM_ADDS;
	char	fill1 [16];
	int	DDTM_BUCKETS[6];
	} ddtm$data;

globalref ddtm$data  *DDTM$AR_PERFORMANCE_CELLS;


static int tp_update_count, tp_update_number;
static int tp_hwmrk_count, tp_hwmrk_number;
static int tp_init = 0;

static int last_ddtm_starts = 0;
static int last_ddtm_prepares = 0;
static int last_ddtm_one_phase = 0;
static int last_ddtm_commits = 0;
static int last_ddtm_aborts = 0;
static int last_ddtm_ends = 0;
static int last_ddtm_branchs = 0;
static int last_ddtm_adds = 0;
static int last_ddtm_buckets[6] = {0,0,0,0,0,0};


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      BannerTPSetup ()
**
**  FORMAL PARAMETERS:
**
**      This is the setup routine for the Banner TP Window display.
**
**  IMPLICIT INPUTS:
**
**      The TP data block
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
void	BannerTPSetup()
{
    int maxnamesize, maxnamepoints;
    int tw, twsize;
    static char membuf[125];
    static char tval2[5], tval3[5], tval4[5];

    tp_update_count = 1;

    if (Bnr.hwmrk_update == 0)
	tp_hwmrk_number = 300;
    else
	tp_hwmrk_number = Bnr.hwmrk_update;
    tp_hwmrk_count = tp_hwmrk_number;

    if (Bnr.tp_update == 0)
	tp_update_number = Bnr.cpu_update;
    else
	tp_update_number = Bnr.tp_update;

/*
 * Now setup the tp data block.
 */

    if (DDTM$AR_PERFORMANCE_CELLS != NULL)
	{
	last_ddtm_starts = DDTM$AR_PERFORMANCE_CELLS->DDTM_STARTS;
	last_ddtm_prepares = DDTM$AR_PERFORMANCE_CELLS->DDTM_PREPARES;
	last_ddtm_one_phase = DDTM$AR_PERFORMANCE_CELLS->DDTM_ONE_PHASE;
	last_ddtm_commits = DDTM$AR_PERFORMANCE_CELLS->DDTM_COMMITS;
	last_ddtm_aborts = DDTM$AR_PERFORMANCE_CELLS->DDTM_ABORTS;
	last_ddtm_ends = DDTM$AR_PERFORMANCE_CELLS->DDTM_ENDS;
	last_ddtm_branchs = DDTM$AR_PERFORMANCE_CELLS->DDTM_BRANCHS;
	last_ddtm_adds = DDTM$AR_PERFORMANCE_CELLS->DDTM_ADDS;
	last_ddtm_buckets[0] = DDTM$AR_PERFORMANCE_CELLS->DDTM_BUCKETS[0];
	last_ddtm_buckets[1] = DDTM$AR_PERFORMANCE_CELLS->DDTM_BUCKETS[1];
	last_ddtm_buckets[2] = DDTM$AR_PERFORMANCE_CELLS->DDTM_BUCKETS[2];
	last_ddtm_buckets[3] = DDTM$AR_PERFORMANCE_CELLS->DDTM_BUCKETS[3];
	last_ddtm_buckets[4] = DDTM$AR_PERFORMANCE_CELLS->DDTM_BUCKETS[4];
	last_ddtm_buckets[5] = DDTM$AR_PERFORMANCE_CELLS->DDTM_BUCKETS[5];
	}

    memset (&Tp, 0, sizeof(Tp));

    maxnamesize = 14;
    maxnamepoints = XTextWidth(Bnr.font_ptr, "1Phase Commits", 14);


    if (Bnr.Param_Bits.total_com_rate)
	{
	Tp.total_com_rate.Name = "Total Commit";
	Tp.total_com_rate.LineActive = 1;
	Tp.total_com_rate.MaxVal = Bnr.tp_max_rate;
	Tp.total_com_rate.MaxNameSize = maxnamesize;
	Tp.total_com_rate.MaxNamePoints = maxnamepoints;
	}

    if (Bnr.Param_Bits.onef_comp_rate)
	{
	Tp.onef_comp_rate.Name = "1Phase Commit";
	Tp.onef_comp_rate.LineActive = 1;
	Tp.onef_comp_rate.MaxVal = Bnr.tp_max_rate;
	Tp.onef_comp_rate.MaxNameSize = maxnamesize;
	Tp.onef_comp_rate.MaxNamePoints = maxnamepoints;
	}

    if (Bnr.Param_Bits.start_rate || Bnr.Param_Bits.prep_rate)
	{
	int half = 0;
	if (Bnr.Param_Bits.start_rate)
	    {
	    half = 1;
	    Tp.start_rate.Name = "Start Rate";
	    Tp.start_rate.LineActive = 1;
	    Tp.start_rate.MaxVal = Bnr.tp_max_rate;
	    Tp.start_rate.MaxNameSize = maxnamesize;
	    Tp.start_rate.MaxNamePoints = maxnamepoints;
	    }
	if (Bnr.Param_Bits.prep_rate)
	    {
	    if (half)
		{
		Tp.start_rate.Name = Tp.prep_rate.Name = "Start/Prep";
		}
	    else
		{
		Tp.prep_rate.Name = "Prep Rate";
		}	    
	    Tp.prep_rate.MaxVal = Bnr.tp_max_rate;
	    Tp.prep_rate.LineActive = 1;
	    Tp.prep_rate.HalfLine = half;
	    Tp.prep_rate.MaxNameSize = maxnamesize;
	    Tp.prep_rate.MaxNamePoints = maxnamepoints;
	    }
	}


    if (Bnr.Param_Bits.abort_rate || Bnr.Param_Bits.end_rate)
	{
	int half = 0;
	if (Bnr.Param_Bits.abort_rate)
	    {
	    half = 1;
	    Tp.abort_rate.Name = "Abort Rate";
	    Tp.abort_rate.LineActive = 1;
	    Tp.abort_rate.MaxVal = Bnr.tp_max_rate;
	    Tp.abort_rate.MaxNameSize = maxnamesize;
	    Tp.abort_rate.MaxNamePoints = maxnamepoints;
	    }
	if (Bnr.Param_Bits.end_rate)
	    {
	    if (half)
		{
		Tp.abort_rate.Name = Tp.end_rate.Name = "Abort/End";
		}
	    else
		{
		Tp.end_rate.Name = "End Rate";
		}	    
	    Tp.end_rate.MaxVal = Bnr.tp_max_rate;
	    Tp.end_rate.LineActive = 1;
	    Tp.end_rate.HalfLine = half;
	    Tp.end_rate.MaxNameSize = maxnamesize;
	    Tp.end_rate.MaxNamePoints = maxnamepoints;
	    }
	}


    if (Bnr.Param_Bits.rem_start_rate || Bnr.Param_Bits.rem_end_rate)
	{
	int half = 0;
	if (Bnr.Param_Bits.rem_start_rate)
	    {
	    half = 1;
	    Tp.rem_start_rate.Name = "Rem Start Rate";
	    Tp.rem_start_rate.LineActive = 1;
	    Tp.rem_start_rate.MaxVal = Bnr.tp_max_rate;
	    Tp.rem_start_rate.MaxNameSize = maxnamesize;
	    Tp.rem_start_rate.MaxNamePoints = maxnamepoints;
	    }
	if (Bnr.Param_Bits.rem_end_rate)
	    {
	    if (half)
		{
		Tp.rem_start_rate.Name = Tp.rem_end_rate.Name = "Rem Start/Add";
		}
	    else
		{
		Tp.rem_end_rate.Name = "Rem Add Rate";
		}	    
	    Tp.rem_end_rate.MaxVal = Bnr.tp_max_rate;
	    Tp.rem_end_rate.LineActive = 1;
	    Tp.rem_end_rate.HalfLine = half;
	    Tp.rem_end_rate.MaxNameSize = maxnamesize;
	    Tp.rem_end_rate.MaxNamePoints = maxnamepoints;
	    }
	}


    if (Bnr.Param_Bits.comp_rate1 || Bnr.Param_Bits.comp_rate2)
	{
	static char buf[20];
	int half = 0;
	if (Bnr.Param_Bits.comp_rate1)
	    {
	    half = 1;
	    Tp.comp_rate1.Name = &buf;
	    Tp.comp_rate1.LineActive = 1;
	    Tp.comp_rate1.MaxVal = Bnr.tp_max_rate;
	    Tp.comp_rate1.MaxNameSize = maxnamesize;
	    Tp.comp_rate1.MaxNamePoints = maxnamepoints;
	    sprintf (buf, "%d-%d Rate", Bnr.tp_rate1_start, Bnr.tp_rate1_end);
	    }
	if (Bnr.Param_Bits.comp_rate2)
	    {
	    if (half)
		{
		Tp.comp_rate1.Name = Tp.comp_rate2.Name = &buf;
		sprintf (buf, "%d-%d/%d-%d", Bnr.tp_rate1_start,
		    Bnr.tp_rate1_end, Bnr.tp_rate2_start, Bnr.tp_rate2_end);
		}
	    else
		{
		Tp.comp_rate2.Name = &buf;
		sprintf (buf, "%d-%d Rate", Bnr.tp_rate2_start, Bnr.tp_rate2_end);
		}	    
	    Tp.comp_rate2.MaxVal = Bnr.tp_max_rate;
	    Tp.comp_rate2.LineActive = 1;
	    Tp.comp_rate2.HalfLine = half;
	    Tp.comp_rate2.MaxNameSize = maxnamesize;
	    Tp.comp_rate2.MaxNamePoints = maxnamepoints;
	    }
	}

    if (Bnr.Param_Bits.comp_rate3 || Bnr.Param_Bits.comp_rate4)
	{
	static char buf1[20];
	int half = 0;
	if (Bnr.Param_Bits.comp_rate3)
	    {
	    half = 1;
	    Tp.comp_rate3.Name = &buf1;
	    Tp.comp_rate3.LineActive = 1;
	    Tp.comp_rate3.MaxVal = Bnr.tp_max_rate;
	    Tp.comp_rate3.MaxNameSize = maxnamesize;
	    Tp.comp_rate3.MaxNamePoints = maxnamepoints;
	    sprintf (buf1, "%d-%d Rate", Bnr.tp_rate3_start, Bnr.tp_rate3_end);
	    }
	if (Bnr.Param_Bits.comp_rate4)
	    {
	    if (half)
		{
		Tp.comp_rate3.Name = Tp.comp_rate4.Name = &buf1;
		sprintf (buf1, "%d-%d/%d+", Bnr.tp_rate3_start,
		    Bnr.tp_rate3_end, Bnr.tp_rate4_start);
		}
	    else
		{
		Tp.comp_rate4.Name = &buf1;
		sprintf (buf1, "%d+ Rate", Bnr.tp_rate4_start);
		}	    
	    Tp.comp_rate4.MaxVal = Bnr.tp_max_rate;
	    Tp.comp_rate4.LineActive = 1;
	    Tp.comp_rate4.HalfLine = half;
	    Tp.comp_rate4.MaxNameSize = maxnamesize;
	    Tp.comp_rate4.MaxNamePoints = maxnamepoints;
	    }
	}

    Tp.total_com_rate.MaxNameSize = maxnamesize;
    Tp.total_com_rate.MaxNamePoints = maxnamepoints;
    if (BannerVmsDisplayName == NULL)
	Tp.total_com_rate.Title = &BannerVmsNodeName;
    else
	Tp.total_com_rate.Title = BannerVmsDisplayName;
    sprintf(tval2, "%d#", Bnr.tp_max_rate/4);
    sprintf(tval3, "%d#", Bnr.tp_max_rate - Bnr.tp_max_rate/4);
    sprintf(tval4, "%d#", Bnr.tp_max_rate);
    Tp.total_com_rate.tval1 = "0#";
    Tp.total_com_rate.tval2 = &tval2;
    Tp.total_com_rate.tval3 = &tval3;
    Tp.total_com_rate.tval4 = &tval4;
    Tp.total_com_rate.TitleChanged = 1;
    Tp.comp_rate4.LastLine = 1;

    tp_init = 1;

    return;
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      BannerTPRefresh ()
**
**  FORMAL PARAMETERS:
**
**      This is the refresh routine for the Banner TP Window display.
**
**  IMPLICIT INPUTS:
**
**      The TP data block
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
void	BannerTPRefresh()
{

    if (!tp_init)
	BannerTPSetup ();

    BannerPaintGraphics (XtDisplay(Bnr.tp_widget), XtWindow(Bnr.tp_widget), 
	Bnr.tp_width, 
	&Tp, 1);

    return;
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      BannerTP ()
**
**  FORMAL PARAMETERS:
**
**	This is the main routine for the Banner TP window display, it 
**  gathers all the statistics, and then calls the display routine, to 
**  display the lines needed.
**
**  IMPLICIT INPUTS:
**
**      The TP data block
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
void	BannerTP()
{
int i;
/*
 * If we have never inited then do it.
 */
    if (!tp_init)
	BannerTPRefresh ();
/*
 * Now check if High water marks should be reset
 */
    tp_hwmrk_count--;
    if (tp_hwmrk_count == 0)
	{
	Display$_Line *Line;

/*
 * reset all high water marks
 */
	tp_hwmrk_count = tp_hwmrk_number;

	Line = &Tp;
	while (1) 
	    {
	    Line->Hwmrk = 0;
	    Line = (int)Line + (int)sizeof(Display$_Line);
	    if (Line->LastLine == 1)
		break;
	    }
	}     
/*
 * Now check if we should sample this pass?
 */    
    tp_update_count --;
    if (tp_update_count > 0)
	return;
	
    tp_update_count = tp_update_number;

/*
 * Now store all the changes stats
 */
    if (DDTM$AR_PERFORMANCE_CELLS == NULL)
	return;

    Tp.onef_comp_rate.CurVal =  DDTM$AR_PERFORMANCE_CELLS->DDTM_ONE_PHASE - last_ddtm_one_phase;
    Tp.total_com_rate.CurVal = Tp.onef_comp_rate.CurVal +
	DDTM$AR_PERFORMANCE_CELLS->DDTM_COMMITS - last_ddtm_commits;
    Tp.start_rate.CurVal = DDTM$AR_PERFORMANCE_CELLS->DDTM_STARTS 
	- last_ddtm_starts ;
    Tp.prep_rate.CurVal = DDTM$AR_PERFORMANCE_CELLS->DDTM_PREPARES 
	- last_ddtm_prepares;
    Tp.abort_rate.CurVal = DDTM$AR_PERFORMANCE_CELLS->DDTM_ABORTS 
	- last_ddtm_aborts;
    Tp.end_rate.CurVal = DDTM$AR_PERFORMANCE_CELLS->DDTM_ENDS 
	- last_ddtm_ends;
    Tp.rem_start_rate.CurVal = DDTM$AR_PERFORMANCE_CELLS->DDTM_BRANCHS 
	- last_ddtm_branchs;
    Tp.rem_end_rate.CurVal = DDTM$AR_PERFORMANCE_CELLS->DDTM_ADDS 
	- last_ddtm_adds;

    Tp.comp_rate1.CurVal = 0;
    Tp.comp_rate2.CurVal = 0;
    Tp.comp_rate3.CurVal = 0;
    Tp.comp_rate4.CurVal = 0;

    for (i=0;  i<=5;  i++)
	{
	if (Bnr.tp_rate1_start <= i && Bnr.tp_rate1_end > i)	
	    Tp.comp_rate1.CurVal += 
		DDTM$AR_PERFORMANCE_CELLS->DDTM_BUCKETS[i] - 
		    last_ddtm_buckets[i];

	if (Bnr.tp_rate2_start <= i && Bnr.tp_rate2_end > i)	
	    Tp.comp_rate2.CurVal += 
		DDTM$AR_PERFORMANCE_CELLS->DDTM_BUCKETS[i] - 
		    last_ddtm_buckets[i];

	if (Bnr.tp_rate3_start <= i && Bnr.tp_rate3_end > i)	
	    Tp.comp_rate3.CurVal += 
		DDTM$AR_PERFORMANCE_CELLS->DDTM_BUCKETS[i] - 
		    last_ddtm_buckets[i];

	if (Bnr.tp_rate4_start <= i)	
	    Tp.comp_rate4.CurVal += 
		DDTM$AR_PERFORMANCE_CELLS->DDTM_BUCKETS[i] - 
		    last_ddtm_buckets[i];
	}
	

    last_ddtm_starts = DDTM$AR_PERFORMANCE_CELLS->DDTM_STARTS;
    last_ddtm_prepares = DDTM$AR_PERFORMANCE_CELLS->DDTM_PREPARES;
    last_ddtm_one_phase = DDTM$AR_PERFORMANCE_CELLS->DDTM_ONE_PHASE;
    last_ddtm_commits = DDTM$AR_PERFORMANCE_CELLS->DDTM_COMMITS;
    last_ddtm_aborts = DDTM$AR_PERFORMANCE_CELLS->DDTM_ABORTS;
    last_ddtm_ends = DDTM$AR_PERFORMANCE_CELLS->DDTM_ENDS;
    last_ddtm_branchs = DDTM$AR_PERFORMANCE_CELLS->DDTM_BRANCHS;
    last_ddtm_adds = DDTM$AR_PERFORMANCE_CELLS->DDTM_ADDS;
    last_ddtm_buckets[0] = DDTM$AR_PERFORMANCE_CELLS->DDTM_BUCKETS[0];
    last_ddtm_buckets[1] = DDTM$AR_PERFORMANCE_CELLS->DDTM_BUCKETS[1];
    last_ddtm_buckets[2] = DDTM$AR_PERFORMANCE_CELLS->DDTM_BUCKETS[2];
    last_ddtm_buckets[3] = DDTM$AR_PERFORMANCE_CELLS->DDTM_BUCKETS[3];
    last_ddtm_buckets[4] = DDTM$AR_PERFORMANCE_CELLS->DDTM_BUCKETS[4];
    last_ddtm_buckets[5] = DDTM$AR_PERFORMANCE_CELLS->DDTM_BUCKETS[5];

/*
 * Now get all the display printed
 */

    Tp.total_com_rate.TitleChanged = 0;

    BannerPaintGraphics (XtDisplay(Bnr.tp_widget), XtWindow(Bnr.tp_widget), 
	Bnr.tp_width, 
	&Tp, 0);

    return;
}
