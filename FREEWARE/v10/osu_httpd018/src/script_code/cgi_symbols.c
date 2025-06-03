/*
 * This program is intended to be run by a HTTP server script in order to
 * convert the CGILIB environment to CLI symbols.  An optional argument
 * is a prefix string for the created symbols (default="WWW_").
 *
 * The HTTP request information is obtained by getting the values of P1,
 * P2, and P3.  The script running this program should not modify these
 * symbols.
 *
 * This program puts the link to the HTTP server into CGI mode and flags
 * the environment so that WWWEXEC will automatically send the CGI terminator
 * when the DCL script exits.  Note that in the CGI mode, the writes to
 * net_link must explicitly include the carriage control.  The first line
 * output must be either and content-type: header or a location: header
 * followed by a blank line (newline sequence).
 *
 * If (and only if) you specify a form prefix as argument 2 on the command 
 * line, the program interprets the request contents as form input,creating a 
 * series of symbols for it. The symbols created will be (xxx is prefix):
 *
 *	xxxFIELDS		Comma-separated list of field names parsed
 *				from form input.  For every name in list
 *				a DCL symbol of form xxxFLD_name will be
 *				created.  Note that the form writer must
 *				garantee that the field names will result in
 *				valid DCL symbol names (e.g. no hyphens).
 *				This list is truncated to 255 characters.
 *
 *	xxxFLD_yyyy		Field value of field yyyy parsed from form
 *				input.  Value is truncated to 255 characters.
 *
 * Examples:
 *	$ run cgi_symbols
 *
 *	$ mcr sys$disk:[]cgi_symbols http_ cgiform_
 *
 * Author: David Jones
 * Date:   26-SEP-1994
 * Revised: 25-OCT-1994		Add option to interpret form input into symbols.
 * Revised: 28-OCT-1994		Bug fixes.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <descrip.h>
#include <libclidef.h>

#include "scriptlib.h"
#include "cgilib.h"
int cgi_show_env();
static void set_form_symbols ( char *prefix );

int main ( int argc, char **argv )
{
    int i, status, LIB$GET_SYMBOL(), table, length, virtual_argc;
    char *virtual_argv[4], *base_prefix, *form_prefix;
    char param_name[4], param_value[256];
    $DESCRIPTOR(pname,"");
    $DESCRIPTOR(pvalue,"");
    /*
     * Check for invalid invocation
     */
    if ( argc > 3 ) {
	printf(
	   "Too many arguments, this program must be run from a DCL script\n");
	exit (20);
    }
    /*
     * Build dummy argument list from P1 through P3 to get the values WWWEXEC 
     * passed to the script.
     */
    virtual_argc = 4;
    virtual_argv[0] = argv[0];		/* for lack of anything better! */
    pname.dsc$w_length = 2;
    pname.dsc$a_pointer = param_name;
    pvalue.dsc$w_length = sizeof(param_value)-1;
    pvalue.dsc$a_pointer = param_value;

    for ( i = 1; i < virtual_argc; i++ ) {
	sprintf ( param_name, "P%d", i );
	length = 0;
	status = LIB$GET_SYMBOL ( &pname, &pvalue, &length );
	if ( (status&1) == 1 ) {
	    /*
	     * Allocate new buffer to hold value.
	     */
	    virtual_argv[i] = malloc ( length+1 );
	    param_value[length] = '\0';
	    strcpy ( virtual_argv[i], param_value );
	}
	else virtual_argv[i] = "";
    }
    /*
     * Load CGI environment and convert to DCL
     */
    status = cgi_init ( virtual_argc, virtual_argv );

    base_prefix = argc > 1 ? argv[1] : "";
    if ( !*base_prefix ) base_prefix = "WWW_";
    cgi_set_dcl_env ( base_prefix );
    /*
     * Check if form input parse wanted.
     */
    form_prefix = argc > 2 ? argv[2] : "";
    if ( *form_prefix ) set_form_symbols ( form_prefix );
    return 1;
}
/***************************************************************************/
/* Generate list of symbols
 */
static void set_form_symbols ( char *prefix )
{
    char *var, *fdata;
    int status, table, i, virt_slist_len, j;
    int content_length, slist_len, length, prefix_len, LIB$SET_SYMBOL();
    $DESCRIPTOR(symbol,"");
    $DESCRIPTOR(value,"");
    char sym_list[256], symname[256];
    /*
     * Initialize
     */
    strcpy ( symname, prefix );
    strcat ( symname, "FLD_" );
    prefix_len = strlen ( symname );
    table = LIB$K_CLI_LOCAL_SYM;
    symbol.dsc$a_pointer = symname;
    slist_len = virt_slist_len = 0;
    /*
     * See if any content present first checking for POST data and using
     * query string as fallback.
     */
    var = cgi_info ( "CONTENT_LENGTH" );
    content_length = var ? atoi(var) : 0;
    
    if ( content_length > 0 ) {
	/*
	 * Allocate buffer and read entire form data into it, forcing final &.
	 */
	fdata = malloc ( content_length+1 );
	if ( !fdata ) return;
	
	length = cgi_read ( fdata, content_length );
    } else {
	var = cgi_info ( "QUERY_STRING" );
	if ( var ) {
	    length = strlen ( var );
	    fdata = malloc ( length + 1 );
	    if ( !fdata ) return;
	    strcpy ( fdata, var );
	} else length = 0;
    }
    if ( length > 0 ) {
	int start, finish, flen;
	/*
	 * Parse the data.
	 */
	if ( fdata[length-1] != '&' ) fdata[length++] = '&';
	start = 0;
	finish = 0;
	for ( i = 0; i < length; i++ ) if ( !fdata[i] || (fdata[i] == '&') ) {
	    /*
	     * Value parsed.  Unescape characters and look for first '='
	     * to delimit field name from value.
	     */
	    flen = i - start;
	    net_unescape_string ( &fdata[start], &flen );
	    finish = start + flen;
	    for ( j = start; j < finish; j++ ) if ( fdata[j] == '=' ) {
		/*
		 * Append name to field list.  A leading comma is included
		 * in the virtual list but not in the actual list, hence the
		 * - 1.
		 */
		virt_slist_len += j - start + 1;
		if ( virt_slist_len < 256 ) {
		    if ( slist_len > 0 ) sym_list[slist_len++] = ',';
		    strncpy ( &sym_list[slist_len], &fdata[start], j-start );
		    slist_len = virt_slist_len - 1;
		}
		/*
		 * Make DCL symbol.  Construct symbol name from prefix
		 * and portion before =, value is portion after.
		 */
		symbol.dsc$w_length = j - start + prefix_len;
		if ( symbol.dsc$w_length > 255 ) symbol.dsc$w_length = 255;
		strncpy ( &symname[prefix_len], &fdata[start],	j - start );
		value.dsc$w_length = finish - j - 1;
		if ( j >= finish ) value.dsc$w_length = 0;
		if ( value.dsc$w_length > 255 ) value.dsc$w_length = 255;
		value.dsc$a_pointer = &fdata[j+1];

		status = LIB$SET_SYMBOL ( &symbol, &value, &table );
		if ( (status&1) == 0 ) fprintf(stderr,
			"Error defining CGI form symbol: %d\n", status );
		break;
	    } else {
		/* make field name upcase */
		fdata[j] = _toupper(fdata[j]);
		if ( fdata[j] == '-' ) fdata[j] = '_';
	    }
	    start = i+1;
	}
    }
    /*
     * Set final symbol, which is list of fields.
     */
    prefix_len = strlen ( prefix );
    strcpy ( &symname[prefix_len], "FIELDS" );
    symbol.dsc$w_length = strlen ( symname );
    value.dsc$w_length = slist_len;
    value.dsc$a_pointer = sym_list;
    status = LIB$SET_SYMBOL ( &symbol, &value, &table );
}
