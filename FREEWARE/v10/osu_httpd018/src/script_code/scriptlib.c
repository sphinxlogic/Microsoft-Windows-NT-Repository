/*
 * Utility routines for script programs.  These routines rationally handle
 * record mode access to the network logical link (zero length records are
 * valid, unlike in the C RTL).
 *
 *    int net_link_open ( )
 *    int net_link_close()
 *    int net_link_read ( char *buffer, int bufsize, int *read )
 *    int net_link_write ( char *buffer, int bufsize )
 *    int net_link_fetch ( char *tag, char *buffer, int bufsize, int *read )
 *    int net_link_set_rundown ( char *tag );
 *    int net_link_set_mode ( int mode );
 *    char * net_unescape_string ( char *arg, int *length )
 *    int net_link_printf ( char *ctl, ... )
 *
 * Date: 8-APR-1994
 * Revised: 29-APR-1994		Added net_unquote_arg() and net_link_printf()
 * Revised: 1-MAY-1994
 * Revised: 7-OCT-1994		Bug in unescape.
 */
#include <stdio.h>
#include <string.h>
#include <rms.h>		/* Symbol and structure definitions for RMS */
#include <stdarg>

#include "scriptlib.h"		/* Validate prototypes against actual */

#define LINK_LOGICAL "NET_LINK:"
#define IO_SIZE 1000
/*
 * Define module-side varaibles.
 */
static int net_link;			/* I/O channel to DECnet link */
static int bin_mode = 0;		/* If non-zero, do binary printfs */
static struct FAB net_fab;		/* RMS file access block */
static struct RAB net_rab;		/* RMS record access block */
static int printf_used;			/* printf buffer used */
static struct {
    void *fwd_link;			/* Next block in chain */
    int (*handler)();			/* exit handler */
    int arg_count;
    int *condition;			/* Exit reason */
    int reason;
    int link_open;				/* flag */
    char rundown_tag[512];		/* Exit string */
} exit_block;
static char printf_buffer[8192];
/**************************************************************************/
/* Call close function if link still open, this will flush the printf buffer.
 */
static int exit_handler ( int *reason )
{
    if ( !exit_block.link_open ) return 1;
    net_link_close();
    return 1;
}
/**************************************************************************/
/* Initialize network connection.  Make RMS open for PPF assigned to
 * net link logical name.  Return value is RMS compleition status.
 */
int net_link_open ( )
{
    int status, SYS$OPEN(), SYS$CONNECT(), SYS$DCLEXH();
    /*
     * Open network link as file.
     */
    net_fab = cc$rms_fab;	/* Set default values */
    net_fab.fab$l_fna = LINK_LOGICAL;
    net_fab.fab$b_fns = strlen(net_fab.fab$l_fna);
    net_fab.fab$b_fac = FAB$M_GET + FAB$M_PUT;
    net_fab.fab$b_rfm = FAB$C_VAR;
    printf_used = 0;

    status = SYS$OPEN ( &net_fab );
    if ( (status&1) == 0 ) return status;
    /*
     * Connect record stream to open file.
     */
    net_rab = cc$rms_rab;
    net_rab.rab$l_fab = &net_fab;
    net_rab.rab$b_rac = RAB$C_SEQ;
    status = SYS$CONNECT ( &net_rab );
    if ( (status&1) == 0 ) return status;
    /*
     * Establish exit handler to cleanly close link.
     */
    exit_block.handler = &exit_handler;
    exit_block.arg_count = 1;
    exit_block.condition = &exit_block.reason;
    exit_block.link_open = 1;
    exit_block.rundown_tag[0] = '\0';
    status = SYS$DCLEXH ( &exit_block );

    return status;
}
/**************************************************************************/
/*  Set value for mode flag:	0 - text mode (default), 1 - binary mode.
 *  In binary mode, printf does not exapnd linefeed to cr/lf pairs.
 *
 *  Return value is previous mode.
 */
int net_link_set_mode ( int new_mode )
{
    int prev_mode;
    prev_mode = bin_mode;
    bin_mode = new_mode;
    return prev_mode;
}
/**************************************************************************/
/*  Set string that close will automatically send at link close.
 *  If string is zero-length, no final message will be sent.
 */
int net_link_set_rundown ( char *tag )
{
    strncpy ( exit_block.rundown_tag, tag, sizeof(exit_block.rundown_tag)-1 );
    exit_block.rundown_tag[sizeof(exit_block.rundown_tag)-1] = '\0';
    return 1;
}
/**************************************************************************/
/*  Flush printf_buffer to network connection.
 */
static int net_link_flush ( )
{
    int status, SYS$PUT();

    if ( printf_used <= 0 ) return 1;
    net_rab.rab$w_rsz = printf_used;
    net_rab.rab$l_rbf = printf_buffer;
    printf_used = 0;
    status = SYS$PUT ( &net_rab );
    return status;
}
/**************************************************************************/
/* Close link openned by net_link_open.
 */
int net_link_close()
{
    int status, SYS$CLOSE ( );
    /*
     * Flush pending printfs and rundown tag if present.
     */
#ifdef DEBUG
printf("closing connection, printf buffer used: %d, rundown tag: '%s'\n", 
printf_used, exit_block.rundown_tag );
#endif
    if ( printf_used > 0 ) status = net_link_flush();
    if ( *exit_block.rundown_tag ) status = net_link_write
	(exit_block.rundown_tag, strlen(exit_block.rundown_tag) );
    exit_block.rundown_tag[0] = '\0';

    net_fab.fab$l_fop = 0;
    status = SYS$CLOSE ( &net_fab );
    exit_block.link_open = 0;
    return status;
}
/**************************************************************************/
/*  Read record from network connection.  Return value is VMS condition code.
 */
int net_link_read ( void *buffer, int bufsize, int *nread )
{
    int status, SYS$GET();

    if ( printf_used > 0 ) status = net_link_flush();
    net_rab.rab$l_ubf = buffer;
    net_rab.rab$w_usz = bufsize;
    status = SYS$GET ( &net_rab );
    *nread = net_rab.rab$w_rsz;		/* size of record read */
    return status;
}
/**************************************************************************/
/*  Write user's buffer to network connection.
 */
int net_link_write ( void *buffer, int bufsize )
{
    int status, SYS$PUT();

    if ( printf_used > 0 ) status = net_link_flush();
    net_rab.rab$w_rsz = bufsize;
    net_rab.rab$l_rbf = buffer;
    status = SYS$PUT ( &net_rab );
    return status;
}
/**************************************************************************/
/*  Write tag as prompt and read response into buffer.
 */
int net_link_query ( void *tag, void *buffer, int bufsize, int *nread )
{
    int status, tag_length;

    status = net_link_write ( tag, strlen ( tag ) );
    if ( (status&1) == 1 ) status = net_link_read ( buffer, bufsize, nread );

    return status;
}
/**************************************************************************/
/* Provide formatted output to net_link, replacing printf calls to stdout.
 * Newline chars in format string are replaced with carriage-return/linefeed
 * pairs.
 */
int net_link_printf ( char *fmt, ... )
{
    va_list alist;
    int state, out_len, status, j;
    char *fc, *conv, *out_buffer;
    char conv_buffer[4096], conv_spec[64], c_arg;
    /*
     * Setup pointer to variable argument list.
     */
    va_start ( alist, fmt );
    /*
     * Transfer characters from control string to output buffer.
     */
    out_len = printf_used;
    out_buffer = printf_buffer;
    for ( state = 0, fc = fmt; *fc; fc++ ) {
	if ( state == 0 ) {
	    /* Looking for next conversion character */
	    if ( *fc == '%' ) {
		/* Begin parse of conversion specification. */
		state = 1;
		conv_spec[0] = '%';
		conv = &conv_spec[1];
		out_buffer[out_len] = '\0';
	    } else {
		/* add CR to precede LF in out_buf */
		if ( *fc == '\n' ) 
		    if ( !bin_mode ) out_buffer[out_len++] = '\r';
		out_buffer[out_len++] = *fc;
	    }
	} else {
	    /*
	     * Append character to conversion string and see if it
	     * is terminal.
	     */
	    *conv++ = *fc;
	    if ( strchr ( "-0123456789.l", *fc ) == NULL ) {
		    long long_arg;
		    int int_arg;
		    char *char_arg;
		    double dbl_arg;
		*conv = '\0';		/* terminate specifier string */
		switch ( *fc ) {
		  case 'd':
		  case 'o':
		  case 'x':
		  case 'D':
		  case 'O':
		  case 'X':
		  case 'u':
		  case 'U':
		    if ( state == 1 ) {
			int_arg = va_arg ( alist, int );
			sprintf ( conv_buffer, conv_spec, int_arg );
		    } else {
			long_arg = va_arg ( alist, int );
			sprintf ( conv_buffer, conv_spec, long_arg );
		    }
		    break;
		  case 's':
		  case 'S':
		    char_arg = va_arg ( alist, char * );
		    sprintf ( conv_buffer, conv_spec, char_arg );
		    break;

		  case 'e':
		  case 'E':
		  case 'f':
		  case 'F':
		  case 'g':
		  case 'G':
		    dbl_arg = va_arg ( alist, double );
		    sprintf ( conv_buffer, conv_spec, dbl_arg );
		    break;

		  case 'c':
		  case 'C':
		    c_arg = va_arg ( alist, char );
		    sprintf ( conv_buffer, conv_spec, c_arg );
		    break;
		    
		  default:
		    /* Copy contents to output string */
		    strcpy ( conv_buffer, &conv_spec[1] );
		}
		/*
		 * Conversion complete, Copy chars to out_buffer, replacing
		 * '\n' with '\r\n';
		 */
		state = 0;
		for ( conv = conv_buffer; *conv; 
			out_buffer[out_len++] = *conv++ ) {
		    if ( *conv == '\n' ) {
			if ( !bin_mode ) out_buffer[out_len++] = '\r';
		    }
		}

	    } else if ( *fc == 'l' ) state = 2;	/* Long */
	}
	/*
	 * Flush buffer if full.
	 */
	while ( out_len >= IO_SIZE ) {
	    printf_used = IO_SIZE;
	    status = net_link_flush();
	    if ( (status&1) == 0 ) return status;
	    /* Transfer characters not moved to beginning of buffer */
	    for ( j = IO_SIZE; j < out_len; j++ ) {
		out_buffer[printf_used++] = out_buffer[j];
	    }
	    out_len = printf_used;
	}
    }
    va_end ( alist );
    /*
     * Flush conversion spec buffer if needed and send final output to
     * server.
     */
    printf_used = out_len;
    if ( state != 0 ) {
	/* Dangling conversion specification */
	status = net_link_flush();
	if ( (status&1) == 0 ) return status;
	*conv = '\0';
	strcpy ( out_buffer, &conv_spec[1] );
	printf_used = strlen ( out_buffer );
    }
    return 1;
}
/**************************************************************************/
/* Convert escaped characters in string to actual values.
 *
 * Arguments:
 * 	string		Character string.  Modified.
 *	length		Int.  On input, original length of string.
 *			On output, final length of unescaped string.
 */
char * net_unescape_string ( char *string, int *length )
{
    int i, j, reslen, modified;
    /*
     * Scan string.
     */
    for ( modified = reslen = i = 0; i < *length; i++ ) {
	if ( string[i] == '%' ) {
	    /*
	     * Escape seen, decode next 2 characters as hex and replace
	     * all three with single byte.
	     */
	    char value[4];
	    int val;
	    value[0] = string[i+1]; value[1] = string[i+2]; value[2] = '\0';
	    i += 2;
	    sscanf ( value, "%2x", &val );
	    if  ( val > 127 ) val |= (-1 ^ 255);	/* Sign extend */
	    string[reslen] = val;
	    modified = 1;
        } 
	else {
	    /* Only copy bytes if escape edit took place. */
	    if ( modified ) string[reslen] = string[i];
	}
        reslen++;
    }
    /* Return value is point to string editted. */
    *length = reslen;
    return string;
}
