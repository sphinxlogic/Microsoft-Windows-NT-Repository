/*
//  Example of LIB$FIND_IMAGE_SYMBOL call.
//  Dynamic image activation...
*/
#include    <descrip.h>
#include    <lib$routines.h>
#include    <ssdef.h>
#include    <stsdef.h>
main()
    {
    int retstat;
    int *addr;
    extern int bogus;
    $DESCRIPTOR( self, "SYS$DISK:[]FIS.EXE" );
    $DESCRIPTOR( target, "BOGUS" );
    retstat = LIB$FIND_IMAGE_SYMBOL( &self, &target, &addr, &self );
    return SS$_NORMAL;
    }
