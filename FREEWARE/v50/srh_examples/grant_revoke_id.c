#module grant_revoke_id "V1.0-000"

/*
/*  grant_revoke_id.c
/*
/*  Example/test program that performs a $GRANTID or a $REVOKID on
/*  the current process.  Called as a foreign command; requires
/*  specification of a single argument, a 1 (grant) or a 0 (revoke).
/*  Requires privileges. Not the most flexible of programs, and
/*  definitely sports the archtypical user-hostile interface.
/*
/*  $ cc diddle_id
/*  $ link diddle_id,sys$input/opt
/*  sys$share:vaxcrtl/share
/*  $ diddle :== $ddcu:[dir]diddle_id
*/

#include descrip

main( argc, argv )
int argc;
char **argv;
    {
    $DESCRIPTOR( id, "BATCH" );

    unsigned long int retstat;
    unsigned long int bool;
    unsigned long int mypid = 0;

    if ( argc != 2 )
        {
        printf(
"usage: foreign command \"diddle_id {1|0}\" to {enable|disable} BATCH\n");
        return 1;
        }

    sscanf( argv[1], "%d", &bool );

    if ( bool )
	retstat = sys$grantid( 0, 0, 0, &id, 0 );
    else
	retstat = sys$revokid( &mypid, 0, 0, &id, 0 );

    return( retstat );
    }

