#module	qdemo	"V1.0"

/*
** COPYRIGHT (c) 1990 BY
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
**
*/

/*
**++
**  FACILITY:  Examples
**
**  MODULE DESCRIPTION:
**
**      This routine contains a demonstration of the VMS self-relative
**	interlocked RTL queue routines LIB$REMQHI() and LIB$INSQTI(),
**	and a demonstration of the VMS VAX C memory management routines.
**
**  AUTHORS:
**
**      Stephen Hoffman
**
**  CREATION DATE:  21-Jan-1990
**
**  DESIGN ISSUES:
**
**      NA
**
**  MODIFICATION HISTORY:
**
**      {@tbs@}...
**--
*/

/*
**  $! queue demo build procedure...
**  $ cc/debug/noopt qdemo
**  $ link qdemo,sys$input/opt/debug
**  sys$share:vaxcrtl/share
**  $!
*/

/*
**
**  INCLUDE FILES
**
*/

#include <libdef.h>
#include <ssdef.h>
#include <stsdef.h>

main()
MAIN_PROGRAM
    {
    unsigned long int retstat;
    unsigned long int i;
    struct queueblock
	{
	unsigned long int *flink;
	unsigned long int *blink;
	unsigned long int dd;
	} *qb;
    /*
    **	Allocate the (zeroed) queue header now.
    **
    **	The interlocked queue forward and backward links located in
    **	the queue header (of self-relative queues) must be initialized
    **	to zero prior to usage.  calloc() performs this for us.  Blocks
    **	allocated and inserted in the queue subsequently need not have
    **  their links zeroed.
    **
    **	NB: On VMS, the calloc() and malloc() routines acquire memory
    **	that is quadword (or better) aligned.  The VAX hardware queue
    **	instructions (and thus the queue routines) require a minimum
    **	of quadword alignment.
    */
    struct queueblock *header = calloc(1, sizeof( struct queueblock ));
    struct queueblock *qtmp = 0;

    printf( "queue demo\n" );

    /*
    **  dynamically allocate the memory for each block, place a value
    **  in the block and insert the block onto the tail of the queue.
    */
    for ( i = 0; i < 10; i++ )
	{
	qtmp = calloc(1,sizeof( struct queueblock ));
	qtmp->dd = i;
	printf( "inserting item: %d\n", qtmp->dd );
	retstat = LIB$INSQTI( qtmp, header );
	};

    /*
    **	Remove queue entries until there are no more.
    */
    retstat = SS$_NORMAL;
    while ( $VMS_STATUS_SUCCESS( retstat ) )
	{
	retstat = LIB$REMQHI( header, &qtmp );
	if ( $VMS_STATUS_SUCCESS( retstat ) )
	    {
	    printf( "removing item: %d\n", qtmp->dd );
	    cfree( qtmp );
	    }
	}

    if ( retstat != LIB$_QUEWASEMP )
	printf( "unexpected status %x received\n", retstat );
    else
	printf( "expected completion status received\n" );

    return SS$_NORMAL; 
    }
