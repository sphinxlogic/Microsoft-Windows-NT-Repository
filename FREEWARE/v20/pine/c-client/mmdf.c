/*
 * Program:	MMDF mail routines
 *
 * Author:	Bruce W. Knipe <bwk@cyberstore.ca> from bezerk.c by:
 *		Mark Crispin
 *		Networks and Distributed Computing
 *		Computing & Communications
 *		University of Washington
 *		Administration Building, AG-44
 *		Seattle, WA  98195
 *		Internet: MRC@CAC.Washington.EDU
 *
 * Date:	15 May 1993
 * Last Edited:	18 November 1993
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
#include <netdb.h>
#include <errno.h>
extern int errno;		/* just in case */
#include <signal.h>
#include <sys/time.h>		/* must be before osdep.h */
#include "mail.h"
#include "osdep.h"
#include <pwd.h>
#include <sys/file.h>
#include <sys/stat.h>
#include "mmdf.h"
#include "rfc822.h"
#include "misc.h"
#include "dummy.h"

/* MMDF mail routines */


/* Driver dispatch used by MAIL */

DRIVER mmdfdriver = {
  "mmdf",			/* driver name */
  (DRIVER *) NIL,		/* next driver */
  mmdf_valid,			/* mailbox is valid for us */
  mmdf_parameters,		/* manipulate parameters */
  mmdf_find,			/* find mailboxes */
  mmdf_find_bboards,		/* find bboards */
  mmdf_find_all,		/* find all mailboxes */
  mmdf_find_all_bboards,	/* find all bboards */
  mmdf_subscribe,		/* subscribe to mailbox */
  mmdf_unsubscribe,		/* unsubscribe from mailbox */
  mmdf_subscribe_bboard,	/* subscribe to bboard */
  mmdf_unsubscribe_bboard,	/* unsubscribe from bboard */
  mmdf_create,			/* create mailbox */
  mmdf_delete,			/* delete mailbox */
  mmdf_rename,			/* rename mailbox */
  mmdf_open,			/* open mailbox */
  mmdf_close,			/* close mailbox */
  mmdf_fetchfast,		/* fetch message "fast" attributes */
  mmdf_fetchflags,		/* fetch message flags */
  mmdf_fetchstructure,		/* fetch message envelopes */
  mmdf_fetchheader,		/* fetch message header only */
  mmdf_fetchtext,		/* fetch message body only */
  mmdf_fetchbody,		/* fetch message body section */
  mmdf_setflag,			/* set message flag */
  mmdf_clearflag,		/* clear message flag */
  mmdf_search,			/* search for message based on criteria */
  mmdf_ping,			/* ping mailbox to see if still alive */
  mmdf_check,			/* check for new messages */
  mmdf_expunge,			/* expunge deleted messages */
  mmdf_copy,			/* copy messages to another mailbox */
  mmdf_move,			/* move messages to another mailbox */
  mmdf_append,			/* append string message to mailbox */
  mmdf_gc			/* garbage collect stream */
};

				/* prototype stream */
MAILSTREAM mmdfproto = {&mmdfdriver};

/* MMDF mail validate mailbox
 * Accepts: mailbox name
 * Returns: our driver if name is valid, NIL otherwise
 */

DRIVER *mmdf_valid (name)
	char *name;
{
  char tmp[MAILTMPLEN];
  return mmdf_isvalid (name,tmp) ? &mmdfdriver : NIL;
}


/* MMDF mail test for valid mailbox
 * Accepts: mailbox name
 * Returns: T if valid, NIL otherwise
 */

int mmdf_isvalid (name,tmp)
	char *name;
	char *tmp;
{
  int i,fd,ti,zn;
  int ret = NIL;
  char *s = tmp,*t;
  struct stat sbuf;
  struct hostent *host_name;
  if (!lhostn) {		/* have local host yet? */
    gethostname(tmp,MAILTMPLEN);/* get local host name */
    lhostn = cpystr ((host_name = gethostbyname (tmp)) ?
		     host_name->h_name : tmp);
  }
				/* INBOX is always accepted */
  if (!strcmp (ucase (strcpy (tmp,name)),"INBOX")) return T;
				/* if file, get its status */
  if (*name != '{' && (stat (mmdf_file (tmp,name),&sbuf) == 0) &&
      ((fd = open (tmp,O_RDONLY,NIL)) >= 0)) {
    if (sbuf.st_size == 0) { 	/* allow empty file if not .TxT */
      if ((i = strlen (tmp)) < 4 || strcmp (tmp + i - 4 ,".TxT"))
	return LONGT;
    }
    else if ((read (fd,tmp,MAILTMPLEN-1) >= 0) &&
	     (tmp[0] == (char)0x01) && VALID (s,t,ti,zn)) ret = T;
    close (fd);			/* close the file */
  }
  return ret;			/* return what we should */
}

/* MMDF manipulate driver parameters
 * Accepts: function code
 *	    function-dependent value
 * Returns: function-dependent return value
 */

void *mmdf_parameters (function,value)
	long function;
	void *value;
{
  fatal ("Invalid mmdf_parameters function");
}

/* MMDF mail find list of mailboxes
 * Accepts: mail stream
 *	    pattern to search
 */

void mmdf_find (stream,pat)
	MAILSTREAM *stream;
	char *pat;
{
  if (stream) dummy_find (NIL,pat);
}


/* MMDF mail find list of bboards
 * Accepts: mail stream
 *	    pattern to search
 */

void mmdf_find_bboards (stream,pat)
	MAILSTREAM *stream;
	char *pat;
{
  if (stream) dummy_find_bboards (NIL,pat);
}


/* MMDF mail find list of all mailboxes
 * Accepts: mail stream
 *	    pattern to search
 */

void mmdf_find_all (stream,pat)
	MAILSTREAM *stream;
	char *pat;
{
  if (stream) dummy_find_all (NIL,pat);
}


/* MMDF mail find list of all bboards
 * Accepts: mail stream
 *	    pattern to search
 */

void mmdf_find_all_bboards (stream,pat)
	MAILSTREAM *stream;
	char *pat;
{
  if (stream) dummy_find_all_bboards (NIL,pat);
}

/* MMDF mail subscribe to mailbox
 * Accepts: mail stream
 *	    mailbox to add to subscription list
 * Returns: T on success, NIL on failure
 */

long mmdf_subscribe (stream,mailbox)
	MAILSTREAM *stream;
	char *mailbox;
{
  char tmp[MAILTMPLEN];
  return sm_subscribe (mmdf_file (tmp,mailbox));
}


/* MMDF mail unsubscribe to mailbox
 * Accepts: mail stream
 *	    mailbox to delete from subscription list
 * Returns: T on success, NIL on failure
 */

long mmdf_unsubscribe (stream,mailbox)
	MAILSTREAM *stream;
	char *mailbox;
{
  char tmp[MAILTMPLEN];
  return sm_unsubscribe (mmdf_file (tmp,mailbox));
}


/* MMDF mail subscribe to bboard
 * Accepts: mail stream
 *	    bboard to add to subscription list
 * Returns: T on success, NIL on failure
 */

long mmdf_subscribe_bboard (stream,mailbox)
	MAILSTREAM *stream;
	char *mailbox;
{
  return NIL;			/* never valid for MMDF */
}


/* MMDF mail unsubscribe to bboard
 * Accepts: mail stream
 *	    bboard to delete from subscription list
 * Returns: T on success, NIL on failure
 */

long mmdf_unsubscribe_bboard (stream,mailbox)
	MAILSTREAM *stream;
	char *mailbox;
{
  return NIL;			/* never valid for MMDF */
}

/* MMDF mail create mailbox
 * Accepts: MAIL stream
 *	    mailbox name to create
 * Returns: T on success, NIL on failure
 */

long mmdf_create (stream,mailbox)
	MAILSTREAM *stream;
	char *mailbox;
{
  char tmp[MAILTMPLEN];
  int i,fd;
				/* must not be a ".TxT" file */
  if ((i = strlen (mailbox)) > 4 && !strcmp (mailbox + i - 4 ,".TxT")) {
    mm_log ("Can't create mailbox: name must not end with .TxT",ERROR);
    return NIL;
  }
  if ((fd = open (mmdf_file (tmp,mailbox),O_WRONLY|O_CREAT|O_EXCL,0600))<0) {
    sprintf (tmp,"Can't create mailbox %s: %s",mailbox,strerror (errno));
    mm_log (tmp,ERROR);
    return NIL;
  }
  close (fd);			/* close the file */
  return T;			/* return success */
}


/* MMDF mail delete mailbox
 * Accepts: MAIL stream
 *	    mailbox name to delete
 * Returns: T on success, NIL on failure
 */

long mmdf_delete (stream,mailbox)
	MAILSTREAM *stream;
	char *mailbox;
{
  return mmdf_rename (stream,mailbox,NIL);
}

/* MMDF mail rename mailbox
 * Accepts: MAIL stream
 *	    old mailbox name
 *	    new mailbox name (or NIL for delete)
 * Returns: T on success, NIL on failure
 */

long mmdf_rename (stream,old,new)
	MAILSTREAM *stream;
	char *old;
	char *new;
{
  long ret = T;
  char tmp[MAILTMPLEN],file[MAILTMPLEN],lock[MAILTMPLEN],lockx[MAILTMPLEN];
  int fd,ld;
				/* get the c-client lock */
  if ((ld = open (lockname (lock,mmdf_file (file,old)),O_RDWR|O_CREAT,0666))
      < 0) {
    sprintf (tmp,"Can't get lock for mailbox %s: %s",old,strerror (errno));
    mm_log (tmp,ERROR);
    return NIL;
  }
				/* lock out other c-clients */
  if (flock (ld,LOCK_EX|LOCK_NB)) {
    close (ld);			/* couldn't lock, give up on it then */
    sprintf (tmp,"Mailbox %s is in use by another process",old);
    mm_log (tmp,ERROR);
    return NIL;
  }
				/* lock out non c-client applications */
  if ((fd = mmdf_lock (file,O_RDONLY,S_IREAD|S_IWRITE,lockx,LOCK_EX)) < 0) {
    sprintf (tmp,"Can't lock mailbox %s: %s",old,strerror (errno));
    mm_log (tmp,ERROR);
    return NIL;
  }
				/* do the rename or delete operation */
  if (new ? rename (file,mmdf_file (tmp,new)) : unlink (file)) {
    sprintf (tmp,"Can't %s mailbox %s: %s",new ? "rename" : "delete",old,
	     strerror (errno));
    mm_log (tmp,ERROR);
    ret = NIL;			/* set failure */
  }
  mmdf_unlock (fd,NIL,lockx);	/* unlock and close mailbox */
  flock (ld,LOCK_UN);		/* release c-client lock lock */
  close (ld);			/* close c-client lock */
  unlink (lock);		/* and delete it */
  return ret;			/* return success */
}

/* MMDF mail open
 * Accepts: Stream to open
 * Returns: Stream on success, NIL on failure
 */

MAILSTREAM *mmdf_open (stream)
	MAILSTREAM *stream;
{
  long i;

  long retry = stream->silent ? 1 : KODRETRY;
  int fd;
  char tmp[MAILTMPLEN];
  struct stat sbuf;
				/* return prototype for OP_PROTOTYPE call */
  if (!stream) return &mmdfproto;
  if (LOCAL) {			/* close old file if stream being recycled */
    mmdf_close (stream);	/* dump and save the changes */
    stream->dtb = &mmdfdriver;/* reattach this driver */
    mail_free_cache (stream);	/* clean up cache */
  }
  stream->local = fs_get (sizeof (MMDFLOCAL));
				/* canonicalize the stream mailbox name */
  mmdf_file (tmp,stream->mailbox);
  fs_give ((void **) &stream->mailbox);
  stream->mailbox = cpystr (tmp);
  /* You may wonder why LOCAL->name is needed.  It isn't at all obvious from
   * the code.  The problem is that when a stream is recycled with another
   * mailbox of the same type, the driver's close method isn't called because
   * it could be IMAP and closing then would defeat the entire point of
   * recycling.  Hence there is code in the file drivers to call the close
   * method such as what appears above.  The problem is, by this point,
   * mail_open() has already changed the stream->mailbox name to point to the
   * new name, and mmdf_close() needs the old name.
   */
  LOCAL->name = cpystr (tmp);	/* local copy for recycle case */
				/* build name of our lock file */
  LOCAL->lname = cpystr (lockname (tmp,stream->mailbox));
  LOCAL->ld = NIL;		/* no state locking yet */
  LOCAL->filesize = 0;		/* initialize file information */
  LOCAL->filetime = 0;
  LOCAL->msgs = NIL;		/* no cache yet */
  LOCAL->cachesize = 0;
  LOCAL->buf = (char *) fs_get ((LOCAL->buflen = CHUNK) + 1);
  stream->sequence++;		/* bump sequence number */

  LOCAL->dirty = NIL;		/* no update yet */
				/* make lock for read/write access */
  if (!stream->readonly) while (retry) {
				/* get a new file handle each time */
    if ((fd = open (LOCAL->lname,O_RDWR|O_CREAT,0666)) < 0)
      mm_log ("Can't open mailbox lock, access is readonly",WARN);
				/* can get the lock? */
    if (flock (fd,LOCK_EX|LOCK_NB)) {
      if (retry-- == KODRETRY) {/* no, first time through? */
				/* yes, get other process' PID */
	if (!fstat (fd,&sbuf) && (i = min (sbuf.st_size,MAILTMPLEN)) &&
	    (read (fd,tmp,i) == i) && (i = atol (tmp))) {
	  kill (i,SIGUSR2);	/* send the Kiss Of Death */
	  sprintf (tmp,"Trying to get mailbox lock from process %ld",i);
	  mm_log (tmp,WARN);
	}
	else retry = 0;		/* give up */
      }
      close (fd);		/* get a new handle next time around */
      if (!stream->silent) {	/* nothing if silent stream */
	if (retry) sleep (1);	/* wait a second before trying again */
	else mm_log ("Mailbox is open by another process, access is readonly",
		     WARN);
      }
    }
    else {			/* got the lock, nobody else can alter state */
      LOCAL->ld = fd;		/* note lock's fd */
      chmod (LOCAL->lname,0666);/* make sure mode OK (don't use fchmod()) */
      if (stream->silent) i = 0;/* silent streams won't accept KOD */
      else {			/* note our PID in the lock */
	sprintf (tmp,"%d",getpid ());
	write (fd,tmp,strlen (tmp));
      }
      ftruncate (fd,i);		/* make sure tied off */
      fsync (fd);		/* make sure it's available */
      retry = 0;		/* no more need to try */
    }
  }

				/* parse mailbox */
  stream->nmsgs = stream->recent = 0;
				/* will we be able to get write access? */
  if (LOCAL->ld && access (LOCAL->name,W_OK) && (errno == EACCES)) {
    mm_log ("Can't get write access to mailbox, access is readonly",WARN);
    flock (LOCAL->ld,LOCK_UN);	/* release the lock */
    close (LOCAL->ld);		/* close the lock file */
    LOCAL->ld = NIL;		/* no more lock fd */
    unlink (LOCAL->lname);	/* delete it */
    fs_give ((void **) &LOCAL->lname);
  }
				/* abort if can't get RW silent stream */
  if (stream->silent && !stream->readonly && !LOCAL->ld) bezerk_abort (stream);
				/* parse mailbox */
  else if ((fd = mmdf_parse (stream,tmp,LOCK_SH)) >= 0) {
    mmdf_unlock (fd,stream,tmp);
    mail_unlock (stream);
  }
  if (!LOCAL) return NIL;	/* failure if stream died */
  stream->readonly = !LOCAL->ld;/* make sure upper level knows readonly */
				/* notify about empty mailbox */
  if (!(stream->nmsgs || stream->silent)) mm_log ("Mailbox is empty",NIL);
  return stream;		/* return stream alive to caller */
}

/* MMDF mail close
 * Accepts: MAIL stream
 */

void mmdf_close (stream)
	MAILSTREAM *stream;
{
  int fd;
  int silent = stream->silent;
  char lock[MAILTMPLEN];
  if (LOCAL && LOCAL->ld) {	/* is stream alive? */
    stream->silent = T;		/* note this stream is dying */
				/* lock mailbox and parse new messages */
    if (LOCAL->dirty && (fd = mmdf_parse (stream,lock,LOCK_EX)) >= 0) {
				/* dump any changes not saved yet */
      if (mmdf_extend	(stream,fd,"Close failed to update mailbox"))
	mmdf_save (stream,fd);
				/* flush locks */
      mmdf_unlock (fd,stream,lock);
      mail_unlock (stream);
    }
    stream->silent = silent;	/* restore previous status */
  }
  mmdf_abort (stream);	/* now punt the file and local data */
}


/* MMDF mail fetch fast information
 * Accepts: MAIL stream
 *	    sequence
 */

void mmdf_fetchfast (stream,sequence)
	MAILSTREAM *stream;
	char *sequence;
{
  return;			/* no-op for local mail */
}


/* MMDF mail fetch flags
 * Accepts: MAIL stream
 *	    sequence
 */

void mmdf_fetchflags (stream,sequence)
	MAILSTREAM *stream;
	char *sequence;
{
  return;			/* no-op for local mail */
}

/* MMDF mail fetch structure
 * Accepts: MAIL stream
 *	    message # to fetch
 *	    pointer to return body
 * Returns: envelope of this message, body returned in body value
 *
 * Fetches the "fast" information as well
 */

ENVELOPE *mmdf_fetchstructure (stream,msgno,body)
	MAILSTREAM *stream;
	long msgno;
	BODY **body;
{
  ENVELOPE **env;
  BODY **b;
  STRING bs;
  LONGCACHE *lelt;
  FILECACHE *m = LOCAL->msgs[msgno - 1];
  long i = max (m->headersize,m->bodysize);
  if (stream->scache) {		/* short cache */
    if (msgno != stream->msgno){/* flush old poop if a different message */
      mail_free_envelope (&stream->env);
      mail_free_body (&stream->body);
    }
    stream->msgno = msgno;
    env = &stream->env;		/* get pointers to envelope and body */
    b = &stream->body;
  }
  else {			/* long cache */
    lelt = mail_lelt (stream,msgno);
    env = &lelt->env;		/* get pointers to envelope and body */
    b = &lelt->body;
  }
  if ((body && !*b) || !*env) {	/* have the poop we need? */
    mail_free_envelope (env);	/* flush old envelope and body */
    mail_free_body (b);
    if (i > LOCAL->buflen) {	/* make sure enough buffer space */
      fs_give ((void **) &LOCAL->buf);
      LOCAL->buf = (char *) fs_get ((LOCAL->buflen = i) + 1);
    }
    INIT (&bs,mail_string,(void *) m->body,m->bodysize);
				/* parse envelope and body */
    rfc822_parse_msg (env,body ? b : NIL,m->header,m->headersize,&bs,lhostn,
		      LOCAL->buf);
  }
  if (body) *body = *b;		/* return the body */
  return *env;			/* return the envelope */
}

/* Bezerk mail snarf message, only for Tenex driver
 * Accepts: MAIL stream
 *	    message # to snarf
 *	    pointer to size to return
 * Returns: message text in RFC822 format
 */

char *bezerk_snarf (stream,msgno,size)
	MAILSTREAM *stream;
	long msgno;
	long *size;
{
  MESSAGECACHE *elt = mail_elt (stream,msgno);
  FILECACHE *m = LOCAL->msgs[msgno - 1];
  if (((*size = m->headersize + m->bodysize) > LOCAL->buflen) ||
      LOCAL->buflen > CHUNK) {	/* make sure stream can hold the text */
				/* fs_resize would do an unnecessary copy */
    fs_give ((void **) &LOCAL->buf);
    LOCAL->buf = (char *) fs_get((LOCAL->buflen = max (*size,(long) CHUNK))+1);
  }
				/* copy the text */
  if (m->headersize) memcpy (LOCAL->buf,m->header,m->headersize);
  if (m->bodysize) memcpy (LOCAL->buf + m->headersize,m->body,m->bodysize);
  LOCAL->buf[*size] = '\0';	/* tie off string */
  return LOCAL->buf;
}

/* MMDF mail fetch message header
 * Accepts: MAIL stream
 *	    message # to fetch
 * Returns: message header in RFC822 format
 */

char *mmdf_fetchheader (stream,msgno)
	MAILSTREAM *stream;
	long msgno;
{
  FILECACHE *m = LOCAL->msgs[msgno - 1];
				/* copy the string */
  strcrlfcpy (&LOCAL->buf,&LOCAL->buflen,m->header,m->headersize);
  return LOCAL->buf;
}


/* MMDF mail fetch message text (only)
	body only;
 * Accepts: MAIL stream
 *	    message # to fetch
 * Returns: message text in RFC822 format
 */

char *mmdf_fetchtext (stream,msgno)
	MAILSTREAM *stream;
	long msgno;
{
  MESSAGECACHE *elt = mail_elt (stream,msgno);
  FILECACHE *m = LOCAL->msgs[msgno - 1];
  if (!elt->seen) {		/* if message not seen */
    elt->seen = T;		/* mark message as seen */
				/* recalculate Status/X-Status lines */
    mmdf_update_status (m->status,elt);
    LOCAL->dirty = T;		/* note stream is now dirty */
  }
  strcrlfcpy (&LOCAL->buf,&LOCAL->buflen,m->body,m->bodysize);
  return LOCAL->buf;
}

/* MMDF fetch message body as a structure
 * Accepts: Mail stream
 *	    message # to fetch
 *	    section specifier
 *	    pointer to length
 * Returns: pointer to section of message body
 */

char *mmdf_fetchbody (stream,m,s,len)
	MAILSTREAM *stream;
	long m;
	char *s;
	unsigned long *len;
{
  BODY *b;
  PART *pt;
  unsigned long i;
  char *base = LOCAL->msgs[m - 1]->body;
  unsigned long offset = 0;
  MESSAGECACHE *elt = mail_elt (stream,m);
				/* make sure have a body */
  if (!(mmdf_fetchstructure (stream,m,&b) && b && s && *s &&
	((i = strtol (s,&s,10)) > 0))) return NIL;
  do {				/* until find desired body part */
				/* multipart content? */
    if (b->type == TYPEMULTIPART) {
      pt = b->contents.part;	/* yes, find desired part */
      while (--i && (pt = pt->next));
      if (!pt) return NIL;	/* bad specifier */
				/* note new body, check valid nesting */
      if (((b = &pt->body)->type == TYPEMULTIPART) && !*s) return NIL;
      offset = pt->offset;	/* get new offset */
    }
    else if (i != 1) return NIL;/* otherwise must be section 1 */
				/* need to go down further? */
    if (i = *s) switch (b->type) {
    case TYPEMESSAGE:		/* embedded message */
      offset = b->contents.msg.offset;
      b = b->contents.msg.body;	/* get its body, drop into multipart case */
    case TYPEMULTIPART:		/* multipart, get next section */
      if ((*s++ == '.') && (i = strtol (s,&s,10)) > 0) break;
    default:			/* bogus subpart specification */
      return NIL;
    }
  } while (i);
				/* lose if body bogus */
  if ((!b) || b->type == TYPEMULTIPART) return NIL;
  if (!elt->seen) {		/* if message not seen */
    elt->seen = T;		/* mark message as seen */
				/* recalculate Status/X-Status lines */
    mmdf_update_status (LOCAL->msgs[m - 1]->status,elt);
    LOCAL->dirty = T;		/* note stream is now dirty */
  }
  return rfc822_contents (&LOCAL->buf,&LOCAL->buflen,len,base + offset,
			  b->size.ibytes,b->encoding);
}

/* MMDF mail set flag
 * Accepts: MAIL stream
 *	    sequence
 *	    flag(s)
 */

void mmdf_setflag (stream,sequence,flag)
	MAILSTREAM *stream;
	char *sequence;
	char *flag;
{
  MESSAGECACHE *elt;
  long i;
  short f = mmdf_getflags (stream,flag);
  if (!f) return;		/* no-op if no flags to modify */
				/* get sequence and loop on it */
  if (mail_sequence (stream,sequence)) for (i = 1; i <= stream->nmsgs; i++)
    if ((elt = mail_elt (stream,i))->sequence) {
				/* set all requested flags */
      if (f&fSEEN) elt->seen = T;
      if (f&fDELETED) elt->deleted = T;
      if (f&fFLAGGED) elt->flagged = T;
      if (f&fANSWERED) elt->answered = T;
				/* recalculate Status/X-Status lines */
      mmdf_update_status (LOCAL->msgs[i - 1]->status,elt);
      LOCAL->dirty = T;		/* note stream is now dirty */
    }
}


/* MMDF mail clear flag
 * Accepts: MAIL stream
 *	    sequence
 *	    flag(s)
 */

void mmdf_clearflag (stream,sequence,flag)
	MAILSTREAM *stream;
	char *sequence;
	char *flag;
{
  MESSAGECACHE *elt;
  long i;
  short f = mmdf_getflags (stream,flag);
  if (!f) return;		/* no-op if no flags to modify */
				/* get sequence and loop on it */
  if (mail_sequence (stream,sequence)) for (i = 1; i <= stream->nmsgs; i++)
    if ((elt = mail_elt (stream,i))->sequence) {
				/* clear all requested flags */
      if (f&fSEEN) elt->seen = NIL;
      if (f&fDELETED) elt->deleted = NIL;
      if (f&fFLAGGED) elt->flagged = NIL;
      if (f&fANSWERED) elt->answered = NIL;
				/* recalculate Status/X-Status lines */
      mmdf_update_status (LOCAL->msgs[i - 1]->status,elt);
      LOCAL->dirty = T;		/* note stream is now dirty */
    }
}

/* MMDF mail search for messages
 * Accepts: MAIL stream
 *	    search criteria
 */

void mmdf_search (stream,criteria)
	MAILSTREAM *stream;
	char *criteria;
{
  long i,n;
  char *d;
  search_t f;
				/* initially all searched */
  for (i = 1; i <= stream->nmsgs; ++i) mail_elt (stream,i)->searched = T;
				/* get first criterion */
  if (criteria && (criteria = strtok (criteria," "))) {
				/* for each criterion */
    for (; criteria; (criteria = strtok (NIL," "))) {
      f = NIL; d = NIL; n = 0;	/* init then scan the criterion */
      switch (*ucase (criteria)) {
      case 'A':			/* possible ALL, ANSWERED */
	if (!strcmp (criteria+1,"LL")) f = mmdf_search_all;
	else if (!strcmp (criteria+1,"NSWERED")) f = mmdf_search_answered;
	break;
      case 'B':			/* possible BCC, BEFORE, BODY */
	if (!strcmp (criteria+1,"CC"))
	  f = mmdf_search_string (mmdf_search_bcc,&d,&n);
	else if (!strcmp (criteria+1,"EFORE"))
	  f = mmdf_search_date (mmdf_search_before,&n);
	else if (!strcmp (criteria+1,"ODY"))
	  f = mmdf_search_string (mmdf_search_body,&d,&n);
	break;
      case 'C':			/* possible CC */
	if (!strcmp (criteria+1,"C"))
	  f = mmdf_search_string (mmdf_search_cc,&d,&n);
	break;
      case 'D':			/* possible DELETED */
	if (!strcmp (criteria+1,"ELETED")) f = mmdf_search_deleted;
	break;
      case 'F':			/* possible FLAGGED, FROM */
	if (!strcmp (criteria+1,"LAGGED")) f = mmdf_search_flagged;
	else if (!strcmp (criteria+1,"ROM"))
	  f = mmdf_search_string (mmdf_search_from,&d,&n);
	break;
      case 'K':			/* possible KEYWORD */
	if (!strcmp (criteria+1,"EYWORD"))
	  f = mmdf_search_flag (mmdf_search_keyword,&d);
	break;
      case 'N':			/* possible NEW */
	if (!strcmp (criteria+1,"EW")) f = mmdf_search_new;
	break;

      case 'O':			/* possible OLD, ON */
	if (!strcmp (criteria+1,"LD")) f = mmdf_search_old;
	else if (!strcmp (criteria+1,"N"))
	  f = mmdf_search_date (mmdf_search_on,&n);
	break;
      case 'R':			/* possible RECENT */
	if (!strcmp (criteria+1,"ECENT")) f = mmdf_search_recent;
	break;
      case 'S':			/* possible SEEN, SINCE, SUBJECT */
	if (!strcmp (criteria+1,"EEN")) f = mmdf_search_seen;
	else if (!strcmp (criteria+1,"INCE"))
	  f = mmdf_search_date (mmdf_search_since,&n);
	else if (!strcmp (criteria+1,"UBJECT"))
	  f = mmdf_search_string (mmdf_search_subject,&d,&n);
	break;
      case 'T':			/* possible TEXT, TO */
	if (!strcmp (criteria+1,"EXT"))
	  f = mmdf_search_string (mmdf_search_text,&d,&n);
	else if (!strcmp (criteria+1,"O"))
	  f = mmdf_search_string (mmdf_search_to,&d,&n);
	break;
      case 'U':			/* possible UN* */
	if (criteria[1] == 'N') {
	  if (!strcmp (criteria+2,"ANSWERED")) f = mmdf_search_unanswered;
	  else if (!strcmp (criteria+2,"DELETED")) f = mmdf_search_undeleted;
	  else if (!strcmp (criteria+2,"FLAGGED")) f = mmdf_search_unflagged;
	  else if (!strcmp (criteria+2,"KEYWORD"))
	    f = mmdf_search_flag (mmdf_search_unkeyword,&d);
	  else if (!strcmp (criteria+2,"SEEN")) f = mmdf_search_unseen;
	}
	break;
      default:			/* we will barf below */
	break;
      }
      if (!f) {			/* if can't determine any criteria */
	sprintf (LOCAL->buf,"Unknown search criterion: %.30s",criteria);
	mm_log (LOCAL->buf,ERROR);
	return;
      }
				/* run the search criterion */
      for (i = 1; i <= stream->nmsgs; ++i)
	if (mail_elt (stream,i)->searched && !(*f) (stream,i,d,n))
	  mail_elt (stream,i)->searched = NIL;
    }
				/* report search results to main program */
    for (i = 1; i <= stream->nmsgs; ++i)
      if (mail_elt (stream,i)->searched) mail_searched (stream,i);
  }
}

/* MMDF mail ping mailbox
 * Accepts: MAIL stream
 * Returns: T if stream alive, else NIL
 * No-op for readonly files, since read/writer can expunge it from under us!
 */

long mmdf_ping (stream)
	MAILSTREAM *stream;
{
  char lock[MAILTMPLEN];
  struct stat sbuf;
  int fd;
				/* does he want to give up readwrite? */
  if (stream->readonly && LOCAL->ld) {
    flock (LOCAL->ld,LOCK_UN);	/* yes, release the lock */
    close (LOCAL->ld);		/* close the lock file */
    LOCAL->ld = NIL;		/* no more lock fd */
    unlink (LOCAL->lname);	/* delete it */
    fs_give ((void **) &LOCAL->lname);
  }
				/* make sure it is alright to do this at all */
  if (LOCAL && LOCAL->ld && !stream->lock) {
				/* get current mailbox size */
    stat (LOCAL->name,&sbuf);	/* parse if mailbox changed */
    if ((sbuf.st_size != LOCAL->filesize) &&
	((fd = mmdf_parse (stream,lock,LOCK_SH)) >= 0)) {
				/* unlock mailbox */
      mmdf_unlock (fd,stream,lock);
      mail_unlock (stream);	/* and stream */
    }
  }
  return LOCAL ? T : NIL;	/* return if still alive */
}

/* MMDF mail check mailbox
 * Accepts: MAIL stream
 * No-op for readonly files, since read/writer can expunge it from under us!
 */

void mmdf_check (stream)
	MAILSTREAM *stream;
{
  char lock[MAILTMPLEN];
  int fd;
				/* parse and lock mailbox */
  if (LOCAL && LOCAL->ld && ((fd = mmdf_parse (stream,lock,LOCK_EX)) >= 0)) {
    if (LOCAL->dirty && mmdf_extend (stream,fd,"Unable to update mailbox"))
      mmdf_save (stream,fd);	/* dump checkpoint if needed */
				/* flush locks */
    mmdf_unlock (fd,stream,lock);
    mail_unlock (stream);
  }
  if (LOCAL && LOCAL->ld) mm_log ("Check completed",NIL);
}

/* MMDF mail expunge mailbox
 * Accepts: MAIL stream
 */

void mmdf_expunge (stream)
	MAILSTREAM *stream;
{
  int fd,j;
  long i = 1;
  long n = 0;
  unsigned long recent;
  MESSAGECACHE *elt;
  char *r = "No messages deleted, so no update needed";
  char lock[MAILTMPLEN];
  if (LOCAL && LOCAL->ld) {	/* parse and lock mailbox */
    if ((fd = mmdf_parse (stream,lock,LOCK_EX)) >= 0) {
      recent = stream->recent;	/* get recent now that new ones parsed */
      while ((j = (i<=stream->nmsgs)) && !(elt = mail_elt (stream,i))->deleted)
	i++;			/* find first deleted message */
      if (j) {			/* found one? */
				/* make sure we can do the worst case thing */
	if (mmdf_extend (stream,fd,"Unable to expunge mailbox")) {
	  do {			/* flush deleted messages */
	    if ((elt = mail_elt (stream,i))->deleted) {
				/* if recent, note one less recent message */
	      if (elt->recent) --recent;
				/* flush local cache entry */
	      fs_give ((void **) &LOCAL->msgs[i - 1]);
	      for (j = i; j < stream->nmsgs; j++)
		LOCAL->msgs[j - 1] = LOCAL->msgs[j];
	      LOCAL->msgs[stream->nmsgs - 1] = NIL;
				/* notify upper levels */
	      mail_expunged (stream,i);
	      n++;		/* count another expunged message */
	    }
	    else i++;		/* otherwise try next message */
	  } while (i <= stream->nmsgs);
				/* dump checkpoint of the results */
	  mmdf_save (stream,fd);
	  sprintf ((r = LOCAL->buf),"Expunged %d messages",n);
	}
      }
				/* notify upper level, free locks */
      mail_exists (stream,stream->nmsgs);
      mail_recent (stream,recent);
      mmdf_unlock (fd,stream,lock);
      mail_unlock (stream);
    }
  }
  else r = "Expunge ignored on readonly mailbox";
  if (LOCAL && !stream->silent) mm_log (r,NIL);
}

/* MMDF mail copy message(s)
	s;
 * Accepts: MAIL stream
 *	    sequence
 *	    destination mailbox
 * Returns: T if copy successful, else NIL
 */

long mmdf_copy (stream,sequence,mailbox)
	MAILSTREAM *stream;
	char *sequence;
	char *mailbox;
{
				/* copy the messages */
  return (mail_sequence (stream,sequence)) ?
    mmdf_copy_messages (stream,mailbox) : NIL;
}


/* MMDF mail move message(s)
	s;
 * Accepts: MAIL stream
 *	    sequence
 *	    destination mailbox
 * Returns: T if move successful, else NIL
 */

long mmdf_move (stream,sequence,mailbox)
	MAILSTREAM *stream;
	char *sequence;
	char *mailbox;
{
  long i;
  MESSAGECACHE *elt;
  if (!(mail_sequence (stream,sequence) &&
	mmdf_copy_messages (stream,mailbox))) return NIL;
				/* delete all requested messages */
  for (i = 1; i <= stream->nmsgs; i++)
    if ((elt = mail_elt (stream,i))->sequence) {
      elt->deleted = T;		/* mark message deleted */
				/* recalculate Status/X-Status lines */
      mmdf_update_status (LOCAL->msgs[i - 1]->status,elt);
      LOCAL->dirty = T;		/* note stream is now dirty */
    }
  return T;
}

/* MMDF mail append message from stringstruct
 * Accepts: MAIL stream
 *	    destination mailbox
 *	    stringstruct of messages to append
 * Returns: T if append successful, else NIL
 */

long mmdf_append (stream,mailbox,message)
	MAILSTREAM *stream;
	char *mailbox;
	STRING *message;
{
  struct stat sbuf;
  int fd;
  char c,tmp[MAILTMPLEN],lock[MAILTMPLEN];
  int i = 0;
  char *s = tmp;
  long t = time (0);
  long size = SIZE (message);
  if ((fd = mmdf_lock (mmdf_file (tmp,mailbox),O_WRONLY|O_APPEND|O_CREAT,
			 S_IREAD|S_IWRITE,lock,LOCK_EX)) < 0) {
    sprintf (tmp,"Can't open append mailbox: %s",strerror (errno));
    mm_log (tmp,ERROR);
    return NIL;
  }
  mm_critical (stream);		/* go critical */
  fstat (fd,&sbuf);		/* get current file size */
  sprintf (tmp,"From %s@%s %s",myusername (),lhostn,ctime (&t));
				/* write header */
  if (write (fd,tmp,strlen (tmp)) < 0) {
    sprintf (tmp,"Header write failed: %s",strerror (errno));
    mm_log (tmp,ERROR);
    ftruncate (fd,sbuf.st_size);
  }
  else while (size--) {		/* copy text, tossing out CR's */
    if ((c = SNX (message)) != '\015') s[i++] = c;
				/* dump if filled buffer or no more data */
    if ((!size) || (i == MAILTMPLEN)) {
      if ((write (fd,tmp,i)) < 0) {
	sprintf (tmp,"Message append failed: %s",strerror (errno));
	mm_log (tmp,ERROR);
	ftruncate (fd,sbuf.st_size);
	break;
      }
      i = 0;			/* restart buffer */
				/* write out final newline if at end */
      if (!size) write (fd,"\n",1);
    }
  }
  fsync (fd);			/* force out the update */
  mmdf_unlock (fd,NIL,lock);	/* unlock and close mailbox */
  mm_nocritical (stream);	/* release critical */
  return T;			/* return success */
}

/* MMDF garbage collect stream
 * Accepts: Mail stream
 *	    garbage collection flags
 */

void mmdf_gc (stream,gcflags)
	MAILSTREAM *stream;
	long gcflags;
{
  /* nothing here for now */
}

/* Internal routines */


/* MMDF mail abort stream
 * Accepts: MAIL stream
 */

void mmdf_abort (stream)
	MAILSTREAM *stream;
{
  long i;
  if (LOCAL) {			/* only if a file is open */
    if (LOCAL->name) fs_give ((void **) &LOCAL->name);
    if (LOCAL->ld) {		/* have a mailbox lock? */
      flock (LOCAL->ld,LOCK_UN);/* yes, release the lock */
      close (LOCAL->ld);	/* close the lock file */
      unlink (LOCAL->lname);	/* and delete it */
    }
    fs_give ((void **) &LOCAL->lname);
    if (LOCAL->msgs) {		/* free local cache */
      for (i = 0; i < stream->nmsgs; ++i) fs_give ((void **) &LOCAL->msgs[i]);
      fs_give ((void **) &LOCAL->msgs);
    }
				/* free local text buffers */
    if (LOCAL->buf) fs_give ((void **) &LOCAL->buf);
				/* nuke the local data */
    fs_give ((void **) &stream->local);
    stream->dtb = NIL;		/* log out the DTB */
  }
}


/* MMDF mail generate file string
 * Accepts: temporary buffer to write into
 *	    mailbox name string
 * Returns: local file string
 */


char *mmdf_file (dst,name)
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
  default:			/* other name */
    if (strcmp (ucase (strcpy (dst,name)),"INBOX"))
      sprintf (dst,"%s/%s",myhomedir (),name);
				/* INBOX becomes mail spool directory file */
    else sprintf (dst,MAILFILE,myusername ());
  }
  return dst;
}

/* MMDF open and lock mailbox
 * Accepts: file name to open/lock
 *	    file open mode
 *	    destination buffer for lock file name
 *	    type of locking operation (LOCK_SH or LOCK_EX)
 */

int mmdf_lock (file,flags,mode,lock,op)
	char *file;
	int flags;
	int mode;
	char *lock;
	int op;
{
  int fd,ld,j;
  int i = LOCKTIMEOUT * 60 - 1;
  char tmp[MAILTMPLEN];
  struct timeval tp;
  struct stat sb;
				/* build lock filename */
  strcat (mmdf_file (lock,file),".lock");
  do {				/* until OK or out of tries */
    gettimeofday (&tp,NIL);	/* get the time now */
#ifdef NFSKLUDGE
  /* SUN-OS had an NFS, As kludgy as an albatross;
   * And everywhere that it was installed, It was a total loss.  -- MRC 9/25/91
   */
				/* build hitching post file name */
    sprintf (tmp,"%s.%d.%d.",lock,time (0),getpid ());
    j = strlen (tmp);		/* append local host name */
    gethostname (tmp + j,(MAILTMPLEN - j) - 1);
				/* try to get hitching-post file */
    if ((ld = open (tmp,O_WRONLY|O_CREAT|O_EXCL,0666)) < 0) {
				/* prot fail & non-ex, don't use lock files */
      if ((errno == EACCES) && (stat (tmp,&sb))) *lock = '\0';
      else {			/* otherwise something strange is going on */
	sprintf (tmp,"Error creating %s: %s",lock,strerror (errno));
	mm_log (tmp,WARN);	/* this is probably not good */
				/* don't use lock files if not one of these */
	if ((errno != EEXIST) && (errno != EACCES)) *lock = '\0';
      }
    }
    else {			/* got a hitching-post */
      chmod (tmp,0666);		/* make sure others can break the lock */
      close (ld);		/* close the hitching-post */
      link (tmp,lock);		/* tie hitching-post to lock, ignore failure */
      stat (tmp,&sb);		/* get its data */
      unlink (tmp);		/* flush hitching post */
      /* If link count .ne. 2, hitch failed.  Set ld to -1 as if open() failed
	 so we try again.  If extant lock file and time now is .gt. file time
	 plus timeout interval, flush the lock so can win next time around. */
      if ((ld = (sb.st_nlink != 2) ? -1 : 0) && (!stat (lock,&sb)) &&
	  (tp.tv_sec > sb.st_ctime + LOCKTIMEOUT * 60)) unlink (lock);
    }

#else
  /* This works on modern Unix systems which are not afflicted with NFS mail.
   * "Modern" means that O_EXCL works.  I think that NFS mail is a terrible
   * idea -- that's what IMAP is for -- but some people insist upon losing...
   */
				/* try to get the lock */
    if ((ld = open (lock,O_WRONLY|O_CREAT|O_EXCL,0666)) < 0) switch (errno) {
    case EEXIST:		/* if extant and old, grab it for ourselves */
      if ((!stat (lock,&sb)) && tp.tv_sec > sb.st_ctime + LOCKTIMEOUT * 60)
	ld = open (lock,O_WRONLY|O_CREAT,0666);
      break;
    case EACCES:		/* protection fail, ignore if non-ex or old */
      if (stat (lock,&sb) || (tp.tv_sec > sb.st_ctime + LOCKTIMEOUT * 60))
	*lock = '\0';		/* assume no world write mail spool dir */
      break;
    default:			/* some other failure */
      sprintf (tmp,"Error creating %s: %s",lock,strerror (errno));
      mm_log (tmp,WARN);	/* this is probably not good */
      *lock = '\0';		/* don't use lock files */
      break;
    }
    if (ld >= 0) {		/* if made a lock file */
      chmod (tmp,0666);		/* make sure others can break the lock */
      close (ld);		/* close the lock file */
    }
#endif
    if ((ld < 0) && *lock) {	/* if failed to make lock file and retry OK */
      if (!(i%15)) {
	sprintf (tmp,"Mailbox %s is locked, will override in %d seconds...",
		 file,i);
	mm_log (tmp,WARN);
      }
      sleep (1);		/* wait 1 second before next try */
    }
  } while (i-- && ld < 0 && *lock);
				/* open file */
  if ((fd = open (file,flags,mode)) >= 0) flock (fd,op);
  else {			/* open failed */
    j = errno;			/* preserve error code */
    if (*lock) unlink (lock);	/* flush the lock file if any */
    errno = j;			/* restore error code */
  }
  return fd;
}

/* MMDF unlock and close mailbox
 * Accepts: file descriptor
 *	    (optional) mailbox stream to check atime/mtime
 *	    (optional) lock file name
 */

void mmdf_unlock (fd,stream,lock)
	int fd;
	MAILSTREAM *stream;
	char *lock;
{
  struct stat sbuf;
  struct timeval tp[2];
  fstat (fd,&sbuf);		/* get file times */
				/* if stream and csh would think new mail */
  if (stream && (sbuf.st_atime <= sbuf.st_mtime)) {
    gettimeofday (&tp[0],NIL);	/* set atime to now */
				/* set mtime to (now - 1) if necessary */
    tp[1].tv_sec = tp[0].tv_sec > sbuf.st_mtime ? sbuf.st_mtime :
      tp[0].tv_sec - 1;
    tp[1].tv_usec = 0;		/* oh well */
				/* set the times, note change */
    if (!utimes (LOCAL->name,tp)) LOCAL->filetime = tp[1].tv_sec;
  }
  flock (fd,LOCK_UN);		/* release flock'ers */
  close (fd);			/* close the file */
				/* flush the lock file if any */
  if (lock && *lock) unlink (lock);
}

/* MMDF mail parse and lock mailbox
 * Accepts: MAIL stream
 *	    space to write lock file name
 *	    type of locking operation
 * Returns: file descriptor if parse OK, mailbox is locked shared
 *	    -1 if failure, stream aborted
 */

int mmdf_parse (stream,lock,op)
	MAILSTREAM *stream;
	char *lock;
	int op;
{
  int fd;
  long delta,i,j,is,is1;
  char *s,*s1,*t,*e;
  int ti,zn;
  int first = T;
  long nmsgs = stream->nmsgs;
  long recent = stream->recent;
  struct stat sbuf;
  MESSAGECACHE *elt;
  FILECACHE *m = NIL,*n;
  mail_lock (stream);		/* guard against recursion or pingers */
				/* open and lock mailbox (shared OK) */
  if ((fd = mmdf_lock (LOCAL->name,LOCAL->ld ? O_RDWR : O_RDONLY,NIL,
			 lock,op)) < 0) {
				/* failed, OK for non-ex file? */
    if ((errno != ENOENT) || LOCAL->filesize) {
      sprintf (LOCAL->buf,"Mailbox open failed, aborted: %s",strerror (errno));
      mm_log (LOCAL->buf,ERROR);
      mmdf_abort (stream);
    }
    else {			/* this is to allow for non-ex INBOX */
      mail_exists (stream,0);	/* make sure upper level sees this as empty */
      mail_recent (stream,0);
    }
    mail_unlock (stream);
    return -1;
  }
  fstat (fd,&sbuf);		/* get status */
				/* calculate change in size */
  if ((delta = sbuf.st_size - LOCAL->filesize) < 0) {
    sprintf (LOCAL->buf,"Mailbox shrank from %d to %d bytes, aborted",
	     LOCAL->filesize,sbuf.st_size);
    mm_log (LOCAL->buf,ERROR);	/* this is pretty bad */
    mmdf_unlock (fd,stream,lock);
    mmdf_abort (stream);
    mail_unlock (stream);
    return -1;
  }

  else if (delta) {		/* get to that position in the file */
    lseek (fd,LOCAL->filesize,L_SET);
    s = s1 = LOCAL->buf;	/* initial read-in location */
    i = 0;			/* initial unparsed read-in count */
    do {
      i = min (CHUNK,delta);	/* calculate read-in size */
				/* increase the read-in buffer if necessary */
      if ((j = i + (s1 - s)) >= LOCAL->buflen) {
	is = s - LOCAL->buf;	/* note former start of message position */
	is1 = s1 - LOCAL->buf;	/* and start of new data position */
	if (s1 - s) fs_resize ((void **) &LOCAL->buf,(LOCAL->buflen = j) + 1);
	else {			/* fs_resize would do an unnecessary copy */
	  fs_give ((void **) &LOCAL->buf);
	  LOCAL->buf = (char *) fs_get ((LOCAL->buflen = j) + 1);
	}
	s = LOCAL->buf + is;	/* new start of message */
	s1 = LOCAL->buf + is1;	/* new start of new data */
      }
      s1[i] = '\0';		/* tie off chunk */
      if (read (fd,s1,i) < 0) {	/* read a chunk of new text */
	sprintf (LOCAL->buf,"Error reading mail file: %s",strerror (errno));
	mm_log (LOCAL->buf,ERROR);
	mmdf_unlock (fd,stream,lock);
	mmdf_abort (stream);
	mail_unlock (stream);
	return -1;
      }
      delta -= i;		/* account for data read in */
				/* validate newly-appended data */
      if (first) {		/* only do this first time! */
	if (!((*s ==(char)0x01) && VALID (s,t,ti,zn))) {
	  mm_log ("Mailbox format invalidated (consult an expert), aborted",
		  ERROR);
	  mmdf_unlock (fd,stream,lock);
	  mmdf_abort (stream);
	  mail_unlock (stream);
	  return -1;
	}
	first = NIL;		/* don't do this again */
      }

				/* found end of message or end of data? */
      while ((e = mmdf_eom (s,s1,i)) || !delta) {
	nmsgs++;		/* yes, have a new message */
	if (e) j = (e - s) - 1;	/* calculate message length */
	else j = strlen (s) - 1;/* otherwise is remainder of data */
	if (m) {		/* new cache needed, have previous data? */
	  n->header = (char *) fs_get (sizeof (FILECACHE) + j + 1);
	  n = (FILECACHE *) n->header;
	}
	else m = n = (FILECACHE *) fs_get (sizeof (FILECACHE) + j + 1);
				/* copy message data */
	strncpy (n->internal,s,j);
	n->internal[j] = '\0';
	n->header = NIL;	/* initially no link */
	n->headersize = j;	/* stash away buffer length */
	if (e) {		/* saw end of message? */
	  i -= e - s1;		/* new unparsed data count */
	  s = s1 = e;		/* advance to new message */
	}
	else break;		/* else punt this loop */
      }
      if (delta) {		/* end of message not found? */
	s1 += i;		/* end of unparsed data */
	if (s != LOCAL->buf){	/* message doesn't begin at buffer? */
	  i = s1 - s;		/* length of message so far */
	  memmove (LOCAL->buf,s,i);
	  s = LOCAL->buf;	/* message now starts at buffer origin */
	  s1 = s + i;		/* calculate new end of unparsed data */
	}
      }
    } while (delta);		/* until nothing more new to read */
  }
  else {			/* no change, don't babble if never got time */
    if (LOCAL->filetime && LOCAL->filetime != sbuf.st_mtime)
      mm_log ("New mailbox modification time but apparently no changes",WARN);
  }
				/* expand the primary cache */
  (*mailcache) (stream,nmsgs,CH_SIZE);
  if (LOCAL->msgs) {		/* have a cache yet? */
    if (nmsgs >= LOCAL->cachesize)
      fs_resize ((void **) &LOCAL->msgs,
		 (LOCAL->cachesize += CACHEINCREMENT) * sizeof (FILECACHE *));
  }
  else LOCAL->msgs = (FILECACHE **)
    fs_get ((LOCAL->cachesize = nmsgs + CACHEINCREMENT) *sizeof (FILECACHE *));

  if (LOCAL->buflen > CHUNK) {	/* maybe move where the buffer is in memory*/
    fs_give ((void **) &LOCAL->buf);
    LOCAL->buf = (char *) fs_get ((LOCAL->buflen = CHUNK) + 1);
  }
  for (i = stream->nmsgs, n = m; i < nmsgs; i++) {
    LOCAL->msgs[i] = m = n;	/* set cache, and next cache pointer */
    n = (FILECACHE *) n->header;
    /* This is a bugtrap for bogons in the new message cache, which may happen
     * if memory is corrupted.  Note that in the case of a totally empty
     * message, a newline is appended and counts adjusted.
     */
    if ((!((s = m->internal) && VALID (s,t,ti,zn))) &&
	!(s && !strchr (s,'\n') && strcat (s,"\n") && VALID (s,t,ti,zn) &&
	  m->headersize++)) fatal ("Bogus entry in new cache list");
				/* pointer to message header */
    m->header = s = strchr (t++,'\n') + 1;
    m->headersize -= m->header - m->internal;
    m->body = NIL;		/* assume no body as yet */
    m->bodysize = 0;
    recent++;			/* assume recent by default */
    (elt = mail_elt (stream,i+1))->recent = T;
				/* calculate initial Status/X-Status lines */
    mmdf_update_status (m->status,elt);
				/* generate plausable IMAPish date string */
    LOCAL->buf[2] = LOCAL->buf[6] = LOCAL->buf[20] = '-';
    LOCAL->buf[11] = ' ';
    LOCAL->buf[14] = LOCAL->buf[17] = ':';
				/* dd */
    LOCAL->buf[0] = t[ti - 3]; LOCAL->buf[1] = t[ti - 2];
				/* mmm */
    LOCAL->buf[3] = t[ti - 7]; LOCAL->buf[4] = t[ti - 6];
    LOCAL->buf[5] = t[ti - 5];
				/* hh */
    LOCAL->buf[12] = t[ti]; LOCAL->buf[13] = t[ti + 1];
				/* mm */
    LOCAL->buf[15] = t[ti + 3]; LOCAL->buf[16] = t[ti + 4];
    if (t[ti += 5] == ':') {	/* ss if present */
      LOCAL->buf[18] = t[++ti];
      LOCAL->buf[19] = t[++ti];
      ti++;			/* move to space */
    }
    else LOCAL->buf[18] = LOCAL->buf[19] = '0';
				/* yy -- advance over timezone if necessary */
    if (zn == ++ti) ti += (((t[zn] == '+') || (t[zn] == '-')) ? 6 : 4);
    LOCAL->buf[7] = t[ti]; LOCAL->buf[8] = t[ti + 1];
    LOCAL->buf[9] = t[ti + 2]; LOCAL->buf[10] = t[ti + 3];
				/* zzz */
    t = zn ? (t + zn) : "LCL";
    LOCAL->buf[21] = *t++; LOCAL->buf[22] = *t++; LOCAL->buf[23] = *t++;
    if ((LOCAL->buf[21] != '+') && (LOCAL->buf[21] != '-'))
      LOCAL->buf[24] = '\0';
    else {			/* numeric time zone */
      LOCAL->buf[24] = *t++; LOCAL->buf[25] = *t++;
      LOCAL->buf[26] = '\0'; LOCAL->buf[20] = ' ';
    }
				/* set internal date */
    if (!mail_parse_date (elt,LOCAL->buf)) mm_log ("Unparsable date",WARN);

    is = 0;			/* initialize newline count */
    e = NIL;			/* no status stuff yet */
    do switch (*(t = s)) {	/* look at header lines */
    case 'X':			/* possible X-Status: line */
      if (s[1] == '-' && s[2] == 'S') s += 2;
      else {
	is++;			/* count another newline */
	break;			/* this is uninteresting after all */
      }
    case 'S':			/* possible Status: line */
      if (s[1] == 't' && s[2] == 'a' && s[3] == 't' && s[4] == 'u' &&
	  s[5] == 's' && s[6] == ':') {
	if (!e) e = t;		/* note deletion point */
	s += 6;			/* advance to status flags */
	do switch (*s++) {	/* parse flags */
	case 'R':		/* message read */
	  elt->seen = T;
	  break;
	case 'O':		/* message old */
	  if (elt->recent) {	/* don't do this more than once! */
	    elt->recent = NIL;
	    recent--;		/* not recent any longer... */
	  }
	  break;
	case 'D':		/* message deleted */
	  elt->deleted = T;
	  break;
	case 'F':		/* message flagged */
	  elt ->flagged = T;
	  break;
	case 'A':		/* message answered */
	  elt ->answered = T;
	  break;
	default:		/* some other crap */
	  break;
	} while (*s && *s != '\n');
				/* recalculate Status/X-Status lines */
	mmdf_update_status (m->status,elt);
      }
      else is++;		/* otherwise random line */
      break;

    case '\n':			/* end of header */
      m->body = ++s;		/* start of body is here */
      j = m->body - m->header;	/* new header size */
				/* calculate body size */
      m->bodysize = m->headersize - j;
      if (e) {			/* saw status poop? */
	*e++ = '\n';		/* patch in trailing newline */
	m->headersize = e - m->header;
      }
      else m->headersize = j;	/* set header size */
      s = NIL;			/* don't scan any further */
      is++;			/* count a newline */
      break;
    case '\0':			/* end of message */
      if (e) {			/* saw status poop? */
	*e++ = '\n';		/* patch in trailing newline */
	m->headersize = e - m->header;
      }
      is++;			/* count an extra newline here */
      break;
    default:			/* anything else is uninteresting */
      if (e) {			/* have status stuff to worry about? */
	j = s - e;		/* yuck!!  calculate size of delete area */
				/* blat remaining number of bytes down */
	memmove (e,s,m->header + m->headersize - s);
	m->headersize -= j;	/* update for new size */
	e = NIL;		/* no more delete area */
				/* tie off old cruft */
	*(m->header + m->headersize) = '\0';
      }
      is++;
      break;
    } while (s && (s = strchr (s,'\n')) && s++);
				/* count newlines in body */
    if (s = m->body) while (*s) if (*s++ == '\n') is++;
    elt->rfc822_size = m->headersize + m->bodysize + is;
  }
  if (n) fatal ("Cache link-list inconsistency");
  while (i < LOCAL->cachesize) LOCAL->msgs[i++] = NIL;
				/* update parsed file size and time */
  LOCAL->filesize = sbuf.st_size;
  LOCAL->filetime = sbuf.st_mtime;
  mail_exists (stream,nmsgs);	/* notify upper level of new mailbox size */
  mail_recent (stream,recent);	/* and of change in recent messages */
  if (recent) LOCAL->dirty = T;	/* mark dirty so O flags are set */
  return fd;			/* return the winnage */
}

/* MMDF search for end of message
 * Accepts: start of message
 *	    start of new data
 *	    size of new data
 * Returns: pointer to start of new message if one found
 */

#define Word unsigned long

char *mmdf_eom (som,sod,i)
	char *som;
	char *sod;
	long i;
{
  char *s = sod;		/* find start of line in current message */
  char *t;
  int ti,zn;
  union {
    unsigned long wd;
    char ch[9];
  } wdtest;
  strcpy (wdtest.ch,"AAAA1234");/* constant for word testing */
  while ((s > som) && *s-- != '\n');
  if (wdtest.wd != 0x41414141) {/* not a 32-bit word machine? */
    while (s = strstr (s,"\nFrom ")) if (s++ && VALID (s,t,ti,zn)) return s;
  }
  else {			/* can do it faster this way */
    register Word m = 0x0a0a0a0a;
    while ((long) s & 3)	/* any characters before word boundary? */
      if ((*s++ == '\n') && s[0] == (char)0x01 && VALID (s,t,ti,zn)) return s;
    i = (sod + i) - s;		/* total number of tries */
    do {			/* fast search for newline */
      if (0x80808080 & (0x01010101 + (0x7f7f7f7f & ~(m ^ *(Word *) s)))) {
				/* interesting word, check it closer */
	if (*s++ == '\n' && s[0] == (char)0x01 && VALID (s,t,ti,zn)) return s;
	else if (*s++ == '\n' && s[0] == (char)0x01 && VALID (s,t,ti,zn)) return s;
	else if (*s++ == '\n' && s[0] == (char)0x01 && VALID (s,t,ti,zn)) return s;
	else if (*s++ == '\n' && s[0] == (char)0x01 && VALID (s,t,ti,zn)) return s;
      }
      else s += 4;		/* try next word */
      i -= 4;			/* count a word checked */
    } while (i > 24);		/* continue until end of plausible string */
  }
  return NIL;
}

/* MMDF extend mailbox to reserve worst-case space for expansion
 * Accepts: MAIL stream
 *	    file descriptor
 *	    error string
 * Returns: T if extend OK and have gone critical, NIL if should abort
 */

int mmdf_extend (stream,fd,error)
	MAILSTREAM *stream;
	int fd;
	char *error;
{
  struct stat sbuf;
  FILECACHE *m;
  char tmp[MAILTMPLEN];
  int i,ok;
  long f;
  char *s;
  int retry;
				/* calculate estimated size of mailbox */
  for (i = 0,f = 0; i < stream->nmsgs; i++) {
    m = LOCAL->msgs[i];		/* get cache pointer */
    f += (m->header - m->internal) + m->headersize + sizeof (STATUS) +
      m->bodysize + 1;		/* update guesstimate */
    }
  mm_critical (stream);		/* go critical */
				/* return now if file large enough */
  if (f <= LOCAL->filesize) return T;
  s = (char *) fs_get (f -= LOCAL->filesize);
  memset (s,0,f);		/* get a block of nulls */
				/* get to end of file */
  lseek (fd,LOCAL->filesize,L_SET);
  do {
    retry = NIL;		/* no retry yet */
    if (!(ok = (write (fd,s,f) >= 0))) {
      i = errno;		/* note error before doing ftruncate */
				/* restore prior file size */
      ftruncate (fd,LOCAL->filesize);
      fsync (fd);		/* is this necessary? */
      fstat (fd,&sbuf);		/* now get updated file time */
      LOCAL->filetime = sbuf.st_mtime;
				/* punt if that's what main program wants */
      if (mm_diskerror (stream,i,NIL)) {
	mm_nocritical (stream);	/* exit critical */
	sprintf (tmp,"%s: %s",error,strerror (i));
	mm_notify (stream,tmp,WARN);
      }
      else retry = T;		/* set to retry */
    }
  } while (retry);		/* repeat if need to try again */
  fs_give ((void **) &s);	/* flush buffer of nulls */
  return ok;			/* return status */
}

/* MMDF save mailbox
 * Accepts: MAIL stream
 *	    mailbox file descriptor
 *
 * Mailbox must be readwrite and locked for exclusive access.
 */

void mmdf_save (stream,fd)
	MAILSTREAM *stream;
	int fd;
{
  struct stat sbuf;
  struct iovec iov[16];
  int iovc;
  long i;
  int e;
  int retry;
  do {				/* restart point if failure */
    retry = NIL;		/* no need to retry yet */
				/* start at beginning of file */
    lseek (fd,LOCAL->filesize = 0,L_SET);
				/* loop through all messages */
    for (i = 1,iovc = 0; i <= stream->nmsgs; i++) {
				/* set up iov's for this message */
      mmdf_write_message (iov,&iovc,LOCAL->msgs[i-1]);
				/* filled up iovec or end of messages? */
      if ((iovc == 16) || (i == stream->nmsgs)) {
				/* write messages */
	if ((e = writev (fd,iov,iovc)) < 0) {
	  sprintf (LOCAL->buf,"Unable to rewrite mailbox: %s",
		   strerror (e = errno));
	  mm_log (LOCAL->buf,WARN);
	  mm_diskerror (stream,e,T);
	  retry = T;		/* must retry */
	  break;		/* abort this particular try */
	}
	else {			/* won */
	  iovc = 0;		/* restart iovec */
	  LOCAL->filesize += e;	/* count these bytes in data */
	}
      }
    }
  } while (retry);		/* repeat if need to try again */
  fsync (fd);			/* make sure the disk has the update */
				/* nuke any cruft after that */
  ftruncate (fd,LOCAL->filesize);
  fsync (fd);			/* is this necessary? */
  fstat (fd,&sbuf);		/* now get updated file time */
  LOCAL->filetime = sbuf.st_mtime;
  LOCAL->dirty = NIL;		/* stream no longer dirty */
  mm_nocritical (stream);	/* exit critical */
}

/* MMDF copy messages
 * Accepts: MAIL stream
 *	    mailbox name
 * Returns: T if copy successful else NIL
 */

int mmdf_copy_messages (stream,mailbox)
	MAILSTREAM *stream;
	char *mailbox;
{
  char file[MAILTMPLEN];
  char lock[MAILTMPLEN];
  struct iovec iov[16];
  int fd,iovc;
  struct stat sbuf;
  long i;
  int ok = T;
				/* make sure valid mailbox */
  if (!mmdf_isvalid (mailbox,file)) {
    if (errno == ENOENT)	/* failed, was it no such file? */
      mm_notify (stream,"[TRYCREATE] Must create mailbox before append",NIL);
    else {
      sprintf (LOCAL->buf,"Not a MMDF-format mailbox: %s",mailbox);
      mm_log (LOCAL->buf,ERROR);
    }
    return NIL;
  }
  if ((fd = mmdf_lock (mmdf_file (file,mailbox),O_WRONLY|O_APPEND|O_CREAT,
			 S_IREAD|S_IWRITE,lock,LOCK_EX)) < 0) {
    sprintf (LOCAL->buf,"Can't open destination mailbox: %s",strerror (errno));
    mm_log (LOCAL->buf,ERROR);
    return NIL;
  }
  mm_critical (stream);		/* go critical */
  fstat (fd,&sbuf);		/* get current file size */
				/* write all requested messages to mailbox */
  for (i = 1,iovc = 0; ok && i <= stream->nmsgs; i++) {
				/* set up iov's if message selected */
    if (mail_elt (stream,i)->sequence)
      mmdf_write_message (iov,&iovc,LOCAL->msgs[i - 1]);
				/* filled up iovec or end of messages? */
    if (iovc && ((iovc == 16) || (i == stream->nmsgs))) {
      if (ok = (writev (fd,iov,iovc) >= 0)) iovc = 0;
      else {
	sprintf (LOCAL->buf,"Message copy failed: %s",strerror (errno));
	mm_log (LOCAL->buf,ERROR);
	ftruncate (fd,sbuf.st_size);
	break;
      }
    }
  }
  fsync (fd);			/* force out the update */
  mmdf_unlock (fd,NIL,lock);	/* unlock and close mailbox */
  mm_nocritical (stream);	/* release critical */
  return ok;			/* return whether or not succeeded */
}

/* MMDF write message to mailbox
 * Accepts: I/O vector
 *	    I/O vector index
 *	    local cache for this message
 *
 * This routine is the reason why the local cache has a copy of the status.
 * We can be called to dump out the mailbox as part of a stream recycle, since
 * we don't write out the mailbox when flags change and hence an update may be
 * needed.  However, at this point the elt has already become history, so we
 * can't use any information other than what is local to us.
 */

void mmdf_write_message (iov,i,m)
	struct iovec iov[];
	int *i;
	FILECACHE *m;
{
  iov[*i].iov_base =m->internal;/* pointer/counter to headers */
				/* length of internal + message headers */
  iov[*i].iov_len = (m->header + m->headersize) - m->internal;
				/* suppress extra newline if present */
  if ((iov[*i].iov_base)[iov[*i].iov_len - 2] == '\n') iov[(*i)++].iov_len--;
  else (*i)++;			/* unlikely but... */
  iov[*i].iov_base = m->status;	/* pointer/counter to status */
  iov[(*i)++].iov_len = strlen (m->status);
  iov[*i].iov_base = m->body;	/* pointer/counter to text body */
  iov[(*i)++].iov_len = m->bodysize;
  iov[*i].iov_base = "\n";	/* pointer/counter to extra newline */
  iov[(*i)++].iov_len = 1;
}

/* MMDF update status string
 * Accepts: destination string to write
 *	    message cache entry
 */

void mmdf_update_status (status,elt)
	char *status;
	MESSAGECACHE *elt;
{
  /* This used to be an sprintf(), but thanks to certain cretinous C libraries
     with horribly slow implementations of sprintf() I had to change it to this
     mess.  At least it should be fast. */
  char *t = status + 8;
  status[0] = 'S'; status[1] = 't'; status[2] = 'a'; status[3] = 't';
  status[4] = 'u'; status[5] = 's'; status[6] = ':';  status[7] = ' ';
  if (elt->seen) *t++ = 'R'; *t++ = 'O'; *t++ = '\n';
  *t++ = 'X'; *t++ = '-'; *t++ = 'S'; *t++ = 't'; *t++ = 'a'; *t++ = 't';
  *t++ = 'u'; *t++ = 's'; *t++ = ':'; *t++ = ' ';
  if (elt->deleted) *t++ = 'D'; if (elt->flagged) *t++ = 'F';
  if (elt->answered) *t++ = 'A';
  *t++ = '\n'; *t++ = '\n'; *t++ = '\0';
}

/* Parse flag list
 * Accepts: MAIL stream
 *	    flag list as a character string
 * Returns: flag command list
 */


short mmdf_getflags (stream,flag)
	MAILSTREAM *stream;
	char *flag;
{
  char *t;
  short f = 0;
  short i,j;
  if (flag && *flag) {		/* no-op if no flag string */
				/* check if a list and make sure valid */
    if ((i = (*flag == '(')) ^ (flag[strlen (flag)-1] == ')')) {
      mm_log ("Bad flag list",ERROR);
      return NIL;
    }
				/* copy the flag string w/o list construct */
    strncpy (LOCAL->buf,flag+i,(j = strlen (flag) - (2*i)));
    LOCAL->buf[j] = '\0';
    t = ucase (LOCAL->buf);	/* uppercase only from now on */

    while (*t) {		/* parse the flags */
      if (*t == '\\') {		/* system flag? */
	switch (*++t) {		/* dispatch based on first character */
	case 'S':		/* possible \Seen flag */
	  if (t[1] == 'E' && t[2] == 'E' && t[3] == 'N') i = fSEEN;
	  t += 4;		/* skip past flag name */
	  break;
	case 'D':		/* possible \Deleted flag */
	  if (t[1] == 'E' && t[2] == 'L' && t[3] == 'E' && t[4] == 'T' &&
	      t[5] == 'E' && t[6] == 'D') i = fDELETED;
	  t += 7;		/* skip past flag name */
	  break;
	case 'F':		/* possible \Flagged flag */
	  if (t[1] == 'L' && t[2] == 'A' && t[3] == 'G' && t[4] == 'G' &&
	      t[5] == 'E' && t[6] == 'D') i = fFLAGGED;
	  t += 7;		/* skip past flag name */
	  break;
	case 'A':		/* possible \Answered flag */
	  if (t[1] == 'N' && t[2] == 'S' && t[3] == 'W' && t[4] == 'E' &&
	      t[5] == 'R' && t[6] == 'E' && t[7] == 'D') i = fANSWERED;
	  t += 8;		/* skip past flag name */
	  break;
	default:		/* unknown */
	  i = 0;
	  break;
	}
				/* add flag to flags list */
	if (i && ((*t == '\0') || (*t++ == ' '))) f |= i;
	else {			/* bitch about bogus flag */
	  mm_log ("Unknown system flag",ERROR);
	  return NIL;
	}
      }
      else {			/* no user flags yet */
	mm_log ("Unknown flag",ERROR);
	return NIL;
      }
    }
  }
  return f;
}

/* Search support routines
 * Accepts: MAIL stream
 *	    message number
 *	    pointer to additional data
 * Returns: T if search matches, else NIL
 */


char mmdf_search_all (stream,msgno,d,n)
	MAILSTREAM *stream;
	long msgno;
	char *d;
	long n;
{
  return T;			/* ALL always succeeds */
}


char mmdf_search_answered (stream,msgno,d,n)
	MAILSTREAM *stream;
	long msgno;
	char *d;
	long n;
{
  return mail_elt (stream,msgno)->answered ? T : NIL;
}


char mmdf_search_deleted (stream,msgno,d,n)
	MAILSTREAM *stream;
	long msgno;
	char *d;
	long n;
{
  return mail_elt (stream,msgno)->deleted ? T : NIL;
}


char mmdf_search_flagged (stream,msgno,d,n)
	MAILSTREAM *stream;
	long msgno;
	char *d;
	long n;
{
  return mail_elt (stream,msgno)->flagged ? T : NIL;
}


char mmdf_search_keyword (stream,msgno,d,n)
	MAILSTREAM *stream;
	long msgno;
	char *d;
	long n;
{
  return NIL;			/* keywords not supported yet */
}


char mmdf_search_new (stream,msgno,d,n)
	MAILSTREAM *stream;
	long msgno;
	char *d;
	long n;
{
  MESSAGECACHE *elt = mail_elt (stream,msgno);
  return (elt->recent && !elt->seen) ? T : NIL;
}

char mmdf_search_old (stream,msgno,d,n)
	MAILSTREAM *stream;
	long msgno;
	char *d;
	long n;
{
  return mail_elt (stream,msgno)->recent ? NIL : T;
}


char mmdf_search_recent (stream,msgno,d,n)
	MAILSTREAM *stream;
	long msgno;
	char *d;
	long n;
{
  return mail_elt (stream,msgno)->recent ? T : NIL;
}


char mmdf_search_seen (stream,msgno,d,n)
	MAILSTREAM *stream;
	long msgno;
	char *d;
	long n;
{
  return mail_elt (stream,msgno)->seen ? T : NIL;
}


char mmdf_search_unanswered (stream,msgno,d,n)
	MAILSTREAM *stream;
	long msgno;
	char *d;
	long n;
{
  return mail_elt (stream,msgno)->answered ? NIL : T;
}


char mmdf_search_undeleted (stream,msgno,d,n)
	MAILSTREAM *stream;
	long msgno;
	char *d;
	long n;
{
  return mail_elt (stream,msgno)->deleted ? NIL : T;
}


char mmdf_search_unflagged (stream,msgno,d,n)
	MAILSTREAM *stream;
	long msgno;
	char *d;
	long n;
{
  return mail_elt (stream,msgno)->flagged ? NIL : T;
}


char mmdf_search_unkeyword (stream,msgno,d,n)
	MAILSTREAM *stream;
	long msgno;
	char *d;
	long n;
{
  return T;			/* keywords not supported yet */
}


char mmdf_search_unseen (stream,msgno,d,n)
	MAILSTREAM *stream;
	long msgno;
	char *d;
	long n;
{
  return mail_elt (stream,msgno)->seen ? NIL : T;
}

char mmdf_search_before (stream,msgno,d,n)
	MAILSTREAM *stream;
	long msgno;
	char *d;
	long n;
{
  MESSAGECACHE *elt = mail_elt (stream,msgno);
  return (char) ((long)((elt->year << 9) + (elt->month << 5) + elt->day) < n);
}


char mmdf_search_on (stream,msgno,d,n)
	MAILSTREAM *stream;
	long msgno;
	char *d;
	long n;
{
  MESSAGECACHE *elt = mail_elt (stream,msgno);
  return (char) (((elt->year << 9) + (elt->month << 5) + elt->day) == n);
}


char mmdf_search_since (stream,msgno,d,n)
	MAILSTREAM *stream;
	long msgno;
	char *d;
	long n;
{
				/* everybody interprets "since" as .GE. */
  MESSAGECACHE *elt = mail_elt (stream,msgno);
  return (char) ((long)((elt->year << 9) + (elt->month << 5) + elt->day) >= n);
}


char mmdf_search_body (stream,msgno,d,n)
	MAILSTREAM *stream;
	long msgno;
	char *d;
	long n;
{
  FILECACHE *m = LOCAL->msgs[msgno - 1];
  return search (m->body,m->bodysize,d,n);
}


char mmdf_search_subject (stream,msgno,d,n)
	MAILSTREAM *stream;
	long msgno;
	char *d;
	long n;
{
  char *s = mmdf_fetchstructure (stream,msgno,NIL)->subject;
  return s ? search (s,strlen (s),d,n) : NIL;
}


char mmdf_search_text (stream,msgno,d,n)
	MAILSTREAM *stream;
	long msgno;
	char *d;
	long n;
{
  FILECACHE *m = LOCAL->msgs[msgno - 1];
  return search (m->header,m->headersize,d,n) ||
    mmdf_search_body (stream,msgno,d,n);
}

char mmdf_search_bcc (stream,msgno,d,n)
	MAILSTREAM *stream;
	long msgno;
	char *d;
	long n;
{
  ADDRESS *a = mmdf_fetchstructure (stream,msgno,NIL)->bcc;
  LOCAL->buf[0] = '\0';		/* initially empty string */
				/* get text for address */
  rfc822_write_address (LOCAL->buf,a);
  return search (LOCAL->buf,(long) strlen (LOCAL->buf),d,n);
}


char mmdf_search_cc (stream,msgno,d,n)
	MAILSTREAM *stream;
	long msgno;
	char *d;
	long n;
{
  ADDRESS *a = mmdf_fetchstructure (stream,msgno,NIL)->cc;
  LOCAL->buf[0] = '\0';		/* initially empty string */
				/* get text for address */
  rfc822_write_address (LOCAL->buf,a);
  return search (LOCAL->buf,(long) strlen (LOCAL->buf),d,n);
}


char mmdf_search_from (stream,msgno,d,n)
	MAILSTREAM *stream;
	long msgno;
	char *d;
	long n;
{
  ADDRESS *a = mmdf_fetchstructure (stream,msgno,NIL)->from;
  LOCAL->buf[0] = '\0';		/* initially empty string */
				/* get text for address */
  rfc822_write_address (LOCAL->buf,a);
  return search (LOCAL->buf,(long) strlen (LOCAL->buf),d,n);
}


char mmdf_search_to (stream,msgno,d,n)
	MAILSTREAM *stream;
	long msgno;
	char *d;
	long n;
{
  ADDRESS *a = mmdf_fetchstructure (stream,msgno,NIL)->to;
  LOCAL->buf[0] = '\0';		/* initially empty string */
				/* get text for address */
  rfc822_write_address (LOCAL->buf,a);
  return search (LOCAL->buf,(long) strlen (LOCAL->buf),d,n);
}

/* Search parsers */


/* Parse a date
 * Accepts: function to return
 *	    pointer to date integer to return
 * Returns: function to return
 */

search_t mmdf_search_date (f,n)
	search_t f;
	long *n;
{
  long i;
  char *s;
  MESSAGECACHE elt;
				/* parse the date and return fn if OK */
  return (mmdf_search_string (f,&s,&i) && mail_parse_date (&elt,s) &&
	  (*n = (elt.year << 9) + (elt.month << 5) + elt.day)) ? f : NIL;
}

/* Parse a flag
 * Accepts: function to return
 *	    pointer to string to return
 * Returns: function to return
 */

search_t mmdf_search_flag (f,d)
	search_t f;
	char **d;
{
				/* get a keyword, return if OK */
  return (*d = strtok (NIL," ")) ? f : NIL;
}


/* Parse a string
 * Accepts: function to return
 *	    pointer to string to return
 *	    pointer to string length to return
 * Returns: function to return
 */


search_t mmdf_search_string (f,d,n)
	search_t f;
	char **d;
	long *n;
{
  char *c = strtok (NIL,"");	/* remainder of criteria */
  if (c) {			/* better be an argument */
    switch (*c) {		/* see what the argument is */
    case '\0':			/* catch bogons */
    case ' ':
      return NIL;
    case '"':			/* quoted string */
      if (!(strchr (c+1,'"') && (*d = strtok (c,"\"")) && (*n = strlen (*d))))
	return NIL;
      break;
    case '{':			/* literal string */
      *n = strtol (c+1,&c,10);	/* get its length */
      if (*c++ != '}' || *c++ != '\015' || *c++ != '\012' ||
	  *n > strlen (*d = c)) return NIL;
      c[*n] = '\255';		/* write new delimiter */
      strtok (c,"\255");	/* reset the strtok mechanism */
      break;
    default:			/* atomic string */
      *n = strlen (*d = strtok (c," "));
      break;
    }
    return f;
  }
  else return NIL;
}
