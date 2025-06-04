#ifdef	VMS
#module XWOODWARD "X1.4"
#endif	/* VMS */

/*
*****************************************************************************
*                                                                           *
*  COPYRIGHT (c) 1988, 1990  BY                                             *
*  DIGITAL EQUIPMENT CORPORATION, MAYNARD, MASSACHUSETTS.                   *
*  ALL RIGHTS RESERVED.                                                     *
*                                                                           *
*  THIS SOFTWARE IS FURNISHED UNDER A LICENSE AND MAY BE USED AND COPIED    *
*  ONLY IN  ACCORDANCE WITH  THE  TERMS  OF  SUCH  LICENSE  AND WITH THE    *
*  INCLUSION OF THE ABOVE COPYRIGHT NOTICE. THIS SOFTWARE OR  ANY  OTHER    *
*  COPIES THEREOF MAY NOT BE PROVIDED OR OTHERWISE MADE AVAILABLE TO ANY    *
*  OTHER PERSON.  NO TITLE TO AND OWNERSHIP OF  THE  SOFTWARE IS  HEREBY    *
*  TRANSFERRED.                                                             * 
*                                                                           *
*  THE INFORMATION IN THIS SOFTWARE IS  SUBJECT TO CHANGE WITHOUT NOTICE    *
*  AND  SHOULD  NOT  BE  CONSTRUED AS  A COMMITMENT BY DIGITAL EQUIPMENT    *
*  CORPORATION.                                                             *
*                                                                           *
*  DIGITAL ASSUMES NO RESPONSIBILITY FOR THE USE  OR  RELIABILITY OF ITS    *
*  SOFTWARE ON EQUIPMENT WHICH IS NOT SUPPLIED BY DIGITAL.                  *
*                                                                           *
*****************************************************************************
*/

/*
**++
**  FACILITY:
**
**      XWoodward   - Report generator for XLiddy
**
**  ABSTRACT:
**
**      XWoodward is the companion program to XLiddy.  It takes the
**	data recorded by Xliddy and formats it in a human-readable
**	format.  Currently, it understands core requests, replies and
**	events and formats these accordingly.  Extension requests, replies
**	and events and all errors are formatted in typical dump
**	fashion.
**
**	There are no guarantees of the quality of the protocol decode:
**	the formatting code was generated from protocol documentation,
**	structures in XProto.H and the current XLib implementation.
**
**  AUTHORS:
**
**      Monty C. Brandenberg
**
**
**  CREATION DATE:     August 18, 1987
**
**  MODIFICATION HISTORY:
**
**	X1.4	MCB0003	    Monty Brandenberg	28-Mar-1989
**		Add byte-swapping for endian-mismatched clients.  Xliddy and
**		xwoodward must be run on same-endian machines.
**
**	X1.3	MCB0002	    Monty Brandenberg	10-Nov-1987
**		Fix the parsing of the connection setup information so that
**		the GPX doesn't blow up.  Original parsing was complete 
**		nonsense.  Also, update to REL1 protocol:  affects
**		ListFontsWithInfo, ConfigureNotify/Request?, xxModifierMappings.
**		"Modernized" my ridiculously inept pointer arithmetic.
**		Use the xDiddle structures (point, rectangle, etc.) instead of
**		the XDiddle.  This might fix the fencepost errors for the
**		PolyDiddle function dumps.
**
**	X1.2	MCB0001	    Monty Brandenberg	14-Sep-1987
**		Give timestamp output a flag to enable it.  Fix the
**		GetProperty routine to handle the various data types
**		intelligently and to correct for a server bug (length
**		field is calculated incorrectly).
**
**	X1.1	JB0001	    John Buford		28-Aug-1987
**		Added VMS timestamps to the log file.  Added support for
**		a brief report. Changed "CARD8 lengthReason" to "CARD16 length"
**		in proc_server_session_rec.
**
**--
**/


/*
**
**  INCLUDE FILES
**
**/

#include    <stdio.h>
#include    <descrip.h>
#include    <file.h>
#include    <errno.h>
#include    <perror.h>
#include    <ctype.h>
/*
#include    <stdlib.h>
#include    <string.h>
*/
/*
 * These defines are required by XProto.h
 */
#define	    NEED_EVENTS
#define	    NEED_REPLIES                                
#define	    X11_REL1_PROTOCOL

#include    "X.h"
#include    "Xproto.h"
#include    "Xlib.h"
#include    "xl.h"



/*
**
**  MACRO DEFINITIONS
**
**/

#define	    VMSERR( _expr_ )	if (((status=(_expr_))&1)==0) lib$signal( status )
#define	    min(x,y)		( (x)<(y)?(x):(y))
#define	    max(x,y)		( (x)>(y)?(x):(y))


#define	    INITIAL_BUFSIZE	2048
#define	    MAX_BUFSIZE		65536
#define	    MAX_PAR_SESSIONS	16
#define	    EXIT_SUCCESS	1
#define	    EXIT_FAILURE	0

#define	    FETCH_CARD16( _i_ )	(pcs->sr_swapped?(unsigned short)((((_i_)<<8)&0xff00)|(((_i_)>>8)&0xff)):(unsigned short) (_i_))
#define	    FETCH_INT16( _i_ )	(pcs->sr_swapped?(short)((((_i_)<<8)&0xff00)|(((_i_)>>8)&0xff)):(short) (_i_))
#define	    FETCH_CARD32( _i_ )	(pcs->sr_swapped?(unsigned long)((((_i_)<<24)&0xff000000)|(((_i_)<<8)&0xff0000)|\
				    (((_i_)>>8)&0xff00)|(((_i_)>>24)&0xff)):(unsigned long) (_i_))
#define	    FETCH_INT32( _i_ )	(pcs->sr_swapped?(long)((((_i_)<<24)&0xff000000)|(((_i_)<<8)&0xff0000)|\
				    (((_i_)>>8)&0xff00)|(((_i_)>>24)&0xff)):(long) (_i_))

static char *xw_infilename = "sys$input",
	    *xw_outfilename = "sys$output";
static int  xw_session = 0,		    /* if single_session is true, session number to search for */
	    xw_single_session = 0,	    /* if non-zero, looking for single session */
	    xw_form_wide = 1,		    /* wide format */
	    xw_form_pretty = 0,		    /* pretty format */
	    xw_form_brief = 0,		    /* brief format */
	    xw_form_timestamp = 0,	    /* generate timestamps on output */
	    xw_form_deltatime = 0,	    /* generate delta times */
	    xw_max_bufsize = MAX_BUFSIZE;   /* maximum allowed request or reply size */

extern int  optind;
extern char *optarg;
static int  in_file_seen = 0,
	    out_file_seen = 0,
	    in_fd;
static FILE *out_fp;

static char *file_buf = NULL,
	    *file_bufpos,
	    *file_bufmax;
static int  file_buf_siz = 0,
	    file_bufcnt;

static char *progname = "Xwoodward";
static char string_buffer[256];

typedef struct session_rec {
    int	    sr_number;		    /* session number */
    int	    sr_last_rec;	    /* type of last record */
    int	    sr_cbuf_siz;	    /* current size of client buffer */
    char    *sr_cbuf;		    /* current client buffer */
    char    *sr_cbufpos;	    /* current parsing position in client buffer */
    int	    sr_cbufcnt;		    /* number of bytes in client buffer */
    int	    sr_c_bytes_needed;	    /* number of bytes needed to complete packet */
    int	    sr_c_state;		    /* state of client */
#define	    XW_CLI_IN_START	0
#define	    XW_CLI_IN_CONNSETUP	1
#define	    XW_CLI_IN_GET_HDR	2
#define	    XW_CLI_IN_GET_REQ	3
#define	    XW_CLI_IN_CLOSE	4
    int	    sr_sbuf_siz;	    /* current size of server buffer */
    char    *sr_sbuf;		    /* current server buffer */
    char    *sr_sbufpos;	    /* current parsing position in server buffer */
    int	    sr_sbufcnt;		    /* number of bytes in server buffer */
    int	    sr_s_bytes_needed;	    /* number of bytes needed to complete packet */
    int	    sr_s_state;		    /* state of server */
#define	    XW_SRV_IN_START	0
#define	    XW_SRV_IN_CONNSETUP	1
#define	    XW_SRV_IN_GET_HDR	2
#define	    XW_SRV_IN_GET_REPLY	3
#define	    XW_SRV_IN_CLOSE	4
    int	    sr_c_seq_number;
    int	    sr_last_request;
    unsigned long
	    sr_basetime[2];	    /* 64 bit timevalue generated at connection initiation */
    int	    sr_swapped : 1;	    /* Client data is swapped */
} SESSION_T;

static SESSION_T session[MAX_PAR_SESSIONS],
		 *pcs = NULL;	    /* pointer to current session record */

static int	cs = -1;	    /* current session */
static char	endian;		    /* endian sense of xwoodward */

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      main
**
**  FORMAL PARAMETERS:
**
**      int argc	number of command line arguments
**	char *argv[]	vector of pointers to arguments
**
**  IMPLICIT INPUTS:
**
**      none
**
**  IMPLICIT OUTPUTS:
**
**      none
**
**  COMPLETION CODES:
**
**      1 on success, 0 on failure
**
**  SIDE EFFECTS:
**
**      none
**
**--
**/
main(argc, argv)
int	argc;
char	*argv[];
{
    int	    status, chan, read_mode = 0;
    char    line[80];
    int	    i, j, k;
    char    option,
	    getopt();
/*
 * parse command arguments
 */

    if ( argc < 2 ) {
	print_usage();
	exit( EXIT_SUCCESS );
    }

    while ( (option = getopt( argc, argv, "wnrpbtTi:o:s:m:" )) != EOF ) {
	switch (option) {
case 's':   if ( sscanf( optarg, "%d", &status ) != 1 ) {
		fprintf( stderr,  "[Bad session number]\n" );
		exit( EXIT_FAILURE );
	    }
	    xw_session = status;
	    xw_single_session = 1;
	    break;

case 'i':   if ( in_file_seen ) {
		fprintf( stderr, "[Input filename already seen]\n" );
	    } else {
		in_file_seen = 1;
		xw_infilename = optarg;
		if ( (in_fd = open( xw_infilename, O_RDONLY, 0, "rfm=udf" ) ) < 0 ) {
		    fprintf( stderr, "[Could not open input file %s]\n", xw_infilename);
		    perror( progname );
		    exit( EXIT_FAILURE );
		}
	    }
	    break;

case 'o':   if ( out_file_seen ) {
		fprintf( stderr, "[Output filename already seen]\n" );
	    } else {
		out_file_seen = 1;
		xw_outfilename = optarg;
		if ( (out_fp = fopen( xw_outfilename, "w" ) ) == NULL ) {
		    fprintf( stderr, "[Could not open output file %s]\n", xw_outfilename);
		    perror( progname );
		    exit( EXIT_FAILURE );
		}
	    }
	    break;

case 'w':   xw_form_wide = 1; break;

case 'n':   xw_form_wide = 0; break;

case 'r':   xw_form_pretty = 0; break;

case 'p':   fprintf( stderr, "[Option not implemented.  Continuing]\n" ); break;

case 'b':   xw_form_brief = 1; break;

case 't':   xw_form_deltatime = 1;	    /* DOES NOT BREAK */
case 'T':   xw_form_timestamp = 1; break;

case 'm':   if ( sscanf( optarg, "%d", &status ) != 1 ) {
		fprintf( stderr,  "[Bad buffer size]\n" );
		exit( EXIT_FAILURE );
	    }
	    xw_max_bufsize = status;
	    break;

case '?':   print_usage();
	    exit( EXIT_SUCCESS );
	}
    }

/*
 * Pick up anything not specified
 */
    if ( !in_file_seen ) {
	if ( (in_fd = open( xw_infilename, O_RDONLY, 0, "rfm=udf" ) ) < 0 ) {
	    fprintf( stderr, "[Could not open input file %s]\n", xw_infilename);
	    perror( progname );
	    exit( EXIT_FAILURE );
	}
    }
    if ( !out_file_seen ) {
	out_fp = stdout;
    }
    if ( (file_buf = malloc( INITIAL_BUFSIZE ) ) == NULL ) {
	perror( progname );
	exit( EXIT_FAILURE );
    }
    file_buf_siz = INITIAL_BUFSIZE;

    for ( i=0; i<MAX_PAR_SESSIONS; i++ ) {
	session[i].sr_number = -1;
    }                        

/*
 * Get endian sense of machine xwoodward is running on
 */
    {
	long	a_longword = 1;

	if ( *(char *)&a_longword == a_longword ) {
	    endian = 'l';
	}
	else {
	    endian = 'B';
	}
    }
/*
 * Okay, let's scan
 */

    if ( xw_form_pretty ) {
	pretty_scan_file();
    } else {
	pretty_scan_file();
    }
}

print_usage() {
    fprintf( stderr, "\n\n\t\tXWoodward - X Protocol Wire Reporter\n\n\
Generates reports from the output of XLiddy\n\n\
Usage:	xwoodward [-r    raw data dump]  [-p    pretty data dump] \n\
                  [-s    session of interest]\n\
		  [-t(T) delta (absolute) timestamps]\n\
                  [-b    brief listing]  [-m    maximum packet size]\n\
                  [-w    wide format]    [-n    narrow format]\n\
		  [-i    input file]     [-o    formatted output file]\n");

}


int
find_session_index( s )
int	s;
{
    int	    i;

    for ( i=0; i<MAX_PAR_SESSIONS; i++ ) {
	if ( session[i].sr_number == s ) {
	    return (i);
	}
    }
    return (-1);
}


int
alloc_session_rec()
{
    int	    i;

    for ( i=0; i<MAX_PAR_SESSIONS; i++ ) {
	if ( session[i].sr_number == -1 ) {
	    return (i);
	}
    }
    return (-1);
}


int
pretty_scan_file()
{
    int		data_header[XL_HEADER_LEN],
		i, j;
    char	*header,
		*data;

    for (;;) {
	/*
	 * get next session record header
	 */
        i = 0;
	header = (char *) &data_header;
	while (i<XL_HEADER_BYTE_LEN) {
	    if ( (j = read( in_fd, header, (XL_HEADER_BYTE_LEN-i) ) ) <= 0 ) {
		output_epilog();
		return;
	    }
	    i += j;
	    header += j;
	}
	/*
	 * get next session record data
	 */
	if ( data_header[XL_BODYSIZE] > file_buf_siz ) {
	    while ( data_header[XL_BODYSIZE] > file_buf_siz ) {
		if ( (file_buf_siz += file_buf_siz) > xw_max_bufsize ) {
		    fprintf( stderr, "[Exceeded data buffer size limit]\n" );
		    output_epilog();
		    exit( EXIT_FAILURE );
		}
	    }
	    if ( (file_buf = realloc( file_buf, file_buf_siz ) ) == NULL ) {
		perror( progname );
		exit( EXIT_FAILURE );
	    }
	}
        i = 0;
	data = file_buf;
	while ( i<data_header[XL_BODYSIZE]) {
	    if ( (j = read( in_fd, data, (data_header[XL_BODYSIZE]-i) ) ) <= 0 ) {
		output_epilog();
		return;
	    }
	    i += j;
	    data += j;
	}
	/*
	 * process section record
	 */
	if ( !xw_single_session || (xw_session == data_header[XL_SESSION]) ) {
	    file_bufpos = file_buf;
	    file_bufcnt = data_header [XL_BODYSIZE];
	    file_bufmax = file_buf + file_bufcnt;
	    switch ( data_header[XL_MSGTYPE] ) {

case XL_SERVER_DATA:	/*
		         * get handle on session record
		         */
		        if ( cs != data_header[XL_SESSION] ) {
			    if ( (i = find_session_index( data_header[XL_SESSION] ) ) == -1 ) {
				fprintf( stderr, "[Could not locate session %d]\n", data_header[XL_SESSION] );
				exit( EXIT_FAILURE );
			    }
			    cs = data_header[XL_SESSION];
			    pcs = &session[i];
			}
			output_data_header( data_header );
			proc_server_session_rec( data_header[XL_SESSION] );
			break;

case XL_CLIENT_DATA:	/*
		         * get handle on session record
		         */
		        if ( cs != data_header[XL_SESSION] ) {
			    if ( (i = find_session_index( data_header[XL_SESSION] ) ) == -1 ) {
				fprintf( stderr, "[Could not locate session %d]\n", data_header[XL_SESSION] );
				exit( EXIT_FAILURE );
			    }
			    cs = data_header[XL_SESSION];
			    pcs = &session[i];
			}
			output_data_header( data_header );
			proc_client_session_rec( data_header[XL_SESSION] );
			break;
                                                                       
case XL_LINK_CONNECT:	if ( find_session_index( data_header[XL_SESSION] ) != -1 ) {
			    fprintf( stderr, "[Duplicate session connect record found {ignoring}]\n" );
			    break;
			}
			if ( (i=alloc_session_rec()) == -1 ) {
			    fprintf( stderr, "[Exhausted session limits.  Session %d not being scanned.]\n", 
				     data_header[XL_SESSION] );
			    break;
			}
			cs = data_header[XL_SESSION];
			pcs = &session[i];
			session[i].sr_number = data_header[XL_SESSION];
			session[i].sr_last_rec = XL_LINK_CONNECT;
			if ( (session[i].sr_cbuf = malloc( INITIAL_BUFSIZE ) ) == NULL ) {
			    perror( progname );
			    exit( EXIT_FAILURE );
			}
			session[i].sr_cbuf_siz = INITIAL_BUFSIZE;
			session[i].sr_cbufpos = session[i].sr_cbuf;
			session[i].sr_cbufcnt = 0;
			session[i].sr_c_state = XW_CLI_IN_START;
			if ( (session[i].sr_sbuf = malloc( INITIAL_BUFSIZE ) ) == NULL ) {
			    perror( progname );
			    exit( EXIT_FAILURE );
			}
			session[i].sr_sbuf_siz = INITIAL_BUFSIZE;
			session[i].sr_sbufpos = session[i].sr_sbuf;
			session[i].sr_sbufcnt = 0;
			session[i].sr_s_state = XW_SRV_IN_START;
			session[i].sr_c_seq_number = 0;
			session[i].sr_basetime[0] = data_header[XL_TIMESTAMP];
			session[i].sr_basetime[1] = data_header[XL_TIMESTAMP+1];
			output_data_header( data_header  );
			output_connect( data_header[XL_SESSION] );
			break;

case XL_LINK_ABORT:	if ( (i=find_session_index( data_header[XL_SESSION] ) ) == -1 ) {
			    fprintf( stderr, "[Extraneous session abort record found {ignoring}]\n" );
			    break;
			}
			cs = data_header[XL_SESSION];
			pcs = &session[i];
			output_data_header( data_header );
			output_disconnect( data_header[XL_SESSION] );
			session[i].sr_number = -1;
			free( session[i].sr_cbuf );
			free( session[i].sr_sbuf );
			if (xw_single_session ) {
			    output_epilog();
			    exit( EXIT_SUCCESS );
			}
			break;

default:	        fprintf( stderr, "[Unrecognized session record type: %d]\n", data_header[XL_MSGTYPE] );
			exit( EXIT_FAILURE );
	    }
	}
    }
}

int
output_prolog()
{

}


int
output_epilog()
{


}

int
output_connect( s )
int	s;
{
    char    line[80];

    sprintf( line, "Connecting Session %d", s );
    output_packet( line, "C", 0 );

}

int
output_disconnect( s )
int	s;
{
    char    line[80];

    sprintf( line, "Disconnection from Session %d", s );
    output_packet( line, "D", 0 );

}

int
proc_server_session_rec( s )
int	s;
{
    int	    i, j, k;

    /*
     * process until we are finished with the session record
     */
    while ( file_bufpos < file_bufmax ) {

	switch ( pcs->sr_s_state ) {
case XW_SRV_IN_GET_HDR:
	    i = min( pcs->sr_s_bytes_needed, file_bufmax-file_bufpos );
	    memcpy( pcs->sr_sbufpos, file_bufpos, i );
	    pcs->sr_sbufpos += i;
	    pcs->sr_sbufcnt += i;
	    file_bufpos += i;
	    if ( (pcs->sr_s_bytes_needed -= i) == 0 ) {
		xGenericReply	*rep = pcs->sr_sbuf;

		if ( rep->type == X_Reply ) pcs->sr_s_bytes_needed =
		    (FETCH_CARD32(rep->length))<<2;
		else pcs->sr_s_bytes_needed = 0;
		pcs->sr_s_state = XW_SRV_IN_GET_REPLY;
		if ( (j = pcs->sr_s_bytes_needed + pcs->sr_sbufcnt) > pcs->sr_sbuf_siz ) {
		    while ( j > pcs->sr_sbuf_siz ) {
			if ( (pcs->sr_sbuf_siz += pcs->sr_sbuf_siz) > xw_max_bufsize ) {
			    fprintf( stderr, "[Exceeded data buffer size limit]\n" );
			    exit( EXIT_FAILURE );
			}
		    }
		    k = pcs->sr_sbufpos - pcs->sr_sbuf;
		    if ( (pcs->sr_sbuf = realloc( pcs->sr_sbuf, pcs->sr_sbuf_siz ) ) == NULL ) {
			perror( progname );
			exit( EXIT_FAILURE );
		    }
		    pcs->sr_sbufpos = pcs->sr_sbuf + k;
		}
	    }
	    /*
	     * note the conditional fall-through
	     */
	    if ( pcs->sr_s_state != XW_SRV_IN_GET_REPLY ) break;

case XW_SRV_IN_GET_REPLY:
	    i = min( pcs->sr_s_bytes_needed, file_bufmax-file_bufpos );
	    memcpy( pcs->sr_sbufpos, file_bufpos, i );
	    pcs->sr_sbufpos += i;
	    pcs->sr_sbufcnt += i;
	    file_bufpos += i;
	    if ( (pcs->sr_s_bytes_needed -= i) == 0 ) {
		output_reply( pcs->sr_sbuf );
		pcs->sr_s_state = XW_SRV_IN_GET_HDR;
		pcs->sr_s_bytes_needed = sizeof( xGenericReply );
		pcs->sr_sbufpos = pcs->sr_sbuf;
		pcs->sr_sbufcnt = 0;
	    }
	    break;

case XW_SRV_IN_CLOSE:
	    break;

case XW_SRV_IN_START:
	    pcs->sr_s_state = XW_SRV_IN_CONNSETUP;
	    pcs->sr_s_bytes_needed = sizeof( xConnSetupPrefix );
	    break;

case XW_SRV_IN_CONNSETUP:
	    i = min( pcs->sr_s_bytes_needed, file_bufmax-file_bufpos );
	    memcpy( pcs->sr_sbufpos, file_bufpos, i );
	    pcs->sr_sbufpos += i;
	    pcs->sr_sbufcnt += i;
	    file_bufpos += i;
	    if ( (pcs->sr_s_bytes_needed -= i) == 0 ) {
		xConnSetupPrefix   *pre = pcs->sr_sbuf;

		j = sizeof( xConnSetupPrefix ) + ((FETCH_CARD16(pre->length))<<2);

		if ( pcs->sr_sbufcnt == j ) {
		    output_packet( "Server Setup Information", " ", 0 );
		    output_card8( "success", pre->success );
		    output_card8( "lengthReason", pre->lengthReason );
		    output_card16( "majorVersion", FETCH_CARD16(pre->majorVersion) );
		    output_card16( "minorVersion", FETCH_CARD16(pre->minorVersion) );
		    if ( pre->success == xTrue ) {
			output_success_data( pcs->sr_sbuf );
		    } else {
			output_string8( "reason", pre->lengthReason, pcs->sr_sbuf + sizeof( xConnSetupPrefix ) );
		    }
		    pcs->sr_s_state = XW_SRV_IN_GET_HDR;
		    pcs->sr_s_bytes_needed = sizeof( xGenericReply );
		    pcs->sr_sbufpos = pcs->sr_sbuf;
		    pcs->sr_sbufcnt = 0;
		} else {
		    pcs->sr_s_bytes_needed = j - pcs->sr_sbufcnt;
		}
	    }
            break;

	}

    }

}

int
proc_client_session_rec( s )
{
    int	    i, j, k;

    /*
     * process until we are finished with the session record
     */
    while ( file_bufpos < file_bufmax ) {

	switch ( pcs->sr_c_state ) {
case XW_CLI_IN_GET_HDR:
	    i = min( pcs->sr_c_bytes_needed, file_bufmax-file_bufpos );
	    memcpy( pcs->sr_cbufpos, file_bufpos, i );
	    pcs->sr_cbufpos += i;
	    pcs->sr_cbufcnt += i;
	    file_bufpos += i;
	    if ( (pcs->sr_c_bytes_needed -= i) == 0 ) {
		xReq *req = pcs->sr_cbuf;

		pcs->sr_c_bytes_needed += (((FETCH_CARD16(req->length))<<2) - sizeof( xReq ));
		pcs->sr_c_state = XW_CLI_IN_GET_REQ;
		if ( (j = pcs->sr_c_bytes_needed + pcs->sr_cbufcnt) > pcs->sr_cbuf_siz ) {
		    while ( j > pcs->sr_cbuf_siz ) {
			if ( (pcs->sr_cbuf_siz += pcs->sr_cbuf_siz) > xw_max_bufsize ) {
			    fprintf( stderr, "[Exceeded data buffer size limit]\n" );
			    exit( EXIT_FAILURE );
			}
		    }
		    k = pcs->sr_cbufpos - pcs->sr_cbuf;
		    if ( (pcs->sr_cbuf = realloc( pcs->sr_cbuf, pcs->sr_cbuf_siz ) ) == NULL ) {
			perror( progname );
			exit( EXIT_FAILURE );
		    }
		    pcs->sr_cbufpos = pcs->sr_cbuf + k;
		}
	    }
	    /*
	     * note the conditional fall-through
	     */
	    if ( pcs->sr_c_state != XW_CLI_IN_GET_REQ ) break;

case XW_CLI_IN_GET_REQ:
	    i = min( pcs->sr_c_bytes_needed, file_bufmax-file_bufpos );
	    memcpy( pcs->sr_cbufpos, file_bufpos, i );
	    pcs->sr_cbufpos += i;
	    pcs->sr_cbufcnt += i;
	    file_bufpos += i;
	    if ( (pcs->sr_c_bytes_needed -= i) == 0 ) {
		output_request( pcs->sr_cbuf );
		pcs->sr_c_state = XW_CLI_IN_GET_HDR;
		pcs->sr_c_bytes_needed = sizeof( xReq );
		pcs->sr_cbufpos = pcs->sr_cbuf;
		pcs->sr_cbufcnt = 0;
	    }
	    break;

case XW_CLI_IN_CLOSE:
	    break;

case XW_CLI_IN_START:
	    pcs->sr_c_state = XW_CLI_IN_CONNSETUP;
	    pcs->sr_c_bytes_needed = sizeof( xConnClientPrefix );
	    break;

case XW_CLI_IN_CONNSETUP:
	    i = min( pcs->sr_c_bytes_needed, file_bufmax-file_bufpos );
	    memcpy( pcs->sr_cbufpos, file_bufpos, i );
	    pcs->sr_cbufpos += i;
	    pcs->sr_cbufcnt += i;
	    file_bufpos += i;
	    if ( (pcs->sr_c_bytes_needed -= i) == 0 ) {
		xConnClientPrefix   *pre = pcs->sr_cbuf;

		if ( pre->byteOrder == endian ) pcs->sr_swapped = 0;
		else pcs->sr_swapped = 1;

		j = FETCH_CARD16(pre->nbytesAuthProto)
		  + FETCH_CARD16(pre->nbytesAuthString)
		  + sizeof( xConnClientPrefix );

		if ( pcs->sr_cbufcnt == j ) {
		    output_packet( "Client Setup Information", " ", 0 );
		    if ( pcs->sr_swapped ) output_card8( "byteOrder (Swapping)", pre->byteOrder );
		    else output_card8( "byteOrder", pre->byteOrder );
		    output_card16( "majorVersion", FETCH_CARD16(pre->majorVersion) );
		    output_card16( "minorVersion", FETCH_CARD16(pre->minorVersion) );
		    output_card16( "nbytesAuthProto", FETCH_CARD16(pre->nbytesAuthProto) );
		    output_card16( "nbytesAuthString", FETCH_CARD16(pre->nbytesAuthString) );
		    output_string8( "AuthProto", FETCH_CARD16(pre->nbytesAuthProto),
			(char *) pcs->sr_cbuf + sizeof( xConnClientPrefix ) );
		    output_string8( "AuthString", FETCH_CARD16(pre->nbytesAuthString),
			(char *) pcs->sr_cbuf + sizeof( xConnClientPrefix )
					      + FETCH_CARD16(pre->nbytesAuthProto) );
		    pcs->sr_c_state = XW_CLI_IN_GET_HDR;
		    pcs->sr_c_bytes_needed = sizeof( xReq );
		    pcs->sr_cbufpos = pcs->sr_cbuf;
		    pcs->sr_cbufcnt = 0;
		} else {
		    pcs->sr_c_bytes_needed = j - pcs->sr_cbufcnt;
		}
	    }
            break;

	}

    }


}


int
output_success_data( setup )
xConnSetupPrefix    *setup;
{
    xConnSetup	    *cs;
    xPixmapFormat   *pf;
    xDepth	    *d;
    xVisualType	    *vt;
    xWindowRoot	    *wr;
    char	    *vendor;
    int		    vendor_len;
    int		    root_cnt;
    int		    format_cnt;
    int		    depth_cnt;
    int		    visual_cnt;
    int		    i, j, k;

    cs = (xConnSetup *) (((char *) setup) + sizeof( xConnSetupPrefix ));
    output_card32( "release", FETCH_CARD32(cs->release) );
    output_card32( "ridBase", FETCH_CARD32(cs->ridBase) );
    output_card32( "ridMask", FETCH_CARD32(cs->ridMask) );
    output_card32( "motionBufferSize", FETCH_CARD32(cs->motionBufferSize) );
    output_card16( "nbytesVendor", FETCH_CARD16(cs->nbytesVendor) );
    vendor_len = FETCH_CARD16(cs->nbytesVendor);
    output_card16( "maxRequestSize", FETCH_CARD16(cs->maxRequestSize) );
    output_card8( "numRoots", cs->numRoots );
    root_cnt = cs->numRoots;
    output_card8( "numFormats", cs->numFormats );
    format_cnt = cs->numFormats;
    if ( cs->imageByteOrder == LSBFirst ) {
	output_string8( "imageByteOrder", 8, "LSBFirst" );
    } else {
	output_string8( "imageByteOrder", 8, "MSBFirst" );
    }
    if ( cs->bitmapBitOrder == LSBFirst ) {
	output_string8( "bitmapBitOrder", 8, "LSBFirst" );
    } else {
	output_string8( "bitmapBitOrder", 8, "MSBFirst" );
    }
    output_card8( "bitmapScanlineUnit", cs->bitmapScanlineUnit );
    output_card8( "bitmapScanlinePad", cs->bitmapScanlinePad );
    output_card8( "minKeyCode", cs->minKeyCode );
    output_card8( "maxKeyCode", cs->maxKeyCode );
    /*
     * Variable fields follow...
     *
     * First, the vendor string
     */

    vendor = (char *) (cs+1);
    output_string8( "Vendor", vendor_len, vendor );

    /*
     * List of PixmapFormats
     */
    pf = (xPixmapFormat *) (vendor + ( (vendor_len + 3) & ~3 ) );
    for ( i=0; i<format_cnt; i++ ) {
	output_card8( "depth", pf->depth );
	output_card8( "bitsPerPixel", pf->bitsPerPixel );
	output_card8( "scanLinePad", pf->scanLinePad );
	pf++;
    }

    /*
     * List of Screens
     */
    wr = (xWindowRoot *) pf;
    for ( i=0; i<root_cnt; i++ ) {
	output_card32( "windowID", FETCH_CARD32(wr->windowId) );
	output_card32( "defaultColormap", FETCH_CARD32(wr->defaultColormap) );
	output_card32( "whitePixel", FETCH_CARD32(wr->whitePixel) );
	output_card32( "blackPixel", FETCH_CARD32(wr->blackPixel) );
	output_card32( "currentInputMask", FETCH_CARD32(wr->currentInputMask) );
	output_card16( "pixWidth", FETCH_CARD16(wr->pixWidth) );
	output_card16( "pixHeight", FETCH_CARD16(wr->pixHeight) );
	output_card16( "mmWidth", FETCH_CARD16(wr->mmWidth) );
	output_card16( "mmHeight", FETCH_CARD16(wr->mmHeight) );
	output_card16( "minInstalledMaps", FETCH_CARD16(wr->minInstalledMaps) );
	output_card16( "maxInstalledMaps", FETCH_CARD16(wr->maxInstalledMaps) );
	output_card32( "rootVisualID", FETCH_CARD32(wr->rootVisualID) );
	switch ( wr->backingStore ) {
case NotUseful:	    output_string8( "backingStore", 9, "NotUseful" ); break;
case WhenMapped:    output_string8( "backingStore", 10, "WhenMapped" ); break;
case Always:	    output_string8( "backingStore", 6, "Always" ); break;
default:	    output_card8( "backingStore", wr->backingStore ); break;
	}
	if ( wr->saveUnders == xTrue ) {
	    output_string8( "saveUnders", 3, "Yes" );
	} else {
	    output_string8( "saveUnders", 2, "No" );
	}
	output_card8( "rootDepth", wr->rootDepth );
	output_card8( "nDepths", wr->nDepths );
	depth_cnt = wr->nDepths;
	d = (xDepth *) (wr+1);
	for ( j=0; j<depth_cnt; j++ ) {
	    output_card8( "depth", d->depth );
	    output_card16( "nVisuals", FETCH_CARD16(d->nVisuals) );
	    visual_cnt = FETCH_CARD16(d->nVisuals);
	    vt = (xVisualType *) (d+1);
	    for ( k=0; k<visual_cnt; k++ ) {
		output_card32( "visualID", FETCH_CARD32(vt->visualID) );
		output_visual_class( "class", vt->class );
		output_card8( "bitsPerRGB", vt->bitsPerRGB );
		output_card16( "colormapEntries", FETCH_CARD16(vt->colormapEntries) );
		output_card32( "redMask", FETCH_CARD32(vt->redMask) );
		output_card32( "greenMask", FETCH_CARD32(vt->greenMask) );
		output_card32( "blueMask", FETCH_CARD32(vt->blueMask) );
		vt++;
	    }
	    d = (xDepth *) vt;
	}
	wr = (xWindowRoot *) d;
    }

}


int
output_reply( rep )
xGenericReply	*rep;
{
    int	    i, j, k;

    switch ( rep->type ) {

case X_Reply:	switch ( pcs->sr_last_request ) {

  case X_GetWindowAttributes :
	{
	xGetWindowAttributesReply   *r = rep;

	output_packet( "X_GetWindowAttributes (reply)", "R", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(r->sequenceNumber) );
	output_backing_store( "backingStore", r->backingStore );
	output_card32( "visual", FETCH_CARD32(r->visualID) );
	output_class( "class", FETCH_CARD16(r->class) );
	output_bit_gravity( "bitGravity", r->bitGravity );
	output_win_gravity( "winGravity", r->winGravity );
	output_card32( "backingBitPlanes", FETCH_CARD32(r->backingBitPlanes) );
	output_card32( "backingPixel", FETCH_CARD32(r->backingPixel) );
	output_bool( "saveUnders", r->saveUnder );
	output_bool( "mapInstalled", r->mapInstalled );
	output_map_state( "mapState", r->mapState );
	output_bool( "override", r->override );
	output_card32( "colormap", FETCH_CARD32(r->colormap) );
	output_card32( "allEventMasks", FETCH_CARD32(r->allEventMasks) );
	output_card32( "yourEventMask", FETCH_CARD32(r->yourEventMask) );
	output_card16( "doNotPropagateMask", FETCH_CARD32(r->doNotPropagateMask) );
	}
	break;

  case X_GetGeometry :
	{
	xGetGeometryReply   *r = rep;

	output_packet( "X_GetGeometry (reply)", "R", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(r->sequenceNumber) );
	output_card32( "window", FETCH_CARD32(r->root) );
	output_card8( "depth", r->depth );
	output_int16( "x", FETCH_INT16(r->x) );
	output_int16( "y", FETCH_INT16(r->y) );
	output_card16( "width", FETCH_CARD16(r->width) );
	output_card16( "height", FETCH_CARD16(r->height) );
	output_card16( "borderWidth", FETCH_CARD16(r->borderWidth) );
	}
	break;

  case X_QueryTree :
	{
	xQueryTreeReply	*r = rep;

	output_packet( "X_QueryTree (reply)", "R", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(r->sequenceNumber) );
	output_card32( "root", FETCH_CARD32(r->root) );
	output_card32( "parent", FETCH_CARD32(r->parent) );
	output_card16( "nChildren", FETCH_CARD16(r->nChildren) );
	output_listofcard32( "child", FETCH_CARD16(r->nChildren), (int *) (r+1) );
	}
	break;

  case X_InternAtom :
	{
	xInternAtomReply    *r = rep;

	output_packet( "X_InternAtom (reply)", "R", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(r->sequenceNumber) );
	output_card32( "atom", FETCH_CARD32(r->atom) );
	}
	break;

  case X_GetAtomName :
	{
	xGetAtomNameReply   *r = rep;

	output_packet( "X_GetAtomName (reply)", "R", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(r->sequenceNumber) );
	output_card16( "nameLength", FETCH_CARD16(r->nameLength) );
	output_string8( "atomName", FETCH_CARD16(r->nameLength), (char *) (r+1) );
	}
	break;

  case X_GetProperty :
	{
	xGetPropertyReply   *r = rep;

	output_packet( "X_GetProperty (reply)", "R", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(r->sequenceNumber) );
	output_card32( "propertyType", FETCH_CARD32(r->propertyType) );
	output_card32( "bytesAfter", FETCH_CARD32(r->bytesAfter) );
	output_card32( "nItems", FETCH_CARD32(r->nItems) );
	switch (r->format) {
case 8:	    output_string8( "format", 4, "Byte" );
	    output_listofcard8( "item", FETCH_CARD32(r->nItems), (char *) (r+1) );
	    break;
case 16:    output_string8( "format", 4, "Word" );
	    output_listofcard16( "item", FETCH_CARD32(r->nItems), (short *) (r+1) );
	    break;
case 32:    output_string8( "format", 8, "Longword" );
	    output_listofcard32( "item", FETCH_CARD32(r->nItems), (long *) (r+1) );
	    break;
default:    output_card8( "format (unknown)", r->format);
	    break;
	    }
	}
	break;

  case X_ListProperties :
	{
	xListPropertiesReply	*r = rep;

	output_packet( "X_ListProperties (reply)", "R", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(r->sequenceNumber) );
	output_card16( "nProperties", FETCH_CARD16(r->nProperties) );
	output_listofcard32( "atom", FETCH_CARD16(r->nProperties), (int *) (r+1) );
	}
	break;

  case X_GetSelectionOwner :
	{
	xGetSelectionOwnerReply	*r = rep;

	output_packet( "X_GetSelectionOwner (reply)", "R", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(r->sequenceNumber) );
	output_card32( "ownerWindow", FETCH_CARD32(r->owner) );
	}
	break;

  case X_GrabPointer :
	{
	xGrabPointerReply *r	= rep;

	output_packet( "X_GrabPointer (reply)", "R", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(r->sequenceNumber) );
	output_grab_status( "status", r->status );
	}
	break;

  case X_GrabKeyboard :
	{
	xGrabKeyboardReply *r	= rep;

	output_packet( "X_GrabKeyboard (reply)", "R", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(r->sequenceNumber) );
	output_grab_status( "status", r->status );
	}
	break;

  case X_QueryPointer :
	{
	xQueryPointerReply  *r	= rep;

	output_packet( "X_QueryPointer (reply)", "R", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(r->sequenceNumber) );
	output_card32( "root", FETCH_CARD32(r->root) );
	output_card32( "child", FETCH_CARD32(r->child) );
	output_bool( "sameScreen", r->sameScreen );
	output_int16( "rootX", FETCH_INT16(r->rootX) );
	output_int16( "rootY", FETCH_INT16(r->rootY) );
	output_int16( "winX", FETCH_INT16(r->winX) );
	output_int16( "winY", FETCH_INT16(r->winY) );
	output_card16( "mask", FETCH_CARD16(r->mask) );
	}
	break;

  case X_GetMotionEvents :
	{
	xGetMotionEventsReply	*r = rep;

	output_packet( "X_GetMotionEvents (reply)", "R", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(r->sequenceNumber) );
	output_card32( "nEvents", FETCH_CARD32(r->nEvents) );
	output_listoftimecoord( "timeCoord", FETCH_CARD32(r->nEvents), (char *) (r+1) );
	}
	break;

  case X_TranslateCoords :
	{
	xTranslateCoordsReply	*r = rep;

	output_packet( "X_TranslateCoords (reply)", "R", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(r->sequenceNumber) );
	output_bool( "sameScreen", r->sameScreen );
	output_card32( "child", FETCH_CARD32(r->child) );
	output_int16( "destX", FETCH_INT16(r->dstX) );
	output_int16( "destY", FETCH_INT16(r->dstY) );
	}
	break;

  case X_GetInputFocus :
	{
	xGetInputFocusReply *r = rep;

	output_packet( "X_GetInputFocus (reply)", "R", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(r->sequenceNumber) );
	output_card32( "focusWindow", FETCH_CARD32(r->focus) );
	switch (r->revertTo) {
    case RevertToNone		: output_string8( "revertTo", 12, "RevertToNone" ); break;
    case RevertToPointerRoot	: output_string8( "revertTo", 19, "RevertToPointerRoot" ); break;
    case RevertToParent		: output_string8( "revertTo", 14, "RevertToParent" ); break;
    default:			  output_card8( "revertTo (bad)", r->revertTo );
	    }
	}
	break;

  case X_QueryKeymap :
	{
	xQueryKeymapReply   *r = rep;

	output_packet( "X_QueryKeymap (reply)", "R", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(r->sequenceNumber) );
	output_listofcard8( "keymap", 32, &r->map[0] );
	}
	break;

  case X_QueryFont:
	{
	xQueryFontReply	    *r = rep;

	output_packet( "X_QueryFont (reply)", "R", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(r->sequenceNumber) );
	output_listofcard32( "longword", (6 + FETCH_CARD32(r->length)), &r->minBounds );
	}
	break;

  case X_QueryTextExtents :
	{
	xQueryTextExtentsReply	*r = rep;

	output_packet( "X_QueryTextExtents (reply)", "R", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(r->sequenceNumber) );
	output_card8( "drawDirection", r->drawDirection );
	output_int16( "fontAscent", FETCH_INT16(r->fontAscent) );
	output_int16( "fontDescent", FETCH_INT16(r->fontDescent) );
	output_int16( "overallAscent", FETCH_INT16(r->overallAscent) );
	output_int16( "overallDescent", FETCH_INT16(r->overallDescent) );
	output_int32( "overallWidth", FETCH_INT32(r->overallWidth) );
	output_int32( "overallLeft", FETCH_INT32(r->overallLeft) );
	output_int32( "overallRight", FETCH_INT32(r->overallRight) );
	}
	break;

  case X_ListFonts :
	{
	xListFontsReply	*r = rep;

	output_packet( "X_ListFonts (reply)", "R", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(r->sequenceNumber) );
	output_card16( "nfonts", FETCH_CARD16(r->nFonts) );
	output_listofstring8( "fontName", FETCH_CARD16(r->nFonts), ((char *) r + sizeof( xListFontsReply ) ) );
	}
	break;

  case X_ListFontsWithInfo :
	{
	xListFontsWithInfoReply	*r = rep;
	int	nbytes;

	output_packet( "X_ListFontsWithInfo (reply)", "R", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(r->sequenceNumber) );

	if ( r->nameLength == 0 ) {
	    output_string8( "END-OF-FONTLIST", 0, " " );
	}
	else {
	    output_card32( "nReplies", FETCH_CARD32(r->nReplies) );
	    nbytes = FETCH_CARD16(r->nFontProps) * sizeof (XFontProp);
	    output_string8( "fontName", r->nameLength, ((char *) (r+1))+nbytes );
	    output_listofcard32( "stuff", (sizeof(xListFontsWithInfoReply)-8)>>2, (unsigned long *) &r->minBounds );
	}
	}
	break;

  case X_GetFontPath :
	{
	xGetFontPathReply   *r = rep;

	output_packet( "X_GetFontPath (reply)", "R", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(r->sequenceNumber) );
	output_card16( "nPaths", FETCH_CARD16(r->nPaths) );
	output_listofstring8( "path", FETCH_CARD16(r->nPaths), (char *) (r+1) );
	}
	break;

  case X_GetImage :
	{
	xGetImageReply	    *r = rep;

	output_packet( "X_GetImage (reply)", "R", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(r->sequenceNumber) );
	output_card32( "visual", FETCH_CARD32(r->visual) );
	output_card8( "depth", r->depth );
	output_image( "image", (FETCH_CARD32(r->length)<<2), (char *) (r+1), 100 );
	}
	break;

  case X_ListInstalledColormaps :
	{
	xListInstalledColormapsReply	*r = rep;

	output_packet( "X_ListInstalledColormaps (reply)", "R", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(r->sequenceNumber) );
	output_card16( "nColormaps", FETCH_CARD16(r->nColormaps) );
	output_listofcard32( "colormap", FETCH_CARD16(r->nColormaps), (int *) (r+1) );
	}
	break;

  case X_AllocColor :
	{
	xAllocColorReply    *r = rep;

	output_packet( "X_AllocColor (reply)", "R", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(r->sequenceNumber) );
	output_card16( "red", FETCH_CARD16(r->red) );
	output_card16( "green", FETCH_CARD16(r->green) );
	output_card16( "blue", FETCH_CARD16(r->blue) );
	output_card32( "pixel", FETCH_CARD32(r->pixel) );
	}
	break;

  case X_AllocNamedColor :
	{
	xAllocNamedColorReply    *r = rep;

	output_packet( "X_AllocNamedColor (reply)", "R", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(r->sequenceNumber) );
	output_card16( "exactRed", FETCH_CARD16(r->exactRed) );
	output_card16( "exactGreen", FETCH_CARD16(r->exactGreen) );
	output_card16( "exactBlue", FETCH_CARD16(r->exactBlue) );
	output_card16( "screenRed", FETCH_CARD16(r->screenRed) );
	output_card16( "screenGreen", FETCH_CARD16(r->screenGreen) );
	output_card16( "screenBlue", FETCH_CARD16(r->screenBlue) );
	output_card32( "pixel", FETCH_CARD32(r->pixel) );
	}
	break;

  case X_AllocColorCells :
	{
	xAllocColorCellsReply	*r = rep;

	output_packet( "X_AllocColorCells (reply)", "R", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(r->sequenceNumber) );
	output_card16( "nPixels", FETCH_CARD16(r->nPixels) );
	output_card16( "nMasks", FETCH_CARD16(r->nMasks) );
	output_listofcard32( "pixelOrMask", (FETCH_CARD16(r->nPixels) + FETCH_CARD16(r->nMasks)), (int *) (r+1) );
	}
	break;

  case X_AllocColorPlanes :
	{
	xAllocColorPlanesReply	*r = rep;

	output_packet( "X_AllocColorPlanes (reply)", "R", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(r->sequenceNumber) );
	output_card32( "redMask", FETCH_CARD32(r->redMask) );
	output_card32( "greenMask", FETCH_CARD32(r->greenMask) );
	output_card32( "blueMask", FETCH_CARD32(r->blueMask) );
	output_card16( "nPixels", FETCH_CARD16(r->nPixels) );
	output_listofcard32( "pixel", FETCH_CARD16(r->nPixels), (int *) (r+1) );
	}
	break;

  case X_QueryColors :
	{
	xQueryColorsReply   *r = rep;

	output_packet( "X_QueryColors (reply)", "R", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(r->sequenceNumber) );
	output_card16( "nColors", FETCH_CARD16(r->nColors) );
	output_listofrgb( "rgb", FETCH_CARD16(r->nColors), (char *) (r+1) );
	}
	break;

  case X_LookupColor :
	{
	xLookupColorReply    *r = rep;

	output_packet( "X_LookupColor (reply)", "R", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(r->sequenceNumber) );
	output_card16( "exactRed", FETCH_CARD16(r->exactRed) );
	output_card16( "exactGreen", FETCH_CARD16(r->exactGreen) );
	output_card16( "exactBlue", FETCH_CARD16(r->exactBlue) );
	output_card16( "screenRed", FETCH_CARD16(r->screenRed) );
	output_card16( "screenGreen", FETCH_CARD16(r->screenGreen) );
	output_card16( "screenBlue", FETCH_CARD16(r->screenBlue) );
	}
	break;

  case X_QueryBestSize :
	{
	xQueryBestSizeReply *r = rep;

	output_packet( "X_QueryBestSize (reply)", "R", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(r->sequenceNumber) );
	output_card16( "width", FETCH_CARD16(r->width) );
	output_card16( "height", FETCH_CARD16(r->height) );
	}
	break;

  case X_QueryExtension :
	{
	xQueryExtensionReply	*r = rep;

	output_packet( "X_QueryExtension (reply)", "R", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(r->sequenceNumber) );
	output_bool( "present", r->present );
	if ( r->present == xTrue ) {
	    output_card8( "major_opcode", r->major_opcode );
	    output_card8( "first_event", r->first_event );
	    output_card8( "first_error", r->first_error );
	    }
	}
	break;

  case X_ListExtensions :
	{
	xListExtensionsReply	*r = rep;

	output_packet( "X_ListExtensions (reply)", "R", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(r->sequenceNumber) );
	output_card8( "nExtensions", r->nExtensions );
	output_listofstring8( "extension", r->nExtensions, (char *) (r+1) );
	}
	break;

  case X_SetModifierMapping :
	{
	xSetModifierMappingReply    *r = rep;

	output_packet( "X_SetModifierMapping (reply)", "R", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(r->sequenceNumber) );
	output_card8( "success", r->success );
	}
	break;

  case X_GetModifierMapping :
	{
	xGetModifierMappingReply    *r = rep;

	output_packet( "X_GetModifierMapping (reply)", "R", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(r->sequenceNumber) );
	output_card8( "keycodesPerModifier", r->numKeyPerModifier );
	output_listofcard8( "keycode", 8 * r->numKeyPerModifier, (char *) (r+1) );
	}
	break;

  case X_SetPointerMapping :
	{
	xSetPointerMappingReply    *r = rep;

	output_packet( "X_SetPointerMapping (reply)", "R", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(r->sequenceNumber) );
	output_card8( "success", r->success );
	}
	break;

  case X_GetPointerMapping :
	{
	xGetPointerMappingReply	*r = rep;

	output_packet( "X_GetPointerMapping (reply)", "R", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(r->sequenceNumber) );
	output_card8( "nElements", r->nElts );
	output_listofcard8( "element", r->nElts, (char *) (r+1) );
	}
	break;

  case X_GetKeyboardMapping :
	{
	xGetKeyboardMappingReply    *r = rep;

	output_packet( "X_GetKeyboardMapping (reply)", "R", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(r->sequenceNumber) );
	output_card8( "keySymsPerKeyCode", r->keySymsPerKeyCode );
	output_listofkeysyms( "keySym", r->keySymsPerKeyCode, (char *) (r+1) );
	}
	break;

  case X_GetKeyboardControl :
	{
	xGetKeyboardControlReply    *r = rep;

	output_packet( "X_GetKeyboardControl (reply)", "R", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(r->sequenceNumber) );
	output_bool( "globalAutoRepeat", r->globalAutoRepeat );
	output_card32( "ledMask", FETCH_CARD32(r->ledMask) );
	output_card8( "keyClickPercent", r->keyClickPercent );
	output_card8( "bellPercent", r->bellPercent );
	output_card16( "bellPitch", FETCH_CARD16(r->bellPitch) );
	output_card16( "bellDuration", FETCH_CARD16(r->bellDuration) );
	output_listofcard8( "keyMap", 32, &r->map[0] );
	}
	break;

  case X_GetPointerControl :
	{
	xGetPointerControlReply	*r = rep;

	output_packet( "X_GetPointerControl (reply)", "R", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(r->sequenceNumber) );
	output_card16( "accelNumerator", FETCH_CARD16(r->accelNumerator) );
	output_card16( "accelDenominator", FETCH_CARD16(r->accelDenominator) );
	output_card16( "threshold", FETCH_CARD16(r->threshold) );
	}
	break;

  case X_GetScreenSaver :
	{
	xGetScreenSaverReply	*r = rep;

	output_packet( "X_GetScreenSaver (reply)", "R", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(r->sequenceNumber) );
	output_card16( "timeout", FETCH_CARD16(r->timeout) );
	output_card16( "interval", FETCH_CARD16(r->interval) );
	switch (r->preferBlanking) {
    case DontPreferBlanking	: output_string8( "preferBlanking", 18, "DontPreferBlanking" ); break;
    case PreferBlanking	: output_string8( "preferBlanking", 14, "PreferBlanking" ); break;
    case DefaultBlanking	: output_string8( "preferBlanking", 15, "DefaultBlanking" ); break;
    default			: output_card8( "preferBlanking (bad)", r->preferBlanking ); break;
	    }
	switch (r->allowExposures) {
    case DontAllowExposures	: output_string8( "allowExposures", 18, "DontAllowExposures" ); break;
    case AllowExposures	: output_string8( "allowExposures", 14, "AllowExposures" ); break;
    case DefaultExposures	: output_string8( "allowExposures", 16, "DefaultExposures" ); break;
    default			: output_card8( "allowExposures (bad)", r->allowExposures ); break;
	    }
	}
	break;

  case X_ListHosts :
	{
	xListHostsReply	*r = rep;

	output_packet( "X_ListHosts (reply)", "R", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(r->sequenceNumber) );
	output_card16( "nHosts", FETCH_CARD16(r->nHosts) );
	output_string8( "Some hosts....", 0, " " );
	}
	break;

  default:
	output_packet( "X_GenericReply (reply)", "R", 0 );
	output_card8( "replyType", pcs->sr_last_request );
	output_card8( "data1", rep->data1 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(rep->sequenceNumber) );
	output_listofcard32( "genericLong", (6 + FETCH_CARD32(rep->length)), &(rep->data00) );
	break; /* default reply */

      }
      break; /* X_Reply */

case X_Error:
	{
	xError	*e = rep;

	output_packet( "X_Error", "ERROR", 0 );
	output_card16_always( "sequenceNumber", e->sequenceNumber );
	output_card8( "errorCode", e->errorCode );
	output_card32( "resource", FETCH_CARD32(e->resourceID) );
	output_card8( "majorCode", e->majorCode );
	output_card16( "minorCode", FETCH_CARD16(e->minorCode) );
	}
	break; /* X_Error */

default:    /* assume events */
	output_event( rep );
	break; /* event */

    }

}


int
output_request( req )
xReq	*req;
{
    int	    i, j, k;

    pcs->sr_c_seq_number++;
    switch ( req->reqType ) {

case X_CreateWindow                  :
	{
	xCreateWindowReq    *r = req;

	output_packet( "X_CreateWindow" , " ", pcs->sr_c_seq_number );
	output_card8( "depth", r->depth );
	output_card32( "window", FETCH_CARD32(r->wid) );
	output_card32( "parent", FETCH_CARD32(r->parent) );
	output_card16( "x", FETCH_CARD16(r->x) );
	output_card16( "y", FETCH_CARD16(r->y) );
	output_card16( "width", FETCH_CARD16(r->width) );
	output_card16( "height", FETCH_CARD16(r->height) );
	output_card16( "borderWidth", FETCH_CARD16(r->borderWidth) );
	output_class( "class", FETCH_CARD16(r->class) );
	output_card32( "visual", FETCH_CARD32(r->visual) );
	output_card32( "mask", FETCH_CARD32(r->mask) );
	}
	break;

case X_ChangeWindowAttributes        :
	{
	xChangeWindowAttributesReq  *r = req;

	output_packet( "X_ChangeWindowAttributes", " ", pcs->sr_c_seq_number  );
	output_card32( "window", FETCH_CARD32(r->window) );
	output_card32( "valueMask", FETCH_CARD32(r->valueMask) );
	output_window_attributes( FETCH_CARD32(r->valueMask), (char *) (r+1) );
	}
	break;

case X_GetWindowAttributes           :
	{
	xResourceReq	*r = req;

	output_packet( "X_GetWindowAttributes", " ", pcs->sr_c_seq_number  );
	output_card32( "window", FETCH_CARD32(r->id) );
	}
	pcs->sr_last_request = req->reqType;
	break;

case X_DestroyWindow                 :
	{
	xResourceReq	*r = req;

	output_packet( "X_DestroyWindow", " ", pcs->sr_c_seq_number  );
	output_card32( "window", FETCH_CARD32(r->id) );
	}
	break;

case X_DestroySubwindows             :
	{
	xResourceReq	*r = req;

	output_packet( "X_DestroySubwindows", " ", pcs->sr_c_seq_number  );
	output_card32( "window", FETCH_CARD32(r->id) );
	}
	break;

case X_ChangeSaveSet                 :
	{
	xChangeSaveSetReq   *r = req;

	output_packet( "X_ChangeSaveSet", " ", pcs->sr_c_seq_number  );
	output_card32( "window", FETCH_CARD32(r->window) );
	switch (r->mode) {
case SetModeInsert: output_string8( "mode", 13, "SetModeInsert" ); break;	
case SetModeDelete: output_string8( "mode", 13, "SetModeDelete" ); break;	
default:	    output_card8( "mode (bad)", r->mode ); break;
	    }
	}
	break;

case X_ReparentWindow                :
	{
	xReparentWindowReq  *r = req;

	output_packet( "X_ReparentWindow", " ", pcs->sr_c_seq_number  );
	output_card32( "window", FETCH_CARD32(r->window) );
	output_card32( "parent", FETCH_CARD32(r->parent) );
	output_int16( "x", FETCH_INT16(r->x) );
	output_int16( "y", FETCH_INT16(r->y) );
	}
	break;

case X_MapWindow                     :
	{
	xResourceReq	*r = req;

	output_packet( "X_MapWindow", " ", pcs->sr_c_seq_number  );
	output_card32( "window", FETCH_CARD32(r->id) );
	}
	break;

case X_MapSubwindows                 :
	{
	xResourceReq	*r = req;

	output_packet( "X_MapSubwindows", " ", pcs->sr_c_seq_number  );
	output_card32( "window", FETCH_CARD32(r->id) );
	}
	break;

case X_UnmapWindow                  :
	{
	xResourceReq	*r = req;

	output_packet( "X_UnmapWindow", " ", pcs->sr_c_seq_number  );
	output_card32( "window", FETCH_CARD32(r->id) );
	}
	break;

case X_UnmapSubwindows              :
	{
	xResourceReq	*r = req;

	output_packet( "X_UnmapSubwindows", " ", pcs->sr_c_seq_number  );
	output_card32( "window", FETCH_CARD32(r->id) );
	}
	break;

case X_ConfigureWindow              :
	{
	xConfigureWindowReq *r = req;

	output_packet( "X_ConfigureWindow", " ", pcs->sr_c_seq_number  );
	output_card32( "window", FETCH_CARD32(r->window) );
	output_card16( "mask", FETCH_CARD16(r->mask) );
	output_window_changes( FETCH_CARD16(r->mask), (char *) (r+1) );
	}
	break;

case X_CirculateWindow              :
	{
	xCirculateWindowReq *r = req;

	output_packet( "X_CirculateWindow", " ", pcs->sr_c_seq_number  );
	output_card32( "window", FETCH_CARD32(r->window) );
	switch (r->direction) {
case RaiseLowest:   output_string8( "direction", 11, "RaiseLowest" ); break;
case LowerHighest:  output_string8( "direction", 12, "LowerHighest" ); break;
default:	    output_card8( "direction (bad)", r->direction ); break;
	    }
	}
	break;
	
case X_GetGeometry                  :
	{
	xResourceReq	*r = req;

	output_packet( "X_GetGeometry", " ", pcs->sr_c_seq_number  );
	output_card32( "window", FETCH_CARD32(r->id) );
	}
	pcs->sr_last_request = req->reqType;
	break;

case X_QueryTree                    :
	{
	xResourceReq	*r = req;

	output_packet( "X_QueryTree", " ", pcs->sr_c_seq_number  );
	output_card32( "window", FETCH_CARD32(r->id) );
	}
	pcs->sr_last_request = req->reqType;
	break;

case X_InternAtom                   :
	{
	xInternAtomReq	*r = req;

	output_packet( "X_InternAtom", " ", pcs->sr_c_seq_number  );
	output_bool( "onlyIfExists", r->onlyIfExists );
	output_card16( "nbytes", FETCH_CARD16(r->nbytes) );
	output_string8( "Atom", FETCH_CARD16(r->nbytes), (char *) (r+1) );
	}
	pcs->sr_last_request = req->reqType;
	break;

case X_GetAtomName                  :
	{
	xResourceReq	*r = req;

	output_packet( "X_GetAtomName", " ", pcs->sr_c_seq_number  );
	output_card32( "AtomID", FETCH_CARD32(r->id) );
	}
	pcs->sr_last_request = req->reqType;
	break;

case X_ChangeProperty               :
	{
	xChangePropertyReq  *r = req;

	output_packet( "X_ChangeProperty", " ", pcs->sr_c_seq_number  );
	output_card32( "window", FETCH_CARD32(r->window) );
	output_card32( "property", FETCH_CARD32(r->property) );
	output_card32( "type", FETCH_CARD32(r->type) );
	output_prop_mode( "mode", r->mode );
	output_card32( "nUnits", FETCH_CARD32(r->nUnits) );
	switch (r->format) {
case 8:	    output_string8( "format", 4, "Byte" );
	    output_listofcard8( "Unit", FETCH_CARD32(r->nUnits), (char *) (r+1) );
	    break;
case 16:    output_string8( "format", 4, "Word" );
	    output_listofcard16( "Unit", FETCH_CARD32(r->nUnits), (short *) (r+1) );
	    break;
case 32:    output_string8( "format", 8, "Longword" );
	    output_listofcard32( "Unit", FETCH_CARD32(r->nUnits), (int *) (r+1) );
	    break;
default:    output_card8( "format (unknown)", r->format);
	    break;
	    }
	}
	break;
	
case X_DeleteProperty               :
	{
	xDeletePropertyReq  *r = req;

	output_packet( "X_DeleteProperty", " ", pcs->sr_c_seq_number  );
	output_card32( "window", FETCH_CARD32(r->window) );
	output_card32( "property", FETCH_CARD32(r->property) );
	}
	break;

case X_GetProperty                  :
	{
	xGetPropertyReq	*r = req;

	output_packet( "X_GetProperty", " ", pcs->sr_c_seq_number  );
	output_card32( "window", FETCH_CARD32(r->window) );
	output_card32( "property", FETCH_CARD32(r->property) );
	output_card32( "type", FETCH_CARD32(r->type) );
	output_bool( "delete", r->delete );
	output_card32( "longOffset", FETCH_CARD32(r->longOffset) );
	output_card32( "longLength", FETCH_CARD32(r->longLength) );
	}
	pcs->sr_last_request = req->reqType;
	break;

case X_ListProperties               :
	{
	xResourceReq	*r = req;

	output_packet( "X_ListProperties", " ", pcs->sr_c_seq_number  );
	output_card32( "AtomID", FETCH_CARD32(r->id) );
	}
	pcs->sr_last_request = req->reqType;
	break;

case X_SetSelectionOwner            :
	{
	xSetSelectionOwnerReq	*r = req;

	output_packet( "X_SetSelectionOwner", " ", pcs->sr_c_seq_number  );
	output_card32( "window", FETCH_CARD32(r->window) );
	output_card32( "selection", FETCH_CARD32(r->selection) );
	output_time( "time", FETCH_CARD32(r->time) );
	}
	break;

case X_GetSelectionOwner            :
	{
	xResourceReq	*r = req;

	output_packet( "X_GetSelectionOwner", " ", pcs->sr_c_seq_number  );
	output_card32( "Window/Atom", FETCH_CARD32(r->id) );
	}
	pcs->sr_last_request = req->reqType;
	break;

case X_ConvertSelection             :
	{
	xConvertSelectionReq	*r = req;

	output_packet( "X_ConvertSelection", " ", pcs->sr_c_seq_number  );
	output_card32( "window", FETCH_CARD32(r->requestor) );
	output_card32( "selection", FETCH_CARD32(r->selection) );
	output_card32( "target", FETCH_CARD32(r->target) );
	output_card32( "property", FETCH_CARD32(r->property) );
	output_time( "time", FETCH_CARD32(r->time) );
	}
	break;

case X_SendEvent                    :
	{
	xSendEventReq	*r = req;

	output_packet( "X_SendEvent", " ", pcs->sr_c_seq_number  );
	output_card32( "destination", FETCH_CARD32(r->destination) );
	output_bool( "propagate", r->propagate );
	output_card32( "eventMask", FETCH_CARD32(r->eventMask) );
	output_listofcard32( "eventLong", sizeof( xEvent )>>2, &r->event );
	}
	break;

case X_GrabPointer                  :
	{
	xGrabPointerReq	*r = req;

	output_packet( "X_GrabPointer", " ", pcs->sr_c_seq_number  );
	output_card32( "grabWindow", FETCH_CARD32(r->grabWindow) );
	output_bool( "ownerEvents", r->ownerEvents );
	output_card16( "eventMask", FETCH_CARD16(r->eventMask) );
	output_grab_mode( "pointerMode", r->pointerMode );
	output_grab_mode( "keyboardMode", r->keyboardMode );
	output_card32( "confineTo", FETCH_CARD32(r->confineTo) );
	output_card32( "cursor", FETCH_CARD32(r->cursor) );
	output_time( "time", FETCH_CARD32(r->time) );
	}
	pcs->sr_last_request = req->reqType;
	break;

case X_UngrabPointer                :
	{
	xResourceReq	*r = req;

	output_packet( "X_UngrabPointer", " ", pcs->sr_c_seq_number  );
	output_time( "time", FETCH_CARD32(r->id) );
	}
	break;

case X_GrabButton                   :
	{
	xGrabButtonReq	*r = req;

	output_packet( "X_GrabButton", " ", pcs->sr_c_seq_number  );
	output_card32( "grabWindow", FETCH_CARD32(r->grabWindow) );
	output_bool( "ownerEvents", r->ownerEvents );
	output_card16( "eventMask", FETCH_CARD16(r->eventMask) );
	output_grab_mode( "pointerMode", r->pointerMode );
	output_grab_mode( "keyboardMode", r->keyboardMode );
	output_card32( "confineTo", FETCH_CARD32(r->confineTo) );
	output_card32( "cursor", FETCH_CARD32(r->cursor) );
	output_button( "button", r->button );
	output_card16( "modifiers", FETCH_CARD16(r->modifiers) );
	}
	break;
	
case X_UngrabButton                 :
	{
	xUngrabButtonReq    *r = req;

	output_packet( "X_UngrabButton", " ", pcs->sr_c_seq_number  );
	output_card32( "grabWindow", FETCH_CARD32(r->grabWindow) );
	output_button( "button", r->button );
	output_modifiers( "modifiers", FETCH_CARD16(r->modifiers) );
	}
	break;

case X_ChangeActivePointerGrab      :
	{
	xChangeActivePointerGrabReq *r = req;

	output_packet( "X_ChangeActivePointerGrab", " ", pcs->sr_c_seq_number  );
	output_card32( "cursor", FETCH_CARD32(r->cursor) );
	output_time( "time", FETCH_CARD32(r->time) );
	output_card16( "eventMask", FETCH_CARD16(r->eventMask) );
	}
	break;

case X_GrabKeyboard                 :
	{
	xGrabKeyboardReq    *r = req;

	output_packet( "X_GrabKeyboard", " ", pcs->sr_c_seq_number  );
	output_card32( "grabWindow", FETCH_CARD32(r->grabWindow) );
	output_bool( "ownerEvents", r->ownerEvents );
	output_time( "time", FETCH_CARD32(r->time) );
	output_grab_mode( "pointerMode", r->pointerMode );
	output_grab_mode( "keyboardMode", r->keyboardMode );
	}
	pcs->sr_last_request = req->reqType;
	break;

case X_UngrabKeyboard               :
	{
	xResourceReq	*r = req;

	output_packet( "X_UngrabKeyboard", " ", pcs->sr_c_seq_number  );
	output_time( "time", FETCH_CARD32(r->id) );
	}
	break;

case X_GrabKey                      :
	{
	xGrabKeyReq *r = req;

	output_packet( "X_GrabKey", " ", pcs->sr_c_seq_number  );
	output_card32( "grabWindow", FETCH_CARD32(r->grabWindow) );
	output_bool( "ownerEvents", r->ownerEvents );
	output_card16( "modifiers", FETCH_CARD16(r->modifiers) );
	output_card8( "key", r->key );
	output_grab_mode( "pointerMode", r->pointerMode );
	output_grab_mode( "keyboardMode", r->keyboardMode );
	}
	break;

case X_UngrabKey                    :
	{
	xUngrabKeyReq	*r = req;

	output_packet( "X_UngrabKey", " ", pcs->sr_c_seq_number  );
	output_card32( "grabWindow", FETCH_CARD32(r->grabWindow) );
	output_card8( "key", r->key );
	output_modifiers( "modifiers", FETCH_CARD16(r->modifiers) );
	}
	break;

case X_AllowEvents                  :
	{
	xAllowEventsReq	*r = req;

	output_packet( "X_AllowEvents", " ", pcs->sr_c_seq_number );
	output_event_mode( "mode", r->mode );
	output_time( "time", FETCH_CARD32(r->time) );
	}
	break;

case X_GrabServer                   :
	output_packet( "X_GrabServer", " ", pcs->sr_c_seq_number  );
	break;

case X_UngrabServer                 :
	output_packet( "X_UngrabServer", " ", pcs->sr_c_seq_number  );
	break;

case X_QueryPointer                 :
	{
	xResourceReq	*r = req;

	output_packet( "X_QueryPointer", " ", pcs->sr_c_seq_number  );
	output_card32( "window", FETCH_CARD32(r->id) );
	}
	pcs->sr_last_request = req->reqType;
	break;

case X_GetMotionEvents              :
	{
	xGetMotionEventsReq *r = req;

	output_packet( "X_GetMotionEvents", " ", pcs->sr_c_seq_number  );
	output_card32( "window", FETCH_CARD32(r->window) );
	output_time( "start", FETCH_CARD32(r->start) );
	output_time( "stop", FETCH_CARD32(r->stop) );
	}
	pcs->sr_last_request = req->reqType;
	break;

case X_TranslateCoords              :
	{
	xTranslateCoordsReq *r = req;

	output_packet( "X_TranslateCoords", " ", pcs->sr_c_seq_number  );
	output_card32( "sourceWindow", FETCH_CARD32(r->srcWid) );
	output_card32( "destWindow", FETCH_CARD32(r->dstWid) );
	output_int16( "sourceX", FETCH_INT16(r->srcX) );
	output_int16( "sourceY", FETCH_INT16(r->srcY) );
	}
	pcs->sr_last_request = req->reqType;
	break;

case X_WarpPointer                  :
	{
	xWarpPointerReq	*r = req;

	output_packet( "X_WarpPointer", " ", pcs->sr_c_seq_number  );
	output_card32( "sourceWindow", FETCH_CARD32(r->srcWid) );
	output_card32( "destWindow", FETCH_CARD32(r->dstWid) );
	output_int16( "sourceX", FETCH_INT16(r->srcX) );
	output_int16( "sourceY", FETCH_INT16(r->srcY) );
	output_card16( "sourceWidth", FETCH_CARD16(r->srcWidth) );
	output_card16( "sourceHeight", FETCH_CARD16(r->srcHeight) );
	output_int16( "destX", FETCH_INT16(r->dstX) );
	output_int16( "destY", FETCH_INT16(r->dstY) );
	}
	break;

case X_SetInputFocus                :
	{
	xSetInputFocusReq   *r = req;

	output_packet( "X_SetInputFocus", " ", pcs->sr_c_seq_number  );
	output_card32( "focusWindow", FETCH_CARD32(r->focus) );
	output_time( "time", FETCH_CARD32(r->time) );
	switch (r->revertTo) {
case RevertToNone		: output_string8( "revertTo", 12, "RevertToNone" ); break;
case RevertToPointerRoot	: output_string8( "revertTo", 19, "RevertToPointerRoot" ); break;
case RevertToParent		: output_string8( "revertTo", 14, "RevertToParent" ); break;
default:			  output_card8( "revertTo (bad)", r->revertTo );
	    }
	}
	break;
	
case X_GetInputFocus                :
	output_packet( "X_GetInputFocus", " ", pcs->sr_c_seq_number  );
	pcs->sr_last_request = req->reqType;
	break;

case X_QueryKeymap                  :
	output_packet( "X_QueryKeymap", " ", pcs->sr_c_seq_number  );
	pcs->sr_last_request = req->reqType;
	break;

case X_OpenFont                     :
	{
	xOpenFontReq	*r = req;

	output_packet( "X_OpenFont", " ", pcs->sr_c_seq_number  );
	output_card32( "font", FETCH_CARD32(r->fid) );
	output_card16( "nbytes", FETCH_CARD16(r->nbytes) );
	output_string8( "font name", FETCH_CARD16(r->nbytes), (char *) (r+1) );
	}
	break;

case X_CloseFont                    :
	{
	xResourceReq	*r = req;

	output_packet( "X_CloseFont", " ", pcs->sr_c_seq_number  );
	output_card32( "font", FETCH_CARD32(r->id) );
	}
	break;

case X_QueryFont                    :
	{
	xResourceReq	*r = req;

	output_packet( "X_QueryFont", " ", pcs->sr_c_seq_number  );
	output_card32( "font", FETCH_CARD32(r->id) );
	}
	pcs->sr_last_request = req->reqType;
	break;

case X_QueryTextExtents             :
	{
	xQueryTextExtentsReq	*r = req;
	int i;

	i = max( 0, (32 + (FETCH_CARD16(r->length)<<2) - sizeof( xQueryTextExtentsReq ) ) );
	output_packet( "X_QueryTextExtents", " ", pcs->sr_c_seq_number  );
	output_card32( "font", FETCH_CARD32(r->fid) );
	output_bool( "oddLength", r->oddLength );
	output_string16( "Text", i, (char *) (r+1) );
	}
	pcs->sr_last_request = req->reqType;
	break;

case X_ListFonts                    :
	{
	xListFontsReq	*r = req;

	output_packet( "X_ListFonts", " ", pcs->sr_c_seq_number  );
	output_card16( "maxNames", FETCH_CARD16(r->maxNames) );
	output_card16( "nbytes", FETCH_CARD16(r->nbytes) );
	output_string8( "fontPattern", FETCH_CARD16(r->nbytes), (char *) (r+1) );
	}
	pcs->sr_last_request = req->reqType;
	break;

case X_ListFontsWithInfo    :
	{
	xListFontsWithInfoReq	*r = req;

	output_packet( "X_ListFontsWithInfo", " ", pcs->sr_c_seq_number  );
	output_card16( "maxNames", FETCH_CARD16(r->maxNames) );
	output_card16( "nbytes", FETCH_CARD16(r->nbytes) );
	output_string8( "fontPattern", FETCH_CARD16(r->nbytes), (char *) (r+1) );
	}
	pcs->sr_last_request = req->reqType;
	break;

case X_SetFontPath                  :
	{
	xSetFontPathReq	*r = req;
	char	*p;
	int	i;

	output_packet( "X_SetFontPath", " ", pcs->sr_c_seq_number  );
	output_card16( "nFonts", FETCH_CARD16(r->nFonts) );
	output_listofstring8( "fontPath", FETCH_CARD16(r->nFonts), (char *) (r+1) );
	}
	break;

case X_GetFontPath                  :
	output_packet( "X_GetFontPath", " ", pcs->sr_c_seq_number  );
	pcs->sr_last_request = req->reqType;
	break;

case X_CreatePixmap                 :
	{
	xCreatePixmapReq    *r = req;

	output_packet( "X_CreatePixmap", " ", pcs->sr_c_seq_number  );
	output_card32( "pid", FETCH_CARD32(r->pid) );
	output_card32( "drawable", FETCH_CARD32(r->drawable) );
	output_card8( "depth", r->depth );
	output_card16( "width", FETCH_CARD16(r->width) );
	output_card16( "height", FETCH_CARD16(r->height) );
	}
	break;

case X_FreePixmap                   :
	{
	xResourceReq	*r = req;

	output_packet( "X_FreePixmap", " ", pcs->sr_c_seq_number  );
	output_card32( "pid", FETCH_CARD32(r->id) );
	}
	break;

case X_CreateGC                     :
	{
	xCreateGCReq	*r = req;

	output_packet( "X_CreateGC", " ", pcs->sr_c_seq_number  );
	output_card32( "gc", FETCH_CARD32(r->gc) );
	output_card32( "drawable", FETCH_CARD32(r->drawable) );
	output_card32( "mask", FETCH_CARD32(r->mask) );
	output_gc_values( FETCH_CARD32(r->mask), (char *) (r+1) );
	}
	break;

case X_ChangeGC                     :
	{
	xChangeGCReq	*r = req;

	output_packet( "X_ChangeGC", " ", pcs->sr_c_seq_number  );
	output_card32( "gc", FETCH_CARD32(r->gc) );
	output_card32( "mask", FETCH_CARD32(r->mask) );
	output_gc_values( FETCH_CARD32(r->mask), (char *) (r+1) );
	}
	break;

case X_CopyGC                       :
	{
	xCopyGCReq  *r = req;

	output_packet( "X_CopyGC", " ", pcs->sr_c_seq_number  );
	output_card32( "sourceGC", FETCH_CARD32(r->srcGC) );
	output_card32( "destGC", FETCH_CARD32(r->dstGC) );
	output_card32( "mask", FETCH_CARD32(r->mask) );
	}
	break;

case X_SetDashes                    :
	{
	xSetDashesReq	*r = req;

	output_packet( "X_SetDashes", " ", pcs->sr_c_seq_number  );
	output_card32( "gc", FETCH_CARD32(r->gc) );
	output_card16( "dashOffset", FETCH_CARD16(r->dashOffset) );
	output_card16( "nDashes", FETCH_CARD16(r->nDashes) );
	output_listofcard8( "dashPattern", FETCH_CARD16(r->nDashes), (char *) (r+1) );
	}
	break;

case X_SetClipRectangles            :
	{
	xSetClipRectanglesReq	*r = req;
	int	i;

	output_packet( "X_SetClipRectanges", " ", pcs->sr_c_seq_number  );
	output_card32( "gc", FETCH_CARD32(r->gc) );
	output_int16( "xOrigin", FETCH_INT16(r->xOrigin) );
	output_int16( "yOrigin", FETCH_INT16(r->yOrigin) );
	output_ordering( "ordering", r->ordering );
	output_listofrectangle(	"rect",
				(((FETCH_CARD16(r->length)<<2)-sizeof(xSetClipRectanglesReq))/sizeof(xRectangle)),
				(xRectangle *) (r+1) );
	}
	break;

case X_FreeGC                       :
	{
	xResourceReq	*r = req;

	output_packet( "X_FreeGC", " ", pcs->sr_c_seq_number  );
	output_card32( "gc", FETCH_CARD32(r->id) );
	}
	break;

case X_ClearArea                    :
	{
	xClearAreaReq	*r = req;

	output_packet( "X_ClearArea", " ", pcs->sr_c_seq_number  );
	output_card32( "window", FETCH_CARD32(r->window) );
	output_bool( "exposures", r->exposures );
	output_int16( "x", FETCH_INT16(r->x) );
	output_int16( "y", FETCH_INT16(r->y) );
	output_card16( "width", FETCH_CARD16(r->width) );
	output_card16( "height", FETCH_CARD16(r->height) );
	}
	break;

case X_CopyArea                     :
	{
	xCopyAreaReq	*r = req;

	output_packet( "X_CopyArea", " ", pcs->sr_c_seq_number  );
	output_card32( "sourceDrawable", FETCH_CARD32(r->srcDrawable) );
	output_card32( "destDrawable", FETCH_CARD32(r->dstDrawable) );
	output_card32( "gc", FETCH_CARD32(r->gc) );
	output_int16( "sourceX", FETCH_INT16(r->srcX) );
	output_int16( "sourceY", FETCH_INT16(r->srcY) );
	output_int16( "destX", FETCH_INT16(r->dstX) );
	output_int16( "destY", FETCH_INT16(r->dstY) );
	output_card16( "width", FETCH_CARD16(r->width) );
	output_card16( "height", FETCH_CARD16(r->height) );
	}
	break;

case X_CopyPlane                    :
	{
	xCopyPlaneReq	*r = req;

	output_packet( "X_CopyPlane", " ", pcs->sr_c_seq_number  );
	output_card32( "sourceDrawable", FETCH_CARD32(r->srcDrawable) );
	output_card32( "destDrawable", FETCH_CARD32(r->dstDrawable) );
	output_card32( "gc", FETCH_CARD32(r->gc) );
	output_int16( "sourceX", FETCH_INT16(r->srcX) );
	output_int16( "sourceY", FETCH_INT16(r->srcY) );
	output_int16( "destX", FETCH_INT16(r->dstX) );
	output_int16( "destY", FETCH_INT16(r->dstY) );
	output_card16( "width", FETCH_CARD16(r->width) );
	output_card16( "height", FETCH_CARD16(r->height) );
	output_card32( "bitPlane", FETCH_CARD32(r->bitPlane) );
	}
	break;
	
case X_PolyPoint                    :
	{
	xPolyPointReq	*r = req;

	output_packet( "X_PolyPoint", " ", pcs->sr_c_seq_number  );
	output_card32( "drawable", FETCH_CARD32(r->drawable) );
	output_card32( "gc", FETCH_CARD32(r->gc) );
	output_coord_mode( "coordMode", r->coordMode );
	output_listofpoint( "point",
			    (((FETCH_CARD16(r->length)<<2)-sizeof(xPolyPointReq))/sizeof( xPoint )),
			    (xPoint *) (r+1)  );
	}
	break;

case X_PolyLine                     :
	{
	xPolyLineReq	*r = req;

	output_packet( "X_PolyLine", " ", pcs->sr_c_seq_number  );
	output_card32( "drawable", FETCH_CARD32(r->drawable) );
	output_card32( "gc", FETCH_CARD32(r->gc) );
	output_coord_mode( "coordMode", r->coordMode );
	output_listofpoint( "point",
			    (((FETCH_CARD16(r->length)<<2)-sizeof(xPolyLineReq))/sizeof( xPoint )),
			    (xPoint *) (r+1) );
	}
	break;

case X_PolySegment                  :
	{
	xPolySegmentReq	*r = req;

	output_packet( "X_PolySegment", " ", pcs->sr_c_seq_number  );
	output_card32( "drawable", FETCH_CARD32(r->drawable) );
	output_card32( "gc", FETCH_CARD32(r->gc) );
	output_listofsegment(	"segment",
				(((FETCH_CARD16(r->length)<<2)-sizeof(xPolySegmentReq))/sizeof( xSegment )),
				(xSegment *) (r+1) );
	}
	break;

case X_PolyRectangle                :
	{
	xPolyRectangleReq	*r = req;

	output_packet( "X_PolyRectangle", " ", pcs->sr_c_seq_number  );
	output_card32( "drawable", FETCH_CARD32(r->drawable) );
	output_card32( "gc", FETCH_CARD32(r->gc) );
	output_listofrectangle(	"rect",
				(((FETCH_CARD16(r->length)<<2)-sizeof(xPolyRectangleReq))/sizeof( xRectangle )),
				(xRectangle *) (r+1) );
	}
	break;

case X_PolyArc                      :
	{
	xPolyArcReq	*r = req;

	output_packet( "X_PolyArc", " ", pcs->sr_c_seq_number  );
	output_card32( "drawable", FETCH_CARD32(r->drawable) );
	output_card32( "gc", FETCH_CARD32(r->gc) );
	output_listofarc(   "arc",
			    (((FETCH_CARD16(r->length)<<2)-sizeof(xPolyArcReq))/sizeof( xArc )),
			    (xArc *) (r+1) );
	}
	break;

case X_FillPoly                     :
	{
	xFillPolyReq	*r = req;

	output_packet( "X_FillPoly", " ", pcs->sr_c_seq_number  );
	output_card32( "drawable", FETCH_CARD32(r->drawable) );
	output_card32( "gc", FETCH_CARD32(r->gc) );
	output_shape( "shape", r->shape );
	output_coord_mode( "coordMode", r->coordMode );
	output_listofpoint( "point",
			    (((FETCH_CARD16(r->length)<<2)-sizeof(xFillPolyReq))/sizeof( xPoint )),
			    (xPoint *) (r+1) );
	}
	break;

case X_PolyFillRectangle            :
	{
	xPolyFillRectangleReq	*r = req;

	output_packet( "X_PolyFillRectangle", " ", pcs->sr_c_seq_number  );
	output_card32( "drawable", FETCH_CARD32(r->drawable) );
	output_card32( "gc", FETCH_CARD32(r->gc) );
	output_listofrectangle(	"rect",
				(((FETCH_CARD16(r->length)<<2)-sizeof(xPolyFillRectangleReq))/sizeof( xRectangle ) ),
				(xRectangle *) (r+1) );
	}
	break;

case X_PolyFillArc                  :
	{
	xPolyFillArcReq	*r = req;

	output_packet( "X_PolyFillArc", " ", pcs->sr_c_seq_number  );
	output_card32( "drawable", FETCH_CARD32(r->drawable) );
	output_card32( "gc", FETCH_CARD32(r->gc) );
	output_listofarc(   "arc",
			    (((FETCH_CARD16(r->length)<<2)-sizeof(xPolyFillArcReq))/sizeof( xArc )),
			    (xArc *) (r+1) );
	}
	break;

case X_PutImage                     :
	{
	xPutImageReq	*r = req;

	output_packet( "X_PutImage", " ", pcs->sr_c_seq_number  );
	output_card32( "drawable", FETCH_CARD32(r->drawable) );
	output_card32( "gc", FETCH_CARD32(r->gc) );
	output_card16( "width", FETCH_CARD16(r->width) );
	output_card16( "height", FETCH_CARD16(r->height) );
	output_int16( "destX", FETCH_INT16(r->dstX) );
	output_int16( "destY", FETCH_INT16(r->dstY) );
	output_card8( "leftPad", r->leftPad );
	output_card8( "depth", r->depth );
	output_image_format( "format", r->format );
	output_image( "image data", ((FETCH_CARD16(r->length)<<2)-sizeof(xPutImageReq)), (char *) (r+1), FETCH_CARD16(r->width) );
	}
	break;

case X_GetImage                     :
	{
	xGetImageReq	*r = req;

	output_packet( "X_GetImage", " ", pcs->sr_c_seq_number  );
	output_card32( "drawable", FETCH_CARD32(r->drawable) );
	output_int16( "x", FETCH_INT16(r->x) );
	output_int16( "y", FETCH_INT16(r->y) );
	output_card16( "width", FETCH_CARD16(r->width) );
	output_card16( "height", FETCH_CARD16(r->height) );
	output_image_format( "format", r->format );
	output_card32( "planeMask", FETCH_CARD32(r->planeMask) );
	}
	pcs->sr_last_request = req->reqType;
	break;

case X_PolyText8                    :
	{
	xPolyText8Req	*r = req;

	output_packet( "X_PolyText8", " ", pcs->sr_c_seq_number  );
	output_card32( "drawable", FETCH_CARD32(r->drawable) );
	output_card32( "gc", FETCH_CARD32(r->gc) );
	output_int16( "x", FETCH_INT16(r->x) );
	output_int16( "y", FETCH_INT16(r->y) );
	output_listoftextitem( "text8", 1, (FETCH_CARD16(r->length)<<2) - sizeof(xPolyText8Req), (xTextElt *) (r+1) );
	}
	break;

case X_PolyText16                   :
	{
	xPolyText16Req	*r = req;

	output_packet( "X_PolyText16", " ", pcs->sr_c_seq_number  );
	output_card32( "drawable", FETCH_CARD32(r->drawable) );
	output_card32( "gc", FETCH_CARD32(r->gc) );
	output_int16( "x", FETCH_INT16(r->x) );
	output_int16( "y", FETCH_INT16(r->y) );
	output_listoftextitem( "text16", 2, (FETCH_CARD16(r->length)<<2) - sizeof(xPolyText16Req), (xTextElt *) (r+1) );
	}
	break;

case X_ImageText8                   :
	{
	xImageText8Req	*r = req;

	output_packet( "X_ImageText8", " ", pcs->sr_c_seq_number  );
	output_card32( "drawable", FETCH_CARD32(r->drawable) );
	output_card32( "gc", FETCH_CARD32(r->gc) );
	output_int16( "x", FETCH_INT16(r->x) );
	output_int16( "y", FETCH_INT16(r->y) );
	output_card8( "nChars", r->nChars );
	output_string8( "string8", r->nChars, (char *) (r+1) );
	}
	break;

case X_ImageText16                  :
	{
	xImageText16Req	*r = req;

	output_packet( "X_ImageText16", " ", pcs->sr_c_seq_number  );
	output_card32( "drawable", FETCH_CARD32(r->drawable) );
	output_card32( "gc", FETCH_CARD32(r->gc) );
	output_int16( "x", FETCH_INT16(r->x) );
	output_int16( "y", FETCH_INT16(r->y) );
	output_card8( "nChars", r->nChars );
	output_string16( "string16", r->nChars, (char *) (r+1) );
	}
	break;

case X_CreateColormap               :
	{
	xCreateColormapReq  *r = req;

	output_packet( "X_CreateColormap", " ", pcs->sr_c_seq_number  );
	output_card32( "colormap", FETCH_CARD32(r->mid) );
	output_card32( "window", FETCH_CARD32(r->window) );
	output_card32( "visual", FETCH_CARD32(r->visual) );
	switch (r->alloc) {
case AllocNone		:   output_string8( "alloc", 9, "AllocNone" ); break;
case AllocAll		:   output_string8( "alloc", 8, "AllocAll" ); break;
default:		    output_card8( "alloc (bad)", r->alloc ); break;
	    }
	}
	break;
	
case X_FreeColormap                 :
	{
	xResourceReq	*r = req;

	output_packet( "X_FreeColormap", " ", pcs->sr_c_seq_number  );
	output_card32( "colormap", FETCH_CARD32(r->id) );
	}
	break;

case X_CopyColormapAndFree          :
	{
	xCopyColormapAndFreeReq	*r = req;

	output_packet( "X_CopyColormapAndFree", " ", pcs->sr_c_seq_number  );
	output_card32( "sourceColormap", FETCH_CARD32(r->srcCmap) );
	output_card32( "destColormap", FETCH_CARD32(r->mid) );
	}
	break;

case X_InstallColormap              :
	{
	xResourceReq	*r = req;

	output_packet( "X_InstallColormap", " ", pcs->sr_c_seq_number  );
	output_card32( "colormap", FETCH_CARD32(r->id) );
	}
	break;

case X_UninstallColormap            :
	{
	xResourceReq	*r = req;

	output_packet( "X_uninstallColormap", " ", pcs->sr_c_seq_number  );
	output_card32( "colormap", FETCH_CARD32(r->id) );
	}
	break;

case X_ListInstalledColormaps       :
	{
	xResourceReq	*r = req;

	output_packet( "X_ListInstalledColormaps", " ", pcs->sr_c_seq_number  );
	output_card32( "window", FETCH_CARD32(r->id) );
	}
	pcs->sr_last_request = req->reqType;
	break;

case X_AllocColor                   :
	{
	xAllocColorReq	*r = req;

	output_packet( "X_AllocColor", " ", pcs->sr_c_seq_number  );
	output_card32( "colormap", FETCH_CARD32(r->cmap) );
	output_card16( "red", FETCH_CARD16(r->red) );
	output_card16( "green", FETCH_CARD16(r->green) );
	output_card16( "blue", FETCH_CARD16(r->blue) );
	}
	pcs->sr_last_request = req->reqType;
	break;

case X_AllocNamedColor              :
	{
	xAllocNamedColorReq *r = req;

	output_packet( "X_AllocNamedColor", " ", pcs->sr_c_seq_number  );
	output_card32( "colormap", FETCH_CARD32(r->cmap) );
	output_card16( "nbytes", FETCH_CARD16(r->nbytes) );
	output_string8( "namedColor", FETCH_CARD16(r->nbytes), (char *) (r+1) );
	}
	pcs->sr_last_request = req->reqType;
	break;

case X_AllocColorCells              :
	{
	xAllocColorCellsReq *r = req;

	output_packet( "X_AllocColorCells", " ", pcs->sr_c_seq_number  );
	output_card32( "colormap", FETCH_CARD32(r->cmap) );
	output_card16( "colors", FETCH_CARD16(r->colors) );
	output_card16( "planes", FETCH_CARD16(r->planes) );
	output_bool( "continuous", r->contiguous );
	}
	pcs->sr_last_request = req->reqType;
	break;

case X_AllocColorPlanes             :
	{
	xAllocColorPlanesReq *r = req;

	output_packet( "X_AllocColorPlanes", " ", pcs->sr_c_seq_number  );
	output_card32( "colormap", FETCH_CARD32(r->cmap) );
	output_card16( "colors", FETCH_CARD16(r->colors) );
	output_card16( "red", FETCH_CARD16(r->red) );
	output_card16( "green", FETCH_CARD16(r->green) );
	output_card16( "blue", FETCH_CARD16(r->blue) );
	output_bool( "continuous", r->contiguous );
	}
	pcs->sr_last_request = req->reqType;
	break;

case X_FreeColors                   :
	{
	xFreeColorsReq	*r = req;

	output_packet( "X_FreeColors", " ", pcs->sr_c_seq_number  );
	output_card32( "colormap", FETCH_CARD32(r->cmap) );
	output_card32( "planeMask", FETCH_CARD32(r->planeMask) );
	output_listofcard32(	"pixel",
				((FETCH_CARD16(r->length)<<2)-sizeof(xFreeColorsReq))>>2,
				(int *) (r+1) );
	}
	break;

case X_StoreColors                  :
	{
	xStoreColorsReq	*r = req;

	output_packet( "X_StoreColors", " ", pcs->sr_c_seq_number  );
	output_card32( "colormap", FETCH_CARD32(r->cmap) );
	output_listofcoloritem(	"colorItem",
				(((FETCH_CARD16(r->length)<<2)-sizeof(xStoreColorsReq))/sizeof(xColorItem)),
				(xColorItem *) (r+1) );
	}
	break;

case X_StoreNamedColor              :
	{
	xStoreNamedColorReq *r = req;

	output_packet( "X_StoreNamedColor", " ", pcs->sr_c_seq_number  );
	output_card32( "colormap", FETCH_CARD32(r->cmap) );
	output_card32( "pixel", FETCH_CARD32(r->pixel) );
	output_card8( "flags", r->flags );
	output_card16( "nbytes", FETCH_CARD16(r->nbytes) );
	output_string8( "colorName", FETCH_CARD16(r->nbytes), (char *) (r+1) );
	}
	break;

case X_QueryColors                  :
	{
	xQueryColorsReq	*r  = req;

	output_packet( "X_QueryColors", " ", pcs->sr_c_seq_number  );
	output_card32( "colormap", FETCH_CARD32(r->cmap) );
	output_listofcard32(	"pixel",
				(((FETCH_CARD16(r->length)<<2)-sizeof(xQueryColorsReq))>>2),
				(int *) (r+1) );
	}
	pcs->sr_last_request = req->reqType;
	break;

case X_LookupColor                  :
	{
	xLookupColorReq	*r = req;

	output_packet( "X_LookupColor", " ", pcs->sr_c_seq_number  );
	output_card32( "colormap", FETCH_CARD32(r->cmap) );
	output_card16( "nbytes", FETCH_CARD16(r->nbytes) );
	output_string8( "colorName", FETCH_CARD16(r->nbytes), (char *) (r+1) );
	}
	pcs->sr_last_request = req->reqType;
	break;

case X_CreateCursor                 :
	{
	xCreateCursorReq    *r = req;

	output_packet( "X_CreateCursor", " ", pcs->sr_c_seq_number  );
	output_card32( "cursor", FETCH_CARD32(r->cid) );
	output_card32( "sourcePixmap", FETCH_CARD32(r->source) );
	output_card32( "maskPixmap", FETCH_CARD32(r->mask) );
	output_card16( "foregroundRed", FETCH_CARD16(r->foreRed) );
	output_card16( "foregroundGreen", FETCH_CARD16(r->foreGreen) );
	output_card16( "foregroundBlue", FETCH_CARD16(r->foreBlue) );
	output_card16( "backgroundRed", FETCH_CARD16(r->backRed) );
	output_card16( "backgroundGreen", FETCH_CARD16(r->backGreen) );
	output_card16( "backgroundBlue", FETCH_CARD16(r->backBlue) );
	output_card16( "x", FETCH_CARD16(r->x) );
	output_card16( "y", FETCH_CARD16(r->y) );
	}
	break;

case X_CreateGlyphCursor            :
	{
	xCreateGlyphCursorReq    *r = req;

	output_packet( "X_CreateGlyphCursor", " ", pcs->sr_c_seq_number  );
	output_card32( "cursor", FETCH_CARD32(r->cid) );
	output_card32( "sourceFont", FETCH_CARD32(r->source) );
	output_card32( "maskFont", FETCH_CARD32(r->mask) );
	output_card16( "sourceChar", FETCH_CARD16(r->sourceChar) );
	output_card16( "maskChar", FETCH_CARD16(r->maskChar) );
	output_card16( "foregroundRed", FETCH_CARD16(r->foreRed) );
	output_card16( "foregroundGreen", FETCH_CARD16(r->foreGreen) );
	output_card16( "foregroundBlue", FETCH_CARD16(r->foreBlue) );
	output_card16( "backgroundRed", FETCH_CARD16(r->backRed) );
	output_card16( "backgroundGreen", FETCH_CARD16(r->backGreen) );
	output_card16( "backgroundBlue", FETCH_CARD16(r->backBlue) );
	}
	break;

case X_FreeCursor                   :
	{
	xResourceReq	*r = req;

	output_packet( "X_FreeCursor", " ", pcs->sr_c_seq_number  );
	output_card32( "cursor", FETCH_CARD32(r->id) );
	}
	break;

case X_RecolorCursor                :
	{
	xRecolorCursorReq    *r = req;

	output_packet( "X_RecolorCursor", " ", pcs->sr_c_seq_number  );
	output_card32( "cursor", FETCH_CARD32(r->cursor) );
	output_card16( "foregroundRed", FETCH_CARD16(r->foreRed) );
	output_card16( "foregroundGreen", FETCH_CARD16(r->foreGreen) );
	output_card16( "foregroundBlue", FETCH_CARD16(r->foreBlue) );
	output_card16( "backgroundRed", FETCH_CARD16(r->backRed) );
	output_card16( "backgroundGreen", FETCH_CARD16(r->backGreen) );
	output_card16( "backgroundBlue", FETCH_CARD16(r->backBlue) );
	}
	break;

case X_QueryBestSize                :
	{
	xQueryBestSizeReq   *r = req;

	output_packet( "X_QueryBestSize", " ", pcs->sr_c_seq_number  );
	output_pixmap_class( "class", r->class );
	output_card32( "drawable", FETCH_CARD32(r->drawable) );
	output_card16( "width", FETCH_CARD16(r->width) );
	output_card16( "height", FETCH_CARD16(r->height) );
	}
	pcs->sr_last_request = req->reqType;
	break;

case X_QueryExtension               :
	{
	xQueryExtensionReq  *r = req;

	output_packet( "X_QueryExtension", " ", pcs->sr_c_seq_number  );
	output_card16( "nbytes", FETCH_CARD16(r->nbytes) );
	output_string8( "extensionName", FETCH_CARD16(r->nbytes), (char *) (r+1) );
	}
	pcs->sr_last_request = req->reqType;
	break;

case X_ListExtensions               :
	output_packet( "X_ListExtensions", " ", pcs->sr_c_seq_number  );
	pcs->sr_last_request = req->reqType;
	break;

case X_ChangeKeyboardMapping        :
	{
	xChangeKeyboardMappingReq   *r = req;

	output_packet( "X_ChangeKeyboardMapping", " ", pcs->sr_c_seq_number  );
	output_card8( "firstKeyCode", r->firstKeyCode );
	output_card8( "keySymsPerKeyCode", r->keySymsPerKeyCode );
	output_card8( "keyCodes", r->keyCodes );
	output_listofkeysyms( "keySym", r->keySymsPerKeyCode, (char *) (r+1) );
	}
	break;

case X_GetKeyboardMapping           :
	{
	xGetKeyboardMappingReq	*r = req;

	output_packet( "X_GetKeyboardMapping", " ", pcs->sr_c_seq_number  );
	output_card8( "firstKeyCode", r->firstKeyCode );
	output_card8( "count", r->count );
	}
	pcs->sr_last_request = req->reqType;
	break;

case X_ChangeKeyboardControl        :
	{
	xChangeKeyboardControlReq   *r = req;

	output_packet( "X_ChangeKeyboardControl", " ", pcs->sr_c_seq_number  );
	output_card32( "mask", FETCH_CARD32(r->mask) );
	output_kbd_values( FETCH_CARD32(r->mask), (char *) (r+1) );
	}
	pcs->sr_last_request = req->reqType;
	break;

case X_GetKeyboardControl           :
	output_packet( "X_GetKeyboardControl", " ", pcs->sr_c_seq_number  );
	break;

case X_Bell                         :
	{
	xBellReq    *r = req;

	output_packet( "X_Bell", " ", pcs->sr_c_seq_number  );
	output_card8( "percent", r->percent );
	}
	break;

case X_ChangePointerControl         :
	{
	xChangePointerControlReq    *r = req;

	output_packet( "X_ChangePointerControl", " ", pcs->sr_c_seq_number  );
	output_int16( "accelNumerator", FETCH_INT16(r->accelNum) );
	output_int16( "accelDenominator", FETCH_INT16(r->accelDenum) );
	output_int16( "threshold", FETCH_INT16(r->threshold) );
	output_bool( "doAccel", r->doAccel );
	output_bool( "doThresh", r->doThresh );
	}
	break;

case X_GetPointerControl            :
	output_packet( "X_GetPointerControl", " ", pcs->sr_c_seq_number  );
	break;

case X_SetScreenSaver               :
	{
	xSetScreenSaverReq	*r = req;

	output_packet( "X_SetScreenSaver", " ", pcs->sr_c_seq_number  );
	output_int16( "timeout", FETCH_INT16(r->timeout) );
	output_int16( "interval", FETCH_INT16(r->interval) );
	switch (r->preferBlank) {
case DontPreferBlanking	: output_string8( "preferBlanking", 18, "DontPreferBlanking" ); break;
case PreferBlanking	: output_string8( "preferBlanking", 14, "PreferBlanking" ); break;
case DefaultBlanking	: output_string8( "preferBlanking", 15, "DefaultBlanking" ); break;
default			: output_card8( "preferBlanking (bad)", r->preferBlank ); break;
	    }
	switch (r->allowExpose) {
case DontAllowExposures	: output_string8( "allowExposures", 18, "DontAllowExposures" ); break;
case AllowExposures	: output_string8( "allowExposures", 14, "AllowExposures" ); break;
case DefaultExposures	: output_string8( "allowExposures", 16, "DefaultExposures" ); break;
default			: output_card8( "allowExposures (bad)", r->allowExpose ); break;
	    }
	}
	break;

case X_GetScreenSaver               :
	output_packet( "X_GetScreenSaver", " ", pcs->sr_c_seq_number  );
	pcs->sr_last_request = req->reqType;
	break;

case X_ChangeHosts                  :
	{
	xChangeHostsReq	*r = req;

	output_packet( "X_ChangeHosts", " ", pcs->sr_c_seq_number  );
	output_host_mode( "mode", r->mode );
	output_card8( "hostFamily", r->hostFamily );
	output_card16( "hostLength", FETCH_CARD16(r->hostLength) );
	output_listofcard8( "hostAddr", FETCH_CARD16(r->hostLength), (char *) (r+1) );
	}
	break;

case X_ListHosts                    :
	output_packet( "X_ListHosts", " ", pcs->sr_c_seq_number  );
	pcs->sr_last_request = req->reqType;
	break;

case X_SetAccessControl             :
	{
	xSetAccessControlReq	*r = req;

	output_packet( "X_SetAccessControl", " ", pcs->sr_c_seq_number  );
	output_change_mode( "mode", r->mode );
	}
	break;

case X_SetCloseDownMode             :
	{
	xSetCloseDownModeReq	*r = req;

	output_packet( "X_SetCloseDownMode", " ", pcs->sr_c_seq_number  );
	output_change_mode( "mode", r->mode );
	}
	break;

case X_KillClient                   :
	{
	xResourceReq	*r = req;

	output_packet( "X_KillClient", " ", pcs->sr_c_seq_number  );
	output_card32( "resource", FETCH_CARD32(r->id) );
	}
	break;

case X_RotateProperties:
	{
	xRotatePropertiesReq	*r = req;

	output_packet( "X_RotateProperties", " ", pcs->sr_c_seq_number  );
	output_card32( "window", FETCH_CARD32(r->window) );
	output_int16( "nPositions", FETCH_INT16(r->nPositions) );
	output_card16( "nAtoms", FETCH_CARD16(r->nAtoms) );
	output_listofcard32( "atom", FETCH_CARD16(r->nAtoms), (int *) (r+1) );
	}
	break;

case X_ForceScreenSaver:
	{
	xForceScreenSaverReq	*r = req;

	output_packet( "X_ForceScreenSaver", " ", pcs->sr_c_seq_number  );
	output_change_mode( "mode", r->mode );
	}
	break;

case X_SetPointerMapping            :
	{
	xSetPointerMappingReq	*r = req;

	output_packet( "X_SetPointerMapping", " ", pcs->sr_c_seq_number  );
	output_card8( "nElements", r->nElts );
	output_listofcard8( "element", r->nElts, (char *) (r+1) );
	}
	pcs->sr_last_request = req->reqType;
	break;

case X_GetPointerMapping            :
	output_packet( "X_GetPointerMapping", " ", pcs->sr_c_seq_number  );
	pcs->sr_last_request = req->reqType;
	break;

case X_SetModifierMapping:
	{
	xSetModifierMappingReq	*r = req;

	output_packet( "X_SetModifierMapping", " ", pcs->sr_c_seq_number  );
	output_card8( "keycodesPerModifier", r->numKeyPerModifier );
	output_listofcard8( "keycode", 8 * r->numKeyPerModifier, (char *) (r+1) );
	}
	pcs->sr_last_request = req->reqType;
	break;

case X_GetModifierMapping:
	output_packet( "X_GetModifierMapping", " ", pcs->sr_c_seq_number  );
	pcs->sr_last_request = req->reqType;
	break;

case X_NoOperation:
	output_packet( "X_NoOperation", " ", pcs->sr_c_seq_number  );
	break;

default:
	if ( req->reqType < 128 ) {
	    output_packet( "Unimplemented Core MajorOpCode", " ", pcs->sr_c_seq_number  );
	    output_card8( "majorOpCode", req->reqType );
	} else {
	    output_extension_request( req );
	}
	pcs->sr_last_request = req->reqType;
	break;
    }
}


int
output_card8( prefix, value )
char		*prefix;
unsigned char	value;
{
    if ( !xw_form_brief )
        fprintf( out_fp, "\t%20s: %4u (0x%2x '%c')\n", prefix, value, value, (isprint(value)?(value&127):'.') );
}

int
output_int8( prefix, value )
char		*prefix;
char		value;
{
    if ( !xw_form_brief )
        fprintf( out_fp, "\t%20s: %4d (0x%2x '%c')\n", prefix, value, value, (isprint(value)?(value&127):'.') );
}

int
output_card16( prefix, value )
char		*prefix;
unsigned short	value;
{
    if ( !xw_form_brief )
        output_card16_always( prefix, value);
}

int
output_card16_always( prefix, value )
char		*prefix;
unsigned short	value;
{
    fprintf( out_fp, "\t%20s: %6u (0x%4x)\n", prefix, value, value );
}

int
output_int16( prefix, value )
char		*prefix;
short		value;
{
    if ( !xw_form_brief )
        fprintf( out_fp, "\t%20s: %6d (0x%4x)\n", prefix, value, value );
}

int
output_card32( prefix, value )
char		*prefix;
unsigned long	value;
{
    if ( !xw_form_brief )
        fprintf( out_fp, "\t%20s: %10u (0x%8x)\n", prefix, value, value );
}

int
output_int32( prefix, value )
char		*prefix;
long		value;
{
    if ( !xw_form_brief )
        fprintf( out_fp, "\t%20s: %10d (0x%8x)\n", prefix, value, value );
}

/*
 * LISTOFMUMBLE output routines
 */
int
output_listofcard8( prefix, nval, value )
char		*prefix;
int		nval;
unsigned char	*value;
{
    int	    i, j, len;
    unsigned char   *dv = value,
		    *hv = value,
		    *cv = value,
		    c;

    if ( !xw_form_brief )
        for ( i=0; i<nval; i+=8 ) {
	    len = min( nval, i+8 );
	    fprintf( out_fp, "\t%13s %6d-%6d: |", prefix, i, len-1 );
	    for ( j=i; j<len; j++ ) fprintf( out_fp, " %02X", *hv++ );
	    fprintf( out_fp, "| " );
	    for ( j=i; j<len; j++ )	fprintf( out_fp, " %4u", *dv++ );
	    fprintf( out_fp, " '" );
	    for ( j=i; j<len; j++ )	fprintf( out_fp, "%c", (isprint((c=(*cv++)))?(c&127):'.') );
	    fprintf( out_fp, "'\n" );
        }
}

int
output_listofint8( prefix, nval, value )
char		*prefix;
int		nval;
char		*value;
{
    int	    i, j, len;
    char	    *dv = value,
		    *hv = value,
		    *cv = value,
		    c;

    if ( !xw_form_brief )
        for ( i=0; i<nval; i+=8 ) {
	    len = min( nval, i+8 );
	    fprintf( out_fp, "\t%13s %6d-%6d: |", prefix, i, len-1 );
	    for ( j=i; j<len; j++ ) fprintf( out_fp, " %02X", *hv++ );
	    fprintf( out_fp, "| " );
	    for ( j=i; j<len; j++ )	fprintf( out_fp, " %4d", *dv++ );
	    fprintf( out_fp, " '" );
	    for ( j=i; j<len; j++ )	fprintf( out_fp, "%c", (isprint((c=(*cv++)))?(c&127):'.') );
	    fprintf( out_fp, "'\n" );
        }
}

int     
output_listofcard16( prefix, nval, value )
char		*prefix;
int		nval;
unsigned short	*value;
{
    int	    i, j, len;
    unsigned short  *dv = value,
		    *hv = value,
		    v;

    if ( !xw_form_brief )
        for ( i=0; i<nval; i+=4 ) {
	    len = min( nval, i+4 );
	    fprintf( out_fp, "\t%13s %6d-%6d: |", prefix, i, len-1 );
	    for ( j=i; j<len; j++ ) { v = *hv++; fprintf( out_fp, " %04X", FETCH_CARD16(v) ); }
	    fprintf( out_fp, "| " );
	    for ( j=i; j<len; j++ ) { v = *dv++; fprintf( out_fp, " %6u", FETCH_CARD16(v) ); }
	    fprintf( out_fp, "\n" );
        }
}

int
output_listofint16( prefix, nval, value )
char		*prefix;
int		nval;
short		*value;
{
    int	    i, j, len;
    short	    *dv = value,
		    *hv = value,
		    v;

    if ( !xw_form_brief )
        for ( i=0; i<nval; i+=4 ) {
	    len = min( nval, i+4 );
	    fprintf( out_fp, "\t%13s %6d-%6d: |", prefix, i, len-1 );
	    for ( j=i; j<len; j++ ) { v = *hv++; fprintf( out_fp, " %04X", FETCH_INT16(v) ); }
	    fprintf( out_fp, "| " );
	    for ( j=i; j<len; j++ ) { v = *dv++; fprintf( out_fp, " %6u", FETCH_INT16(v) ); }
	    fprintf( out_fp, "\n" );
        }
}

int
output_listofcard32( prefix, nval, value )
char		*prefix;
int		nval;
unsigned long	*value;
{
    int	    i, j, len;
    unsigned long   *dv = value,
		    *hv = value,
		    v;

    if ( !xw_form_brief )
        for ( i=0; i<nval; i+=4 ) {
	    len = min( nval, i+4 );
	    fprintf( out_fp, "\t%13s %6d-%6d: |", prefix, i, len-1 );
	    for ( j=i; j<len; j++ ) { v = *hv++; fprintf( out_fp, " %08X", FETCH_CARD32(v) ); }
	    fprintf( out_fp, "| " );
	    for ( j=i; j<len; j++ ) { v = *dv++; fprintf( out_fp, " %10u", FETCH_CARD32(v) ); }
	    fprintf( out_fp, "\n" );
        }
}

int
output_listofint32( prefix, nval, value )
char		*prefix;
int		nval;
long		*value;
{
    int	    i, j, len;
    long	    *dv = value,
		    *hv = value,
		    v;

    if ( !xw_form_brief )
        for ( i=0; i<nval; i+=4 ) {
	    len = min( nval, i+4 );
	    fprintf( out_fp, "\t%13s %6d-%6d: |", prefix, i, len-1 );
	    for ( j=i; j<len; j++ ) { v = *hv++; fprintf( out_fp, " %08X", FETCH_INT32(v) ); }
	    fprintf( out_fp, "| " );
	    for ( j=i; j<len; j++ ) { v = *dv++; fprintf( out_fp, " %10u", FETCH_INT32(v) ); }
	    fprintf( out_fp, "\n" );
        }
}


int
output_string8( prefix, len, str )
char		*prefix;
long		len;
char		*str;
{
    if ( !xw_form_brief ) {
	len = min( len, 255 );
	strncpy( string_buffer, str, len );
	string_buffer[len] = '\0';
        fprintf( out_fp, "\t%20s: \"%*s\"\n", prefix, len, (len!=0?string_buffer:"\0") );
    }
}



int
output_string16( prefix, len, str )
char		*prefix;
long		len;
char		*str;
{
    int	    i;

    if ( !xw_form_brief ) {
        fprintf( out_fp, "\t%20s: ", prefix );
        for ( i=0; i<len; i++ ) fprintf( out_fp, "(%3x,%3x) ", *str++, *str++ );
        fprintf( out_fp, "\n" );
    }
}
                                                


int  
output_event( e )
xEvent	*e;
{

    switch (e->u.u.type) {

case KeyPress		:
case KeyRelease		:
case ButtonPress		:
case ButtonRelease		:
	switch (e->u.u.type) {
  case KeyPress: output_packet( "X_Event (KeyPress)", "E", 0 ); break;
  case KeyRelease: output_packet( "X_Event (KeyRelease)", "E", 0 ); break;
  case ButtonPress: output_packet( "X_Event (ButtonPress)", "E", 0 ); break;
  case ButtonRelease: output_packet( "X_Event (ButtonRelease)", "E", 0 ); break;
	}
	output_card16_always( "sequenceNumber", FETCH_CARD16(e->u.u.sequenceNumber) );
	output_card8( "detail", e->u.u.detail );
	output_time( "time", FETCH_CARD32(e->u.keyButtonPointer.time) );
	output_card32( "root", FETCH_CARD32(e->u.keyButtonPointer.root) );
	output_card32( "event", FETCH_CARD32(e->u.keyButtonPointer.event) );
	output_card32( "child", FETCH_CARD32(e->u.keyButtonPointer.child) );
	output_int16( "rootX", FETCH_INT16(e->u.keyButtonPointer.rootX) );
	output_int16( "rootY", FETCH_INT16(e->u.keyButtonPointer.rootY) );
	output_int16( "eventX", FETCH_INT16(e->u.keyButtonPointer.eventX) );
	output_int16( "eventY", FETCH_INT16(e->u.keyButtonPointer.eventY) );
	output_card16( "state", FETCH_CARD16(e->u.keyButtonPointer.state) );
	output_bool( "sameScreen", e->u.keyButtonPointer.sameScreen );
	break;

case MotionNotify		:
case EnterNotify		:
case LeaveNotify		:
	switch (e->u.u.type) {
  case MotionNotify: output_packet( "X_Event (MotionNotify)", "E", 0 ); break;
  case EnterNotify: output_packet( "X_Event (EnterNotify)", "E", 0 ); break;
  case LeaveNotify: output_packet( "X_Event (LeaveNotify)", "E", 0 ); break;
	}
	output_card16_always( "sequenceNumber", FETCH_CARD16(e->u.u.sequenceNumber) );
	output_card8( "detail", e->u.u.detail );
	output_time( "time", FETCH_CARD32(e->u.enterLeave.time) );
	output_card32( "root", FETCH_CARD32(e->u.enterLeave.root) );
	output_card32( "event", FETCH_CARD32(e->u.enterLeave.event) );
	output_card32( "child", FETCH_CARD32(e->u.enterLeave.child) );
	output_int16( "rootX", FETCH_INT16(e->u.enterLeave.rootX) );
	output_int16( "rootY", FETCH_INT16(e->u.enterLeave.rootY) );
	output_int16( "eventX", FETCH_INT16(e->u.enterLeave.eventX) );
	output_int16( "eventY", FETCH_INT16(e->u.enterLeave.eventY) );
	output_card16( "state", FETCH_CARD16(e->u.enterLeave.state) );
	output_card8( "mode", e->u.enterLeave.mode );
	output_card8( "flags", e->u.enterLeave.flags );
	break;

case FocusIn			:
	output_packet( "X_Event (FocusIn)", "E", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(e->u.u.sequenceNumber) );
	output_card8( "detail", e->u.u.detail );
	output_card32( "window", FETCH_CARD32(e->u.focus.window) );
	output_card8( "mode", e->u.focus.mode );
	break;

case FocusOut		:
	output_packet( "X_Event (FocusOut)", "E", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(e->u.u.sequenceNumber) );
	output_card8( "detail", e->u.u.detail );
	output_card32( "window", FETCH_CARD32(e->u.focus.window) );
	output_card8( "mode", e->u.focus.mode );
	break;

case KeymapNotify		:
	output_packet( "X_Event (KeymapNotify)", "E", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(e->u.u.sequenceNumber) );
	output_card8( "detail", e->u.u.detail );
	break;

case Expose			:
	output_packet( "X_Event (Expose)", "E", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(e->u.u.sequenceNumber) );
	output_card8( "detail", e->u.u.detail );
	output_card32( "window", FETCH_CARD32(e->u.expose.window) );
	output_card16( "x", FETCH_CARD16(e->u.expose.x) );
	output_card16( "y", FETCH_CARD16(e->u.expose.y) );
	output_card16( "width", FETCH_CARD16(e->u.expose.width) );
	output_card16( "height", FETCH_CARD16(e->u.expose.height) );
	output_card16( "count", FETCH_CARD16(e->u.expose.count) );
	break;

case GraphicsExpose		:
	output_packet( "X_Event (GraphicsExposure)", "E", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(e->u.u.sequenceNumber) );
	output_card8( "detail", e->u.u.detail );
	output_card32( "drawable", FETCH_CARD32(e->u.graphicsExposure.drawable) );
	output_card16( "x", FETCH_CARD16(e->u.graphicsExposure.x) );
	output_card16( "y", FETCH_CARD16(e->u.graphicsExposure.y) );
	output_card16( "width", FETCH_CARD16(e->u.graphicsExposure.width) );
	output_card16( "height", FETCH_CARD16(e->u.graphicsExposure.height) );
	output_card16( "minorEvent", FETCH_CARD16(e->u.graphicsExposure.minorEvent) );
	output_card16( "count", FETCH_CARD16(e->u.graphicsExposure.count) );
	output_card8( "majorEvent", e->u.graphicsExposure.majorEvent );
	break;

case NoExpose		:
	output_packet( "X_Event (NoExposure)", "E", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(e->u.u.sequenceNumber) );
	output_card8( "detail", e->u.u.detail );
	output_card32( "drawable", FETCH_CARD32(e->u.noExposure.drawable) );
	output_card16( "minorEvent", FETCH_CARD16(e->u.noExposure.minorEvent) );
	output_card8( "majorEvent", e->u.noExposure.majorEvent );
	break;

case VisibilityNotify	:
	output_packet( "X_Event (VisibilityNotify)", "E", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(e->u.u.sequenceNumber) );
	output_card8( "detail", e->u.u.detail );
	output_card32( "window", FETCH_CARD32(e->u.visibility.window) );
	output_card8( "state", e->u.visibility.state );
	break;

case CreateNotify		:
	output_packet( "X_Event (CreateNotify)", "E", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(e->u.u.sequenceNumber) );
	output_card8( "detail", e->u.u.detail );
	output_card32( "window", FETCH_CARD32(e->u.createNotify.window) );
	output_card32( "parent", FETCH_CARD32(e->u.createNotify.parent) );
	output_card16( "x", FETCH_CARD16(e->u.createNotify.x) );
	output_card16( "y", FETCH_CARD16(e->u.createNotify.y) );
	output_card16( "width", FETCH_CARD16(e->u.createNotify.width) );
	output_card16( "height", FETCH_CARD16(e->u.createNotify.height) );
	output_card16( "borderWidth", FETCH_CARD16(e->u.createNotify.borderWidth) );
	output_bool( "override", e->u.createNotify.override );
	break;

case DestroyNotify		:
	output_packet( "X_Event (DestroyNotify)", "E", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(e->u.u.sequenceNumber) );
	output_card8( "detail", e->u.u.detail );
	output_card32( "event", FETCH_CARD32(e->u.destroyNotify.event) );
	output_card32( "window", FETCH_CARD32(e->u.destroyNotify.window) );
	break;

case UnmapNotify		:
	output_packet( "X_Event (UnmapNotify)", "E", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(e->u.u.sequenceNumber) );
	output_card8( "detail", e->u.u.detail );
	output_card32( "event", FETCH_CARD32(e->u.unmapNotify.event) );
	output_card32( "window", FETCH_CARD32(e->u.unmapNotify.window) );
	output_bool( "fromConfigure", e->u.unmapNotify.fromConfigure );
	break;

case MapNotify		:
	output_packet( "X_Event (MapNotify)", "E", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(e->u.u.sequenceNumber) );
	output_card8( "detail", e->u.u.detail );
	output_card32( "event", FETCH_CARD32(e->u.mapNotify.event) );
	output_card32( "window", FETCH_CARD32(e->u.mapNotify.window) );
	output_bool( "override", e->u.mapNotify.override );
	break;

case MapRequest		:
	output_packet( "X_Event (MapRequest)", "E", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(e->u.u.sequenceNumber) );
	output_card8( "detail", e->u.u.detail );
	output_card32( "parent", FETCH_CARD32(e->u.mapRequest.parent) );
	output_card32( "window", FETCH_CARD32(e->u.mapRequest.window) );
	break;

case ReparentNotify		:
	output_packet( "X_Event (ReparentNotify)", "E", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(e->u.u.sequenceNumber) );
	output_card8( "detail", e->u.u.detail );
	output_card32( "event", FETCH_CARD32(e->u.reparent.event) );
	output_card32( "window", FETCH_CARD32(e->u.reparent.window) );
	output_card32( "parent", FETCH_CARD32(e->u.reparent.parent) );
	output_int16( "x", FETCH_INT16(e->u.reparent.x) );
	output_int16( "y", FETCH_INT16(e->u.reparent.y) );
	output_bool( "override", e->u.reparent.override );
	break;

case ConfigureNotify		:
	output_packet( "X_Event (ConfigureNotify)", "E", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(e->u.u.sequenceNumber) );
	output_card8( "detail", e->u.u.detail );
	output_card32( "event", FETCH_CARD32(e->u.configureNotify.event) );
	output_card32( "window", FETCH_CARD32(e->u.configureNotify.window) );
	output_card32( "aboveSibling", FETCH_CARD32(e->u.configureNotify.aboveSibling) );
	output_int16( "x", FETCH_INT16(e->u.configureNotify.x) );
	output_int16( "y", FETCH_INT16(e->u.configureNotify.y) );
	output_card16( "width", FETCH_CARD16(e->u.configureNotify.width) );
	output_card16( "height", FETCH_CARD16(e->u.configureNotify.height) );
	output_card16( "borderWidth", FETCH_CARD16(e->u.configureNotify.borderWidth) );
	output_bool( "override", e->u.configureNotify.override );
	break;

case ConfigureRequest	:
	output_packet( "X_Event (ConfigureRequest)", "E", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(e->u.u.sequenceNumber) );
	output_card8( "detail", e->u.u.detail );
	output_card32( "parent", FETCH_CARD32(e->u.configureRequest.parent) );
	output_card32( "window", FETCH_CARD32(e->u.configureRequest.window) );
	output_card32( "sibling", FETCH_CARD32(e->u.configureRequest.sibling) );
	output_int16( "x", FETCH_INT16(e->u.configureRequest.x) );
	output_int16( "y", FETCH_INT16(e->u.configureRequest.y) );
	output_card16( "width", FETCH_CARD16(e->u.configureRequest.width) );
	output_card16( "height", FETCH_CARD16(e->u.configureRequest.height) );
	output_card16( "borderWidth", FETCH_CARD16(e->u.configureRequest.borderWidth) );

	break;

case GravityNotify		:
	output_packet( "X_Event (Gravity)", "E", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(e->u.u.sequenceNumber) );
	output_card8( "detail", e->u.u.detail );
	output_card32( "event", FETCH_CARD32(e->u.gravity.event) );
	output_card32( "window", FETCH_CARD32(e->u.gravity.window) );
	output_int16( "x", FETCH_INT16(e->u.gravity.x) );
	output_int16( "y", FETCH_INT16(e->u.gravity.y) );
	break;

case ResizeRequest		:
	output_packet( "X_Event (ResizeRequest)", "E", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(e->u.u.sequenceNumber) );
	output_card8( "detail", e->u.u.detail );
	output_card32( "window", FETCH_CARD32(e->u.resizeRequest.window) );
	output_int16( "width", FETCH_INT16(e->u.resizeRequest.width) );
	output_int16( "height", FETCH_INT16(e->u.resizeRequest.height) );
	break;

case CirculateNotify		:
	output_packet( "X_Event (CirculateNotify)", "E", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(e->u.u.sequenceNumber) );
	output_card8( "detail", e->u.u.detail );
	output_card32( "event", FETCH_CARD32(e->u.circulate.event) );
	output_card32( "window", FETCH_CARD32(e->u.circulate.window) );
	output_card32( "parent", FETCH_CARD32(e->u.circulate.parent) );
	output_card8( "place", e->u.circulate.place );
	break;

case CirculateRequest	:
	output_packet( "X_Event (CirculateRequest)", "E", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(e->u.u.sequenceNumber) );
	output_card8( "detail", e->u.u.detail );
	output_card32( "event", FETCH_CARD32(e->u.circulate.event) );
	output_card32( "window", FETCH_CARD32(e->u.circulate.window) );
	output_card32( "parent", FETCH_CARD32(e->u.circulate.parent) );
	output_card8( "place", e->u.circulate.place );
	break;

case PropertyNotify		:
	output_packet( "X_Event (PropertyNotify)", "E", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(e->u.u.sequenceNumber) );
	output_card8( "detail", e->u.u.detail );
	output_card32( "window", FETCH_CARD32(e->u.property.window) );
	output_card32( "atom", FETCH_CARD32(e->u.property.atom) );
	output_time( "time", FETCH_CARD32(e->u.property.time) );
	output_card8( "state", e->u.property.state );
	break;

case SelectionClear		:
	output_packet( "X_Event (SelectionClear)", "E", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(e->u.u.sequenceNumber) );
	output_card8( "detail", e->u.u.detail );
	output_card32( "window", FETCH_CARD32(e->u.selectionClear.window) );
	output_card32( "atom", FETCH_CARD32(e->u.selectionClear.atom) );
	output_time( "time", FETCH_CARD32(e->u.selectionClear.time) );
	break;

case SelectionRequest	:
	output_packet( "X_Event (SelectionRequeset)", "E", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(e->u.u.sequenceNumber) );
	output_card8( "detail", e->u.u.detail );
	output_card32( "requestor", FETCH_CARD32(e->u.selectionRequest.requestor) );
	output_card32( "selection", FETCH_CARD32(e->u.selectionRequest.selection) );
	output_card32( "target", FETCH_CARD32(e->u.selectionRequest.target) );
	output_card32( "property", FETCH_CARD32(e->u.selectionRequest.property) );
	output_time( "time", FETCH_CARD32(e->u.selectionRequest.time) );
	break;

case SelectionNotify		:
	output_packet( "X_Event (SelectionNotify)", "E", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(e->u.u.sequenceNumber) );
	output_card8( "detail", e->u.u.detail );
	output_card32( "requestor", FETCH_CARD32(e->u.selectionNotify.requestor) );
	output_card32( "selection", FETCH_CARD32(e->u.selectionNotify.selection) );
	output_card32( "target", FETCH_CARD32(e->u.selectionNotify.target) );
	output_card32( "property", FETCH_CARD32(e->u.selectionNotify.property) );
	output_time( "time", FETCH_CARD32(e->u.selectionRequest.time) );
	break;

case ColormapNotify		:
	output_packet( "X_Event (ColormapNotify)", "E", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(e->u.u.sequenceNumber) );
	output_card8( "detail", e->u.u.detail );
	output_card32( "window", FETCH_CARD32(e->u.colormap.window) );
	output_card32( "colormap", FETCH_CARD32(e->u.colormap.colormap) );
	output_bool( "new", e->u.colormap.new );
	output_card8( "state", e->u.colormap.state );
	break;

case ClientMessage		:
	output_packet( "X_Event (ClientMessage)", "E", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(e->u.u.sequenceNumber) );
	output_card8( "detail", e->u.u.detail );
	output_card32( "window", FETCH_CARD32(e->u.clientMessage.window) );
	output_card32( "atom", FETCH_CARD32(e->u.clientMessage.u.l.type) );
	output_listofcard32( "longword", 5, &e->u.clientMessage.u.l.longs0 );
	break;

case MappingNotify		:
	output_packet( "X_Event (MappingNotify)", "E", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(e->u.u.sequenceNumber) );
	output_card8( "detail", e->u.u.detail );
	output_card8( "firstKeyCode", e->u.mappingNotify.firstKeyCode );
	output_card8( "count", e->u.mappingNotify.count );
	break;

default		:
	output_packet( "X_Event (non-Core)", "E", 0 );
	output_card16_always( "sequenceNumber", FETCH_CARD16(e->u.u.sequenceNumber) );
	output_card8( "detail", e->u.u.detail );
	output_listofcard32( "longword", 7, &e->u.clientMessage.window );
	break;
    }

}


int
output_packet( str, prefix, seq )
char	*str;
char	*prefix;
int	seq;
{
    if ( !xw_form_brief) fprintf( out_fp, "\n" );
    fprintf( out_fp, "%s       [%s]", prefix, str);
    if ( seq ) fprintf( out_fp, " <SN: %d>", seq );
    fprintf( out_fp, "\n" );
}



int
output_time( prefix, time )
char	*prefix;
Time	time;
{
    if ( !xw_form_brief )
        fprintf( out_fp, "\t%20s: %d\n", prefix, time );

}


int
output_bool( prefix, bool )
char	*prefix;
Bool	bool;
{
    if ( !xw_form_brief ) {
        if ( bool == xTrue ) fprintf( out_fp, "\t%20s: Yes\n", prefix );
    	else fprintf( out_fp, "\t%20s: No\n", prefix );
    }
}



int
output_prop_mode( prefix, mode )
char	    *prefix;
int	    mode;
{
    if ( !xw_form_brief )
        switch (mode) {
case PropModeReplace         : fprintf( out_fp, "\t%20s: PropModeReplace\n", prefix ); break;
case PropModePrepend         : fprintf( out_fp, "\t%20s: PropModePrepend\n", prefix ); break;
case PropModeAppend          : fprintf( out_fp, "\t%20s: PropModeAppend\n", prefix ); break;
default			: fprintf( out_fp, "\t%20s: Unknown mode: %d\n", prefix, mode ); break;
        }
}



int
output_pixmap_class( prefix, class )
char	    *prefix;
int	    class;
{
    if ( !xw_form_brief )
        switch (class) {
case CursorShape:   fprintf( out_fp, "\t%20s: CursorShape\n", prefix ); break;
case TileShape:	    fprintf( out_fp, "\t%20s: TileShape\n", prefix ); break;
case StippleShape:  fprintf( out_fp, "\t%20s: StippleShape\n", prefix ); break;
default:	    fprintf( out_fp, "\t%20s: Unknown class: %d\n", prefix, class ); break;
        }
}       



int
output_image_format( prefix, class )
char	    *prefix;
int	    class;
{
    if ( !xw_form_brief )
        switch (class) {
case XYBitmap:   fprintf( out_fp, "\t%20s: XYBitmap\n", prefix ); break;
case XYPixmap:	    fprintf( out_fp, "\t%20s: XYPixmap\n", prefix ); break;
case ZPixmap:  fprintf( out_fp, "\t%20s: ZPixmap\n", prefix ); break;
default:	    fprintf( out_fp, "\t%20s: Unknown image format: %d\n", prefix, class ); break;
        }
}



int
output_coord_mode( prefix, mode )
char	    *prefix;
int	    mode;
{
    if ( !xw_form_brief )
        switch (mode) {
case CoordModeOrigin:   fprintf( out_fp, "\t%20s: CoordModeOrigin\n", prefix ); break;
case CoordModePrevious:	    fprintf( out_fp, "\t%20s: CoordModePrevious\n", prefix ); break;
default:	    fprintf( out_fp, "\t%20s: Unknown coordMode: %d\n", prefix, mode ); break;
        }
}



int
output_ordering( prefix, order )
char	    *prefix;
int	    order;
{
    if ( !xw_form_brief )
        switch (order) {
case Unsorted:   fprintf( out_fp, "\t%20s: Unsorted\n", prefix ); break;
case YSorted:	    fprintf( out_fp, "\t%20s: YSorted\n", prefix ); break;
case YXSorted:  fprintf( out_fp, "\t%20s: YXSorted\n", prefix ); break;
case YXBanded:  fprintf( out_fp, "\t%20s: YXBanded\n", prefix ); break;
default:	    fprintf( out_fp, "\t%20s: Unknown order: %d\n", prefix, order ); break;
        }
}



int
output_event_mode( prefix, mode )
char	    *prefix;
int	    mode;
{
    if ( !xw_form_brief )
        switch (mode) {
case AsyncPointer		: fprintf( out_fp, "\t%20s: AsyncPointer\n", prefix ); break;
case SyncPointer		: fprintf( out_fp, "\t%20s: SyncPointer\n", prefix ); break;
case ReplayPointer		: fprintf( out_fp, "\t%20s: ReplayPointer\n", prefix ); break;
case AsyncKeyboard		: fprintf( out_fp, "\t%20s: AsyncKeyboard\n", prefix ); break;
case SyncKeyboard		: fprintf( out_fp, "\t%20s: SyncKeyboard\n", prefix ); break;
case ReplayKeyboard		: fprintf( out_fp, "\t%20s: ReplayKeyboard\n", prefix ); break;
default				: fprintf( out_fp, "\t%20s: Unknown event mode: %d\n", prefix, mode ); break;
        }
}



int
output_grab_mode( prefix, mode )
char	    *prefix;
int	    mode;
{
    if ( !xw_form_brief )
        switch (mode) {

case GrabModeSync	: fprintf( out_fp, "\t%20s: GrabModeSync\n", prefix ); break;
case GrabModeAsync	: fprintf( out_fp, "\t%20s: GrabModeAsync\n", prefix ); break;
default			: fprintf( out_fp, "\t%20s: Unknown mode: %d\n", prefix, mode ); break;
        }
}



int
output_button( prefix, button )
char	    *prefix;
int	    button;
{
    if ( !xw_form_brief )
        switch (button) {
case AnyButton		: fprintf( out_fp, "\t%20s: AnyButton\n", prefix ); break;
case Button1		: fprintf( out_fp, "\t%20s: Button1\n", prefix ); break;
case Button2		: fprintf( out_fp, "\t%20s: Button2\n", prefix ); break;
case Button3		: fprintf( out_fp, "\t%20s: Button3\n", prefix ); break;
case Button4		: fprintf( out_fp, "\t%20s: Button4\n", prefix ); break;
case Button5		: fprintf( out_fp, "\t%20s: Button5\n", prefix ); break;
default			: fprintf( out_fp, "\t%20s: Unknown Button: %d\n", prefix, button ); break;
        }
}



int
output_class( prefix, class )
char	    *prefix;
int	    class;
{
    if ( !xw_form_brief )
        switch (class) {
case CopyFromParent		: fprintf( out_fp, "\t%20s: CopyFromParent\n", prefix ); break;
case InputOutput		: fprintf( out_fp, "\t%20s: InputOutput\n", prefix ); break;
case InputOnly		: fprintf( out_fp, "\t%20s: InputOnly\n", prefix ); break;
default			: fprintf( out_fp, "\t%20s: Unknown Window Class: %d\n", prefix, class ); break;
        }
}



int
output_visual_class( prefix, class )
char	    *prefix;
int	    class;
{
    if ( !xw_form_brief )
        switch (class) {
case StaticGray		: fprintf( out_fp, "\t%20s: StaticGray\n", prefix ); break;
case GrayScale		: fprintf( out_fp, "\t%20s: GrayScale\n", prefix ); break;
case StaticColor	: fprintf( out_fp, "\t%20s: StaticColor\n", prefix ); break;
case PseudoColor	: fprintf( out_fp, "\t%20s: PseudoColor\n", prefix ); break;
case TrueColor		: fprintf( out_fp, "\t%20s: TrueColor\n", prefix ); break;
case DirectColor	: fprintf( out_fp, "\t%20s: DirectColor\n", prefix ); break;
default			: fprintf( out_fp, "\t%20s: Unknown Visual Class: %d\n", prefix, class ); break;
        }
}



int
output_shape( prefix, shape )
char	    *prefix;
int	    shape;
{
    if ( !xw_form_brief )
        switch (shape) {
case Complex		: fprintf( out_fp, "\t%20s: Complex\n", prefix ); break;
case Nonconvex		: fprintf( out_fp, "\t%20s: Noncomplex\n", prefix ); break;
case Convex		: fprintf( out_fp, "\t%20s: Convex\n", prefix ); break;
default			: fprintf( out_fp, "\t%20s: Unknown shape: %d\n", prefix ); break;

        }
}



int
output_host_mode( prefix, mode )
char	    *prefix;
int	    mode;
{
    if ( !xw_form_brief )
        switch (mode) {
case HostInsert		: fprintf( out_fp, "\t%20s: HostInsert\n", prefix ); break;
case HostDelete		: fprintf( out_fp, "\t%20s: HostDelete\n", prefix ); break;
default			: fprintf( out_fp, "\t%20s: Unknown host mode: %d\n", prefix ); break;
        }
}



int
output_extension_request( req )
xReq	*req;
{
    output_packet( "X Extension Request", " ", pcs->sr_c_seq_number );
    output_card8( "reqType", req->reqType );
    output_card8( "data", req->data );
    output_listofcard32( "longword", (FETCH_CARD16(req->length) - 1), ((char *) req + sizeof( xReq ) ) );

}



int
output_change_mode( prefix, mode )
char	    *prefix;
int	    mode;
{
    if ( !xw_form_brief )
        switch (mode) {
case EnableAccess		: fprintf( out_fp, "\t%20s: EnableAccess\n", prefix ); break;
case DisableAccess		: fprintf( out_fp, "\t%20s: DisableAccess\n", prefix ); break;
default			: fprintf( out_fp, "\t%20s: Unknown change mode: %d\n", prefix, mode ); break;
        }
}



int
output_grab_status( prefix, status )
char	    *prefix;
int	    status;
{
    if ( !xw_form_brief )
        switch (status) {

case GrabSuccess		: fprintf( out_fp, "\t%20s: GrabSuccess\n", prefix ); break;
case AlreadyGrabbed		: fprintf( out_fp, "\t%20s: AlreadyGrabbed\n", prefix ); break;
case GrabInvalidTime		: fprintf( out_fp, "\t%20s: GrabInvalidTime\n", prefix ); break;
case GrabNotViewable		: fprintf( out_fp, "\t%20s: GrabNotViewable\n", prefix ); break;
case GrabFrozen		: fprintf( out_fp, "\t%20s: GrabFrozen\n", prefix ); break;
default			: fprintf( out_fp, "\t%20s: Unknown grab status: %d\n", prefix, status ); break;
        }
}



output_listoftimecoord( prefix, ntc, tc )
char	    *prefix;
int	    ntc;
xTimecoord  *tc;
{
    int	    i;

    if ( !xw_form_brief )
        for (i=0; i<ntc; i++ ) {
	    fprintf( out_fp, "\t%13s %6d: (time: %9u, x: %5u, y: %5u)\n", prefix, i,
		FETCH_CARD32(tc->time), FETCH_INT16(tc->x), FETCH_INT16(tc->y) );
	    tc++;
        }
}




int
output_listofrgb( prefix, nc, color )
char		*prefix;
int		nc;
unsigned short	*color;
{
    int	    i;
    unsigned short  c1, c2, c3;

    if ( !xw_form_brief )
        for (i=0; i<nc; i++ ) {
	    c1 = *color++;
	    c2 = *color++;
	    c3 = *color++;
	    fprintf( out_fp, "\t%13s %6d: (rgb: %5u, %5u, %5u)\n", prefix, i, FETCH_CARD16(c1), FETCH_CARD16(c2),
		FETCH_CARD16(c3) );
	}
}



int
output_listofcoloritem( prefix, nci, ci )
char	    *prefix;
int	    nci;
xColorItem  *ci;
{
    int	    i;

    if ( !xw_form_brief )
        for (i=0; i<nci; i++ ){
	    fprintf( out_fp, "\t%13s %6d: (pixel: 0x%9x, red: %5u, green: %5u, blue: %5u:, flags: %3u)\n", prefix, i,
	        FETCH_CARD32(ci->pixel), FETCH_CARD16(ci->red), FETCH_CARD16(ci->green),
		FETCH_CARD16(ci->blue), ci->flags );
	    ci++;
        }
}



int
output_listofstring8( prefix, nstr, str )
char	    *prefix;
int	    nstr;
char	    *str;
{
    int	    i, len;

    if ( !xw_form_brief )
        for ( i=0; i<nstr; i++ ) {
	    len = (unsigned char) *str++;
	    strncpy( string_buffer, str, len );
	    string_buffer[len] = '\0';
	    fprintf( out_fp, "\t%13s %6d: \"%*s\"\n", prefix, i, len, string_buffer );
	    str += len;
        }
}



int
output_listofrectangle( prefix, nrec, rect )
char	    *prefix;
int	    nrec;
xRectangle  *rect;
{
    int	    i;

    if ( !xw_form_brief )
        for (i=0; i<nrec; i++ ) {
	    fprintf( out_fp, "\t%13s %6d: (%6d,%6d)(+ %5u, + %5u)\n", 
		prefix, i, FETCH_INT16(rect->x), FETCH_INT16(rect->y), FETCH_CARD16(rect->width), FETCH_CARD16(rect->height) );
	    rect++;
         }
}



int
output_listofpoint( prefix, nrec, point )
char	    *prefix;
int	    nrec;
xPoint	    *point;
{
    int	    i;

    if ( !xw_form_brief )
        for (i=0; i<nrec; i++ ) {
	    fprintf( out_fp, "\t%13s %6d: (%6d,%6d)\n", prefix, i, FETCH_INT16(point->x), FETCH_INT16(point->y) );
	    point++;
        }
}




int
output_listofsegment( prefix, nrec, seg )
char	    *prefix;
int	    nrec;
xSegment    *seg;
{
    int	    i;

    if ( !xw_form_brief )
        for (i=0; i<nrec; i++ ) {
	    fprintf( out_fp, "\t%13s %6d: (%6d,%6d)(%6d,%6d)\n", prefix, i, FETCH_INT16(seg->x1), FETCH_INT16(seg->y1),
		FETCH_INT16(seg->x2), FETCH_INT16(seg->y2) );
	    seg++;
        }
}



int
output_listofarc( prefix, nrec, arc )
char	    *prefix;
int	    nrec;
xArc	    *arc;
{
    int	    i;

    if ( !xw_form_brief )
        for (i=0; i<nrec; i++ ) {
	    fprintf( out_fp, "\t%13s %6d: (%6d,%6d)(+ %5u, + %5u)(a1: %6d, a2: %6d)\n",
		prefix, i, FETCH_INT16(arc->x), FETCH_INT16(arc->y),
		FETCH_CARD16(arc->width), FETCH_CARD16(arc->height),
		FETCH_INT16(arc->angle1), FETCH_INT16(arc->angle2) );
	    arc++;
        }
}



int
output_listofkeysyms( prefix, nks, ks )
char	    *prefix;
int	    nks;
long	    *ks;
{
    output_listofcard32( prefix, nks, ks );
}



int
output_kbd_values( mask, xkc )
unsigned long	    mask;
unsigned long	    *xkc;
{
    int	    i;

    if ( mask & KBKeyClickPercent ) { i = *xkc++; output_int32( "keyClickPercent", FETCH_INT32( i ) ); }
    if ( mask & KBBellPercent ) { i = *xkc++; output_int32( "bellPercent", FETCH_INT32( i ) ); }
    if ( mask & KBBellDuration ) { i = *xkc++; output_int32( "bellDuration", FETCH_INT32( i ) ); }
    if ( mask & KBBellPitch ) { i = *xkc++; output_int32( "bellPitch", FETCH_INT32( i ) ); }
    if ( mask & KBLed ) { i = *xkc++; output_int32( "led", FETCH_INT32( i ) ); }
    if ( mask & KBLedMode ) { i = *xkc++; output_int32( "ledMode", FETCH_INT32( i ) ); }
    if ( mask & KBKey ) { i = *xkc++; output_int32( "key", FETCH_INT32( i ) ); }
    if ( mask & KBAutoRepeatMode ) { i = *xkc++; output_int32( "autoRepeatMode", FETCH_INT32( i ) ); }

}



int
output_window_changes( mask, xwc )
unsigned long	mask;
unsigned long	*xwc;
{
    int	    i;

    if ( mask & CWX ) { i = *xwc++; output_int32( "x", FETCH_INT32( i ) ); }
    if ( mask & CWY ) { i = *xwc++; output_int32( "y", FETCH_INT32( i ) ); }
    if ( mask & CWWidth ) { i = *xwc++; output_int32( "width", FETCH_INT32( i ) ); }
    if ( mask & CWHeight ) { i = *xwc++; output_int32( "height", FETCH_INT32( i ) ); }
    if ( mask & CWBorderWidth ) { i = *xwc++; output_int32( "borderWidth", FETCH_INT32( i ) ); }
    if ( mask & CWSibling ) { i = *xwc++; output_int32( "sibling", FETCH_INT32( i ) ); }
    if ( mask & CWStackMode ) { i = *xwc++; output_int32( "stackMode", FETCH_INT32( i ) ); }
}

               

int
output_window_attributes( mask, val )
unsigned long	    mask;
unsigned long	    *val;
{
    unsigned long   i;

    if ( mask & CWBackPixmap ) {
	i = *val++;
	output_card32( "backgroundPixmap", FETCH_CARD32( i ) );
    }
    if ( mask & CWBackPixel ) {
	i = *val++;
	output_card32( "backgroundPixel", FETCH_CARD32( i ) );
    }
    if ( mask & CWBorderPixmap ) {
	i = *val++;
	output_card32( "borderPixmap", FETCH_CARD32( i ) );
    }
    if ( mask & CWBorderPixel ) {
	i = *val++;
	output_card32( "borderPixel", FETCH_CARD32( i ) );
    }
    if ( mask & CWBitGravity ) {
	i = *val++;
	output_bit_gravity( "bitGravity", FETCH_CARD32( i ) );
    }
    if ( mask & CWWinGravity ) {
	i = *val++;
	output_win_gravity( "winGravity", FETCH_CARD32( i ) );
    }
    if ( mask & CWBackingStore ) {
	i = *val++;
	output_backing_store( "backingStore", FETCH_CARD32( i ) );
    }
    if ( mask & CWBackingPlanes ) {
	i = *val++;
	output_card32( "backingPlanes", FETCH_CARD32( i ) );
    }
    if ( mask & CWBackingPixel ) {
	i = *val++;
	output_card32( "backingPixel", FETCH_CARD32( i ) );
    }
    if ( mask & CWOverrideRedirect ) {
	i = *val++;
	output_bool( "overrideRedirect", FETCH_CARD32( i ) );
    }
    if ( mask & CWSaveUnder ) {
	i = *val++;
	output_bool( "saveUnders", FETCH_CARD32( i ) );
    }
    if ( mask & CWEventMask ) {
	i = *val++;
	output_card32( "eventMask", FETCH_CARD32( i ) );
    }
    if ( mask & CWDontPropagate ) {
	i = *val++;
	output_card32( "doNotPropagateMask", FETCH_CARD32( i ) );
    }
    if ( mask & CWColormap ) {
	i = *val++;
	output_card32( "colormap", FETCH_CARD32( i ) );
    }
    if ( mask & CWCursor ) {
	i = *val++;
	output_card32( "cursor", FETCH_CARD32( i ) );
    }

}



int
output_win_gravity( prefix, gravity )
char	    *prefix;
int	    gravity;
{
    if ( !xw_form_brief )
        switch (gravity) {
case UnmapGravity		: fprintf( out_fp, "\t%20s: UnmapGravity\n", prefix ); break;
case NorthWestGravity	: fprintf( out_fp, "\t%20s: NorthWestGravity\n", prefix ); break;
case NorthGravity		: fprintf( out_fp, "\t%20s: NorthGravity\n", prefix ); break;
case NorthEastGravity	: fprintf( out_fp, "\t%20s: NorthEastGravity\n", prefix ); break;
case WestGravity		: fprintf( out_fp, "\t%20s: WestGravity\n", prefix ); break;
case CenterGravity		: fprintf( out_fp, "\t%20s: CenterGravity\n", prefix ); break;
case EastGravity		: fprintf( out_fp, "\t%20s: EastGravity\n", prefix ); break;
case SouthWestGravity	: fprintf( out_fp, "\t%20s: SouthWestGravity\n", prefix ); break;
case SouthGravity		: fprintf( out_fp, "\t%20s: SouthGravity\n", prefix ); break;
case SouthEastGravity	: fprintf( out_fp, "\t%20s: SouthEastGravity\n", prefix ); break;
case StaticGravity		: fprintf( out_fp, "\t%20s: StaticGravity\n", prefix ); break;
default			: fprintf( out_fp, "\t%20s: Unknown window gravity: %d\n", prefix, gravity ); break;
        }

}


int
output_bit_gravity( prefix, gravity )
char	    *prefix;
int	    gravity;
{
    if ( !xw_form_brief )
        switch (gravity) {
case ForgetGravity		: fprintf( out_fp, "\t%20s: ForgetGravity\n", prefix ); break;
case NorthWestGravity	: fprintf( out_fp, "\t%20s: NorthWestGravity\n", prefix ); break;
case NorthGravity		: fprintf( out_fp, "\t%20s: NorthGravity\n", prefix ); break;
case NorthEastGravity	: fprintf( out_fp, "\t%20s: NorthEastGravity\n", prefix ); break;
case WestGravity		: fprintf( out_fp, "\t%20s: WestGravity\n", prefix ); break;
case CenterGravity		: fprintf( out_fp, "\t%20s: CenterGravity\n", prefix ); break;
case EastGravity		: fprintf( out_fp, "\t%20s: EastGravity\n", prefix ); break;
case SouthWestGravity	: fprintf( out_fp, "\t%20s: SouthWestGravity\n", prefix ); break;
case SouthGravity		: fprintf( out_fp, "\t%20s: SouthGravity\n", prefix ); break;
case SouthEastGravity	: fprintf( out_fp, "\t%20s: SouthEastGravity\n", prefix ); break;
case StaticGravity		: fprintf( out_fp, "\t%20s: StaticGravity\n", prefix ); break;
default			: fprintf( out_fp, "\t%20s: Unknown bit gravity: %d\n", prefix, gravity ); break;
        }

}



int
output_backing_store( prefix, store )
char	    *prefix;
int	    store;
{
    if ( !xw_form_brief )
        switch (store) {
case NotUseful               : fprintf( out_fp, "\t%20s: NotUseful\n", prefix ); break;
case WhenMapped              : fprintf( out_fp, "\t%20s: WhenMapped\n", prefix ); break;
case Always                  : fprintf( out_fp, "\t%20s: Always\n", prefix ); break;
default			: fprintf( out_fp, "\t%20s: Unknown backing store: %d\n", prefix, store ); break;
        }
}



int
output_gc_values( mask, gc )
unsigned long	    mask;
unsigned long	    *gc;
{
    unsigned long   val;

    if ( !xw_form_brief && mask & GCFunction ) {
	val = *gc++;
	switch (FETCH_CARD32(val)) {
case GXclear			: fprintf( out_fp, "\t%20s: GCclear\n", "gcFunction" ); break;
case GXand			: fprintf( out_fp, "\t%20s: GCand\n", "gcFunction" ); break;
case GXandReverse		: fprintf( out_fp, "\t%20s: GCandReverse\n", "gcFunction" ); break;
case GXcopy			: fprintf( out_fp, "\t%20s: GCcopy\n", "gcFunction" ); break;
case GXandInverted		: fprintf( out_fp, "\t%20s: GCandInverted\n", "gcFunction" ); break;
case GXnoop			: fprintf( out_fp, "\t%20s: GCnoop\n", "gcFunction" ); break;
case GXxor			: fprintf( out_fp, "\t%20s: GCxor\n", "gcFunction" ); break;
case GXor			: fprintf( out_fp, "\t%20s: GCor\n", "gcFunction" ); break;
case GXnor			: fprintf( out_fp, "\t%20s: GCnor\n", "gcFunction" ); break;
case GXequiv			: fprintf( out_fp, "\t%20s: GCequiv\n", "gcFunction" ); break;
case GXinvert		: fprintf( out_fp, "\t%20s: GCinvert\n", "gcFunction" ); break;
case GXorReverse		: fprintf( out_fp, "\t%20s: GCorReverse\n", "gcFunction" ); break;
case GXcopyInverted		: fprintf( out_fp, "\t%20s: GCcopyInverted\n", "gcFunction" ); break;
case GXorInverted		: fprintf( out_fp, "\t%20s: GCorInverted\n", "gcFunction" ); break;
case GXnand			: fprintf( out_fp, "\t%20s: GCnand\n", "gcFunction" ); break;
case GXset			: fprintf( out_fp, "\t%20s: GCset\n", "gcFunction" ); break;
default			: fprintf( out_fp, "\t%20s: Unknown function code: %d\n", "gcFunction", FETCH_CARD32(val) ); break;
	}
    }
    if ( mask & GCPlaneMask ) { val = *gc++; output_card32( "gcPlaneMask", FETCH_CARD32( val ) ); }
    if ( mask & GCForeground ) { val = *gc++; output_card32( "gcForegroundPixel", FETCH_CARD32( val ) ); }
    if ( mask & GCBackground ) { val = *gc++; output_card32( "gcBackgroundPixel", FETCH_CARD32( val ) ); }
    if ( mask & GCLineWidth ) { val = *gc++; output_int32( "gcLineWidth", FETCH_INT32( val ) ); }
    if ( !xw_form_brief && mask & GCLineStyle ) {
	val = *gc++;
	switch (FETCH_CARD32(val)) {
  case LineSolid:	    fprintf( out_fp, "\t%20s: LineSolid\n", "gcLineStyle" ); break;
  case LineOnOffDash:	    fprintf( out_fp, "\t%20s: LineOnOffDash\n", "gcLineStyle" ); break;
  case LineDoubleDash:	    fprintf( out_fp, "\t%20s: LineDoubleDash\n", "gcLineStyle" ); break;
  default:		    fprintf( out_fp, "\t%20s: Unknown line style: %d\n", "gcLineStyle", FETCH_CARD32(val) ); break;
	}
    }
    if ( !xw_form_brief && mask & GCCapStyle ) {
	val = *gc++;
	switch (FETCH_CARD32(val)) {
  case CapNotLast:	    fprintf( out_fp, "\t%20s: CapNotLast\n", "gcCapStyle" ); break;
  case CapButt:		    fprintf( out_fp, "\t%20s: CapButt\n", "gcCapStyle" ); break;
  case CapRound:	    fprintf( out_fp, "\t%20s: CapRound\n", "gcCapStyle" ); break;
  case CapProjecting:	    fprintf( out_fp, "\t%20s: CapProjecting\n", "gcCapStyle" ); break;
  default:		    fprintf( out_fp, "\t%20s: Unknown cap style: %d\n", "gcCapStyle", FETCH_CARD32(val) ); break;
	}
    }
    if ( !xw_form_brief && mask & GCJoinStyle ) {
	val = *gc++;
	switch (FETCH_CARD32(val)) {
  case JoinMiter:	    fprintf( out_fp, "\t%20s: JoinMiter\n", "gcJoinStyle" ); break;
  case JoinRound:	    fprintf( out_fp, "\t%20s: JoinRound\n", "gcJoinStyle" ); break;
  case JoinBevel:	    fprintf( out_fp, "\t%20s: JoinBevel\n", "gcJoinStyle" ); break;
  default:		    fprintf( out_fp, "\t%20s: Unknown join style: %d\n", "gcJoinStyle", FETCH_CARD32(val) ); break;
	}
    }
    if ( !xw_form_brief && mask & GCFillStyle ) {
	val = *gc++;
	switch (FETCH_CARD32(val)) {
  case FillSolid:	    fprintf( out_fp, "\t%20s: FillSolid\n", "gcFillStyle" ); break;
  case FillTiled:	    fprintf( out_fp, "\t%20s: FIllTiled\n", "gcFillStyle" ); break;
  case FillStippled:	    fprintf( out_fp, "\t%20s: FillStippled\n", "gcFillStyle" ); break;
  case FillOpaqueStippled:  fprintf( out_fp, "\t%20s: FillOpaqueStippled\n", "gcFillStyle" ); break;
  default:		    fprintf( out_fp, "\t%20s: Unknown fill style: %d\n", "gcFillStyle", FETCH_CARD32(val) ); break;
	}
    }
    if ( !xw_form_brief && mask & GCFillRule ) {
	val = *gc++;
	switch (FETCH_CARD32(val)) {
  case EvenOddRule:	    fprintf( out_fp, "\t%20s: EvenOddRule\n", "gcFillRule" ); break;
  case WindingRule:	    fprintf( out_fp, "\t%20s: WindingRule\n", "gcFillRule" ); break;
  default:		    fprintf( out_fp, "\t%20s: Unknown fill rule: %d\n", "gcFillRule", FETCH_CARD32(val) ); break;
	}
    }
    if ( mask & GCTile ) { val = *gc++; output_card32( "gcTile", FETCH_CARD32( val ) ); }
    if ( mask & GCStipple ) { val = *gc++; output_card32( "gcStipple", FETCH_CARD32( val ) ); }
    if ( mask & GCTileStipXOrigin ) { val = *gc++; output_int32( "gcTileStipXOrigin", FETCH_INT32( val ) ); }
    if ( mask & GCTileStipYOrigin ) { val = *gc++; output_int32( "gcTileStipYOrigin", FETCH_INT32( val ) ); }
    if ( mask & GCFont ) { val = *gc++; output_card32( "gcFont", FETCH_CARD32( val ) ); }
    if ( !xw_form_brief && mask & GCSubwindowMode ) {
	val = *gc++;
	switch (FETCH_CARD32(val)) {
  case ClipByChildren:	    fprintf( out_fp, "\t%20s: ClipByChildren\n", "gcSubwindowMode" ); break;
  case IncludeInferiors:    fprintf( out_fp, "\t%20s: IncludeInferiors\n", "gcSubwindowMode" ); break;
  default:		    fprintf( out_fp, "\t%20s: Unknown subwindow mode:%d\n", "gcSubwindowMode", FETCH_CARD32(val) ); break;
	}
    }
    if ( mask & GCGraphicsExposures ) { val = *gc++; output_bool( "gcGraphicsExposures", FETCH_CARD32( val ) ); }
    if ( mask & GCClipXOrigin ) { val = *gc++; output_int32( "gcClipXOrigin", FETCH_INT32( val ) ); }
    if ( mask & GCClipYOrigin ) { val = *gc++; output_int32( "gcClipYOrigin", FETCH_INT32( val ) ); }
    if ( mask & GCClipMask ) { val = *gc++; output_card32( "gcClipMask", FETCH_CARD32( val ) ); }
    if ( mask & GCDashOffset ) { val = *gc++; output_card32( "gcDashOffset", FETCH_CARD32( val ) ); }
    if ( mask & GCArcMode ) { val = *gc++; output_int32( "gcArcMode", FETCH_INT32( val ) ); }
    if ( mask & GCDashList ) { val = *gc++; output_card8( "gcDashes", FETCH_CARD32( val ) ); }
}



int
output_image( prefix, len, data, width )
char		*prefix;
int		len,
		width;
unsigned char	*data;
{
    int	    i, j, line, w;

    w = max( 8, min( 32, width ) );
    if ( !xw_form_brief )
        for ( i=0; i<len; i+=w ) {
	    line = min( i+w, len );
	    fprintf( out_fp, "\t%10s %6u-%6u: ", prefix, i, line-1 );
	    for ( j=i; j<line; j++ ) fprintf( out_fp, "%2x ", *data++ );
	    fprintf( out_fp, "\n" );
        }

}



int
output_modifiers( prefix, mods )
char	    *prefix;
int	    mods;
{
    output_card32( prefix, mods );
}



int
output_map_state( prefix, state )
char	    *prefix;
int	    state;
{
    if ( !xw_form_brief )
        switch (state) {
case IsUnmapped		: fprintf( out_fp, "\t%20s: IsUnmapped\n", prefix ); break;
case IsUnviewable		: fprintf( out_fp, "\t%20s: IsUnviewable\n", prefix ); break;
case IsViewable		: fprintf( out_fp, "\t%20s: IsViewable\n", prefix ); break;
default			: fprintf( out_fp, "\t%20s: Unknown map state: %d\n", prefix, state ); break;
        }
}



int
output_listoftextitem( prefix, itemSize, length, te )
char	    *prefix;
int	    length;
xTextElt    *te;
{

    unsigned char *pElt;
    unsigned char *pNextElt;
    unsigned char *endReq;
    int		numItems;
#define TextEltHeader 2
#define FontShiftSize 5

    if ( !xw_form_brief ) {
	pElt = (char *) te;
	endReq = pElt + length;

	numItems = 0;
	while ( ((int) (endReq - pElt)) > TextEltHeader) {
	    fprintf( out_fp, "\t%20s%d:\n", "Text Item #", numItems );
	    if (*pElt == FontChange) {
		Font	fid;

		fid = FETCH_CARD32( (*(unsigned long *)(pElt+1)) ); /* big-endian */

		output_card32( "fontChange", fid );
		pElt += FontShiftSize;
	    }
	    else if (*pElt == 0) {	/* delta only */
		xTextElt    *pT = pElt;

		output_int8( "deltaOnly", pT->delta );
		pElt += TextEltHeader;
	    }
	    else { /* it is a string */
		xTextElt    *pT = pElt;

		output_int8( "delta", pT->delta );
		pNextElt = pElt + TextEltHeader + (*pElt)*itemSize;
		if ( itemSize = 1 ) {
		    output_listofcard8( "string8", *pElt, (pElt+2) );
		}
		else {
		    output_listofcard16( "string16", *pElt, (pElt+2) );
		}
		pElt = pNextElt;
	    }
	    numItems++;
	} /* end while more items */

#undef TextEltHeader
#undef FontShiftSize
    }
}


int
output_data_header( dh )
int	dh[];
{
    int		status;
    unsigned long
		oldval,
		worktime[2],
		holdtime[2];
    char	asctime[] = "00:00:00.00";
    char	*msgtype;

    $DESCRIPTOR( asctime_desc, asctime) ;

    if ( xw_form_timestamp ) {
	if ( xw_form_deltatime )
	{
	    holdtime[0] = (unsigned long) dh[XL_TIMESTAMP];
	    holdtime[1] = (unsigned long) dh[XL_TIMESTAMP+1];
	    worktime[1] = pcs->sr_basetime[1] - holdtime[1];
	    oldval = worktime[0];
	    worktime[0] = pcs->sr_basetime[0] - holdtime[0];
	    if ( worktime[0] > oldval ) worktime[1]--;
	    VMSERR( (sys$asctim( 0, &asctime_desc, worktime, 1 ) ) );
	} else {
	    VMSERR( (sys$asctim( 0, &asctime_desc, &dh[XL_TIMESTAMP], 1) ) );
	}
    }
    switch( dh[XL_MSGTYPE] ) {
case XL_LINK_CONNECT:	msgtype = "Connect";	break;
case XL_LINK_ABORT:	msgtype = "Abort";	break;
case XL_SERVER_DATA:	msgtype = "Server";	break;
case XL_CLIENT_DATA:	msgtype = "Client";	break;
default:		msgtype = "Unknown";	break;
    }
    if ( !xw_form_brief ) fprintf( out_fp, "\n" );
    if ( xw_form_timestamp )
	fprintf( out_fp, "\nM   <%d><%s> [%s] <%d bytes>\n", dh[XL_SESSION], asctime, msgtype, dh[XL_BODYSIZE] );
    else
	fprintf( out_fp, "\nM   <%d>              [%s] <%d bytes>\n", dh[XL_SESSION], msgtype, dh[XL_BODYSIZE] );
}                                                      

