/*
 * $XFree86: mit/util/makedepend/def.h,v 2.1 1994/02/10 21:27:52 dawes Exp $
 * $XConsortium: def.h,v 1.17 91/05/13 10:23:29 rws Exp $
 */
#include <X11/Xos.h>
#include <stdio.h>
#include <ctype.h>
#ifndef X_NOT_POSIX
#ifndef _POSIX_SOURCE
#if !(BSD >= 199103)
#define _POSIX_SOURCE
#endif
#endif
#endif
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>

#define MAXDEFINES	512
#define MAXFILES	512
#define MAXDIRS		30
#define SYMTABINC	10	/* must be > 1 for define() to work right */
#define	TRUE		1
#define	FALSE		0

/* the following must match the directives table in main.c */
#define	IF		0
#define	IFDEF		1
#define	IFNDEF		2
#define	ELSE		3
#define	ENDIF		4
#define	DEFINE		5
#define	UNDEF		6
#define	INCLUDE		7
#define	LINE		8
#define	PRAGMA		9
#define ERROR           10
#define IDENT           11
#define SCCS            12
#define ELIF            13
#define EJECT           14
#define IFFALSE         15     /* pseudo value --- never matched */
#define ELIFFALSE       16     /* pseudo value --- never matched */
#define INCLUDEDOT      17     /* pseudo value --- never matched */
#define IFGUESSFALSE    18     /* pseudo value --- never matched */
#define ELIFGUESSFALSE  19     /* pseudo value --- never matched */

#ifdef DEBUG
extern int	_debugmask;
/*
 * debug levels are:
 * 
 *     0	show ifn*(def)*,endif
 *     1	trace defined/!defined
 *     2	show #include
 *     3	show #include SYMBOL
 *     4-6	unused
 */
#define debug(level,arg) { if (_debugmask & (1 << level)) warning arg; }
#else
#define	debug(level,arg) /**/
#endif /* DEBUG */

typedef	unsigned char boolean;

struct symtab {
	char	*s_name;
	char	*s_value;
};

struct	inclist {
	char		*i_incstring;	/* string from #include line */
	char		*i_file;	/* path name of the include file */
	struct inclist	**i_list;	/* list of files it itself includes */
	int		i_listlen;	/* length of i_list */
	struct symtab	*i_defs;	/* symbol table for this file */
	struct symtab	*i_lastdef;	/* last symbol defined */
	int		i_deflen;	/* number of defines */
	boolean		i_defchecked;	/* whether defines have been checked */
	boolean		i_notified;	/* whether we have revealed includes */
	boolean		i_marked;	/* whether it's in the makefile */
	boolean		i_searched;	/* whether we have read this */
	boolean         i_included_sym; /* whether #include SYMBOL was found */
					/* Can't use i_list if TRUE */
};

struct filepointer {
	char	*f_p;
	char	*f_base;
	char	*f_end;
	long	f_len;
	long	f_line;
};

#ifndef X_NOT_STDC_ENV
#include <stdlib.h>
#if defined(macII) && !defined(__STDC__)  /* stdlib.h fails to define these */
char *malloc(), *realloc();
#endif /* macII */
#else
char			*malloc();
char			*realloc();
#endif

char			*copy();
char			*basename();
char			*getline();
struct symtab		*slookup();
struct symtab		*isdefined();
struct symtab		*fdefined();
struct filepointer	*getfile();
struct inclist		*newinclude();
struct inclist		*inc_path();
