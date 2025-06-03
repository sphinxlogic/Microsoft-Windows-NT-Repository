/*
 * The http_max_server_clients function computes the maximum number of
 * current clients that the current process can safely.  This number
 * is the minimum of:
 *
 *     quota   	limit				Observed usage
 *     bytlm    (bytlm-500) / 4000		2200 bytes/thread.
 *     fillm	(fillm-4) / 2			1 file/thread.
 *     biolm	(biolm-4) / 2			1 I/O / thread.
 *     diolm	(diolm-4) / 2			1 I/O / thread.
 *     astlm	(astlm-6) / 2			1 ast / thread.
 *     pgfil	(pgfil-3000) / 200		 <100 pages /thread
 *
 * The is_sysgrp function checks if current process uic will implcitly
 * grants sysprv to the process (UIC group number less than sysgen parameter
 * SYSGRP.
 *
 * Revised: 2-sep-1995		write messages to log file rather than printf.
 */     

#include <stdio.h>
#include <jpidef.h>
#include <syidef.h>
#ifdef VAXC
#ifndef __ALPHA
#pragma builtins
#endif
#endif

#define LIMIT_MESSAGE "Process %s limits server to %d client threads.\n"
#define LIMIT_MESSAGE2 "Process !AZ limits server to !SL client threads.!/"

int tlog_putlog(int,char *,...);

int http_max_server_clients ( int user_limit, int cache_size )
{
    int bytlm, fillm, biolm, diolm, astlm, pgfil, limit, test_limit;
    int LIB$GETJPI();
    /*
     * Get the current quotas.
     */

    LIB$GETJPI ( &JPI$_BYTLM, 0, 0, &bytlm );
    LIB$GETJPI ( &JPI$_BIOLM, 0, 0, &biolm );
    LIB$GETJPI ( &JPI$_DIOLM, 0, 0, &diolm );
    LIB$GETJPI ( &JPI$_FILLM, 0, 0, &fillm );
    LIB$GETJPI ( &JPI$_ASTLM, 0, 0, &astlm );
    LIB$GETJPI ( &JPI$_PGFLQUOTA, 0, 0, &pgfil );
    /*
     * Restrict limit.
     */
    limit = user_limit;
    test_limit = (bytlm-500) / 4000;
    if ( test_limit < user_limit ) {
	if ( test_limit < limit ) limit = test_limit;
	tlog_putlog( 0,LIMIT_MESSAGE2, "bytlm", test_limit );
    }

    test_limit = (fillm-4) / 2;
    if ( test_limit < user_limit ) {
	if ( test_limit < limit ) limit = test_limit;
	tlog_putlog(0, LIMIT_MESSAGE2, "fillm", test_limit );
    }                                                             

    test_limit = (biolm-4) / 2;
    if ( test_limit < user_limit ) {
	if ( test_limit < limit ) limit = test_limit;
	tlog_putlog(0, LIMIT_MESSAGE2, "biolm", test_limit );
    }

    test_limit = (diolm-4) / 2;
    if ( test_limit < user_limit ) {
	if ( test_limit < limit ) limit = test_limit;
	tlog_putlog(0, LIMIT_MESSAGE2, "diolm", test_limit );
    }

    test_limit = (astlm-6) / 2;
    if ( test_limit < user_limit ) {
	if ( test_limit < limit ) limit = test_limit;
	tlog_putlog(0, LIMIT_MESSAGE2, "astlm", test_limit );
    }

    test_limit = (pgfil-(3000 + (cache_size*9))) / 200;
    if ( test_limit < user_limit ) {
	if ( test_limit < limit ) limit = test_limit;
	tlog_putlog(0, LIMIT_MESSAGE2, "pagefile quota", test_limit );
    }

    return limit;
}
/**************************************************************************/
/* Return 1 if process uic is less than sysgen parameter. */
int is_sysgrp()
{
    int uic_grp, maxsysgrp, LIB$GETJPI(), LIB$GETSYI(), status;

    status = LIB$GETJPI ( &JPI$_GRP, 0, 0, &uic_grp );
    if ( (status&1) == 0 ) printf ( "Error in getjpi: %d\n", status );

    status = LIB$GETSYI ( &SYI$_MAXSYSGROUP, &maxsysgrp );
    if ( (status&1) == 0 ) printf ( "Error in getsyi: %d\n", status );
    return (uic_grp <= maxsysgrp ) ? 1 : 0;
}
