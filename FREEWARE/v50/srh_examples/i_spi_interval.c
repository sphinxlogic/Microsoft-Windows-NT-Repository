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
//  01-Jul-1996 Stephen (XDELTA::) Hoffman
//	Now displays interval; updated to display interrupt time alone
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
#define  DELAY_INTERVAL_IN_SECONDS	10
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
	unsigned long int interrupt;    /* kernel on i-stack *and* idle ticks */
	unsigned long int mpsynch;	/* spinlock/kernel ticks */
	unsigned long int kernel;	/* kernel mode ticks */
	unsigned long int exec;		/* exec mode ticks */
	unsigned long int super;	/* supervisor mode ticks */
	unsigned long int user;		/* user mode ticks */
	unsigned long int compat;	/* compatibility mode ticks */
	unsigned long int idle;		/* idle ticks */
        };
#pragma __member_alignment __restore
    struct ModeEntry *ModeEntryPtr0, *ModeEntryPtr1;
    char *ModeEntryArray0, *ModeEntryArray1;
    unsigned long CurStateProcs;
    unsigned long int TotalTicks0, TotalTicks1, IdleTicks0, IdleTicks1;
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
    if (!$VMS_STATUS_SUCCESS( retstat )) return retstat;
    retstat = sys$getsyi( ef, 0, 0, &syi_itmlst, &iosb, 0, 0);
    if (!$VMS_STATUS_SUCCESS( retstat )) return retstat;
    retstat = sys$synch( ef, &iosb);
    if (!$VMS_STATUS_SUCCESS( retstat )) return retstat;

    /*
    //	Calculate the Size Of The Array Needed For The Data:
    //	((NumberOfCPUs * ModeArrayEntrySize) + LongwordForActiveCPUs
    //	+ LongwordFudgeFactor)
    */
    ModeEntrySize = (sizeof( struct ModeEntry ) * ActiveCPUs) + (2 * sizeof( int ));
    ModeEntryArray0 = malloc( ModeEntrySize );
    memset( ModeEntryArray0, 0, ModeEntrySize );
    ModeEntryArray1 = malloc( ModeEntrySize );
    memset( ModeEntryArray1, 0, ModeEntrySize );

    /*
    //	Create the itemlist.  Note that this itemlist gets reused below, so
    //	beware of re-ordering _any_ of the itemcode entries in this list...
    */
    i = 0;
    spi_itmlst[i].item_size			= ModeEntrySize;
    spi_itmlst[i].item_code			= SPI$_MODES;
    spi_itmlst[i].address			= ModeEntryArray0;
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
    //	Collect a sample...
    */
    printf("Collecting a sample...\n");
    retstat = exe$getspi( ef, 0, 0, &spi_itmlst, &iosb, 0, 0);
    if (!$VMS_STATUS_SUCCESS( retstat )) return retstat;
    retstat = sys$synch( ef, &iosb);
    if (!$VMS_STATUS_SUCCESS( retstat )) return retstat;
    printf("...Sample collected.\n");

    /*
    //	Accumulate the ticks from all processors from the first sample...
    //	(->idle is *not* counted here, as it's also part of ->interrupt.)
    */
    ModeEntryPtr0 = (void *)((char *) ModeEntryArray0 + sizeof( int ));
    IdleTicks0 = 0;
    TotalTicks0 = 0;
    for ( i = 0; i < ActiveCPUs; i++ )
	{
	TotalTicks0 +=
	    ModeEntryPtr0->interrupt +
	    ModeEntryPtr0->mpsynch   +
	    ModeEntryPtr0->kernel    +
            ModeEntryPtr0->exec      +
            ModeEntryPtr0->super     +
            ModeEntryPtr0->user      +
            ModeEntryPtr0->compat;
	IdleTicks0 += ModeEntryPtr0->interrupt - ModeEntryPtr0->idle;
	ModeEntryPtr0 += 1;
	};

    /*
    //	Reset the itemlist -- note we basically reuse the original list...
    */
    i = 0;
    spi_itmlst[i].address			= ModeEntryArray1;

    /*
    //	Delay by some number of seconds...
    */
    printf("Waiting 0x0%x seconds...\n", DELAY_INTERVAL_IN_SECONDS );
    sleep( DELAY_INTERVAL_IN_SECONDS );

    /*
    //	Collect another sample...
    */
    printf("Collecting a sample...\n");
    retstat = exe$getspi( ef, 0, 0, &spi_itmlst, &iosb, 0, 0);
    if (!$VMS_STATUS_SUCCESS( retstat )) return retstat;
    retstat = sys$synch( ef, &iosb);
    if (!$VMS_STATUS_SUCCESS( retstat )) return retstat;
    printf("...Sample collected.\n");

    /*
    //	Accumulate the ticks from all processors from the second sample...
    //	(->idle is *not* counted here, as it's also part of ->interrupt.)
    */
    ModeEntryPtr1 = (void *)((char *) ModeEntryArray1 + sizeof( int ));
    IdleTicks1 = 0;
    TotalTicks1 = 0;
    for ( i = 0; i < ActiveCPUs; i++ )
	{
	TotalTicks1 +=
	    ModeEntryPtr1->interrupt +
	    ModeEntryPtr1->mpsynch  + 
	    ModeEntryPtr1->kernel    +
            ModeEntryPtr1->exec      +
            ModeEntryPtr1->super     +
            ModeEntryPtr1->user      +
            ModeEntryPtr1->compat;
	IdleTicks1 += ModeEntryPtr1->interrupt - ModeEntryPtr1->idle;
	ModeEntryPtr1 += 1;
	};

    /*
    //	Now display the results...
    */
    printf("Number of CPUs present:   0x0%08.8x\n", *(int *) ModeEntryArray0 );
    printf("number of active CPUs:    0x0%08.8x\n", ActiveCPUs );

    printf("\n");

    printf("Total Ticks (all CPUs):   0x0%08.8x\n", TotalTicks1 - TotalTicks0 );

    ModeEntryPtr0 = (void *)((char *) ModeEntryArray0 + sizeof( int ));
    ModeEntryPtr1 = (void *)((char *) ModeEntryArray1 + sizeof( int ));
    printf("\n");
    printf("Accumulated (per-CPU) counts:\n");
    for ( i = 0; i < ActiveCPUs; i++ )
	{
	printf("0x0%02.2x:interrupt:         0x0%08.8x\n", ModeEntryPtr1->number, IdleTicks1 - IdleTicks0 );
	printf("0x0%02.2x:mpsynch:           0x0%08.8x\n", ModeEntryPtr1->number, ModeEntryPtr1->mpsynch - ModeEntryPtr0->mpsynch);
	printf("0x0%02.2x:kernel:            0x0%08.8x\n", ModeEntryPtr1->number, ModeEntryPtr1->kernel - ModeEntryPtr0->kernel);
	printf("0x0%02.2x:executive:         0x0%08.8x\n", ModeEntryPtr1->number, ModeEntryPtr1->exec - ModeEntryPtr0->exec);
	printf("0x0%02.2x:supervisor:        0x0%08.8x\n", ModeEntryPtr1->number, ModeEntryPtr1->super - ModeEntryPtr0->super);
	printf("0x0%02.2x:user:              0x0%08.8x\n", ModeEntryPtr1->number, ModeEntryPtr1->user - ModeEntryPtr0->user);
	printf("0x0%02.2x:compatibility:     0x0%08.8x\n", ModeEntryPtr1->number, ModeEntryPtr1->compat - ModeEntryPtr0->compat);
	printf("0x0%02.2x:idle               0x0%08.8x\n", ModeEntryPtr1->number, ModeEntryPtr1->idle - ModeEntryPtr0->idle);
	printf("\n");
	ModeEntryPtr0 += 1;
	ModeEntryPtr1 += 1;
	};

    printf(" number of processes in CUR state: %d\n", CurStateProcs );

    return SS$_NORMAL;
    }
    

