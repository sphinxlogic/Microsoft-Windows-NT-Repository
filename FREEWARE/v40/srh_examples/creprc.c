#module srh_creprc "V1.0"

/*
**++
**  FACILITY:  examples
**
**  MODULE DESCRIPTION:
**
**      performs a lib$spawn, but doesn't need a cli around
**
**  AUTHORS:
**
**      steve hoffman
**
**  CREATION DATE:  2-jul-1993
**
**  DESIGN ISSUES:
**
**      while this routine performs the indicated task of spawning
**	a subprocess, note that use of the sys$sndjbc() system service
**	to submit the specified procedure may also be appropriate.
**
**  MODIFICATION HISTORY:
**
**      {@tbs@}...
**--
*/

#include <descrip.h>
#include <pqldef.h>
#include <ssdef.h>
#include <stsdef.h>

main()
    {
    unsigned long int retstat;
    $DESCRIPTOR( log, "SYS$MANAGER:X.LOG" );
    $DESCRIPTOR( cmd, "SYS$MANAGER:LOGIN.COM" );
    
    retstat = srh_creprc( &cmd, &log, 0, 0 );
    return SS$_NORMAL;
    }

int srh_creprc( struct dsc$descriptor *cmdspec_a,
    struct dsc$descriptor *logspec_a,
    unsigned long int *pid_a,
    struct dsc$descriptor *prcnam_a )
    {
    unsigned long int retstat;
    struct dsc$descriptor *logspec;
    struct dsc$descriptor *prcnam;
    $DESCRIPTOR( nla0, "_NLA0:" );
    $DESCRIPTOR( loginout, "SYS$SYSTEM:LOGINOUT.EXE" );
    $DESCRIPTOR( srh, "SRH" );
    unsigned long int *pid;
    unsigned long int pid_bogus;
    unsigned long int baspri = 4;
    unsigned long int mbxunt = 0;
    unsigned long int uic = 0;
    unsigned long int stsflgs = 0;
    struct
	{
	unsigned char pql_code;
	unsigned long int pql_value;
	} pql[] =
	    {
	    {	PQL$_ASTLM,        600	    },
	    {	PQL$_BIOLM,        100	    },
	    {	PQL$_BYTLM,        131072   },
	    {	PQL$_CPULM,        0	    },
	    {	PQL$_DIOLM,        100	    },
	    {	PQL$_FILLM,        50	    },
	    {	PQL$_PGFLQUOTA,    40960    },
	    {	PQL$_PRCLM,        16	    },
	    {	PQL$_TQELM,        600	    },
	    {	PQL$_WSDEFAULT,    512	    },
	    {	PQL$_WSQUOTA,      2048	    },
	    {	PQL$_ENQLM,        600	    },
	    {	PQL$_WSEXTENT,     4096	    },
	    {	PQL$_JTQUOTA,      4096	    },
	    {	PQL$_LISTEND,      0	    }
	    };

    /*
    **	Allow the log file specification and the process name
    **	specification to be defaulted.
    */
    logspec = ((int) logspec_a) ? logspec_a : &nla0;
    prcnam = ((int) prcnam_a) ? prcnam_a : &srh;
    pid = ((int) pid_a) ? pid_a : &pid_bogus;

    /*
    **	Perform the process creation.
    */	
    retstat = sys$creprc( pid,
	&loginout, cmdspec_a, logspec, &nla0, 0,
	pql, prcnam, baspri, uic, mbxunt, stsflgs );

    return retstat;

    }
