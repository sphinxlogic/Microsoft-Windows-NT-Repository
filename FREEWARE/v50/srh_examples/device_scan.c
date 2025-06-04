/*
**  DEVICE_SCAN.C
**
**  A test program for the VMS V5.2 $DEVICE_SCAN system service.  Scans
**  through the system looking for any *VX%0: devices.  If found, this
**  routine checks for an owner PID and for whether or not the DECvoice
**  ACP is mounted.
*/
#include    dvidef
#include    ssdef
#include    stsdef
#include    descrip

#ifndef SS$_NOMOREDEV
#define SS$_NOMOREDEV	2648
#endif

#define DEVNAMLEN   16
#define FAOBUFLEN   80

main()
    {
    struct dsc$descriptor retdevnam =
	{ DEVNAMLEN, DSC$K_DTYPE_T, DSC$K_CLASS_S, malloc(DEVNAMLEN) };
    struct dsc$descriptor fao_dsc =
	{ FAOBUFLEN, DSC$K_DTYPE_T, DSC$K_CLASS_S, malloc(FAOBUFLEN) };
	
    unsigned long int cntxt[2] = { 0, 0 };
    unsigned short int retdevlen = 0;
    unsigned short int devbuflen = 16;
    unsigned long int finstat = SS$_NORMAL;
    unsigned long int pid;
    unsigned long int mnt;
    unsigned long int retstat;
    struct
	{
	unsigned short int buflen;
	unsigned short int itmcod;
	unsigned char *bufadr;
	unsigned char *bufrla;
	} dvi[] =
	    {
	    4, DVI$_PID, &pid, 0,
	    4, DVI$_MNT, &mnt, 0,
	    0, 0, 0, 0,
	    };

    $DESCRIPTOR( mynameis, "DEVICE_SCAN demo program.  Requires VMS V5.2");
    $DESCRIPTOR( decvoice, "*VX%0:" );
    $DESCRIPTOR( faoctrl, "DECvoice !AS, owned by PID !8XL, !AS mounted." );
    $DESCRIPTOR( yesmnt, "is" );
    $DESCRIPTOR( nomnt, "is not" );

    /* The following loop executes at least once.  It attempts to	    */
    /* locate all DECvoice devices configured on the current system.	    */
    /* It then displays the name of each, and whether or not it is	    */
    /* currently allocated to a user.					    */
    
    retstat = lib$put_output( &mynameis );
    do
	{
        retdevnam.dsc$w_length = DEVNAMLEN;
        fao_dsc.dsc$w_length = FAOBUFLEN;
        retstat = sys$device_scan( &retdevnam, &retdevnam.dsc$w_length,
	    &decvoice, 0, cntxt );
	switch ( retstat )
	    {
	    case SS$_NORMAL:
		retstat = sys$getdviw( 0, 0, &retdevnam,
		    dvi, 0, 0, 0, 0 );
		if ( !( $VMS_STATUS_SUCCESS( retstat )) )
		    return( retstat );
		retstat = sys$fao( &faoctrl,
		    &fao_dsc.dsc$w_length,
		    &fao_dsc, &retdevnam, pid,
		    (mnt ? &yesmnt : &nomnt ));
		if ( !( $VMS_STATUS_SUCCESS( retstat )) )
		    return( retstat );
		retstat = lib$put_output( &fao_dsc );
		if ( !( $VMS_STATUS_SUCCESS( retstat )) )
		    return( retstat );
	        break;
	    case SS$_NOMOREDEV:
		break;
	    default:
		finstat = retstat;
		break;
	    }
	}
    while ( $VMS_STATUS_SUCCESS( retstat ));
    
    free( retdevnam.dsc$a_pointer );
    return( finstat );
    
    }
