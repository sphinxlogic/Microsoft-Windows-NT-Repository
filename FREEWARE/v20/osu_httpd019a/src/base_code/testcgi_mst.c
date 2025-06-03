/* This shareable image module provides a bare-bones example of a
 * dynamically loaded MST-based scriptserver.  Use the following rules
 * in the configuration file to load this module and bind and exec path
 * to it:
 *
 *    ThreadPool test stack=60000 q_flag=1 limit=4
 *    Service testsrv pool=test dynamic=(testcgi,testcgi_mst)
 *    Exec /$testcgi/* %testsrv:/myarg
 *
 * The ThreadPool rule defines a pool of MSTs (message-based service
 * threads) for servicing requests.  The service rule defines the
 * service 'testsrv', assigning it to pool 'test' and setting it's
 * MST start routine as 'testcgi' in image testcgi_mst (image is assumed
 * to be in directory WWW_SYSTEM).  The Exec rule makes it so that URL paths 
 * beginning with '/$testcgi/' will invoke the testcgi scriptserver,
 * the percent and colon are a special syntax to indicate an MST-based
 * script rather than a DECnet-based script.
 *
 * Author: David Jones
 * Date:   10-JUN-1995
 */
#include "mst_share.h"
#include "tutil.h"
#include <stdio.h>
/***************************************************************************/
/* Every dynamically loaded service must have an INIT routine, which is
 * called once during the processing of the Service rule to initialize
 * static structures needed by the MST.  By default the init routine
 * name is the start routine + the string _init, and 'init=name' clause
 * on the service rule will override the default.
 *
 * Arguments:
 *    mst_linkage vector  Structure containing addresses of essential
 *			items wee need from main program, such as
 *			address of putlog() routine.
 *
 *    char *info	Administrator provided argument (info=).
 *
 *    char *errmsg	Error text for log file when error status returned.
 */
int testcgi_init ( mst_linkage vector, char *info, char *errmsg )
{
   /*
    * The first thing any init routine must do is call mstshr_init() to
    * initialize global variables declared in mst_share.h.  The callback
    * vector includes a version number which mstshr_init checks to make
    * sure the vector format being used by the server hasn't changed.
    * If mstshr_init fails, we can't proceed so return immediately.
    */
   if ( (1&mstshr_init ( vector, info, errmsg )) == 0 ) return 0;
   /*
    * Now do any initialization specific to this shareable image.
    * The info argument is an optional string that can be specified
    * on the service rule using 'info=string'
    *
.   * Any errors should place a text message describing the error in errmsg (up 
    * to 255 characters) and return an even (i.i. error) status code.
    *
    * Note that if there is nothing to be done we can dispense with this
    * routine entirely by specifying 'init=mstshr_init' on the Service
    * definition.
    */

    /*
     * Return success status.
     */
    tu_strcpy ( errmsg, "testcgi scriptserver sucessfully initialized" );
    return 1;
}
/***************************************************************************/
/* Main routine to handle client requests.  To the server, this routine
 * must behave like a DECnet scriptserver task (e.g. WWWEXEC) only messages 
 * are transferred via mst_read() and mst_write() rather than $QIO's to a 
 * logical link.
 *
 * Arguments:
 *    mst_link_t link	Connection structure used by mst_read(), mst_write().
 *
 *    char *service	Service name (for logging purposes).
 *
 *    char *info	Script-directory argument from 'exec' rule that
 *			triggered the MST (exec /path/* %service:info).
 *
 *    int ndx		Service thread index, all services sharing same pool
 *			share the same thread index numbers.
 *
 *    int avail		Number of remaining threads in pool assigned to service.
 */
int testcgi ( mst_link_t link, char *service, char *info, int ndx, int avail )
{
    int i, status, length;
    char line[512];
    struct mstshr_envbuf env;
    /*
     * Log that we began execution
     */
    if ( http_log_level > 0 ) tlog_putlog ( 1, 
	"Service!AZ/!SL connected, info: '!AZ', pool remaining: !SL!/", 
	service, ndx, info, avail );
    /*
     * Setup cgi environment (reads prologue as a consequence).
     */
    status = mstshr_cgi_symbols ( link, info, &env );
    if ( (status &1) == 0 ) return 0;
    /*
     * Place connection in TEXT mode since that is simplest to deal with.
     */
    status = mst_write ( link, "<DNETTEXT>", 10, &length );
    if ( (status &1) == 0 ) return 0;

    status = mst_write ( link, "200 dummping env array", 10, &length );
    if ( (status &1) == 0 ) return 0;
    /*
     * Dump the scriptserver prologue back to the client.
     */
    for ( i = 0; i < 4; i++ ) {
	status = mst_write 
		(link, env.prolog[i], tu_strlen(env.prolog[i]), &length);
        if ( (status &1) == 0 ) break;
    }
    /*
     * Dump the 'environment' variables loaded by mstshr_cgi_symbols.
     */
    for ( i = 0; i < env.count; i++ ) {
	status = mst_write (link, env.ptr[i], tu_strlen(env.ptr[i]), &length);
        if ( (status &1) == 0 ) break;
    }
    /*
     * Cleanly shutdown connection and return.
     */
    status = mst_write ( link, "</DNETTEXT>", 11, &length );
    if ( (status &1) == 0 ) return 0;

    mst_close ( link );
    
    return 1;
}
