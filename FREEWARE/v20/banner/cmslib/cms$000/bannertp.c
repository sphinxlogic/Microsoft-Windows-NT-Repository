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
*3D
 **  CREATION DATE:     278th September 1989
*3E
*3I
 **  CREATION DATE:     20th May 1990
*3E
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
*3D
 static void CollectCpuData ();
 
 extern void BANNERCMKRNLTRAP();
*3E
*3I
 
*3E
 /* 
  * Definitions of other static items we use throughout the banner program
  * to reduce our setup times. 
  */
 
 extern	 Bnr$_Blk    Bnr;
*3D
 extern	 Tp$_Blk    Tp;
*3E
*3I
 extern	 Tp$_Blk     Tp;
*3E
 extern   Mon$_Blk    Mon;
 extern	 GC	     BannerGC;
 
*6I
*7D
 extern   char	     *BannerVmsDisplayName[];
*7E
*7I
 extern   char	     *BannerVmsDisplayName;
*7E
*6E
 extern   char	     BannerVmsNodeName[];
 extern   char	     BannerVmsVersion[];
 
*3D
 globalref int MON_V_NI_MNTVERIP, MON_V_NI_BUSY, MON_V_NI_SYS_MV;
 
 static int OLD_MON_V_NI_MNTVERIP, OLD_MON_V_NI_BUSY, OLD_MON_V_NI_SYS_MV;
*3E
*3I
 typedef struct _ddtm_data {
*4I
 	char	fill [148];
*4E
 	int	DDTM_STARTS;
 	int	DDTM_PREPARES;
*4D
 	int	DDTM_ONE_PHASE;
*4E
 	int	DDTM_COMMITS;
 	int	DDTM_ABORTS;
 	int	DDTM_ENDS;
*4I
 	int	DDTM_ONE_PHASE;
*4E
 	int	DDTM_BRANCHS;
 	int	DDTM_ADDS;
*5I
 	char	fill1 [16];
*5E
 	int	DDTM_BUCKETS[6];
 	} ddtm$data;
 
*5D
 globalref ddtm$data  DDTM$AR_PERFORMANCE_CELLS;
*5E
*5I
 globalref ddtm$data  *DDTM$AR_PERFORMANCE_CELLS;
*5E
*3E
 
 
 static int tp_update_count, tp_update_number;
 static int tp_hwmrk_count, tp_hwmrk_number;
 static int tp_init = 0;
 
*3D
 static float last_total = 0;
 static float last_null = 0;
 
 static float total_time = 0;
 static float total_null = 0;
 
 static int last_pfg_rate;
 static int last_syf_rate;
 static int last_pg_io;
 static int last_rem_io;
 static int last_disk1_io;
 static int last_disk2_io;
 static int last_disk3_io;
 
 typedef struct _cpudata {
 char fill_1[632];
 int cpu$l_kernel;
 int cpu$l_executive;
 int cpu$l_super;
 int cpu$l_user;
 int cpu$l_interrupt;
 int cpu$l_compatibility;
 int cpu$l_spinlock;
 int cpu$l_nullcpu;
 } cpu$data;
     
 globalref cpu$data *SMP$GL_CPU_DATA[];	    /* CPU data pointer array */
 globalref int SMP$GL_ACTIVE_CPUS;	    /* Active cpu mask */
 
 globalref int PMS$GL_PREADIO, PMS$GL_PWRITIO, PMS$GL_FAULTS, 
 	   SCH$GL_MFYCNT, SCH$GL_MFYLIM, SCH$GL_FREECNT, MMG$GL_MAXMEM;
 
 globalref short int SGN$GW_PAGFILCT, SGN$GW_SWPFILES;
 globalref int (*MMG$GL_PAGSWPVC)[];
 
 typedef struct _pgfiledata {
 int	    bitmap_addr;
 int	    scan_offset;
 short int   size;
 char	    type;
 char	    pfc;
 int	    wincntrlblk;
 int	    bvn;
 int	    bmap_size;
 int	    alloc_size;
 int	    res_size;
 int	    bitmap;
 } pagefile$data;
 
 static int pgf_size, pgf_alloc_size, pgf_res_size;
 static int swp_size, swp_alloc_size, swp_res_size;
*3E
*3I
 static int last_ddtm_starts = 0;
 static int last_ddtm_prepares = 0;
 static int last_ddtm_one_phase = 0;
*4D
 static int  last_ddtm_commits = 0;
*4E
*4I
 static int last_ddtm_commits = 0;
*4E
 static int last_ddtm_aborts = 0;
 static int last_ddtm_ends = 0;
 static int last_ddtm_branchs = 0;
 static int last_ddtm_adds = 0;
 static int last_ddtm_buckets[6] = {0,0,0,0,0,0};
*3E
 
 
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
*3I
     static char tval2[5], tval3[5], tval4[5];
*3E
 
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
 
*3D
     BannerMonSetup ();
     CollectCpuData (0, 0);
 
     last_disk1_io = BannerDeviceCount (0, 1);
     last_disk2_io = BannerDeviceCount (1, 1);
     last_disk3_io = BannerDeviceCount (2, 1);
 
     OLD_MON_V_NI_SYS_MV = -1;
     OLD_MON_V_NI_MNTVERIP = -1;
     OLD_MON_V_NI_BUSY = -1;
*3E
*3I
*5D
     if (&DDTM$AR_PERFORMANCE_CELLS != NULL)
*5E
*5I
     if (DDTM$AR_PERFORMANCE_CELLS != NULL)
*5E
 	{
*5D
 	last_ddtm_starts = DDTM$AR_PERFORMANCE_CELLS.DDTM_STARTS;
 	last_ddtm_prepares = DDTM$AR_PERFORMANCE_CELLS.DDTM_PREPARES;
 	last_ddtm_one_phase = DDTM$AR_PERFORMANCE_CELLS.DDTM_ONE_PHASE;
*5E
*4I
*5D
 	last_ddtm_commits = DDTM$AR_PERFORMANCE_CELLS.DDTM_COMMITS;
*5E
*4E
*5D
 	last_ddtm_aborts = DDTM$AR_PERFORMANCE_CELLS.DDTM_ABORTS;
 	last_ddtm_ends = DDTM$AR_PERFORMANCE_CELLS.DDTM_ENDS;
 	last_ddtm_branchs = DDTM$AR_PERFORMANCE_CELLS.DDTM_BRANCHS;
 	last_ddtm_adds = DDTM$AR_PERFORMANCE_CELLS.DDTM_ADDS;
 	memcpy(&last_ddtm_buckets[0],
 	    &DDTM$AR_PERFORMANCE_CELLS.DDTM_BUCKETS[0],6*sizeof(int) );
*5E
*5I
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
*5E
 	}
*3E
 
     memset (&Tp, 0, sizeof(Tp));
 
*3D
     maxnamesize = 13;
     maxnamepoints = XTextWidth(Bnr.font_ptr, "Pgf/Sysf Rate", 13);
 
     if (Mon.dua0 && Bnr.disk_0 != NULL)
 	{
 	tw = name_size(Bnr.disk_0_name) - 1;
 	twsize = XTextWidth(Bnr.font_ptr, Bnr.disk_0_name, 
 		tw);
 	if (twsize > maxnamepoints) 
 	    {
 	    maxnamesize = tw;
 	    maxnamepoints = twsize;
 	    }
 	}
 
 
     if (Mon.dua1 && Bnr.disk_1 != NULL)
 	{
 	tw = name_size(Bnr.disk_1_name) - 1;
 	twsize = XTextWidth(Bnr.font_ptr, Bnr.disk_1_name, 
 		tw);
 	if (twsize > maxnamepoints) 
 	    {
 	    maxnamesize = tw;
 	    maxnamepoints = twsize;
 	    }
 	}
 
 
     if (Mon.dua2 && Bnr.disk_2 != NULL)
 	{
 	tw = name_size(Bnr.disk_2_name) - 1;
 	twsize = XTextWidth(Bnr.font_ptr, Bnr.disk_2_name, 
 		tw);
 	if (twsize > maxnamepoints) 
*3E
*3I
     maxnamesize = 14;
*4D
     maxnamepoints = XTextWidth(Bnr.font_ptr, "1Phase Commitx", 14);
*4E
*4I
     maxnamepoints = XTextWidth(Bnr.font_ptr, "1Phase Commits", 14);
*4E
 
 
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
*3E
 	    {
*3D
 	    maxnamesize = tw;
 	    maxnamepoints = twsize;
*3E
*3I
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
*3E
 	    }
 	}
 
 
*3D
     if (Mon.dua0)
 	{
 	Tp.Disk1.Name = Bnr.disk_0_name;
 	Tp.Disk1.LineActive = 1;
 	Tp.Disk1.MaxVal = BannerDeviceCount (0, 2);
 
 	Tp.Disk1Rate.Name = Bnr.disk_0_name;
 	Tp.Disk1Rate.MaxVal = Bnr.sys_ioratemax;
 	Tp.Disk1Rate.LineActive = Tp.Disk1Rate.HalfLine = 1;
 	Tp.Disk1Rate.MaxNameSize = maxnamesize;
 	Tp.Disk1Rate.MaxNamePoints = maxnamepoints;
 	}
 
     if (Mon.dua1)
 	{
 	Tp.Disk2.Name = Bnr.disk_1_name;
 	Tp.Disk2.LineActive = 1;
 	Tp.Disk2.MaxVal = BannerDeviceCount (1, 2);
 	Tp.Disk2.MaxNameSize = maxnamesize;
 	Tp.Disk2.MaxNamePoints = maxnamepoints;
 
 	Tp.Disk2Rate.Name = Bnr.disk_1_name;
 	Tp.Disk2Rate.MaxVal = Bnr.sys_ioratemax;
 	Tp.Disk2Rate.LineActive = Tp.Disk2Rate.HalfLine = 1;
 	Tp.Disk2Rate.MaxNameSize = maxnamesize;
 	Tp.Disk2Rate.MaxNamePoints = maxnamepoints;
 	}
 
     if (Mon.dua2)
 	{
 	Tp.Disk3.Name = Bnr.disk_2_name;
 	Tp.Disk3.LineActive = 1;
 	Tp.Disk3.MaxVal = BannerDeviceCount (2, 2);
 	Tp.Disk3.MaxNameSize = maxnamesize;
 	Tp.Disk3.MaxNamePoints = maxnamepoints;
 
 	Tp.Disk3Rate.Name = Bnr.disk_2_name;
 	Tp.Disk3Rate.MaxVal = Bnr.sys_ioratemax;
 	Tp.Disk3Rate.LineActive = Tp.Disk3Rate.HalfLine = 1;
 	Tp.Disk3Rate.MaxNameSize = maxnamesize;
 	Tp.Disk3Rate.MaxNamePoints = maxnamepoints;
 	}
 
     if (Mon.fp)
*3E
*3I
     if (Bnr.Param_Bits.rem_start_rate || Bnr.Param_Bits.rem_end_rate)
*3E
 	{
*3D
 	sprintf(membuf, "Mem (%d Mb)", (MMG$GL_MAXMEM/2048)+1);
 	tw = name_size(membuf) - 1;
 	twsize = XTextWidth(Bnr.font_ptr, membuf, 
 		tw);
 	if (twsize > maxnamepoints) 
*3E
*3I
 	int half = 0;
 	if (Bnr.Param_Bits.rem_start_rate)
*3E
 	    {
*3D
 	    maxnamesize = tw;
 	    maxnamepoints = twsize;
*3E
*3I
 	    half = 1;
 	    Tp.rem_start_rate.Name = "Rem Start Rate";
 	    Tp.rem_start_rate.LineActive = 1;
 	    Tp.rem_start_rate.MaxVal = Bnr.tp_max_rate;
 	    Tp.rem_start_rate.MaxNameSize = maxnamesize;
 	    Tp.rem_start_rate.MaxNamePoints = maxnamepoints;
*3E
 	    }
*3D
 	Tp.FreeMem.Name = &membuf;
 	Tp.FreeMem.LineActive = 1;
 	Tp.FreeMem.MaxVal = MMG$GL_MAXMEM;
 	Tp.FreeMem.MaxNameSize = maxnamesize;
 	Tp.FreeMem.MaxNamePoints = maxnamepoints;
 
 	if (Mon.mp)
*3E
*3I
 	if (Bnr.Param_Bits.rem_end_rate)
*3E
 	    {
*3D
 	    Tp.ModMem.Name = &membuf;
 	    Tp.ModMem.LineActive = 1;
 	    Tp.ModMem.HalfLine = 1;
 	    Tp.ModMem.MaxVal = SCH$GL_MFYLIM;
 	    Tp.ModMem.MaxNameSize = maxnamesize;
 	    Tp.ModMem.MaxNamePoints = maxnamepoints;
*3E
*3I
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
*3E
 	    }
 	}
 
*3D
     Tp.FreeCpu.Name = "Cpu";
     Tp.FreeCpu.LineActive = 1;
     Tp.FreeCpu.MaxNameSize = maxnamesize;
     Tp.FreeCpu.MaxNamePoints = maxnamepoints;
 
     if (Mon.pf)
*3E
*3I
 
     if (Bnr.Param_Bits.comp_rate1 || Bnr.Param_Bits.comp_rate2)
*3E
 	{
*3D
 	Tp.Pgf.Name = "Page file";
 	Tp.Pgf.LineActive = 1;
 	Tp.Pgf.MaxNameSize = maxnamesize;
 	Tp.Pgf.MaxNamePoints = maxnamepoints;
 
 	Tp.PgfRate.Name = "Page file";
 	Tp.PgfRate.LineActive = 1;
 	Tp.PgfRate.HalfLine = 1;
 	last_pfg_rate = PMS$GL_FAULTS;
 	Tp.PgfRate.MaxVal = Bnr.sys_faultratemax;
 	Tp.PgfRate.MaxNameSize = maxnamesize;
 	Tp.PgfRate.MaxNamePoints = maxnamepoints;
 	}
 
     if (Mon.pio)
*3E
*3I
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
*3E
 	{
*3D
 	Tp.PgIO.Name = "Pflt IO";
 	last_pg_io = (PMS$GL_PREADIO + PMS$GL_PWRITIO);
 	Tp.PgIO.MaxVal = Bnr.sys_ioratemax;
 	Tp.PgIO.LineActive = 1;
 	Tp.PgIO.MaxNameSize = maxnamesize;
 	Tp.PgIO.MaxNamePoints = maxnamepoints;
 
 	if (Mon.ni_io)
*3E
*3I
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
*3E
 	    {
*3D
 	    Tp.RemIO.Name = "Pflt/Rem IO";
 	    Tp.PgIO.Name = "Pflt/Rem IO";
 	    last_rem_io = REMOTE_IO_COUNT ();
 	    Tp.RemIO.MaxVal = Bnr.sys_ioratemax;
 	    Tp.RemIO.LineActive = 1;
 	    Tp.RemIO.HalfLine = 1;
 	    Tp.RemIO.MaxNameSize = maxnamesize;
 	    Tp.RemIO.MaxNamePoints = maxnamepoints;
 	    }
 	}
 
     Tp.Disk1.MaxNameSize = maxnamesize;
     Tp.Disk1.MaxNamePoints = maxnamepoints;
     Tp.Disk1.Title = &BannerVmsNodeName;
     Tp.Disk1.TitleChanged = 1;
     Tp.RemIO.LastLine = 1;
*3E
*3I
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
*6D
     Tp.total_com_rate.Title = &BannerVmsNodeName;
*6E
*6I
     if (BannerVmsDisplayName == NULL)
 	Tp.total_com_rate.Title = &BannerVmsNodeName;
     else
 	Tp.total_com_rate.Title = BannerVmsDisplayName;
*6E
     sprintf(tval2, "%d#", Bnr.tp_max_rate/4);
     sprintf(tval3, "%d#", Bnr.tp_max_rate - Bnr.tp_max_rate/4);
     sprintf(tval4, "%d#", Bnr.tp_max_rate);
     Tp.total_com_rate.tval1 = "0#";
     Tp.total_com_rate.tval2 = &tval2;
     Tp.total_com_rate.tval3 = &tval3;
     Tp.total_com_rate.tval4 = &tval4;
     Tp.total_com_rate.TitleChanged = 1;
     Tp.comp_rate4.LastLine = 1;
*3E
 
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
 
*3D
     CollectCpuData (0, 0);
 
*3E
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
*3D
 int io;
*3E
*3I
 int i;
*3E
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
*3D
  * Now collect all the statistics
  */
 
 /*
  * Collect the disk statistics
*3E
*3I
  * Now store all the changes stats
*3E
  */
*3D
 
     if (Tp.Disk1.LineActive == 1)
 	{
 	Tp.Disk1.CurVal = BannerDeviceCount (0, 0);
 	Tp.Disk1.CurVal = Tp.Disk1.MaxVal - Tp.Disk1.CurVal;
 	io = BannerDeviceCount (0, 1);
 	Tp.Disk1Rate.CurVal = (io - last_disk1_io)/tp_update_number;
 	last_disk1_io = io;
 	}    
 
     if (Tp.Disk2.LineActive == 1)
 	{
 	Tp.Disk2.CurVal = BannerDeviceCount (1, 0);
 	Tp.Disk2.CurVal = Tp.Disk2.MaxVal - Tp.Disk2.CurVal;
 	io = BannerDeviceCount (1, 1);
 	Tp.Disk2Rate.CurVal = (io - last_disk2_io)/tp_update_number;
 	last_disk2_io = io;
 	}    
 
     if (Tp.Disk3.LineActive == 1)
*3E
*3I
*5D
     if (&DDTM$AR_PERFORMANCE_CELLS == NULL)
*5E
*5I
     if (DDTM$AR_PERFORMANCE_CELLS == NULL)
*5E
 	return;
 
*5D
     Tp.onef_comp_rate.CurVal =  DDTM$AR_PERFORMANCE_CELLS.DDTM_ONE_PHASE - last_ddtm_one_phase;
     Tp.start_rate.CurVal = DDTM$AR_PERFORMANCE_CELLS.DDTM_STARTS - last_ddtm_starts ;
     Tp.prep_rate.CurVal = DDTM$AR_PERFORMANCE_CELLS.DDTM_PREPARES - last_ddtm_prepares;
     Tp.abort_rate.CurVal = DDTM$AR_PERFORMANCE_CELLS.DDTM_ABORTS - last_ddtm_aborts;
     Tp.end_rate.CurVal = DDTM$AR_PERFORMANCE_CELLS.DDTM_ENDS - last_ddtm_ends;
     Tp.rem_start_rate.CurVal = DDTM$AR_PERFORMANCE_CELLS.DDTM_BRANCHS - last_ddtm_branchs;
     Tp.rem_end_rate.CurVal = DDTM$AR_PERFORMANCE_CELLS.DDTM_ADDS - last_ddtm_adds;
*5E
*5I
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
*5E
 
     Tp.comp_rate1.CurVal = 0;
     Tp.comp_rate2.CurVal = 0;
     Tp.comp_rate3.CurVal = 0;
     Tp.comp_rate4.CurVal = 0;
*5I
 
*5E
     for (i=0;  i<=5;  i++)
*3E
 	{
*3D
 	Tp.Disk3.CurVal = BannerDeviceCount (2, 0);
 	Tp.Disk3.CurVal = Tp.Disk3.MaxVal - Tp.Disk3.CurVal;
 	io = BannerDeviceCount (2, 1);
 	Tp.Disk3Rate.CurVal = (io - last_disk3_io)/tp_update_number;
 	last_disk3_io = io;
 	}    
 /*
  * Do the CPU active since the last time.
  * Null time -> MaxVal
  * Used time -> CurVal
  *
  */
 
     CollectCpuData (&Tp.FreeCpu.CurVal, &Tp.FreeCpu.MaxVal);
 
     Tp.FreeCpu.CurVal = Tp.FreeCpu.MaxVal - Tp.FreeCpu.CurVal;
 
 /*
  * Now the memory stats
  */
 
     Tp.FreeMem.CurVal = Tp.FreeMem.MaxVal - SCH$GL_FREECNT;
     Tp.ModMem.CurVal = SCH$GL_MFYCNT;
 /*
  * Now colect the page fault rate stats, NOTE: Per second
  */
 
*3E
*2D
      sys$cmkrnl (
 	&PageFileAllocation, 
 	0);
*2E
*3D
 
     if (Bnr.sysrespgfile)
 	Tp.Pgf.CurVal = pgf_size - pgf_res_size;
     else
 	Tp.Pgf.CurVal = pgf_size - pgf_alloc_size;
 
     Tp.Pgf.MaxVal = pgf_size;
 
     io = PMS$GL_FAULTS;
     Tp.PgfRate.CurVal = (io - last_pfg_rate)/tp_update_number;
     last_pfg_rate = io;
 
 
 /*
  * Now do Page, and Remote IO rates
  */
 
     io = (PMS$GL_PREADIO + PMS$GL_PWRITIO);
     Tp.PgIO.CurVal = (io - last_pg_io)/tp_update_number;
     last_pg_io = io;
 
     io = REMOTE_IO_COUNT ();
     Tp.RemIO.CurVal = (io - last_rem_io)/tp_update_number;
     last_rem_io = io;
 
 /*
  * See if we have anything outstanding in the Ni stats
  */
 	if (OLD_MON_V_NI_SYS_MV != MON_V_NI_SYS_MV
 	||  OLD_MON_V_NI_MNTVERIP != MON_V_NI_MNTVERIP
 	||  OLD_MON_V_NI_BUSY != MON_V_NI_BUSY)
 	    {
 	    static char foo[125] = "";
 
 	    if ( MON_V_NI_SYS_MV != 0 ||
 		 MON_V_NI_MNTVERIP != 0 ||
 		 MON_V_NI_BUSY != 0)
 		 {
 		 sprintf (foo, "%s - %s", BannerVmsNodeName, "(");
 		 if (MON_V_NI_SYS_MV != 0)
 		    sprintf (foo, "%s%s", foo, "S");
 		 if (MON_V_NI_MNTVERIP != 0)
 		    sprintf (foo, "%s%s", foo, "M");
 		 if (MON_V_NI_BUSY != 0)
 		    sprintf (foo, "%s%s", foo, "P");
 		 sprintf (foo, "%s%s", foo, ")");
 		 
 		 Tp.Disk1.Title = &foo;
 		 Tp.Disk1.TitleChanged = 1;
 		 Tp.Disk1.highltitle = 1;
 		 }
 	    else
 		{
 		Tp.Disk1.Title = &BannerVmsNodeName;
 		Tp.Disk1.TitleChanged = 1;
 		Tp.Disk1.highltitle = 0;
  		}
     
 	    OLD_MON_V_NI_SYS_MV = MON_V_NI_SYS_MV;
 	    OLD_MON_V_NI_MNTVERIP = MON_V_NI_MNTVERIP;
 	    OLD_MON_V_NI_BUSY = MON_V_NI_BUSY;
 	    }
*3E
*3I
 	if (Bnr.tp_rate1_start <= i && Bnr.tp_rate1_end > i)	
 	    Tp.comp_rate1.CurVal += 
*5D
 		DDTM$AR_PERFORMANCE_CELLS.DDTM_BUCKETS[i] - 
*5E
*5I
 		DDTM$AR_PERFORMANCE_CELLS->DDTM_BUCKETS[i] - 
*5E
 		    last_ddtm_buckets[i];
 
 	if (Bnr.tp_rate2_start <= i && Bnr.tp_rate2_end > i)	
 	    Tp.comp_rate2.CurVal += 
*5D
 		DDTM$AR_PERFORMANCE_CELLS.DDTM_BUCKETS[i] - 
*5E
*5I
 		DDTM$AR_PERFORMANCE_CELLS->DDTM_BUCKETS[i] - 
*5E
 		    last_ddtm_buckets[i];
 
 	if (Bnr.tp_rate3_start <= i && Bnr.tp_rate3_end > i)	
 	    Tp.comp_rate3.CurVal += 
*5D
 		DDTM$AR_PERFORMANCE_CELLS.DDTM_BUCKETS[i] - 
*5E
*5I
 		DDTM$AR_PERFORMANCE_CELLS->DDTM_BUCKETS[i] - 
*5E
 		    last_ddtm_buckets[i];
 
 	if (Bnr.tp_rate4_start <= i)	
 	    Tp.comp_rate4.CurVal += 
*5D
 		DDTM$AR_PERFORMANCE_CELLS.DDTM_BUCKETS[i] - 
*5E
*5I
 		DDTM$AR_PERFORMANCE_CELLS->DDTM_BUCKETS[i] - 
*5E
 		    last_ddtm_buckets[i];
 	}
*5D
 
     Tp.total_com_rate.CurVal = Tp.onef_comp_rate.CurVal +
 	DDTM$AR_PERFORMANCE_CELLS.DDTM_COMMITS - last_ddtm_commits;
*5E
 	
 
*5D
     last_ddtm_starts = DDTM$AR_PERFORMANCE_CELLS.DDTM_STARTS;
     last_ddtm_prepares = DDTM$AR_PERFORMANCE_CELLS.DDTM_PREPARES;
     last_ddtm_one_phase = DDTM$AR_PERFORMANCE_CELLS.DDTM_ONE_PHASE;
     last_ddtm_commits = DDTM$AR_PERFORMANCE_CELLS.DDTM_COMMITS;
     last_ddtm_aborts = DDTM$AR_PERFORMANCE_CELLS.DDTM_ABORTS;
     last_ddtm_ends = DDTM$AR_PERFORMANCE_CELLS.DDTM_ENDS;
     last_ddtm_branchs = DDTM$AR_PERFORMANCE_CELLS.DDTM_BRANCHS;
     last_ddtm_adds = DDTM$AR_PERFORMANCE_CELLS.DDTM_ADDS;
     memcpy(&last_ddtm_buckets[0],
 	&DDTM$AR_PERFORMANCE_CELLS.DDTM_BUCKETS[0],6*sizeof(int) );
*5E
*5I
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
*5E
 
*3E
 /*
  * Now get all the display printed
  */
 
*3I
     Tp.total_com_rate.TitleChanged = 0;
 
*3E
     BannerPaintGraphics (XtDisplay(Bnr.tp_widget), XtWindow(Bnr.tp_widget), 
 	Bnr.tp_width, 
 	&Tp, 0);
 
     return;
 }
*3D
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **      CollectCpuData (Null, Used)
 **
 **  This routine collects CPU data, returns the Null time, and the Used time
 **  of all CPU's
 **
 **  FORMAL PARAMETERS:
 **
 **      See Above
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
 static void	CollectCpuData(int *Null, int *Used)
 {
 int i, j;
 
 /*
  * calculate time in each mode for each CPU
  */
     total_time = 0;
     total_null = 0;
 
     j=0;
     for (i=1;  i<=SMP$GL_ACTIVE_CPUS;  i=i*2)
 	{
 	if ((i & SMP$GL_ACTIVE_CPUS) > 0)
 	    {
 	    total_time = total_time + SMP$GL_CPU_DATA[j]->cpu$l_kernel;
 	    total_time = total_time + SMP$GL_CPU_DATA[j]->cpu$l_executive;
 	    total_time = total_time + SMP$GL_CPU_DATA[j]->cpu$l_user;
 	    total_time = total_time + SMP$GL_CPU_DATA[j]->cpu$l_super;
 	    total_time = total_time + SMP$GL_CPU_DATA[j]->cpu$l_compatibility;
 	    total_time = total_time + SMP$GL_CPU_DATA[j]->cpu$l_spinlock;
 	    total_time = total_time + SMP$GL_CPU_DATA[j]->cpu$l_nullcpu;
 	    total_null = total_null + SMP$GL_CPU_DATA[j]->cpu$l_nullcpu;
 /*
  * Interrupt time should include NULL time, so subtract NULL time 
  * from Interrupt time, if that is the case.
  */
 		if (SMP$GL_CPU_DATA[j]->cpu$l_interrupt >=
 		    SMP$GL_CPU_DATA[j]->cpu$l_nullcpu)
 		    total_time = total_time + 
 			SMP$GL_CPU_DATA[j]->cpu$l_interrupt -
 			SMP$GL_CPU_DATA[j]->cpu$l_nullcpu;
 		else
 		    total_time = total_time + 
 			SMP$GL_CPU_DATA[j]->cpu$l_interrupt;
 	    }
 	j++;
 	}
 
 
 /*
  * return the values if we want them.
  */
     if (Null != 0)
 	*Null = total_null - last_null;
     if (Used != 0)
 	*Used = total_time - last_total;
 
     last_null = total_null;
     last_total = total_time;
 
     return;
 }
*3E
