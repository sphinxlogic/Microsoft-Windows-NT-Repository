#include rms
#include descrip
#include psldef
#include lnmdef
main()
    {
    struct FAB fab = cc$rms_fab;
    struct NAM nam = cc$rms_nam;
    char rsa[255];
    int retstat;
    int lnmlength;
    char lnmbuf[255];
    struct
	{
	short size;
	short code;
	int addr;
	int rla;
	} itmlst[] =
	    {
	    255, LNM$_STRING, lnmbuf, 0,
	    4, LNM$_LENGTH, &lnmlength, 0,
	    0, 0, 0, 0
	    };
    $DESCRIPTOR( sysinput, "SYS$INPUT" );
    $DESCRIPTOR( lnmprocess, "LNM$PROCESS" );
    retstat = SYS$TRNLNM( 0, &lnmprocess, &sysinput, &PSL$C_SUPER, itmlst );
    fab.fab$w_ifi = *((unsigned short *) (lnmbuf + 2));
    fab.fab$l_nam = &nam;
    nam.nam$l_rsa = rsa;
    nam.nam$b_rss = 255;
    retstat = SYS$DISPLAY( &fab, 0, 0 );
    exit( 1 );
    }
