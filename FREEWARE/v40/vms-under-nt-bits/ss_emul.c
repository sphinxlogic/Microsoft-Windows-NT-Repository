/*
 * Emulate VMS system services SYS$FAOL and SYS$NUMTIM (as SYS_FAOL and
 * SYS_NUMTIM)
 *
 * The following FAO directives are supported:
 *     !Sx !Ux !Zx !Ox !%S !%D !%T !AS !AZ !AC !AD !AF !n*c
 * 
 * Times are represented as pointers to time_t values rather than
 * pointers to VMS time structures.
 *
 * Revised:	1-NOV-1995	Fix bug in !XL processing.
 * Revised:	3-JAN-1995	Fix bug in NUMTIM emulation (month #).
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#ifndef VMS
typedef struct { int tm_sec, tm_min, tm_hour, tm_mday, tm_mon,
	tm_year, tm_wday, tm_yday, tm_isdst, tm_gmtoff;
	char *tm_zone; } tm_t2;
typedef struct tm tm_t;
#endif
#include "pthread_1c_np.h"
#define SS$_BUFFEROVF 1537
#define SS$_NORMAL 1
#define SS$_BADPARAM 20
#define MIN_INT 0x80000000
#define MIN_INT_STR "-2147483648"
#define MIN_INT_STR_L 11

#define LONG unsigned long
#define APPEND_C(c) if (j<ctx->outsize) ctx->out[j++]=c; else \
	{ ctx->outused=j; return SS$_BUFFEROVF; }

typedef struct { unsigned short l, class; char *a; } decsriptor, *string;
typedef struct {
    char *out;				/* Output buffer */
    LONG *prmlst;			/* Parameter list */
    int ctloffset;
    int outused;			/* Output position */
    int outsize;			/* Size of output buffer */
    int width_type;
    int width;
    int value;				/* last value formatted for !%s */
} control_buf;

static int output_directive ( int d1, int d2, control_buf *ctx );
static int fmt_signed ( int value, control_buf *ctx );
static int fmt_unsigned ( LONG value, int base, char fill, control_buf *ctx );
static int fmt_time  ( int date, time_t *value, control_buf *ctx );
static int fmt_pad ( char fill, int cur, control_buf *ctx );

int SYS_FAOL ( string ctlstr, int *outlen, string outbuf, LONG *prmlst )
{
    LONG uvalue;
    char *ctl, *out;
    control_buf ctx;
    int i, j, k, ctllen, outsize, length, repcnt, dirlen, *tmp;
    int status;
    char d1, d2;
    string pdx;
    /*
     * Setup for scanning string.
     */
    ctllen = ctlstr->l;
    ctl = ctlstr->a;
    ctx.outsize = outsize = outbuf->l;
    ctx.out = out = outbuf->a;
    ctx.prmlst = prmlst;
    ctx.value = 0;
    /*
     * Main loop, i is position in control string, j = output point.
     */
    j = i = 0;
    while ( i < ctllen ) {
	/*
	 * Copy characters until a directive encountered.
	 */
	while ( ctl[i] != '!' ) {
	    if ( j < outsize ) {
		out[j++] = ctl[i++];
		if ( i >= ctllen ) {
		    *outlen = j;
		    return SS$_NORMAL;		/* All done */
		}
	    } else {
		*outlen = outsize;
		return SS$_BUFFEROVF;		/* Output buffer full */
	    }
	}
	/*
	 * Parse directive, first parse field length/repeat count
	 */
	ctx.width = ctx.width_type = 0;
	repcnt = 0;
	for ( i++; i < ctllen; i++ ) {
	    if ( ctl[i] == '#' ) {
		ctx.width_type = 2;
		break;
	    } else if ( (ctl[i] >= '0') && (ctl[i] <= '9') ) {
		ctx.width_type = 1;
		ctx.width = (ctx.width * 10) + (int) ctl[i] - (int) '0';
		if ( ctx.width > 0x0ffff ) return SS$_BADPARAM+2;
	    } else if ( ctl[i] == '(' ) {
		/* Width field is really repeat count */
		repcnt = ctx.width;
		ctx.width = 0;
		for ( i++; i < ctllen; i++ ) {
		    if ( ctl[i] == ')' ) break;
		    if ( ctl[i] == '#' ) {
			ctx.width_type = 2;
			break;
		    } else if ( (ctl[i] >= '0') && (ctl[i] <= '9') ) {
			ctx.width_type = 1;
			ctx.width = (ctx.width * 10) + (int) ctl[i] - (int) '0';
			if ( ctx.width > 0x0ffff ) return SS$_BADPARAM+4;
		    }
		}
	    } else break;	/* end of numerics */
	}
	if ( i >= ctllen ) return SS$_BADPARAM+6;
	if ( repcnt == 0 ) repcnt = 1; else if (repcnt < 0) repcnt = *ctx.prmlst++;
	if ( ctx.width_type == 2 ) ctx.width = *ctx.prmlst++;
	/*
	 * Interpret directive.
	 */
	d1 = ctl[i++];
	if ( i < ctllen ) { d2 = ctl[i]; } else d2 = '\0';
	ctx.outused = j;
	for ( k = 0; k < repcnt; k++ ) {
	    status = output_directive ( d1, d2, &ctx );
	    if ( status != 1 ) { *outlen = ctx.outused; return status; }
	}
	j = ctx.outused;
	i += ctx.ctloffset;
    }
    *outlen = j;
    return SS$_NORMAL;
}
/************************************************************************/
/* Process single directive. 
 */
static int output_directive ( int d1, int d2, control_buf *ctx )
{
    int j, value, srclen, status;
    LONG uvalue;
    char *src, *out;

    j = ctx->outused;
    ctx->ctloffset = 1;
    /* printf("output directive d1: %d(%c), d2: %d(%c)\n", d1, d1, d2, d2 ); */
    switch ( d1 ) {
	case 'A':
	{	/* Ascii output */
	    int srclen; char *src;
 	    if ( d2 == 'Z' ) {
		for ( src = (char *) *ctx->prmlst++; *src; src++ ) {
		    APPEND_C ( *src );
		}
		break;
	    } else if ( d2 == 'D' ) {
		srclen = *ctx->prmlst++;
		src = (char *) *ctx->prmlst++;
	    } else if ( d2 == 'C' ) {
		src = (char *) *ctx->prmlst++;
		srclen = (unsigned char) *src++;
	    } else if ( d2 == 'S' ) {
		string desc;
		desc = (string) *ctx->prmlst++;
		srclen = desc->l; src = desc->a;
	    } else if ( d2 == 'F' ) {
		srclen = *ctx->prmlst++;
	       for ( src = (char *) *ctx->prmlst++; srclen > 0; srclen-- ) {
		   if ( isprint(*src) ) { APPEND_C(*src);
		   } else { APPEND_C('.'); }
		   src++;
		}
		break;
	    } else { return SS$_BADPARAM+8; }
	    if ( srclen > (ctx->outsize-j) ) { 
	        for ( ; srclen > 0; srclen-- ) { APPEND_C(*src); *src++; }
	    } else {
		for ( out = ctx->out; srclen > 0; srclen-- ) out[j++] = *src++;
	    }
        } break;
	case 'S':
	{	/* Blank filled signed decimal */
	    value = (int) *ctx->prmlst++;
	    if ( d2 == 'L' ) {
	    } else if ( d2 == 'B' ) {
		value = (value & 255); if ( value > 127 ) value |= 0xffffff00;
	    } else if ( d2 == 'W' ) {
		value = (value & 0x0ffff); 
		if ( value > 0x7fff ) value |= 0xffff0000;
	    } else { return SS$_BADPARAM+10; }
	    ctx->value = value;
	    status = fmt_signed ( value, ctx );
	    return status;
	}
	case 'U':
	case 'Z':
	case 'O':
	case 'X':
	{
	    int base;
	    char fill;
	    uvalue = *ctx->prmlst++;
	    base = 10;
	    fill = '0';
	    if ( d1 == 'U' ) fill = ' ';
	    else if  ( d1 == 'X' ) base = 16;
	    else if ( d1 == 'O' ) base = 8;

	    if ( d2 == 'L' ) {
	    } else if ( d2 == 'B' ) {
		uvalue = (uvalue &255);
	    } else if ( d2 == 'W' ) {
		uvalue = (uvalue & 0x0ffff); 
	    } else { return SS$_BADPARAM+12; }
	    ctx->value = (uvalue == 1) ? 1 : 0;
	    status = fmt_unsigned ( uvalue, base, fill, ctx );
	    return status;
        }
	case '/':	/* newline */
	    ctx->ctloffset = 0;
	    APPEND_C ( '\r' )
	    APPEND_C ( '\n' )
	    break;
        case '_':	/* tab */
	    ctx->ctloffset = 0;
	    APPEND_C ( '\t' );
	    break;
	case '^': 	/* form feed */
	    ctx->ctloffset = 0;
	    APPEND_C ( '\014' );
	    break;
	case '!':	/* exclamation mark */
	    ctx->ctloffset = 0;
	    APPEND_C ( '!' );
	    break;
	case '%':	/* system call, date-time */
	{	    
	    char *src; int srclen;
	    if ( d2 == 'D' || d2 == 'T' ) {
	        status = fmt_time ( (d2=='D') ? 1 : 0,
			(time_t *) *ctx->prmlst++, ctx );
	        return status;
	    } else if ( d2 == 'S' ) {
		if ( (j > 0) && (ctx->value != 1) ) {
		    char prev;
		    prev = ctx->out[j-1];
		    if ( _tolower(prev) == prev ) {APPEND_C('s');}
		    else { APPEND_C('S'); }
		}
	    } else {
		return SS$_BADPARAM+14;
	    }
	} break;
	case '*': 	/* Repeat character */
	    return fmt_pad ( (char) d2, j, ctx );
	default:
	    return SS$_BADPARAM+16;
    }
    if ( ctx->width_type ) return fmt_pad ( ' ', j, ctx );
    else ctx->outused = j;
    return SS$_NORMAL;
}
/************************************************************************/
static int fmt_pad ( char fill, int cur, control_buf *ctx )
{
   char * out;
   int j, limit;
   if ( cur >= 0 ) {
       limit = ctx->width + ctx->outused;
       if ( limit < cur ) 
	   { cur = ctx->outused; fill = '*'; ctx->width_type =  (-1); }
   } else {
	/* Right justify, fil field to (-cur) spaces */
	limit = ctx->width + ctx->outused + cur;
        if ( limit < ctx->outused ) 
	    { limit = limit-cur; fill = '*'; ctx->width_type = (-1); }
	cur = ctx->outused;
   }

   if ( limit <= ctx->outsize ) {
	out = ctx->out;
	ctx->outused = limit;
	for ( j = cur; j < limit; ) out[j++] = fill;
	return SS$_NORMAL;
   } else {
	ctx->outused = ctx->outsize;
	for ( j = cur; j < ctx->outused; ) out[j++] = fill;
	return SS$_BUFFEROVF;
   }
}
/************************************************************************/
static int fmt_signed ( int value, control_buf *ctx )
{
    int j, length, status, negative, digit_stack[12], *dg;
    /*
     * Test for zero case (algorithm below results in null string on zero).
     * If not zero, normalize negative number (should test for MIN_INT).
     */
    j = ctx->outused;
    if ( value == 0 ) {
	if ( ctx->width_type ) {
	    status = fmt_pad ( ' ', -1, ctx );
	    if ( status != 1 ) return status; else j = ctx->outused;
	    if ( ctx->width_type == -1 ) return SS$_NORMAL;
	}
	APPEND_C ( '0' );
	ctx->outused = j;
	return 1;

    } else if ( value < 0 ) {
	/* In 2's complement format, the negative of the min. integer can
	 * not be represented in the same number of bits, check for that case.
	 */
	if ( value == MIN_INT ) {
	    char *str;
	    if ( ctx->width_type ) {
		status = fmt_pad ( ' ', -MIN_INT_STR_L, ctx );
		if ( status != 1 ) return status; else j = ctx->outused;
		if ( ctx->width_type == -1 ) return SS$_NORMAL;
	    }
	    for ( str = MIN_INT_STR; *str; str++ ) {
		APPEND_C ( *str );
	    }
	    ctx->outused = j;
	    return 1;
	}
	negative = 1;
	value = -value;
    }
    else negative = 0;
    /*
     *  Build stack of successively higher order digits.
     */
    for (length=0; value > 0; value = value/10) digit_stack[length++] = value;
    if ( ctx->width_type ) { 
	status = fmt_pad ( ' ', -(length+negative), ctx );
	if ( status != 1 ) return status; else j = ctx->outused;
	if ( ctx->width_type == -1 ) return SS$_NORMAL;
    }
    /*
     * pop stack and make lowest digit an ascii char in output buffer.
     */
    if ( negative ) APPEND_C('-');
    for ( digit_stack[length] = 0; length > 0; length-- ) {
	APPEND_C( (char) ((digit_stack[length-1]-(digit_stack[length]*10)) + 48) );
    }
    ctx->outused = j;
    return 1;
}
/************************************************************************/
static int fmt_unsigned ( LONG value, int base, char fill, control_buf *ctx )
{
    int j, status, digit_stack[12], length;
    char *ascdigit = "0123456789ABCDEF";
    /*
     * Test for zero case (algorithm below results in null string on zero).
     */
    j = ctx->outused;
    if ( value == 0 ) {
	if ( ctx->width_type ) {
	    status = fmt_pad ( fill, -1, ctx );
	    if ( status != 1 ) return status; else j = ctx->outused;
	    if ( ctx->width_type == -1 ) return SS$_NORMAL;
	}
	APPEND_C ( '0' );
	ctx->outused = j;
	return 1;
    }
    /*
     *  Build stack of successively higher order digits.
     */
    for (length=0; value > 0; value = value/base) digit_stack[length++] = value;
    if ( ctx->width_type ) { 
	status = fmt_pad ( fill, -(length), ctx );
	if ( status != 1 ) return status; else j = ctx->outused;
	if ( ctx->width_type == -1 ) return SS$_NORMAL;
    }
    /*
     * pop stack and make lowest digit an ascii char in output buffer.
     */
    for ( digit_stack[length] = 0; length > 0; length-- ) {
	int i = digit_stack[length-1]-(digit_stack[length]*base);
	APPEND_C ( ascdigit[i] );
	/* APPEND_C( (char) 
		((digit_stack[length-1]-(digit_stack[length]*base)) + 48) ); */
    }
    ctx->outused = j;
    return 1;
}
/************************************************************************/
static int fmt_time ( int date_flag, time_t *value, control_buf *ctx )
/* char *out, int j, int outsize, int *outlen ) */
{
    int i, j, status;
    char *out, buf[32];
    tm_t *local_time;
    time_t now;
    /*
     * Test for zero case (algorithm below results in null string on zero).
     */
    j = ctx->outused;
    pthread_lock_global_np();
    if ( !value ) {
	value = &now;
	time ( value );
    }
    local_time = localtime ( value );
    strcpy ( buf, asctime ( local_time ) );
    pthread_unlock_global_np();
    /*
     *  Copy to output.
     */
    i = ( date_flag ) ? 0 : 11;
    if ( j + 26 > ctx->outsize ) {
	for ( ; buf[i] && (buf[i]!='\n'); i++ ) { APPEND_C(out[j]); j++; }
    } else {
	out = ctx->out;
	for ( ; buf[i] && (buf[i]!='\n'); i++ ) out[j++] = buf[i];
    }
    if ( ctx->width_type ) return fmt_pad ( ' ', j, ctx );
    ctx->outused = j;
    return SS$_NORMAL;
}
/*****************************************************************************/
int SYS_NUMTIM ( short int *num, time_t *value )
{
    tm_t *local_time;
    time_t now;
    /*
     * Test for zero case and use current time.
     */
    pthread_lock_global_np();
    if ( !value ) {
	value = &now;
	time ( value );
    }
    local_time = localtime ( value );
    num[0] = local_time->tm_year;
    num[1] = local_time->tm_mon + 1;
    num[2] = local_time->tm_mday;
    num[3] = local_time->tm_hour;
    num[4] = local_time->tm_min;
    num[5] = local_time->tm_sec;
    num[6] = 0;		/* fraction of second not available */
    pthread_unlock_global_np();
    if ( num[0] < 100 ) num[0] += (num[0] > 50) ? 1900 : 2000;
    return 1;
}
