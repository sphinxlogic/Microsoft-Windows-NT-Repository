/* This shareable image module supports multiple image map formats:
 *
 *     CERN HTIMAGE format:  htimage
 *     NCSA mapimage format: ncsa
 *     URL redirection:	     url
 *
 *  Setup the MST in the configuration file as follows:
 *    ThreadPool test stack=60000 q_flag=1 limit=4
 *    Service testsrv pool=test dynamic=(omnimap,omnimap_mst)
 *
 *  To invoke the MST from a virtual script directory:
 *    Exec /$omnimap/* %omnimap:/omni		# for virtual script director.
 *
 *  To invoke the MST as a presentation script:
 *    suffix .htimage omnimap/htimage
 *    presentation omnimap/htimage %omnimap:/htimage
 *    suffix .mapimage omnimap/ncsa
 *    presentation omnimap/ncsa %omnimap:/ncsa
 *
 * Author: David Jones
 * Date:   16-JUN-1995
 */
#include "mst_share.h"
#include "tutil.h"
#include "file_access.h"
#include <stdio.h>
/* #include <math.h> */
struct token_list {
   int type;			/* -1: URL, 0: text, 1: default, 2: rect
				    3: circle, 4: polygon  */
   char *s;			/* Text pointer */
   float x, y;
};
struct stream_ctx {
    void *ff;			/* Input file */
    int used, filled;
    char buffer[4096];
};
typedef struct stream_ctx *stream;
#define isspace(a) ((a==' ')||(a=='\t')||(a=='\n')||(a=='\r'))
#define isdigit(a) ((a<='9') && (a>='0'))
static int map_htimage (stream ctx, float x, float y, char *match_url);
static int map_ncsa (mst_link_t link, stream ctx, 
		float x, float y, char *match_url);
static int error_abort ( mst_link_t link, char *status_line, char *message );
static int check_polygon(), get_command(), get_token();
static int decode_float ( char *string, float *result, char **tail );
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
int omnimap_init ( mst_linkage vector, char *info, char *errmsg )
{
    int status;
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
    status = (tf_initialize("") == 0) ? 1 : 0;
    /*
     * Return success status.
     */
    if ( status == 1 ) {
        tu_strcpy ( errmsg, "Omnimap scriptserver sucessfully initialized" );
     } else {
	tu_strcpy ( errmsg, "Omnimap init error: " );
	tu_strint ( status, &errmsg[20] );
     }
     return 1;
}

/***************************************************************************/
/* Put link into CGI mode and send redirect, allow for remote, absolute
 * local (begins with /) and relative local (use path_info path).
 */
static int send_redirect ( mst_link_t link, char *location, 
	struct mstshr_envbuf *env )
{
    int i, status, scheme, host, length;

    scheme = host = -1;
    if ( location[0] == '/' ) {
	/* Definately no scheme, see if remote or local */
	if ( location[1] == '/' ) {	/* host address included */
	    host = 0;
	}
    } else {
        for ( i = 0; location[i]; i++ ) {
	    if ( location[i] == ':' ) {
		scheme = i;
		break;
	    }
	    if ( location[i] == '/' ) break;
	}
	if ( location[scheme+1] == '/' ) {
	    if ( location[scheme+2] == '/' ) host = scheme+1;
	}
    }

    mst_write ( link, "<DNETCGI>", 9, &length );
    mst_write ( link, "Location: ", 10, &length );
    if ( location[0] == '/' || (host >= 0) ) {
	/*
	 * Path is either full remote address or absolute local file.
	 */
	mst_write ( link, location, tu_strlen ( location ), &length );
    } else {
	/*
	 * Do local fixup to make a complete path relative to PAHT_INFO
	 */
	char *path; int last_slash;
	path = mstshr_getenv ( "PATH_INFO", env );
	if ( !path ) {
	    return 0;
	}
	for ( last_slash = -1, i = 0; path[i]; i++ ) {
	    if ( path[i] == '/' ) last_slash = i;
	}
	if ( scheme > 0 ) {
	    mst_write ( link, location, scheme+1, &length );
	    if ( host < 0 ) {
		char * server_host;
		server_host = mstshr_getenv ( "SERVER_NAME", env );
		if ( server_host ) {
		    mst_write ( link, "//", 2, &length );
		    mst_write ( link, server_host, 
			tu_strlen (server_host), &length );
		}
	    }
	}
	if ( last_slash >= 0 ) {
	    mst_write ( link, path, last_slash + 1, &length );
	}
	mst_write ( link, &location[scheme+1],
		tu_strlen ( &location[scheme+1] ), &length );
    }
    mst_write ( link, "\r\n\r\n", 4, &length );
    status = mst_write ( link, "</DNETCGI>", 10, &length );
    return status;
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
int omnimap ( mst_link_t link, char *service, char *info, int ndx, int avail )
{
    float x, y;				/* coordinates in query */
    int i, status, length, map_type;
    char *script_name, *config_file, *query_string;
    void *cf;
    char *tail, line[512], match_url[1024];
    struct mstshr_envbuf env;
    struct stream_ctx local_ctx;
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
    if ( (status &1) == 0 ) return error_abort ( link, "500 error",
	"internal error getting cgi symbols" );
    /*
     * Get the script name, triming any preceding script_path.
     */
    script_name = mstshr_getenv ( "SCRIPT_NAME", &env );
    if ( !script_name ) {	/* abort */
	return error_abort ( link, "500 error", 
		"Internal error, no script_name" );
    }
    for ( i = 0; script_name[i]; i++ ) if ( script_name[i] == '/' ) {
	script_name = &script_name[i+1];
	i = -1;		/* will start at zero */
    }
    /*
     * Validate the script name.
     */
    if ( tu_strncmp ( script_name, "htimage", 7 ) == 0 ) {
	map_type = 1;
    } else if ( tu_strncmp ( script_name, "ncsa", 5 ) == 0 ) {
	map_type = 2;
    } else if ( tu_strncmp ( script_name, "url", 4 ) == 0 ) {
	map_type = 3;
    } else {
	/* Unknown script name */
	return error_abort ( link, 
		"400 unknown script", "Unknown script name" );
    }
    /*
     * Validate the METHOD: GET  or HEAD only allowed.  Handle HEAD
     * immediately.
     */
    if ( tu_strncmp ( env.prolog[1], "HEAD", 5 ) == 0 ) {
	return error_abort ( link, "200 script valid", "Omnimap script valid" );
    } else if ( tu_strncmp ( env.prolog[1], "GET", 4 ) != 0 ) {
	return error_abort ( link, "401 unsupported method",
		"Requested method unsupported" );
    }
    /*
     * For htimage and mapimage, decode the query arguments.
     */
    if ( map_type < 3 ) {
        query_string = mstshr_getenv ( "QUERY_STRING", &env );
        if ( !query_string ) query_string = "";
        if ( ! *query_string ) {
	    return error_abort ( link, "400 missing query", "missing query" );
	}
	tu_strnzcpy ( line, query_string, 50 );
	query_string = line;
        for ( i = 0; line[i]; i++ ) if ( line[i] == ',' ) break;
        if ( !line[i] ) {
	    /* Search argument doesn't have ',', look for '+' and convert */
	    for ( i = 0; line[i]; i++ ) if ( line[i] == '+' ) {
	        line[i] = ',';
	        break;
	    }
        }
	decode_float ( line, &x, &tail );
	if ( *tail == ',' ) {
	    i = decode_float ( &tail[1], &y, &tail );
	}
	if ( *tail != '\0' ) {
	    return error_abort ( link, "400 syntax error",
		"Syntax error in query" );
	}
    }
    /*
     * Path translated is config file to use, try to open it.
     */
    config_file = mstshr_getenv ( "PATH_TRANSLATED", &env );
    if ( !config_file ) {
	return error_abort(link,"500 bad path", "Illegal path in request" );
    }
    if ( !config_file[0] ) {
	return error_abort(link,"400 bad path", "Illegal path in request" );
    }
    local_ctx.used = local_ctx.filled = 0;
    local_ctx.ff = tf_open ( config_file, "r", line );
    if ( !local_ctx.ff ) {
	tu_strcpy ( line, "File open error on: " );
	tu_strnzcpy ( &line[20], config_file, sizeof(line)-21 );
	return error_abort(link,"500 I/O error",  line );
    }
    /*
     * We know the script name is good and file is readable, take particular
     * action based upon script name.
     */
    match_url[0] = '\0';
    switch ( map_type ) {
      case 1: 		/* htimage format */
	status = map_htimage ( &local_ctx, x, y, match_url );
	break;
      case 2:
	status = map_ncsa ( link, &local_ctx, x, y, match_url );
	break;
      case 3:
	/*
	 * URL case, redirect to first line in file.
	 */
	i = tf_getline ( local_ctx.ff, match_url, sizeof(match_url), 1 );
	if ( i > 3 ) match_url[i-2] = '\0';   /* trim CRLF */
	else match_url[i] = '\0';
	break;
    }
    tf_close ( local_ctx.ff );

    send_redirect ( link, match_url, &env );
    /*
     * Cleanly shutdown connection and return.
     */
    if ( (status &1) == 0 ) return 0;

    mst_close ( link );
    
    return 1;
}
/***************************************************************************/
/* Place scriptserver link into text mode and exit with error message
 */
static int error_abort ( mst_link_t link, char *statline, char *message )
{
    int length, status;
    static int abort = 0;
    status = mst_write ( link, "<DNETTEXT>", 10, &length );
    if ( (status &1) == 0 ) return status;
    status = mst_write ( link, statline, tu_strlen(statline), &length );
    if ( (status &1) == 0 ) return status;
    status = mst_write ( link, message, tu_strlen(message), &length );
    if ( (status &1) == 0 ) return status;
    status = mst_write ( link, "</DNETTEXT>", 11, &length );
    if ( (status &1) == 0 ) return status;
    status = mst_close ( link );
    if ( (status &1) == 0 ) return status;
    status = mst_exit ( &abort );
    return status;	/* shouldn't get here */
}
/***************************************************************************/
/* Scan string for numeric input, set tail to point to character that
 * terminated scan.
 */
static int decode_float ( char *string, float *result, char **tail )
{
    double value, divisor;
    int i, decimal, sign;
    unsigned char letter;
    decimal = 0;
    value = 0.0; 
    if ( string[0] == '-' ) { i = 1; sign = -1.0; }
    else if ( string[0] == '+' ) { i = 1; sign = 1.0; }
    else { i = 0; sign = 1.0; }
    for ( i = 0; string[i]; i++ ) {
	letter = string[i];
	if ( (letter >= '0') && (letter <= '9') ) {
	    value = value * 10.0 + ((int)letter - (int)'0');
	    if ( decimal ) divisor = divisor * 10;
	    if ( i > 15 ) break;
	} else if ( (letter == '.') && !decimal ) {
	    decimal = 1.0;
	    divisor = 1.0;
	} else {
	    /* terminating character */
	    break;
	}
    }
    *tail = &string[i];
    if ( decimal ) value = sign * value / divisor;
    *result = value;
    return i;
}
/***************************************************************************/
/* Interpret CERN htimage file format.
 */
static int map_htimage ( stream ctx, float x, float y, char *match_url )
{
    float d, min_x, max_x, min_y, max_y, dx, dy;
    struct token_list tok[512];
    char *tokbuf[4096];
    int tcnt;
    /*
     * Process file  test every figure to allow for overlapping regions.
     */
    match_url[0] = '\0';		/* default to no match */
    while ( (tcnt = 
		get_command ( ctx, tok, 512, tokbuf, sizeof(tokbuf)))>1) {
	switch ( tok[0].type ) {
	    case 1:		/* Default if no match */
		if ( match_url[0] == '\0' )
		    tu_strnzcpy ( match_url, tok[1].s, 1023 );
		break;

	    case 2:		/* rectangle */
		min_x = tok[1].x < tok[2].x ? tok[1].x : tok[2].x;
		max_x = tok[1].x > tok[2].x ? tok[1].x : tok[2].x;
		min_y = tok[1].y < tok[2].y ? tok[1].y : tok[2].y;
		max_y = tok[1].y > tok[2].y ? tok[1].y : tok[2].y;
		if ( (x >= min_x) && (x <= max_x) &&
		    (y >= min_y) && (y <= max_y) ) {
		    tu_strnzcpy ( match_url, tok[3].s, 1023 );
		}
		break;

	    case 3:		/* circle */
		dx = x - tok[1].x;
		dy = y - tok[1].y;
		if ( ( (dx*dx) + (dy*dy) ) <= (tok[2].x * tok[2].x) ) {
		    tu_strnzcpy ( match_url, tok[3].s, 1023 );
		}
		break;

	    case 4:		/* polygon */
		check_polygon ( tok, match_url, x, y );
		break;
	}
    }
    return 1;
}
/**************************************************************************/
/* Test whether x/y point lies within polygon defined by point within
 * token list.
 */
static int check_polygon (
	struct token_list *tok, char *match_url, double dblx, double dbly )
{
    int npnts, count, i, prev;
    float x1, y1, x2, y2, t, x, y;
    /*
     * Find number of points in polygon and duplicate first point
     */
    x = dblx; y = dbly;
    for ( npnts = 0; tok[npnts+1].type > -1; npnts++ );
    tok[npnts+1].x = tok[1].x; tok[npnts+1].y = tok[1].y;
#ifdef DEBUG
    printf("Testing point (%f, %f)\n", x, y );
#endif
    /*
     * Count number of polygon sides that cross same y value and are
     * to left (less) than x value.
     */
    for ( prev = count = 0, i = 1; i <= npnts; i++ ) {
	/* Set vertices. */
	x1 = tok[i].x; y1 = tok[i].y; x2 = tok[i+1].x; y2 = tok[i+1].y;
#ifdef DEBUG
	printf("segment[%d] = (%f,%f) (%f,%f)\n", i, x1, y1, x2, y2 );
#endif
	if ( ((y1 <= y) && (y2 > y)) || ((y1 > y) && (y2 <= y)) ) {
#ifdef DEBUG
	   printf("   segment in proper y range x range: %f %f\n", x1, x2);
#endif
	    /*
	     * Determine if test point is to right of line segment.
	     * Lying on a horizontal line counts as a YES since the
	     * the 2 adjacent sides will match also and cancel each other.
	     */
	    if ( (x >= x1) && (x >= x2) ) count++;	/* easy test */
	    else if ( (x < x1) && (x < x2) );		/* to left */
	    else {
		/* Tougher case, find intercept position. */
		float slope;
		slope = (x2 - x1) / (y2 - y1);
		x1 = x1 + (slope * (y - y1));
		/* Test if we are 2 right of intercept position */
#ifdef DEBUG
		printf("Intercept x: %f y: %f test: %f\n", x1, y, x );
#endif
		if ( x >= x1 ) count++;
	    }
	} else if ( (y1 == y2) && (y == y1) ) {
	    /* Edge is horizontal, see if test point on line. */
	    if ( ((x >= x1) && (x <= x2)) || ((x <= x1) && (x >= x2)) ) {
		count = 1;		/* force successfull match */
		break;
	    }
	}
#ifdef DEBUG
	if ( count > prev ) {
	    printf("    point is right of segment\n"); prev = count;
	}
#endif
    }
    /*
     * Now we have the count, determine if we are inside the polygon.
     * We are inside if we are 2 right of an odd number of sides.
     */
#ifdef DEBUG
	printf("polygon cross count = %d: %s", count, match_url );
#endif
    if ( (count % 2) == 1 ) {
	tu_strnzcpy ( match_url, tok[npnts+1].s, 1023 );
    }
    return 1;
} 
/**************************************************************************/
/* Parse next directive out of input stream, value return is number of
 * tokens parsed 
 */
static int get_command ( stream ctx, 
	struct token_list *tok, int tok_limit, 
	char *tokbuf, int tokbufsize )
{
    int i, tokpos, length, status, get_token();
    char *tail;
    struct { char *keyword, *abbrev; int code; } cmds[] =
	{ { "default", "def", 1 }, { "rectangle", "rect", 2 },
	  { "circle", "circ", 3 }, { "polygon", "poly", 4 }, { "", "", -1 } };
    /*
     * Load first token into callers buffer.
     */
    tokpos = 0;
    tok[0].s = &tokbuf[tokpos];
    length = get_token ( ctx, tok[0].s, tokbufsize );
    if ( length <= 0 ) return length;
    tokpos += length;
    tokbuf[tokpos++] = '\0';	/* terminate string  and allocate */
    /*
     * Convert first token to code number.
     */
    tu_strlowcase ( tokbuf, tokbuf );

    for ( tok[0].type = i = 0; cmds[i].code > 0; i++ ) {
	if ( (tu_strncmp ( tokbuf, cmds[i].keyword, length+1 ) == 0) ||
	     (tu_strncmp ( tokbuf, cmds[i].abbrev, length+1 ) == 0) ) {
	    tok[0].type = cmds[i].code;
	    break;
	}
    }
    if ( tok[0].type == 0 ) return 1;
    /*
     * Load additional tokens.
     */
    for ( i = 1; i < tok_limit; i++ ) {
	/*
	 * Read next token from input file, update tokpos.
	 */
	tok[i].s = &tokbuf[tokpos];
	length = get_token ( ctx, tok[i].s, tokbufsize - tokpos-1 );
	if ( length <= 0 ) return i;
	tokpos += length;
	tokbuf[tokpos++] = '\0';
	/*
	 * Convert x,y pairs.  Tail should end up  pointing to closing ")".
	 */
	if ( *tok[i].s == '(' ) {
	    decode_float ( &tok[i].s[1], &tok[i].x, &tail );
	    if ( *tail == ',' ) decode_float ( &tail[1], &tok[i].y, &tail );
	    else tail = ",";
	    /* status = sscanf ( tok[i].s, "(%f,%f)", &tok[i].x, &tok[i].y );*/
	} else tail = "?";
	/*
	 * Interpret token in context of command type.
	 */
	switch ( tok[0].type ) {
	  case 1:		/* Default */
	    tok[i].type = -1;		/* only argument URL */
	    return i+1;

	  case 2:		/* Rectangle */
	    if ( i < 3 ) {
		tok[i].type = 0;
		if ( *tok[i].s != '(' ) 
		    tlog_putlog ( 0, "Syntax error in rect!/" );
	    } else {
		tok[i].type = -1;
		return i + 1;
	    }
	    break;

	  case 3:		/* Circle */
	    if ( i == 1 ) {
		tok[i].type = 0;
	    } else if ( i == 2 ) {
		decode_float ( tok[i].s, &tok[i].x, &tail );
		/* status = sscanf ( tok[i].s, "%f", &tok[i].x ); */
		tok[i].type = 0;
	    } else {
		tok[i].type = -1;
		return i + 1;
	    }
	    break;

	  case 4:		/* Polygon */
	    if ( *tok[i].s == '(' ) {
		tok[i].type = 0;
	    } else {
		tok[i].type = -1;		/* Last one */
		return i + 1;
	    }
        }
    }    

}
/**************************************************************************/
/* Parse next token out of input file. */
static int get_token ( stream ctx, char *token, int maxlen )
{
    int tlen, state, used, filled;
    char *linebuf, letter;
    /*
     * Skip whitespace and comments.
     */
    linebuf = ctx->buffer;
    used = ctx->used; filled = ctx->filled;
    for ( state = 0; state >= 0; used++ ) {
	while ( used >= filled ) {
	    used = 0;
	    filled = tf_read (ctx->ff, linebuf,	sizeof(ctx->buffer) );
	    if ( filled <= 0 ) {
		ctx->filled = -1;
		ctx->used = 0;
		return filled;
	    }
	}
        if ( state == 0 ) {
	    /* Look for first non-blank */
	    if ( !isspace(linebuf[used]) ) {
		if ( linebuf[used] == '#' ) state = 1;
	        else { break; }
	    }
	} else if ( state == 1 ) {
	    /* Comment line, look for line terminator */
	    if ( linebuf[used] == '\n' ) state = 0;	/* start at next line*/
	}
    }
    /*
     * Scan for end of token.
     */
    state = 0;
    if ( linebuf[used] == '(' ) state = 1;
    token[0] = linebuf[used++]; 
    for ( tlen = 1; tlen < maxlen; used++) {
	while ( used >= filled ) {
	    used = 0;
	    filled = tf_read (ctx->ff, linebuf, sizeof(ctx->buffer) );
	    if ( filled <= 0 ) {
		ctx->filled = -1;
		ctx->used = 0;
		return tlen;
	    }
	}
	letter = linebuf[used];
	if ( state == 0 ) {
	    /* Break on whitespace or append to token string */
	    if ( isspace(letter) ) break;
	    else token[tlen++] = letter;

        } else {
	    /* Break on closing ')', ignore whitespace */
	    if ( !isspace(letter) ) {
		token[tlen++] = letter;
		if ( letter == ')' ) {
		    used++;		/* don't re-scan the ')' next time */
		    break;
		}
	    }
        }
    }
    ctx->used = used;
    ctx->filled = filled;
    return tlen;
}
/*****************************************************************************/
/*
** mapper 1.2
** 7/26/93 Kevin Hughes, kevinh@pulua.hcc.hawaii.edu
** "macmartinized" polygon code copyright 1992 by Eric Haines, erich@eye.com
** All suggestions, help, etc. gratefully accepted!
**
** 1.1 : Better formatting, added better polygon code.
** 1.2 : Changed isname(), added config file specification.
**
** 11/13/93: Rob McCool, robm@ncsa.uiuc.edu
**
** Rewrote configuration stuff for NCSA /htbin script
**
** 12/05/93: Rob McCool, robm@ncsa.uiuc.edu
** 
** Made CGI/1.0 compliant.
**
** 06/27/94: Chris Hyams, cgh@rice.edu
**          Based on an idea by Rick Troth (troth@rice.edu)
**
** 04/05/95: David Jones, vman+@osu.edu
**	    Converted to DECthreads CGI environment.
**
** Imagemap configuration file in PATH_INFO.  Backwards compatible.
**
**  Old-style lookup in imagemap table:
**    <a href="http://foo.edu/cgi-bin/imagemap/oldmap">
**
**  New-style specification of mapfile relative to DocumentRoot:
**    <a href="http://foo.edu/cgi-bin/imagemap/path/for/new.map">
**
**  New-style specification of mapfile in user's public HTML directory:
**    <a href="http://foo.edu/cgi-bin/imagemap/~username/path/for/new.map">
**
** 07/11/94: Craig Milo Rogers, Rogers@ISI.Edu
**
** Added the "point" datatype.  The nearest point wins.  Overrides "default".
**
** 08/28/94: Carlos Varela, cvarela@ncsa.uiuc.edu
**
** Fixed bug:  virtual URLs are now understood.
** Better error reporting when not able to open configuration file.
*/

#ifndef VAXC
#include <sys/stat.h>
#else
#include <stat.h>
#endif

#ifndef CONF_FILE
#define CONF_FILE "/www_root/conf/imagemap.conf"
#endif

#define MAXLINE 500
#define MAXVERTS 100
#define X 0
#define Y 1
/*
 * Not sure what function getline is supposed to do.  Assume it works
 * like fgets (same arguments) but concatentates lines that end in backslash.
 * Return 0 on success.
 */
static int getline ( char *buf, int bufsize, void *ff )
{
    int i, status, line_len, len;
    char *cur;
    for ( i = 0; i < bufsize; ) {
	buf[i] = '\0';
	line_len = tf_getline ( ff, &buf[i], bufsize-i, 1 );
	if ( line_len <= 0 ) return (i>0) ? 0 : -1;
	cur = &buf[i];
	if ( !cur ) return (i > 0) ? 0 : -1;
	if ( line_len < 2 ) i += line_len;
	else i += (line_len-2);
	if ( i > 0 ) if ( buf[i-1] == '\\' ) {
	    buf[i-1] = ' ';	/* ensure line break counts as whitespace */
	    continue;
	}
	break;
    }
    return 0;
}

static int isname(char);
static int pointincircle(double point[2], double coords[MAXVERTS][2]);
static int pointinpoly(double point[2], double coords[MAXVERTS][2]);
static int pointinrect(double point[2], double coords[MAXVERTS][2]);

static int map_ncsa ( mst_link_t link, stream ctx, 
		float x, float y, char *match_url )
{
    char input[MAXLINE], def[MAXLINE], conf[MAXLINE];
    double testpoint[2], pointarray[MAXVERTS][2];
    int status, i, j, k;
    char *t, *tail, *query_string;
    double dist, mindist;
    int sawpoint = 0;

    testpoint[X] = x;
    testpoint[Y] = y;

    while(!(getline(input,MAXLINE,ctx->ff))) {
        char type[MAXLINE];
        char url[MAXLINE];
        char num[10];

        if((input[0] == '#') || (!input[0]))
            continue;

        type[0] = '\0';url[0] = '\0';

        for(i=0;isname(input[i]) && (input[i]);i++)
            type[i] = input[i];
        type[i] = '\0';

        while(isspace(input[i])) ++i;
        for(j=0;input[i] && isname(input[i]);++i,++j)
            url[j] = input[i];
        url[j] = '\0';

        if(!tu_strncmp(type,"default",8) && !sawpoint) {
            tu_strcpy(def,url);
            continue;
        }

        k=0;
        while (input[i]) {
            while (isspace(input[i]) || input[i] == ',')
                i++;
            j = 0;
            while (isdigit(input[i]))
                num[j++] = input[i++];
            num[j] = '\0';
            if (num[0] != '\0') {
		decode_float ( num, &x, &tail );
                pointarray[k][X] = (double) x;
            } else
                break;
            while (isspace(input[i]) || input[i] == ',')
                i++;
            j = 0;
            while (isdigit(input[i]))
                num[j++] = input[i++];
            num[j] = '\0';
            if (num[0] != '\0') {
		decode_float ( num, &y, &tail );
                pointarray[k++][Y] = (double) y;
            } else {
		error_abort ( link, "400 query error", "Missing y value" );
            }
        }
        pointarray[k][X] = -1;
        if(!tu_strncmp(type,"poly",5))
            if(pointinpoly(testpoint,pointarray)) {
                tu_strnzcpy ( match_url, url, 1023 ); return 1;
	    }
        if(!tu_strncmp(type,"circle",7))
            if(pointincircle(testpoint,pointarray)) {
                tu_strnzcpy ( match_url, url, 1023 ); return 1;
	    }
        if(!tu_strncmp(type,"rect",5))
            if(pointinrect(testpoint,pointarray)) {
                tu_strnzcpy ( match_url, url, 1023 ); return 1;
	    }
        if(!tu_strncmp(type,"point",6)) {
	    /* Don't need to take square root. */
	    dist = ((testpoint[X] - pointarray[0][X])
		    * (testpoint[X] - pointarray[0][X]))
		   + ((testpoint[Y] - pointarray[0][Y])
		      * (testpoint[Y] - pointarray[0][Y]));
	    /* If this is the first point, or the nearest, set the default. */
	    if ((! sawpoint) || (dist < mindist)) {
		mindist = dist;
	        tu_strcpy(def,url);
	    }
	    sawpoint++;
	}
    }
    if(def[0]) tu_strnzcpy ( match_url, def, 1023 );
    else error_abort ( link, "400 map file error", "No default specified.");
}

int pointinrect(double point[2], double coords[MAXVERTS][2])
{
        return ((point[X] >= coords[0][X] && point[X] <= coords[1][X]) &&
        (point[Y] >= coords[0][Y] && point[Y] <= coords[1][Y]));
}

int pointincircle(double point[2], double coords[MAXVERTS][2])
{
        int radius1, radius2;

        radius1 = ((coords[0][Y] - coords[1][Y]) * (coords[0][Y] -
        coords[1][Y])) + ((coords[0][X] - coords[1][X]) * (coords[0][X] -
        coords[1][X]));
        radius2 = ((coords[0][Y] - point[Y]) * (coords[0][Y] - point[Y])) +
        ((coords[0][X] - point[X]) * (coords[0][X] - point[X]));
        return (radius2 <= radius1);
}

int pointinpoly(double point[2], double pgon[MAXVERTS][2])
{
        int i, numverts, inside_flag, xflag0;
        int crossings;
        double *p, *stop;
        double tx, ty, y;

        for (i = 0; pgon[i][X] != -1 && i < MAXVERTS; i++)
                ;
        numverts = i;
        crossings = 0;

        tx = point[X];
        ty = point[Y];
        y = pgon[numverts - 1][Y];

        p = (double *) pgon + 1;
        if ((y >= ty) != (*p >= ty)) {
                if ((xflag0 = (pgon[numverts - 1][X] >= tx)) ==
                (*(double *) pgon >= tx)) {
                        if (xflag0)
                                crossings++;
                }
                else {
                        crossings += (pgon[numverts - 1][X] - (y - ty) *
                        (*(double *) pgon - pgon[numverts - 1][X]) /
                        (*p - y)) >= tx;
                }
        }

        stop = pgon[numverts];

        for (y = *p, p += 2; p < stop; y = *p, p += 2) {
                if (y >= ty) {
                        while ((p < stop) && (*p >= ty))
                                p += 2;
                        if (p >= stop)
                                break;
                        if ((xflag0 = (*(p - 3) >= tx)) == (*(p - 1) >= tx)) {
                                if (xflag0)
                                        crossings++;
                        }
                        else {
                                crossings += (*(p - 3) - (*(p - 2) - ty) *
                                (*(p - 1) - *(p - 3)) / (*p - *(p - 2))) >= tx;
                        }
                }
                else {
                        while ((p < stop) && (*p < ty))
                                p += 2;
                        if (p >= stop)
                                break;
                        if ((xflag0 = (*(p - 3) >= tx)) == (*(p - 1) >= tx)) {
                                if (xflag0)
                                        crossings++;
                        }
                        else {
                                crossings += (*(p - 3) - (*(p - 2) - ty) *
                                (*(p - 1) - *(p - 3)) / (*p - *(p - 2))) >= tx;
                        }
                }
        }
        inside_flag = crossings & 0x01;
        return (inside_flag);
}

static int isname(char c)
{
        return (!isspace(c));
}



