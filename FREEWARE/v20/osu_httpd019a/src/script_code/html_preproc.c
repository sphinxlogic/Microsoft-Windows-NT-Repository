/*
 * This program is run by the WWWEXEC scriptserver to do pre-processing of
 * html files, dynamically inserting files or other generated data.
 *
 * To specify the preprocessing you must give the file a distinct file type
 * (e.g. htmlx) and add the following to the configuration file:
 *
 *	suffix .htmlx text/x-server-parsed-html
 *	presentation text/x-server-parsed-html html_preproc
 *
 * WWWEXEC will then execute the following command line:
 *
 *	html_preproc method url protocol
 *
 *	argv[1]		Method specified in request (e.g. GET).
 *	argv[2]		Ident portion of requested URL, after translation by
 *			rule file.
 *	argv[3]		Protocol specified in request, "" or "HTTP/1.0".
 *
 * None of the resulting file contents is returned until the entire file
 * has been processed.  This is required for 2 reasons:
 *
 *   1. The HTTP status, which is the first part of the response, is not known
 *       until processing is complete.
 *
 *   2.  Processing of the file may require using additional server functions,
 *       such as <DNETXLATE>, which become unavailable once the HTTP response
 *       <DNETRAW> is started.
 *
 * Author:	David Jones
 * Date:	25-AUG-1994
 * Revised:	3-SEP-1994	Re-coded parsing.
 * Revised:	6-OCT-1994	Support fsize/flastmod.
 * Revised:	11-OCT_1994	Adjust length in translate.
 * Revised:	20-DEC-1994	Fix length in do_include
 * Revised:	22-APR-1995	Added extra echo variables suggested by
 *			        Kent Covert (kacovert@miavx1.acs.muohio.edu):
 *				    DOCUMENT_NAME, LAST_MODIFIED, 
 *				    ACCESSES/ACCESSES_ORDINAL
 * Revised:	23-APR-1995	Add permissions file checks to ACCESSES var.
 * Revised:	25-APR-1995	Overhaul:
 *				  - Add strftime() formatting options to
 *				    LAST_MODIFIED, DATE_LOCAL, ACCESSES*
 *				  - Add version numbering to ACCESSES.
 *				  - Rename indexio.h to access_db.h
 * Revised:	15-MAY-1995	Use <DNETXLATEV> for virtual includes
 *				(does protection check).
 * Revised:	21-JUN-1995	Remove '\r's from net_link_printf formats as
 *				net_link_printf will add them.
 * Revised:	8-AUG-1995	Use CGI mode rather than RAW mode (stsline 
 *				can be sent using status: CGI header)
 * Revised:	31-AUG-1995	Fixup formatting of ACCESSES values for
 *				VAXC compatibility.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stat.h>
#include <time.h>
#include <errno.h>
#include <ctype.h>
#include "scriptlib.h"
#include "access_db.h"

int LIB$INIT_TIMER(), LIB$SHOW_TIMER();
struct segment { int length; char *addr; };
typedef struct segment *segptr;

static int send_http_header ( char *stsline, char *content );
static int parse_directive 
	( char *path, char **targ, int *outlen, char **outbuf );
static int parse_tag ( char *tag, int maxlen, int *taglen, char **targ );
static segptr alloc_segment ( segptr seg, int sc, int *seg_size, char *start );
static char *http_protocol_version;

static char *access_file_fdl = "\
  FILE; ORGANIZATION indexed; PROTECTION (system:RWED,owner:RWED,group,world);\
  RECORD; CARRIAGE_CONTROL carriage_return; FORMAT fixed; SIZE 120;\
  KEY 0; CHANGES no; PROLOG 3; SEG0_LENGTH 100; SEG0_POSITION 0; TYPE string;\
";
/****************************************************************************/
/* Main program.
 */
int main ( int argc, char **argv )
{
    FILE *hfile;
    int status, is_directive, i, j, k, length, s_size, used, sc, sg_size;
    segptr seg;
    char *source, *targ[11];
    /*
     * Make connection back to server and set protocol version field for
     * the response to server.
     */
    status = net_link_open();
    if ( 0 == (status&1) ) exit ( status );
    http_protocol_version = argv[3][0] ? "HTTP/1.0 " : "";
    /*
     * Validate method (argv[1]).  We only understand GET and HEAD.
     */
    if ( argc < 4 ) exit ( 20 );
    if ( strcmp("GET",argv[1]) && strcmp("HEAD",argv[1]) ) {
	send_http_header ( "501 unsupported method", "text/plain" );
	net_link_printf ("Unsupported method (%s)\n", argv[1] );
	return 1;
    }
    /*
     * Open file specified by argv[2].
     */
#ifdef DEBUG
    LIB$INIT_TIMER();
    printf("input file: %s\n", argv[2] );
#endif
    hfile = fopen ( argv[2], "r", "mbc=32" );
    if ( !hfile ) {
	send_http_header ( "404 Error openning file", "text/plain" );
	net_link_printf ( "Error opening file for HTML pre-processing." );
	return 1;
    }
    /*
     * Read entire file into memory.
     */
    s_size = 20000;
    source = malloc ( s_size );
    used = 0;
    
    while ( (length=fread(&source[used], 1, s_size-used, hfile)) > 0 ) {
	used += length;
	if ( used >= s_size ) {
	    s_size += 20000;
	    source = realloc ( source, s_size );
	}
    }
    fclose(hfile);
#ifdef DEBUG
LIB$SHOW_TIMER();
printf("read %d bytes from htmlx file\n", used );
#endif
    /*
     * Parse file, making list of individual blocks of text to send
     * back to server.
     */
    sg_size = 1000;
    seg = (struct segment *) malloc ( sg_size * sizeof(struct segment) );
    seg[0].length = 0;
    seg[0].addr = source;
    for ( sc = i = 0; i < used; i++ ) {
	char cur;
	cur = source[i];
	if ( cur == '<' ) {
	    /*
	     * At beggining of HTML tag, go to state machine to parse.
	     */
	    is_directive = parse_tag ( &source[i], used-i, &j, targ );
	    if ( j > 0 ) {
		/*
		 * Include parsed tag in output, as a comment the client
		 * will ignore it.
		 */
		seg[sc].length += j;
		while ( seg[sc].length >= 1024 ) {
		    /* Make current segment 1024 and put rest in new segment */
		    seg = alloc_segment ( seg, sc, &sg_size, 
				&seg[sc].addr[1024] );
		    seg[sc+1].length = seg[sc].length - 1024;
		    seg[sc++].length = 1024;
		}
	    }
	    i += j - 1;

	    if ( is_directive ) {
		/*
		 * Terminate current segment and make fresh one to hold
		 * included info.  Init start address of new segment to
		 * point after tag.
		 */
		if ( seg[sc].length > 0 ) {
		    seg = alloc_segment ( seg, sc++, &sg_size, &source[i] );
		} else seg[sc].addr = &source[i];
	 	j = 0;	/* remove already added. */
		status = parse_directive 
			( argv[2], targ, &seg[sc].length, &seg[sc].addr );
		if ( status < 0 ) return 1;
		while ( seg[sc].length > 1024 ) {
		    /* Make current segment 1024 and put rest in new segment */
		    seg = alloc_segment ( seg, sc, &sg_size, 
				&seg[sc].addr[1024] );
		    seg[sc+1].length = seg[sc].length - 1024;
		    seg[sc++].length = 1024;
		}
		if ( seg[sc].length > 0 ) {
		    seg = alloc_segment ( seg, sc++, &sg_size, &source[i+1] );
		}
	    }
	} else {
	    /*
	     * Extend current segment to include this one.
	     */
	    seg[sc].length++;
	    if ( seg[sc].length >= 1024 ) {
		/* Segment at write limit, advance to next */
		seg = alloc_segment ( seg, sc++, &sg_size, &source[i+1] );
	    }
	}
    }
#ifdef DEBUG
LIB$SHOW_TIMER();
printf("Parsed source into %d segment%s\n", sc+1, sc ? "s" : "" );
#endif
    /*
     * Flush accumulated segments.
     */
    send_http_header ( "200 sending processed HTML", "text/html" );
    if ( 0 == strcmp(argv[1],"HEAD") ) return 1;

    for ( i = 0; i <= sc; i++ ) if ( seg[i].length > 0 ) {
	status = net_link_write ( seg[i].addr, seg[i].length );
	if ( (status&1) == 0 ) break;
    }
#ifdef DEBUG
LIB$SHOW_TIMER();
#endif
    return 1;
}
/**************************************************************************/
/* Convert binary time to ascii string.  If fmt_str does not start with '=',
 * use ctime format, otherwise use string after '=' as format for strftime().
 */
static char *format_time (char *buffer, int bufsize, char *fmt, void *timebuf)
{
#ifdef __DECC
    if ( fmt ) if ( *fmt == '=' ) {
	size_t size;
	size = strftime ( buffer, bufsize, &fmt[1], 
		localtime((time_t *)timebuf) );
	if ( (size > 0) && (size < bufsize) ) buffer[size] = '\0';
	return buffer;
    }
#endif
    /*
     * fallback to using ctime() routine.
     */
    strncpy ( buffer, ctime((unsigned long *) timebuf), bufsize-1 );
    buffer[bufsize-1] = '\0';
    return buffer;
}
/**************************************************************************/
static segptr alloc_segment ( segptr seg, int sc, int *seg_size, char *start ) {
    sc = sc + 1;
    if ( sc >= *seg_size ) {
	*seg_size += 1000;
	seg = realloc ( seg, sizeof(struct segment)*(*seg_size) );
    }
    seg[sc].length = 0;
    seg[sc].addr = start;
    return seg;
}
/**************************************************************************/
/* Prepare to send back response.  Build standard response header.
 */
static int send_http_header ( char *stsline, char *content )
{
    int status;
    /*
     * Enter CGI mode, set rundown to terminate mode on exit.
     */
    status = net_link_write ( "<DNETCGI>", 9 );
    if ( 0 == (status&1) ) exit ( status );
    status = net_link_set_rundown ( "</DNETCGI>" );
    /*
     * Send back standard header.
     */
    if ( status&1 ) status = net_link_printf ( 
	"Content-type:%s\nstatus: %s\n\n", content, stsline );
    return status;
}
/*****************************************************************************/
/* Handle the file-related server directives (include, fsize, flastmod).
 */
static int do_include ( int opcode,	/* 1 - include, 2-fsize, 3-flastmod */
	char *path,	/* pathname of file being parsed */
	char **drctv,		/* Parsed directive tokens 1=tag, 2=fname */
	int *outlen, 
	char **outbuf )
{
    char *ibuf, *tag, *name, fname[512], translation[512];
    int length, buf_used, buf_size;
    FILE *ifile;
    /*
     * Examine field1 argument to determine include type.
     */
    tag = drctv[1];
    name = drctv[2];
    *outlen = 0;
    if ( 0 == strcmp ( tag, "FILE" ) ) {
	/*
	 * Construct filename relative to current path.
	 */
	strncpy ( fname, path, 255 ); fname[255] = '\0'; 
	for ( length = strlen(fname); 
		(length > 0) && (fname[length] != '/'); --length );
	length++;
	strcpy ( &fname[length], name );
	length = strlen ( fname );
	if ( length > 0 ) if ( fname[length-1] == '"' ) fname[length-1] = '\0';

    } else if ( 0 == strcmp ( tag, "VIRTUAL" ) ) {
	/*
	 * Have server translate name.
	 */
	int status;

	net_link_write ( "<DNETXLATEV>",  12 );
	status = net_link_query ( name, fname, sizeof(fname)-1, &length);
	if ( 0 == (status&1) ) length = 0;
	fname[length] = '\0';
    }
    /*
     * Take action based upon opcode.
     */
    if ( opcode == 1 ) {
	/*
	 * Include the file.
	 */
	ifile = (length > 0) ? fopen ( fname, "r", "mbc=32" ) : (FILE *) 0;
	if ( !ifile ) {
	    send_http_header ( "500 Failed to open include file", "text/plain");
	    net_link_printf ( "Could not open include file (%s)\n", name );
	    return -1;
	}
	buf_size = 10000;
	buf_used = 0;
	ibuf = malloc ( buf_size );
	while ( (length=fread(&ibuf[buf_used], 1, 10000, ifile)) > 0) {
	    buf_used += length;
	    if ( buf_used >= buf_size ) {
	        buf_size += 10000;
	        ibuf = realloc ( ibuf, buf_size );
	    }
	}
	fclose(ifile);
	*outbuf = ibuf;
	*outlen = buf_used;
    } else {
	/*
	 * fsize(2) and flastmod(3).  Load stat structure.
	 */
	stat_t info;
	int status;
	status = (length > 0) ? stat ( fname, &info ) : -1;
	if ( status != 0 ) {
	    send_http_header ( "500 Failed to access file", "text/plain");
	    net_link_printf ( "Could not read file attributes (%s)\n", name );
	    return -1;
	}
	/*
	 * Format information.
	 */
	*outbuf = malloc ( 64 );
	if ( opcode == 2 ) {
	    sprintf ( *outbuf, "%d", info.st_size );
	} else {
	    /*
	     * Check if user included FMT tag.
	     */
	    char fmt[100];
	    fmt[0] = '\0';
	    if ( strcmp(drctv[3],"FMT") == 0 ) {
		fmt[0] = '=';
		strncpy ( &fmt[1], drctv[4], sizeof(fmt)-2 );
		fmt[99] = '\0';
	    }
	    format_time ( *outbuf, 64, fmt, &info.st_mtime );
	    /* strcpy ( *outbuf, ctime ( (unsigned long *) &info.st_mtime ) );*/
	}
	*outlen = strlen ( *outbuf );
    }
    return *outlen;
}
/*****************************************************************************/
/* Return accesses count for indicated path, updating count by 1 on first call
 * A version number less than or equal to 0 means ignore the version number.
 */
static int get_access_count ( char *path, int version ) {
    IFILE *fp;
    static int accesses_known = 0, accesses, rec_version;
    char accessesRecord[128];
    char accessesPath[104];
    char accessesStr[20];
    int status, i, new_rec;
    size_t length;
    stat_t info;
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
	fp = ifopen ( "www_root:[000000]accesses.dat","r+" );
	if ( !fp ) {
	    /*
	     * See if we have permission to write new records.
	     */
	    status = stat ( path, &info );
	    if ( status == 0 ) 	status = icheck_access 
		( "www_root:[000000]accesses.permissions", info.st_uid );
	    if ( (status&1) == 0 ) {
	        send_http_header ( "500 Failed to create file", "text/plain");
	        net_link_printf ( 
		    "No permission to create accesses.dat file, code %d\n",
			status );
	        return -1;
	    }
	    if ( (status&1) == 0 ) {
	    }
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
	    net_link_printf ( "Could not open accesses.dat file.");
	    return -1;
	}
	/*
	 * File open, do indexed read to get record for specified path.
	 */
	for ( i = 0; (i < 100) && path[i]; i++ ) 
		accessesPath[i] = _toupper ( path[i] );
	while ( i < 100 ) accessesPath[i++] = ' ';
	accessesPath[i] = '\0';

	status = ifread_rec(accessesRecord,120,&length,fp,0,accessesPath,100);
	if ( status & 1 ) {
	    accessesRecord[120] = '\0';
	    sscanf(accessesRecord+100,"%10d%10d",&accesses, &rec_version);
	    new_rec = 0;
	} else {
	    /*
	     * Read error on record, assume non-existent and make initial.
	     */
	    accesses = 0;
	    rec_version = 0;
	    new_rec = 1;
	    /*
	     * Check if file owner has permission to add to accesses.dat file.
	     */
	    status = stat ( path, &info );
	    if ( status == 0 ) 	status = icheck_access 
		( "www_root:[000000]accesses.permissions", info.st_uid );
	    if ( (status&1) == 0 ) {
	        send_http_header ( "500 Failed to extend file", "text/plain");
	        net_link_printf ( 
		    "No permission to add records to accesses.dat, code %d\n",
			status );
	        return -1;
	    }
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
/*****************************************************************************/
/* Lookup indicate variables and return their values.
 */
static int do_echo ( char *path,	/* pathname of file being parsed */
	char **drctv,			/* Elements of directive */
	int *outlen, 
	char **outbuf )
{
    int SYS$ASCTIM(), timlen;
    char *timestr, *tag, *name;
    tag = drctv[1];
    name = drctv[2];
    *outlen = 0;
    if ( strcmp ( tag, "VAR" ) ) return 0;	/* syntax error */

    if ( 0 == strncmp(name,"DATE_LOCAL",10) ) {
	/*
	 * See if format info was supplied.
	 */
	if ( name[10] == '=' ) {
	    time_t now;
	    *outbuf = malloc ( 64 );
	    now = time(NULL);
	    format_time ( *outbuf, 64, &name[10], &now );
	    *outlen = strlen ( *outbuf );
	} else {
	    struct { long l; char * a; } timebuf;
	    *outbuf = malloc ( 24 );
	    timebuf.l = 24;
	    timebuf.a = *outbuf;
	    SYS$ASCTIM ( outlen, &timebuf, 0, 0 );
	}
    } else if ( 0 == strcmp(name,"DOCUMENT_NAME") ) {
	/* Echo filename of document (part to right of last /) */
	*outbuf = strrchr(path,'/');
	if ( *outbuf ) *outbuf = *outbuf+1; else *outbuf = path;
	*outlen = strlen ( *outbuf );
    } else if ( 0 == strncmp(name,"LAST_MODIFIED",13) ) {
	/*
	 * Echo last modified date of current file path.
	 */
	stat_t info;
	int status;
	status = stat ( path, &info );
	if ( status != 0 ) {
	    send_http_header ( "500 Failed to access file", "text/plain");
	    net_link_printf ( "Could not read file attributes (%s)\n", path );
	    return -1;
	}
	/*
	 * Format information.
	 */
	*outbuf = malloc ( 64 );
	format_time ( *outbuf, 64, &name[13], &info.st_mtime );
	/* strcpy ( *outbuf, ctime ( (unsigned long *) &info.st_mtime ) ); */
	*outlen = strlen ( *outbuf );
    } else if ( (0 == strncmp(name,"ACCESSES",8)) ) {
	/*
	 * Echo access count.
	 */
	int accesses, version, last, last2;
	/*
	 * see if name includes version number.
	 */
	version = 0;
	if ( name[8] == ';' ) version = atoi(&name[9]);
	else if ( strncmp(&name[8],"_ORDINAL;",9) == 0 ) 
		version = atoi(&name[17]);

	accesses = get_access_count ( path, version );
	if ( accesses < 0 ) return accesses;
	*outbuf = malloc ( 24 );
	if ( accesses < 1000 ) sprintf(*outbuf, "%d",accesses );
	else if ( accesses < 1000000 ) sprintf(*outbuf, "%d,%03.3d",
		accesses/1000, accesses%1000 );
	else sprintf ( *outbuf, "%d,%03.3d,%03.3d", accesses/1000000,
	    accesses%1000000/1000, accesses%1000 );
	if ( 0 == strncmp(name,"ACCESSES_ORDINAL",16) ) {
	    /* tack on suffix */
	    static char *ordination[10] = { "th", "st", "nd", "rd",
		"th", "th", "th", "th", "th", "th" };
	    char *suffix;
	    last2 = accesses %100;
	    suffix = &(*outbuf)[strlen(*outbuf)];
	    last = accesses % 10;
	    if ( (last2 >= 11) && (last2 <= 13) ) last = 0;
	    strcpy ( suffix, ordination[last] );
	}
	*outlen = strlen ( *outbuf );
    }
    return *outlen;
}
/*****************************************************************************/
static int do_config ( char *path,
	char *field1, char *field2,
	int *outlen, 
	char **outbuf )
{ *outlen = 0; return 0; }
/*****************************************************************************/
/* Top routine to handle interpreting an server-side html directive,
 * optionally returning to the caller a pointer to buffer containing
 * additional data to send to the client (inserted at the point of
 * the directive).  A zero outlen indicates no optional data is returned.
 *
 * The directive takes the form <!--#command [tag1=value [tag2=value]]-->
 *
 * Note that tag array is modified by this routine.
 * Note also that output buffers must be statically allocated, they are not
 * read by the caller until final processing.
 *
 * If this routine generates an error response, it must return -1 as the
 * function value.
 */
static int parse_directive ( 
	char *path,			/* Pathname of file being parsed */
	char **drctv,			/* 5 elements of parsed directive */
	int *outlen, 			/* Size of result output buffer */
	char **outbuf )			/* Address of result buffer */
{
    int i, taglen;
    char *command;
    /*
     * Scan the directive tag and parse into command and tags.
     */
#ifdef DEBUG
printf("Directive: '%s' '%s'='%s' '%s'='%s'\n\n", drctv[0], drctv[1],
drctv[2],drctv[3],drctv[4] );
#endif
    command = drctv[0];
    /*
     * Now interpret command.
     */
    if ( 0 == strcmp(command,"INCLUDE") ) {
	/*
	 * Include file.
	 */
	return do_include ( 1, path, drctv, outlen, outbuf );

    } else if ( 0 == strcmp ( command, "ECHO" ) ) {
	/*
	 * Echo will insert values of special variables into stream.
	 */
	return do_echo ( path, drctv, outlen, outbuf );
    } else if ( 0 == strcmp(command,"FSIZE") ) {
	/*
	 * File attributes.
	 */
	return do_include ( 2, path, drctv, outlen, outbuf );
    } else if ( 0 == strcmp(command,"FLASTMOD") ) {
	return do_include ( 3, path, drctv, outlen, outbuf );

    } else if ( 0 == strcmp ( command, "CONFIG" ) ) {
	/*
	 * Set configuration parameters.
	 */
	return do_config ( path, drctv[1], drctv[2], outlen, outbuf );
    }
    /*
     * Invalid directive in file.
     */
    send_http_header ( "500 Bad directive in file", "text/plain" );
    net_link_printf("invalid directive: (%s %s %s)\n", command, drctv[1],
		drctv[2] );
    *outlen = 0;
    return -1;
}
/*****************************************************************************/
/* Parse HTML tag.  If HTML tag looks like a pre-processor directive,
 * fill in targ array with parsed elments. (command, (tag,value) pairs).
 *
 * Return value:
 *	0	Tag is NOT a pro
 */
static int parse_tag ( char *tag, 	/* start of tag */
    int maxlen, 			/* remaining bytes in input file */
    int *taglen, 			/* Size of tag, including closing '>'*/
    char **targ )			/* Elements, cmd and up to 5 tag/value
					   pairs */
{
    int state, i, j, eot_ok;
    char *directive, cur;
    /*
     * First, determine if tag if directive or not, Size of tag is
     * also determined.
     */
    if ( *tag != '<' ) { *taglen = 0; return 0; }

    for ( state = i = 0; (state >= 0) && (i < maxlen); i++ ) {
	cur = tag[i];
	switch (state)  {
          case 0:
	    if ( cur != '<' ) { *taglen = i+1; return 0; }
	    state = 1;
	    break;
	   case 1:
	     if ( cur == '!' ) { state = 3; break; }
	     state = 2;
	   case 2:	/* Not a directive, search for end. */
	     if ( cur == '>' ) { *taglen = i+1; return 0; }
	     break;

	   case 3:
	     if ( cur != '-' ) { state = 2; --i; break; }
	     state = 4;
	     break;
	   case 4:
	     if ( cur != '-' ) { state = 2; --i; break; }
	     state = 5;
	     break;
	   case 5:
	     if ( cur != '#' ) { state = 2; --i; break; }
	     state = 6;
	     break;

	   case 6:		/* Find terminator */
	     if ( cur == '-' ) state = 7;
	     break;
	   case 7:
	     if ( cur == '-' ) state = 8; else state = 6;
	     break;
	   case 8:
	    if ( cur == '>' ) state = -1; else state = (cur=='-') ? 8 : 6;
	    break;
	}
    }
    /*
     * Getting to this point means we parsed a directive (state = -1).
     * Make copy of tag and parse the copy (portions of copy will be upcased).
     */
    directive = malloc ( i + 1 );
    memcpy ( directive, tag, i );
    directive[i] = '\0';
    tag = directive;
#ifdef DEBUG
printf("comment: '%s' l = %d\n", directive, i );
#endif
    /*
     * Initialize return array to all null strings.
     */
    *taglen = i;
    targ[0] = &tag[5];
    for ( i = 1; i < 11; i++ ) targ[i] = "";
    eot_ok = 1;
    /*
     * Parse tag into directive components using state machine.
     */
    for ( state = j = 0, i = 5; (state >=0) && (i < *taglen-3); i++ ) {
#ifdef DEBUG
printf("state = %d, tag[%d] = '%c'\n", state, i, tag[i] );
#endif
	cur = tag[i];
	switch ( state ) {
	   case 0:
		if ( isspace(cur) || (cur=='\n') )  {
		    /* found end of targ[0] */
		    tag[i] = '\0'; 
		    state = 1;
		    j = 1;
		    targ[j] = &tag[i]; 
		}
		else tag[i] = _toupper(cur);
	        break;

	   case 1:			/* look for start of targ[j] */
		if ( !isspace(cur) /* || (cur == '\n') */ ) {
		    targ[j] = &tag[i];
		    state = 2;
		    eot_ok = 0;
		}
		else break;
	   case 2:			/* look for end of targ[1] */
		if ( cur == '=' ) { tag[i] = '\0'; state = 3; }
		else if ( isspace(cur) ) state = -1; /* syntax error */
		else tag[i] = _toupper(cur);
		break;

	   case 3:			/* Check for proper syntax */
		if ( cur == '"' ) {
		    j++;		/* Advance to value part */
		    targ[j] = &tag[i+1];
		    state = 4;
	 	} else state = -1;
		break;
	    case 4:
		if ( cur == '"' ) {	/* end of targ[2] */
		    tag[i] = '\0';
		    j++;
		    targ[j] = &tag[i];
		    state = 1;		/* look for next value */
		    eot_ok = 1;
		}
		break;
	}
    }
    if ( state == -1 ) {
	free ( directive );
	return 0;	/* improper syntax */
    }
    return 1;
}
