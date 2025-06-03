/* config.h
 * This file was produced by running the config.h.SH script, which
 * gets its values from config.sh, which is generally produced by
 * running Configure.
 *
 * Feel free to modify any of this as the need arises.  Note, however,
 * that running config.h.SH again will wipe out any changes you've made.
 * For a more permanent change edit config.sh and rerun config.h.SH.
 * $Id: config.h.SH,v 1.2 92/01/11 16:28:01 usenet Exp $
 *
 * $Log:	config.h.SH,v $
 * Revision 1.2  92/01/11  16:28:01  usenet
 * Lots of bug fixes:
 * 
 *    o    More upgrade notes for trn 1.x users (see ** LOOK ** in NEW).
 *    o    Enhanced the article-reading code to not remember our side-trip
 *         to the end of the group between thread selections (to fix '-').
 *    o    Extended trrn's handling of new articles (we fetch the active file
 *         more consistently, and listen to what the GROUP command tells us).
 *    o    Enhanced the thread selector to mention when new articles have
 *         cropped up since the last visit to the selector.
 *    o    Changed strftime to use size_t and added a check for size_t in
 *         Configure to make sure it is defined.  Also made it a bit more
 *         portable by using gettimeofday() and timezone() in some instances.
 *    o    Fixed a problem with the "total" structure not getting zero'ed in
 *         mthreads (causing bogus 'E'rrors on null groups).
 *    o    Fixed a reference to tmpbuf in intrp.c that was bogus.
 *    o    Fixed a problem with using N and Y with the newgroup code and then
 *         trying to use the 'a' command.
 *    o    Fixed an instance where having trrn get ahead of the active file
 *         might declare a group as being reset when it wasn't.
 *    o    Reorganized the checks for Apollo's C library to make sure it doesn't
 *         find the Domain OS version.
 *    o    Added a check for Xenix 386's C library.
 *    o    Made the Configure PATH more portable.
 *    o    Fixed the arguments prototypes to int_catcher() and swinch_catcher().
 *    o    Fixed the insert-my-subject-before-my-sibling code to not do this
 *         when the sibling's subject is the same as the parent.
 *    o    Fixed a bug in the RELAY code (which I'm suprised is still being used).
 *    o    Twiddled the mthreads.8 manpage.
 *    o    mthreads.8 is now installed and the destination is prompted for in
 *         Configure, since it might be different from the .1 destination.
 *    o    Fixed a typo in newsnews.SH and the README.
 * 
 * Revision 4.4.2.1  1991/12/01  18:05:42  sob
 * Fixed problems with CTRLA flag.
 *
 * Revision 4.4  1991/09/09  20:18:23  sob
 * release 4.4
 *
 *
 * 
 */
/* This software is Copyright 1991 by Stan Barber. 
 *
 * Permission is hereby granted to copy, reproduce, redistribute or otherwise
 * use this software as long as: there is no monetary profit gained
 * specifically from the use or reproduction of this software, it is not
 * sold, rented, traded or otherwise marketed, and this copyright notice is
 * included prominently in any copy made. 
 *
 * The author make no claims as to the fitness or correctness of this software
 * for any use whatsoever, and it is provided as is. Any use of this software
 * is at the user's own risk. 
 */

/* name of the site.  May be overridden by HOSTFILE, gethostname, uname, etc. */
#define SITENAME "onlyyou.lemis.de"
#define HOSTFILE "/etc/sitename"	/* defined if we read the hostname from a file */

/* domain name for the site */
#define OURDOMAIN "lemis.de"

/* name of the organization, may be a file name */
#define ORGNAME "/etc/organization"

/* ignore the ORGANIZATION environment variable */
/*#undef IGNOREORG		/**/

/* login name of news administrator, if any. */
#define NEWSADMIN "news"	/**/

/* news library, may use only ~ and %l expansion */
#define LIB "~news"

/* rn private library, may use ~ expansion, %x and %l */
#define RNLIB "/opt/lib/news"

/* mthreads private files */
#define MTLIB "/opt/lib/news"

/* location of the news spool directory, may use ~ expansion, %x and %l */
#define SPOOL "/var/spool/news"

/* the server's spool directory if generating an NNTP trn & a local mthreads */
/*#undef SERVERSPOOL ""	/* forces local mthreads */

/* put thread files in each spool directory, unless THREAD_DIR is defined */
/*#undef THREAD_DIR "/usr/spool/threads"	/* base directory */

/* save in subdirectories unless LONG_THREAD_NAMES & THREAD_DIR are defined */
/*#undef LONG_THREAD_NAMES		/* not for short-name systems */

/* I doubt you'd want to undefine this, but someone may */
#define USETMPTHREAD			/* use tmpthread to update threads */

/* location of the active file, may use ~ expansion, %x and %l */
#define ACTIVE "~news/active"

/* define where the active.times file is kept if we're using it */
#define ACTIVE_TIMES "~news/active.times"	/* including name */

/* location of spooled mail */
#define MAILFILE "/usr/mail/%L"

/* default shell--ok to be a slow shell like csh */
#define PREFSHELL "/opt/bin/bash"

/* default editor */
#define DEFEDITOR "/opt/bin/emacs"

/* root uid */
#define ROOTID 0

/* what is the first character of a mailbox? */
#define MBOXCHAR 'F'

/* how to cancel an article */
#define CANCEL "/opt/lib/news/inews -h <%h"

/* distribution groups */
#define LOCDIST   "none"
#define ORGDIST   "none"
#define CITYDIST  "none"
#define STATEDIST "none"
#define CNTRYDIST "de"
#define CONTDIST  "eu"

#define THREAD_INIT FALSE
#define SELECT_INIT TRUE

#define index  strchr		/* cultural */
#define rindex strrchr		/*  differences? */
#define bcopy(s,d,n) memcpy((char*)d,(char*)s,(int)n)	/* Different */
#define bzero(d,n)   memset((char*)d,0,(int)n)		/*  flavors. */
#define RENAME		/* is rename() a system call? */
/*#undef FTRUNCATE	/* is ftruncate() available? */
/*#undef USLEEP		/* do we have usleep? */
/*#undef USELECT 	/* should we use select to emulate usleep? */
#define STRFTIME	/* is strftime() available? */
#define FOUNDSIZET	/* is size_t around? */
/*#undef CHSIZE		/* is chsize() available? */
#define TZSET		/* modern timezone functions? */
/*#undef void int	/* is void to be avoided? */
#define vfork fork	/* is vfork too virtual? */
/*#undef SUNOS4		/* running SunOS 4.X? */
/*#undef EUNICE		/* no linking? */
/*#undef VMS		/* not currently used, here just in case */
#define GETCWD		/* do we have getcwd()? */
/*#undef GETWD		/* do we have getwd()? */
#define SETVBUF 	/* do we have setvbuf()? */
/*#undef SETBUFFER	/* do we have setbuffer()? */
/*#undef USENDIR 	/* include ndir.c? */
/*#undef LIBNDIR 	/* include /usr/include/ndir.h? */
#define DIRTYPE dirent
#ifndef USENDIR
#define DIRINC <dirent.h>
#endif
#define MININACT	/* include 2.10.2 optimization? */
#define PORTABLE	/* do we do extra lookups to start up? */
#define PASSNAMES	/* do names come from the passwd file? */
			/*  (undef to take name from ~/.fullname) */
#define BERKNAMES	/* if so, are they Berkeley format? */
			/* (that is, ":name,stuff:") */
/*#undef USGNAMES	/* or are they USG format? */
			/* (that is, ":stuff-name(stuff):") */
/*#undef WHOAMI		/* should we include whoami.h? */
/*#undef RDCHK		/* do we have rdchk()? */
#define TERMIO		/* is this a termio system? */
/*#undef TERMIOS 	/* is this a termios system? */
#define FCNTL		/* should we include fcntl.h? */
#define IOCTL		/* are ioctl args all defined in one place? */
#define PTEM		/* has the ptem.h include file ? */
/*#undef NORMSIG 	/* use signal rather than sigset? */
/*#undef SIGBLOCK	/* use sigblock and sigsetmask */
#define HAVESIGHOLD	/* use sighold and sigrelse */
#define SIGRET void	/* what does signal() return? */
#define HAVETERMLIB	/* do we have termlib-style routines? */
/*#undef GETUIDGID	/* allow setuid (if possible) */
/*#undef GETPWENT	/* should we include getpwent? */
#define INTERNET	/* does our mailer do INTERNET addressing? */
#define GETHOSTNAME	/* do we have a gethostname function? */
/*#undef DOUNAME 	/* do we have a uname function? */
/*#undef PHOSTNAME "cat /etc/sitename"	/* how to get host name with popen */
#define NORELAY 	/* 2.10.3 doesn't have Relay-Version line */
/*#undef SERVER		/* rrn server code */
/*#undef SERVER_FILE ""	/* news server file */
/*#undef USESYSLOG 	/* use syslog for mthreads' log messages */
/*#undef XTHREAD 	/* get thread files via NNTP */
