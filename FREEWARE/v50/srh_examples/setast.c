/*
 *  AST and $SETAST demo
 *  raise the AST shields
 *  request an AST, parameter is 1
 *  request an AST, parameter is 2
 *  lower the shields
 *  <bing1><bing2>
 */
main()
    {
    int retstat = 0;
    int bogus();
    int SYS$SETAST();
    int SYS$DCLAST();

    printf("\ndisabling\n");
    /*
     * $SETAST() returns SS$_WASSET and SS$_WASCLR depending
     * on the previous setting of the AST shield.  Watch out,
     * SS$_WASSET looks like a SUCCESSFUL SS$_ACCVIO.  (ie:
     * a debug EXAMINE/COND shows SS$_WASSET as the error
     * %SYSTEM-S-ACCVIO.  *Real* ACCVIO's never have the "-S-"
     *  code!)
     */
    retstat = SYS$SETAST( 0 );
    printf("\n  disable/ was: %d\n", retstat );

    retstat = SYS$DCLAST( bogus, 1, 0 );
    retstat = SYS$DCLAST( bogus, 2, 0 );
    printf("\ndclast %x\n", retstat );

    printf("\nenabling\n" );
    retstat = SYS$SETAST( 1 );

    /*
     *  and, since we just lowered the shields, the ASTs should hit
     *  in here somewhere....
     */
    printf("\n  enable/ was: %d\n", retstat );

    return( 1 );
    };

/*
 *  and, here's the entire, sophisticated, twisted AST code...
 */
bogus( astprm )
int astprm;
    {
    printf("\nAST tripped.  ast parameter was 0x%x\n\n", astprm);
    return( 1 );
    };
