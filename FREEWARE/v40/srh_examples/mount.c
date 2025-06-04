#include    <descrip.h>
#include    <lib$routines.h>
#include    <mntdef.h>
#include    <ssdef.h>
#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    <stsdef.h>
#include    <starlet.h>

#define DEVNAMLEN   64

unsigned int
MountSignalHandler( void *SigArgsVoid, void *MechArgsVoid )
    {
    int RetStat;
    int n;
    int *SigArgs = SigArgsVoid;
    int *MechArgs = MechArgsVoid;

    if (( SigArgs[1] == SS$_UNWIND ) ||
        ( SigArgs[1] == SS$_DEBUG ))
        {
        return SS$_RESIGNAL;
        }

    RetStat = $VMS_STATUS_SEVERITY( SigArgs[1] );
    if (( RetStat == STS$K_SUCCESS ) ||
        ( RetStat == STS$K_INFO ))
        return SS$_CONTINUE;

    return SS$_RESIGNAL;
    }
main()
    {
    int RetStat;
    int i;
    char devnam[DEVNAMLEN];
    struct
	{
	unsigned short int buflen;
	unsigned short int itmcod;
	void *bufadr;
	unsigned short int *bufrla;
	} itmlst[10];
    int MntFlags[2];

    strcpy( devnam, "XDELTA$MKA500" );

    MntFlags[0] = MNT$M_FOREIGN;    /* | MNT$M_MESSAGE */;
    MntFlags[1] = 0;

    i = 0;
    itmlst[i].buflen = strlen( devnam );
    itmlst[i].itmcod = MNT$_DEVNAM;
    itmlst[i].bufadr = devnam;
    itmlst[i++].bufrla = NULL;
    itmlst[i].buflen = 8;
    itmlst[i].itmcod = MNT$_FLAGS;
    itmlst[i].bufadr = MntFlags;
    itmlst[i++].bufrla = NULL;
    itmlst[i].buflen = 0;
    itmlst[i].itmcod = 0;
    itmlst[i].bufadr = NULL;
    itmlst[i++].bufrla = NULL;

    VAXC$ESTABLISH( MountSignalHandler );

    RetStat = sys$mount( itmlst );
    if ( !( $VMS_STATUS_SUCCESS( RetStat )) )
	return( RetStat );

    VAXC$ESTABLISH( lib$sig_to_ret );

    RetStat = sys$mount( itmlst );
    if ( !( $VMS_STATUS_SUCCESS( RetStat )) )
	return( RetStat );

    return RetStat;
    }

