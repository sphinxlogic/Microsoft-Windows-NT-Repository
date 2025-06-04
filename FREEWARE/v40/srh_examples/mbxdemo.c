#module	MBXDEMO "SRH X1.0-000"
#pragma builtins

/*
** COPYRIGHT (c) 1993 BY
** DIGITAL EQUIPMENT CORPORATION, MAYNARD, MASSACHUSETTS.
** ALL RIGHTS RESERVED.
**
** THIS SOFTWARE IS FURNISHED UNDER A LICENSE AND MAY BE USED AND COPIED
** ONLY  IN  ACCORDANCE  OF  THE  TERMS  OF  SUCH  LICENSE  AND WITH THE
** INCLUSION OF THE ABOVE COPYRIGHT NOTICE. THIS SOFTWARE OR  ANY  OTHER
** COPIES THEREOF MAY NOT BE PROVIDED OR OTHERWISE MADE AVAILABLE TO ANY
** OTHER PERSON.  NO TITLE TO AND  OWNERSHIP OF THE  SOFTWARE IS  HEREBY
** TRANSFERRED.
**
** THE INFORMATION IN THIS SOFTWARE IS  SUBJECT TO CHANGE WITHOUT NOTICE
** AND  SHOULD  NOT  BE  CONSTRUED  AS A COMMITMENT BY DIGITAL EQUIPMENT
** CORPORATION.
**
** DIGITAL ASSUMES NO RESPONSIBILITY FOR THE USE  OR  RELIABILITY OF ITS
** SOFTWARE ON EQUIPMENT WHICH IS NOT SUPPLIED BY DIGITAL.
*/

/*
**++
**  Facility:
**
**	Examples
**
**  Version: V1.0
**
**  Abstract:
**
**	Shows some master-slave mailbox operations from C
**
**  Author:
**	Brian Breathnach
**
**  Creation Date:  ??-Mar-1993?
**
**  Modification History:
**	11-Mar-1993 Steve Hoffman
**	    Heavily modified and reorganized, debugged
**
**--
*/

#include <ctype.h>
#include <descrip.h>
#include <dvidef.h>
#include <lib$routines.h>
#include <rms.h>
#include <prcdef.h>
#include <iodef.h>
#include <ssdef.h>
#include <starlet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stsdef.h>

typedef unsigned short int ushort_t;
typedef unsigned long int ulong_t;
typedef unsigned char uchar_t;
typedef struct
    {
    ushort_t cond_value;
    ushort_t count;
    ulong_t info;
    } mbxiosb_t;
typedef struct itemlist_3
    {
    ushort_t itmlen, itmcod;
    uchar_t *itmbuf;
    ushort_t *itmrla;
    } itemlist_3_t;
#define MAXDEVNAMLEN	16
#define MAXMBXMSG	1024
#define MAXMBXBUF       4096
#define MBXNAM		"MBXDEMOMBX"
#define MASTERNAM	"MBXDEMO_MASTER"
#define SLAVENAM	"MBXDEMO_SLAVE"
#define MASTEREXE	"_XDELTA$DKB300:[HOFFMAN.EXAMPLES]MBXDEMO_MASTER.EXE"
#define SLAVEEXE	"_XDELTA$DKB300:[HOFFMAN.EXAMPLES]MBXDEMO_SLAVE.EXE"
#define MASTERLOG	"_XDELTA$DKB300:[HOFFMAN.EXAMPLES]MBXDEMO_MASTER.LOG"
#define SLAVELOG	"_XDELTA$DKB300:[HOFFMAN.EXAMPLES]MBXDEMO_SLAVE.LOG"
#define MASTERDAT	"_XDELTA$DKB300:[HOFFMAN.EXAMPLES]MBXDEMO_MASTER.DAT"
#define SLAVEDAT	"_XDELTA$DKB300:[HOFFMAN.EXAMPLES]MBXDEMO_SLAVE.DAT"

#ifdef MASTER
main()
    {
    mbxiosb_t iosb;
    uchar_t devnam_b[MAXDEVNAMLEN];
    ulong_t pid;
    ulong_t status;
    ushort_t channel;
    ushort_t ret_len;
    FILE fHandle;
    struct dsc$descriptor devnam_d =
	{
	MAXDEVNAMLEN, 0, 0, devnam_b
	};
    itemlist_3_t getdvi_il[2] =
        {
        MAXDEVNAMLEN, DVI$_FULLDEVNAM, devnam_b, &devnam_d.dsc$w_length,
        0, 0, 0, 0
        };
    uchar_t *datfil_b = SLAVEDAT;
    $DESCRIPTOR( mbxnam_d,  MBXNAM );
    $DESCRIPTOR( nla0_d,    "NLA0:");
    $DESCRIPTOR( imgfnm_d,  SLAVEEXE );
    $DESCRIPTOR( logfnm_d,  SLAVELOG );
    $DESCRIPTOR( prcnam_d,  SLAVENAM );


    /*
    **	Write the selection parameters out to a file.
    **	First, open the file.
    */
    fHandle = fopen(datfil_b, "w");
    if (fHandle == 0)
	return SS$_BADPARAM;

    /*
    **	Write out some details to a file.
    */
    fprintf(fHandle, "Program: %s\n", MASTERNAM );
    fprintf(fHandle, "Time1: %s\n", "01-Jan-1993");
    fprintf(fHandle, "Time2: %s\n", "31-Jan-1993");

    /*
    **	Close the selection parameter file.
    */
    fclose(fHandle);


    /*
    **	Create a mailbox to enable communication with the detached process.
    */
    status = sys$crembx(0, &channel, MAXMBXMSG, MAXMBXBUF,
	0, 0, &mbxnam_d, 0);
    if (!$VMS_STATUS_SUCCESS(status))
	return status;
    status = sys$getdviw( 0, channel, 0, getdvi_il, 0, 0, 0, 0 );
    if (!$VMS_STATUS_SUCCESS( status ))
        return status;

    /*
    **	Create a detached process running image imgfnm_d...
    **
    **	Can be enabled or disabled under the debugger -- deposit a
    **	zero/non-zero value here as required.  (Note that the detached
    **	process should *not* be linked with DEBUG, as the debugger
    **	has no way of obtaining reasonable input in the detached
    **	process... Other than eating commands from the input mbx.)
    */
    status = 0;
    if (status == 0)
	{
	status = sys$creprc(
	    &pid,	    /* PIDADR */
	    &imgfnm_d,	    /* IMGDSC */
	    &devnam_d,	    /* Input */
	    &logfnm_d,	    /* Output */
	    &nla0_d,	    /* error */
	    0,		    /* prvadr */
	    0,		    /* quota */
	    &prcnam_d,	    /* PRNDSC */
	    4, 0, 0,	    /* Baspri, Uic, TrmMbxUnt */
	    PRC$M_DETACH ); /* Stsflg */
	if ( !$VMS_STATUS_SUCCESS(status))
	    return status;
	printf("created proces PID 0x0%x\n",pid);
	}
    /*
    **	Store the name of the parameter file in the message to go into the
    **	mailbox.
    */
    status = sys$qio( 0,
	channel,
	IO$_WRITEVBLK,
	&iosb,
	0, 0, 
	datfil_b, strlen( datfil_b ), 0, 0, 0, 0);
    if (!$VMS_STATUS_SUCCESS(status))
	return status;
    status = sys$synch( 0, &iosb );
    if (!$VMS_STATUS_SUCCESS(status))
	return status;
    if (!$VMS_STATUS_SUCCESS(iosb.cond_value))
	return iosb.cond_value;

    return SS$_NORMAL;
    }
#endif

#ifdef SLAVE
main()
    {
    mbxiosb_t iosb;
    ushort_t channel;
    $DESCRIPTOR( sysinp_d, "SYS$INPUT");
    uchar_t input_b[NAM$C_MAXRSS];
    ulong_t status;

    printf("MBXDEMO detached/slave process starting!\n");

    /*	  
    **	Get a pointer to where the selection parameters are.  Assign
    **	a channel to the SYS$INPUT device -- which is assumed to be
    **	a mail box device -- and read the input from the mailbox.)
    */	  
    status = sys$assign( &sysinp_d, &channel, 0, 0, 0 );
    if (!$VMS_STATUS_SUCCESS(status))
        return status;

    /*
    **	Read from the input device -- the mailbox.
    */
    status = sys$qio(0,
		      channel,
		      IO$_READVBLK,
		      &iosb,
		      0,
		      0,
		      input_b, NAM$C_MAXRSS, 0, 0, 0, 0);
    if (!$VMS_STATUS_SUCCESS(status))
        return status;
    status = sys$synch( 0, &iosb );
    if (!$VMS_STATUS_SUCCESS(status))
        return status;
    if (!$VMS_STATUS_SUCCESS(iosb.cond_value))
         return iosb.cond_value;

    printf("%*s\n", iosb.count, input_b );
    printf("Done!\n");

    return SS$_NORMAL;
    }
#endif
