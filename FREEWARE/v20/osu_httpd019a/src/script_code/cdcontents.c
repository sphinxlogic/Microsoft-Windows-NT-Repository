/*
 * This program is a CGI script for converting it's argument, a DEC CONDIST
 * CDMENU data file to HTML format.  The argument is the portion of the
 * path string following the script name in the URL (i.e. PATH_INFO).
 *
 * Author:	David Jones
 * Date:	15-SEP-1994
 * Revised:	28-OCT-1994
 * Revised:	21-JUN-1995	Remove '\r' from format lines as
 *				cgi_printf already converts the '\n'.
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include "cgilib.h"

static FILE *cdmf;
int cdm_init(), cdm_product_scan();
char *net_unescape_string ( char *string, int *length );

static void upcase_string ( char *dst, char *src, int maxlen )
{
    int i;
    for ( i = 0; (i < maxlen) && src[i]; i++ ) {
	dst[i] = _toupper(src[i]);
    }
    dst[i] = '\0';
}

int main ( int argc, char **argv ) {
    int status, count, i, ver, upi, state, kit, disc, matches;
    char *prod_status, *method;
    char *query_string, *menu_file, *rel_date, *headers[5], line[2048];
    char test_line[2048], test_kit[128], test_ver[128], match_word[20][128];
    /*
     * Set up CGI environment.
     */
    status = cgi_init ( argc, argv );
    if ( (status&1) == 0 ) exit ( status );
    /*
     * Get requested method and make sure we can handle it.
     */
    method = cgi_info ( "REQUEST_METHOD" );
    if ( !method ) method = "";
    if ( strcmp(method, "GET") && strcmp(method,"HEAD") ) {
	cgi_printf("status: 400 Bad request\n" );
	cgi_printf("content-type: text/plain\n\n" );
	cgi_printf("Method is not supported (%s).\n", method );
	exit(1);
    }
    /*
     * Get filename.
     */
    menu_file = cgi_info ( "PATH_TRANSLATED" );
    printf("menu_file: '%s'\n", menu_file ? menu_file : "<NULL>" );
    if ( !menu_file ) menu_file = "";
    if ( !(*menu_file) ) {
	cgi_printf("status: 403 forbidden\n" );
	cgi_printf("content-type: text/plain\n\n" );
	cgi_printf("Access to menu file disallowed by rule file\n");
	exit(1);
    }
    /*
     * Open input file and read headers.
     */
    status = cdm_init ( menu_file, headers, &rel_date );
    if ( 0 == (1&status) ) {
	printf("error in cdm_init: %d\n", status );
	cgi_printf("status: 404 file not found\n" );
	cgi_printf("content-type: text/plain\n\n" );
	cgi_printf("Error opening menu file (%d)\n", status);
	exit ( 1 ); 
    } else if ( !headers[0] ) {
	cgi_printf("status: 500 invalid file format.\n" );
	cgi_printf("content-type: text/plain\n\n" );
	cgi_printf("File is not in valid CD_CONTENTS format.\n");
	cgi_printf("(No product_name headers)");
	exit ( 1 ); 
    }
    if ( 0 == strcmp ( method, "HEAD" ) ) exit ( 1 );
    /*
     * Print html header, using header info from cd_contents file.
     */
    cgi_printf("content-type: text/html\n\n");
    cgi_printf("<HTML>\n<HEAD>\n<TITLE>%s CD contents</TITLE>\n", rel_date );
    cgi_printf("<ISINDEX></HEAD>\n<BODY>\n<H1>Condist CD contents</H1>\n");
    for ( i = 0; headers[i]; i++ ) cgi_printf("%s ", headers[i] );
    cgi_printf("<BR>Release date: %s<P>\n", rel_date );
    /*
     * See if a search requested.
     */
    query_string = cgi_info("QUERY_STRING");
    if ( !query_string ) query_string = "";
    else if ( *query_string ) {
	/*
	 * Parse query string into separate match words, upcased.
	 */
	char *cp, *wrd; int last, j, word_length;
	for ( j = 0, wrd = cp = query_string; (j<19); cp++ ) {
	    /*
	     * Break into separate words delimited by '+'
	     */
	    if ( !*cp || (*cp == '+') ) {
		last = *cp ? 0 : 1;
		*cp = '\0';
		word_length = strlen ( wrd );
		net_unescape_string ( wrd, &word_length );
		wrd[word_length] = '\0';
		upcase_string ( match_word[j++], wrd, 127 );
		/* printf("match_word[%d] = '%s'\n", j-1, match_word[j-1] ); */
		if ( last ) break;
		wrd = cp+1;
	    }
	}
	match_word[j][0] = '\0';
	/*
	 * Report parse results.
	 */
	cgi_printf("<H2>Search Criteria</H2>\nThe products listed");
	cgi_printf(" in the products section contain the following strings");
	cgi_printf(" in their description field:<UL>");
	for ( j = 0; match_word[j][0]; j++ ) {
	    cgi_printf("<LI>%s</LI>", match_word[j] );
	}
	cgi_printf("</UL>");
    }
    /*
     * Make list of entries.
     */
    count = matches = 0;
    cgi_printf("<H2>Products</H2>\n<DL>\n");
    while ( cdm_product_scan ( line, sizeof(line), &ver, &upi, &state, 
		&kit, &disc ) ) {
	count++;
	/*
	 * Screen products is this is a search.
	 */
	if ( *query_string ) {
	    /*
	     * Only accept product for listing if its description
	     * contains every match word.
	     */
	    int j;
	    upcase_string ( test_line, line, sizeof(test_line)-1 );
	    upcase_string ( test_kit, &line[kit], sizeof(test_kit)-1 );
	    upcase_string ( test_ver, &line[ver], sizeof(test_ver)-1 );
	    for ( j = 0; match_word[j][0]; j++ ) {
	        if ( strstr ( test_line, match_word[j] ) ) continue;
	        if ( strstr ( test_kit, match_word[j] ) ) continue;
	        if ( !strstr ( test_ver, match_word[j] ) ) break;
	    }
	    if ( match_word[j][0] ) continue;	/* test failed */
	}
        /*
	 * Translate product status code.
	 */
	prod_status = &line[state];
	if ( *prod_status == 'A' ) prod_status = "unchanged";
	else if ( *prod_status == 'N' ) {
	    prod_status = (line[state+1] == 'C') ? "unchanged" : "New";
	}
	else if ( *prod_status == 'U' ) prod_status = "Updated";

	cgi_printf("<DT>%s</DT>\n<DD>Version: %s, Disc: %d, Kit: %s, Status: %s</DD>\n",
	    line, &line[ver], disc, &line[kit], prod_status );
	matches++;
    }
    cgi_printf("</DL><HR>\n");	/* end list */
    /*
     * Print summary of contents.
     */
    if ( matches < count ) cgi_printf ( "Number of matches: %d, \n", matches );
    cgi_printf("Number of products: %d\n</BODY></HTML>\n", count );
    return 1;
}
/*
 * This module defines routines to read the CDMENU contents files.
 *
 * int cdm_init ( char *fname, char **headers, char **release_date );
 * int cdm_product_scan ( 
 *	char *buf, int buflen, 		* Product name.
 *	int *ver, 			* version 
 *	int *upi,			* part number 
 *	int *state,			* A-already N-new U-update 
 *	int *kit, 			* VMSINSTAL kit name. 
 *       int *disc )			* Disc number (decoded). 
 */
/**************************************************************************/
/* Open file and position.
 */
int cdm_init ( char *fname, char **headers, char **release_date )
{
    int j, i;
    char line[2048];
    /*
     * Open file.
     */
    cdmf = fopen ( fname, "r" );
    if ( !cdmf ) {
	return vaxc$errno;
    }
    *release_date = "";
    /*
     * Read lines until "%end-of-data" seen.
     */
    i = 0;
    while ( fgets ( line, sizeof(line), cdmf ) ) {
	if ( strncmp ( line, "%END_OF_DATA", 12 ) == 0 ) break;
	if ( (i < 4) && (strncmp ( line, "%DISC_PRODUCT_NAME", 18) == 0) ) {
	    /*
	     * Copy first 4 product name lines to header.
	     */
	    headers[i] = malloc ( strlen(line)+1 );
	    for ( j = 18; line[j] && !isspace(line[j]); j++ );
	    while ( isspace(line[j]) ) j++;
	    strcpy ( headers[i], &line[j] );
	    for ( j = 0; headers[i][j]; j++ ) if (headers[i][j] == '\n') {
		headers[i][j] = '\0'; break;
	    }
	    i++;
	} else if ( 0 == strncmp(line,"%RELEASE_DATE",13 ) ) {
	    release_date[0] = malloc ( strlen(line)+1 );
	    for ( j = 13; line[j] && !isspace(line[j]); j++ );
	    while ( isspace(line[j]) ) j++;
	    strcpy ( release_date[0], &line[j] );
	    for ( j = 0; release_date[0][j]; j++ ) 
		if (release_date[0][j] == '\n') {
			release_date[0][j] = '\0'; break;
	    	}
	}
    }
    headers[i] = NULL;
    return 1;
}
/**************************************************************************/
/*
 * Read next line and parse into components.  Name is first component,
 * other components are given by offset.  Comment lines are skipped.
 */
int cdm_product_scan ( 
	char *buf, int buflen, 		/* Product name. */
	int *ver, 			/* version */
	int *upi,			/* part number */
	int *state,			/* A-already N-new U-update */
	int *kit, 			/* VMSINSTAL kit name. */
        int *disc )			/* Disc number (decoded). */
{
    int status, length, i, j, in_tok, token[10];
    char *tmp;
    /*
     * First read line and parse out first element.
     */
    if ( !cdmf ) return 0;
    for ( length = 0; length == 0; ) {
	tmp = fgets ( buf, buflen, cdmf );
	if ( !tmp ) return 0;
	/*
	 * Find start of first element.
	 */
	for ( i = 0; buf[i] && isspace(buf[i]); i++ );
	if ( (buf[i] == '\0') || (buf[i] == '!') ) continue;
	/*
	 * Collapse leading space and opening quote.
	 */
	j = 0; 
	if ( buf[i] == '"' ) {
	    for ( i++; buf[i] && (buf[i] != '"'); ) buf[j++] = buf[i++];
	    buf[j] = '\0';
	} else {
	    while ( buf[i] && !isspace(buf[i]) ) buf[j++] = buf[i++];
	}
	buf[j] = '\0';
	if ( buf[i] ) i++;
	/*
	 * Parse out rest of line.
	 */
	for ( j=0, in_tok = 0; (j < 9) && buf[i]; i++ ) {
	    if ( in_tok ) {
		if ( isspace ( buf[i] ) ) {
		    in_tok = 0;
		    buf[i] = '\0';		/* terminate current */
		}
	    } else {
		if ( !isspace (buf[i]) ) {
		    in_tok = 1;
		    token[j++] = i;
		}
	    }
	}
	token[j] = i;
	/*
	 * Return select tokens.
	 */
	*ver = j > 1 ? token[1] : i;
	*upi = j > 0 ? token[0] : i;
	*state = j > 3 ? token[3] : i;
	*kit = j > 7 ? token[7] : i;
	*disc = j > 8 ? atoi(&buf[token[8]]) : -1;
	length = j;
    }
    return 1;
}
