/*
**  Copyright © 1992, 1994 by MadGoat Software.  This code may be freely
**  distributed and modified for non-commercial purposes as long as this
**  copyright notice is retained.
*/
#define module_name FLIST
#define module_vers "2.2-1"
/*
**  Program:	FLIST
**
**  Author:	Hunter Goatley
**		VMS Systems Programmer
**		Western Kentucky University
**		Bowling Green, KY 42101
**		E-mail: goathunter@WKUVX1.WKU.EDU
**
**  Date:	November 18, 1992
**
**  Abstract:	Main routine for FLIST, which calls TPU.  This module
**		handles the FLIST callbacks.
**
**  Modified by:
**
**	2.2-1		Hunter Goatley		24-MAR-1994 10:14
**		It took this long for someone to find the first bug.
**		Fix translate_logical_name accvio bug.
**
**	2.2		Hunter Goatley		18-NOV-1992 22:06
**		C rewrite of original MACRO-32 program.
**
*/

#ifdef __alpha
#pragma module module_name module_vers
#else
#module module_name module_vers
#endif

/*
**  The headers....
*/
#include <atrdef.h>
#include <descrip.h>
#include <fibdef.h>
#include <iodef.h>
#include <lnmdef.h>
#include <rms.h>
#include <sbkdef.h>
#include <ssdef.h>
#include <stdio.h>
#include <string.h>

#ifdef __alpha
#include "fchdef_alpha.h"	/* Not provided with DEC C */
#else
#include <fchdef.h>		/* Provided with VAX C v3.2 */
#endif /* if __alpha */

#include "fatdef.h"		/* Not provided at all */

#define DDESC	struct dsc$descriptor_d
#define SDESC	struct dsc$descriptor_s
#define errret(s)	if (!(s & 1)) return (s);


#define BIG_COPY_BUFFER_SIZE 127*512	/* Number of bytes to READ/WRITE */

/*
 *  Under Alpha, the FIB unions are declared as variant_unions.
 *  FIBDEF.H includes the definition of __union, which we check
 *  below to make sure we access the structure correctly.
 */
#if defined(__union) && (__union == variant_union)
#define FIB$W_FID	fib$w_fid
#else
#define FIB$W_FID	fib$r_fid_overlay.fib$w_fid
#endif  /* #if __union == variant_union */

/*
**  EXTERNAL ROUTINE
*/
int	TPU$CLEANUP();
int	TPU$CONTROL();
int	TPU$EXECUTE_INIFILE();
int	TPU$FILEIO();
int	TPU$HANDLER();
int	TPU$INITIALIZE();

globalvalue
	TPU$_FILEIO,
	TPU$_CALLUSER,
	TPU$_OPTIONS,
	TPU$_SECTIONFILE,
	TPU$_FILENAME;

globalvalue
	TPU$M_DELETE_CONTEXT,
	TPU$M_DISPLAY,
	TPU$M_SECTION;

/*  A couple of structure definitions */
struct bpv {				/* Bound procedure value */
   int *routine_addr;
   long int env;
};

struct itmlst_entry			/* An itemlist entry */
{ short buflen;
  short itmcod;
  int *bufadr;
  int *retlen;
};


/* Global variables to make life easier */

DDESC for_buff;					/* Command line buffer*/
static retmsg_buff[256];			/* Buffer for return msgs */
static SDESC retmsg =				/* Descriptor for return msgs */
	{sizeof(retmsg_buff), DSC$K_DTYPE_T,
	 DSC$K_CLASS_S, retmsg_buff};

/*
**  Variables used by search_for_files()
*/
static struct FAB search_fab;
static struct NAM search_nam;
static char search_file_spec[NAM$C_MAXRSS];		
static char search_file_expanded[NAM$C_MAXRSS];		
static char search_file_resultant[NAM$C_MAXRSS];		

/*
**  Variables used by parse_file()
*/
static struct FAB parse_fab;
static struct NAM parse_nam;

/*
**  Variables used by copy_file()
*/
static struct FAB copy_infab;
static struct FAB copy_outfab;
static struct RAB copy_inrab;
static struct RAB copy_outrab;
static struct NAM copy_nam;
static char copy_resultant[NAM$C_MAXRSS];		

static $DESCRIPTOR (job_lnmtable, "LNM$JOB");


/* This one is used by search_for_files() and check_for_dir(), so it's here */
static char atr_uchar[ATR$S_UCHAR];	/* File characteristics mask */

/*
** FORWARD ROUTINE
*/
void	initialize_globals	(void);
struct itmlst_entry *initialize_callback(void);
int	flist_calluser		(int *itmcod, DDESC *inpstr, DDESC *retstr);
int	delete_file	(DDESC *filespec);
int	rename_file	(DDESC *filespec);
int	copy_file		(DDESC *instr);
int	parse_file		(DDESC *instr);
int	search_for_files	(DDESC *instr);
int	define_job_logical	(DDESC *instr);
int	delete_job_logical	(DDESC *logical_name);
int	translate_logical_name	(DDESC *logical_name);
int	check_for_dir		(void);
int	format_return_msg	(int istat, DDESC *retstr);


/*
**  main()
**
*/
int main(void)
{
    struct bpv callback_bpv;		/* Bound procedure value for callback */
    int control_options = 1;		/* Prevent "journaling" message */
    int cleanup_flag = TPU$M_DELETE_CONTEXT;
    unsigned long int status;

    /*
    **  Initialize all the global FABs and RABs, etc.
    */
    initialize_globals();

    /*
    **  Get the foreign command (a file spec may be given on the command line.
    */
    status = LIB$GET_FOREIGN (&for_buff, 0, &for_buff);

    /*
    **  Set up TPU's error handler as our error handler.
    */
    LIB$ESTABLISH (TPU$HANDLER);

    /*
    **  Initialize the bound procedure value for the callback routine.
    */
    callback_bpv.routine_addr = initialize_callback;
    callback_bpv.env  = 0;

    /*
    **  Call the TPU initialization routine.
    */
    status = TPU$INITIALIZE (&callback_bpv);
    errret(status);

    /*
    **  Let TPU execute the initialization file
    */
    status = TPU$EXECUTE_INIFILE();
    errret(status);

    /*
    **  Turn control over to TPU (and don't print "journaling" message).
    */
    status = TPU$CONTROL (&control_options);
    errret(status);

    /*
    **  We're back....  Tell TPU to clean up after itself.
    */
    TPU$CLEANUP(&cleanup_flag);

    return (status);		/* Need to clear INHIB_MSG bit */
}



void initialize_globals(void)
/*
**  Function:	initialize_globals
**
**  Functional description:
**
**	This function initializes all of the global structures (the
**	FABs, RABs, and NAM blocks used by more than one routine).
**
**  Formal parameters:
**
**	None.
**
**  Implicit inputs:
**
**	search_fab, search_nam, parse_fab, parse_nam, copy_infab, copy_outfab,
**	copy_inrab, copy_outrab, copy_nam, for_buff
**
**  Returns:
**
**	Nothing.
**
*/
{
    /* Initialize global structures */
    search_fab = cc$rms_fab;			/* Initialize the search FAB */
    search_fab.fab$l_fop = FAB$M_NAM;
    search_fab.fab$l_fna = search_file_spec;
    search_fab.fab$l_nam = &search_nam;

    search_nam = cc$rms_nam;
    search_nam.nam$l_esa = search_file_expanded;
    search_nam.nam$b_ess = NAM$C_MAXRSS;
    search_nam.nam$l_rsa = search_file_resultant;
    search_nam.nam$b_rss = NAM$C_MAXRSS;

    parse_fab = cc$rms_fab;			/* Initialize the parse FAB */
    parse_fab.fab$l_fop = FAB$M_NAM;
    parse_fab.fab$l_nam = &parse_nam;

    parse_nam = cc$rms_nam;			/* Initialize the parse NAM */
    parse_nam.nam$l_esa = retmsg.dsc$a_pointer;
    parse_nam.nam$b_ess = NAM$C_MAXRSS;
    parse_nam.nam$b_nop = NAM$M_SYNCHK;

    copy_infab = cc$rms_fab;			/* Initialize copy input FAB */
    copy_infab.fab$b_fac = FAB$M_BIO | FAB$M_GET;
    copy_infab.fab$b_shr = FAB$M_SHRGET;

    copy_inrab = cc$rms_rab;
    copy_inrab.rab$l_fab = &copy_infab;
    copy_inrab.rab$b_rac = RAB$C_SEQ;
    copy_inrab.rab$w_usz = BIG_COPY_BUFFER_SIZE;
    copy_inrab.rab$l_ubf = malloc (BIG_COPY_BUFFER_SIZE);
    if (copy_inrab.rab$l_ubf == NULL) {
	printf("Memory exhausted\n");
	SYS$EXIT(SS$_ABORT);
    }

    /*
    **  copy_outfab is initialized by copy_file().
    */
    copy_outrab = cc$rms_rab;
    copy_outrab.rab$l_fab = &copy_outfab;
    copy_outrab.rab$b_rac = RAB$C_SEQ;
    copy_outrab.rab$w_usz = BIG_COPY_BUFFER_SIZE;
    copy_outrab.rab$l_rbf = copy_inrab.rab$l_ubf;

    copy_nam = cc$rms_nam;
    copy_nam.nam$l_rsa = copy_resultant;
    copy_nam.nam$b_rss = NAM$C_MAXRSS;

    for_buff.dsc$w_length = 0;
    for_buff.dsc$b_dtype = DSC$K_DTYPE_T;
    for_buff.dsc$b_class = DSC$K_CLASS_D;
    for_buff.dsc$a_pointer = NULL;

}

struct itmlst_entry *initialize_callback (void)
/*
**  Function:	initialize_callback
**
**  Functional description:
**
**	This routine is the TPU initialization callback routine.  It is
**	called by TPU$INITIALIZE to establish TPU defaults, etc.
**
**  Formal parameters:
**
**	None.
**
**  Implicit inputs:
**
**	for_buff
**
**  Returns:
**
**	Pointer to an item list containing the TPU information.
**
*/
{
   char *section_name = "FLIST_TPU_SECTION";
   int options;
   static struct bpv fileio_bpv, calluser_bpv;
   static struct itmlst_entry init_itmlst[] =
	{ {4, 0, 0, 0},
	  {4, 0, 0, 0},
	  {4, 0, 0, 0},
	  {0, 0, 0, 0},
	  {0, 0, 0, 0},
	  {0, 0, 0, 0}};

   init_itmlst[0].itmcod = TPU$_FILEIO;
   init_itmlst[1].itmcod = TPU$_CALLUSER;
   init_itmlst[2].itmcod = TPU$_OPTIONS;
   init_itmlst[3].itmcod = TPU$_SECTIONFILE;
   init_itmlst[4].itmcod = TPU$_FILENAME;

   fileio_bpv.routine_addr = TPU$FILEIO;
   fileio_bpv.env	   = 0;

   calluser_bpv.routine_addr = flist_calluser;
   calluser_bpv.env	   = 0;

   init_itmlst[0].bufadr = &fileio_bpv;
   init_itmlst[1].bufadr = &calluser_bpv;
   options = TPU$M_SECTION | TPU$M_DISPLAY;
   init_itmlst[2].bufadr = &options;   

   init_itmlst[3].buflen = strlen(section_name);
   init_itmlst[3].bufadr = section_name;
   init_itmlst[4].buflen = for_buff.dsc$w_length;
   init_itmlst[4].bufadr = for_buff.dsc$a_pointer;

   return(init_itmlst);

}


int flist_calluser(int *itmcod, DDESC *inpstr, DDESC *retstr)
/*
**  Function:	flist_calluser
**
**  Functional description:
**
**	This routine is called by the TPU built-in CALL_USER.  It dispatches
**	FLIST requests to the appropriate functions.
**
**  Formal parameters:
**
**	int *itmcod	- pointer to the item code
**	DDESC *inpstr	- pointer to dynamic descriptor for input string
**	DDESC *retstr	- pointer to dynamic descriptor for return string
**
**  Implicit inputs:
**
**	retmsg
**
**  Returns:
**
**	Status in R0.
**	String in retstr.
**
*/
{
   int status;
   int x;

   retmsg.dsc$w_length = 256;		/* Always reset retmsg length */

   switch(abs(*itmcod)) {		/* Use absolute value of item code */
      case 1:
	status = delete_file (inpstr);
	break;

      case 2:
	status = rename_file(inpstr);
	break;

      case 3:
	status = copy_file(inpstr);
	break;

      case 4:
/*	printf("EDT file"); */
	break;

      case 5:
	status = parse_file(inpstr);
	break;

      case 6:
	status = search_for_files(inpstr);
	break;

      case 7:
	status = define_job_logical(inpstr);
	break;

      case 8:
	status = delete_job_logical(inpstr);
	break;

      case 9:
	status = translate_logical_name(inpstr);
	break;

      case 10:
	status = check_for_dir();
	break;

      default:
	status = SS$_ABORT;
   }

   /*
   **  Now call format_return_msg() to format the string that is to
   **  be returned to TPU.  If status indicates an error occurred, the
   **  error message text is returned to TPU.
   */
   format_return_msg (status, retstr);

   return SS$_NORMAL;
}


int delete_file (DDESC *filespec)
/*
**  Function:	delete_file
**
**  Functional description:
**
**	This function deletes a file.
**
**  Formal parameters:
**
**	DDESC *filespec		- pointer to descriptor for filename to delete
**
**  Implicit inputs:
**
**	retmsg
**
**  Returns:
**
**	Status in R0.  On success, message is written to retmsg.
**
*/
{
    static $DESCRIPTOR(delete_fao, "!AS deleted");
    int status;
    status = LIB$DELETE_FILE (filespec);
    if (status & 1)
	status = SYS$FAO (&delete_fao, &retmsg, &retmsg, filespec);
    return (status);
}


int rename_file (DDESC *instr)
/*
**  Function:	rename_file
**
**  Functional description:
**
**	This function renames a file by calling LIB$RENAME_FILE.
**
**  Formal parameters:
**
**	DDESC *instr	- pointer to dynamic descriptor for input string
**
**	Because TPU only provides one input string to the call_user routine,
**	the two filenames (old and new) are stored as ASCIC strings within
**	the instr variable.
**
**  Implicit inputs:
**
**	retmsg
**
**  Returns:
**
**	Status in R0.  On success, a message is written to retmsg.
**
*/
{
    static $DESCRIPTOR(rename_fao, "!AS renamed to !AS");
    char *cptr;
    int status, x;
    static SDESC oldfile_d =
		{0, DSC$K_DTYPE_T, DSC$K_CLASS_S, 0},
		newfile_d = {0, DSC$K_DTYPE_T, DSC$K_CLASS_S, 0};
    static SDESC resfile_d =
		{0, DSC$K_DTYPE_T, DSC$K_CLASS_D, 0};

    /*
    **  Set up oldfile_d descriptor to point to first ASCIC string.
    */
    cptr = (char *) instr->dsc$a_pointer;	/* Start at beginning */
    x = (int) *cptr++;				/* Get length of 1st string */
    oldfile_d.dsc$w_length = x;			/* Set up the descriptor */
    oldfile_d.dsc$a_pointer = cptr;

    cptr += x;					/* Bump pointer past 1st str */
    x = (int) *cptr++;				/* Get length of 2nd string */
    newfile_d.dsc$w_length = x;			/* Set up the descriptor */
    newfile_d.dsc$a_pointer = cptr;

    /*
    **  Rename the file, putting the result in resfile_d.
    */
    status = LIB$RENAME_FILE (&oldfile_d, &newfile_d, &oldfile_d,
			0, 0, 0, 0, 0, 0, 0, &resfile_d, 0);

    /*
    **  If the rename was successful, return a message saying so to TPU.
    */
    if (status & 1)
	status = SYS$FAO (&rename_fao, &retmsg, &retmsg,
		&oldfile_d, &resfile_d);

    return (status);

}


int copy_file (DDESC *instr)
/*
**  Function:	copy_file
**
**  Functional description:
**
**	This function copies a file using RMS block I/O.
**
**  Formal parameters:
**
**	DDESC *instr	- pointer to dynamic descriptor for input string
**
**	Because TPU only provides one input string to the call_user routine,
**	the two filenames (old and new) are stored as ASCIC strings within
**	the instr variable.
**
**  Implicit inputs:
**
**	retmsg
**
**  Returns:
**
**	Status in R0.  On success, a message is written to retmsg.
**
*/
{
    $DESCRIPTOR (copy_fao, "!AD copied to !AD");
    char *cptr;
    int status, x;
    static SDESC oldfile_d =
		{0, DSC$K_DTYPE_T, DSC$K_CLASS_S, 0},
		newfile_d = {0, DSC$K_DTYPE_T, DSC$K_CLASS_S, 0};

    /*
    **  The file names come in from FLIST (TPU) as two ASCIC strings
    **  concatenated together to form the string described by the input
    **  descriptor.  The FABs are set to point to the actual filenames
    **  in the input string.
    */
    cptr = (char *) instr->dsc$a_pointer;	/* Point to string buffer */
    x = (int) *cptr++;				/* x = length of old file */
    copy_infab.fab$b_fns = x;			/* Set the fab fns */
    copy_infab.fab$l_fna = cptr;		/* Set the fab fna */

    status = SYS$OPEN (&copy_infab, 0, 0);	/* Try to open the file */
    errret(status);				/* Return any error     */

    status = SYS$CONNECT (&copy_inrab, 0, 0);	/* Connect the RAB to it */
    errret(status);

    /*
    **  Now that the input fab has been set, just copy it to the output
    **  fab and clear the appropriate fields.  This ensures that all
    **  FAB fields that should be copied are copied---otherwise, each
    **  field would have to be copied individually, which is much more
    **  error-prone (typos, new fields, etc.).
    */
    copy_outfab = copy_infab;			/* Initialize the output FAB */

    /*
    **  Zero and initialize the appropriate fields in the output fab.
    */
    copy_outfab.fab$b_shr = 0;
    copy_outfab.fab$w_ifi = 0;
    copy_outfab.fab$l_xab = copy_outfab.fab$l_fna =
		copy_outfab.fab$l_dna = NULL;
    copy_outfab.fab$b_fac = FAB$M_BIO | FAB$M_PUT;
    copy_outfab.fab$l_nam = &copy_nam;
	
    /*
    **  Now set the filename and default file name using the input string.
    */
    copy_outfab.fab$b_dns = x;		/* Old name is the default name */
    copy_outfab.fab$l_dna = cptr;	/* ... for the copy.		*/
    cptr += x;				/* Bump pointer to 2nd string   */
    x = (int) *cptr++;			/* Get the new filename length  */
    copy_outfab.fab$b_fns = x;		/* Save them in the FAB.	*/
    copy_outfab.fab$l_fna = cptr;

    /*
    **  Now create the output file.
    */
    status = SYS$CREATE (&copy_outfab, 0, 0);
    if (status & 1)
       status = SYS$CONNECT (&copy_outrab, 0, 0);

    if (status & 1) {
	/*
	**  Loop, reading lots of blocks from the input file and writing
	**  them to the output file.
	*/
	status = SYS$READ (&copy_inrab, 0, 0);
	while (status & 1) {
	    copy_outrab.rab$w_rsz = copy_inrab.rab$w_rsz;
	    status = SYS$WRITE (&copy_outrab, 0, 0);
	    if (status & 1)
		status = SYS$READ (&copy_inrab, 0, 0);
	}
	if (status == RMS$_EOF)		/* If end-of-file was reached,  */
	   status = SS$_NORMAL;		/* ... change error to OK.	*/
    }
    SYS$CLOSE (&copy_infab);
    SYS$CLOSE (&copy_outfab);

    /*
    **  If the copy was successful, write a message saying so to retmsg.
    */
    if (status & 1) {
	int x = copy_infab.fab$b_fns, y = copy_nam.nam$b_rsl;
	status = SYS$FAO (&copy_fao, &retmsg, &retmsg,
			x, copy_infab.fab$l_fna, y, copy_nam.nam$l_rsa);
    }

    return (status);

}


int parse_file (DDESC *instr)
/*
**  Function:	parse_file
**
**  Functional description:
**
**	This function parses a file spec, returning the file's name, type,
**	and version number.  Written to avoid multiple calls to the TPU
**	built-in file_parse().
**
**  Formal parameters:
**
**	DDESC *instr	- pointer to dynamic descriptor for filename to parse
**
**  Implicit inputs:
**
**	parse_fab, parse_nam
**
**  Returns:
**
**	Status in R0.  The parsed file name is written to retmsg.
**
*/
{
   /*
   **  Set up the parse_fab with the address and length of the file name.
   */
   parse_fab.fab$l_fna = instr->dsc$a_pointer;
   parse_fab.fab$b_fns = instr->dsc$w_length;

   /*
   **  Parse the file spec.  parse_fab points to the retmsg buffer
   */
   SYS$PARSE (&parse_fab, 0, 0);

   retmsg.dsc$w_length = parse_nam.nam$b_name +		/* Set the proper  */
			 parse_nam.nam$b_type +		/* length for retmsg */
			 parse_nam.nam$b_ver;

   return(SS$_NORMAL);

}


int search_for_files (DDESC *instr)
/*
**  Function:	search_for_files
**
**  Functional description:
**
**	This function is called repeatedly by the FLIST TPU code to
**	find all files matching the given file specification.
**
**  Formal parameters:
**
**	DDESC *instr	- pointer to descriptor for the file specification
**
**  Implicit inputs:
**
**	search_fab, search_nam, atr_uchar,
**
**  Returns:
**
**	Status in R0.  On success or "No more files", a filespec is returned
**	in retmsg.
**
*/
{
   static $DESCRIPTOR (search_fao, "!39AD !7UL/!7<!UL!> !%D");
   static long fao_prmlst[5];			/* SYS$FAOL parameter list */

   static char file_disk[NAM$C_MAXRSS];		/* Name of current disk */
   static SDESC file_disk_d = {0, DSC$K_DTYPE_T,
		DSC$K_CLASS_S, file_disk};
   static short qio_iosb[4];			/* I/O status block */
   /*
   **  Define the File Information Block to be used in the ACP $QIO calls.
   */
   static struct fibdef search_fib;		/* The FIB itself */
   static struct fib_descriptor {		/* The FIB descriptor */
	long size;
	struct fibdef *addr;
   } search_fib_d = { sizeof(search_fib), &search_fib};

   /*
   **  Define the Attribute blocks.
   */
   static struct sbkdef atr_statblk;		/* ACP QIO statistics block  */
   static char	atr_credate[ATR$S_CREDATE],	/* File creation date	     */
		atr_recattr[ATR$S_RECATTR];	/* ACP QIO record attributes */
   static struct atrdef atr_itmlst[] = {	/* ATR Item List for $QIO    */
		{ATR$S_STATBLK, ATR$C_STATBLK, (long) &atr_statblk},
		{ATR$S_CREDATE, ATR$C_CREDATE, (long) atr_credate},
		{ATR$S_RECATTR, ATR$C_RECATTR, (long) atr_recattr},
		{ATR$S_UCHAR,   ATR$C_UCHAR,   (long) atr_uchar},
		{0, 0, 0}};

   static int device_chan = 0;			/* I/O channel to disk */
   int status;
   int length;

   length = instr->dsc$w_length;		/* Get string length */
   /*
   **  If the specified file is different from the last one used, then
   **  call $PARSE again.
   */
   if ((length != search_fab.fab$b_fns) ||
      (strncmp(instr->dsc$a_pointer, search_fab.fab$l_fna, length) != 0)) {
	/*
	 *  A new search file specification has been given on this call.
	 *  Copy the file spec to our local storage (the search_fab
	 *  already points to this data area.
	 */
	search_fab.fab$b_fns = length;
	strncpy(search_fab.fab$l_fna, instr->dsc$a_pointer, length);
	status = SYS$PARSE (&search_fab, 0, 0);
	errret(status);
   }

   /*
   **  Call $SEARCH, using the passed in (or saved) file specification.
   */
   status = SYS$SEARCH (&search_fab, 0, 0);	/* Call the $SEARCH service */

   if (status == RMS$_NMF) {			/* No more files?  If so,   */
	retmsg.dsc$w_length = 0;		/* ... set the return msg   */
	return (SS$_NORMAL);			/* ... length to 0 return   */
   }

   errret(status);				/* Return any other error */

   /*
   **  Copy the file id returned in the NAM block to the File Information
   **  Block for the ACP $QIOs.
   */
   search_fib.FIB$W_FID[0] = search_nam.nam$w_fid[0];
   search_fib.FIB$W_FID[1] = search_nam.nam$w_fid[1];
   search_fib.FIB$W_FID[2] = search_nam.nam$w_fid[2];

   /*
   **  See if it's the same device.  If not, save the new device name and
   **  assign a channel to that device.
   */
   length = search_nam.nam$b_dev;
   if ((length != file_disk_d.dsc$w_length) ||
      (strncmp(search_nam.nam$l_dev, file_disk_d.dsc$a_pointer, length) != 0))
	{
	file_disk_d.dsc$w_length = length;
	strncpy(file_disk_d.dsc$a_pointer, search_nam.nam$l_dev, length);

	if (device_chan != 0)			/* If there's a chennel, */
	    SYS$DASSGN (device_chan);		/* ... get rid of it */

	/*
	**  Assign an I/O channel to the disk device.
	*/
	status = SYS$ASSIGN (&file_disk_d, &device_chan, 0, 0, 0);
	errret(status);
   } /* if ((length.... */

   /*
   **  Call $QIO to ask the ACP to return information for the file whose
   **  file ID was stored in the FIB.
   */
   status = SYS$QIOW (0, device_chan, IO$_ACCESS, &qio_iosb,
			0, 0, &search_fib_d, 0, 0, 0, atr_itmlst, 0);
   errret(status);				/* Return any error */
   if (!(qio_iosb[0]&1)) return (qio_iosb[0]);	/* Check IOSB too   */

   /*
   **  Now copy the information to the $FAOL parameter list for the
   **  call to $FAOL.
   */
   fao_prmlst[0] = search_nam.nam$b_name +	/* Length of file name */
		   search_nam.nam$b_type +
		   search_nam.nam$b_ver;
   fao_prmlst[1] = search_nam.nam$l_name;	/* Address of file name */

   /*
   **  For PDP-11 compatibility, the End-of-file block and the
   **  filesize are stored with the high- and low-words swapped. 
   **  Before they can be put in the $FAO arglst, we have to rotate
   **  them so they're in the correct order.
   **
   **  When the end-of-file position corresponds to a block
   **  boundary, by convention FAT$L_EFBLK contains the end-of-file
   **  VBN plus 1, and FAT$W_FFBYTE contains 0.
   */
   {	unsigned short int temp_blocks[2];
	unsigned short *blkptr;

	blkptr = (unsigned short *) &atr_recattr[FAT$L_EFBLK];

	temp_blocks[1] = *blkptr++;
	temp_blocks[0] = *blkptr;
	fao_prmlst[2] = *((unsigned long int *) temp_blocks);
	if ((fao_prmlst[2] != 0) && ((short) atr_recattr[FAT$W_FFBYTE] == 0))
		   fao_prmlst[2]--;

	temp_blocks[1] = atr_statblk.sbk$w_filesizh;
	temp_blocks[0] = atr_statblk.sbk$w_filesizl;
	fao_prmlst[3] = *((unsigned long int *) temp_blocks);

   }
   fao_prmlst[4] = (long) &atr_credate;		/* The file's creation date */

   /*
   **  Call $FAOL to format the string to be returned to FLIST for
   **  display in the file buffer.
   */
   status = SYS$FAOL (&search_fao, &retmsg, &retmsg, fao_prmlst);

   return (status);
}


int define_job_logical (DDESC *instr)
/*
**  Function:	define_job_logical
**
**  Functional description:
**
**	This function defines a logical in the LNM$JOB logical name table.
**
**  Formal parameters:
**
**	DDESC *instr	- pointer to dynamic descriptor for input string
**
**	Because TPU only provides one input string to the call_user routine,
**	the logical and its equivalence string are stored as ASCIC strings
**	within the instr variable.
**
**  Implicit inputs:
**
**	retmsg
**
**  Returns:
**
**	Status in R0.  On success, a message is written to retmsg.
**
*/
{
   char *logical, *equiv;
   int x, status;

   static SDESC logical_d =
		{0, DSC$K_DTYPE_T, DSC$K_CLASS_S, 0},
		equiv_d = {0, DSC$K_DTYPE_T, DSC$K_CLASS_S, 0};

   /*
   **  Set uo the descriptors to point to the two ASCIC strings.
   */
   logical = (char *) instr->dsc$a_pointer;	/* Start at beginning */
   x = (int) *logical++;			/* Length of 1st string */
   logical_d.dsc$w_length = x;			/* Set up the descriptor */
   logical_d.dsc$a_pointer = logical;

   logical += x;				/* Bump pointer to 2nd string */
   x = (int) *logical++;			/* Length of 2nd string */
   equiv_d.dsc$w_length = x;			/* Set up the descriptor */
   equiv_d.dsc$a_pointer = logical;

   /*
   **  Define the logical in supervisor mode in the LNM$JOB table.
   */
   status = LIB$SET_LOGICAL (&logical_d, &equiv_d, &job_lnmtable);

   if (status & 1)				/* If successful, return a */
      retmsg.dsc$w_length = 0;			/* ... null string to TPU  */

   return(status);

}


int delete_job_logical (DDESC *logical_name)
/*
**  Function:	delete_job_logical
**
**  Functional description:
**
**	This routines deassigns a job logical.
**
**  Formal parameters:
**
**	DDESC *logical_name	- pointer to descriptor for name to deassign
**
**  Implicit inputs:
**
**	None.
**
**  Returns:
**
**	Status in R0.
**
*/
{
   int status;

   status = LIB$DELETE_LOGICAL (logical_name, &job_lnmtable);

   /*
   **  If successful, return null string to TPU.
   */
   if (status & 1)
      retmsg.dsc$w_length = 0;

   return(status);

}


int translate_logical_name (DDESC *logical_name)
/*
**  Function:	translate_logical_name
**
**  Functional description:
**
**	This routine translates a logical name.
**
**  Formal parameters:
**
**	DDESC *logical_name	- pointer to descriptor for logical to
**				  translate
**
**  Implicit inputs:
**
**	None.
**
**  Returns:
**
**	Status in R0.  If successful, equivalence is returned in retmsg.
**
*/
{
   $DESCRIPTOR(lnm$table, "LNM$DCL_LOGICAL");
   struct {				/* $TRNLNM item list */
	short length;
	short itmcod;
	unsigned long int bufadr;
	unsigned long int retlen;
   } trnlnm_itmlst[] = {
		{LNM$C_NAMLENGTH, LNM$_STRING, 0, 0}, {0, 0, 0, 0}};

   /*
   **  Set up item list so equivalence string is returned in retmsg.
   */
   trnlnm_itmlst[0].bufadr = (retmsg.dsc$a_pointer);
   trnlnm_itmlst[0].retlen = (unsigned long int) &retmsg.dsc$w_length;

   return (SYS$TRNLNM (0, &lnm$table, logical_name, 0, trnlnm_itmlst));

}

int check_for_dir (void)
/*
**  Function:	check_for_dir
**
**  Functional description:
**
**	This function checks to see if the last file $SEARCHed is a
**	real directory file.
**
**  Formal parameters:
**
**	None.
**
**  Implicit inputs:
**
**	atr_uchar, retmsg
**
**  Returns:
**
**	Status in R0 (SS$_NORMAL).  retmsg contains either "Y" or "N"
**	to be returned to TPU indicating whether or not file is a directory.
**
*/
{
  *(retmsg.dsc$a_pointer) = ( (*((unsigned long int *) atr_uchar) &
				  FCH$M_DIRECTORY) ? 'Y' : 'N');
  retmsg.dsc$w_length = 1;

  return(SS$_NORMAL);

}


int format_return_msg (int istat, DDESC *retstr)
/*
**  Function:	format_return_msg
**
**  Functional description:
**
**	This function formats the string to be returned to TPU.  If
**	istat indicates an error occurred, the error message text is
**	copied to TPU's return string.  Otherwise, the current content
**	of retmsg is copied to the return string.
**
**  Formal parameters:
**
**	int istat	- status value
**	DDESC *retstr	- pointer to descriptor for the return string
**
**  Implicit inputs:
**
**	retmsg
**
**  Returns:
**
**	Status in R0.  The final message is copied to retstr.
**
*/
{
   int status;

   if (!(istat & 1)) {
	retmsg.dsc$w_length = 256;		/* Reset length to 256 */
	status = SYS$GETMSG(istat, &retmsg, &retmsg, 15, 0);
   }

   status = STR$COPY_DX (retstr, &retmsg);

   return(status);

}
