/*
 * This program is a server script for the DECthread HTTP_SERVER.  It
 * provides support for clickable images, mapping regions defined in a
 * configuration file to URLs.
 *
 * This program assumes that it was invoked by the WWWEXEC decnet task and
 * the logical dnet_link is a process permanent file connected to the
 * the HTTP server.
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
 */ 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "scriptlib.h"		/* net_link_... routines */

#define CGI_RELOCATE "location: %s\r\n\r\n"

struct stream_ctx {
    FILE *f;			/* Input file */
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
static int error_abort ( char *status_line, char *message );
static int check_polygon(), get_command(), get_token();
/********************************************************************/
/* Main routine.
 */
int main ( int argc, char **argv )
{
    int tcnt, length, i, status, xlate_len;
    float x, y, d, min_x, max_x, min_y, max_y, dx, dy, sqrt();
    char *inp, search_arg[256], tokbuf[4096], match_url[4096];
    char bin_path[256], xlate_buf[256];
    stream ctx;
    struct token_list tok[512];
    /*
     * Validate arguments command line arguments.  The config file name
     * is appended to the script name that invokes this program.
     */
    if ( argc < 3 ) {
	fprintf( stderr, "Missing arguments, usage: prog method url\n");
	exit ( 1 );
    }
    /*
     * Open network link and get search/point argument.  Note that network
     * connection is record oriented, a zero length record is allowed.
     */
    status = net_link_open();
    if ( (status&1) == 0 ) { perror("Network connect fail"); exit (status); }
    /*
     * Verify that URL starts with the appropriate path, then trim
     * off path and script name from the URL.
     */
    status = net_link_query 
		( "<DNETPATH>", bin_path, sizeof(bin_path)-1, &length );
    if ( (status&1) == 0 ) { perror("Network I/O failure"); exit (status); }
    if ( length < 0 ) exit ( 1 );

    bin_path[length] = '\0';		/* terminate string */

    if ( bin_path[0] == '/' ) {
        if ( 0 != strncmp(argv[2], bin_path, length) ) {
	   fprintf( stderr, 
	       "Invalid URL (%s), must start with %s\n",argv[2], bin_path);
	    exit ( 1 );
	}
        for ( inp = &argv[2][length]; *inp && (*inp != '/'); inp++ );
        /*
         * Translate the path.
         */
        status = net_link_write ( "<DNETXLATE>", 11 );
        if ( (status&1) == 0 ) { perror("Network I/O failure"); exit (status); }
        status = net_link_query ( inp, xlate_buf, sizeof(xlate_buf)-1, &xlate_len );
        if ( (status&1) == 0 ) { perror("Network I/O failure"); exit (status); }
        xlate_buf[xlate_len] = '\0';
        inp = xlate_buf;
    } else {
	/*  Script invoked via presenation rule, use ident as passed */
	inp = argv[2];
    }
    /*
     * Retrieve search string and decode coordinates.
     */
    status = net_link_query 
		( "<DNETARG2>", search_arg, sizeof(search_arg)-1, &length );
    if ( (status&1) == 0 ) { perror("Network I/O failure"); exit (status); }
    if ( length < 0 ) exit ( 1 );
    search_arg[length] = '\0';
    if ( !strchr ( search_arg, ',' ) ) {
	/* Search argument doesn't have ',', look for '+' and convert */
	for ( i = 0; search_arg[i]; i++ ) if ( search_arg[i] == '+' ) {
	    search_arg[i] = ',';
	    break;
	}
    }
    if ( 2 != sscanf ( search_arg, "?%f,%f", &x, &y ) ) {
	printf("Bad search arguments: %s\n", search_arg );
	error_abort ( "500 invalid request", 
		"Error decoding map position arguments." );
    }
    /*
     * Initialize structure for reading input file and open file.
     */
    ctx = (stream) malloc ( sizeof(struct stream_ctx) );
    
    ctx->used = ctx->filled = 0;
    ctx->f = fopen ( inp, "r", "mbc=32" );  /* configuration file */
    if ( !ctx->f ) {
	error_abort ( "404 open failure", "Open failure on file" );
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
			sprintf ( match_url, CGI_RELOCATE, tok[1].s );
		break;

	    case 2:		/* rectangle */
		min_x = tok[1].x < tok[2].x ? tok[1].x : tok[2].x;
		max_x = tok[1].x > tok[2].x ? tok[1].x : tok[2].x;
		min_y = tok[1].y < tok[2].y ? tok[1].y : tok[2].y;
		max_y = tok[1].y > tok[2].y ? tok[1].y : tok[2].y;
		if ( (x >= min_x) && (x <= max_x) &&
		    (y >= min_y) && (y <= max_y) ) {
		    sprintf ( match_url, CGI_RELOCATE, tok[3].s );
		}
		break;

	    case 3:		/* circle */
		dx = x - tok[1].x;
		dy = y - tok[1].y;
		if ( sqrt ( (dx*dx) + (dy*dy) ) <= tok[2].x ) {
		    sprintf ( match_url, CGI_RELOCATE, tok[3].s );
		}
		break;

	    case 4:		/* polygon */
		check_polygon ( &tok, match_url, x, y );
		break;
	}
    }
    fclose ( ctx->f );
    /*
     * Either send error message or send relocate.
     */
    if ( *match_url ) {
	/*
	 * Send relocate.
	 */
	status = net_link_write ( "<DNETCGI>", 9 );
	status = net_link_write ( match_url, strlen(match_url) );
	status = net_link_write ( "</DNETCGI>", 10 );
    } else {
	error_abort ( "500 no match", "No translation for image point" );
    }
    status = net_link_close ();
    return 1;
}

static int error_abort ( char *status_line, char *message )
{
    int written, length, status;
    status = net_link_write ( "<DNETTEXT>", 10 );
    status = net_link_write ( status_line, strlen(status_line) );
    status = net_link_write ( message, strlen(message) );
    status = net_link_write ( "</DNETTEXT>", 11 );
    status = net_link_close();
    exit ( 1 );
    return 1;			/* Keep the compiler happy */
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
	if ( ((y1 <= y) && (y2 > y)) || ((y1 >= y) && (y2 < y)) ) {
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
	    else if ( y1 == y2 ) count++;		/* on horiz. line */
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
	    } else if ( (x >= x1) && (x >= x2) ) count++;
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
	sprintf ( match_url, CGI_RELOCATE, tok[npnts+1].s );
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
    for ( i = 0; i < length; i++ ) tokbuf[i] = _tolower ( tokbuf[i] );

    for ( tok[0].type = i = 0; cmds[i].code > 0; i++ ) {
	if ( (strcmp ( tokbuf, cmds[i].keyword ) == 0) ||
	     (strcmp ( tokbuf, cmds[i].abbrev ) == 0) ) {
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
	 * Convert x,y pairs.
	 */
	if ( *tok[i].s == '(' ) {
	    status = sscanf ( tok[i].s, "(%f,%f)", &tok[i].x, &tok[i].y );
	}
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
		if ( *tok[i].s != '(' ) printf ( "Syntax error in rect\n" );
	    } else {
		tok[i].type = -1;
		return i + 1;
	    }
	    break;

	  case 3:		/* Circle */
	    if ( i == 1 ) {
		tok[i].type = 0;
	    } else if ( i == 2 ) {
		status = sscanf ( tok[i].s, "%f", &tok[i].x );
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
     * Skip whitespace
     */
    linebuf = ctx->buffer;
    used = ctx->used; filled = ctx->filled;
    for ( state = 0; state >= 0; used++ ) {
	while ( used >= filled ) {
	    used = 0;
	    filled = fread (linebuf, sizeof(char), sizeof(ctx->buffer),ctx->f);
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
	    filled = fread (linebuf, sizeof(char), sizeof(ctx->buffer),ctx->f);
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
