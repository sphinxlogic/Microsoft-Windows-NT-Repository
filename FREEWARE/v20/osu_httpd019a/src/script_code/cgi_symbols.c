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
 * Revised: 16-MAY-1995		Support multiple occureces of form values.
 *				If value appears more than once, concatenate
 *				values and separate by commas.
 * Revised: 8-SEP-1995		Fix bug in handling of plus signs in input
 *				fields.  To revert to old behaviour compile
 *				with KEEP_PLUSES macro symbol defined.
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
/* Search comma-delimited list of names (namlist) for specified name
 * and add to end of list if not found.  If found, fetch value of DCL symbol
 * given by prefix+symname and return that to caller.
 *
 * Return values:
 *    0		Name too large to add to list,	vallen set to 0.
 *    1		Name appended to end of list, vallen set to 0.
 *    2		Name found, symbol value stored symval, length in vallen.
 */
static int add_symbol ( char *prefix, char *symname, char *namlist, 
	int *nl_len, char *symval, int *vallen )
{
    int i,j,k, status, prefix_len, LIB$GET_SYMBOL(), table;
    /*
     * Search list from back.
     */
    *vallen = 0;
    for ( i = *nl_len-1; i >= 0; --i ) {
	if ( (i == 0) || namlist[i] == ',' ) {
	    /*
	     * namlist[i] or namlist[i+1] is start of name.
	     */
	    j = (i==0) ? i : i+1;
	    for ( k = 0; symname[k] != '='; k++ ) {
		if ( symname[k] != namlist[j] ) break;
		j++;
	    }
	    if ( (symname[k]=='=') && (namlist[j] == ',' || namlist[j] == '\0') ) {
		/* 
		 * found match, retrieve DCL symbol
		 */
		$DESCRIPTOR(symbol,"");
		$DESCRIPTOR(value,"");
		char prefixed_sym[256];
		/* Build symbol name from prefix */
		prefix_len = strlen(prefix);
 		symbol.dsc$w_length = k + prefix_len;
		if ( symbol.dsc$w_length > 255 ) symbol.dsc$w_length = 255;
		strncpy ( prefixed_sym, prefix, symbol.dsc$w_length );
		strncpy ( &prefixed_sym[prefix_len], symname,	
			symbol.dsc$w_length - prefix_len);
		symbol.dsc$a_pointer = prefixed_sym;

		/*
		 * Make descriptor for result and fetch.
		 */
		value.dsc$w_length = 255;
		value.dsc$a_pointer = symval;

		status = LIB$GET_SYMBOL ( &symbol, &value, vallen, &table );
		if ( (status&1) == 0 ) *vallen = 0;
		return 2;
	    }
	}
    }
    /*
     * No match, append name to namlist.
     */
    i = *nl_len;
    if ( i > 0 && i < 255 ) namlist[i++] = ',';
    for ( j = 0; symname[j] && symname[j] != '='; j++ ) if ( i < 255 ) {
	namlist[i++] = symname[j];
    } else return 0;

    *nl_len = i;
    return 1;
}
/***************************************************************************/
/* Generate list of symbols for FORM input.
 */
static void set_form_symbols ( char *prefix )
{
    char *var, *fdata;
    int status, table, i, j, k, vallen;
    int content_length, slist_len, length, prefix_len, LIB$SET_SYMBOL();
    $DESCRIPTOR(symbol,"");
    $DESCRIPTOR(value,"");
    char sym_list[256], symname[256], symval[256];
    /*
     * Initialize
     */
    strcpy ( symname, prefix );
    strcat ( symname, "FLD_" );
    prefix_len = strlen ( symname );
    table = LIB$K_CLI_LOCAL_SYM;
    symbol.dsc$a_pointer = symname;
    slist_len = 0;
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
#ifndef KEEP_PLUSES
	    for ( j = start; j < i; j++ ) 
		if ( fdata[j] == '+' ) fdata[j] = ' ';
#endif
	    net_unescape_string ( &fdata[start], &flen );
	    finish = start + flen;
	    for ( j = start; j < finish; j++ ) if ( fdata[j] == '=' ) {
		/*
		 * Append name to field list or retrieve current DCL symbol
		 * value if already defined once.
		 */
		symname[prefix_len] = '\0';
		k = add_symbol ( symname, &fdata[start], sym_list, &slist_len,
			symval, &vallen );
		/*
		 * Make DCL symbol.  Construct symbol name from prefix
		 * and portion before =, value is portion after.
		 */
 		symbol.dsc$w_length = j - start + prefix_len;
		if ( symbol.dsc$w_length > 255 ) symbol.dsc$w_length = 255;
		strncpy ( &symname[prefix_len], &fdata[start],	
			symbol.dsc$w_length );

		if ( j < finish ) if ( (vallen + finish-j) < 254 ) {
		    /* Append new value to string */
		    if ( vallen > 0 ) symval[vallen++] = ',';
		    strncpy(&symval[vallen], &fdata[j+1], finish-j-1 );
		    vallen += (finish-j-1);
		}
		value.dsc$w_length = vallen;
		value.dsc$a_pointer = symval;

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
