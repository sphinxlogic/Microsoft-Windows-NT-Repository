/*
**  lib$lookup_key.c
**
**  An example call to the lib$lookup_key.c routine.
**
**  build procedure:
**
**	$ cc lib$lookup_key
**	$ link lib$lookup_key,sys$input/option
**	sys$share:vaxcrtl/share
**	$ run lib$lookup_key
**
**  Author:
**
**	Stephen (XDELTA::) Hoffman
**	Digital Equipment Corporation
*/
/*
**  The ASCIC macro converts an ASCIZ format to ASCIC format.  
*/
#define ASCIZ_TO_ASCIC( string, constant ) \
  string[0] = (char) strlen( constant ); \
  memmove( &((string)[1]), constant, strlen( constant ) ); \

#include <descrip.h>
#include <ssdef.h>
#include <stdio.h>
#include <string.h>
#include <stsdef.h>

#define MAXSEARCHLEN   128
#define MAXKEYLEN   4+1	/* Max ASCII length, plus the Count Byte */
#define NUMKEYS	    4

key_test()
    {
    unsigned long int retstat;
    char searchbuf[MAXSEARCHLEN];
    struct dsc$descriptor searchdsc;
    char matchbuf[MAXSEARCHLEN];
    struct dsc$descriptor matchdsc;
    $DESCRIPTOR( promptdsc, "SEARCH> ");
    $DESCRIPTOR( commentdsc, "The key matched was:");
    char blakey[MAXKEYLEN];
    char ownkey[MAXKEYLEN];
    char inikey[MAXKEYLEN];
    char hlpkey[MAXKEYLEN];
    struct
	{
	unsigned long int keycount;
	struct
	    {
	    char *keyword;
	    unsigned long int keyval;
	    } kw[NUMKEYS];
	} keytable;
    unsigned long int matchval;
    unsigned short int matchlen;
    unsigned long int lib$lookup_key(
	struct dsc$descriptor *, 
	unsigned char *,
	unsigned long int *,
	struct dsc$descriptor *,
	unsigned short int * );

    /*
    **	Convert the ASCIZ keyword strings...
    */
    ASCIZ_TO_ASCIC( blakey, "    " );
    ASCIZ_TO_ASCIC( ownkey, "OWN" );
    ASCIZ_TO_ASCIC( inikey, "INIT" );
    ASCIZ_TO_ASCIC( hlpkey, "HELP" );

    /*
    **	And initialize the keyword table.
    */
    keytable.keycount = NUMKEYS * 2;
    keytable.kw[0].keyword = blakey;
    keytable.kw[0].keyval = 100;
    keytable.kw[1].keyword = hlpkey;
    keytable.kw[1].keyval = 101;
    keytable.kw[2].keyword = inikey;
    keytable.kw[2].keyval = 102;
    keytable.kw[3].keyword = ownkey;
    keytable.kw[3].keyval = 103;

    /*
    **	Build one of the string descriptors manually.  (The other
    **	descriptors are built by the $DESCRIPTOR macro, above.)
    **	Then go beg the user for the command.  (We actually write
    **	the new length into the existing descriptor -- effective,
    **	but not the best practice.
    */
    searchdsc.dsc$w_length = MAXSEARCHLEN;
    searchdsc.dsc$b_class = DSC$K_CLASS_S;
    searchdsc.dsc$b_dtype = DSC$K_DTYPE_T;
    searchdsc.dsc$a_pointer = searchbuf;
    retstat = lib$get_input( &searchdsc, &promptdsc, &searchdsc.dsc$w_length );
    if ( !$VMS_STATUS_SUCCESS( retstat ))
	return retstat;

    /*
    **	lib$lookup_key, like c, is case sensitive.  Force the user search
    **	input into upper case.
    */
    retstat = str$upcase( &searchdsc, &searchdsc );
    if ( !$VMS_STATUS_SUCCESS( retstat ))
	return retstat;

    /*
    **	Build the key match buffer descriptor manually, too.  Then
    **	perform the lookup.
    */
    matchdsc.dsc$w_length = MAXSEARCHLEN;
    matchdsc.dsc$b_class = DSC$K_CLASS_S;
    matchdsc.dsc$b_dtype = DSC$K_DTYPE_T;
    matchdsc.dsc$a_pointer = matchbuf;
    retstat = lib$lookup_key( &searchdsc, &keytable, &matchval,
	&matchdsc, &matchdsc.dsc$w_length );
    if ( !$VMS_STATUS_SUCCESS( retstat ))
	return retstat;

    /*
    **	Now display the results.  And if successful, exit normally.
    */
    retstat = lib$put_output( &commentdsc );
    if ( !$VMS_STATUS_SUCCESS( retstat ))
	return retstat;
    retstat = lib$put_output( &matchdsc );
    if ( !$VMS_STATUS_SUCCESS( retstat ))
	return retstat;

    return SS$_NORMAL;
    }
