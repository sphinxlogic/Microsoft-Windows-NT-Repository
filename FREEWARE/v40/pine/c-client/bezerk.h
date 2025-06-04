/*
 * Program:	Berkeley mail routines
 *
 * Author:	Mark Crispin
 *		Networks and Distributed Computing
 *		Computing & Communications
 *		University of Washington
 *		Administration Building, AG-44
 *		Seattle, WA  98195
 *		Internet: MRC@CAC.Washington.EDU
 *
 * Date:	20 December 1989
 * Last Edited:	21 September 1993
 *
 * Copyright 1993 by the University of Washington
 *
 *  Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appears in all copies and that both the
 * above copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the University of Washington not be
 * used in advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  This software is made
 * available "as is", and
 * THE UNIVERSITY OF WASHINGTON DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED,
 * WITH REGARD TO THIS SOFTWARE, INCLUDING WITHOUT LIMITATION ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, AND IN
 * NO EVENT SHALL THE UNIVERSITY OF WASHINGTON BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, TORT
 * (INCLUDING NEGLIGENCE) OR STRICT LIABILITY, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */


/* Dedication:
 *  This file is dedicated with affection to those Merry Marvels of Musical
 * Madness . . .
 *  ->  The Incomparable Leland Stanford Junior University Marching Band  <-
 * who entertain, awaken, and outrage Stanford fans in the fact of repeated
 * losing seasons and shattered Rose Bowl dreams [Cardinal just don't have
 * HUSKY FEVER!!!].
 *
 */

/* Validate line known to start with ``F''
 * Accepts: pointer to candidate string to validate as a From header
 *	    return pointer to end of date/time field
 *	    return pointer to offset from t of time (hours of ``mmm dd hh:mm'')
 *	    return pointer to offset from t of time zone (if non-zero)
 * Returns: T if valid From string, t,ti,zn set; else NIL
 */

#define VALID(s,x,ti,zn) \
  (s[1] == 'r') && (s[2] == 'o') && (s[3] == 'm') && (s[4] == ' ') && \
  (x = strchr (s+5,'\n')) && \
  ((x-s < 41) || ((ti = ((x[-2] == ' ') ? -14 : (x[-3] == ' ') ? -15 : \
			 (x[-4] == ' ') ? -16 : (x[-5] == ' ') ? -17 : \
			 (x[-6] == ' ') ? -18 : (x[-7] == ' ') ? -19 : \
			 (x[-8] == ' ') ? -20 : (x[-9] == ' ') ? -21 : \
			 (x[-10]== ' ') ? -22 : (x[-11]== ' ') ? -23 : 0)) && \
		  (x[ti]   == ' ') && (x[ti+1] == 'r') && (x[ti+2] == 'e') && \
		  (x[ti+3] == 'm') && (x[ti+4] == 'o') && (x[ti+5] == 't') && \
		  (x[ti+6] == 'e') && (x[ti+7] == ' ') && (x[ti+8] == 'f') && \
		  (x[ti+9] == 'r') && (x[ti+10]== 'o') && (x[ti+11]== 'm') && \
		  (x += ti)) || T) && \
  (x-s >= 27) && \
  ((x[ti = -5] == ' ') ? ((x[-8] == ':') ? !(zn = 0) : \
			  ((x[ti = zn = -9] == ' ') || \
			   ((x[ti = zn = -11] == ' ') && \
			    ((x[-10] == '+') || (x[-10] == '-'))))) : \
   ((x[zn = -4] == ' ') ? (x[ti = -9] == ' ') : \
    ((x[zn = -6] == ' ') && ((x[-5] == '+') || (x[-5] == '-')) && \
     (x[ti = -11] == ' ')))) && \
  (x[ti - 3] == ':') && (x[ti -= ((x[ti - 6] == ':') ? 9 : 6)] == ' ') && \
  (x[ti - 3] == ' ') && (x[ti - 7] == ' ') && (x[ti - 11] == ' ')


/* You are not expected to understand this macro, but read the next page if
 * you are not faint of heart.
 *
 * Known formats to the VALID macro are:
 * 		From user Wed Dec  2 05:53 1992
 * BSD		From user Wed Dec  2 05:53:22 1992
 * SysV		From user Wed Dec  2 05:53 PST 1992
 * rn		From user Wed Dec  2 05:53:22 PST 1992
 *		From user Wed Dec  2 05:53 -0700 1992
 *		From user Wed Dec  2 05:53:22 -0700 1992
 *		From user Wed Dec  2 05:53 1992 PST
 *		From user Wed Dec  2 05:53:22 1992 PST
 *		From user Wed Dec  2 05:53 1992 -0700
 * Solaris	From user Wed Dec  2 05:53:22 1992 -0700
 *
 * Plus all of the above with `` remote from xxx'' after it. Thank you very
 * much, smail and Solaris, for making my life considerably more complicated.
 */

/*
 * What?  You want to understand the VALID macro anyway?  Alright, since you
 * insist.  Actually, it isn't really all that difficult, provided that you
 * take it step by step.
 *
 * Line 1	Validates that the 2-5th characters are ``rom ''.
 * Line 2	Sets x to point to the end of the line.
 * Lines 3-12	First checks to see if the line is at least 41 characters long.
 *		If so, it scans backwards up to 10 characters (the UUCP system
 *		name length limit due to old versions of UNIX) to find a space.
 *		If one is found, it backs up 12 characters from that point, and
 *		sees if the string from there is `` remote from''.  If so, it
 *		sets x to that position.  The ``|| T'' is there so the parse
 *		will still continue.
 * Line 13	Makes sure that there are at least 27 characters in the line.
 * Lines 14-17	Checks if the date/time ends with the year.  If so, It sees if
 *		there is a colon 3 characters further back; this would mean
 *		that there is no timezone field and zn is set to 0 and ti is
 *		left in front of the year.  If not, then it expects there to
 *		either to be a space four characters back for a three-letter
 *		timezone, or a space six characters back followed by a + or -
 *		for a numeric timezone.  If a timezone is found, both zn and
 *		ti are the offset of the space immediately before it.
 * Lines 18-20	Are the failure case for a date/time not ending with a year in
 *		line 14.  If there is a space four characters back, it is a
 *		three-letter timezone; there must be a space for the year nine
 *		characters back.  Otherwise, there must be a space six
 *		characters back and a + or - five characters back to indicate a
 *		numeric timezone and a space eleven characters back to indicate
 *		a year.  zn and ti are set appropriately.
 * Line 21	Make sure that the string before ti is of the form hh:mm or
 *		hh:mm:ss.  There must be a colon three characters back, and a
 *		space six or nine characters back (depending upon whether or
 *		not the character six characters back is a colon).  ti is set
 *		to be the offset of the space immediately before the time.
 * Line 22	Make sure the string before ti is of the form www mmm dd.
 *		There must be a space three characters back (in front of the
 *		day), one seven characters back (in front of the month), and
 *		one eleven characters back (in front of the day of week).
 *
 * Why a macro?  It gets invoked a *lot* in a tight loop.  On some of the
 * newer pipelined machines it is faster being open-coded than it would be if
 * subroutines are called.
 *
 * Why does it scan backwards from the end of the line, instead of doing the
 * much easier forward scan?  There is no deterministic way to parse the
 * ``user'' field, because it may contain unquoted spaces!  Yes, I tested it to
 * see if unquoted spaces were possible.  They are, and I've encountered enough
 * evil mail to be totally unwilling to trust that ``it will never happen''.
 */

/* Build parameters */

#define KODRETRY 15		/* kiss-of-death retry in seconds */
#define LOCKTIMEOUT 5		/* lock timeout in minutes */
#define CHUNK 8192		/* read-in chunk size */


/* Command bits from bezerk_getflags() */

#define fSEEN 1
#define fDELETED 2
#define fFLAGGED 4
#define fANSWERED 8


/* Status string */

#define STATUS "Status: RO\nX-Status: DFA\n\n"

/* BEZERK per-message cache information */

typedef struct file_cache {
  char *header;			/* pointer to RFC 822 header */
  unsigned long headersize;	/* size of RFC 822 header */
  char *body;			/* pointer to message body */
  unsigned long bodysize;	/* size of message body */
  char status[sizeof (STATUS)];	/* status flags */
  char internal[1];		/* start of internal header and data */
} FILECACHE;


/* BEZERK I/O stream local data */

typedef struct bezerk_local {
  unsigned int dirty : 1;	/* disk copy needs updating */
  int ld;			/* lock file descriptor */
  char *name;			/* local file name for recycle case */
  char *lname;			/* lock file name */
  off_t filesize;		/* file size parsed */
  time_t filetime;		/* last file time */
  unsigned long cachesize;	/* size of local cache */
  FILECACHE **msgs;		/* pointers to message-specific information */
  char *buf;			/* temporary buffer */
  unsigned long buflen;		/* current size of temporary buffer */
} BEZERKLOCAL;


/* Convenient access to local data */

#define LOCAL ((BEZERKLOCAL *) stream->local)

/* Function prototypes */

DRIVER *bezerk_valid  ();
int bezerk_isvalid  ();
void *bezerk_parameters  ();
void bezerk_find  ();
void bezerk_find_bboards  ();
void bezerk_find_all  ();
void bezerk_find_all_bboards  ();
long bezerk_subscribe  ();
long bezerk_unsubscribe  ();
long bezerk_subscribe_bboard  ();
long bezerk_unsubscribe_bboard  ();
long bezerk_create  ();
long bezerk_delete  ();
long bezerk_rename  ();
MAILSTREAM *bezerk_open  ();
void bezerk_close  ();
void bezerk_fetchfast  ();
void bezerk_fetchflags  ();
ENVELOPE *bezerk_fetchstructure  ();
char *bezerk_snarf  ();
char *bezerk_fetchheader  ();
char *bezerk_fetchtext  ();
char *bezerk_fetchbody ();
void bezerk_setflag  ();
void bezerk_clearflag  ();
void bezerk_search  ();
long bezerk_ping  ();
void bezerk_check  ();
void bezerk_expunge  ();
long bezerk_copy  ();
long bezerk_move  ();
long bezerk_append  ();
long bezerk_append_putc  ();
void bezerk_gc  ();

void bezerk_abort  ();
char *bezerk_file  ();
int bezerk_lock  ();
void bezerk_unlock  ();
int bezerk_parse  ();
char *bezerk_eom  ();
int bezerk_extend  ();
void bezerk_save  ();
int bezerk_copy_messages  ();
void bezerk_write_message  ();
void bezerk_update_status  ();
short bezerk_getflags  ();
char bezerk_search_all  ();
char bezerk_search_answered  ();
char bezerk_search_deleted  ();
char bezerk_search_flagged  ();
char bezerk_search_keyword  ();
char bezerk_search_new  ();
char bezerk_search_old  ();
char bezerk_search_recent  ();
char bezerk_search_seen  ();
char bezerk_search_unanswered  ();
char bezerk_search_undeleted  ();
char bezerk_search_unflagged  ();
char bezerk_search_unkeyword  ();
char bezerk_search_unseen  ();
char bezerk_search_before  ();
char bezerk_search_on  ();
char bezerk_search_since  ();
char bezerk_search_body  ();
char bezerk_search_subject  ();
char bezerk_search_text  ();
char bezerk_search_bcc  ();
char bezerk_search_cc  ();
char bezerk_search_from  ();
char bezerk_search_to  ();

typedef char (*search_t)  ();
search_t bezerk_search_date  ();
search_t bezerk_search_flag  ();
search_t bezerk_search_string  ();
