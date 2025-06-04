#pragma module NICE "SRH X1.0-000"

/*
** COPYRIGHT 2001 by Compaq Computer Corporation
*/

/*
**++
**
**  Facility:
**
**	Examples
**
**  Version: V1.1
**
**  Abstract:
**
**	Example of working with the DECnet (Phase IV) NICE protocol.
**
**  Author:
**	Stephen Hoffman
**
**  Creation Date:   1-Jan-1990  V1.0  Written (in VAX C)
**
**  Modification History:
**                  29-Mar-2001  V1.1  Updates to permit Compaq C compile
**
**--
*/

/*
**  This procedure chats with the Network Management Layer of DECnet
**  to retrieve node information.  It was intended as a learning tool
**  and test program.  No claims are made for performance or style.
**
**  The NICE protocol is a DECnet Phase IV/IV+ protocol and is not 
**  particularly useful under DECnet-Plus (Phase V, DECnet/OSI)
**
**  For the technical details of the NICE protocol, see the DECnet
**  Network Management Functional Specification, order AA-X437A-TK.
**  Pointers to the DECnet architecture documentation are included
**  in the OpenVMS Frequently Asked Questions (FAQ) document.
*/
#include    <iodef.h>
#include    <ssdef.h>
#include    <starlet.h>
#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    <stsdef.h>
#include    <descrip.h>

#define	NICE$K_NICEVER	    4
#define	NICE$K_DECECO	    0
#define	NICE$K_USERECO	    0

/*
**  Return status codes -- found in first byte.
*/
#define NICE$K_SUCCESS	    ((unsigned char) 1)
#define NICE$K_ACCEPTED	    ((unsigned char) 2)
#define NICE$K_PARTIAL	    ((unsigned char) 3)
#define NICE$K_NOPRIV	    ((unsigned char) -3)
#define NICE$K_UNRECCOMP    ((unsigned char) -8)
#define NICE$K_FILEOPENERR  ((unsigned char) -13)
#define NICE$K_OPFAILED	    ((unsigned char) -25)
#define NICE$K_DONE	    ((unsigned char) -128)
#define NICE$K_NOSTATUS	    ((unsigned short int) ~0)

/*
**  entity type number
*/
#define NICE$K_EN_NODE	    0
#define NICE$K_EN_LINE	    1
#define NICE$K_EN_LOGGING   2
#define NICE$K_EN_CIRCUIT   3
#define NICE$K_EN_MODULE    4
#define NICE$K_EN_AREA	    5

/*
**  Node identification stuff
**  if >0 then it's the start of the ASCIC nodename string.
*/
#define	NICE$K_NI_SIGNIFICANT	((unsigned char) -5)
#define	NICE$K_NI_ADJACENT	((unsigned char) -4)
#define	NICE$K_NI_LOOP		((unsigned char) -3)
#define	NICE$K_NI_ACTIVE	((unsigned char) -2)
#define	NICE$K_NI_KNOWN		((unsigned char) -1)
#define	NICE$K_NI_ADDRESS	((unsigned char) 0)
#define	NICE$M_NI_EXECUTOR	((unsigned char) 1 << 7)


/*
** Change Option bitmasks
*/
#define	NICE$M_CO_PERMANENT     (1 << 7)    /* volative if zero */
#define	NICE$M_CO_CLEARPURGE	(1 << 6)    /* set/define if zero */
#define	NICE$M_CO_ENTITYTYPE    (0x07)	    /* entity type field */

/*
**  NICE operation codes.
*/	    
#define NICE$K_OP_WRITE	19
#define NICE$K_OP_READ	20

#define RUNT$S_BUFFER	512
#define	RUNT$S_NCBDATA	17
#define	RUNT$S_CHANNEL	2
main()
    {
    char *src_node = "XDELTA";
    char *trg_node = "STAR";
    unsigned char *nice = calloc( 1, RUNT$S_BUFFER );
    unsigned char *ncb	= calloc( 1, RUNT$S_BUFFER );
    unsigned char *index;
    unsigned long int node16;
    unsigned long int area6;
    unsigned long int number10;
    unsigned long int RetStat;
    struct dsc$descriptor dncb =
	{
	0,
	DSC$K_DTYPE_T, DSC$K_CLASS_S,
	(void *) ncb
	};
    unsigned short int src_chan;
    unsigned short int trg_chan;
    unsigned short int iosb[4];

    /*
    **	Build the NCB
    */
    strcpy( (char *) ncb, src_node );
    strcat( (char *) ncb, "::\"NML=/" );

    dncb.dsc$w_length = strlen( (char *) ncb );
    ncb[dncb.dsc$w_length++] = 0;
    ncb[dncb.dsc$w_length++] = 0;

    ncb[dncb.dsc$w_length++] = 3;
    ncb[dncb.dsc$w_length++] = NICE$K_NICEVER;
    ncb[dncb.dsc$w_length++] = NICE$K_DECECO;
    ncb[dncb.dsc$w_length++] = NICE$K_USERECO;
    dncb.dsc$w_length = strlen( (char *) ncb ) + RUNT$S_CHANNEL + RUNT$S_NCBDATA;
    ncb[dncb.dsc$w_length++] = '\"';

    /*
    **	Open a channel to the network...
    */
    RetStat = sys$assign( &dncb, &src_chan, 0, 0 );
    if ( !$VMS_STATUS_SUCCESS( RetStat ))
	return RetStat;

    /*
    **	Build a NICE "show node" request
    */
    dncb.dsc$w_length = 0;
    ncb[dncb.dsc$w_length++] = NICE$K_OP_READ;
    ncb[dncb.dsc$w_length++] = 0x0;
    ncb[dncb.dsc$w_length++] = strlen( trg_node );
    strcpy( (char *) &(ncb[dncb.dsc$w_length]), trg_node );
    dncb.dsc$w_length += strlen( trg_node );

    /*
    **	Write the NCB message and read the response.  (We are
    **	rather cavalier about the $QIOW and the IOSB status.)
    */
    RetStat = sys$qiow( 0, src_chan, IO$_WRITEVBLK, iosb, 0, 0,
	ncb, dncb.dsc$w_length, 0, 0, 0, 0 );
    if (!$VMS_STATUS_SUCCESS( RetStat ))
        return RetStat;
    if (!$VMS_STATUS_SUCCESS( iosb[0] ))
        return iosb[0];

    RetStat = sys$qiow( 0, src_chan, IO$_READVBLK, iosb, 0, 0,
	ncb, RUNT$S_BUFFER, 0, 0, 0, 0 );
    if (!$VMS_STATUS_SUCCESS( RetStat ))
        return RetStat;
    if (!$VMS_STATUS_SUCCESS( iosb[0] ))
        return iosb[0];

    if ( *ncb != NICE$K_ACCEPTED )
        return SS$_ABORT;

    RetStat = sys$qiow( 0, src_chan, IO$_READVBLK, iosb, 0, 0,
	ncb, RUNT$S_BUFFER, 0, 0, 0, 0 );
    if (!$VMS_STATUS_SUCCESS( RetStat ))
        return RetStat;
    if (!$VMS_STATUS_SUCCESS( iosb[0] ))
        return iosb[0];

    /*
    **	Did we get it back?
    */
    if ( *ncb != NICE$K_SUCCESS )
        return SS$_ABORT;

    /*
    **	Is the status reasonable?
    */
    if ( *(unsigned short int *) (ncb + 1 ) != NICE$K_NOSTATUS )
	return SS$_ABORT;

    /*
    **	Now that we've got the reply, parse it.
    */
    if ( *(unsigned char *)(ncb + 3 ) == NICE$K_NI_ADDRESS )
	{
	node16 = *(short*)(ncb + 4);
	area6 = node16 >> 10;
	number10 = node16 & 0x03ff;
	printf("Node %s:: is at address %d.%d.\n",
	    trg_node, area6, number10 );
	}
    else
	return SS$_ABORT;

    if ( *(unsigned char *)(ncb + 6 ) & NICE$M_NI_EXECUTOR )
	printf("(executor)\n" );

    RetStat = sys$qiow( 0, src_chan, IO$_READVBLK, iosb, 0, 0,
	ncb, RUNT$S_BUFFER, 0, 0, 0, 0 );
    if (!$VMS_STATUS_SUCCESS( RetStat ))
        return RetStat;
    if (!$VMS_STATUS_SUCCESS( iosb[0] ))
        return iosb[0];

    if ( *ncb != NICE$K_DONE )
	    return SS$_ABORT;

    /*
    **	All done... Clean up and exit...
    */
    RetStat = sys$dassgn( src_chan );
    if (!$VMS_STATUS_SUCCESS( RetStat ))
        return RetStat;
    if (!$VMS_STATUS_SUCCESS( iosb[0] ))
        return iosb[0];

    return SS$_NORMAL;
    }

