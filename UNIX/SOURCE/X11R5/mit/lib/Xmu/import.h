/* libXmu : import.h
 * indirection defines
 * Template created by mkshtmpl.sh 1.4
 * Copyright (c) 1992, 1993 by Thomas Wolfram, Berlin, Germany
 * (thomas@aeon.in-berlin.de, wolf@prz.tu-berlin.de)
 * Conception derived from work of Thomas Roell
 *
 * $XFree86: mit/lib/Xmu/import.h,v 2.0 1993/09/22 15:32:44 dawes Exp $
 */

#ifndef _libXmu_import
#define _libXmu_import
#ifdef SVR3SHLIB

#define XShapeCombineMask  (*_libXmu_XShapeCombineMask)
#define _ctype  (*_libXmu__ctype)
#define _filbuf  (*_libXmu__filbuf)
#define _iob  (*_libXmu__iob)
#define atoi  (*_libXmu_atoi)
#define bcopy  (*_libXmu_bcopy)
#define calloc  (*_libXmu_calloc)
#define exit  (*_libXmu_exit)
#define fclose  (*_libXmu_fclose)
#define fgets  (*_libXmu_fgets)
#define fopen  (*_libXmu_fopen)
#define fprintf  (*_libXmu_fprintf)
#define fputs  (*_libXmu_fputs)
/* prevent name conflict with member free of XFontSetMethods */
#define free(ptr) (*_libXmu_free)(ptr)
#define getenv  (*_libXmu_getenv)
#define gethostbyname  (*_libXmu_gethostbyname)
#define gethostname  (*_libXmu_gethostname)
#define malloc  (*_libXmu_malloc)
#define qsort  (*_libXmu_qsort)
#define realloc  (*_libXmu_realloc)
#define sprintf  (*_libXmu_sprintf)
#define sscanf  (*_libXmu_sscanf)
#define strcat  (*_libXmu_strcat)
#define strcmp  (*_libXmu_strcmp)
#define strcpy  (*_libXmu_strcpy)
#define strncmp  (*_libXmu_strncmp)
#define strncpy  (*_libXmu_strncpy)
#define strrchr  (*_libXmu_strrchr)
#define uname  (*_libXmu_uname)

#define _XA_CLASS  (*_libXmu__XA_CLASS)
#define _XA_CLIENT_WINDOW  (*_libXmu__XA_CLIENT_WINDOW)
#define _XA_HOSTNAME  (*_libXmu__XA_HOSTNAME)
#define _XA_IP_ADDRESS  (*_libXmu__XA_IP_ADDRESS)
#define _XA_NAME  (*_libXmu__XA_NAME)
#define _XA_NET_ADDRESS  (*_libXmu__XA_NET_ADDRESS)
#define _XA_OWNER_OS  (*_libXmu__XA_OWNER_OS)
#define _XA_TARGETS  (*_libXmu__XA_TARGETS)
#define _XA_TIMESTAMP  (*_libXmu__XA_TIMESTAMP)
#define _XA_USER  (*_libXmu__XA_USER)


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
#ifdef ATT
extern int uname();
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
extern int (*_libXmu_close)();
extern void (*_libXmu_free)();
extern int (*_libXmu_select)();
#endif

#endif
#endif

