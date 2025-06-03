/* config.h
 * This file was produced by running the config.h.SH script, which
 * gets its values from config.sh, which is generally produced by
 * running Configure.
 *
 * Feel free to modify any of this as the need arises.  Note, however,
 * that running config.h.SH again will wipe out any changes you've made.
 * For a more permanent change edit config.sh and rerun config.h.SH.
 */


/* BIN:
 *	This symbol holds the name of the directory in which the user wants
 *	to put publicly executable images for the package in question.  It
 *	is most often a local directory such as /usr/local/bin.
 */
#define BIN "/opt/bin"             /**/

/* CPPSTDIN:
 *	This symbol contains the first part of the string which will invoke
 *	the C preprocessor on the standard input and produce to standard
 *	output.	 Typical value of "cc -E" or "/lib/cpp".
 */
/* CPPMINUS:
 *	This symbol contains the second part of the string which will invoke
 *	the C preprocessor on the standard input and produce to standard
 *	output.  This symbol will have the value "-" if CPPSTDIN needs a minus
 *	to specify standard input, otherwise the value is "".
 */
#define CPPSTDIN "/usr/local/bin/cpp"
#define CPPMINUS ""

/* GETOPT:
 *	This symbol, if defined, indicates that the getopt() routine exists.
 */
#define	GETOPT		/**/

/* MEMCPY:
 *	This symbol, if defined, indicates that the memcpy routine is available
 *	to copy blocks of memory.  Otherwise you should probably use bcopy().
 *	If neither is defined, roll your own.
 */
#define	MEMCPY		/**/

/* MKDIR:
 *	This symbol, if defined, indicates that the mkdir routine is available
 *	to create directories.  Otherwise you should fork off a new process to
 *	exec /bin/mkdir.
 */
#define	MKDIR		/**/

/* RENAME:
 *	This symbol, if defined, indicates that the rename routine is available
 *	to rename files.  Otherwise you should do the unlink(), link(), unlink()
 *	trick.
 */
#define	RENAME		/**/

/* SYMLINK:
 *	This symbol, if defined, indicates that the symlink routine is available
 *	to create symbolic links.
 */
#define	SYMLINK		/**/

/* WHOAMI:
 *	This symbol, if defined, indicates that the program may include
 *	whoami.h.
 */
/*#undef	WHOAMI		/**/

/* PREFSHELL:
 *	This symbol contains the full name of the preferred user shell on this
 *	system.  Usual values are /bin/csh, /bin/ksh, /bin/sh.
 */
#define PREFSHELL "/opt/bin/bash"		/**/

/* EUNICE:
 *	This symbol, if defined, indicates that the program is being compiled
 *	under the EUNICE package under VMS.  The program will need to handle
 *	things like files that don't go away the first time you unlink them,
 *	due to version numbering.  It will also need to compensate for lack
 *	of a respectable link() command.
 */
/* VMS:
 *	This symbol, if defined, indicates that the program is running under
 *	VMS.  It is currently only set in conjunction with the EUNICE symbol.
 */
/*#undef	EUNICE		/**/
/*#undef	VMS		/**/

/* CONFIGURE_DATE
 *	This symbol contains the last date that configure was run for elm -v output.
 */
#define		CONFIGURE_DATE	"Sat May 01 17:12:36 1993"

/* ALTCHECK:
 *	This symbol, if defined, means that altzone exists.
 */
#define ALTCHECK		/**/

/* ASCII_CTYPE:
 *	This symbol, if defined, indicates that the ctype functions and
 *	macros are ascii specific and not 8 bit clean.
 */
/*#undef	ASCII_CTYPE	/**/

/* ENABLE_CALENDAR:
 *	This symbol, if defined, indicates that the calendar feature
 *	should be supported.
 */
#define	ENABLE_CALENDAR	/**/
#define dflt_calendar_file	"calendar"	

/* DONT_ESCAPE_MESSAGES:
 *	This symbol, if defined, indicates that a binary capable MTA is in use
 *	that honors the content-length header and no message constructs need
 *	to be escaped.
 */
#define	DONT_ESCAPE_MESSAGES /**/

/* CRYPT:
 *	This symbol, if defined, indicates that the crypt routine is available
 *	to encrypt passwords and the like.
 */
#define	CRYPT		/**/

/* HAS_CUSERID:
 *	This symbol, if defined, means not to include our own cuserid().
 */
#define HAS_CUSERID		/**/

/* DISP_HOST:
 *	This symbol, if defined, indicates that elm should display the
 *	host name on the index screen.
 */
/*#undef	DISP_HOST /**/

/* GETDOMAINNAME
 *	This symbol, if defined, indicates that the getdomainname system call
 *	is available to obtain the domain name.  Note that the /domain
 *	file overrides the value of getdomainname().  If getdomainname() exists,
 *	MYDOMAIN will be ignored.
 */
/* USEGETDOMAINNAME
 *	This symbol, if defined, indicates that the getdomainname system call
 *	should be used to obtain the domain name.  Note that the /domain
 *	file overrides the value of getdomainname().  If this variable is set
 *	MYDOMAIN (and therefore DEFAULT_DOMAIN) will be ignored.
 */
/*#undef	GETDOMAINNAME		/**/

/*#undef	USEGETDOMAINNAME		/**/

/* ERRLST:
 *	This symbol, if defined, indicates that the sys_errlist and sys_nerr
 *	symbols exist.
 */
#define	ERRLST	/**/

/* USE_FLOCK_LOCKING
 *	This symbol, if defined, indicates that the flock mailbox locking should be used.
 */
/* USE_DOTLOCK_LOCKING
 *	This symbol, if defined, indicates that the .lock mailbox locking should be used.
 */
/* USE_FCNTL_LOCKING
 *	This symbol, if defined, indicates that SYSV style fcntl file locking should be used.
 */
/* LOCK_DIR
 *	This symbol is the name of the lock directory for access (not mailbox) locks.
 *	It will be /usr/spool/locks or /usr/spool/uucp
 */
/*#undef	USE_FLOCK_LOCKING		/**/

#define	USE_DOTLOCK_LOCKING		/**/

#define	USE_FCNTL_LOCKING	/**/

#define		LOCK_DIR	"/usr/spool/locks"	/**/

/* FTRUNCATE:
 *	This symbol, if defined, indicates that the ftruncate() routine exists.
 */
#define	FTRUNCATE		/**/

/* GETHOSTNAME:
 *	This symbol, if defined, indicates that the C program may use the
 *	gethostname() routine to derive the host name.  See also DOUNAME
 *	and PHOSTNAME.
 */
/* DOUNAME:
 *	This symbol, if defined, indicates that the C program may use the
 *	uname() routine to derive the host name.  See also GETHOSTNAME and
 *	PHOSTNAME.
 */
/* PHOSTNAME:
 *	This symbol, if defined, indicates that the C program may use the
 *	contents of PHOSTNAME as a command to feed to the popen() routine
 *	to derive the host name.  See also GETHOSTNAME and DOUNAME.
 */
/* HOSTCOMPILED:
 *	This symbol, if defined, indicated that the host name is compiled
 *	in from the string hostname
 */
/*#undef	GETHOSTNAME	/**/
#define	DOUNAME		/**/
/*#undef	PHOSTNAME ""	/**/
/*#undef	HOSTCOMPILED	/**/

/* HAVETERMLIB:
 *	This symbol, when defined, indicates that termlib-style routines
 *	are available.  There is nothing to include.
 */
#define	HAVETERMLIB	/**/

/* index:
 *	This preprocessor symbol is defined, along with rindex, if the system
 *	uses the strchr and strrchr routines instead.
 */
/* rindex:
 *	This preprocessor symbol is defined, along with index, if the system
 *	uses the strchr and strrchr routines instead.
 */
#define	index strchr	/* cultural */
#define	rindex strrchr	/*  differences? */

/* INTERNET:
 *	This symbol, if defined, indicates that there is a mailer available
 *	which supports internet-style addresses (user@site.domain).
 */
#define	INTERNET	/**/

/* ISPELL:
 *	This symbol, if defined, indicates that the GNU ispell
 *	spelling checker that is available to Elm.
 */
/* ISPELL_PATH:
 *	This symbol contains the path to the GNU ispell
 *	spelling checker that is available to Elm.
 */
/* ISPELL_OPTIONS:
 *	This symbol contains the options to the GNU ispell
 *	spelling checker that is available to Elm.
 */
#define	ISPELL	/**/
#define		ISPELL_PATH	"/opt/bin/ispell" /**/
#define		ISPELL_OPTIONS	"-x" /**/

/* I_LOCALE:
 *	This symbol, if defined, indicates that the file locale.h
 *	should be included
 */
/* MSGCAT:
 *	This symbol, if defined, indicates that the MSGCAT NLS libraries
 *	are available.
 */
/* USENLS:
 *	This symbol, if defined, indicates that the Elm NLS libraries
 *	are being used instead of the system NLS libraries.
 */
#define	I_LOCALE	/**/
/*#undef	I_NL_TYPES	/**/
#define	MSGCAT		/**/
#define	USENLS		/**/

/* ALLOW_MAILBOX_EDITING:
 *	This symbol, if defined, indicates that the E)dit mailbox
 *	function is to be allowed.
 */
#define	ALLOW_MAILBOX_EDITING	/**/

/* MIME:
 *	This symbol, if defined, indicates that the MIME mail
 *	extension utilities are acailable
 */
/*#undef	MIME	/**/

/* MMDF:
 *	This symbol, if defined, indicates that mailboxes are in
 *	the MMDF format.
 */
/*#undef	MMDF	/**/

/* AUTO_BACKGROUND:
 *	This symbol, if defined, indicates that newmail should go to
 *	the background automatically.
 */
#define AUTO_BACKGROUND /**/

/* DONT_ADD_FROM:
 *	This symbol, if defined, indicates that elm should not adD
 *	the From: header
 */
/* USE_DOMAIN:
 *	This symbol, if defined, indicates that elm should add
 *	the domain name to our address
 */
/* NOCHECK_VALIDNAME:
 *	This symbol, if defined, indicates that elm should not
 *	check the addresses against mailboxes on this system.
 */
#define	DONT_ADD_FROM /**/
/*#undef	USE_DOMAIN /**/
#define NOCHECK_VALIDNAME	/**/

/* NO_XHEADER:
 *	This symbol, if defined, will not automatically add "X-Mailer:"
 *	headers.
 */
/*#undef	NO_XHEADER	/**/

/* PIDCHECK:
 *	This symbol, if defined, means that the kill(pid, 0) will
 *	check for an active pid.
 */
#define PIDCHECK		/**/

/* PORTABLE:
 *	This symbol, if defined, indicates to the C program that it should
 *	not assume that it is running on the machine it was compiled on.
 *	The program should be prepared to look up the host name, translate
 *	generic filenames, use PATH, etc.
 */
#define	PORTABLE	/**/

/* PTEM:
 *	This symbol, if defined, indicates that the sys/ptem.h include file is
 *	needed for window sizing.
 */
#define	PTEM		/**/

/* PUTENV:
 *	This symbol, if defined, indicates that putenv() exists.
 */
#define	PUTENV	/**/

/* REMOVE_AT_LAST:
 *	This symbol, if defined, tells the C code to remove the lock
 *	file on lock failure.
 */
/* MAX_ATTEMPTS:
 *	This symbol defines to the C code the number of times to try
 *	locking the mail file.
 */
/*#undef REMOVE_AT_LAST	/**/
#define MAX_ATTEMPTS	6

/* SAVE_GROUP_MAILBOX_ID:
 *	This symbol, if defined, indicates that Elm needs to restore the
 *	group id of the file, as it is running setgid.
 */
#define SAVE_GROUP_MAILBOX_ID	/**/

/* SIGVEC:
 *	This symbol, if defined, indicates that BSD reliable signals routine
 *	sigvec is available.
 */
/* SIGVECTOR:
 *	This symbol, if defined, indicates that the sigvec() routine is called
 *	sigvector() instead, and that sigspace() is provided instead of
 *	sigstack().  This is probably only true for HP-UX.
 */
/* SIGSET:
 *	This symbol, if defined, indicates that BSD reliable signal routine
 *	sigset is available.
 */
/* POSIX_SIGNALS:
 *	This symbol, if defined, indicated that POSIX sigaction
 *	routine is available.
 */
/* HASSIGHOLD:
 *	This symbol, if defined, indicates that sighold routine is
 *	available.
 */
/* HASSIGBLOCK
 *	This symbol, if defined, indicates that the sigblock routine is
 *	available.
 */
/* HASSIGPROCMASK:
 *	This symbol, if defined, indicates that POSIX sigprocmask
 *	routine is available.
 */
/*#undef	SIGVEC		/**/

/*#undef	SIGVECTOR	/**/

/*#undef	SIGSET	/**/

#define	POSIX_SIGNALS	/**/

/*#undef	HASSIGHOLD	/**/

/*#undef	HASSIGBLOCK	/**/

#define	HASSIGPROCMASK	/**/

/* STRSPN:
 *	This symbol, if defined, indicates that the strspn() routine exists.
 */
/* STRCSPN:
 *	This symbol, if defined, indicates that the strcspn() routine exists.
 */
/* STRPBRK:
 *	This symbol, if defined, indicates that the strpbrk() routine exists.
 */
#define	STRSPN		/**/

#define	STRCSPN		/**/

#define	STRPBRK		/**/

/* STRINGS:
 *	This symbol, if defined, indicates that the file strings.h
 *	should be included not string.h
 */
/* PWDINSYS:
 *	This symbol, if defined, indicates that the file pwd.h
 *	is in the sys sub directory
 */
/*#undef	STRINGS		/**/
/*#undef	PWDINSYS	/**/

/* STRSTR:
 *	This symbol, if defined, indicates that strstr() exists.
 */
#define	STRSTR	/**/

/* STRTOK:
 *	This symbol, if defined, indicates that strtok() exists.
 */
#define	STRTOK	/**/

/* ALLOW_SUBSHELL:
 *	This symbol, if defined, indicates that the '!' subshell
 *	function is to be allowed at various places.
 */
#define	ALLOW_SUBSHELL	/**/

/* TEMPNAM:
 *	This symbol, if defined, indicates that the tempnam() routine exists.
 */
#define	TEMPNAM		/**/

/* TERMIOS:
 *	This symbol, if defined, indicates that the program should include
 *	termios.h rather than sgtty.h or termio.h.  There are also differences
 *	in the ioctl() calls that depend on the value of this symbol.
 */
/* TERMIO:
 *	This symbol, if defined, indicates that the program should include
 *	termio.h rather than sgtty.h.  There are also differences in the
 *	ioctl() calls that depend on the value of this symbol.
 */
#define	TERMIOS		/**/

/*#undef	TERMIO		/**/

/* TZ_MINUTESWEST:
 *	This symbol is defined if this system uses tz_minutes west
 *	in time.h instead of timezone.  Only for BSD Systems
 */
#define	TZ_MINUTESWEST 	/**/

/* TZNAME:
 *	This symbol, if defined, indicates that extern char *tzname[] exists.
 */
#define	TZNAME	/**/

/* USE_EMBEDDED_ADDRESSES:
 *	This symbol, if defined, indicates that replyto: and from:
 *	headers can be trusted.
 */
#define USE_EMBEDDED_ADDRESSES	 /**/

/* UTIMBUF:
 *	This symbol is defined if this system defines struct utimbuf.
 */
#define UTIMBUF		/**/

/* VFORK:
 *	This symbol, if defined, indicates that vfork() exists.
 */
#define	VFORK	/**/

/* DEFEDITOR:
 *	This symbol contains the name of the default editor.
 */
/* EDITOROPTS:
 *	This symbol contains the command line options for the default editor.
 */
#define DEFEDITOR "/opt/bin/emacs"		/**/

#define EDITOROPTS ""		/**/

/* HOSTNAME:
 *	This symbol contains name of the host the program is going to run on.
 *	The domain is not kept with hostname, but must be gotten from MYDOMAIN.
 *	The dot comes with MYDOMAIN, and need not be supplied by the program.
 *	If gethostname() or uname() exist, HOSTNAME may be ignored.
 */
/* MYDOMAIN:
 *	This symbol contains the domain of the host the program is going to
 *	run on.  The domain must be appended to HOSTNAME to form a complete
 *	host name.  The dot comes with MYDOMAIN, and need not be supplied by
 *	the program.  If the host name is derived from PHOSTNAME, the domain
 *	may or may not already be there, and the program should check.
 */
#define HOSTNAME "onlyyou"		/**/
#define MYDOMAIN ".lemis.de"		/**/

/* I_MEMORY:
 *	This symbol, if defined, indicates that the file memory.h
 *	should be included instead of declaring the memory routines.
 */
#define	I_MEMORY	/**/

/* I_STDLIB:
 *	This symbol, if defined, indicates that the file stdlib.h
 *	should be included instead of declaring the stdlib routines.
 */
#define	I_STDLIB	/**/

/* I_TIME:
 *	This symbol is defined if the program should include <time.h>.
 */
/* I_SYSTIME:
 *	This symbol is defined if the program should include <sys/time.h>.
 */
/* I_SYSTIMEKERNEL:
 *	This symbol is defined if the program should include <sys/time.h>
 *	with KERNEL defined.
 */
#define		I_TIME	 	/**/
#define	I_SYSTIME 	/**/
/*#undef	SYSTIMEKERNEL 	/**/

/* I_UTIME:
 *	This symbol, if defined, indicates that the file utime.h
 *	should be included instead of declaring our own utimbuf.
 */
/* I_USYSTIME:
 *	This symbol, if defined, indicates that the file sys/utime.h
 *	should be included instead of declaring our own utimbuf.
 */
#define	I_UTIME	/**/
/*#undef	I_SYSUTIME	/**/

/* PASSNAMES:
 *	This symbol, if defined, indicates that full names are stored in
 *	the /etc/passwd file.
 */
/* BERKNAMES:
 *	This symbol, if defined, indicates that full names are stored in
 *	the /etc/passwd file in Berkeley format (name first thing, everything
 *	up to first comma, with & replaced by capitalized login id, yuck).
 */
/* USGNAMES:
 *	This symbol, if defined, indicates that full names are stored in
 *	the /etc/passwd file in USG format (everything after - and before ( is
 *	the name).
 */
#define	PASSNAMES /*  (undef to take name from ~/.fullname) */
#define	BERKNAMES /* (that is, ":name,stuff:") */
/*#undef	USGNAMES  /* (that is, ":stuff-name(stuff):") */

/* SIG_TYPE:
 *	This symbol contains the type name of the signal handler functions.
 */
#define	SIGHAND_TYPE	void

/* XENIX:
 *	This symbol, if defined, indicates this is a Xenix system,
 *	for knocking  out the far keyword in selected places.
 */
/* BSD:
 *	This symbol, if defined, indicates this is a BSD type system,
 */
/*#undef	XENIX	/**/
/*#undef	BSD	/**/

