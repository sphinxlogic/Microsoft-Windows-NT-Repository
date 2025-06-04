/*  DSBD.C  Days Since Base Date
/*  Sets a DCL symbol to the number of days since the system base date.
/*  Used by the operator backup procedure to select the correct tape
/*  set.
/*
/*  30-Aug-1989	Hoffman
/*  Needed a second bias: account for the day of the week of the
/*  VMS system base date -- it was a Wednesday.
*/
#include <descrip.h>
#include <ssdef.h>
#include <stsdef.h>
#define LIB$K_CLI_GLOBAL_SYM	2
#define WEDNESDAY		4   /* 17-Nov-1858 was a Wednesday */

/*
/*  Number of tape sets kept and the weeks-since-base-date bias
*/
#define MAXTAPESET		2   /* number of sets */
#define WEEKLYBIAS		1   /* week 1 uses tapeset B */


main()
    {
    int retstat;
    int LIB$SET_SYMBOL();
    int LIB$DAY();
    int dayssincebasedate;
    char days_s[9];
    char tapes_s[2];
    struct dsc$descriptor days_d =
	{ 8, DSC$K_DTYPE_T, DSC$K_CLASS_S, days_s };
    struct dsc$descriptor tapes_d =
	{ 1, DSC$K_DTYPE_T, DSC$K_CLASS_S, tapes_s };
    $DESCRIPTOR( dsbd, "VPG$DAYSSINCEBASEDATE" );
    $DESCRIPTOR( bts, "VPG$BACKUPTAPESET" );

    retstat = LIB$DAY( &dayssincebasedate );
    if (!$VMS_STATUS_SUCCESS( retstat )) return( retstat );

    /*
    /* convert the days since base-date to a string and convert
    /* the days since base-date into the weekly tape set code.
    /* Take the tapeset weekly bias and the fact that the base
    /* date was not a Sunday but a Wednesday into account.
    */
    sprintf( days_s, "%8.8d", dayssincebasedate );
    sprintf( tapes_s, "%c",
	(int) 'A' +
	(((
	(dayssincebasedate - WEDNESDAY) / 7) + WEEKLYBIAS)
	% MAXTAPESET
	) );

    /*
    /*	Set a couple of DCL symbols for the BACKUP command procedure.
    */
    retstat = LIB$SET_SYMBOL( &dsbd, &days_d, &LIB$K_CLI_GLOBAL_SYM );
    if (!$VMS_STATUS_SUCCESS( retstat )) return( retstat );
    retstat = LIB$SET_SYMBOL( &bts, &tapes_d, &LIB$K_CLI_GLOBAL_SYM );
    if (!$VMS_STATUS_SUCCESS( retstat )) return( retstat );

    return( SS$_NORMAL );
    }
