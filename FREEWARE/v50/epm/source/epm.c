#define module_name EPM
#define module_ident "V2.0-000"
/*

  Copyright © 1991, 1993 by Edward A. Heinrich of The LOKI Group, Inc.  
  This code may be freely distributed and modified for non-commercial purposes 
  as long as this copyright notice is retained.

  Copyright © 2000 by Hunter Goatley.

*/
/*******************************************************************************
 *									       *
 * FACILITY:								       *
 *	Extensible Performance Monitor - DECUS Presentation		       *
 *									       *
 * PROGRAM:								       *
 *	EPM.C								       *
 *									       *
 * AUTHOR:				CREATION DATE:			       *
 *	Edward A. Heinrich			26-Oct-1990	10:15	       *
 *	The LOKI Group, Inc.						       *
 *	1957 East Oakshire Lane						       *
 *	Sandy, Utah 84092 						       *
 *	heinrich@BYU.EDU						       *
 *									       *
 * ABSTRACT:								       *
 *	This program is ONLY an EXAMPLE of how to call EPMDRIVER routines and  *
 *	format the datum returned from EPMDRIVER.  This was put together       *
 *	quickly for a Fall 1990 DECUS Session and has not been totally	       *
 *	debugged nor tested.  It contains only minimumal functionality required*
 *	to interface with EPMDRIVER and is NOT intended to be a functional     *
 *	replacement for any commercially available performance monitoring      *
 *	product.  Neither Edward A. Heinrich nor The LOKI Group, Inc. make     *
 *	any warranties about the usefulness of any statistics displayed by     *
 *	this program.						       	       *
 *									       *
 *									       *
 * MODIFICATION HISTORY:						       *
 *	V2.0-00		Hunter Goatley		25-MAR-2000 23:07	       *
 *		Minor modifications to compile cleanly with DEC C.	       *
 *		Also, eliminate need for SET COMMAND by getting the	       *
 *		foreign command line and calling CLI$DCL_PARSE.		       *
 *									       *
 *	V1.1-00		Edward A. Heinrich	07-Apr-1993	14:00	       *
 *		Added CLI interface, removed hard-coded file counts, device    *
 *		names, and interval times. Bumped module IDENT to 1.3 to       *
 *		match EPMDRIVER's id.					       *
 *									       *
 ******************************************************************************/
#ifdef __DECC
#pragma module module_name module_ident	/* .OBJ identification (.Ident)       */
#else
#module module_name module_ident	/* .OBJ identification (.Ident)       */
#endif

/*
**	VMS include files
*/
#include <climsgdef.h>			/* CLI$_ variables		      */
#include <descrip .h>			/* Descriptor definitions	      */
#include <iodef.h>			/* VMS I/O definitions		      */
#include <ssdef.h>			/* System service codes 	      */
#include <starlet.h>			/* System service prototypes	      */
#include <stdio.h>			/* Standard I/O file		      */
#include <stdlib.h>			/* Memory allocation routines	      */
#include <string.h>			/* C string routines		      */
#include <stsdef.h>			/* STS$M_ bits defined		      */
/*
**	Program constants
**/
#define FAO_S_LENGTH	300		/* Size of FAO buffer		      */
#define FILE_S_LENGTH	256		/* Size of resultant file name	      */
#define EPB_S_NAME	256		/* Size of file name in EPB	      */

/*
**	Define embedded EPF blocks - to define file counts
*/
struct	epfdef	{
	unsigned int	fid;		/* File id - Number & sequence	      */
	unsigned short	rvn;		/* Relative volume number	      */
	unsigned short	unused;		/* Available word		      */
	unsigned int	count;		/* Number of times file was accessed  */
		};
#define EPF_S_LENGTH	sizeof (struct epfdef)
/*
**	Define EPM Data Blocks - used to return disk data to program.
*/
struct	epbdef	{
		char	name [EPB_S_NAME];
					/* Ascii device name representation   */
	struct	dsc$descriptor_s device;/* Device name as descriptor	      */
	unsigned int	iosb [2];	/* IOSB for this device 	      */
	unsigned int	token;		/* Pointer to our EPB in EPMDRIVER    */
	unsigned int	time [2];	/* Quadword for system time	      */
	unsigned int	writes; 	/* Number of writes to target disk    */
	unsigned int	reads;		/* Number of read I/O's to target disk*/
	unsigned int	swapio; 	/* Number of swapping I/O's detected  */
	unsigned int	pageio; 	/* Number of paging I/O operations    */
	unsigned int	splitio;	/* Number of split I/O's              */
	unsigned int	block1; 	/* Count of 1 block transfers	      */
	unsigned int	block4; 	/* Number of 2-4 block transfers      */
	unsigned int	block16;	/* Count of 5-16 block transfers      */
	unsigned int	block32;	/* Count of 17-32 block transfers     */
	unsigned int	block64;	/* Count of 33-64 block transfers     */
	unsigned int	block96;	/* Count of 65-96 block transfers     */
	unsigned int	large;		/* Count of 97+ block transfers       */
	unsigned char	files;		/* Base of accessed/split file info   */
		} ;
#define EPB_S_LENGTH	sizeof (struct epbdef)
					/* Let compiler compute length for us */
#define	EPB_S_OVERHEAD	17+EPB_S_NAME	/* Overhead of EPB structure	      */
#define EPB_S_DATA	(EPB_S_LENGTH-EPB_S_OVERHEAD)

/*
**	Define global data
*/
short	epm_w_chan;			/* Channel to EPM device	      */
short	tt_w_chan;			/* Channel to input device	      */
char	fao_t_buff  [FAO_S_LENGTH];	/* Formatted output buffer	      */
char	file_t_buff [FILE_S_LENGTH];	/* Full file name buffer	      */
struct	dsc$descriptor_s fao_q_desc;	/* Formatted output descriptor	      */
struct	dsc$descriptor_s file_q_desc;	/* Formatted output descriptor	      */
long	iosb [2];			/* Local I/O status block	      */
int	fileCount = 10;			/* Number of files to collect info on */
int	dataSize;			/* Size of returned data area	      */
/*
**	Allocate VMS descriptor storage
*/
static	$DESCRIPTOR (epm_q_desc, "EPMA0");
static	$DESCRIPTOR (epmcmd_q_desc, "EPM");
static	$DESCRIPTOR (hdrsep_q_desc,
"-------------------------------------------------------------------------------");
static	$DESCRIPTOR (fao_q_fmt1,"!/Disk !AS Statistics as of !%T!/");
static	$DESCRIPTOR (fao_q_fmt3,
	"!(7UL)  !_!(7UL) !_!(7UL) !_   !(7UL)!_  !(7UL)");
static	$DESCRIPTOR (fao_q_fmt2," Writes !_  Reads!_!_Swapper!_!_Paging I/O!_Split I/O ");
static	$DESCRIPTOR (fao_q_fmt4,
"1 Block 2-4 Blocks 5-16 Blocks 17-32 Blocks 33-64 Blocks 65-96 Blocks 97+ Blocks");
static	$DESCRIPTOR (fao_q_fmt4a, 
"!(7UL)    !(7UL)     !(7UL)      !(7UL)      !(7UL)      !(7UL)    !(7UL)");
static	$DESCRIPTOR (fao_q_fmt5,"  Count File Name");
static	$DESCRIPTOR (fao_q_fmt7,"!(7UL)!_!AS");
static	$DESCRIPTOR (fao_q_fmt8,"!/   !AS Split I/O Statistics");
static	$DESCRIPTOR (fao_q_fmt9,"!/   !AS Most Accessed File Statistics");


/******************************************************************************
 *			Start of main-line code
 ******************************************************************************/
int main (void) 			/* Do the .Entry MAIN, 0 gig	      */

{
/*
**	Declare external routines.
*/
    extern int  CLI$PRESENT ();		/* Determine if entity is present     */
    extern int  CLI$GET_VALUE ();	/* Obtain value from command line     */
    extern int  CLI$DCL_PARSE();	/* Parse a command line		      */
    extern int  LIB$CVT_DTB ();		/* LIBRTL convert Decimal to Binary   */
    extern int  LIB$GET_INPUT ();	/* LIBRTL input routine		      */
    extern int  LIB$PUT_OUTPUT ();	/* LIBRTL output routine	      */
    extern int  LIB$GET_FOREIGN();	/* LIBRTL get foreign command line rtn*/
    extern int  STR$CONCAT();		/* LIBRTL concatenate dynamic strings */
    extern void epb_data_ast ();	/* AST routine to process EPM data    */
    extern epm_cld();			/* CLI table address		      */

    struct dsc$descriptor_s cmd_q_desc;	/* Formatted output descriptor	      */
    struct epbdef *epb;			/* EPB for disk device		      */
    int	memSize;			/* Required memory size in bytes      */
    int	status; 			/* Local status field		      */
    int	ccode;				/* Condition code variable	      */
    int	seconds = 60;			/* Number of seconds to wait	      */
    /*
    **	String descriptors for CLI$ routines.
    */
    $DESCRIPTOR (intervalDesc, "INTERVAL");
    $DESCRIPTOR (fileCntDesc,  "FILE_COUNT");
    $DESCRIPTOR (deviceDesc,   "DEVICES");
    /*
    **	Do runtime initialization of descriptor variables...
    */
    fao_q_desc.dsc$a_pointer = fao_t_buff; /* Buffer address		      */
    fao_q_desc.dsc$b_dtype	 = DSC$K_DTYPE_T;
    fao_q_desc.dsc$b_class	 = DSC$K_CLASS_S;
    file_q_desc.dsc$a_pointer = file_t_buff;
    file_q_desc.dsc$b_dtype  = DSC$K_DTYPE_T;
    file_q_desc.dsc$b_class  = DSC$K_CLASS_S;
    cmd_q_desc.dsc$b_dtype = DSC$K_DTYPE_T;
    cmd_q_desc.dsc$b_class = DSC$K_CLASS_D;
    cmd_q_desc.dsc$a_pointer = 0;
    cmd_q_desc.dsc$w_length = 0;

    /*
    **	Get the command line and ask DCL to parse it for us.
    */
    if (!((status = LIB$GET_FOREIGN (&cmd_q_desc)) & 1))
	return (status);

    STR$CONCAT(&cmd_q_desc, &epmcmd_q_desc, &cmd_q_desc);

    if (!((status = CLI$DCL_PARSE (&cmd_q_desc, &epm_cld, LIB$GET_INPUT)) & 1))
	return (status | STS$M_INHIB_MSG);

    /*
    **	Parse for a different time interval.
    */
    if ((status = CLI$PRESENT (&intervalDesc)) == CLI$_PRESENT)
	{
	fao_q_desc.dsc$w_length = 4;
	status = CLI$GET_VALUE (&intervalDesc, &fao_q_desc, 
				&fao_q_desc.dsc$w_length);
	status = LIB$CVT_DTB (fao_q_desc.dsc$w_length, fao_q_desc.dsc$a_pointer,
				&seconds);
	if (!(status & STS$M_SUCCESS))
	    seconds = 60;
	}
    /*
    **	Parse command line for file counts.
    */
    if ((status = CLI$PRESENT (&fileCntDesc)) == CLI$_PRESENT)
	{
	fao_q_desc.dsc$w_length = 4;	/* Enough buffer space		      */
	status = CLI$GET_VALUE (&fileCntDesc, &fao_q_desc, 
				&fao_q_desc.dsc$w_length);
	status = LIB$CVT_DTB (fao_q_desc.dsc$w_length, fao_q_desc.dsc$a_pointer,
				&fileCount);
	if (!(status & STS$M_SUCCESS))
	    fileCount = 10;
	}

    memSize   = EPB_S_LENGTH + (2*EPF_S_LENGTH*fileCount);
    dataSize  = memSize;			/* Compute returned data buffer length*/
    dataSize -= EPB_S_OVERHEAD;		/* Subtract overhead (compiler bug?)  */

    /*
     *	Assign a channel to the EPM device.
     */
    status = sys$assign (		/* Assign a channel to EPMDRIVER      */
		&epm_q_desc,		/* Address of device to assign to     */
		&epm_w_chan,		/* Resultant channel holder	      */
		0, 0);			/* No ACMODE or MBXNAM parameters     */
    if (status != SS$_NORMAL)		/* See if we had success in the ASSIGN*/
	sys$exit (status);		/* Return error if one		      */

    /*
    **	Obtain the list of devices to be monitored.
    */
    while (1)				/* Loop awhile			      */
	{
	fao_q_desc.dsc$w_length = FAO_S_LENGTH;
	status = CLI$GET_VALUE (&deviceDesc, &fao_q_desc,
				&fao_q_desc.dsc$w_length);
	/*
	**	Allocate memory to contain EPB for this device.
	*/
	epb = (struct epbdef *)calloc (memSize, sizeof(char));
	if ((char *)epb == NULL)
	    {
	    printf ("\nEPM-F-NOMEMORY, unable to obtain memory for EPB");
	    sys$exit (SS$_INSFMEM);
	    }
	/*
	**	Build the device name descriptor inside of the EPB.
	*/
	strncpy (epb->name, fao_q_desc.dsc$a_pointer, fao_q_desc.dsc$w_length);
	epb->device.dsc$a_pointer = epb->name;
	epb->device.dsc$w_length  = fao_q_desc.dsc$w_length;
	epb->device.dsc$b_dtype   = DSC$K_DTYPE_T;
	epb->device.dsc$b_class   = DSC$K_CLASS_S;
	/*
	**  Issue the QIO to EPMDRIVER to express an interest in the device
	*/
	ccode = sys$qiow ( 2,		/* Use event flag # 2 here	      */
		epm_w_chan,		/* Assigned channel offset	      */
		IO$_MOUNT,		/* Start monitoring		      */
		&epb->iosb,		/* Include an IOSB address - always   */
		0, 0,			/* No AST or ASTPRM arguments	      */
		&epb->device,		/* Device to monitor		      */
		0,			/* No P2 argument		      */
		fileCount,		/* Number of files to collect on      */
		seconds,		/* P4 is number of seconds	      */
		0,0);			/* No P5-P6 parameters		      */
	if (ccode & STS$M_SUCCESS)	/* If R0 has success		      */
	    ccode = epb->iosb [0];	/* Then obtain IOSB status code	      */
	if (ccode != SS$_NORMAL)	/* If not successful here	      */
	    sys$exit (ccode);		/* Then exit the image now	      */
	/*
	**  IOSB[1] contains the token we pass for reading the device statistics.
	*/
	epb->token = epb->iosb [1];	/* Grab EPB address from IOSB+4       */
	ccode = sys$qio (0,		/* Take default EFN #0		      */
			epm_w_chan,	/* Channel assigned to EPMDRIVER      */
			IO$_READVBLK,	/* Read virtual is funtion code       */
			&epb->iosb,	/* IOSB address 		      */
			epb_data_ast,	/* AST routine address		      */
			(char *)epb,	/* AST parameter		      */
			&epb->token,	/* Beginning of returned data area    */
			dataSize,	/* Length of the data buffer	      */
			0,		/* No P3 parameter		      */
			epb->token,	/* EPB address must be passed back    */
			0,0);		/* No P5 or P6 parameters required    */
	if (!(ccode & STS$M_SUCCESS))
	    sys$exit (ccode);
	if (status == CLI$_COMMA || status == CLI$_CONCAT)
	    continue;			/* Loop back if more devies to parse  */
	else
	    break;			/* Else break from the loop	      */
	}

    if (status == SS$_NORMAL)		/* If successfully queued the IRP     */
	sys$hiber ();			/* Go to sleep my baby		      */

    status = sys$dassgn (epm_w_chan);	/* Deassign channel to EPM device     */
    return (SS$_NORMAL);		/* Return to CLI when we awaken       */
}



/*
** ROUTINE:	epb_data_ast 
**
** FUNCTIONAL DESCRIPTION:
**	AST routine to process any data sent back to us from EPMDRIVER
**	concerning the disk being monitored.
**
** INPUTS:
**	4(AP)	address of EPB of interest
**
** ENVIRONMENT:
**	User-mode AST.
**
*/
void epb_data_ast (struct epbdef *epbptr)
					/* AST for returned statistics	      */
{
    struct epfdef *epfptr;
    unsigned char *ptr;
    extern int  LIB$PUT_OUTPUT ();	/* LIBRTL output routine	      */
    extern int  display_files ();	/* Output file names from FID	      */
    int index;				/* Loop index			      */
    int status; 			/* Local status field		      */
    /*
     *	Tell 'em what disk we're looking at and what time is was...
     */
    LIB$PUT_OUTPUT (&hdrsep_q_desc);	/* Output the heading line	      */
    fao_q_desc.dsc$w_length = FAO_S_LENGTH; /* Reset maximum size in descriptor   */
    sys$fao(&fao_q_fmt1,		/* Format control string address      */
	&fao_q_desc.dsc$w_length,	/* Resultant length field address     */
	&fao_q_desc,			/* Output buffer descriptor address   */
	&epbptr->device, 		/* Device we're monitoring            */
	&epbptr->time); 		/* Time stats were passed on to us    */
    LIB$PUT_OUTPUT (&fao_q_desc);	/* Output the heading line	      */
    fao_q_desc.dsc$w_length = FAO_S_LENGTH; /* Reset maximum size in descriptor   */
    sys$fao(&fao_q_fmt2,		/* Format contrl string address       */
	&fao_q_desc.dsc$w_length,	/* Output length address	      */
	&fao_q_desc);			/* Output descriptor address	      */
    LIB$PUT_OUTPUT (&fao_q_desc);	/* Output the heading line	      */
    fao_q_desc.dsc$w_length = FAO_S_LENGTH; /* Reset maximum size in descriptor   */
    /*
    **	Tell us what type of I/O's we had
    */
    sys$fao(&fao_q_fmt3,		/* Format control string address      */
	&fao_q_desc.dsc$w_length,	/* Resultant length field	      */
	&fao_q_desc,			/* Output descriptor address	      */
	epbptr->writes, 		/* Number of writes counted	      */
	epbptr->reads,			/* Total number of read operations    */
	epbptr->swapio, 		/* Number of SWAPPER I/O's            */
	epbptr->pageio, 		/* Number of PAGING I/O's             */
	epbptr->splitio);		/* Number of split I/O's detected     */
    LIB$PUT_OUTPUT (&fao_q_desc);	/* Output the heading line	      */
    fao_q_desc.dsc$w_length = FAO_S_LENGTH; /* Reset maximum size in descriptor   */
    sys$fao(&fao_q_fmt4,		/* Go format the data		      */
	&fao_q_desc.dsc$w_length,	/* Resultant length address	      */
	&fao_q_desc);			/* Output descriptor address	      */
    LIB$PUT_OUTPUT (&fao_q_desc);	/* Output the heading line	      */

    /*
    **	Tell us the distribution of requests in terms of blocks per request
    */
    fao_q_desc.dsc$w_length = FAO_S_LENGTH; /* Reset maximum size in descriptor   */
    sys$fao(&fao_q_fmt4a,		/* Format control string address      */
	&fao_q_desc.dsc$w_length,	/* Formatted string length field      */
	&fao_q_desc,			/* Output descriptor address	      */
	epbptr->block1, 		/* Number of 1 block accesses	      */
	epbptr->block4, 		/* Number of 2-4 block accesses       */
	epbptr->block16,		/* Number of 5-16 block accesses      */
	epbptr->block32,		/* Number of 17-32 block transfers    */
	epbptr->block64,		/* Number of 33-64 block transfers    */
	epbptr->block96,
	epbptr->large);

    LIB$PUT_OUTPUT (&fao_q_desc);	/* Output the heading line	      */
    /*
    **	Obtain the list of files that had split I/O operations
    */
    ptr = &epbptr->files;	/* Base address of file information */
    ptr += (fileCount * sizeof(struct epfdef));	/* Skip past to accesses */
    epfptr = (struct epfdef *) ptr;
    if (epfptr->fid)			/* If there were any		      */
	{
	fao_q_desc.dsc$w_length = FAO_S_LENGTH;
	sys$fao(&fao_q_fmt8,		/* Format control string address      */
		&fao_q_desc.dsc$w_length,
		&fao_q_desc,		/* Output buffer descriptor address   */
		&epbptr->device); 	/* Device we're monitoring            */
	LIB$PUT_OUTPUT (&fao_q_desc);	/* Then output the heading lines      */
	LIB$PUT_OUTPUT (&fao_q_fmt5);
	display_files (epfptr, epbptr);	/* Go display the actual filenames    */
	}
    /*
    **	Display the top most accessed files.
    */
    ptr = &epbptr->files;
    epfptr = (struct epfdef *) ptr;
    if (epfptr->fid)			/* If non-zero FID entry in table     */
	{
	fao_q_desc.dsc$w_length = FAO_S_LENGTH;
	sys$fao(&fao_q_fmt9,		/* Format control string address      */
		&fao_q_desc.dsc$w_length,
		&fao_q_desc,		/* Output buffer descriptor address   */
		&epbptr->device); 	/* Device we're monitoring            */
	LIB$PUT_OUTPUT (&fao_q_desc);	/* Then output the heading lines      */
	LIB$PUT_OUTPUT (&fao_q_fmt5);
	display_files (epfptr, epbptr);	/* Go display the actual filenames    */
	}
    /*
    **	Reissue the READ QIO to EPMDRIVER and dismiss this AST unless an error.
    */
    if (epbptr->iosb [0] != SS$_NORMAL)/* If not success from last I/O	      */
	sys$wake (0,0); 		/* Wake ourself up		      */
    else
	sys$qio(0,			/* Request more data in time	      */
		epm_w_chan,		/* Channel to EPMA0 device	      */
		IO$_READVBLK,		/* Function is READ data	      */
		&epbptr->iosb,		/* Address of IOSB		      */
		epb_data_ast,		/* Come back here to display data     */
		epbptr, 		/* Pass our EPB address as ASTPRM     */
		&epbptr->token, 	/* Address of returned data buffer    */
		dataSize,		/* Size of our buffer		      */
		0,			/* No P3 parameter		      */
		epbptr->token,		/* Our EPB address		      */
		0,0);			/* Requeue I/O			      */
    return; 				/* Dismiss the AST		      */
}


/*
** ROUTINE:	display_files
**
** FUNCTIONAL DESCRIPTION:
**	Local procedure to output filenames from FID list.
**
** INPUTS:
**	epf	address of EPF pointer
**	epb	address of EPB pointer
**
** OUTPUTS:
**	Always returns SS$_NORMAL.
**
*/
int	display_files (struct epfdef *epf, struct epbdef *epb)
{
    extern int LIB$FID_TO_NAME (); 	/* Convert File ID to file name       */
    extern int LIB$PUT_OUTPUT ();	/* LIBRTL output routine	      */
    int index;				/* Local loop index		      */
    int ccode;				/* Condition code variable	      */
    unsigned char *ptr;

    for (index=0; index<fileCount; index++)
	{				/* Loop thru all FID entries	      */
	if (epf->fid == 0)
	    break;			/* We be done if FID is zero	      */
	fao_q_desc.dsc$w_length  = FAO_S_LENGTH;
	file_q_desc.dsc$w_length = FILE_S_LENGTH;
					/* Reset sizes in descriptors	      */
	/*
	 *	 Call LIB$ routine to get file name
	*/
	ccode = LIB$FID_TO_NAME (	/* Convert FID to ascii file name     */
			 &epb->device,	/* Device it resides on		      */
			 &epf->fid,	/* FID to convert		      */
			 &file_q_desc,	/* Descriptor for file name	      */
			 &file_q_desc,	/* Output length		      */
			 0, 0); 	/* Skip directory id and ACP status   */
	if (ccode == SS$_NORMAL)	/* Do only if LIB$FID_TO_NAME worked  */
	    {
	    sys$fao(&fao_q_fmt7,	/* Format control string address      */
			&fao_q_desc.dsc$w_length,
			&fao_q_desc,	/* Where to stash resultant string    */
			epf->count,	/* Access count field		      */
			&file_q_desc);	/* File name as descriptor	      */
	    LIB$PUT_OUTPUT (&fao_q_desc);
					/* Output the heading line	      */
	    }				/* End if (ccode == SS$_NORMAL)       */

	ptr = (unsigned char *) epf + sizeof(struct epfdef);
	epf = (struct epfdef *) ptr;
					/* Advance to next EPF entry	      */
	}				/* End of FOR INDEX loop	      */
    return (SS$_NORMAL);		/* Status to caller		      */
}
