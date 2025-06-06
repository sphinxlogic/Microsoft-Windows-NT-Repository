/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * MSDOS Machine-dependent things.
 */

#include "os_dos.h"		/* common MS-DOS and Win32 stuff */

#define BINARY_FILE_IO
#define USE_EXE_NAME		/* use argv[0] for $VIM */
#define NO_COOKED_INPUT		/* mch_inchar() doesn't return whole lines */
#define SYNC_DUP_CLOSE		/* sync() a file with dup() and close() */
#define USE_TERM_CONSOLE
#ifdef DJGPP
# include <fcntl.h>		/* defines _USE_LFN */
# define USE_LONG_FNAME _USE_LFN    /* decide at run time */
# define USE_FNAME_CASE
#else
# define SHORT_FNAME		/* always 8.3 file name */
#endif
#define HAVE_STDLIB_H
#define HAVE_STRING_H
#define HAVE_FCNTL_H
#define HAVE_STRCSPN
#define HAVE_STRICMP
#define HAVE_STRFTIME		/* guessed */
#define HAVE_STRNICMP
#define HAVE_MEMSET
#define HAVE_QSORT
#if defined(__DATE__) && defined(__TIME__)
# define HAVE_DATE_TIME
#endif
#define BREAKCHECK_SKIP	    1	/* call mch_breakcheck() each time, it's fast */
#define HAVE_AVAIL_MEM

#ifndef MIN_FEAT
# define VIM_BACKTICK		/* internal backtick expansion */
#endif

#ifdef DJGPP
# ifndef USE_GUI_GTK		/* avoid problems when generating prototypes */
#  define SIZEOF_INT 4		/* 32 bit ints */
# endif
# define DOS32
#else
# ifndef USE_GUI_GTK		/* avoid problems when generating prototypes */
#  define SIZEOF_INT 2		/* 16 bit ints */
# endif
# define SMALL_MALLOC		/* 16 bit storage allocation */
# define DOS16
#endif

/*
 * Borland C++ 3.1 doesn't have _RTLENTRYF
 */
#ifdef __BORLANDC__
# if __BORLANDC__ < 0x450
#  define _RTLENTRYF
# endif
#endif

#define FNAME_ILLEGAL "\"*?><|" /* illegal characters in a file name */

#include <dos.h>
#include <dir.h>
#include <time.h>

#ifdef DJGPP
# include <unistd.h>
# define HAVE_LOCALE_H
# define setlocale(c, p)    djgpp_setlocale()
#endif

#ifndef DJGPP
typedef long off_t;
#endif

/*
 * Try several directories to put the temp files.
 */
#define TEMPDIRNAMES	"$TMP", "$TEMP", "c:\\TMP", "c:\\TEMP", ""
#define TEMPNAME	"v?XXXXXX"
#define TEMPNAMELEN	128

#ifndef MAXMEM
# define MAXMEM		256		/* use up to 256Kbyte for buffer */
#endif
#ifndef MAXMEMTOT
# define MAXMEMTOT	0		/* decide in set_init */
#endif

#ifdef DJGPP
# define BASENAMELEN  (_USE_LFN?250:8)	/* length of base of file name */
#else
# define BASENAMELEN	    8		/* length of base of file name */
#endif

/* codes for msdos mouse event */
#define MSDOS_MOUSE_LEFT	0x01
#define MSDOS_MOUSE_RIGHT	0x02
#define MSDOS_MOUSE_MIDDLE	0x04

#ifdef DJGPP
int mch_rename(const char *OldFile, const char *NewFile);
#else
# define mch_rename(src, dst) rename(src, dst)
#endif

#define mch_setenv(name, val, x) setenv(name, val, x)
