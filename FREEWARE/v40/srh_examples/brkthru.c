/*
/* This program demonstrates the $BRKTHRUW system service; this
/* program sends a message to a hardcoded username; this demo
/* uses the MAIL broadcast class with a five second timeout.
*/

#include brkdef
#include descrip
#include ssdef
#include stsdef

main()
    {
    unsigned long int carcon = 0x20;
    unsigned long int retstat;
    unsigned short int iosb[4];

    $DESCRIPTOR( trg, "HOFFMAN" );
    $DESCRIPTOR( msg, "This is a $BRKTHRU test" );

    /*
    /*  And now we do what we came here for...  The $brkthru.
    */
    retstat = sys$brkthruw( 0, &msg,
	&trg, BRK$C_USERNAME, iosb, &carcon, 0, BRK$C_MAIL, &5, 0, 0 );
    if  (!$VMS_STATUS_SUCCESS( retstat ))
	return( retstat );
    if  (!$VMS_STATUS_SUCCESS( iosb[0] ))
	return( iosb[0] );

    return( SS$_NORMAL );
    }
