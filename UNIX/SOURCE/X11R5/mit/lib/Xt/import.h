/* libXt : import.h
 * indirection defines
 * Template created by mkshtmpl.sh 1.4
 * Copyright (c) 1992, 1993 by Thomas Wolfram, Berlin, Germany
 * (thomas@aeon.in-berlin.de, wolf@prz.tu-berlin.de)
 * Conception derived partially from work of Thomas Roell
 *
 * $XFree86: mit/lib/Xt/import.h,v 2.0 1993/09/22 15:33:05 dawes Exp $
 */

#ifndef _libXt_import
#define _libXt_import
#ifdef SVR3SHLIB

#define _ctype  (*_libXt__ctype)
#define _iob  (*_libXt__iob)
#define access  (*_libXt_access)
#define atof  (*_libXt_atof)
#define calloc  (*_libXt_calloc)
#define errno  (*_libXt_errno)
#define exit  (*_libXt_exit)
#define fclose  (*_libXt_fclose)
#define fopen  (*_libXt_fopen)
#define fprintf  (*_libXt_fprintf)
/* prevent name conflict with member free of XFontSetMethods */
#define free(ptr) (*_libXt_free)(ptr)
#define getenv  (*_libXt_getenv)
#define gethostname  (*_libXt_gethostname)
#define getpwnam  (*_libXt_getpwnam)
#define getpwuid  (*_libXt_getpwuid)
#define gettimeofday  (*_libXt_gettimeofday)
#define getuid  (*_libXt_getuid)
#define malloc  (*_libXt_malloc)
#define memset  (*_libXt_memset)
#define printf  (*_libXt_printf)
#define qsort  (*_libXt_qsort)
#define realloc  (*_libXt_realloc)
/* prevent name conflict with bitfield select of _XtEventRec */
#define select(max, rd, wr, ex, to) (*_libXt_select)(max, rd, wr, ex, to)
#define sprintf  (*_libXt_sprintf)
/* prevent name conflict with struct stat */
#define stat(path, buf) (*_libXt_stat)(path, buf)
#define strcat  (*_libXt_strcat)
#define strchr  (*_libXt_strchr)
#define strcmp  (*_libXt_strcmp)
#define strcpy  (*_libXt_strcpy)
#define strncpy  (*_libXt_strncpy)
#define strrchr  (*_libXt_strrchr)
/* #define _XtInherit  (*_libXt__XtInherit)  /* must be imported */
#define XtCXtToolkitError  (*_libXt__XtCXtToolkitError)
#define _XtGlobalTM  (*_libXt__XtGlobalTM)
/* #define _XtInheritTranslations  (*_libXt__XtInheritTranslations) */
#define _XtQString  (*_libXt__XtQString)
/*
#define XtShellStrings  (*_libXt_XtShellStrings)
#define XtStrings  (*_libXt_XtStrings)
*/

/*
 * Composite.c
 */
#define compositeWidgetClass  (*_libXt_compositeWidgetClass)

/*
 * Constraint.c
 */
#define constraintWidgetClass  (*_libXt_constraintWidgetClass)

/*
 * Core.c
 */
#define coreWidgetClass  (*_libXt_coreWidgetClass)

/*
 * Object.c
 */

/*
 * RectObj.c
 */
#define rectObjClass  (*_libXt_rectObjClass)

/*
 * Shell.c
 */
#define shellWidgetClass  (*_libXt_shellWidgetClass)
#define overrideShellWidgetClass  (*_libXt_overrideShellWidgetClass)
#define wmShellWidgetClass  (*_libXt_wmShellWidgetClass)
#define transientShellWidgetClass  (*_libXt_transientShellWidgetClass)
#define topLevelShellWidgetClass  (*_libXt_topLevelShellWidgetClass)
#define applicationShellWidgetClass  (*_libXt_applicationShellWidgetClass)

/*
 * Vendor.c
 */
#define vendorShellWidgetClass  (*_libXt_vendorShellWidgetClass)


/* Imported functions declarations
 * Why declaring some imported functions here?
 * This should be done gracefully through including of the systems
 * header files. Unfortunatly there are some source files don't
 * include all headers they should include, there are also some functions
 * nowhere declared in the systems headers and some are declared
 * without extern and cause problems since the names are redefined
 * and these declarations then become undesired false pointer definitions.
 */

/* This section needs editing! It's only an example for the X libs. */

#ifdef __STDC__
extern int creat(char const *, unsigned short);
extern void exit(int);
#ifndef SCO
extern int printf(char const *, ...);
extern int sprintf(char *, char const *, ...);
extern int sscanf(char *, char const *, ...);
#endif
extern int atoi(char const *);
extern int access(char const *, int);
extern unsigned int alarm(unsigned int);
extern unsigned int sleep(unsigned int);
extern int close(int);
extern int read(int, char *, unsigned int);
extern int write(int, char const *, unsigned int);
extern int grantpt(int);
extern char *ptsname(int);
extern int unlockpt(int);
extern char *getenv(char const *);
#else
extern int creat();
extern void exit();
#ifndef SCO
extern int printf();
extern int sprintf();
extern int sscanf();
#endif
extern int atoi();
extern int access();
extern unsigned int alarm();
extern unsigned int sleep();
extern int close();
extern int read();
extern int write();
extern int grantpt();
extern char *ptsname();
extern int unlockpt();
extern char *getenv();
#endif

/* use char * also for __STDC__, the sources want it */ 
extern char *malloc(), *realloc(), *calloc();

#include <string.h>

extern char *sys_errlist[];
extern int sys_nerr;

extern void qsort();
#ifndef ISC40
extern int _flsbuf();
extern int _filbuf();
#endif
extern int ioctl();
extern int getmsg();
extern int putmsg();

extern unsigned short ntohs(), htons();
extern unsigned long ntohl(), htonl();
extern unsigned long inet_addr();
extern int connect();
extern int gethostname();
extern int setsockopt();
extern int socket();
extern int writev();
extern int gettimeofday();

#if defined(ISC202) || defined(ATT)
extern int (*_libXt_stat)();
#endif

/* Functions with ambiguous names */

#ifdef __STDC__
extern int close(int);
extern void free(void *);
#include <sys/time.h>
#include <sys/bsdtypes.h>
#ifdef SCO
#include <sys/types.h>
#endif /* SCO */
extern int select(int, fd_set *, fd_set *, fd_set *, struct timeval *);
#else
extern int (*_libXt_close)();
extern void (*_libXt_free)();
extern int (*_libXt_select)();
#endif

extern void (*_libXt__XtInherit)(
#if NeedFunctionPrototypes
    void
#endif
);

#endif
#endif

