/*
**  $getdviw test program.
*/
#include    dvidef
#include    ssdef
#include    stsdef
#include    descrip
#include    lib$routines
#include    starlet

#define FAOBUFLEN   80
#define DEVBUFLEN   16

main()
    {
	
    unsigned long int dc, dt;
    unsigned long int retstat;
    unsigned char devbuf[DEVBUFLEN];
    struct dsc$descriptor faoschwartz =
	{
	FAOBUFLEN, DSC$K_DTYPE_T, DSC$K_CLASS_S, malloc(FAOBUFLEN)
	};
    struct
	{
	unsigned short int buflen;
	unsigned short int itmcod;
	unsigned char *bufadr;
	unsigned char *bufrla;
	} dvi[] =
	    {
	    4, DVI$_DEVCLASS, &dc, 0,
	    4, DVI$_DEVTYPE, &dt, 0,
	    DEVBUFLEN, DVI$_DEVNAM, devbuf, 0,
	    0, 0, 0, 0,
	    };

    $DESCRIPTOR( mynameis, "$GETDVIW demo program.");
    $DESCRIPTOR( target, "SYS$LOGIN:LOGIN.COM" );
    $DESCRIPTOR( faoctrl1, "Using target: !AS." );
    $DESCRIPTOR( faoctrl2, "Device: !AZ, class: !8XL, type: !8XL." );

    retstat = lib$put_output( &mynameis );

    faoschwartz.dsc$w_length = FAOBUFLEN;
    retstat = sys$fao( &faoctrl1,
	&faoschwartz.dsc$w_length,
	&faoschwartz, &target );
    if ( !( $VMS_STATUS_SUCCESS( retstat )) ) return( retstat );

    retstat = lib$put_output( &faoschwartz );
    if ( !( $VMS_STATUS_SUCCESS( retstat )) ) return( retstat );
    
    retstat = sys$getdviw( 0, 0, &target,
	dvi, 0, 0, 0, 0 );
    if ( !( $VMS_STATUS_SUCCESS( retstat )) ) return( retstat );

    faoschwartz.dsc$w_length = FAOBUFLEN;
    retstat = sys$fao( &faoctrl2,
	&faoschwartz.dsc$w_length,
	&faoschwartz, devbuf, dc, dt );
    if ( !( $VMS_STATUS_SUCCESS( retstat )) ) return( retstat );

    retstat = lib$put_output( &faoschwartz );
    if ( !( $VMS_STATUS_SUCCESS( retstat )) ) return( retstat );
    
    free( faoschwartz.dsc$a_pointer );

    return retstat;
    }
