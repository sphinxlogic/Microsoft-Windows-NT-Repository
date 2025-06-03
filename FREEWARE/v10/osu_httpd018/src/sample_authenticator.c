/*
 * Test authenticator for verifying authlib routines.
 *
 * Author:	David Jones
 * Date:	29-MAY-1994
 * Revised:	 3-FEB-1995		Additional comments.
 * Revised:	23-FEB-1995		Use auth_callback prototype.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "authlib.h"
auth_callback simple;
/*
 * Main routine.
 */
int main ( int argc, char **argv )
{
    int status;
    /*
     * Initialize data structures needed to perform authentication.  For
     * this sample implentation, there are none.
     */

    /*
     * Process server requests.  The auth_server routine connects to the
     * the HTTP server that created us and effecitively makes the authenticate
     * function (simple()) a remote procedure call for the server.
     * The third argument is text to be placed in the server's log file.
     */
    status = auth_server ( argc, argv, 
	"Sample Authenticator, V 1.0, initialized successfully.", 1, simple);
    /*
     * Getting to this point means an error occurred since auth_server
     * loops to continuous get authentication requests.  Most likely the
     * the HTTP server exitted so we should run down as well.  Perform any
     * necessary cleanup and exit.
     */
    exit ( status );
}
/************************************************************************/
/*
 * Dummy password checking routine.  Assume the password is a simple
 * function of the username.
 */
static int check_password ( char *username, char *password )
{
    char *u, *p;
    int delta;
    /*
     * Starting with second username character, find delta between it and
     * the previous character and check that next password character is
     * this delta + 65 (BBD in username yields AB).
     */
    for ( u = username, p = password; *u; u++ ) if ( u != username ) {
	delta = u[-1] - u[0]; if ( delta < 0 ) delta = 0 - delta;
	if ( *p++ != (delta+65) ) return 0;
    }
    /*
     * Final character in password is length of input string + 65 (i.e.
     * for an 6 character username, last password char is 'G').
     */
    if ( *p++ != (strlen(username)+65)) return 0;
    if ( *p ) return 0;				/* chars left in password. */
    else return 1;
}
/*************************************************************************/
/* Authenticator callback routine for auth_server().  This routine is
 * called to process an authentication request sent by the server.
 *
 * This routine determines whether access to the requested object (ident)
 * with the specified method is to be granted, validating the authorization
 * information against the protections defined by the setup file.
 *
 * return values:
 *	0	Access to the object is denied.
 *	1	Access to the object is permitted.
 *
 * When access is denied, this routine sets the response argument to point
 * to a statically allocated string containing the HTTP error response headers
 * to send to the client, sans the HTTP version (i.e. "403 Access denied\r\n").
 * The newlines are included.  If a 401 status is returned, the response header
 * includes the WWW-authenticate headers for the authorization schemes that
 * this routine understands.
 */
int simple ( 
	int local_port, 		/* Local port # of TCP/IP connection */
	int remote_port, 		/* Remote TCP/IP port of client */
	unsigned char remote_address[4],/* Remote binary IP address of client */
	char *method, 			/* Requested method (GET, PUT, etc); */
	char *setup_file, 		/* Protection setup file name */
	char *ident, 			/* Ident portion of translated URL */
	char *authorization, 		/* Auth. headers included in request */
	char **response, 		/* HTTP error response headers */
	char **log )			/* Optional log message */
{
    int status, auth_basic_authorization();
    char username[40], password[40];
    /*
     * Note that response and log buffers must be statically allocated.
     * Fail and succeed are pointers to string constants (implicitly static),
     * if you make them char arrays be to all make them static storage class.
     */
    static char log_message[200];
    char *fail = 
	"401 Unauthorized\r\nWWW-Authenticate: Basic realm=\"sample\"\r\n";
    char *succeed = "200 access to protected file granted\r\n";
    /*
     * At this point, a real authenticator would read the protection setup
     * file to locate the databases (password file, group file) that define
     * the protections for the object.  If this operation failed, we would
     * return failure (0) with a "500" error status line.
     */

    /*
     * Set default response to the "unauthorized" error response headers string
     */
    *response = fail;
    /*
     * See if request has a basic scheme authorization line and decode it
     * using utility routines from authlib.c.  The last argument in the
     * auth_basic_authorization() call forces the result to upper case.
     */
    if ( auth_basic_authorization ( authorization, username, sizeof(username),
		password, sizeof(password), 1 ) ) {
	/*
	 * Validate password using simple rule.  A real authenticator would
	 * validate it against the password database.
	 */
	int allowed = check_password ( username, password );
	if ( allowed ) *response = succeed;
	/*
	 * Generate a message for the server's log file.  A real authenticator
	 * wouldn't place the password in the log file.
	 *
	 * The server checks if the log line begins with "[username]" and
	 * makes 'username' the local user in the access.log file.
	 */
	*log = log_message;
	sprintf(log_message,"[%s]Authentication for user='%s' pass='%s' %s.",
		allowed ? username : "", username, password, allowed ? 
		"succeeded" : "failed" );
	*log = log_message;
	return allowed;
    }
    return 0;
}
