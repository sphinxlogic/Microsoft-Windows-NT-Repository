/*
 * Program:	Dummy routines
 *
 * Author:	Mark Crispin
 *		Networks and Distributed Computing
 *		Computing & Communications
 *		University of Washington
 *		Administration Building, AG-44
 *		Seattle, WA  98195
 *		Internet: MRC@CAC.Washington.EDU
 *
 * Date:	9 May 1991
 * Last Edited:	28 September 1993
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


#include <stdio.h>
#include <ctype.h>
#include <errno.h>
extern int errno;		/* just in case */
#include "mail.h"
#include "osdep.h"
#include <pwd.h>
#include <sys/file.h>
#include "dummy.h"
#include "misc.h"

/* Dummy routines */


/* Driver dispatch used by MAIL */

DRIVER dummydriver = {
  "dummy",			/* driver name */
  (DRIVER *) NIL,		/* next driver */
  dummy_valid,			/* mailbox is valid for us */
  dummy_parameters,		/* manipulate parameters */
  dummy_find,			/* find mailboxes */
  dummy_find_bboards,		/* find bboards */
  dummy_find_all,		/* find all mailboxes */
  dummy_find_all_bboards,	/* find all bboards */
  dummy_subscribe,		/* subscribe to mailbox */
  dummy_unsubscribe,		/* unsubscribe from mailbox */
  dummy_subscribe_bboard,	/* subscribe to bboard */
  dummy_unsubscribe_bboard,	/* unsubscribe from bboard */
  dummy_create,			/* create mailbox */
  dummy_delete,			/* delete mailbox */
  dummy_rename,			/* rename mailbox */
  dummy_open,			/* open mailbox */
  dummy_close,			/* close mailbox */
  dummy_fetchfast,		/* fetch message "fast" attributes */
  dummy_fetchflags,		/* fetch message flags */
  dummy_fetchstructure,		/* fetch message structure */
  dummy_fetchheader,		/* fetch message header only */
  dummy_fetchtext,		/* fetch message body only */
  dummy_fetchbody,		/* fetch message body section */
  dummy_setflag,		/* set message flag */
  dummy_clearflag,		/* clear message flag */
  dummy_search,			/* search for message based on criteria */
  dummy_ping,			/* ping mailbox to see if still alive */
  dummy_check,			/* check for new messages */
  dummy_expunge,		/* expunge deleted messages */
  dummy_copy,			/* copy messages to another mailbox */
  dummy_move,			/* move messages to another mailbox */
  dummy_append,			/* append string message to mailbox */
  dummy_gc			/* garbage collect stream */
};

/* Dummy validate mailbox
 * Accepts: mailbox name
 * Returns: our driver if name is valid, NIL otherwise
 */

DRIVER *dummy_valid (name)
	char *name;
{
  return (name && *name) ? &dummydriver : NIL;
}


/* Dummy manipulate driver parameters
 * Accepts: function code
 *	    function-dependent value
 * Returns: function-dependent return value
 */

void *dummy_parameters (function,value)
	long function;
	void *value;
{
  return NIL;
}

/* Dummy find list of subscribed mailboxes
 * Accepts: mail stream
 *	    pattern to search
 */

void dummy_find (stream,pat)
	MAILSTREAM *stream;
	char *pat;
{
  void *s = NIL;
  char *t;
  if (*pat == '{') return;	/* local only */
  if (t = sm_read (&s)) { 	/* if have subscription database */
    do if ((*t != '{') && (*t != '*') && strcmp (t,"INBOX") && pmatch (t,pat))
      mm_mailbox (t);
    while (t = sm_read (&s));	/* until no more subscriptions */
  }
  else dummy_find_all (stream,pat);
}


/* Dummy find list of subscribed bboards
 * Accepts: mail stream
 *	    pattern to search
 */

void dummy_find_bboards (stream,pat)
	MAILSTREAM *stream;
	char *pat;
{
  void *s = NIL;
  char *t,tmp[MAILTMPLEN];
  if (*pat == '{') return;	/* local only */
				/* no-op if have a subscription database */
  if (t = sm_read (&s)) {	/* if have subscription database */
    do if ((*t == '*') && (t[1] != '{') && pmatch (t+1,pat)) mm_bboard (t+1);
    while (t = sm_read (&s));	/* read subscription database */
  }
  else dummy_find_all_bboards (stream,pat);
}

/* Dummy find list of all mailboxes
 * Accepts: mail stream
 *	    pattern to search
 */

void dummy_find_all (stream,pat)
	MAILSTREAM *stream;
	char *pat;
{
  DIR *dirp;
  struct direct *d;
  char tmp[MAILTMPLEN],file[MAILTMPLEN];
  char *s,*t;
  int i = 0;
  if (*pat == '{') return;	/* local only */
  if (s = strrchr (pat,'/')) {	/* directory specified in pattern? */
    strncpy (file,pat,i = (++s) - pat);
    file[i] = '\0';		/* tie off prefix */
    t = dummy_file (tmp,pat);	/* make fully-qualified file name */
				/* tie off directory name */
    if (s = strrchr (t,'/')) *s = '\0';
  }
  else t = myhomedir ();	/* use home directory to search */
  if (dirp = opendir (t)) {	/* now open that directory */
    while (d = readdir (dirp)) {/* for each directory entry */
      strcpy (file + i,d->d_name);
      if (pmatch (file,pat)) mm_mailbox (file);
    }
    closedir (dirp);		/* flush directory */
  }
				/* always an INBOX */
  if (pmatch ("INBOX",pat)) mm_mailbox ("INBOX");
}

/* Dummy find list of all bboards
 * Accepts: mail stream
 *	    pattern to search
 */

void dummy_find_all_bboards (stream,pat)
	MAILSTREAM *stream;
	char *pat;
{
  DIR *dirp;
  struct direct *d;
  struct passwd *pw;
  char tmp[MAILTMPLEN],file[MAILTMPLEN];
  int i = 1;
  char *s;
				/* local only */
  if ((*pat == '{') || !((pw = getpwnam ("ftp")) && pw->pw_dir)) return;
  file[0] = '*';		/* bboard designator */
				/* directory specified in pattern? */
  if (s = strrchr (pat,'/')) strncpy (file + 1,pat,i += (++s) - pat);
  file[i] = '\0';		/* tie off prefix */
  sprintf (tmp,"%s/%s",pw->pw_dir,(file[1] == '/') ? file + 2 : file + 1);
  if (dirp = opendir (tmp)) {	/* now open that directory */
    while (d = readdir (dirp)) {/* for each directory entry */
      strcpy (file + i,d->d_name);
      if (pmatch (file + 1,pat)) mm_bboard (file + 1);
    }
    closedir (dirp);		/* flush directory */
  }
}

/* Dummy subscribe to mailbox
 * Accepts: mail stream
 *	    mailbox to add to subscription list
 * Returns: T on success, NIL on failure
 */

long dummy_subscribe (stream,mailbox)
	MAILSTREAM *stream;
	char *mailbox;
{
  return NIL;			/* always fails */
}


/* Dummy unsubscribe to mailbox
 * Accepts: mail stream
 *	    mailbox to delete from subscription list
 * Returns: T on success, NIL on failure
 */

long dummy_unsubscribe (stream,mailbox)
	MAILSTREAM *stream;
	char *mailbox;
{
  return NIL;			/* always fails */
}


/* Dummy subscribe to bboard
 * Accepts: mail stream
 *	    bboard to add to subscription list
 * Returns: T on success, NIL on failure
 */

long dummy_subscribe_bboard (stream,mailbox)
	MAILSTREAM *stream;
	char *mailbox;
{
  return NIL;			/* always fails */
}


/* Dummy unsubscribe to bboard
 * Accepts: mail stream
 *	    bboard to delete from subscription list
 * Returns: T on success, NIL on failure
 */

long dummy_unsubscribe_bboard (stream,mailbox)
	MAILSTREAM *stream;
	char *mailbox;
{
  return NIL;			/* always fails */
}

/* Dummy create mailbox
 * Accepts: mail stream
 *	    mailbox name to create
 *	    driver type to use
 * Returns: T on success, NIL on failure
 */

long dummy_create (stream,mailbox)
	MAILSTREAM *stream;
	char *mailbox;
{
  return NIL;			/* always fails */
}


/* Dummy delete mailbox
 * Accepts: mail stream
 *	    mailbox name to delete
 * Returns: T on success, NIL on failure
 */

long dummy_delete (stream,mailbox)
	MAILSTREAM *stream;
	char *mailbox;
{
  return NIL;			/* always fails */
}


/* Mail rename mailbox
 * Accepts: mail stream
 *	    old mailbox name
 *	    new mailbox name
 * Returns: T on success, NIL on failure
 */

long dummy_rename (stream,old,new)
	MAILSTREAM *stream;
	char *old;
	char *new;
{
  return NIL;			/* always fails */
}

/* Dummy open
 * Accepts: stream to open
 * Returns: stream on success, NIL on failure
 */

MAILSTREAM *dummy_open (stream)
	MAILSTREAM *stream;
{
  int fd;
  char tmp[MAILTMPLEN];
				/* OP_PROTOTYPE call or silence */
  if (!stream || stream->silent) return NIL;
  if (*stream->mailbox == '*')	/* is it a bboard? */
    sprintf (tmp,"No such bboard: %s",stream->mailbox+1);
				/* remote specification? */
  else if (*stream->mailbox == '{')
    sprintf (tmp,"Invalid remote specification: %s",stream->mailbox);
  else if ((fd = open (dummy_file (tmp,stream->mailbox),O_RDONLY,NIL)) < 0)
    sprintf (tmp,"%s: %s",strerror (errno),stream->mailbox);
  else {			/* must be bogus format file */
    sprintf (tmp,"%s is not a mailbox",stream->mailbox);
    close (fd);			/* toss out the fd */
  }
  mm_log (tmp,ERROR);
  return NIL;			/* always fails */
}


/* Dummy close
 * Accepts: MAIL stream
 */

void dummy_close (stream)
	MAILSTREAM *stream;
{
  /* Exit quietly */
}

/* Dummy fetch fast information
 * Accepts: MAIL stream
 *	    sequence
 */

void dummy_fetchfast (stream,sequence)
	MAILSTREAM *stream;
	char *sequence;
{
  fatal ("Impossible dummy_fetchfast");
}


/* Dummy fetch flags
 * Accepts: MAIL stream
 *	    sequence
 */

void dummy_fetchflags (stream,sequence)
	MAILSTREAM *stream;
	char *sequence;
{
  fatal ("Impossible dummy_fetchflags");
}


/* Dummy fetch envelope
 * Accepts: MAIL stream
 *	    message # to fetch
 *	    pointer to return body
 * Returns: envelope of this message, body returned in body value
 */

ENVELOPE *dummy_fetchstructure (stream,msgno,body)
	MAILSTREAM *stream;
	long msgno;
	BODY **body;
{
  fatal ("Impossible dummy_fetchstructure");
  return NIL;
}


/* Dummy fetch message header
 * Accepts: MAIL stream
 *	    message # to fetch
 * Returns: message header in RFC822 format
 */

char *dummy_fetchheader (stream,msgno)
	MAILSTREAM *stream;
	long msgno;
{
  fatal ("Impossible dummy_fetchheader");
  return NIL;
}

/* Dummy fetch message text (only)
	body only;
 * Accepts: MAIL stream
 *	    message # to fetch
 * Returns: message text in RFC822 format
 */

char *dummy_fetchtext (stream,msgno)
	MAILSTREAM *stream;
	long msgno;
{
  fatal ("Impossible dummy_fetchtext");
  return NIL;
}


/* Berkeley fetch message body as a structure
 * Accepts: Mail stream
 *	    message # to fetch
 *	    section specifier
 * Returns: pointer to section of message body
 */

char *dummy_fetchbody (stream,m,sec,len)
	MAILSTREAM *stream;
	long m;
	char *sec;
	unsigned long *len;
{
  fatal ("Impossible dummy_fetchbody");
  return NIL;
}


/* Dummy set flag
 * Accepts: MAIL stream
 *	    sequence
 *	    flag(s)
 */

void dummy_setflag (stream,sequence,flag)
	MAILSTREAM *stream;
	char *sequence;
	char *flag;
{
  fatal ("Impossible dummy_setflag");
}


/* Dummy clear flag
 * Accepts: MAIL stream
 *	    sequence
 *	    flag(s)
 */

void dummy_clearflag (stream,sequence,flag)
	MAILSTREAM *stream;
	char *sequence;
	char *flag;
{
  fatal ("Impossible dummy_clearflag");
}


/* Dummy search for messages
 * Accepts: MAIL stream
 *	    search criteria
 */

void dummy_search (stream,criteria)
	MAILSTREAM *stream;
	char *criteria;
{
  fatal ("Impossible dummy_search");
}

/* Dummy ping mailbox
 * Accepts: MAIL stream
 * Returns: T if stream alive, else NIL
 * No-op for readonly files, since read/writer can expunge it from under us!
 */

long dummy_ping (stream)
	MAILSTREAM *stream;
{
  fatal ("Impossible dummy_ping");
  return NIL;
}


/* Dummy check mailbox
 * Accepts: MAIL stream
 * No-op for readonly files, since read/writer can expunge it from under us!
 */

void dummy_check (stream)
	MAILSTREAM *stream;
{
  fatal ("Impossible dummy_check");
}


/* Dummy expunge mailbox
 * Accepts: MAIL stream
 */

void dummy_expunge (stream)
	MAILSTREAM *stream;
{
  fatal ("Impossible dummy_expunge");
}

/* Dummy copy message(s)
	s;
 * Accepts: MAIL stream
 *	    sequence
 *	    destination mailbox
 * Returns: T if copy successful, else NIL
 */

long dummy_copy (stream,sequence,mailbox)
	MAILSTREAM *stream;
	char *sequence;
	char *mailbox;
{
  fatal ("Impossible dummy_copy");
  return NIL;
}


/* Dummy move message(s)
	s;
 * Accepts: MAIL stream
 *	    sequence
 *	    destination mailbox
 * Returns: T if move successful, else NIL
 */

long dummy_move (stream,sequence,mailbox)
	MAILSTREAM *stream;
	char *sequence;
	char *mailbox;
{
  fatal ("Impossible dummy_move");
  return NIL;
}


/* Dummy append message string
 * Accepts: mail stream
 *	    destination mailbox
 *	    stringstruct of message to append
 * Returns: T on success, NIL on failure
 */

long dummy_append (stream,mailbox,message)
	MAILSTREAM *stream;
	char *mailbox;
	STRING *message;
{
  int fd = -1,e;
  char tmp[MAILTMPLEN];
				/* see if such a file */
  if ((*mailbox != '*') && (*mailbox != '{') &&
      (fd = open (dummy_file (tmp,mailbox),O_RDONLY,NIL)) < 0) {
    if ((e = errno) == ENOENT)	/* failed, was it no such file? */
      mm_notify (stream,"[TRYCREATE] Must create mailbox before append",NIL);
    sprintf (tmp,"%s: %s",strerror (e),mailbox);
  }
  else {			/* must be bogus format file */
    sprintf (tmp,"%s is not a valid mailbox",mailbox);
    close (fd);			/* toss out the fd */
  }
  mm_log (tmp,ERROR);		/* pass up error */
  return NIL;			/* always fails */
}


/* Dummy garbage collect stream
 * Accepts: mail stream
 *	    garbage collection flags
 */

void dummy_gc (stream,gcflags)
	MAILSTREAM *stream;
	long gcflags;
{
  fatal ("Impossible dummy_gc");
}

/* Dummy mail generate file string
 * Accepts: temporary buffer to write into
 *	    mailbox name string
 * Returns: local file string
 */

char *dummy_file (dst,name)
	char *dst;
	char *name;
{
  struct passwd *pw;
  char *s,*t,tmp[MAILTMPLEN];
  switch (*name) {
  case '/':			/* absolute file path */
    strcpy (dst,name);		/* copy the mailbox name */
    break;
  case '~':			/* home directory */
    if (name[1] == '/') t = myhomedir ();
    else {
      strcpy (tmp,name + 1);	/* copy user name */
      if (s = strchr (tmp,'/')) *s = '\0';
      t = ((pw = getpwnam (tmp)) && pw->pw_dir) ? pw->pw_dir : "/NOSUCHUSER";
    }
    sprintf (dst,"%s%s",t,(s = strchr (name,'/')) ? s : "");
    break;
  default:			/* any other name */
    sprintf (dst,"%s/%s",myhomedir (),name);
  }
  return dst;
}
