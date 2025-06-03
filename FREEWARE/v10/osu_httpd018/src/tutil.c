/*
 * Utility routines for DECthread servers.
 *
 * Revised:  31-aug-1994	Fixed bug in tu_read_raw.
 * Revised:  25-sep-1994	Remove dead delcarations of tcp server i/o.
 * Revised:  17-NOV-1994	Fixed bug in tu_read_raw.
 * Revised:  18-JAN-1995	Add tu_strstr function
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tutil.h"	/* validate prototype header file */
#define MIN_INT -2147483648		/* Minimum integer (2s comp.) */
#define MIN_INT_STR "-2147483648"
/**************************************************************************/
/* 
 * thread-reentrant versions of string routines 
 */
int tu_strlen(char *str ) 
{ int i = 0; while ( *str++ ) i++; return i; }

char *tu_strcpy ( char *str1, char *str2 ) 
{ char *p;
    p = str1;
    while ( (*p++ = *str2++) != '\0' );
    return str1;
}

char *tu_strncpy ( char *str1, char *str2, int limit ) 
{ char *p;
    if ( limit > 0 ) {
    p = str1;
    while ( (*p++ = *str2++) != '\0' ) if ( (--limit) <= 0 ) break;
    }
    return str1;
}
char *tu_strnzcpy ( char *str1, char *str2, int limit ) 
{ char *p;
   /* Assume str1 buffer is 1 larger than limit */
    p = str1;
    while ( (*p++ = *str2++) != '\0' ) 
	if ( (--limit) <= 0 ) { *p = '\0'; break; }
    return str1;
}

int tu_strncmp ( char *str1, char *str2, int maxl )
{ int i=0; char c1, c2;
    for ( i = 0; i < maxl; i++ ) {
	c1 = str1[i];
	if (c1 != str2[i]) return c1 < str2[i] ? -1 : 1;
	if (c1 == '\0') return  str2[i] ? 1: 0;	/* str2 is longer */
    }
    return 0;		/* equal to maxl chars. */
}

char *tu_strstr ( char *s1, char *s2 )
{
    char *cand;
    unsigned int i,j;

    for ( cand = s1; *cand; cand++ ) if ( *cand == *s2 ) {
	for ( i = 1; s2[i]; i++ ) {
	    if ( cand[i]=='\0' ) return (char *) 0;	/* too short */
	    if ( s2[i] != cand[i] ) break;
	}
	if ( s2[i] == '\0' ) return cand;	/* found match */
    }
    if ( !*s2 ) return s1;		/* Zero length input string */
    return (char *) 0;
}
/************************************************************************/
/* Copy string str2 to string str1, detecting and decoding 'escaped' values.
 * The limit argument must be at most 1 less that actual size of dest
 * array since a zero is always appended to terminate the final string.
 */
char *tu_unescape ( char *str1, char *str2, int limit ) 
{ char *p;
   /* Assume str1 buffer is 1 larger than limit */
    p = str1;
    while ( (*p = *str2++) != '\0' )  {
	if ( *p == '%' ) {
	    unsigned char h1, h2;
	    /* Get 2 hex digits following the %, don't advance past end of
	  	string */
	    h1 = *str2; if ( h1 ) str2++;
	    h2 = *str2; if ( h2 ) str2++;
	    if ( (h1 >= '0') && (h1 <= '9') ) h1 = h1 - '0';
	    else if ( (h1 >= 'a') && (h1 <= 'f') ) h1 = 10 + (h1-'a');
	    else if ( (h1 >= 'A') && (h1 <= 'F') ) h1 = 10 + (h1-'A');
	    else h1 = '\0';

	    if ( (h2 >= '0') && (h2 <= '9') ) h2 = h2 - '0';
	    else if ( (h2 >= 'a') && (h2 <= 'f') ) h2 = 10 + (h2-'a');
	    else if ( (h2 >= 'A') && (h2 <= 'F') ) h2 = 10 + (h2-'A');
	    else h2 = '\0';

	    *p = (h1<<4) | h2;
	}
	p++;
	if ( (--limit) <= 0 ) { *p = '\0'; break; }
    }
    return str1;
}
/************************************************************************/
/* 
 * Load data file into text array, separating records with CR/LF pairs. 
 */
int tu_load_file ( char *name, char *defname, int limit, char *txt, int *size )
{
    FILE *inp;
    int pos, length, recl, i, status;
    char *lp, dna[300], line[1000];

    /* Set default name argument (VMS extension to fopen) */
    if ( (defname[0] != '\0') && (tu_strlen(defname) < 256) ) {
	tu_strcpy ( dna, "dna=" );
	tu_strcpy ( &dna[4], defname );
    } else tu_strcpy ( dna, "dna=sys$disk:[].txt" );

    *size = 0;
#ifdef VMS
    inp = fopen ( name, "r", dna, "mbc=32" );
#else
    inp = fopen ( name, "r" );
#endif
    if ( !inp ) return 20;
    pos = 0;
    status = 1;
    while ( (recl=fread(line, sizeof(char), 1000, inp)) != 0 ) {
	for ( lp = line, i=0; i < recl; i++ ) {
	    if ( *lp == '\n' ) { 
		txt[pos++] = '\r'; 
		txt[pos++] = *lp++;
		*size = pos; 
	    }
	    else txt[pos++] = *lp++;
	    if  ( pos+2 >= limit ) { status = 2; break; }
	}
    }
    fclose ( inp );
    if ( *size+4 < limit ) {
	tu_strcpy ( &txt[*size], ".\r\n" ); *size += 3;
    }
    return status;
}

/***********************************************************************/
/*  Read_line is a utility routine for breaking the input stream into
 *  logical records delimited by carriage-return linefeed sequences.
 *  Since unix clients don't always follow the spec, break on naked linefeeds
 *  as well.
 *
 *  The line terminators are NOT included in the returned data, giving the
 *  caller the oportunity to append a consistent terminator if required.
 *  Buffer must be 1 larger that maxlen to accomodate the ending NUL.
 */
int tu_init_stream ( void *ctx, int getrtn(), tu_stream stream )
{
    stream->ctx = ctx;
    stream->getrtn = getrtn;
    stream->state = 0;
    stream->bufsize = 1000;
    stream->used = 0;
    stream->filled = 0;
    return 1;
}
    
int tu_read_line ( tu_stream stream, char *line, int maxlen, int *length )
{
    int state, used, filled, status, count;
    unsigned short iosb[4];
    char *buf, c;
    /*
     * Move stream context information to local variables.
     */
    state = stream->state;
    used = stream->used;
    filled = stream->filled;
    buf = stream->data;
    status = 1;
    /*
     * Load up to maxlen characters into output buffer, saving number
     * moved in variable count;
     */
    for ( count=0; count < maxlen; ) {
        /*
	 * Replenish input buffer if we've used all from previous read.
	 */
	if ( used >= filled ) {
	    int readsize = stream->bufsize;
	    if ( readsize > sizeof(stream->data) )
			readsize = sizeof(stream->data);
    	    status = (stream->getrtn) ( stream->ctx, buf, readsize, &filled );
	    if ( (status&1) == 0 ) break;
	    used = 0;
	}
	/*
	 * Take next character from input buffer
	 */
	c = buf[used++];
	if ( state == 0 ) {
	    if ( c == '\015' ) { state = 1; break; }	/* carriage return */
	    else if ( c == '\012' ) break;		/* Line-feed */

	    line[count++] = c;

	} else if ( state == 1 ) {
	    /* Everything but linefeeds following the CR are added to output */
	    if ( c != '\012' ) line[count++] = c;
	    if ( c != '\015' ) state = 0;
	}
    }
    /*
     * Save state in callers structure and return status.
     */
    line[count] = '\0';			/* Turn CR into '0' to terminate line */
    stream->state = state;
    stream->used = used;
    stream->filled = filled;
    *length = count;
    return status;
}

/***********************************************************************/
/*  read_raw returns any remaining data in the stream buffer or reads
 *  directly into the user's buffer.
 */
int tu_read_raw ( tu_stream stream, char *line, int maxlen, int *length )
{
    int state, used, filled, status, count;
    unsigned short iosb[4];
    char *buf, c;
    /*
     * Move stream context information to local variables.
     */
    state = stream->state;
    used = stream->used;
    filled = stream->filled;
    buf = stream->data;
    status = 1;
    if ( state == 1 ) {
	/*
	 * Flush pending linefeed from stream.
	 */
	state = 0;
	if ( used >= filled ) {
	    int readsize = stream->bufsize;
	    filled = used = 0;
	    if ( readsize > sizeof(stream->data) )
			readsize = sizeof(stream->data);
    	    status = (stream->getrtn) ( stream->ctx, buf, readsize, &filled );
	}
	if ((status&1) == 1) if (buf[used] == '\n' && (used < filled)) used++;
    }
    /*
     * either dump remaining characters from stream buffer into users buffer
     * or read new.
     */
    if ( used < filled ) {
	for ( count = 0; (count < maxlen) && (used < filled); count++) {
	    line[count] = buf[used++];
	}
	*length = count;
	status = 1;
    } else {
	int readsize, readret;
	/* readsize = (maxlen > 1000) ? 1000 : maxlen; */
    	status = (stream->getrtn) ( stream->ctx, line, maxlen, length );
    }
    /*
     * Save state in callers structure and return status.
     */
    stream->state = state;
    stream->used = used;
    stream->filled = filled;
    return status;
}
/****************************************************************************/
/*
 * Format int value as a string, storing in buffer.  Buffer needs to be
 * at least 12 long.
 */
char *tu_strint ( int value, char *buffer )
{
    int digit_stack[12], *dg;
    char *bptr;
    /*
     * Test for zero case (algorithm below results in null string on zero).
     * If not zero, normalize negative number (should test for MIN_INT).
     */
    bptr = buffer;
    if ( value == 0 ) {
	*bptr++ = '0'; *bptr = '\0';
	return buffer;
    } else if ( value < 0 ) {
	/* In 2's complement format, the negative of the min. integer can
	 * not be represented in the same number of bits, check for that case.
	 */
	if ( value == MIN_INT ) return tu_strcpy ( buffer, MIN_INT_STR );
	value = -value;
	*bptr++ = '-';
    }
    /*
     *  Build stack of successively higher order digits.
     */
    for ( dg = &digit_stack[0]; value > 0; value = value / 10 ) *dg++ = value;
    /*
     * pop stack and make lowest digit an ascii char in output buffer.
     */
    for ( *dg = 0; dg > &digit_stack[0]; ) {
	--dg;
	*bptr++ = (char) ((*dg-(dg[1]*10)) + 48);
    }
    *bptr = '\0';		/* terminate string */
    return buffer;
}
/****************************************************************************/
/* Copy string and upcase all characters. */
char *tu_strupcase ( char *dst, char *src )
{
    int i;
#ifdef VMS
    for ( i = 0; src[i]; i++ ) dst[i] = _toupper(src[i]);
#else
    for ( i = 0; src[i]; i++ ) 
	if ((src[i] >= 'a') && (src[i] <= 'z')) dst[i] = _toupper(src[i]);
	else dst[i] = src[i];
#endif
    dst[i] = '\0';
    return dst;
}
/****************************************************************************/
/* Copy string and lower case all characters. */
char *tu_strlowcase ( char *dst, char *src )
{
    int i;
#ifdef VMS
    for ( i = 0; src[i]; i++ ) dst[i] = _tolower(src[i]);
#else
    for ( i = 0; src[i]; i++ ) 
	if ((src[i] >= 'A') && (src[i] <= 'Z')) dst[i] = _tolower(src[i]);
	else dst[i] = src[i];
#endif
    dst[i] = '\0';
    return dst;
}
/****************************************************************************/
/* Append text to end of text buffer */
int tu_add_text ( tu_text buf, char *src, int maxlen )
{
    char *dst;
    int i, length;
    length = buf->l;
    if ( (length + maxlen) > buf->size ) maxlen = buf->size - length;
    dst = &buf->s[length];
    for ( i = maxlen; i > 0; --i ) {
	if ( (*dst++ = *src++) == '\0' ) break;
    }
    buf->l = length + (maxlen-i);
    if ( i == 0 ) {		/* maxlen chars moved */
	if ( buf->l < buf->size ) *dst = '\0';	/* terminate string if we can*/
	else return 0; 	/* buffer full */
    }
    return 1;
}
