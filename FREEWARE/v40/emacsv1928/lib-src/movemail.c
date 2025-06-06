/* movemail foo bar -- move file foo to file bar,
   locking file foo the way /bin/mail respects.
   Copyright (C) 1986, 1992, 1993, 1994 Free Software Foundation, Inc.

This file is part of GNU Emacs.

GNU Emacs is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU Emacs is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Emacs; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* Important notice: defining MAIL_USE_FLOCK or MAIL_USE_LOCKF *will
   cause loss of mail* if you do it on a system that does not normally
   use flock as its way of interlocking access to inbox files.  The
   setting of MAIL_USE_FLOCK and MAIL_USE_LOCKF *must agree* with the
   system's own conventions.  It is not a choice that is up to you.

   So, if your system uses lock files rather than flock, then the only way
   you can get proper operation is to enable movemail to write lockfiles there.
   This means you must either give that directory access modes
   that permit everyone to write lockfiles in it, or you must make movemail
   a setuid or setgid program.  */

/*
 * Modified January, 1986 by Michael R. Gretzinger (Project Athena)
 *
 * Added POP (Post Office Protocol) service.  When compiled -DPOP
 * movemail will accept input filename arguments of the form
 * "po:username".  This will cause movemail to open a connection to
 * a pop server running on $MAILHOST (environment variable).  Movemail
 * must be setuid to root in order to work with POP.
 * 
 * New module: popmail.c
 * Modified routines:
 *	main - added code within #ifdef MAIL_USE_POP; added setuid (getuid ())
 *		after POP code. 
 * New routines in movemail.c:
 *	get_errmsg - return pointer to system error message
 *
 */

#define NO_SHORTNAMES   /* Tell config not to load remap.h */
#include <../src/config.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <errno.h>
#include <../src/syswait.h>

#ifdef MSDOS
#undef access
#endif /* MSDOS */

#ifdef USG
#include <fcntl.h>
#include <unistd.h>
#ifndef F_OK
#define F_OK 0
#define X_OK 1
#define W_OK 2
#define R_OK 4
#endif
#endif /* USG */

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef XENIX
#include <sys/locking.h>
#endif

#ifdef MAIL_USE_LOCKF
#define MAIL_USE_SYSTEM_LOCK
#endif

#ifdef MAIL_USE_FLOCK
#define MAIL_USE_SYSTEM_LOCK
#endif

#ifdef MAIL_USE_MMDF
extern int lk_open (), lk_close ();
#endif

/* Cancel substitutions made by config.h for Emacs.  */
#undef open
#undef read
#undef write
#undef close

char *concat ();
char *xmalloc ();
#ifndef errno
extern int errno;
#endif

/* Nonzero means this is name of a lock file to delete on fatal error.  */
char *delete_lockname;

main (argc, argv)
     int argc;
     char **argv;
{
  char *inname, *outname;
  int indesc, outdesc;
  int nread;
  WAITTYPE status;

#ifndef MAIL_USE_SYSTEM_LOCK
  struct stat st;
  long now;
  int tem;
  char *lockname, *p;
  char *tempname;
  int desc;
#endif /* not MAIL_USE_SYSTEM_LOCK */

  delete_lockname = 0;

  if (argc < 3)
    fatal ("two arguments required");

  inname = argv[1];
  outname = argv[2];

#ifdef MAIL_USE_MMDF
  mmdf_init (argv[0]);
#endif

  /* Check access to output file.  */
  if (access (outname, F_OK) == 0 && access (outname, W_OK) != 0)
    pfatal_with_name (outname);

  /* Also check that outname's directory is writeable to the real uid.  */
  {
    char *buf = (char *) xmalloc (strlen (outname) + 1);
    char *p, q;
    strcpy (buf, outname);
    p = buf + strlen (buf);
    while (p > buf && p[-1] != '/')
      *--p = 0;
    if (p == buf)
      *p++ = '.';
    if (access (buf, W_OK) != 0)
      pfatal_with_name (buf);
    free (buf);
  }

#ifdef MAIL_USE_POP
  if (!strncmp (inname, "po:", 3))
    {
      int status; char *user;

      for (user = &inname[strlen (inname) - 1]; user >= inname; user--)
	if (*user == ':')
	  break;

      status = popmail (user, outname);
      exit (status);
    }

  setuid (getuid ());
#endif /* MAIL_USE_POP */

  /* Check access to input file.  */
  if (access (inname, R_OK | W_OK) != 0)
    pfatal_with_name (inname);

#ifndef MAIL_USE_MMDF
#ifndef MAIL_USE_SYSTEM_LOCK
  /* Use a lock file named /usr/spool/mail/$USER.lock:
     If it exists, the mail file is locked.  */
  /* Note: this locking mechanism is *required* by the mailer
     (on systems which use it) to prevent loss of mail.

     On systems that use a lock file, extracting the mail without locking
     WILL occasionally cause loss of mail due to timing errors!

     So, if creation of the lock file fails
     due to access permission on /usr/spool/mail,
     you simply MUST change the permission
     and/or make movemail a setgid program
     so it can create lock files properly.

     You might also wish to verify that your system is one
     which uses lock files for this purpose.  Some systems use other methods.

     If your system uses the `flock' system call for mail locking,
     define MAIL_USE_SYSTEM_LOCK in config.h or the s-*.h file
     and recompile movemail.  If the s- file for your system
     should define MAIL_USE_SYSTEM_LOCK but does not, send a bug report
     to bug-gnu-emacs@prep.ai.mit.edu so we can fix it.  */

  lockname = concat (inname, ".lock", "");
  tempname = (char *) xmalloc (strlen (inname) + strlen ("EXXXXXX") + 1);
  strcpy (tempname, inname);
  p = tempname + strlen (tempname);
  while (p != tempname && p[-1] != '/')
    p--;
  *p = 0;
  strcpy (p, "EXXXXXX");
  mktemp (tempname);
  unlink (tempname);

  while (1)
    {
      /* Create the lock file, but not under the lock file name.  */
      /* Give up if cannot do that.  */
      desc = open (tempname, O_WRONLY | O_CREAT | O_EXCL, 0666);
      if (desc < 0)
	pfatal_with_name ("lock file--see source file lib-src/movemail.c");
      close (desc);

      tem = link (tempname, lockname);
      unlink (tempname);
      if (tem >= 0)
	break;
      sleep (1);

      /* If lock file is a minute old, unlock it.  */
      if (stat (lockname, &st) >= 0)
	{
	  now = time (0);
	  if (st.st_ctime < now - 60)
	    unlink (lockname);
	}
    }

  delete_lockname = lockname;
#endif /* not MAIL_USE_SYSTEM_LOCK */
#endif /* not MAIL_USE_MMDF */

  if (fork () == 0)
    {
      setuid (getuid ());

#ifndef MAIL_USE_MMDF
#ifdef MAIL_USE_SYSTEM_LOCK
      indesc = open (inname, O_RDWR);
#else  /* if not MAIL_USE_SYSTEM_LOCK */
      indesc = open (inname, O_RDONLY);
#endif /* not MAIL_USE_SYSTEM_LOCK */
#else  /* MAIL_USE_MMDF */
      indesc = lk_open (inname, O_RDONLY, 0, 0, 10);
#endif /* MAIL_USE_MMDF */

      if (indesc < 0)
	pfatal_with_name (inname);

#if defined (BSD) || defined (XENIX)
      /* In case movemail is setuid to root, make sure the user can
	 read the output file.  */
      /* This is desirable for all systems
	 but I don't want to assume all have the umask system call */
      umask (umask (0) & 0333);
#endif /* BSD or Xenix */
      outdesc = open (outname, O_WRONLY | O_CREAT | O_EXCL, 0666);
      if (outdesc < 0)
	pfatal_with_name (outname);
#ifdef MAIL_USE_SYSTEM_LOCK
#ifdef MAIL_USE_LOCKF
      if (lockf (indesc, F_LOCK, 0) < 0) pfatal_with_name (inname);
#else /* not MAIL_USE_LOCKF */
#ifdef XENIX
      if (locking (indesc, LK_RLCK, 0L) < 0) pfatal_with_name (inname);
#else
      if (flock (indesc, LOCK_EX) < 0) pfatal_with_name (inname);
#endif
#endif /* not MAIL_USE_LOCKF */
#endif /* MAIL_USE_SYSTEM_LOCK */

      {
	char buf[1024];

	while (1)
	  {
	    nread = read (indesc, buf, sizeof buf);
	    if (nread != write (outdesc, buf, nread))
	      {
		int saved_errno = errno;
		unlink (outname);
		errno = saved_errno;
		pfatal_with_name (outname);
	      }
	    if (nread < sizeof buf)
	      break;
	  }
      }

#ifdef BSD
      if (fsync (outdesc) < 0)
	pfatal_and_delete (outname);
#endif

      /* Check to make sure no errors before we zap the inbox.  */
      if (close (outdesc) != 0)
	pfatal_and_delete (outname);

#ifdef MAIL_USE_SYSTEM_LOCK
#if defined (STRIDE) || defined (XENIX)
      /* Stride, xenix have file locking, but no ftruncate.  This mess will do. */
      close (open (inname, O_CREAT | O_TRUNC | O_RDWR, 0666));
#else
      ftruncate (indesc, 0L);
#endif /* STRIDE or XENIX */
#endif /* MAIL_USE_SYSTEM_LOCK */

#ifdef MAIL_USE_MMDF
      lk_close (indesc, 0, 0, 0);
#else
      close (indesc);
#endif

#ifndef MAIL_USE_SYSTEM_LOCK
      /* Delete the input file; if we can't, at least get rid of its
	 contents.  */
#ifdef MAIL_UNLINK_SPOOL
      /* This is generally bad to do, because it destroys the permissions
	 that were set on the file.  Better to just empty the file.  */
      if (unlink (inname) < 0 && errno != ENOENT)
#endif /* MAIL_UNLINK_SPOOL */
	creat (inname, 0600);
#endif /* not MAIL_USE_SYSTEM_LOCK */

      exit (0);
    }

  wait (&status);
  if (!WIFEXITED (status))
    exit (1);
  else if (WRETCODE (status) != 0)
    exit (WRETCODE (status));

#if !defined (MAIL_USE_MMDF) && !defined (MAIL_USE_SYSTEM_LOCK)
  unlink (lockname);
#endif /* not MAIL_USE_MMDF and not MAIL_USE_SYSTEM_LOCK */
  exit (0);
}

/* Print error message and exit.  */

fatal (s1, s2)
     char *s1, *s2;
{
  if (delete_lockname)
    unlink (delete_lockname);
  error (s1, s2);
  exit (1);
}

/* Print error message.  `s1' is printf control string, `s2' is arg for it. */

error (s1, s2, s3)
     char *s1, *s2, *s3;
{
  printf ("movemail: ");
  printf (s1, s2, s3);
  printf ("\n");
}

pfatal_with_name (name)
     char *name;
{
  extern int errno;
  extern char *strerror ();
  char *s;

  s = concat ("", strerror (errno), " for %s");
  fatal (s, name);
}

pfatal_and_delete (name)
     char *name;
{
  extern int errno;
  extern char *strerror ();
  char *s;

  s = concat ("", strerror (errno), " for %s");
  unlink (name);
  fatal (s, name);
}

/* Return a newly-allocated string whose contents concatenate those of s1, s2, s3.  */

char *
concat (s1, s2, s3)
     char *s1, *s2, *s3;
{
  int len1 = strlen (s1), len2 = strlen (s2), len3 = strlen (s3);
  char *result = (char *) xmalloc (len1 + len2 + len3 + 1);

  strcpy (result, s1);
  strcpy (result + len1, s2);
  strcpy (result + len1 + len2, s3);
  *(result + len1 + len2 + len3) = 0;

  return result;
}

/* Like malloc but get fatal error if memory is exhausted.  */

char *
xmalloc (size)
     unsigned size;
{
  char *result = (char *) malloc (size);
  if (!result)
    fatal ("virtual memory exhausted", 0);
  return result;
}

/* This is the guts of the interface to the Post Office Protocol.  */

#ifdef MAIL_USE_POP

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <pwd.h>

#ifdef USG
#include <fcntl.h>
/* Cancel substitutions made by config.h for Emacs.  */
#undef open
#undef read
#undef write
#undef close
#endif /* USG */

#define NOTOK (-1)
#define OK 0
#define DONE 1

char *progname;
FILE *sfi;
FILE *sfo;
char Errmsg[80];

static int debug = 0;

char *get_errmsg ();
char *getenv ();
int mbx_write ();

popmail (user, outfile)
     char *user;
     char *outfile;
{
  char *host;
  int nmsgs, nbytes;
  char response[128];
  register int i;
  int mbfi;
  FILE *mbf;
  struct passwd *pw = (struct passwd *) getpwuid (getuid ());
  if (pw == NULL)
    fatal ("cannot determine user name");

  host = getenv ("MAILHOST");
  if (host == NULL)
    {
      fatal ("no MAILHOST defined");
    }

  if (pop_init (host) == NOTOK)
    {
      fatal (Errmsg);
    }

  if (getline (response, sizeof response, sfi) != OK)
    {
      fatal (response);
    }

  if (pop_command ("USER %s", user) == NOTOK
      || pop_command ("RPOP %s", pw->pw_name) == NOTOK)
    {
      pop_command ("QUIT");
      fatal (Errmsg);
    }

  if (pop_stat (&nmsgs, &nbytes) == NOTOK)
    {
      pop_command ("QUIT");
      fatal (Errmsg);
    }

  if (!nmsgs)
    {
      pop_command ("QUIT");
      return 0;
    }

  mbfi = open (outfile, O_WRONLY | O_CREAT | O_EXCL, 0666);
  if (mbfi < 0)
    {
      pop_command ("QUIT");
      pfatal_and_delete (outfile);
    }
  fchown (mbfi, getuid (), -1);

  if ((mbf = fdopen (mbfi, "w")) == NULL)
    {
      pop_command ("QUIT");
      pfatal_and_delete (outfile);
    }

  for (i = 1; i <= nmsgs; i++)
    {
      mbx_delimit_begin (mbf);
      if (pop_retr (i, mbx_write, mbf) != OK)
	{
	  pop_command ("QUIT");
	  close (mbfi);
	  unlink (outfile);
	  fatal (Errmsg);
	}
      mbx_delimit_end (mbf);
      fflush (mbf);
    }

  if (fsync (mbfi) < 0)
    {
      pop_command ("QUIT");
      pfatal_and_delete (outfile);
    }

  if (close (mbfi) == -1)
    {
      pop_command ("QUIT");
      pfatal_and_delete (outfile);
    }

  for (i = 1; i <= nmsgs; i++)
    {
      if (pop_command ("DELE %d", i) == NOTOK)
	{
	  /* Better to ignore this failure.  */
	}
    }

  pop_command ("QUIT");
  return (0);
}

pop_init (host)
     char *host;
{
  register struct hostent *hp;
  register struct servent *sp;
  int lport = IPPORT_RESERVED - 1;
  struct sockaddr_in sin;
  register int s;

  hp = gethostbyname (host);
  if (hp == NULL)
    {
      sprintf (Errmsg, "MAILHOST unknown: %s", host);
      return NOTOK;
    }

  sp = getservbyname ("pop", "tcp");
  if (sp == 0)
    {
      strcpy (Errmsg, "tcp/pop: unknown service");
      return NOTOK;
    }

  sin.sin_family = hp->h_addrtype;
  bcopy (hp->h_addr, (char *)&sin.sin_addr, hp->h_length);
  sin.sin_port = sp->s_port;
  s = rresvport (&lport);
  if (s < 0)
    {
      sprintf (Errmsg, "error creating socket: %s", get_errmsg ());
      return NOTOK;
    }

  if (connect (s, (char *)&sin, sizeof sin) < 0)
    {
      sprintf (Errmsg, "error during connect: %s", get_errmsg ());
      close (s);
      return NOTOK;
    }

  sfi = fdopen (s, "r");
  sfo = fdopen (s, "w");
  if (sfi == NULL || sfo == NULL)
    {
      sprintf (Errmsg, "error in fdopen: %s", get_errmsg ());
      close (s);
      return NOTOK;
    }

  return OK;
}

pop_command (fmt, a, b, c, d)
     char *fmt;
{
  char buf[128];
  char errmsg[64];

  sprintf (buf, fmt, a, b, c, d);

  if (debug) fprintf (stderr, "---> %s\n", buf);
  if (putline (buf, Errmsg, sfo) == NOTOK) return NOTOK;

  if (getline (buf, sizeof buf, sfi) != OK)
    {
      strcpy (Errmsg, buf);
      return NOTOK;
    }

  if (debug)
    fprintf (stderr, "<--- %s\n", buf);
  if (*buf != '+')
    {
      strcpy (Errmsg, buf);
      return NOTOK;
    }
  else
    {
      return OK;
    }
}

    
pop_stat (nmsgs, nbytes)
     int *nmsgs, *nbytes;
{
  char buf[128];

  if (debug)
    fprintf (stderr, "---> STAT\n");
  if (putline ("STAT", Errmsg, sfo) == NOTOK)
    return NOTOK;

  if (getline (buf, sizeof buf, sfi) != OK)
    {
      strcpy (Errmsg, buf);
      return NOTOK;
    }

  if (debug) fprintf (stderr, "<--- %s\n", buf);
  if (*buf != '+')
    {
      strcpy (Errmsg, buf);
      return NOTOK;
    }
  else
    {
      sscanf (buf, "+OK %d %d", nmsgs, nbytes);
      return OK;
    }
}

pop_retr (msgno, action, arg)
     int (*action)();
{
  char buf[128];

  sprintf (buf, "RETR %d", msgno);
  if (debug) fprintf (stderr, "%s\n", buf);
  if (putline (buf, Errmsg, sfo) == NOTOK) return NOTOK;

  if (getline (buf, sizeof buf, sfi) != OK)
    {
      strcpy (Errmsg, buf);
      return NOTOK;
    }

  while (1)
    {
      switch (multiline (buf, sizeof buf, sfi))
	{
	case OK:
	  (*action)(buf, arg);
	  break;
	case DONE:
	  return OK;
	case NOTOK:
	  strcpy (Errmsg, buf);
	  return NOTOK;
	}
    }
}

getline (buf, n, f)
     char *buf;
     register int n;
     FILE *f;
{
  register char *p;
  int c;

  p = buf;
  while (--n > 0 && (c = fgetc (f)) != EOF)
    if ((*p++ = c) == '\n') break;

  if (ferror (f))
    {
      strcpy (buf, "error on connection");
      return NOTOK;
    }

  if (c == EOF && p == buf)
    {
      strcpy (buf, "connection closed by foreign host");
      return DONE;
    }

  *p = NULL;
  if (*--p == '\n') *p = NULL;
  if (*--p == '\r') *p = NULL;
  return OK;
}

multiline (buf, n, f)
     char *buf;
     register int n;
     FILE *f;
{
  if (getline (buf, n, f) != OK)
    return NOTOK;
  if (*buf == '.')
    {
      if (*(buf+1) == NULL)
	return DONE;
      else
	strcpy (buf, buf+1);
    }
  return OK;
}

char *
get_errmsg ()
{
  extern int errno;
  extern char *strerror ();
  return strerror (errno);
}

putline (buf, err, f)
     char *buf;
     char *err;
     FILE *f;
{
  fprintf (f, "%s\r\n", buf);
  fflush (f);
  if (ferror (f))
    {
      strcpy (err, "lost connection");
      return NOTOK;
    }
  return OK;
}

mbx_write (line, mbf)
     char *line;
     FILE *mbf;
{
  fputs (line, mbf);
  fputc (0x0a, mbf);
}

mbx_delimit_begin (mbf)
     FILE *mbf;
{
  fputs ("\f\n0, unseen,,\n", mbf);
}

mbx_delimit_end (mbf)
     FILE *mbf;
{
  putc ('\037', mbf);
}

#endif /* MAIL_USE_POP */

#ifndef HAVE_STRERROR
char *
strerror (errnum)
     int errnum;
{
  extern char *sys_errlist[];
  extern int sys_nerr;

  if (errnum >= 0 && errnum < sys_nerr)
    return sys_errlist[errnum];
  return (char *) "Unknown error";
}

#endif /* ! HAVE_STRERROR */
