#ifndef CONFIG_H
#define CONFIG_H
/*
 * $Header: /net/objy27/wrld/mnt11/ricks/src/master/xrn/config.h,v 1.10 1993/02/04 18:22:14 ricks Exp $
 */

/*
 * xrn - an X-based NNTP news reader
 *
 * Copyright (c) 1988-1993, Ellen M. Sentovich and Rick L. Spickelmier.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the University of California not
 * be used in advertising or publicity pertaining to distribution of 
 * the software without specific, written prior permission.  The University
 * of California makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty.
 *
 * THE UNIVERSITY OF CALIFORNIA DISCLAIMS ALL WARRANTIES WITH REGARD TO 
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND 
 * FITNESS, IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE FOR
 * ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * config.h: configurable defaults
 */

#if defined(__ultrix) && defined(__STDC__) && !defined(ultrix)
#define ultrix
#endif

#if defined(__STDC__) && (__STDC__)
#define STDC
#endif

/* place to report bugs and request new features */
#ifndef GRIPES
#define GRIPES "murphy@dco.dec.com"
#endif

/* if you want xrn to provide a titlebar */
/* #define TITLEBAR */

/* display compilation time in the XRN title bar */
#define WANT_DATE

/* if you want a short icon name */
#define SHORT_ICONNAME

/* define to use the DEC cursor font */
#if defined(ultrix) || defined (VMS) || defined (__osf__)
#define USE_DEC_CURSOR
#endif

/*
 * Note: You can probably skip ahead to the DISTRIBUTION section if you are
 * using InterNetNews - INN
 */

/*
 * your internet domain name, examples are:
 *
 * .Berkeley.EDU
 * .mit.EDU
 * .CSS.GOV
 * .CS.NET
 * .BITNET
 * .UUCP
 *
 * wouldn't it be nice if there were a call like 'gethostname'
 * that would get the domain name...
 *
 * DOMAIN_NAME can be overriden with the DOMAIN environment variable 
 * or the DOMAIN_FILE (see below)
 *
 * NOTE: use of the RESOLVER define should make DOMAIN_NAME obsolete 
 *
 */
#ifndef DOMAIN_NAME
#define DOMAIN_NAME "" /* Shouldn't be needed because of resolve! */
#endif

/*
 * Define RETURN_HOST if you want a default return address host name.
 * Useful if your site uses MX records or something to address all
 * mail to a pseudo host (e.g. user@Berkeley.EDU)
 */
/*#define RETURN_HOST "NoHost.NoDomain"*/

/* organization name */
#ifndef ORG_NAME
#define ORG_NAME	" "  /* Supply only through ORG_FILE */
#endif

/* uucp netnews name (if different from your internet host name) */
/* #define UUCPNAME  "/etc/uucpname" */

/* define this if you don't want xrn to add the hostname to the Path field */
/* #define HIDE_PATH */

#ifndef VMS
/*
 * name of the nntp server is in this file - can be overriden
 * by command line option, X resource, or environment variable
 */
#ifndef SERVER_FILE
#define SERVER_FILE "/usr/local/lib/rn/server"
#endif
/*
 * name of the host to use as the users host name in the From field
 * in a composition is in thie file
 */
#ifndef HIDDEN_FILE
#define HIDDEN_FILE "/usr/local/lib/news/hiddenhost"
#endif
/*
 * name of the host to use as the users hosts name in the Path field 
 * in a composition is in the file
 */
#ifndef PATH_FILE
#define PATH_FILE "/usr/local/lib/news/pathhost"
#endif
/*
 * name of the internet domain is in this file
 */
#ifndef DOMAIN_FILE
#define DOMAIN_FILE "/usr/local/lib/news/domain"
#endif
/*
 * name of the organization is in this file
 */
#ifndef ORG_FILE
#define ORG_FILE "/usr/local/lib/news/organization"
#endif
#else /* VMS */
#define SERVER_FILE "SYS$LOGIN:NNTP.SERVER"
#define HIDDEN_FILE "SYS$LOGIN:HIDDEN.HOST"
#define PATH_FILE "SYS$LOGIN:PATH.HOST"
#define DOMAIN_FILE "SYS$LOGIN:DOMAIN.NAME"
#define ORG_FILE "SYS$LOGIN:ORG.NAME"
#endif /* VMS */

/* default distribution */
#ifndef DISTRIBUTION
#define DISTRIBUTION "world"
#endif

/*
 * maximum size of a signature file (in bytes), if file is bigger than this,
 * it is not included
 */
#define MAX_SIGNATURE_SIZE 1000

/*
 * Prefix for each line included from an article
 */
#define INCLUDEPREFIX	"|> "

/*
 * for author names: email address (default) or fullname
 */
#define AUTHOR_FULLNAME

/*
 * number of article headers to prefetch when searching subjects
 * in the backwards direction
 */
#define SUBJECTS	10


/*
 * use the name server to find out your official (with domain) host name
 */
#ifndef RESOLVER
#define RESOLVER
#endif

/*
 * For debugging only, define if you want core dumps, rather than
 * a death notification box and an attempt at updating the .newsrc
 * and cleaning up the temporary files
 *
 */
/* #define DUMPCORE */

/*
 * use MOTIF widgets
 */
/* #define MOTIF */

/*
 * for various bugs in the toolkit / widget set
 */
/* These defines are probably no longer needed */
/* #define TRANSLATIONBUG */	/* DEC (VMS) X windows release		*/
/* #define ERRORBUG */		/* DEC (VMS) X windows release		*/
/* #define DECFOCUSPATCH */	/* for certain DEC window managers	*/

/*
 * If XFILESEARCHPATH is defined, then its contents are treated as a string 
 * which is appended to the end of the XFILESEARCHPATH environment variable.
 * The purpose of this is that if you are planning on installing xrn
 * (and its app-defaults file) in a non-standard location, you can make 
 * sure that it will find its app-defaults file by compiling the program 
 * with the path to the file set in XFILESEARCHPATH (for example, when 
 * I compile the program I #define XFILESEARCHPATH to be 
 * "/usr/sipb/lib/%T/%N", since I install it in the Student Information 
 * Processing Board (SIPB) filesystem.  This patch addresses a general 
 * flaw in X11R4's (and X11R3's) handling of application defaults file --
 * there is no way for the programmer to suggest to the toolkit
 * where to look for the file without mucking with environment variables.
 *
 * (Jonathan I. Kamens <jik@pit-manager.mit.edu>)
 */
/* #define XFILESEARCHPATH "path"*/

/*
 * generate Message-ID and Date fields
 *
 *   NNTP POST should handle this, but does not for some
 *   implementations.  Do not define this if you are using INN.
 */
/*#define GENERATE_EXTRA_FIELDS*/

/*
 * environment variable containing timezone name
 *
 *   Used only with GENERATE_EXTRA_FIELDS. Since gmtime is allowed to
 *   return NULL, we must be prepared to use localtime and provide a
 *   timezone specification. This macro defines the name of the environment
 *   variable containing the time zone.
 */
/* #define TIMEZONE_ENV "UUCP_TIME_ZONE" */

/*
 * don't use the XHDR NNTP command, use HEADER and cache information...
 */
/* #define DONT_USE_XHDR */

/*
 * don't use the XHDR NNTP command for getting a single header field
 * from a single article (for performance reasons), use HEADER and 
 * cache information...
 */
/* #define DONT_USE_XHDR_FOR_A_SINGLE_ITEM */

/*
 * the active file of most currently used news systems has a problem:
 *   if the first article number equals the last article number for a
 *   newsgroup, this means that there are 0 OR 1 ARTICLES.
 *
 * Cnews, INN, and and Bnews 2.11 (patch #19) have fixed this problem.
 */
#ifdef INN
#define FIXED_ACTIVE_FILE
#endif /* INN */

/*
 * deal with stupid C-news problem of NOT updating the low number
 * of the group entries in the active file - define this if you
 * have fixed the problem
 */
/* #define FIXED_C_NEWS_ACTIVE_FILE */

/*
 * old versions of NNTP (pre-1.5.11) read the 'active' file only once
 * per session, so reissuing the 'LIST' command would always return
 * the EXACT same data. New versions of NNTP will reread the file
 * for each LIST command.  If you are using 1.5.11 or greater, or INN
 * define NNTP_REREADS_ACTIVE_FILE
 */

#ifdef INN
#define NNTP_REREADS_ACTIVE_FILE
#endif /* INN */


/*
 * Do you want to use inews for postings?  For INN, it is suggested that you do.
 */
/* #define INEWS "/usr/lib/news/inews" */

/*
 * Does INEWS read the signature file? The version of INEWS for INN
 * reads the signature file by default.  If you define this incorrectly
 * you will probably end up with two signatures on postings.
 */
/* #define INEWS_READS_SIG */

/*
 * Define if you want AUTHINFO support for authorization for
 * the NNTP server. (see clientlib.c).
 */
/* #define AUTHINFO */

/*
 * display articles with local time rather than GMT
 *
 *  if you are running SunOS 3.5, get rid of the '|| defined(sun)'
 */
#define USE_LOCALTIME
#if defined(USE_LOCALTIME) && (defined(SYSV) || defined(ultrix) || defined(apollo) ||  defined(sun))
#define REALLY_USE_LOCALTIME
#endif

/*
 * non-display defaults
 */
#ifndef PRINTCOMMAND
#ifndef VMS
#define PRINTCOMMAND	"lpr"
#else
#define PRINTCOMMAND   "PRINT/DELETE"
#endif
#endif

#ifndef SENDMAIL
#ifndef VMS
#define SENDMAIL       "/usr/lib/sendmail -oi -t"
#else
#define SENDMAIL       "MAIL "
#endif
#endif

#define SAVEMODE       "normal,headers,onedir"
#define SAVE_DIR_DEFAULT	ONEDIR_SAVE
#ifndef VMS
#define SAVEDIR        "~/News"
#define NEWSRCFILE     "~/.newsrc"
#define SAVENEWSRCFILE "~/.oldnewsrc"
#define SIGNATUREFILE  "~/.signature"
#else
#define SAVEDIR	       "SYS$SCRATCH:"
#define NEWSRCFILE     "SYS$LOGIN:NEWS.RC"
#define SAVENEWSRCFILE "SYS$LOGIN:OLDNEWS.RC"
#define SIGNATUREFILE  "SYS$LOGIN:NEWS.SIGNATURE"
#endif
#define TOPLINES	"10"
#define MINLINES	"3"
#define MAXLINES	"8"
#define TEXTLINES	"24"
#define CANCELCOUNT	"20"
#define LINELENGTH	"0"
#define BREAKLENGTH	"0"
/* ONLYSHOW set to "0" turns off the feature */
#define ONLYSHOW	"0"
/* RESCAN_TIME is in seconds, default is 300 seconds (5 minutes) */
#define RESCAN_TIME	"300"

#ifndef VMS
#define DEADLETTER     "~/dead.letter"
#define SAVEPOSTINGS   "~/Articles"
#define TEMPORARY_DIRECTORY "/tmp"
#else
#define DEADLETTER     "SYS$SCRATCH:DEAD.LETTER"
#define SAVEPOSTINGS   "SYS$SCRATCH:SAVED.POSTING"
#define TEMPORARY_DIRECTORY "SYS$SCRATCH:"
#endif

#define WATCH

/* definitions to convert from e.g. ISO-646 to ISO-8859 */
/* #define XLATE */
/* define the strings of equivalent characters */
#ifdef XLATE
#ifdef SWEDISH
/*	Swedish ISO-646 to ISO-8859-1 */
#define XLATE_FROM	"[]\\{}|$@^`~"
#define XLATE_TO	"\304\305\326\344\345\366\244\311\334\351\374"
#endif /* SWEDISH */

#ifdef NORWEGIAN
/*
        Norwegian ISO-646 to ISO-8859-1  - contributed by
	Per Steinar Iversen, iversen@vsfys1.fi.uib.no
 */
#define XLATE_FROM   "[]\\{}|"
#define XLATE_TO     "\306\305\330\346\345\370"
#endif /* NORWEGIAN */

#endif /* XLATE */


/*
 *
 * End of User/Site Configuration Parameters
 *
 */

/* SYSTEM V regex package */
#if defined(macII) || defined(aiws) || defined(hpux) || (defined(SYSV) && !defined(i386)) || (defined(i386) && !defined(linux))
#define SYSV_REGEX
#if !defined(SYSV)
#define SYSV
#endif
#endif

/* SYSTEM V doesn't have index/rindex...*/			    /* ggh */
#ifdef SYSV							    /* ggh */
#define index strchr						    /* ggh */
#define rindex strrchr						    /* ggh */
#endif								    /* ggh */

/* vfork supported */
#if defined(ultrix) || defined(sun) || defined(apollo) || (defined(sony) && !defined(SVR4))
#define VFORK_SUPPORTED
#endif

/* define this if your popen uses vfork - ultrix uses fork... */
#if !defined(VFORK_SUPPORTED) || (!defined(ultrix) && (!defined(SYSV) || defined(sgi)))
#define POPEN_USES_INEXPENSIVE_FORK
#endif

/* bsd b* functions */
#if defined(sequent) || !defined(SYSV)
#ifndef VMS
#define BSD_BFUNCS
#endif
#endif

/* v{s,f}printf functions */
#if defined(sequent) || defined(ibm032) || defined(hp300) || (defined(sony) && !defined(SYSV) && !defined(_ANSI_C_SOURCE))
#define NEED_VPRINTF
#endif


/*
 * define this if your system C library doesn't have the tempnam
 * function - note that there a number of buggy tempnam implementations
 * in various vendors libc's.  You might want to always define NEED_TEMPNAM
 * and not use the libc one at all.
 */
/*
#if defined(convex) || (defined(sony) && !defined(SVR4)) || (!defined(sun) && !defined(ultrix) && !defined(__osf__) && !defined(SYSV) && !defined(linux) && !defined(_XOPEN_SOURCE)) || defined(ibm032) || defined(sequent) || defined(_IBMR2)
*/
/* Always define NEED_TEMPNAM */
#define NEED_TEMPNAM
/*
#else
#define utTempnam tempnam
#endif
*/
     
/* strtok function */
#if defined(sequent) || (!defined(SYSV) && !defined(_ANSI_C_SOURCE))
#define NEED_STRTOK
#endif

/* strstr function */
#if !defined(_ANSI_C_SOURCE)
#define NEED_STRSTR
#endif

#if defined(sun) || defined(hpux) || defined(clipper) || (defined(sony) && defined(SYSV)) || (defined(i386) && defined(SYSV))
#define NEED_STRCASECMP
#endif

#if defined(mips) || defined(hpux) || defined(sun) || (defined(i386) && !defined(sequent)) || defined(_IBMR2)
#define USE_PUTENV
#endif

#ifndef _ARGUMENTS
#if defined(FUNCPROTO) || defined(__STD__) || defined(__cplusplus) || defined(c_plusplus) || defined(STDC)
#define _ARGUMENTS(arglist) arglist
#else
#define _ARGUMENTS(arglist) ()
#endif
#endif

#endif /* CONFIG_H */
