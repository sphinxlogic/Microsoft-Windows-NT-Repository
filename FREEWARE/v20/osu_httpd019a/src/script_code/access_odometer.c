/* # Credits:
#   Original C code:  Frans van Hoesel (hoesel@chem.rug.nl)
#   Original port to Perl: Dan Rich (drich@corp.sgi.com)
#   Modifications for cgi: Michael Nelson (m.l.nelson@larc.nasa.gov)
#   Modifications for NSF: Mike Morse (mmorse@nsf.gov)
#   Modified from NSF version: Spencer Thomas (spencer.thomas@med.umich.edu)
#   Modified for flexible use: Stefan Powell (spowell@netaccess.on.ca)
#   Modified to ease installation and included instructions: 
#    Jeff Squyres (squyres@nd.edu)
*   Converted back to C for use by VMS DECthreads server: David Jones.
*
*   Usage:
*	In your html document place an image with a src reference
*	of /htbin/access_odometer/name, e.g.
*
*	<IMG SRC="/htbin/access_odometer/cool_pics.html;1">
*
*       The path-info (name) is used to uniquely name a record in the
*	file www_root:[000000]accesses.dat.  Version number 0 is always used.
*
*   After compiling, link with access_db.obj, cgilib.obj, and scriptlib.obj.
*
*   For CERN server:
*	Compile with NOCGILIB macro defined (/define=NOCGILIB)
*	Link with access_db.obj only (plus C RTL is necessary).
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "access_db.h"
#define IMAGE_MIME_TYPE "image/x-xbm"
#define ACCESS_DB "www_root:[000000]accesses.dat"
/*
 * Handle both DECthreads and CERN CGI environments via macros.
 */
#ifdef NOCGILIB
#define cgi_init(a,b) 1
#define cgi_info(a) getenv(strcpy(cgi_info_buf[4], a)-4)
static char cgi_info_buf[64] = {'W', 'W', 'W', '_' };
#define cgi_printf printf
#define net_link_set_mode(a) 1;
#else
#include "cgilib.h"
#include "scriptlib.h"
#endif
static int send_http_header ( char *stsline, char *content );
static char *access_file_fdl = "\
  FILE; ORGANIZATION indexed; PROTECTION (system:RWED,owner:RWED,group,world);\
  RECORD; CARRIAGE_CONTROL carriage_return; FORMAT fixed; SIZE 120;\
  KEY 0; CHANGES no; PROLOG 3; SEG0_LENGTH 100; SEG0_POSITION 0; TYPE string;\
";


static int xbm_int ( int number, 	/* number to represent as bitmap */
	int places, 		/* Number of digits */
	int inverse,		/* If true, invert foreground/background */
	int margin,		/* If true, put 4-bit margin around image */
	char *outbuf,
	int *outlen )
{
   static char *invdigit_map[10] = {
	"c3 99 99 99 99 99 99 99 99 c3", "cf c7 cf cf cf cf cf cf cf c7",
        "c3 99 9f 9f cf e7 f3 f9 f9 81", "c3 99 9f 9f c7 9f 9f 9f 99 c3",
        "cf cf c7 c7 cb cb cd 81 cf 87", "81 f9 f9 f9 c1 9f 9f 9f 99 c3",
        "c7 f3 f9 f9 c1 99 99 99 99 c3", "81 99 9f 9f cf cf e7 e7 f3 f3",
        "c3 99 99 99 c3 99 99 99 99 c3", "c3 99 99 99 99 83 9f 9f cf e3"};
   
    static char *digit_map[10] = {
	"3c 66 66 66 66 66 66 66 66 3c", "30 38 30 30 30 30 30 30 30 30",
         "3c 66 60 60 30 18 0c 06 06 7e", "3c 66 60 60 38 60 60 60 66 3c",
         "30 30 38 38 34 34 32 7e 30 78", "7e 06 06 06 3e 60 60 60 66 3c",
         "38 0c 06 06 3e 66 66 66 66 3c", "7e 66 60 60 30 30 18 18 0c 0c",
         "3c 66 66 66 3c 66 66 66 66 3c", "3c 66 66 66 66 7c 60 60 30 1c"};

    char **dmap, *column[20];
    int i, digit, row, off, length;
    /*
     * Convert number to digits.
     */
    if ( number < 0 ) number = 0;
    dmap = inverse ? invdigit_map : digit_map;
    for ( i = 0; i < places; i++ ) column[i] = dmap[0];

    for ( i = 0; (number > 0) && (i < places); i++ ) {
	column[i] = dmap[number%10];
	number = number / 10;
    }
    /*
     * Put prolog in outbuf.
     */
    sprintf ( outbuf, "#define count_width %d\n#define count_height %d\n",
	places*8, margin ? 16 : 10 );
    length = strlen ( outbuf );
    strcpy ( &outbuf[length], "static char count_bits[] = {\n" );
    length += 29;
    /*
     * Produce output rows.
     */
    if ( margin ) {
	for ( row = 0; row < 3; row++ ) {
	    for ( i = 0; i < places; i++ ) {
		strcpy ( &outbuf[length], inverse ? "0xff," : "0x00," );
		length += 5;
	    }
	    outbuf[length++] = '\n';
	}
    }
    for ( row = 0; row < 10; row++ ) {
	off = row * 3;
	for ( i = places-1; i >= 0; --i ) {
	    outbuf[length++] = '0'; outbuf[length++] = 'x';
	    outbuf[length++] = column[i][off];
	    outbuf[length++] = column[i][off+1];
	    if ( (i == 0) && (row == 9) && !margin ) {
		outbuf[length++] = '}'; outbuf[length++] = ';';
	    } else outbuf[length++] = ',';
	}
	outbuf[length++] = '\n';
    }

    if ( margin ) {
	for ( row = 0; row < 3; row++ ) {
	    for ( i = places-1; i >= 0; --i ) {
		strcpy ( &outbuf[length], inverse ? "0xff" : "0x00" );
		length += 4;
		if ( (i == 0) && ( row == 3 ) ) {
		    outbuf[length++] = '}'; outbuf[length++] = ';';
		}
		else outbuf[length++] = ',';
	    }
	    outbuf[length++] = '\n';
	}
    }
    *outlen = length;
    outbuf[length] = '\0';
    return length;
}

/*****************************************************************************/
/* Return accesses count for indicated path, updating count by 1 on first call
 * A version number less than or equal to 0 means ignore the version number.
 *
 * The path string is prefixed with "//odometer/" to avoid confusion with
 * the pre-processor counters.  No checks for access permissions are made.
 */
static int get_access_count ( char *path, int version ) {
    IFILE *fp;
    static int accesses_known = 0, accesses, rec_version;
    char accessesRecord[128];
    char accessesPath[104];
    char accessesStr[20];
    int status, i, new_rec;
    size_t length;
    /*
     * Reset accesses_known if version changes.
     */
    if ( accesses_known ) if ( (version > 0) && (version != rec_version) ) {
	accesses_known = 0;
    }

    if ( !accesses_known ) {
	/*
	 * Get/update access count/version in file.  Try to open file.
	 */
	fp = ifopen ( ACCESS_DB, "r+" );
	if ( !fp ) {
	    /*
	     * Attempt to create indexed file and retry open.
	     */
	    status = ifdlcreate 
		( access_file_fdl, "accesses.dat", "www_root:[000000]" );
	    if ( status & 1 ) 
		fp = ifopen ( "www_root:[000000]accesses.dat","r+" );
	}
	if ( !fp ) {
	    send_http_header ( "500 Failed to access file", "text/plain" );
	    cgi_printf ( "Could not open accesses.dat file.");
	    return -1;
	}
	/*
	 * File open, do indexed read to get record for specified path.
	 */
	strcpy ( accessesPath, "//odometer/" );
	strncpy ( &accessesPath[11], path, sizeof(accessesPath)-12 );
	accessesPath[sizeof(accessesPath)-1] = '\0';
	for ( i = 0; (i < 100) && accessesPath[i]; i++ ) 
		accessesPath[i] = _toupper ( accessesPath[i] );
	while ( i < 100 ) accessesPath[i++] = ' ';
	accessesPath[i] = '\0';

	status = ifread_rec(accessesRecord,120,&length,fp,0,accessesPath,100);
	if ( status & 1 ) {
	    sscanf(accessesRecord+100,"%10d%10d",&accesses, &rec_version);
	    new_rec = 0;
	} else {
	    /*
	     * Read error on record, assume non-existent and make initial.
	     */
	    accesses = 0;
	    rec_version = 0;
	    new_rec = 1;
	}
        /*
	 * Reset or update access count by 1 and update record in file.
	 */
	if ( version <= 0 ) version = rec_version;
	else if ( version != rec_version ) accesses = 0;
	accesses++;

	sprintf(accessesRecord,"%-100.100s%10.10d%10.10d",accessesPath,accesses,
		version);
	if ( new_rec ) {
	    iferror(fp);
	    status = ifwrite_rec(accessesRecord,120,fp);
	} else {
	    status = ifupdate_rec(accessesRecord,120,fp);
	}
	if ( (status&1) == 0 ) iferror(fp);
	ifclose(fp);
	accesses_known = 1;
    }
    return accesses;
}
/**************************************************************************/
/* Prepare to send back response.  Build standard response header.
 */
static int send_http_header ( char *stsline, char *content )
{
    int status;
    /*
     */
    cgi_printf ( "Content-type: %s\n", content );
    cgi_printf ( "status: %s\n\n", stsline );
    return 1;
}

/****************************************************************************/
/* Main routine.
 */
int main ( int argc, char **argv )
{
    int number, length, status;
    char *path, buffer[2024];
    /*
     * Initialize CGI library.
     */
    status = cgi_init ( argc, argv );
    if ( (status&1) == 0 ) exit ( status );
    /*
     * Get path info and and use for access path.
     */
    path = cgi_info ( "PATH_INFO" );
    if ( !path ) {
	send_http_header ( "500 bad path info", "text/plain" );
	return 20;
    }
    number = get_access_count ( path, 0 );
    if ( number < 1 ) exit ( 1 );	/* Response already sent */
    /*
     * Generate xbm odometer image.
     */
    xbm_int ( number, 
	7, 				/* 7 digits */
	0, 				/* Normal (not inverted) */
	0, 				/* No margin */
	buffer, &length );		/* Recieves output image */
    cgi_printf ( "Content-type: %s\n", IMAGE_MIME_TYPE );
    cgi_printf ( "Content-transfer-encoding: 7bit\n\n" );
    net_link_set_mode ( 1 );	/* put into binary mode */
    cgi_printf ( "%s", buffer );
    return 1;
}
