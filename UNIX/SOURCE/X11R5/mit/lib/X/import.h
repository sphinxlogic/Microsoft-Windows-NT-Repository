/* libX11 : import.h
 * indirection defines
 * Template created by mkshtmpl.sh 1.4
 * Copyright (c) 1992, 1993 by Thomas Wolfram, Berlin, Germany
 * (thomas@aeon.in-berlin.de, wolf@prz.tu-berlin.de)
 * Conception derived partially from work of Thomas Roell
 *
 * $XFree86: mit/lib/X/import.h,v 2.1 1993/09/22 15:31:38 dawes Exp $
 */

#ifndef _libX11_import
#define _libX11_import
#ifdef SVR3SHLIB

#define _ctype  (*_libX11__ctype)
#define _filbuf  (*_libX11__filbuf)
#define _flsbuf  (*_libX11__flsbuf)
#define _iob  (*_libX11__iob)
#define access  (*_libX11_access)
#define alarm  (*_libX11_alarm)
#define atoi  (*_libX11_atoi)
#define calloc  (*_libX11_calloc)
/* prevent name conflict with member close of XIM */
#define close(fd) (*_libX11_close)(fd)
#define connect  (*_libX11_connect)
#define errno  (*_libX11_errno)
#define exit  (*_libX11_exit)
#define fclose  (*_libX11_fclose)
#define fcntl  (*_libX11_fcntl)
#define fgets  (*_libX11_fgets)
#define fopen  (*_libX11_fopen)
#define fprintf  (*_libX11_fprintf)
#define fputs  (*_libX11_fputs)
#define fread  (*_libX11_fread)
/* prevent name conflict with member free of XFontSetMethods */
#define free(ptr) (*_libX11_free)(ptr)
#define fwrite  (*_libX11_fwrite)
#define getenv  (*_libX11_getenv)
#define gethostbyname  (*_libX11_gethostbyname)
#define uname  (*_libX11_uname)
#define getmsg  (*_libX11_getmsg)
#define getpid   (*_libX11_getpid)
#define getpwnam  (*_libX11_getpwnam)
#define getpwuid  (*_libX11_getpwuid)
#define getuid  (*_libX11_getuid)
#define grantpt  (*_libX11_grantpt)
#define htons  (*_libX11_htons)
#define inet_addr  (*_libX11_inet_addr)
#define ioctl  (*_libX11_ioctl)
#define malloc  (*_libX11_malloc)
#define memset  (*_libX11_memset)
#define ntohl  (*_libX11_ntohl)
#define ntohs  (*_libX11_ntohs)
#define open  (*_libX11_open)
#define ptsname  (*_libX11_ptsname)
#define putmsg  (*_libX11_putmsg)
#define qsort  (*_libX11_qsort)
#define read  (*_libX11_read)
#define realloc  (*_libX11_realloc)
#define rewind  (*_libX11_rewind)
/* prevent name conflict with bitfield select of _XtEventRec */
#define select(max, rd, wr, ex, to) (*_libX11_select)(max, rd, wr, ex, to)
#define setsockopt  (*_libX11_setsockopt)
#define signal  (*_libX11_signal)
#define sleep  (*_libX11_sleep)
#define socket  (*_libX11_socket)
#define sprintf  (*_libX11_sprintf)
#define sscanf  (*_libX11_sscanf)
/* prevent name conflict with struct stat */
#define stat(path, buf) (*_libX11_stat)(path, buf)
#define strcat  (*_libX11_strcat)
#define strchr  (*_libX11_strchr)
#define strcmp  (*_libX11_strcmp)
#define strcpy  (*_libX11_strcpy)
#define strncmp  (*_libX11_strncmp)
#define strncpy  (*_libX11_strncpy)
#define strrchr  (*_libX11_strrchr)
#define strtok (*_libX11_strtok)
#define sys_errlist  (*_libX11_sys_errlist)
#define sys_nerr  (*_libX11_sys_nerr)
#define tolower  (*_libX11_tolower)
#define unlockpt  (*_libX11_unlockpt)
#define write  (*_libX11_write)
#define writev  (*_libX11_writev)
#define XauDisposeAuth  (*_libX11_XauDisposeAuth)
#define XauFileName  (*_libX11_XauFileName)
#define XauGetBestAuthByAddr  (*_libX11_XauGetBestAuthByAddr)
#define XauReadAuth  (*_libX11_XauReadAuth)
/*
#define _Xevent_to_mask  (*_libX11__Xevent_to_mask)
*/

/* for XDM authorization */
#define _XdmcpAuthSetup (*_libX11__XdmcpAuthSetup)
#define _XdmcpAuthDoIt (*_libX11__XdmcpAuthDoIt)
#define XdmcpWrap (*_libX11_XdmcpWrap)

#define getsockname (*_libX11_getsockname)
/* prevent name conflict */
#define time(tloc)  (*_libX11_time)(tloc)


/* Imported functions declarations
 * Why declaring some imported functions here?
 * This should be done gracefully through including of the systems
 * header files. Unfortunatly there are some source files don't
 * include all headers they should include, there are also some functions
 * nowhere declared in the systems headers and some are declared
 * without extern and cause problems since the names are redefined
 * and these declarations then become undesired false pointer definitions.
 */

/* <time.h> on ISC 2.2.1 doesn't declare time(2) for __STDC__
 * so get time_t from it and declare it excplicit
 */
#ifndef ATT
#include <time.h>
#endif

#ifdef __STDC__
#ifndef ATT
extern time_t time(time_t *);
#else
extern long time(time_t *);   /* gcc 2.3.3 seems to have problems */
#endif                        /* with typedefs on AT&T            */
extern int creat(char const *, unsigned short);
extern void exit(int);
#ifndef SCO
extern int sprintf(char *, char const *, ...);
extern int sscanf(char *, char const *, ...);
#else
extern int open();
extern int fcntl();
#ifndef SCO324
extern int uname();
#endif
#endif /* SCO */
extern int atoi(char const *);
extern int access(char const *, int);
extern unsigned int alarm(unsigned int);
extern unsigned int sleep(unsigned int);
extern int close(int);
#ifdef ISC32
extern int read(int, const char *, unsigned int);
#else
extern int read(int, char *, unsigned int);
#endif
extern int write(int, char const *, unsigned int);
extern int grantpt(int);
extern char *ptsname(int);
extern int unlockpt(int);
extern char *getenv(char const *);
extern short /*pid_t*/ getpid(void);
extern void XdmcpWrap(unsigned char *, unsigned char*, 
		      unsigned char *, int);
#else /* ifndef __STDC__ */
#ifndef ATT
extern time_t time();    
#else
extern long time();     /* gcc 2.3.3 seems to have problems with */
#endif                  /* typedefs on AT&T                      */
extern int creat();
extern void exit();
#ifndef SCO
extern int sprintf();
extern int sscanf();
#else
extern int open();
extern int fcntl();
#ifndef SCO324
extern int uname();
#endif
#endif /* SCO */
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
extern short /*pid_t*/ getpid();
extern void XdmcpWrap();
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
extern int getsockname();

extern _XdmcpAuthSetup(); 
extern _XdmcpAuthDoIt();

#if defined(ISC202) || defined(ATT)
extern int (*_libX11_stat)();
extern char (*_libX11_tolower)();
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
extern int (*_libX11_close)();
extern void (*_libX11_free)();
extern int (*_libX11_select)();
#endif

#endif
#endif
