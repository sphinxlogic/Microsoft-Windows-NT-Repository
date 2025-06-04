              <<< NOTED::NOTES$7:[NOTES$LIBRARY]HACKERS.NOTE;1 >>>
                               -< ** Hackers ** >-
================================================================================
Note 394.29  $getspi: Monitoring % CPU Utilization and Process Count    29 of 37
XDELTA::HOFFMAN "Steve, OpenVMS Engineering"        220 lines   2-MAY-1996 12:11
                                  -< I_SPI.C >-
--------------------------------------------------------------------------------
:1.  Interrupt stack time appears to be wrong.  If I take 2 samples, add
:    up the differences of all the modes except interrupt stack, I get
:    a number approx. equal to my elapsed time, in ticks.

   This data alignment stuff is turning into a rather popular question
   of late -- there are questions appearing in other conferences that
   trace back to padding-related problems.

:    I imagine I need updated structure definitions for the Alpha.  Has
:    someone already done this work?  Can I get a copy?

   The structure is (still) correct, it's the compiler that's padding
   the fields in the definitions -- this is a common situation with
   many of the (non-naturally-aligned) structures used by OpenVMS
   system service calls.

   Here's an updated version that runs under DEC C under OpenVMS VAX V6.2,
   and under OpenVMS Alpha V6.2:

	--

/*
//  Demonstration of the undocumented GET SYSTEM PERFORMANCE INFORMATION
//  call EXE$GETSPI used by MONITOR.  The call first appeared in VMS V4.4.
//  Note the calling interface to this particular routine is not documented,
//  and the interface has changed in the past, and may change in the future.
//
//  Use the following LINK command to pick up the definition of EXE$GETSPI.
//
//	$
//	$ CC/DECC I_SPI
//	$ LINK I_SPI,SYS$INPUT/OPTIONS
//	SYS$SHARE:SPISHR/SHARE	! the SPI shareable
//	$
//
//  Note that the SPISHR.EXE shareable image MUST be installed.  (But
//  note MONITOR requires that the shareable be installed, too.)  This
//  program was tested under OpenVMS V6.2.
//
//  Note certain itemcodes are known to have changed at/hear the T5.0
//  OpenVMS release.
//
//  Note the spidef.h module present on recent versions of OpenVMS is
//  not associated and not related to the spidef.h module used here.
//
//  17-Feb-1988	Stephen (XDELTA::) Hoffman
//	Program twisted into a demonstration suitable for public
//	consumption.  I can't be sure any of this really works.
//	(Though I did seem to get the SPI$_DISKS call working...)
//  22-Feb-1988	Stephen (XDELTA::) Hoffman
//	Updated for T5.0 VMS.
//  10-Sep-1992 Peter (RANGER::) Vatne
//	Updated for VMS V5.5.
//  02-Apr-1996 Stephen (XDELTA::) Hoffman
//	Updated for OpenVMS VAX and Alpha V6.2, DEC C.
//
*/
#include <lib$routines.h>
#include <ssdef.h>
#include <starlet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stsdef.h>
#include <syidef.h>
#include "spidef.h"
#define  IL3_MAX	10
/*
//  The following is a vanilla three longword itemlist structure used
//  by the $GETSPI call...  The same format itemlist structure used by
//  the standard/documented SYS$GETxxI calls.
*/
struct item_list_3
    {
    unsigned short item_size;
    unsigned short item_code;
    void *address;
    void *return_length_address;
    };
int exe$getspi( 
	int,	    /* event flag */
	void *,	    /* VMScluster CSID */
	void *,	    /* address of node name descriptor */
	void *,	    /* itemlist address */
	void *,	    /* IOSB address */
	void *,	    /* AST Address */
	void * );   /* AST parameter */
main()
    {
    unsigned long int ef;
    unsigned long int retstat;
    unsigned short int iosb[4];
    int i;
    unsigned long int ModeEntrySize;
#pragma __member_alignment __save
#pragma __nomember_alignment
    /*
    //	The pragma member alignment is used to prevent DEC C
    //	from padding the structure fields, and thus throwing
    //	off the arrays.
    */
    struct ModeEntry
	{
	unsigned char number;		/* cpu number */
	unsigned long int interrupt;    /* interrupt stack ticks */
	unsigned long int mpsynch;	/* spinlock/kernel ticks */
	unsigned long int kernel;	/* kernel mode ticks */
	unsigned long int exec;		/* exec mode ticks */
	unsigned long int super;	/* supervisor mode ticks */
	unsigned long int user;		/* user mode ticks */
	unsigned long int compat;	/* compatibility mode ticks */
	unsigned long int idle;		/* idle ticks */
        };
#pragma __member_alignment __restore
    struct ModeEntry *ModeEntryPtr;
    char *ModeEntryArray;
    unsigned long CurStateProcs;
    unsigned long int TotalTicks;
    unsigned long int ActiveCPUs;
    struct item_list_3 spi_itmlst[IL3_MAX];
    struct item_list_3 syi_itmlst[IL3_MAX];


    i = 0;
    syi_itmlst[i].item_size			= sizeof( int );
    syi_itmlst[i].item_code			= SYI$_ACTIVECPU_CNT;
    syi_itmlst[i].address			= &ActiveCPUs;
    syi_itmlst[i++].return_length_address	= NULL;
    syi_itmlst[i].item_size			= 0;
    syi_itmlst[i].item_code			= 0;
    syi_itmlst[i].address			= NULL;
    syi_itmlst[i++].return_length_address	= NULL;

    /*
    //	Get the number of processors.
    */
    retstat = lib$get_ef( &ef );
    if (!$VMS_STATUS_SUCCESS( retstat ))
	return retstat;
    retstat = sys$getsyi( ef, 0, 0, &syi_itmlst, &iosb, 0, 0);
    if (!$VMS_STATUS_SUCCESS( retstat ))
	return retstat;
    retstat = sys$synch( ef, &iosb);
    if (!$VMS_STATUS_SUCCESS( retstat ))
	return retstat;

    printf("number of active CPUs: %d\n", ActiveCPUs );
    printf("\n");

    /*
    //	Calculate the Size Of The Array Needed For The Data:
    //	((NumberOfCPUs * ModeArrayEntrySize) + LongwordForActiveCPUs
    //	+ LongwordFudgeFactor)
    */
    ModeEntrySize = (sizeof( struct ModeEntry ) * ActiveCPUs) +
	(2 * sizeof( int ));
    ModeEntryArray = malloc( ModeEntrySize );
    memset( ModeEntryArray, 0, ModeEntrySize );

    i = 0;
    spi_itmlst[i].item_size			= ModeEntrySize;
    spi_itmlst[i].item_code			= SPI$_MODES;
    spi_itmlst[i].address			= ModeEntryArray;
    spi_itmlst[i++].return_length_address	= NULL;
    spi_itmlst[i].item_size			= sizeof( CurStateProcs);
    spi_itmlst[i].item_code			= SPI$_CUR;
    spi_itmlst[i].address			= &CurStateProcs;
    spi_itmlst[i++].return_length_address	= NULL;
    spi_itmlst[i].item_size			= 0;
    spi_itmlst[i].item_code			= 0;
    spi_itmlst[i].address			= NULL;
    spi_itmlst[i++].return_length_address	= NULL;

    /*
    //	Make the call. 
    //	The node name argument can be alphanumeric only: No blanks,
    //	underscores or colons are permitted.
    */
    retstat = exe$getspi( ef, 0, 0, &spi_itmlst, &iosb, 0, 0);
    if (!$VMS_STATUS_SUCCESS( retstat ))
	return retstat;
    retstat = sys$synch( ef, &iosb);
    if (!$VMS_STATUS_SUCCESS( retstat ))
	return retstat;

    printf("exe$getspi CPU count 0x0%08.8x\n", *(int *) ModeEntryArray );
    printf("\n");

    ModeEntryPtr = (void *)((char *) ModeEntryArray + sizeof( int ));
    for ( i = 0; i < ActiveCPUs; i++ )
	{
	TotalTicks =
	    ModeEntryPtr->interrupt +
	    ModeEntryPtr->mpsynch  + 
	    ModeEntryPtr->kernel    +
            ModeEntryPtr->exec      +
            ModeEntryPtr->super     +
            ModeEntryPtr->user      +
            ModeEntryPtr->compat    +
            ModeEntryPtr->idle;
	printf("cpu number:     0x0%02.2x\n", ModeEntryPtr->number);
	printf("Total Ticks:    0x0%08.8x\n", TotalTicks );
	printf("interrupt:      0x0%08.8x\n", ModeEntryPtr->interrupt);
	printf("mpsynch:        0x0%08.8x\n", ModeEntryPtr->mpsynch);
	printf("kernel:         0x0%08.8x\n", ModeEntryPtr->kernel);
	printf("executive:      0x0%08.8x\n", ModeEntryPtr->exec);
	printf("supervisor:     0x0%08.8x\n", ModeEntryPtr->super);
	printf("user:           0x0%08.8x\n", ModeEntryPtr->user);
	printf("compatibility:  0x0%08.8x\n", ModeEntryPtr->compat);
	printf("idle            0x0%08.8x\n", ModeEntryPtr->idle);
	printf("\n");
	ModeEntryPtr += 1;
	};

    printf(" number of processes in CUR state: %d\n", CurStateProcs );

    return SS$_NORMAL;
    }
    

