/*
 * This module is an MST server script for the DECthread HTTP_SERVER.  It
 * provides support for clickable images, mapping regions defined in a
 * configuration file to URLs.
 *
 * The location of the configuration file is derived from the path string 
 * following the /$mapimage in the triggering URL, optionally prefixed by a 
 * string defined in the server's configuration (rule) file.  This location
 * is translated by the server's rule set to get the actual filename
 * of the image configuration file.
 *
 * The mapimage_mst routine is called as the main routine for a service thread
 * created at the request of a client thread.  It uses functions provided
 * by the message_service module read and write messages to the client
 * thread using the scriptserver (WWWEXEC) protocol.
 *
 * Image config file format:
 *    Comment lines begin with "#", otherwise map entries start with
 *    one of the keywords(abrev): default(def), circle(circ), 
 *    rectangle(rect), polygon(poly).
 *
 *    default URL
 *	Default URL to return if point not within any of the other
 *	figures.
 *
 *    circle (cx,cy) r URL
 *	Return URL if point is within circle.
 *
 *    rectangle (x1,y1) (x2,y2) URL
 *	Retrun URL if point is within rectangle defined by any two
 *      oposing corners.
 *
 *    polygon (x1,y1) ... (xn,yn) URL
 *	Return URL if point is within polygon defined by list of
 *	points.
 *
 *  Author:	David Jones
 *  Date:	19-MAR-1994
 *  Revised:	20-JUL-1994		Add xlate operation to inp file.
 *  Revised:    27-SEP-1994		Support case sensitive paths.
 *  Revised:    13-JAN-1994		Make default work indpendant of position.
 *  Revised:     6-FEB-1995		Accept + as coordinate delimiter.
 *  Revised:	24-MAR-1995		Convert from WWWEXEC-based script
 *					to MST script.
 *  Revised:	31-MAR-1995		Change "location:" to "Location".
 *  Revised:	11-MAY-1995		Declare sqrt function using math.h.
 *  Revised:	23-JUN-1995		Fix bugs in check_polygon routine.
 */ 
#include <stdlib.h>
#include <math.h>
/* #include <stdio.h> */
#include "tutil.h"
#include "file_access.h"
#ifdef DYNAMIC_MST
#include "mst_share.h"	/* mst support routines */
#else
#include "message_service.h"
int http_log_level, tlog_putlog();
#endif

#define isspace(a) ((a==' ')||(a=='\t')||(a=='\n')||(a=='\r'))

struct stream_ctx {
    void *ff;			/* Input file */
    int used, filled;
    char buffer[4096];
};
typedef struct stream_ctx *stream;

struct token_list {
   int type;			/* -1: URL, 0: text, 1: default, 2: rect
				    3: circle, 4: polygon  */
   char *s;			/* Text pointer */
   float x, y;
};
static int error_abort ( mst_link_t link, char *status_line, char *message );
static int check_polygon(), get_command(), get_token();
static int decode_float ( char *string, float *result, char **tail );
static void cgi_relocate_string ( char *out, char *url )
{
    int i, j;
    tu_strcpy ( out, "Location: " );
    for ( i = 0; (i < 1000) && url[i]; i++ ) out[i+10] = url[i];
    tu_strcpy ( &out[i+10], "\r\n\r\n" );
}
/********************************************************************/
/* Init routine for use when this MST is a dynamically loaded shareable image.
 */
#ifdef DYNAMIC_MST
int mapimage_init ( mst_linkage vector, char *info, char *errmsg )
{
    int status;
    status = mstshr_init ( vector, info, errmsg );
    if ( (status&1) == 0 ) return status;
    tf_initialize("");
    return 1;
}
#endif
/********************************************************************/
/* Main routine, called by shell start routine for newly create service
 * thread.
 */
int mapimage_mst
	( mst_link_t link, char *service, char *info, int ndx, int avail )
{
    int argc; char *argv[5], *prolog[5];
    int tcnt, length, length2, i, j, status, xlate_len;
    float x, y, d, min_x, max_x, min_y, max_y, dx, dy;
    char *inp, *tail, arg_buf[2048], search_arg[256], tokbuf[4096];
    char match_url[1024], bin_path[256], xlate_buf[256], errmsg[256];
    struct stream_ctx local_ctx;
    stream ctx;
    struct token_list tok[512];
    /*
     * Read prologue sent by server and build fake argv.
     */
    if ( http_log_level > 0 ) tlog_putlog ( 1, 
	"Service!AZ/!SL connected, info: '!AZ', pool remaining: !SL!/", 
	service, ndx, info, avail );
    j = 0;		/* portion of arg_buf filled */
    for ( i = 0; i < 4; i++ ) {
	prolog[i] = &arg_buf[j];
	status = mst_read ( link, prolog[i], sizeof(arg_buf)-j, &length );
	if ( status != MST_NORMAL ) break;
	arg_buf[j+length] = '\0';
	j = j + length + 1;
    }
    if ( i < 4 ) return 1;
    argc = i;
    argv[0] = "mapimage_mst";
    argv[1] = prolog[1];
    argv[2] = prolog[3];
    argv[3] = prolog[2];
    /*
     * Verify that URL starts with the appropriate path, then trim
     * off path and script name from the URL.
     */
    status = mst_write ( link, "<DNETPATH>", 10, &length );
    if ( status == MST_NORMAL )  status = mst_read 
		( link, bin_path, sizeof(bin_path)-1, &length );
    if ( status != MST_NORMAL ) return status;

    bin_path[length] = '\0';		/* terminate string */

    if ( bin_path[0] == '/' ) {
        if ( 0 != tu_strncmp(argv[2], bin_path, length) ) {
	   tlog_putlog(0,
	       "Invalid URL (!AZ), must start with !AZ!/",argv[2], bin_path);
	    return 0;
	}
        for ( inp = &argv[2][length-1]; *inp && (*inp != '/'); inp++ );
	if ( *info ) {
	    /* Construct path in temporary space */
	    for ( i = 0; (i < 250) && info[i]; i++ ) xlate_buf[i] = info[i];
	    tu_strnzcpy ( &xlate_buf[i], inp, 254 - i );
	    inp = xlate_buf;
	}
        /*
         * Translate the path.
         */
        status = mst_write ( link, "<DNETXLATE>", 11, &length2 );
        if ( (status&1) == 0 ) { 
	    tlog_putlog(0,"Network I/O failure in mapimage!/"); return status;}
        status = mst_write ( link, inp, tu_strlen(inp), &length2 );
	status = mst_read ( link, xlate_buf, sizeof(xlate_buf)-1, &xlate_len);
        if ( (status&1) == 0 ) { 
	    tlog_putlog(0,"Network I/O failure in mapimage!/"); return status;}
        xlate_buf[xlate_len] = '\0';
        inp = xlate_buf;
    } else {
	/*  Script invoked via presentation rule, use ident as passed */
	inp = argv[2];
    }
    /*
     * Retrieve search string and decode coordinates.
     */
    status = mst_write ( link, "<DNETARG2>", 10, &length2 );
    status = mst_read ( link, search_arg, sizeof(search_arg)-1, &length );
    if ( (status&1) == 0 ) { 
	    tlog_putlog(0,"Network I/O failure in mapimage!/"); return status;}
    if ( length < 0 ) return 1;
    search_arg[length] = '\0';
    for ( i = 0; search_arg[i]; i++ ) if ( search_arg[i] == ',' ) break;
    if ( !search_arg[i] ) {
	/* Search argument doesn't have ',', look for '+' and convert */
	for ( i = 0; search_arg[i]; i++ ) if ( search_arg[i] == '+' ) {
	    search_arg[i] = ',';
	    break;
	}
    }
    if ( *search_arg == '?' ) {
	decode_float ( &search_arg[1], &x, &tail );
	if ( *tail == ',' ) {
	    i = decode_float ( &tail[1], &y, &tail );
	    if ( isspace(*tail) ) tail = "";
	}
    } else tail = "?";
    if ( *tail != '\0' ) {
    	/* if ( 2 != sscanf ( search_arg, "?%f,%f", &x, &y ) ) { */
	error_abort ( link, "500 invalid request", 
		"Error decoding map position arguments." );
    }
    /*
     * Initialize structure for reading input file and open file.
     */
    ctx = &local_ctx;
    ctx->used = ctx->filled = 0;
    ctx->ff = tf_open ( inp, "r", errmsg );  /* configuration file */
    if ( !ctx->ff ) {
	error_abort ( link, "404 open failure", "Open failure on file" );
    }
    /*
     * Process file  test every figure to allow for overlapping regions.
     */
    match_url[0] = '\0';		/* default to no match */
    while ( (tcnt = 
		get_command ( ctx, tok, 512, tokbuf, sizeof(tokbuf)))>1) {
	switch ( tok[0].type ) {
	    case 1:		/* Default if no match */
		if ( match_url[0] == '\0' )
		    cgi_relocate_string ( match_url, tok[1].s );
		break;

	    case 2:		/* rectangle */
		min_x = tok[1].x < tok[2].x ? tok[1].x : tok[2].x;
		max_x = tok[1].x > tok[2].x ? tok[1].x : tok[2].x;
		min_y = tok[1].y < tok[2].y ? tok[1].y : tok[2].y;
		max_y = tok[1].y > tok[2].y ? tok[1].y : tok[2].y;
		if ( (x >= min_x) && (x <= max_x) &&
		    (y >= min_y) && (y <= max_y) ) {
		    cgi_relocate_string ( match_url, tok[3].s );
		}
		break;

	    case 3:		/* circle */
		dx = x - tok[1].x;
		dy = y - tok[1].y;
		if ( sqrt ( (dx*dx) + (dy*dy) ) <= tok[2].x ) {
		    cgi_relocate_string ( match_url, tok[3].s );
		}
		break;

	    case 4:		/* polygon */
		check_polygon ( tok, match_url, x, y );
		break;
	}
    }
    tf_close ( ctx->ff );
    /*
     * Either send error message or send relocate.
     */
    if ( *match_url ) {
	/*
	 * Send relocate.
	 */
	status = mst_write ( link, "<DNETCGI>", 9, &length2 );
	status = mst_write ( link, match_url, tu_strlen(match_url), &length2 );
	status = mst_write ( link, "</DNETCGI>", 10, &length2 );
    } else {
	error_abort ( link, "500 no match", "No translation for image point" );
    }
    status = mst_close(link);
    return 1;
}

static int error_abort ( mst_link_t link, char *status_line, char *message )
{
    int written, length, status;
    static int exit_status;
    status = mst_write ( link, "<DNETTEXT>", 10, &written );
    status = mst_write ( link, status_line, tu_strlen(status_line), &written );
    status = mst_write ( link, message, tu_strlen(message), &written );
    status = mst_write ( link, "</DNETTEXT>", 11, &written );

    status = mst_close ( link );
    exit_status = -1;
    return mst_exit ( &exit_status );
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
	    /* 
	     * Edge is horizontal, see if test point on line and force
	     * match.  Otherwise endpoint test for the adjacent lines will
	     * determine crossings.
	     */
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
	cgi_relocate_string ( match_url, tok[npnts+1].s );
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
/***************************************************************************/
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
