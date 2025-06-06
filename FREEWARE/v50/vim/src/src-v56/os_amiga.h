/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * Amiga Machine-dependent things
 */

#define CASE_INSENSITIVE_FILENAME   /* ignore case when comparing file names */
#define SPACE_IN_FILENAME
#define USE_FNAME_CASE		    /* adjust case of file names */
#define USE_TERM_CONSOLE
#define HAVE_AVAIL_MEM

#ifndef HAVE_CONFIG_H
# ifndef _DCC
#  define HAVE_STAT_H
# endif
# define HAVE_STDLIB_H
# define HAVE_STRING_H
# define HAVE_FCNTL_H
# define HAVE_STRCSPN
# define HAVE_STRICMP
# define HAVE_STRNICMP
# define HAVE_STRFTIME	    /* guessed */
# define HAVE_SETENV
# define HAVE_MEMSET
# define HAVE_QSORT
# if defined(__DATE__) && defined(__TIME__)
#  define HAVE_DATE_TIME
# endif

#ifndef MIN_FEAT
# define VIM_BACKTICK		/* internal backtick expansion */
#endif

#define ERRORFILE	"AztecC.Err"
#define MAKEEF		"t:vim##.Err"

/*
 * Be conservative about sizeof(int). It could be 4 too.
 */
#ifndef USE_GUI_GTK	/* avoid problems when generating prototypes */
# define SIZEOF_INT  2
#endif

#define BASENAMELEN	26	/* Amiga */

#define TEMPNAME	"t:v?XXXXXX"
#define TEMPNAMELEN	12

#endif /* HAVE_CONFIG_H */

#define FNAME_ILLEGAL ";*?`#%" /* illegal characters in a file name */

/*
 * Manx doesn't have off_t, define it here.
 */
#ifdef AZTEC_C
typedef long off_t;
#endif

#ifdef LATTICE
# define USE_TMPNAM	/* use tmpnam() instead of mktemp() */
#endif

/*
 * arpbase.h must be included before functions.h
 */
#ifndef NO_ARP
# include <libraries/arpbase.h>
#endif

/*
 * This won't be needed if you have a version of Lattice 4.01 without broken
 * break signal handling.
 */
#include <signal.h>

/*
 * Names for the EXRC, HELP and temporary files.
 * Some of these may have been defined in the makefile.
 */
#ifndef SYS_VIMRC_FILE
# define SYS_VIMRC_FILE "$VIM/vimrc"
#endif
#ifndef SYS_GVIMRC_FILE
# define SYS_GVIMRC_FILE "$VIM/gvimrc"
#endif
#ifndef SYS_MENU_FILE
# define SYS_MENU_FILE	"$VIMRUNTIME/menu.vim"
#endif
#ifndef VIM_HLP
# define VIM_HLP	"$VIMRUNTIME/doc/help.txt"
#endif
#ifndef FILETYPE_FILE
# define FILETYPE_FILE	"$VIMRUNTIME/filetype.vim"
#endif
#ifndef FTOFF_FILE
# define FTOFF_FILE	"$VIMRUNTIME/ftoff.vim"
#endif
#ifndef SYNTAX_FNAME
# define SYNTAX_FNAME	"$VIMRUNTIME/syntax/%s.vim"
#endif

#ifndef USR_EXRC_FILE
# define USR_EXRC_FILE	"s:.exrc"
#endif
#ifndef USR_EXRC_FILE2
# define USR_EXRC_FILE2	"home:.exrc"
#endif

#ifndef USR_VIMRC_FILE
# define USR_VIMRC_FILE	"s:.vimrc"
#endif
#ifndef USR_VIMRC_FILE2
# define USR_VIMRC_FILE2 "home:.vimrc"
#endif
#ifndef USR_VIMRC_FILE3
# define USR_VIMRC_FILE3 "$VIM/.vimrc"
#endif

#ifndef USR_GVIMRC_FILE
# define USR_GVIMRC_FILE "s:.gvimrc"
#endif
#ifndef USR_GVIMRC_FILE2
# define USR_GVIMRC_FILE2 "home:.gvimrc"
#endif
#ifndef USR_GVIMRC_FILE3
# define USR_GVIMRC_FILE3 "$VIM/.gvimrc"
#endif

#ifdef VIMINFO
#ifndef VIMINFO_FILE
# define VIMINFO_FILE	"s:.viminfo"
#endif
#endif /* VIMINFO */

#ifndef EXRC_FILE
# define EXRC_FILE	".exrc"
#endif

#ifndef VIMRC_FILE
# define VIMRC_FILE	".vimrc"
#endif

#ifndef GVIMRC_FILE
# define GVIMRC_FILE	".gvimrc"
#endif

#ifndef DEF_BDIR
# define DEF_BDIR	".,t:"	    /* default for 'backupdir' */
#endif

#ifndef DEF_DIR
# define DEF_DIR	".,t:"	    /* default for 'directory' */
#endif

#ifndef MAXMEM
# define MAXMEM		256	/* use up to 256Kbyte for buffer */
#endif
#ifndef MAXMEMTOT
# define MAXMEMTOT	0	/* decide in set_init */
#endif

#if defined(SASC)
int setenv(const char *, const char *);
#endif

#define mch_remove(x) remove((char *)(x))
#define mch_rename(src, dst) rename(src, dst)
#define mch_chdir(s) chdir(s)
