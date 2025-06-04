#include <descrip.h>
set_logical()
MAIN_PROGRAM
    {
    $DESCRIPTOR( lognam_d, "TEST_LOGICAL_NAME" );
    $DESCRIPTOR( logval_d, "TEST_LOGICAL_VALUE" );
    return lib$set_logical( &lognam_d, &logval_d );
    }
