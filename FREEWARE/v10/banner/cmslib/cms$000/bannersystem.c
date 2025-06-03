 #module BannerSystem "V1.0"
 
 /*
 **++
 **  FACILITY:
 **
 **      The DECwindows Banner program
 **
 **  ABSTRACT:
 **
 **      This module contains all the code concerned with the system monitor
 **  window in the banner program.
 **
 **  AUTHORS:
 **
 **      Jim Saunders
 **
 **
 **  CREATION DATE:     278th September 1989
 **
 **  MODIFICATION HISTORY:
 **--
 **/
 
*11D
 #include <stdio.h>
 #include <decw$include/DECwDwtWidgetProg.h>
*11E
*11I
 
 #include "stdio.h"
*15I
 #include "dvidef.h"
*15E
 #include "MrmAppl.h"
 
 
*11E
 #include "Banner"
 
 
 /*
 **
 **  MACRO DEFINITIONS
 **
 **/
 
*2I
 
 void BannerSYSSetup ();
 void BannerSYSRefresh ();
 void BannerSYS ();
*8I
 void PageFileAllocation ();
*8E
 static void CollectCpuData ();
*2E
*8I
*15I
 int BannerDeviceCount();
*15E
 
 extern void BANNERCMKRNLTRAP();
*8E
 /* 
  * Definitions of other static items we use throughout the banner program
  * to reduce our setup times. 
  */
 
*13I
 extern	 Son$_Blk    Son;
*13E
 extern	 Bnr$_Blk    Bnr;
 extern	 Sys$_Blk    Sys;
*2I
 extern   Mon$_Blk    Mon;
*2E
 extern	 GC	     BannerGC;
 
*2I
*7I
*17I
*19D
 extern   char	     *BannerVmsDisplayName[];
*19E
*19I
 extern   char	     *BannerVmsDisplayName;
*19E
*17E
 extern   char	     BannerVmsNodeName[];
 extern   char	     BannerVmsVersion[];
 
 globalref int MON_V_NI_MNTVERIP, MON_V_NI_BUSY, MON_V_NI_SYS_MV;
 
 static int OLD_MON_V_NI_MNTVERIP, OLD_MON_V_NI_BUSY, OLD_MON_V_NI_SYS_MV;
 
 
*7E
 static int sys_update_count, sys_update_number;
 static int sys_hwmrk_count, sys_hwmrk_number;
 static int sys_init = 0;
*14I
 static int errorcnt;
*14E
 
 static float last_total = 0;
 static float last_null = 0;
 
 static float total_time = 0;
 static float total_null = 0;
 
 static int last_pfg_rate;
*5I
 static int last_syf_rate;
 static int last_pg_io;
 static int last_rem_io;
*5E
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
 
*8I
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
*8E
 
*15I
 #define SS$_NORMAL 1
 
 typedef struct _vms_item_list {
 	short	 buffer_length;
 	short	 item_code;
 	void	*buffer;
 	int	*returned_buffer_length;
 	} VMS_ItemList;
 
 static int device_iocnt[3] = {0, 0, 0};
 static int device_rate[3] = {0, 0, 0};
 static int device_lnblnk[3] = {0, 0, 0};
 
 #define lnb_size (12 + 64)	/* Header + 64 bytes names
 				 *      ----------------------
 				 * (0)  !   forward link     !
 				 *      ----------------------
 				 * (1)  !   Device unit count!
 				 *      ----------------------
 				 * (2)  !   64-byte name     !
 				 *      !                    !
 				 *      !                    !
 				 *      !                    !
 				 *      ----------------------
 				 */
 
 static int lnb_pool[lnb_size * 100];
 
 static char dua2_string[] = "DUA2:";
 static int dua2[2] = {5, &dua2_string};
 static char dua1_string[] = "DUA1:";
 static int dua1[2] = {5, &dua1_string};
 static char dua0_string[] = "DUA0:";
 static int dua0[2] = {5, &dua0_string};
 
 
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **	Device_logicals
 **
 **      This routine expands a logical device name, into all its sub 
 **	components.
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
*16D
 static void
*16E
*16I
 void
*16E
 device_logicals()
 {
 int i, j, pos;
 char buffer[256], device[256], count[4];
 char *curpos,*seperator;
 int (*lnb)[], (*prev_lnb)[],next_lnb;
 /*
  * got through each one of the disks and see what should be active
  * and parse the disk description.
  */
 lnb = 0;
 next_lnb = &lnb_pool;
 
 i = 0;
 while (i <= 2)
     {
     /*
      * Are we doing Dua0 this time?
      */
     memset( buffer, 0, sizeof(buffer) );
 
     if (i == 0 &&
 	Mon.dua0 != 0)
 	{
 	memcpy(&buffer, Bnr.disk_0, name_size(Bnr.disk_0) );
 	}
     /*
      * Are we doing Dua1 this time?
      */
     if (i == 1 &&
 	Mon.dua1 != 0)
 	{
 	memcpy(&buffer, Bnr.disk_1, name_size(Bnr.disk_1) );
 	}
     /*
      * Are we doing Dua2 this time?
      */
     if (i == 2 &&
 	Mon.dua2 != 0)
 	{
 	memcpy(&buffer, Bnr.disk_2, name_size(Bnr.disk_2) );
 	}
     /*
      * Know search the string, if we find a '%d' then this 
      * is a list of one or more disk of the form 'XQA%d:n'
      */
     j = 0;
     if (buffer[0] != 0)
 	curpos = &buffer;
     else
 	curpos = NULL;
 
     seperator = NULL;
 
     device_lnblnk[i] = 0;    
     prev_lnb = &device_lnblnk[i];
     while (curpos != NULL)
 	{
 	char device[100];
 	int count;
 	char subbuffer[256];
 /*
  * look for a seperator ","
  */
 	seperator = loc_sep( curpos, "," );
 /*
  * now save this device
  */
 	strcpy(subbuffer, curpos);
 	strcpy(device, curpos);
 	count = 0;
 	curpos = seperator;
 
 	seperator = loc_sep (subbuffer, "%d");
 	if (seperator != NULL)
 	    {
 /*
  * this is a counted device type
  */
 	    strcpy (device, subbuffer);
 	    sprintf(device, "%s%%d", device);
 /*
  * is the ':' there ?
  */
 	    if (loc_sep(seperator, ":") != NULL)
 		seperator++;
 	    count = strtol( seperator, NULL, NULL );
 	    }	
 
 /*
  * O.K we have a string, so start there and build a new LNB for
  * this user.
  */	    
 	lnb = next_lnb;
 	(*prev_lnb)[0] = lnb;
 	next_lnb = next_lnb + lnb_size;
 	(*lnb)[0] = 0;
 	(*lnb)[1] = count;
 	memcpy(&(*lnb)[2],device,name_size(device) );
 	prev_lnb = lnb;
 	}    
     i++;
     }
 }
 
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **	loc_sep (string, seperator)
 **
 **      This locates a sub string in string.
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
 static int	
 loc_sep(str, sep)
 char *str, *sep;
 {
 int size, i;
 
     size = name_size (sep) - 1;
     
     while (strncmp(str, sep, size) != 0)
 	{
 	str++;
 	if (*(str + size) == '\0')
 	    return 0;
 	}
     for (i=0;  i<size;  i++)
 	{
 	*str = '\0';
 	str++;
 	}
     return str;
 }
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **	BannerDeviceCount (indx, flag, error)
 **
 **      This rouitne accumilates the IO counts for one of our device lists.
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
 int BannerDeviceCount(indx, type, error)
 int indx;
 int type;
 int *error;
 {
     int iocnt, total_iocnt, (*lnb)[], len;
     int errorcnt;
     VMS_ItemList itmlst[3] = {
 	4, DVI$_OPCNT, &iocnt, &len, 
 	4, DVI$_ERRCNT, &errorcnt, &len,
 	0, 0 , 0, 0}; 
 
 
     if (error != NULL)
 	*error = 0;
 /*
  * Do we want IOCNT or Freeblocks?
  */
     if (type == 1)
 	itmlst[0].item_code = DVI$_OPCNT;
     else
 	if (type == 2)
 	    itmlst[0].item_code = DVI$_MAXBLOCK;
 	else
 	    itmlst[0].item_code = DVI$_FREEBLOCKS;
 
     total_iocnt = 0;
     lnb = device_lnblnk[indx];
 /*
  * know go through all the devices listed
  */
 
     while (lnb != NULL)
 	{
 	int device[2];
 
 	if ((*lnb)[1] > 0 )
 	    {
 	    int i;
 	    for (i=0;  i<(*lnb)[1];  i++)
 		{
 		char buffer [50];
 		int status;
 
 		sprintf(buffer, &(*lnb)[2], i);
 		device[0] = name_size (buffer) - 1;
 		device[1] = &buffer;
 		status = SYS$GETDVIW (NULL, NULL, &device, &itmlst, NULL,
 		    NULL, NULL, NULL);
 		if (status == 1)
 		    {
 		    total_iocnt = total_iocnt + iocnt;
 		    if (error != NULL)
 			*error = *error + errorcnt;
 		    }
 		else
 		    if (error != NULL)
 			*error = *error + 1;
 		}
 	    }
 	else
 	    {
 	    int status;
 
 	    device[0] = name_size (&(*lnb)[2]) - 1;
 	    device[1] = &(*lnb)[2];
 	    status = SYS$GETDVIW (NULL, NULL, &device, &itmlst, NULL,
 		NULL, NULL, NULL);
 	    if (status == 1)
 		{
 		total_iocnt = total_iocnt + iocnt;
 		if (error != NULL)
 		    *error = *error + errorcnt;
 		}
 	    else
 		if (error != NULL)
 		    *error = *error + 1;
 	    }
 	lnb = (*lnb)[0];
 	}
 return total_iocnt;
 }
 
*15E
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **      BannerSYSSetup ()
 **
 **  FORMAL PARAMETERS:
 **
 **      This is the setup routine for the Banner System Window display.
 **
 **  IMPLICIT INPUTS:
 **
 **      The Sys data block
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
 void	BannerSYSSetup()
 {
     int maxnamesize, maxnamepoints;
     int tw, twsize;
*6I
     static char membuf[125];
*6E
 
*15I
     if (Bnr.disk_0 != NULL)
 	{
 	dua0[0] = name_size (Bnr.disk_0) - 1;
 	dua0[1] = Bnr.disk_0;
 	}
     else
 	{
 	Bnr.disk_0 = dua0_string;
 	Bnr.disk_0_name = dua0_string;
 	}
 
     if (Bnr.disk_1 != NULL)
 	{
 	dua1[0] = name_size (Bnr.disk_1) - 1; 
 	dua1[1] = Bnr.disk_1;
 	}
     else
 	{
 	Bnr.disk_1_name = dua1_string;
 	Bnr.disk_1 = dua1_string;
 	}
 
     if (Bnr.disk_2 != NULL)
 	{
 	dua2[0] = name_size (Bnr.disk_2) - 1;
 	dua2[1] = Bnr.disk_2;
 	}
     else
 	{
 	Bnr.disk_2_name = dua2_string;
 	Bnr.disk_2 = dua2_string;
 	}
 
 
*15E
     sys_update_count = 1;
 
     if (Bnr.hwmrk_update == 0)
 	sys_hwmrk_number = 300;
     else
 	sys_hwmrk_number = Bnr.hwmrk_update;
     sys_hwmrk_count = sys_hwmrk_number;
 
     if (Bnr.sys_update == 0)
 	sys_update_number = Bnr.cpu_update;
     else
 	sys_update_number = Bnr.sys_update;
 
 /*
  * Now setup the Sys data block.
  */
 
*15D
     BannerMonSetup ();
*15E
     CollectCpuData (0, 0);
 
*5D
     last_pfg_rate = PMS$GL_FAULTS;
*5E
*14D
     last_disk1_io = BannerDeviceCount (0, 1);
     last_disk2_io = BannerDeviceCount (1, 1);
     last_disk3_io = BannerDeviceCount (2, 1);
*14E
*14I
*15I
     device_logicals ();
 
*15E
     last_disk1_io = BannerDeviceCount (0, 1, NULL);
     last_disk2_io = BannerDeviceCount (1, 1, NULL);
     last_disk3_io = BannerDeviceCount (2, 1, NULL);
*14E
 
*7I
     OLD_MON_V_NI_SYS_MV = -1;
     OLD_MON_V_NI_MNTVERIP = -1;
     OLD_MON_V_NI_BUSY = -1;
 
*7E
     memset (&Sys, 0, sizeof(Sys));
*5D
     Sys.FreeSpf.LastLine = 1;
*5E
 
*6D
     maxnamesize = 9;
     maxnamepoints = XTextWidth(Bnr.font_ptr, "Page File", 9);
*6E
*6I
     maxnamesize = 13;
     maxnamepoints = XTextWidth(Bnr.font_ptr, "Pgf/Sysf Rate", 13);
*6E
 
*5D
     if (Bnr.disk_0 != NULL)
*5E
*5I
     if (Mon.dua0 && Bnr.disk_0 != NULL)
*5E
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
 
 
*5D
     if (Bnr.disk_1 != NULL)
*5E
*5I
     if (Mon.dua1 && Bnr.disk_1 != NULL)
*5E
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
 
 
*5D
     if (Bnr.disk_2 != NULL)
*5E
*5I
     if (Mon.dua2 && Bnr.disk_2 != NULL)
*5E
 	{
 	tw = name_size(Bnr.disk_2_name) - 1;
 	twsize = XTextWidth(Bnr.font_ptr, Bnr.disk_2_name, 
 		tw);
 	if (twsize > maxnamepoints) 
 	    {
 	    maxnamesize = tw;
 	    maxnamepoints = twsize;
 	    }
 	}
 
*5I
*6D
     Sys.Disk1.MaxNameSize = maxnamesize;
     Sys.Disk1.MaxNamePoints = maxnamepoints;
*6E
 
*5E
     if (Mon.dua0)
 	{
 	Sys.Disk1.Name = Bnr.disk_0_name;
 	Sys.Disk1.LineActive = 1;
*14D
 	Sys.Disk1.MaxVal = BannerDeviceCount (0, 2);
*14E
*14I
 	Sys.Disk1.MaxVal = BannerDeviceCount (0, 2, &errorcnt);
*14E
*5D
 	Sys.Disk1.MaxNameSize = maxnamesize;
 	Sys.Disk1.MaxNamePoints = maxnamepoints;
*5E
 
 	Sys.Disk1Rate.Name = Bnr.disk_0_name;
*3D
 	Sys.Disk1Rate.MaxVal = 50;
*3E
*3I
 	Sys.Disk1Rate.MaxVal = Bnr.sys_ioratemax;
*3E
 	Sys.Disk1Rate.LineActive = Sys.Disk1Rate.HalfLine = 1;
 	Sys.Disk1Rate.MaxNameSize = maxnamesize;
 	Sys.Disk1Rate.MaxNamePoints = maxnamepoints;
 	}
 
     if (Mon.dua1)
 	{
 	Sys.Disk2.Name = Bnr.disk_1_name;
 	Sys.Disk2.LineActive = 1;
*14D
 	Sys.Disk2.MaxVal = BannerDeviceCount (1, 2);
*14E
*14I
 	Sys.Disk2.MaxVal = BannerDeviceCount (1, 2, &errorcnt);
*14E
 	Sys.Disk2.MaxNameSize = maxnamesize;
 	Sys.Disk2.MaxNamePoints = maxnamepoints;
 
 	Sys.Disk2Rate.Name = Bnr.disk_1_name;
*3D
 	Sys.Disk2Rate.MaxVal = 50;
*3E
*3I
 	Sys.Disk2Rate.MaxVal = Bnr.sys_ioratemax;
*3E
 	Sys.Disk2Rate.LineActive = Sys.Disk2Rate.HalfLine = 1;
 	Sys.Disk2Rate.MaxNameSize = maxnamesize;
 	Sys.Disk2Rate.MaxNamePoints = maxnamepoints;
 	}
 
     if (Mon.dua2)
 	{
 	Sys.Disk3.Name = Bnr.disk_2_name;
 	Sys.Disk3.LineActive = 1;
*14D
 	Sys.Disk3.MaxVal = BannerDeviceCount (2, 2);
*14E
*14I
 	Sys.Disk3.MaxVal = BannerDeviceCount (2, 2, &errorcnt);
*14E
 	Sys.Disk3.MaxNameSize = maxnamesize;
 	Sys.Disk3.MaxNamePoints = maxnamepoints;
 
 	Sys.Disk3Rate.Name = Bnr.disk_2_name;
*3D
 	Sys.Disk3Rate.MaxVal = 50;
*3E
*3I
 	Sys.Disk3Rate.MaxVal = Bnr.sys_ioratemax;
*3E
 	Sys.Disk3Rate.LineActive = Sys.Disk3Rate.HalfLine = 1;
 	Sys.Disk3Rate.MaxNameSize = maxnamesize;
 	Sys.Disk3Rate.MaxNamePoints = maxnamepoints;
 	}
 
*5D
     Sys.FreeMem.Name = "Memory";
     Sys.FreeMem.LineActive = 1;
     Sys.FreeMem.MaxVal = MMG$GL_MAXMEM;
     Sys.FreeMem.MaxNameSize = maxnamesize;
     Sys.FreeMem.MaxNamePoints = maxnamepoints;
*5E
*5I
     if (Mon.fp)
 	{
*6D
 	Sys.FreeMem.Name = "Memory";
*6E
*6I
 	sprintf(membuf, "Mem (%d Mb)", (MMG$GL_MAXMEM/2048)+1);
 	tw = name_size(membuf) - 1;
 	twsize = XTextWidth(Bnr.font_ptr, membuf, 
 		tw);
 	if (twsize > maxnamepoints) 
 	    {
 	    maxnamesize = tw;
 	    maxnamepoints = twsize;
 	    }
 	Sys.FreeMem.Name = &membuf;
*6E
 	Sys.FreeMem.LineActive = 1;
 	Sys.FreeMem.MaxVal = MMG$GL_MAXMEM;
 	Sys.FreeMem.MaxNameSize = maxnamesize;
 	Sys.FreeMem.MaxNamePoints = maxnamepoints;
*5E
 
*5D
     Sys.ModMem.Name = "Memory";
     Sys.ModMem.LineActive = 1;
     Sys.ModMem.HalfLine = 1;
     Sys.ModMem.MaxVal = SCH$GL_MFYLIM;
     Sys.ModMem.MaxNameSize = maxnamesize;
     Sys.ModMem.MaxNamePoints = maxnamepoints;
*5E
*5I
 	if (Mon.mp)
 	    {
*6D
 	    Sys.ModMem.Name = "Memory";
*6E
*6I
 	    Sys.ModMem.Name = &membuf;
*6E
 	    Sys.ModMem.LineActive = 1;
 	    Sys.ModMem.HalfLine = 1;
 	    Sys.ModMem.MaxVal = SCH$GL_MFYLIM;
 	    Sys.ModMem.MaxNameSize = maxnamesize;
 	    Sys.ModMem.MaxNamePoints = maxnamepoints;
 	    }
 	}
*5E
 
     Sys.FreeCpu.Name = "Cpu";
     Sys.FreeCpu.LineActive = 1;
     Sys.FreeCpu.MaxNameSize = maxnamesize;
     Sys.FreeCpu.MaxNamePoints = maxnamepoints;
 
*5D
     Sys.PgfRate.Name = "Pgf Rate";
     Sys.PgfRate.LineActive = 1;
*5E
*3D
     Sys.PgfRate.MaxVal = 100;
*3E
*3I
*5D
     Sys.PgfRate.MaxVal = Bnr.sys_faultratemax;
*5E
*3E
*5D
     Sys.PgfRate.MaxNameSize = maxnamesize;
     Sys.PgfRate.MaxNamePoints = maxnamepoints;
 
     Sys.FreePgf.Name = "P/S File";
     Sys.FreePgf.LineActive = 1;
     Sys.FreePgf.MaxNameSize = maxnamesize;
     Sys.FreePgf.MaxNamePoints = maxnamepoints;
 
     Sys.FreeSpf.Name = "P/S File";
     Sys.FreeSpf.LineActive = 1;
     Sys.FreeSpf.HalfLine = 1;
*5E
*4I
*5D
     Sys.FreeSpf.LastLine = 1;
*5E
*4E
*5D
     Sys.FreeSpf.MaxNameSize = maxnamesize;
     Sys.FreeSpf.MaxNamePoints = maxnamepoints;
*5E
*5I
     if (Mon.pf)
 	{
*6D
 	Sys.PgfRate.Name = "P/S Rate";
*6E
*6I
*8D
 	Sys.PgfRate.Name = "Pflt Rate";
*8E
*6E
*8D
 	last_pfg_rate = PMS$GL_FAULTS;
*8E
*8I
 	Sys.Pgf.Name = "Page file";
 	Sys.Pgf.LineActive = 1;
 	Sys.Pgf.MaxNameSize = maxnamesize;
 	Sys.Pgf.MaxNamePoints = maxnamepoints;
 
 	Sys.PgfRate.Name = "Page file";
*8E
 	Sys.PgfRate.LineActive = 1;
*8I
 	Sys.PgfRate.HalfLine = 1;
 	last_pfg_rate = PMS$GL_FAULTS;
*8E
 	Sys.PgfRate.MaxVal = Bnr.sys_faultratemax;
 	Sys.PgfRate.MaxNameSize = maxnamesize;
 	Sys.PgfRate.MaxNamePoints = maxnamepoints;
*8D
 
 	if (Mon.sysflt)
 	    {
*8E
*6D
 	    Sys.SyfRate.Name = "P/S Rate";
*6E
*6I
*8D
 	    Sys.SyfRate.Name = "Pgf/Sysf Rate";
 	    Sys.PgfRate.Name = "Pgf/Sysf Rate";
*8E
*6E
*8D
 	    Sys.SyfRate.LineActive = 1;
 	    Sys.SyfRate.HalfLine = 1;
 	    last_syf_rate = BANNER_KERNEL_SYSFLTS ();
 	    Sys.SyfRate.MaxVal = Bnr.sys_faultratemax;
 	    Sys.SyfRate.MaxNameSize = maxnamesize;
 	    Sys.SyfRate.MaxNamePoints = maxnamepoints;
 	    }
*8E
 	}
 
     if (Mon.pio)
 	{
*6D
 	Sys.PgIO.Name = "P/R IO";
*6E
*6I
 	Sys.PgIO.Name = "Pflt IO";
*6E
 	last_pg_io = (PMS$GL_PREADIO + PMS$GL_PWRITIO);
 	Sys.PgIO.MaxVal = Bnr.sys_ioratemax;
 	Sys.PgIO.LineActive = 1;
 	Sys.PgIO.MaxNameSize = maxnamesize;
 	Sys.PgIO.MaxNamePoints = maxnamepoints;
 
 	if (Mon.ni_io)
 	    {
*6D
 	    Sys.RemIO.Name = "P/R IO";
*6E
*6I
 	    Sys.RemIO.Name = "Pflt/Rem IO";
 	    Sys.PgIO.Name = "Pflt/Rem IO";
*6E
 	    last_rem_io = REMOTE_IO_COUNT ();
 	    Sys.RemIO.MaxVal = Bnr.sys_ioratemax;
 	    Sys.RemIO.LineActive = 1;
 	    Sys.RemIO.HalfLine = 1;
 	    Sys.RemIO.MaxNameSize = maxnamesize;
 	    Sys.RemIO.MaxNamePoints = maxnamepoints;
 	    }
 	}
 
*6I
     Sys.Disk1.MaxNameSize = maxnamesize;
     Sys.Disk1.MaxNamePoints = maxnamepoints;
*6E
*7I
*17D
     Sys.Disk1.Title = &BannerVmsNodeName;
*17E
*17I
     if (BannerVmsDisplayName == NULL)
 	Sys.Disk1.Title = &BannerVmsNodeName;
     else
*18D
 	Sys.Disk1.Title = &BannerVmsDisplayName;
*18E
*18I
 	Sys.Disk1.Title = BannerVmsDisplayName;
*18E
*17E
*12I
     Sys.Disk1.tval1 = "0%";
     Sys.Disk1.tval2 = "25%";
     Sys.Disk1.tval3 = "75%";
     Sys.Disk1.tval4 = "100%";
*12E
     Sys.Disk1.TitleChanged = 1;
*7E
     Sys.RemIO.LastLine = 1;
*5E
 
     sys_init = 1;
 
     return;
 }
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **      BannerSYSRefresh ()
 **
 **  FORMAL PARAMETERS:
 **
 **      This is the refresh routine for the Banner System Window display.
 **
 **  IMPLICIT INPUTS:
 **
 **      The Sys data block
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
 void	BannerSYSRefresh()
 {
 
     if (!sys_init)
 	BannerSYSSetup ();
 
     CollectCpuData (0, 0);
 
     BannerPaintGraphics (XtDisplay(Bnr.sys_widget), XtWindow(Bnr.sys_widget), 
 	Bnr.sys_width, 
 	&Sys, 1);
 
     return;
 }
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **      BannerSYS ()
 **
 **  FORMAL PARAMETERS:
 **
 **	This is the main routine for the Banner system window display, it 
 **  gathers all the statistics, and then calls the display routine, to 
 **  display the lines needed.
 **
 **  IMPLICIT INPUTS:
 **
 **      The Sys data block
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
 void	BannerSYS()
 {
 int io;
 /*
  * If we have never inited then do it.
  */
     if (!sys_init)
 	BannerSYSRefresh ();
 /*
  * Now check if High water marks should be reset
  */
     sys_hwmrk_count--;
     if (sys_hwmrk_count == 0)
 	{
 	Display$_Line *Line;
 
 /*
  * reset all high water marks
  */
 	sys_hwmrk_count = sys_hwmrk_number;
 
 	Line = &Sys;
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
     sys_update_count --;
     if (sys_update_count > 0)
 	return;
 	
     sys_update_count = sys_update_number;
 
 /*
  * Now collect all the statistics
  */
 
 /*
  * Collect the disk statistics
  */
 
     if (Sys.Disk1.LineActive == 1)
 	{
*14D
 	Sys.Disk1.CurVal = BannerDeviceCount (0, 0);
*14E
*14I
 	Sys.Disk1.CurVal = BannerDeviceCount (0, 0, &errorcnt);
*14E
 	Sys.Disk1.CurVal = Sys.Disk1.MaxVal - Sys.Disk1.CurVal;
*14D
 	io = BannerDeviceCount (0, 1);
*14E
*14I
 	io = BannerDeviceCount (0, 1, &errorcnt);
*14E
 	Sys.Disk1Rate.CurVal = (io - last_disk1_io)/sys_update_number;
 	last_disk1_io = io;
*3D
 	if (Sys.Disk1Rate.CurVal > Sys.Disk1Rate.MaxVal)
 	    Sys.Disk1Rate.MaxVal = Sys.Disk1Rate.CurVal;
*3E
*13I
 	Sys.Disk1.Sound = &Son.Disk1;
*13E
*14I
 	if ((errorcnt > 0 && !Sys.Disk1.NameChanged) ||
 	    (errorcnt == 0 && Sys.Disk1.NameChanged))	    
 	    Sys.Disk1.NameChanged = 1;
 	Sys.Disk1.highlname = (errorcnt > 0); 
*14E
 	}    
 
     if (Sys.Disk2.LineActive == 1)
 	{
*14D
 	Sys.Disk2.CurVal = BannerDeviceCount (1, 0);
*14E
*14I
 	Sys.Disk2.CurVal = BannerDeviceCount (1, 0, &errorcnt);
*14E
 	Sys.Disk2.CurVal = Sys.Disk2.MaxVal - Sys.Disk2.CurVal;
*14D
 	io = BannerDeviceCount (1, 1);
*14E
*14I
 	io = BannerDeviceCount (1, 1, &errorcnt);
*14E
 	Sys.Disk2Rate.CurVal = (io - last_disk2_io)/sys_update_number;
 	last_disk2_io = io;
*3D
 	if (Sys.Disk2Rate.CurVal > Sys.Disk2Rate.MaxVal)
 	    Sys.Disk2Rate.MaxVal = Sys.Disk2Rate.CurVal;
*3E
*13I
 	Sys.Disk2.Sound = &Son.Disk2;
*13E
*14I
 	if ((errorcnt > 0 && !Sys.Disk2.NameChanged) ||
 	    (errorcnt == 0 && Sys.Disk2.NameChanged))	    
 	    Sys.Disk2.NameChanged = 1;
 	Sys.Disk2.highlname = (errorcnt > 0); 
*14E
 	}    
 
     if (Sys.Disk3.LineActive == 1)
 	{
*14D
 	Sys.Disk3.CurVal = BannerDeviceCount (2, 0);
*14E
*14I
 	Sys.Disk3.CurVal = BannerDeviceCount (2, 0, &errorcnt);
*14E
 	Sys.Disk3.CurVal = Sys.Disk3.MaxVal - Sys.Disk3.CurVal;
*14D
 	io = BannerDeviceCount (2, 1);
*14E
*14I
 	io = BannerDeviceCount (2, 1, &errorcnt);
*14E
 	Sys.Disk3Rate.CurVal = (io - last_disk3_io)/sys_update_number;
 	last_disk3_io = io;
*3D
 	if (Sys.Disk3Rate.CurVal > Sys.Disk3Rate.MaxVal)
 	    Sys.Disk3Rate.MaxVal = Sys.Disk3Rate.CurVal;
*3E
*13I
 	Sys.Disk3.Sound = &Son.Disk3;
*13E
*14I
 	if ((errorcnt > 0 && !Sys.Disk3.NameChanged) ||
 	    (errorcnt == 0 && Sys.Disk3.NameChanged))	    
 	    Sys.Disk3.NameChanged = 1;
 	Sys.Disk3.highlname = (errorcnt > 0); 
*14E
 	}    
 /*
  * Do the CPU active since the last time.
  * Null time -> MaxVal
  * Used time -> CurVal
  *
  */
 
     CollectCpuData (&Sys.FreeCpu.CurVal, &Sys.FreeCpu.MaxVal);
 
     Sys.FreeCpu.CurVal = Sys.FreeCpu.MaxVal - Sys.FreeCpu.CurVal;
 
 /*
  * Now the memory stats
  */
 
     Sys.FreeMem.CurVal = Sys.FreeMem.MaxVal - SCH$GL_FREECNT;
*13I
     Sys.FreeMem.Sound = &Son.Memory;
*13E
     Sys.ModMem.CurVal = SCH$GL_MFYCNT;
 /*
  * Now colect the page fault rate stats, NOTE: Per second
*8D
  *
  * If we exceed our current Max Rate then up the Max Rate.
*8E
  */
 
*5D
     Sys.PgfRate.CurVal = (PMS$GL_FAULTS - last_pfg_rate)/sys_update_number;
     last_pfg_rate = PMS$GL_FAULTS;
*5E
*5I
*8I
      sys$cmkrnl (
 	&PageFileAllocation, 
 	0);
 
*9D
     Sys.Pgf.CurVal = pgf_size - pgf_alloc_size;
*9E
*9I
     if (Bnr.sysrespgfile)
 	Sys.Pgf.CurVal = pgf_size - pgf_res_size;
     else
 	Sys.Pgf.CurVal = pgf_size - pgf_alloc_size;
 
*9E
     Sys.Pgf.MaxVal = pgf_size;
*13I
     Sys.Pgf.Sound = &Son.PageFile;
*13E
 
*8E
     io = PMS$GL_FAULTS;
     Sys.PgfRate.CurVal = (io - last_pfg_rate)/sys_update_number;
     last_pfg_rate = io;
 
*8D
     io = BANNER_KERNEL_SYSFLTS ();
     Sys.SyfRate.CurVal = (io - last_syf_rate)/sys_update_number;
     last_syf_rate = io;
*8E
*8I
 
*8E
 /*
  * Now do Page, and Remote IO rates
  */
 
     io = (PMS$GL_PREADIO + PMS$GL_PWRITIO);
     Sys.PgIO.CurVal = (io - last_pg_io)/sys_update_number;
     last_pg_io = io;
 
     io = REMOTE_IO_COUNT ();
     Sys.RemIO.CurVal = (io - last_rem_io)/sys_update_number;
     last_rem_io = io;
*5E
*3D
     if (Sys.PgfRate.CurVal > Sys.PgfRate.MaxVal)
 	Sys.PgfRate.MaxVal = Sys.PgfRate.CurVal;
*3E
*7I
 
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
*13I
 		 BannerSound (&Son.PendingIO, Son.Volume, Son.Time);		 
*13E
*17D
 		 sprintf (foo, "%s - %s", BannerVmsNodeName, "(");
 		 if (MON_V_NI_SYS_MV != 0)
 		    sprintf (foo, "%s%s", foo, "S");
 		 if (MON_V_NI_MNTVERIP != 0)
 		    sprintf (foo, "%s%s", foo, "M");
 		 if (MON_V_NI_BUSY != 0)
 		    sprintf (foo, "%s%s", foo, "P");
 		 sprintf (foo, "%s%s", foo, ")");
 		 
 		 Sys.Disk1.Title = &foo;
 		 Sys.Disk1.TitleChanged = 1;
 		 Sys.Disk1.highltitle = 1;
*17E
*17I
 		 if (BannerVmsDisplayName == NULL)
 		    {
 		     sprintf (foo, "%s - %s", BannerVmsNodeName, "(");
 		     if (MON_V_NI_SYS_MV != 0)
 			sprintf (foo, "%s%s", foo, "S");
 		     if (MON_V_NI_MNTVERIP != 0)
 			sprintf (foo, "%s%s", foo, "M");
 		     if (MON_V_NI_BUSY != 0)
 			sprintf (foo, "%s%s", foo, "P");
 		     sprintf (foo, "%s%s", foo, ")");
 		     
 		     Sys.Disk1.Title = &foo;
 		     Sys.Disk1.TitleChanged = 1;
 		     Sys.Disk1.highltitle = 1;
 		    }
*17E
 		 }
 	    else
 		{
*17D
 		Sys.Disk1.Title = &BannerVmsNodeName;
*17E
*17I
 		if (BannerVmsDisplayName == NULL)
 		    Sys.Disk1.Title = &BannerVmsNodeName;
 		else
*18D
 		    Sys.Disk1.Title = &BannerVmsDisplayName;
*18E
*18I
 		    Sys.Disk1.Title = BannerVmsDisplayName;
*18E
*17E
 		Sys.Disk1.TitleChanged = 1;
 		Sys.Disk1.highltitle = 0;
  		}
     
 	    OLD_MON_V_NI_SYS_MV = MON_V_NI_SYS_MV;
 	    OLD_MON_V_NI_MNTVERIP = MON_V_NI_MNTVERIP;
 	    OLD_MON_V_NI_BUSY = MON_V_NI_BUSY;
 	    }
*7E
 /*
  * Now get all the display printed
  */
 
     BannerPaintGraphics (XtDisplay(Bnr.sys_widget), XtWindow(Bnr.sys_widget), 
 	Bnr.sys_width, 
 	&Sys, 0);
 
     return;
 }
 
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
*2E
*8I
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **      PageFileAllocation ();
 **
 **  This is a KRNL mode routine which examines the pagefile and swapfile
 **  data structures, and accumilates the information about allocation, and 
 **  free space.
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
 **      pgf_* and swp_* counts.
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
 void	PageFileAllocation()
 {
 pagefile$data	*ps_file;
 int		i;    
 /*
  * Init the counts to 0
  */
     pgf_size = pgf_alloc_size = pgf_res_size = 0;
     swp_size = swp_alloc_size = swp_res_size = 0;
 
 /*
  * First do the swap files
  */
 
     for (i=1;  i<=SGN$GW_SWPFILES;  i++)
 	{
 	ps_file = (*MMG$GL_PAGSWPVC)[i];
 	if (ps_file != 	(*MMG$GL_PAGSWPVC)[0])
 	    {
 /*	    swp_size = swp_size + ps_file->size; */
 	    swp_size = swp_size + (ps_file->bmap_size)*8; 
 	    swp_alloc_size = swp_alloc_size + ps_file->alloc_size; 
 	    swp_res_size = swp_res_size + ps_file->res_size;	    
 	    }
 	}
 
 /*
  * Now do the page files
  */
     for (i=1 + SGN$GW_SWPFILES;  i<=SGN$GW_SWPFILES + SGN$GW_PAGFILCT;  i++)
 	{
 	ps_file = (*MMG$GL_PAGSWPVC)[i];
 	if (ps_file != 	(*MMG$GL_PAGSWPVC)[0])
 	    {
 /*	    pgf_size = pgf_size + ps_file->size; */
 	    pgf_size = pgf_size + (ps_file->bmap_size)*8; 
 	    pgf_alloc_size = pgf_alloc_size + ps_file->alloc_size; 
 	    pgf_res_size = pgf_res_size + ps_file->res_size;	    
 	    }
 	}
     
 }
*8E
