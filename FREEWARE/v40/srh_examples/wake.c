#module wake "V1.0-000"

/*
/*  Example/test program that performs a $WAKE on the specified
/*  process.  Called as a foreign command, with one argument, the
/*  hexidecimal PID of the target process.  May require GROUP or
/*  WORLD privilege.
*/

main( argc, argv )
int argc;
char **argv;
    {
    unsigned long int retstat;
    unsigned long int pid;
    char hex[12];
    sscanf( argv[1], "%x", &pid );
    retstat = sys$wake( &pid, 0 );
    return( retstat );
    }

