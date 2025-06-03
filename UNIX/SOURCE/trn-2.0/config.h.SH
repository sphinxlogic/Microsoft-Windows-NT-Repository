: create config.h file
case $CONFIG in
    '') . ./config.sh ;;
esac
echo "Extracting config.h (with variable substitutions)"
case "$threaddir" in
'.')	threaddir='/usr/spool/threads'
	tdir='undef';;
*)	tdir='define';;
esac
case "$serverspool" in
'') sspool='undef';;
*)  sspool='define';;
esac
case "$activetimes" in
'none')	acttimes='undef';;
*)	acttimes='define';;
esac
case "$syslog" in
LOG_*)	syslog2='define';;
*)	syslog2='undef';;
esac
case "$hostfile" in
'')	usehostfile='undef';;
*)	usehostfile='define';;
esac

$sed 's,^#undef,/*#undef,' >config.h <<EOT
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
#define SITENAME "$sitename"
#$usehostfile HOSTFILE "$hostfile"	/* defined if we read the hostname from a file */

/* domain name for the site */
#define OURDOMAIN "$domain"

/* name of the organization, may be a file name */
#define ORGNAME "$orgname"

/* ignore the ORGANIZATION environment variable */
#$ignoreorg IGNOREORG		/**/

/* login name of news administrator, if any. */
#$isadmin NEWSADMIN "$newsadmin"	/**/

/* news library, may use only ~ and %l expansion */
#define LIB "$lib"

/* rn private library, may use ~ expansion, %x and %l */
#define RNLIB "$rnlib"

/* mthreads private files */
#define MTLIB "$mtlib"

/* location of the news spool directory, may use ~ expansion, %x and %l */
#define SPOOL "$spool"

/* the server's spool directory if generating an NNTP trn & a local mthreads */
#$sspool SERVERSPOOL "$serverspool"	/* forces local mthreads */

/* put thread files in each spool directory, unless THREAD_DIR is defined */
#$tdir THREAD_DIR "$threaddir"	/* base directory */

/* save in subdirectories unless LONG_THREAD_NAMES & THREAD_DIR are defined */
#undef LONG_THREAD_NAMES		/* not for short-name systems */

/* I doubt you'd want to undefine this, but someone may */
#define USETMPTHREAD			/* use tmpthread to update threads */

/* location of the active file, may use ~ expansion, %x and %l */
#define ACTIVE "$active"

/* define where the active.times file is kept if we're using it */
#$acttimes ACTIVE_TIMES "$activetimes"	/* including name */

/* location of spooled mail */
#define MAILFILE "$maildir"

/* default shell--ok to be a slow shell like csh */
#define PREFSHELL "$pref"

/* default editor */
#define DEFEDITOR "$defeditor"

/* root uid */
#define ROOTID $rootid

/* what is the first character of a mailbox? */
#define MBOXCHAR '$mboxchar'

/* how to cancel an article */
#define CANCEL "$inews -h <%h"

/* distribution groups */
#define LOCDIST   "$locpref"
#define ORGDIST   "$orgpref"
#define CITYDIST  "$citypref"
#define STATEDIST "$statepref"
#define CNTRYDIST "$cntrypref"
#define CONTDIST  "$contpref"

#define THREAD_INIT $threaddef
#define SELECT_INIT $selectdef

#$strchr index  strchr		/* cultural */
#$strchr rindex strrchr		/*  differences? */
#$memcpy bcopy(s,d,n) memcpy((char*)d,(char*)s,(int)n)	/* Different */
#$memcpy bzero(d,n)   memset((char*)d,0,(int)n)		/*  flavors. */
#$rename RENAME		/* is rename() a system call? */
#$ftruncate FTRUNCATE	/* is ftruncate() available? */
#$usleep USLEEP		/* do we have usleep? */
#$uselect USELECT 	/* should we use select to emulate usleep? */
#$strftime STRFTIME	/* is strftime() available? */
#$foundsizet FOUNDSIZET	/* is size_t around? */
#$chsize CHSIZE		/* is chsize() available? */
#$tzset TZSET		/* modern timezone functions? */
#$novoid void int	/* is void to be avoided? */
#$novfork vfork fork	/* is vfork too virtual? */
#$sunos4 SUNOS4		/* running SunOS 4.X? */
#$eunice EUNICE		/* no linking? */
#$eunice VMS		/* not currently used, here just in case */
#$getcwd GETCWD		/* do we have getcwd()? */
#$getwd GETWD		/* do we have getwd()? */
#$setvbuf SETVBUF 	/* do we have setvbuf()? */
#$setbuffer SETBUFFER	/* do we have setbuffer()? */
#$usendir USENDIR 	/* include ndir.c? */
#$libndir LIBNDIR 	/* include /usr/include/ndir.h? */
#define DIRTYPE $dirtype
#ifndef USENDIR
#define DIRINC $dirinc
#endif
#$mininact MININACT	/* include 2.10.2 optimization? */
#$portable PORTABLE	/* do we do extra lookups to start up? */
#$passnam PASSNAMES	/* do names come from the passwd file? */
			/*  (undef to take name from ~/.fullname) */
#$berknam BERKNAMES	/* if so, are they Berkeley format? */
			/* (that is, ":name,stuff:") */
#$usgnam USGNAMES	/* or are they USG format? */
			/* (that is, ":stuff-name(stuff):") */
#$whoami WHOAMI		/* should we include whoami.h? */
#$rdchk RDCHK		/* do we have rdchk()? */
#$termio TERMIO		/* is this a termio system? */
#$termios TERMIOS 	/* is this a termios system? */
#$fcntl FCNTL		/* should we include fcntl.h? */
#$ioctl IOCTL		/* are ioctl args all defined in one place? */
#$ptem PTEM		/* has the ptem.h include file ? */
#$normsig NORMSIG 	/* use signal rather than sigset? */
#$sigblock SIGBLOCK	/* use sigblock and sigsetmask */
#$sighold HAVESIGHOLD	/* use sighold and sigrelse */
#define SIGRET $sigret	/* what does signal() return? */
#$havetlib HAVETERMLIB	/* do we have termlib-style routines? */
#$getuidgid GETUIDGID	/* allow setuid (if possible) */
#$getpwent GETPWENT	/* should we include getpwent? */
#$internet INTERNET	/* does our mailer do INTERNET addressing? */
#$gethostname GETHOSTNAME	/* do we have a gethostname function? */
#$douname DOUNAME 	/* do we have a uname function? */
#$phostname PHOSTNAME "$hostcmd"	/* how to get host name with popen */
#$norelay NORELAY 	/* 2.10.3 doesn't have Relay-Version line */
#$isrrn SERVER		/* rrn server code */
#$isrrn SERVER_FILE "$serverfile"	/* news server file */
#$syslog2 USESYSLOG $syslog	/* use syslog for mthreads' log messages */
#$xthread XTHREAD 	/* get thread files via NNTP */
EOT
