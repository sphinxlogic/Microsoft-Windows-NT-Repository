/* config.h 1.1	(c) copyright 1986 (Dan Heller) */

/* Default names and locations for files */
#define MAILRC		".mushrc"
#define ALTERNATE_RC	".mailrc"
#define DEFAULT_RC	"/etc/mail/mush/Mushrc"
#define ALT_DEF_RC	"/etc/mail/mush/Mail.rc"
#define SIGNATURE	".signature"
#define FORTUNE		"/usr/games/fortune"
#define COMMAND_HELP	"/etc/mail/mush/cmd_help"
#ifdef SUNTOOL
#    define TOOL_HELP	"/usr/lib/tool_help"
#endif /* SUNTOOL */
#define ALTERNATE_HOME	"/tmp"       /* Path must be read/write to EVERYONE */
#define EDFILE  	".edXXXXXX"  /* file/pathname added to user's "home" */

#define LS_COMMAND	"ls"
#define LPR		"lpr"
#define DEF_PRINTER	"lp"
/* If your lpr command understands only -P or -d (or some other flag) then
 * define PRINTER_OPT to the appropriate value.  If you want to be able to
 * use either one, don't define this at all.  The defaults (when neither
 * -P nor -d is used on the mush "lpr" command line) are as noted here.
 * If your lpr requires that the option and the printer name be separate
 * arguments, include a trailing space in this definition.
 */
#ifdef SYSV
#define PRINTER_OPT	"-d"
#endif /* SYSV */
#ifdef BSD
#define PRINTER_OPT	"-P"
#endif /* BSD */

/* default settings for some variable strings */
#define DEF_PROMPT	"Msg %m of %t: "
#define DEF_PAGER	"more" /* set to "internal" to use internal pager */
#define DEF_SHELL	"csh"
#define DEF_EDITOR	"vi"
#define DEF_FOLDER	"~/Mail"        /* default Mail folder */
#define DEF_MBOX	"~/mbox"	/* default mbox */
#define DEF_INDENT_STR	"> "		/* indent included mail */
#define DEF_ESCAPE	"~"
#define DEF_HDR_FMT	"%25f %7d (%l/%c) \"%s\"" /* default hdr_format */
#define DEF_CURSES_HELP	\
    "display save mail reply next-msg back-msg screen-next screen-back"

/* Headers that will NOT be included when forwarding mail */
#define IGNORE_ON_FWD	"status,priority,return-receipt-to"

#define	MAXMSGS	1000		/* Maximum number of messages we can read */
#define HDRSIZ	(2*BUFSIZ)	/* This should not be < BUFSIZ! */

/*
 * Define INTERNAL_MALLOC and recompile if you have trouble with mush
 * core-dumping due to malloc/free errors.  Also, if you run a System 5
 * variant, you might notice a performance improvement if you define this
 * variable.  It uses the malloc distributed by Larry Wall for perl v2.
 */
/* #define INTERNAL_MALLOC /**/

/*
 * Define TIMEZONE if your system has neither the SysV external variable
 * tzname nor the BSD timezone() function.  The example below is for
 * Gould BSD4.3 systems; others should define it as a string, e.g. "PST"
 * If TIMEZONE is defined, DAYLITETZ can also be defined, e.g. "PDT"
 *
 * Define USA if you are using US/NorthAmerican time zone abbreviations.
 * If USA is not defined, dates in outgoing mail will include timezones
 * specified as offsets from GMT, e.g. Pacific Standard Time is -0800.
 */
/* #define TIMEZONE T->tm_zone /**/
/* #define USA /**/

/* mail delivery system macros and defines... */

/*
 * If you are using MMDF, define MMDF here.
 */
/* #define MMDF /**/
#if defined(M_UNIX) && defined(M_XENIX)
#define MMDF
/* #define M_EXECMAIL	/* Define if you haven't fixed submit permissions */
#endif /* M_UNIX && M_XENIX */
#ifdef MMDF
/*
 * If MMDF delivers mail the user's home directory, define HOMEMAIL.
 * Also check the definition of the delivery file name MAILFILE, below.
 */
/* #define HOMEMAIL /**/
#ifdef M_EXECMAIL
#define MAIL_DELIVERY	"/usr/lib/mail/execmail"
# define VERBOSE_ARG	"-v"
# define METOO_ARG	"-m"
#else /* M_EXECMAIL */
#define MAIL_DELIVERY	"exec /usr/mmdf/bin/submit -mlnr"
#define VERBOSE_ARG	"Ww"
#define MTA_EXIT	9	/* exit status for successful submit */
#endif /* M_EXECMAIL */
#else /* MMDF */
/*
 * If you are not using MMDF, check these definitions.
 */
#define MAIL_DELIVERY	"/bin/rmail" /* "-i" works like "-oi" */
#define VERBOSE_ARG	""    /* undef if none exists */
#define METOO_ARG	""    /* man sendmail for more info. */
#define MTA_EXIT	0	/* exit status for successful mail delivery */
#endif /* MMDF */

/* If your mail transfer agent uses something *besides* "From " to separate
 * adjacent messages in a folder, define MSG_SEPARATOR to be this string.
 * If that string is 4 ^A's, then the string would be "\001\001\001\001".
 * With the exception of MMDF, below, you should OMIT a trailing newline
 * from the setting of MSG_SEPARATOR.
 * If you don't know what any of this means, leave it alone.
 */
/* #define MSG_SEPARATOR "From " /**/
#ifdef MMDF
/*
 * These values should be identical (respectively) to the contents of
 * delim1 and delim2 in MMDFSRC/conf/yoursite/conf.c (sans newline).
 */
#define MSG_SEPARATOR	"\001\001\001\001\n"
#define END_MSG_SEP	"\001\001\001\001\n"
/*
 * You only need to define LCKDFLDIR if you have MMDF configured to use the
 * locking routines in lib/util/lk_lock.c (ie., link(2)-based locking).
 * Most of you WILL NOT need this, since you probably use one of the more
 * sophisticated locking modules provided with MMDF.  Remember to alter the
 * Makefile so as to access the MMDF library at the link step.
 */
/* #define LCKDFLDIR	"/usr/spool/mmdf/lockfiles" /* (for example) */
#else /* !MMDF */
#ifdef M_XENIX
#define DOT_LOCK	/* DOT_LOCK should be used for SCO Xenix */
#endif /* M_XENIX */
#endif /* MMDF */


#ifdef HOMEMAIL
#define MAILFILE	"Mailbox"	/* or whatever */
#else /* HOMEMAIL */
#ifndef MAILDIR
#if defined(SYSV) && !defined(SVR4)
#define MAILDIR		"/usr/mail"
#else /* BSD || SVR4 */
#define MAILDIR		"/var/mail"
#endif /* SYSV && !SVR4 */
#endif /* MAILDIR */
#endif /* HOMEMAIL */

/* If your mailer does not understand commas between addresses, you should
 * define NO_COMMAS.  This includes pre-3.0 smail and default MTAs used on
 * xenix, and sys-v systems.
 * This does NOT apply to MMDF or sendmail, in most cases.  SunOS 4.1 as
 * shipped needs this, but note that if you are allowing mush to default
 * to SUN_4_1 (see the end of this file) you won't get this.
 */
#ifdef SUN_4_1		/* SunOS 4.1 has warped sendmail.cf */
#define NO_COMMAS /**/
#endif /* SUN_4_1 */

#define NO_COMMAS /**/
/*
 * Most RFC822-compliant mailers (sendmail) will add the headers From:
 * and Date: on outgoing mail.  If the user or UA sends these headers,
 * such MTAs will generally use the headers as the user or UA set them.
 * Mailers that are not RFC822-compliant sometimes fail to add these
 * headers at all, so the UA must supply them.  Mush normally does so.
 *
 * However, there are certain MTAs which will insist on adding new From:
 * and Date: headers.  These "picky mailers" may remove the user or UA 
 * From: and Date: headers, or they may leave them but insert a leading
 * '>', which makes the headers very ugly and redundant or contradictory.
 * PICKY_MAILER prevents mush from adding From: or Date:, so the MTA can.
 *
 * Many RFC822-compliant mailers are "picky", including certain sendmail
 * configurations.  It is advisable to set PICKY_MAILER *UNLESS* your MTA
 * is known not to be RFC822 compiant (xenix, older sys-v) or can be
 * counted on to pass through user-defined headers without modification.
 */
/* #define PICKY_MAILER /**/

/* If your system supports the vprintf() functions, True for sys-v and
 * later sun versions (3.0+ ?).  Typically not true for BSD systems, but
 * that will probably change in the future.
 */
#if defined(SYSV) || defined(sun)
#define VPRINTF
#endif /* SYSV || sun */

/* If your system uses the getwd() system call (as opposed to getcwd()),
 * and your system is not a BSD system (e.g. MIPS), define GETWD below.
 */
/* #define GETWD	/**/

/* If your system has the BSD-style directory routines (opendir/readdir),
 * define DIRECTORY.  This happens automatically in glob.h for BSD.
 */
#ifdef SVR4
#ifndef DIRECTORY
#define DIRECTORY
#endif
#endif /* SVR4 */

/* If your system has the POSIX "utimbuf" structure for utime(2),
 * define POSIX_UTIME.  You may have to change <sys/utime.h> to <utime.h>
 * in mush.h.
 */
#if defined(SVR4) && !defined(sun)
#define POSIX_UTIME
#endif /* SVR4 && !sun */

/* The remainder of this file attempt to automatically supply some sanity.
 * You shouldn't need to change this part unless your system is really odd.
 */

#ifdef BSD
#    undef SYSV /* Sorry, can't do both BSD and SYSV */
#    undef SVR4
#    undef SVR3
#    undef SVR2
#endif /* BSD */
#if defined(SUNTOOL) || defined(SUN_3_5) || defined(SUN_4_0) || defined(SUN_4_1)
#if !defined(BSD) && !defined(SYSV)
#    define BSD /* default to BSD */
#endif /* !BSD && !SYSV */
#if !defined(SUN_3_5) && !defined(SUN_4_0)
#    ifndef SUN_4_1
#        define SUN_4_1 /* default to sun 4.1 */
#    endif /* SUN_4_1 */
#    define SUN_4_0 /* 4.0 stuff needed too */
#endif /* !SUN_3_5 && !SUN_4_0 */
#ifdef SUN_4_0
#    undef SUN_3_5
#    undef SIGRET
#    define SIGRET void
#endif /* SUN_4_0 */
#endif /* SUNTOOL || SUN_3_5 || SUN_4_0 || SUN_4_1 */
#ifdef SVR4
#    undef SVR3
#    undef SYSV
#    define SYSV
#else
#ifdef SYSV
#    undef SVR3
#    ifndef SVR2
#        define SVR3
#    endif /* SVR2 */
#endif /* SYSV */
#endif /* SVR4 */
#ifdef SVR3
#    undef SYSV
#    define SYSV
#endif /* SVR3 */
