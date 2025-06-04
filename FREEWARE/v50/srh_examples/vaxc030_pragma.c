#pragma builtins

#include <ssdef.h>
#include <stdio.h>
#include <string.h>

main()
    {
    char instring[] = "FourScoreAndSeven";
    char outstring1[100];
    char outstring2[100];
    unsigned int bytcnt;

    bytcnt = strlen( instring ) + 1;
    _MOVC3( bytcnt, instring, outstring1 );
    printf( "%s\n", outstring1 );
    
    return( SS$_NORMAL );
    }
