#include descrip
main()
    {
    unsigned long int retstat;
    $DESCRIPTOR( adigits, "100");
    $DESCRIPTOR( bdigits, "10" );
    $DESCRIPTOR( cdigits, "0000" );
    unsigned long int csign, cexp;
    retstat = STR$DIVIDE(
	&0, &0, &adigits,
	&0, &0, &bdigits,
	&0, &1,
	&csign, &cexp, &cdigits );
    return( 1 );
    }
