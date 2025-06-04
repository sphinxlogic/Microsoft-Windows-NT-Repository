#define module_name files_info
#define module_ident "V2.0-000"
/*******************************************************************************
 
 Copyright 1992, 1994 by Edward A. Heinrich of The LOKI Group, Inc.
 This code may be freely distributed and modified for non-commercial purposes
 as long as this copyright notice is retained.

*******************************************************************************/

/*
 * FACILITY:
 *	VMS privileged utilities
 *
 * PROGRAM:
 *	FILES_INFO.C
 *
 * AUTHOR:				CREATION DATE:
 *	Edward A. Heinrich			January 22, 1992
 *
 * BUILD INSTRUCTIONS:
 *
 *	$ cc   files_info
 *	$ mac  files_macro
 *	$ link files_info, sys$input/opt
 *	  files_macro
 *	  sys$share:vaxcrtl/share
 *	  sys$system:sys.stb/selective
 *	  sys$system:sysdef.stb/selective
 *
 * USAGE:
 *	$ set command files_info
 *	$ files_info FILENAME.EXT
 *
 * ENVIRONMENT:
 *	User and kernel mode code that acquires the FILSYS spinlock.
 *
 *
 * MODIFICATION HISTORY:
 *
 *	V2.0-000	Hunter Goatley		23-NOV-1994 21:54
 *		Modified to compile under OpenVMS AXP (using CC/STANDARD=VAXC).
 *		Someday it would be nice to ANSI-ize it....
 *
 *	V1.2		Hunter Goatley		17-FEB-1994 07:47
 *		Modified to allow the use of a foreign symbol and to
 *		automatically try to enable the required privileges.
 *
 *	V1.1-001	Edward A. Heinrich	15-Apr-1993	22:10 (Tax day!)
 *		This time *really* fix it, (and test it to insure it is fixed!)
 *		While the last edit prevented the kernel mode code from 
 *		trampling on memory, I neglected to restrict the user-mode
 *		display loop from exceeding the bounds of the KARGS list.
 *		Added some more display bits from WCB$W_ACON field. Changed 
 *		KARGS.COUNT to UNSIGNED (thanks WHG!) to fix value displayed.
 *
 *	V1.1-00		Edward A. Heinrich	06-Apr-1993	11:00
 *		Ooops!  Catch exceeding processCount limit on number of
 *		processes that are accessing a particular file.  Some
 *		corrections and additions to comments.
 *
 */
#ifdef __DECC
#pragma module module_name module_ident
#else
#module module_name module_ident
#endif

#include <dcdef.h>			/* Device classes definition file     */
#include <descrip.h>			/* VMS descriptor definitions	      */
#include <devdef.h>			/* Device characteristics	      */
#include <fab.h>			/* RMS FAB block definitions	      */
#include <jpidef.h>			/* GETJPI codes			      */
#include <nam.h>			/* RMS NAM block definitions	      */
#include <psldef.h>			/* PSL bit definitions		      */
#include <ssdef.h>			/* System completion status codes     */
#include <stsdef.h>			/* Status return codes		      */
#include <prvdef.h>			/* Privilege mask codes		      */
#include <stdio.h>
#include <lib$routines.h>
#include <str$routines.h>
#include <starlet.h>

#define	processCount 50			/* Number of processes we care about  */

#ifdef __DECC
#define GLOBALVALUE extern
#define GLOBALREF extern
#else
#define GLOBALVALUE globalvalue
#define GLOBALREF globalref
#endif

#ifdef __DECC
#pragma extern_model save
#pragma extern_model globalvalue
#endif
GLOBALVALUE WCB$M_NOTFCP;		/* Not accessed via XQP		      */
GLOBALVALUE WCB$M_CATHEDRAL;		/* Cathedral window		      */
GLOBALVALUE WCB$M_OVERDRAWN;		/* Exceeded disk quota		      */
GLOBALVALUE WCB$M_COMPLETE;		/* File completely mapped	      */
GLOBALVALUE WCB$M_READ;			/* File opened for READ access	      */
GLOBALVALUE WCB$M_WRITE;		/* File is opened for WRITE access    */
GLOBALVALUE WCB$V_SEQONLY;		/* Sequential access only	      */
GLOBALVALUE WCB$V_SPOOL;		/* SPOOL file on close		      */
GLOBALVALUE WCB$V_NOREAD;		/* No read sharing		      */
GLOBALVALUE WCB$V_NOWRITE;		/* No write sharing		      */

#ifdef __DECC
#pragma extern_model strict_refdef
#endif
GLOBALREF files_cld;			/* The CLD table		      */
#ifdef __DECC
#pragma extern_model restore
#endif

extern long LOCK_NONPAGED_CODE();
extern long cli$dcl_parse();
extern long cli$get_value();
/*
 *	Global storage shared between privileged/non-privileged code threads
 */
typedef struct wcbstr
	{
	long	pid;			/* PID of process w/ file opened      */
	long	reads;			/* Number of read operations done     */
	long	writes;			/* Number of write operations	      */
#ifdef __alpha
	unsigned long   acon;		/* Access control information	      */
#else
	unsigned short	acon;		/* Access control information	      */
#endif
	unsigned short	access;		/* Access control byte		      */
	};
struct 	{
	struct	wcbstr open [processCount];
	unsigned int	count;		/* Number of times file was opened    */
	struct	dsc$descriptor_s file;	/* Address of filename to search for  */
	struct	dsc$descriptor_s device;/* Address of device name to look on  */
	union	{
		long    fileid;		/* Overlay FID as long and 3 shorts   */
		short	fid [3];	/* File id of file interested in      */
		} id;
	unsigned int	fSize;		/* File size			      */
	unsigned int	eof;		/* End of file marker		      */
	short	acnt;			/* XQP access count		      */
	short	wcnt;			/* Write count			      */
	short	sts;			/* Copy of FCB$W_STATUS field	      */
	} kargs;			/* Allocate kernel argument list      */


main ()					/* You've gotta start somewhere ...   */
{
/*
 *	Declare externally defined routines we reference
 */
extern int  search_list ();		/* Declare forward routine to compiler*/
extern int  convert_pid ();		/* IPID to EPID conversion routine    */
extern void dummy ();			/* Last address to be locked in memory*/

long	lockCode [2];			/* Starting/ending addresses to lock  */
long	lockData [2];			/* Same thing for data structures     */
long	status;				/* Local status variable	      */
long	i,j;				/* Local loop indicies		      */
long	faoList [15];			/* SYS$FAOL item list		      */
long	userLen;			/* Returned length of userName	      */
short	lowPID;				/* Used to determine if process/VMS   */
/*
 *	Define RMS related data structures
 */
char	rss [NAM$C_MAXRSS];		/* Buffer for resultant filename      */
char	ess [NAM$C_MAXRSS];		/* Buffer for expanded filename string*/
struct	FAB	fab;			/* Allocate storage for FAB block     */
struct	NAM	nam;			/* Storage for NAM block	      */
/*
 *	Define formatting and display structures
 */
char	fileBuffer [255];		/* Space for file name storage	      */
char 	fmt [] = {"!XL !12AS   !7UL   !7UL  !AS!AS!AS!AS!AS!AS!AS!AS!AS"};
char	userName [16];			/* Username buffer		      */
struct 	dsc$descriptor_s foreignDesc = {0, DSC$K_DTYPE_T, DSC$K_CLASS_D, 0};
struct 	dsc$descriptor_s userDesc = {0, DSC$K_DTYPE_T, DSC$K_CLASS_S, 0};
struct 	dsc$descriptor_s faoFmt = {0, DSC$K_DTYPE_T, DSC$K_CLASS_S, 0};
struct 	dsc$descriptor_s fileDesc = {255, DSC$K_DTYPE_T, DSC$K_CLASS_S,
			0};	/* Descriptor for filename 	      */
$DESCRIPTOR (faoFmt1, "!/FILE: !AS");	/* FAO format string for filename     */
$DESCRIPTOR (faoFmt2, 
"Total access count of !UW, XQP access !UW, writers !UW, size !UL/!UL");
$DESCRIPTOR (p1Desc, "P1");		/* Name of required parameter	      */
$DESCRIPTOR (strRead,      "Read");	/* File opened for READ access	      */
$DESCRIPTOR (strReadC,     "Read (I/O cache)");	/* File opened for READ access	      */
$DESCRIPTOR (strWrite,     "Write");	/* File opened for WRITE access	      */
$DESCRIPTOR (strNotFCP,    ", Not FCP");/* Not accessed via XQP		      */
$DESCRIPTOR (strOverdrawn, ", Exquota");/* User has exceed disk quota	      */
$DESCRIPTOR (strComplete,  ", Complete");
$DESCRIPTOR (strCathedral, ", Cathedral");
$DESCRIPTOR (strDir,     ", Directory");/* File is a directory file	      */
$DESCRIPTOR (strMarkdel, ", Delete");	/* File is marked for delete	      */
$DESCRIPTOR (strSpool,   ", Spool");	/* File spooled upon close	      */
$DESCRIPTOR (strSeqonly, ", Sequential");
$DESCRIPTOR (strNoRead,  ", NoReadShr");/* File not opened for READ sharing   */
$DESCRIPTOR (strNoWrite, ", NoWriteShr");
					/* File not opened for WRITE sharing  */
$DESCRIPTOR (systemDesc, "VMS");	/* Opened by VMS, not a process	      */
$DESCRIPTOR (CLIverbDesc, "FILES ");	/* Verb used by CLI$DCL_PARSE	      */
struct	{				/* Define structure used for GETJPI   */
	short	len;
	short	code;			/* We obtain current username	      */
	long	*buff;			/* Address of buffer to contain it    */
	long	rlen;			/* Resultant length buffer address    */
	long	mbz;			/* Terminate the item list here	      */
	} jpiList = {12, JPI$_USERNAME, 0, 0, 0};
unsigned long int minpriv[2];

/*
 *  Init the descriptors, etc., here to keep DEC C quiet.
 */
userDesc.dsc$a_pointer = userName;
faoFmt.dsc$a_pointer = fmt;
fileDesc.dsc$a_pointer = fileBuffer;

jpiList.buff = (long *)&userName;
jpiList.rlen = (long) &userLen;

/*
 *	Using the DCL parsing routines, obtain the filename we are interested
 *	in looking for.
 */

minpriv[0] = PRV$M_CMKRNL | PRV$M_PSWAPM;

status = sys$setprv (1, &minpriv,0,0);	/* Turn on needed privileges	      */
if (!(status & STS$M_SUCCESS)) return (status);

lib$get_foreign (&foreignDesc);		/* Get foreign command line           */
str$concat (&foreignDesc, &CLIverbDesc, &foreignDesc);
status = cli$dcl_parse (&foreignDesc, &files_cld, &lib$get_input,
			&lib$get_input);
if (!(status & STS$M_SUCCESS)) return(SS$_NORMAL);

cli$get_value (&p1Desc, &fileDesc, &fileDesc);
/*
 *	Initialize the RMS FAB data structure
 */
fab		= cc$rms_fab;		/* Initialize the FAB		      */
fab.fab$l_nam	= &nam;			/* Point FAB to NAM block	      */
fab.fab$l_fop	= FAB$M_NAM;		/* Say we want to parse NAM block     */
fab.fab$b_fns	= fileDesc.dsc$w_length;/* Length of target device name	      */
fab.fab$l_fna	= fileDesc.dsc$a_pointer;/* Address of text string	      */
/*
 *	Next set up the RMS NAM structure
 */
nam		= cc$rms_nam;		/* Ditto for the NAM block	      */
nam.nam$b_ess	= NAM$C_MAXRSS;		/* Size of ESS buffer		      */
nam.nam$l_esa	= ess;			/* Resultant file name address	      */
nam.nam$l_rsa	= rss;			/* Resultant file name address	      */
nam.nam$b_rss	= NAM$C_MAXRSS;		/* Go for the full size here	      */
nam.nam$b_nop	= NAM$M_NOCONCEAL;	/* Don't conceal device name	      */
/*
 *	Now try to parse it.... (with fingers crossed)
 */
status	= sys$parse (&fab);		/* Parse the input filename 	      */
if	(!(status & STS$M_SUCCESS))	/* Check for errors on the PARSE call */
	{
	printf ("\n\7Error parsing filename");
	return sys$exit (status);	/* Log message and exit w/ status     */
	}
/*
 *	See if we have a node name in there, if we do issue error message and
 *	exit to DCL.
 */
if	(nam.nam$l_fnb & NAM$M_NODE)	/* If a node name was part of it      */
	{
	printf ("\n\7Filename cannot contain a NODENAME\n");
	return;
	}
/*
 *	Also check for wildcard characters.  We do not allow any.
 */
if	(nam.nam$l_fnb & NAM$M_WILDCARD)/* If a wildcard was part of it       */
	{
	printf ("\n\7Filename cannot contain any WILDCARDS\n");
	return;
	}
/*
 *	Next search for the file.  This is done to obtain the FID.
 */
status	= sys$search (&fab);		/* Search for the filename 	      */
if	(!(status & STS$M_SUCCESS))	/* Check for errors on the SEARCH     */
	{
	printf ("\n\7File did not exist %d", status);
	return;
	}
/*
 *	We need to lock the pages containing both code and data that will be
 *	referenced by code executing at elevated IPL and while holding the
 *	FILSYS spinlock.
 */
lockData [0] = (long) &kargs;			/* Stash address of KARGS structure   */
lockData [1] = lockData [0] + sizeof (kargs);
#if 0
lockCode [0] = search_list;		/* Also save addresses of code to     */
lockCode [1] = dummy;			/* ...be locked down in memory	      */
#endif
status = sys$lckpag (lockData, lockData, PSL$C_USER);
/*
 *	If we have success in locking the data, then try to lock the code
 */
if	(status & STS$M_SUCCESS)
	status = LOCK_NONPAGED_CODE();
if	(!(status & STS$M_SUCCESS))
	{
	printf ("\n\7Unable to lock code/data in memory");
	sys$exit (status);		/* Display the exact VMS message      */
	}
kargs.file.dsc$w_length	   = nam.nam$b_rsl;
kargs.file.dsc$a_pointer   = nam.nam$l_rsa;
kargs.device.dsc$a_pointer = nam.nam$l_dev;
kargs.device.dsc$w_length  = nam.nam$b_dev;
kargs.device.dsc$b_dtype   = DSC$K_DTYPE_T;
kargs.device.dsc$b_class   = DSC$K_CLASS_S;
kargs.id.fid [0] = nam.nam$w_fid [0];	/* Propagate the file id to the       */
kargs.id.fid [1] = nam.nam$w_fid [1];	/* ...argument list used by the	      */
kargs.id.fid [2] = nam.nam$w_fid [2];	/* ...kernel mode code		      */
status = sys$cmkrnl (search_list, 0);	/* Kernel mode to search FCB chain    */
/*
 *	We have finished w/ the elevated IPL code so we can unlock the pages
 *	of memory we locked down earlier.
 */
/* sys$ulkpag (lockCode, lockCode, PSL$C_USER); */
sys$ulkpag (lockData, lockData, PSL$C_USER);

fileDesc.dsc$w_length = 120;
sys$fao (&faoFmt1, &fileDesc, &fileDesc, &kargs.file);
lib$put_output (&fileDesc);
if	(status != SS$_NORMAL)
	printf ("is NOT opened at this time");
else	{
	fileDesc.dsc$w_length = 120;
	sys$fao (&faoFmt2, &fileDesc, &fileDesc, kargs.count, kargs.acnt,
		kargs.wcnt, kargs.eof, kargs.fSize);
	lib$put_output (&fileDesc);
	if	(kargs.count > processCount)
		{
		printf ("\n\nOnly displaying %d accesses out of of %ld total\n",
			processCount, kargs.count);
		kargs.count = processCount;
		}
	printf ("\n   PID     USERNAME      READS    WRITES   ACCESS CHARACTERISTICS");
	printf ("\n-------- ------------  --------  --------  ----------------------");

	for	(i=0; i<kargs.count; i++)
		{
		j = 5;			/* Starting item list offset	      */
		fileDesc.dsc$w_length = 120;
		lowPID = kargs.open[i].pid;
		if	(lowPID != 0)	/* If not opened by VMS		      */
			{
			convert_pid (kargs.open[i].pid, &kargs.open[i].pid);
			status	= sys$getjpiw (0, &kargs.open[i].pid, 0, 
				&jpiList, 0, 0, 0);
			}
		else	status = 0;
		if	(status & STS$M_SUCCESS)
			{
			userDesc.dsc$w_length = userLen;
			faoList [0] = kargs.open[i].pid;
			faoList [1] = (long) &userDesc;
			}
		else	{
			faoList [0] = 0;
			faoList [1] = (long) &systemDesc;
			}
		faoList [2] = kargs.open[i].reads;
		faoList [3] = kargs.open[i].writes;
		if	(kargs.open[i].access & WCB$M_WRITE)
			faoList [4] = (long) &strWrite;
		else	faoList [4] = (long) &strRead; 
		if	(kargs.open[i].access & WCB$M_CATHEDRAL)
			faoList [j++] = (long) &strCathedral;
		if	(kargs.open[i].access & WCB$M_NOTFCP)
			faoList [j++] = (long) &strNotFCP;
		if	(kargs.open[i].access & WCB$M_OVERDRAWN)
			faoList [j++] = (long) &strOverdrawn;
		if	(kargs.open[i].access & WCB$M_COMPLETE)
			faoList [j++] = (long) &strComplete;
		if	(kargs.open[i].acon & (1<<WCB$V_SEQONLY))
			faoList [j++] = (long) &strSeqonly;
		if	(kargs.open[i].acon & (1<<WCB$V_SPOOL))
			faoList [j++] = (long) &strSpool;
		if	(kargs.open[i].acon & (1<<WCB$V_NOREAD))
			faoList [j++] = (long) &strNoRead;
		if	(kargs.open[i].acon & (1<<WCB$V_NOWRITE))
			faoList [j++] = (long) &strNoWrite;

		faoList [j] = 0;	/* Terminate item list here	      */

		/* Special case: on AXP, the Virtual I/O Cache has files
		 * open for some periods of time.  Include a special check
		 * here for that case (process is VMS, reads = 0, writes is
		 * non-zero, and access is READ.
		 */
		if ((lowPID == 0)		/* Opened by VMS */
		&& (kargs.open[i].reads == 0)	/* No reads */
		&& (kargs.open[i].writes != 0)	/* Some # of writes */
		&& !(kargs.open[i].access & WCB$M_WRITE))
			faoList[4] = (long) &strReadC;

		faoFmt.dsc$w_length = 28 + ((j-5)*3);
					/* Compute length of item list	      */
		sys$faol (&faoFmt, &fileDesc, &fileDesc, faoList);
		lib$put_output (&fileDesc);
		}
	}
}

/*
 * ROUTINE:	SEARCH_LIST
 *
 * FUNCTIONAL DESCRIPTION:
 *	Subroutine to traverse FCB list looking for a matching FID.
 *
 * INPUTS:
 *	None.
 *
 * IMPLICIT INPUTS:
 *	KARGS structure
 *
 * ENVIRONMENT:
 *	Kernel mode code that acquires the FILSYS spinlock.
 *
 */
int search_list ()
{

extern	int find_device_ucb ();		/* Find UCB associated w/ device      */
extern 	int lock_filsys ();		/* Acquire FILSYS spinlock	      */
extern 	int unlock_filsys ();		/* Release FILSYS spinlock	      */

#ifdef __DECC
#pragma extern_model save
#pragma extern_model strict_refdef
#endif
GLOBALREF long EXE$GL_SYSUCB;		/* Address of SYS$SYSDEVICE UCB	      */

#ifdef __DECC
#pragma extern_model globalvalue
#endif
GLOBALVALUE UCB$L_VCB;			/* Obtain Volume Control Block offset */
GLOBALVALUE VCB$L_FCBFL;		/* VCB FCB listhead offset	      */
GLOBALVALUE VCB$L_FCBBL;		/* VCB FCB listtail offset	      */
GLOBALVALUE FCB$L_FCBFL;		/* FCB forward queue pointer	      */
GLOBALVALUE FCB$L_WLFL;			/* FCB WCB queue forward pointer      */
GLOBALVALUE FCB$L_FILESIZE;		/* Size of the file		      */
GLOBALVALUE FCB$L_EFBLK;		/* End of file marker 		      */
GLOBALVALUE FCB$W_SEGN;			/* FCB file segment number	      */
GLOBALVALUE FCB$W_FID_NUM;		/* FCB file number		      */
GLOBALVALUE FCB$W_FID_SEQ;		/* FCB file sequence number	      */
GLOBALVALUE FCB$W_FID_RVN;		/* FCB relative volume number	      */
GLOBALVALUE WCB$L_PID;			/* Process ID with file open	      */
GLOBALVALUE WCB$L_READS;		/* Number of reads completed 	      */
GLOBALVALUE WCB$L_WRITES;		/* Number of writes completed	      */
GLOBALVALUE WCB$B_ACCESS;		/* Access bits			      */
GLOBALVALUE UCB$L_DEVCHAR;		/* Device characteristics	      */

#ifdef __alpha
GLOBALVALUE FCB$L_ACNT;			/* FCB Access CouNT field	      */
GLOBALVALUE FCB$L_REFCNT;		/* FCB number of times file is opened */
GLOBALVALUE FCB$L_STATUS;		/* FCB status field		      */
GLOBALVALUE FCB$L_WCNT;			/* Writer count from FCB	      */
GLOBALVALUE WCB$L_ACON;
#define FCB$W_ACNT FCB$L_ACNT
#define FCB$W_REFCNT FCB$L_REFCNT
#define FCB$W_STATUS FCB$L_STATUS
#define FCB$W_WCNT FCB$L_WCNT
#define WCB$W_ACON WCB$L_ACON
long	*acon;				/* Address of WCB$W_ACON field	      */
#define ACON_CAST (long *)
long	*acnt;				/* Address of FCB$W_ACNT field	      */
#define ACNT_CAST (long *)
unsigned long	*count;			/* Address of FCB$W_REFCNT field      */
#define COUNT_CAST (unsigned long *)
long	*wcnt;				/* Address of FCB$W_WCNT field	      */
#define WCNT_CAST (long *)
long	*sts;				/* Address of FCB$W_STATUS field      */
#define STS_CAST (long *)
#else
GLOBALVALUE FCB$W_ACNT;			/* FCB Access CouNT field	      */
GLOBALVALUE FCB$W_REFCNT;		/* FCB number of times file is opened */
GLOBALVALUE FCB$W_STATUS;		/* FCB status field		      */
GLOBALVALUE FCB$W_WCNT;			/* Writer count from FCB	      */
GLOBALVALUE WCB$W_ACON;
short	*acon;				/* Address of WCB$W_ACON field	      */
#define ACON_CAST (short *)
short	*acnt;				/* Address of FCB$W_ACNT field	      */
#define ACNT_CAST (short *)
unsigned short	*count;			/* Address of FCB$W_REFCNT field      */
#define COUNT_CAST (unsigned short *)
short	*wcnt;				/* Address of FCB$W_WCNT field	      */
#define WCNT_CAST (short *)
short	*sts;				/* Address of FCB$W_STATUS field      */
#define STS_CAST (short *)
#endif

#ifdef __DECC
#pragma extern_model restore
#endif

long	*fcb;				/* Address of FCB queue listhead      */
long	*fcbHeader;			/* Saved copy of above address	      */
long	ucb;				/* Address of UCB for disk device     */
long	vcb;				/* Address of Volume Control Block    */
long 	*chr;				/* Address of UCB$L_DEVCHAR field     */
long	status;				/* Local status variable	      */
char	*access;			/* Address of WCB$B_ACCESS field      */
short	*seg;				/* Address of FCB$W_SEGN field	      */
long	*id;				/* Used to map FID into a longword    */
short	*fid;				/* Used to map FID into a word	      */
long	*size;				/* Address of FCB$L_FILESIZE field    */
long	*eof;				/* End of file mark, FCB$L_EFBLK      */
long	*wcbList;			/* Address of 1st WCB queued to FCB   */
long	*wcb;				/* Address of WCB entry		      */
long	*pid;				/* Address of WCB$L_PID field	      */
long	*reads;				/* Address of WCB$L_READS field	      */
long	*writes;			/* Address of WCB$L_WRITES field      */
long	i;				/* Index into kargs.open array	      */
/*
 *	If the user did not specify a device name in the filespec then we will
 *	search thru the FCBs hanging off the system disk UCB.  Otherwise, we
 *	call FIND_DEVICE_UCB to obtain the UCB associated w/ the device.
 */
if	(kargs.device.dsc$w_length == 0)/* If no device was specified	      */
	ucb = EXE$GL_SYSUCB;		/* Use address of system disk UCB     */
else	{				/* Else go look up UCB address	      */
	status = find_device_ucb (&kargs.device, &ucb);
	if	(!(status & STS$M_SUCCESS))
		return status;		/* Exit back to caller if errors      */
/*
 *	Make sure this is a files oriented device....
 */
	chr = (long *) (ucb + UCB$L_DEVCHAR);
	if	(!(*chr & DEV$M_FOD))	/* Is this for a FILES-oriented device*/
		return (SS$_IVDEVNAM);	/* No - exit to user mode then	      */
	}
/*
 *	We now have the UCB of the device to examine.  Acquire the FILSYS
 *	spinlock to prevent changes from hosing us.  Then obtain the VCB
 *	address from the UCB.
 */
status	= SS$_NOSUCHFILE;		/* Presume not opened by anyone       */
lock_filsys ();				/* Call Macro-32 routine for spinlock */
vcb = ucb + UCB$L_VCB;			/* Obtain Volume Control Block address*/
fcbHeader = (long *) (vcb + VCB$L_FCBFL);	/* Address of FCB listhead	      */
if	(*fcbHeader == vcb+ VCB$L_FCBBL)/* If there are NO opened files       */
	{				/* ... on this device  - unlikely?    */
	unlock_filsys ();		/* Then relinquish FILSYS spinlock    */
	return status;			/* And return to caller		      */
	}
fcb  = (long *) *fcbHeader;		/* Get address of 1st FCB on VCB queue*/
fcbHeader = fcb;			/* Address of 1st entry on list	      */
while (1)				/* Loop until we get back to head of Q*/
     {
#ifdef __alpha
     count = (unsigned long *) ((char *)fcb + FCB$W_REFCNT);/* Get REFERENCE count from FCB	      */
#else
     count = (unsigned short *) ((char *)fcb + FCB$W_REFCNT);/* Get REFERENCE count from FCB	      */
#endif
     if	   (*count != 0)		/* Is this FCB in use?		      */
	   {
/*
 *	File ids, FIDs, consist of six bytes, three contiguous 2 byte entities.
 *	We take advantage of that and map the first two entries as a longword
 *	to make the comparisons faster.
 */
	   id = (long *) ((char *) fcb + FCB$W_FID_NUM);
	   if    (*id == kargs.id.fileid)
		 {
		 fid = (short *) ((char *) fcb + FCB$W_FID_RVN);
/*
 *	If we found the desired file is opened, then collect some information
 *	on it.  Obtain from the FCB: Sizes, Active Count, Write Count, & Status.
 */
		 seg = (short *) ((char *) fcb + FCB$W_SEGN);
		 if	((*fid == kargs.id.fid [2])
		    &&   (*seg == 0))
			{		/* Is this an extension FCB?	      */
			kargs.count = *count;
			size = (long *) ((char *) fcb + FCB$L_FILESIZE);		
			kargs.fSize = *size;
			eof = (long *) ((char *) fcb + FCB$L_EFBLK);
			kargs.eof = *eof;
			acnt = ACNT_CAST ((char *) fcb + FCB$W_ACNT);
			kargs.acnt = *acnt;
			wcnt = WCNT_CAST ((char *) fcb + FCB$W_WCNT);
			kargs.wcnt = *wcnt;
			seg = (short *) ((char *) fcb + FCB$W_SEGN);
			sts = STS_CAST ((char *) fcb + FCB$W_STATUS);
			kargs.sts = *sts;/* Copy status field from FCB	      */
			i = 0;		/* Initialize the process index to 0  */
			wcbList = (long *) ((char *) fcb + FCB$L_WLFL);
			wcb = (long *) *wcbList; /* Get 1st WCB entry on FCB queue     */
/*
 *	For each WCB entry queued to the FCB obtain information about the
 *	process and the way it is accessing the file.
 */
			while (1)
		 		{
				pid = (long *) ((char *) wcb + WCB$L_PID);
				kargs.open[i].pid  = *pid;
				reads = (long *) ((char *) wcb + WCB$L_READS);
				kargs.open[i].reads = *reads;
				writes = (long *) ((char *) wcb + WCB$L_WRITES);
				kargs.open[i].writes = *writes;
				access = (char *) wcb + WCB$B_ACCESS;
				kargs.open[i].access = *access;
				acon = ACON_CAST ((char *) wcb + WCB$W_ACON);
				kargs.open[i++].acon = *acon;
				wcb = (long *) *wcb;
				if	(wcb == wcbList)
					break;
				if	(i > processCount)
					break;
				}
			status = SS$_NORMAL;
			break;		/* Say found the file open & exit loop*/
			}
		 }
	   }
     fcb  = (long *) *fcb;		/* Get address of next FCB in queue   */
     if (fcb == fcbHeader)		/* Loop until we get back to head of Q*/
	   break;			/* Jump out of loop if we do	      */
     }					/* Point at NEXT FCB and loop back    */
/*
 *	When we get here we have either found the file of interest and taken
 *	an expedited exit from the loop or we have search thru all the FCBs
 *	and were unable to find the file on the FCB list.  Either way, we 
 *	will release the FILSYS spinlock and return to our caller.
 */
unlock_filsys ();			/* Relinquish FILSYS spinlock first   */
return status;				/* Then return to caller	      */
}

/*
 *	Dummy routine to make addressing the end of routine easier.
 */
void dummy ()
{
}
