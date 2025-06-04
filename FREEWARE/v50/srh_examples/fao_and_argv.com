$
$BUILD_EXE:
$
$ CC SYS$INPUT/OBJECT=SYS$SCRATCH:FAOAF
#include <descrip.h>
#include <lib$routines.h>
#include <ssdef.h>
#include <starlet.h>
#include <stsdef.h>

#define OUTBUFLEN 512

main( int argc, char **argv )
    {
    unsigned long int retstat;
    $DESCRIPTOR(ctrstr, "!AF");
    $DESCRIPTOR(symb, "AF");
    struct dsc$descriptor_s output = {0, 0, 0, 0};

    output.dsc$w_length = OUTBUFLEN;
    output.dsc$a_pointer = malloc( OUTBUFLEN );

    retstat = sys$fao( &ctrstr,
	&output.dsc$w_length, &output,
	strlen( *(argv + 1) ), *(argv + 1));
    if (!$VMS_STATUS_SUCCESS( retstat )) return retstat;

    retstat = lib$set_symbol( &symb, &output );
    if (!$VMS_STATUS_SUCCESS( retstat )) return retstat;

    return SS$_NORMAL;
    }
$
$ LINK/EXECU=SYS$SCRATCH:FAOAF.EXE SYS$SCRATCH:FAOAF,SYS$INPUT/OPTIONS
SYS$SHARE:VAXCRTL/SHARE
$
$ DELETE SYS$SCRATCH:FAOAF.OBJ;*
$
$ FAOAF :== $SYS$SCRATCH:FAOAF.EXE
$
$PERFORM_TEST:
$
$! make up a string that has a <BEL> at the front.
$
$ TEST = "*TEST STRING"
$ TEST[0,7] = 7
$
$! Pass it into the .EXE
$
$ FAOAF 'TEST'
$
$! And see what we got back...
$
$ SHOW SYMBOL AF
$
$ Exit
