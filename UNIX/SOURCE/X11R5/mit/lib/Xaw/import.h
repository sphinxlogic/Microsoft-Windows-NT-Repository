/* libXaw : import.h
 * indirection defines
 * Template created by mkshtmpl.sh 1.4
 * Copyright (c) 1992, 1993 by Thomas Wolfram, Berlin, Germany
 * (thomas@aeon.in-berlin.de, wolf@prz.tu-berlin.de)
 * Conception derived from work of Thomas Roell
 *
 * $XFree86: mit/lib/Xaw/import.h,v 2.0 1993/09/22 15:32:03 dawes Exp $
 */

#define FUTURE_WIDGET_COUNT 20

#ifndef _libXaw_import
#define _libXaw_import
#ifdef SVR3SHLIB

#define XShapeCombineMask  (*_libXaw_XShapeCombineMask)
#define XShapeQueryExtension  (*_libXaw_XShapeQueryExtension)
#define _ctype  (*_libXaw__ctype)
#define _iob  (*_libXaw__iob)
#define asctime  (*_libXaw_asctime)
#define atof  (*_libXaw_atof)
#define atoi  (*_libXaw_atoi)
#define bcopy  (*_libXaw_bcopy)
/* prevent name conflict with member close of XIM */
#define close(fd) (*_libXaw_close)(fd)
#define creat  (*_libXaw_creat)
#define errno  (*_libXaw_errno)
#define exit  (*_libXaw_exit)
#define fclose  (*_libXaw_fclose)
#define fopen  (*_libXaw_fopen)
#define fprintf  (*_libXaw_fprintf)
#define fread  (*_libXaw_fread)
#define fseek  (*_libXaw_fseek)
#define ftell  (*_libXaw_ftell)
#define getlogin  (*_libXaw_getlogin)
#define getpwuid  (*_libXaw_getpwuid)
#define getuid  (*_libXaw_getuid)
#define localtime  (*_libXaw_localtime)
#define memset  (*_libXaw_memset)
#define open  (*_libXaw_open)
#define printf  (*_libXaw_printf)
#define read  (*_libXaw_read)
#define sprintf  (*_libXaw_sprintf)
/* prevent name conflict with struct stat */
#define stat(path, buf) (*_libXaw_stat)(path, buf)
#define strcat  (*_libXaw_strcat)
#define strchr  (*_libXaw_strchr)
#define strcmp  (*_libXaw_strcmp)
#define strcpy  (*_libXaw_strcpy)
#define strncpy  (*_libXaw_strncpy)
#define sys_errlist  (*_libXaw_sys_errlist)
#define sys_nerr  (*_libXaw_sys_nerr)
#define system  (*_libXaw_system)
/* prevent name conflict with member time of TextWidget */
#define time(tloc)  (*_libXaw_time)(tloc)
#define tmpnam  (*_libXaw_tmpnam)
#define toupper  (*_libXaw_toupper)
#define write  (*_libXaw_write)
#define _XawTextActionsTableCount  (*_libXaw__XawTextActionsTableCount)
#define asciiSinkObjectClass  (*_libXaw_asciiSinkObjectClass)
#define asciiSrcObjectClass  (*_libXaw_asciiSrcObjectClass)
#define asciiTextWidgetClass  (*_libXaw_asciiTextWidgetClass)
#define boxWidgetClass  (*_libXaw_boxWidgetClass)
#define clockWidgetClass  (*_libXaw_clockWidgetClass)
#define commandWidgetClass  (*_libXaw_commandWidgetClass)
#define dialogWidgetClass  (*_libXaw_dialogWidgetClass)
#define formWidgetClass  (*_libXaw_formWidgetClass)
#define gripWidgetClass  (*_libXaw_gripWidgetClass)
#define labelWidgetClass  (*_libXaw_labelWidgetClass)
#define listWidgetClass  (*_libXaw_listWidgetClass)
#define logoWidgetClass (*_libXaw_logoWidgetClass)
#define mailboxWidgetClass  (*_libXaw_mailboxWidgetClass)
#define menuButtonWidgetClass  (*_libXaw_menuButtonWidgetClass)
#define panedWidgetClass  (*_libXaw_panedWidgetClass)
#define pannerWidgetClass  (*_libXaw_pannerWidgetClass)
#define portholeWidgetClass  (*_libXaw_portholeWidgetClass)
#define repeaterWidgetClass  (*_libXaw_repeaterWidgetClass)
#define scrollbarWidgetClass  (*_libXaw_scrollbarWidgetClass)
#define simpleWidgetClass  (*_libXaw_simpleWidgetClass)
#define simpleMenuWidgetClass  (*_libXaw_simpleMenuWidgetClass)
#define smeObjectClass  (*_libXaw_smeObjectClass)
#define smeBSBObjectClass  (*_libXaw_smeBSBObjectClass)
#define smeLineObjectClass  (*_libXaw_smeLineObjectClass)
#define stripChartWidgetClass  (*_libXaw_stripChartWidgetClass)
#define textWidgetClass  (*_libXaw_textWidgetClass)
#define textSinkObjectClass  (*_libXaw_textSinkObjectClass)
#define textSrcObjectClass  (*_libXaw_textSrcObjectClass)
#define toggleWidgetClass  (*_libXaw_toggleWidgetClass)
#define treeWidgetClass  (*_libXaw_treeWidgetClass)
#define vendorShellWidgetClass  (*_libXaw_vendorShellWidgetClass)
#define viewportWidgetClass  (*_libXaw_viewportWidgetClass)

#define transientShellWidgetClass  (*_libXaw_transientShellWidgetClass)

#define _XA_CHARACTER_POSITION  (*_libXaw__XA_CHARACTER_POSITION)
#define _XA_COMPOUND_TEXT  (*_libXaw__XA_COMPOUND_TEXT)
#define _XA_DELETE  (*_libXaw__XA_DELETE)
#define _XA_LENGTH  (*_libXaw__XA_LENGTH)
#define _XA_LIST_LENGTH  (*_libXaw__XA_LIST_LENGTH)
#define _XA_NULL  (*_libXaw__XA_NULL)
#define _XA_SPAN  (*_libXaw__XA_SPAN)
#define _XA_TARGETS  (*_libXaw__XA_TARGETS)
#define _XA_TEXT  (*_libXaw__XA_TEXT)

/* Imported functions declarations
 * Why declaring some imported functions here?
 * This should be done gracefully through including of the systems
 * header files. Unfortunatly there are some source files don't
 * include all headers they should include, there are also some functions
 * nowhere declared in the systems headers and some are declared
 * without extern and cause problems since the names are redefined
 * and these declarations then become undesired false pointer definitions.
 */

extern void XShapeCombineMask();
extern int XShapeQueryExtension();

/* <time.h> on ISC 2.2.1 doesn't declare time(2) for __STDC__
 * so get time_t from it and declare it excplicit
 */
#ifndef ATT
#include <time.h>
#endif

#if defined(ISC202) || defined(ATT)
extern int (*_libXaw_stat)();
extern char (*_libXaw_toupper)();
#endif

#ifdef ISC202
#include <sys/types.h>
#endif

#ifdef ATT
extern int open();
#endif

#ifdef __STDC__
#ifndef ATT
extern time_t time(time_t *);
#else
extern long time(time_t *);   /* gcc 2.3.3 seems to have problems */
#endif                        /* with typedefs on AT&T            */
#ifndef SCO324
extern int creat(char const *, unsigned short);
#endif
extern void exit(int);
extern unsigned short /*uid_t*/ getuid(void);
#ifndef SCO
extern int printf(char const *, ...);
extern int sprintf(char *, char const *, ...);
extern int sscanf(char *, char const *, ...);
#endif
extern int system(char const *);
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
#ifndef ATT
extern time_t time();    
#else
extern long time();     /* gcc 2.3.3 seems to have problems with */
#endif                  /* typedefs on AT&T                      */
#ifndef SCO324
extern int creat();
#endif
extern void exit();
extern unsigned short /*uid_t*/ getuid();
#ifndef SCO
extern int printf();
extern int sprintf();
extern int sscanf();
#endif
extern int system();
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


/* Functions with ambiguous names */

#ifdef __STDC__
extern int close(int);
extern void free(void *);
#include <sys/time.h>
#include <sys/bsdtypes.h>
#ifdef SCO
#include <sys/types.h>
#ifdef SCO324
extern double atof(char *);
#endif /* SCO324 */
#endif /* SCO */
extern int select(int, fd_set *, fd_set *, fd_set *, struct timeval *);
#else
extern int (*_libXaw_close)();
extern void (*_libXaw_free)();
extern int (*_libXaw_select)();
#endif

#endif
#endif

