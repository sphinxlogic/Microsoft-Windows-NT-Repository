/****************************************************************************
 * F.Macrides 13-Jul-1994   Worked in Alan's second set of patches.
 * F.Macrides 02-Jul-1994   DECC has strftime().
 * F.Macrides 01-Jul-1994   Added Lance's internationalization port to VMS:
 *   JL Wilkinson 23-June-1994	VMS provides only a modicum of support for 
 *				strftime() -- it's only in the unshared library
 *				on VAX, so use the one in COMPATIBLE.C on VMS.
 */
/********************************************************************
 Changes from Alan Coopersmith's patches:
    - ifndef NO_UNISTD_H, include it so we can use POSIX stuff
    - use POSIX alternatives for getdtablesize()
    - ifdef _POSIX_SOURCE use POSIX functions like getcwd(), etc.
 ********************************************************************/

/********************************************************************
 * lindner
 * 3.23
 * 1994/05/14 04:14:40
 * /home/mudhoney/GopherSrc/CVS/gopher+/object/compatible.h,v
 * Exp
 *
 * Paul Lindner, University of Minnesota CIS.
 *
 * Copyright 1991, 1992 by the Regents of the University of Minnesota
 * see the file "Copyright" in the distribution for conditions of use.
 *********************************************************************
 * MODULE: compatible.h
 * Header file to make things compatible and define stuff in compatible.c
 *********************************************************************
 * Revision History:
 * compatible.h,v
 * Revision 3.23  1994/05/14  04:14:40  lindner
 * Fix for svr4.2 on unisys
 *
 * Revision 3.22  1994/05/11  02:41:57  lindner
 * Automagically define NO_VFORK for SCO
 *
 * Revision 3.21  1994/04/25  03:36:17  lindner
 * eliminate redefinition of open to open_vmsopt() for ALPHA (Fote)
 *
 * Revision 3.20  1994/04/08  20:05:56  lindner
 * gcc -Wall fixes
 *
 * Revision 3.19  1994/04/01  04:44:49  lindner
 * Fix for VMS
 *
 * Revision 3.18  1994/03/17  04:38:21  lindner
 * VMS weird directory routines
 *
 * Revision 3.17  1994/03/08  07:28:42  lindner
 * Fix for vfork on Aix
 *
 * Revision 3.16  1994/03/08  03:23:50  lindner
 * Waitpid for vms
 *
 * Revision 3.15  1994/03/07  04:43:54  lindner
 * Add pid_t, vfork
 *
 * Revision 3.14  1993/11/05  07:24:15  lindner
 * Fixes for NeXTs
 *
 * Revision 3.13  1993/10/27  18:51:12  lindner
 * Updates for VMS files/records
 *
 * Revision 3.12  1993/09/28  19:23:56  lindner
 * update for VMS getwd()
 *
 * Revision 3.11  1993/08/16  19:40:58  lindner
 * Fixes for Sequent Dynix
 *
 * Revision 3.10  1993/08/03  20:57:42  lindner
 * Really fix it for hocky pucks
 *
 * Revision 3.9  1993/08/03  06:43:48  lindner
 * Compatibility fix for hpux and seteguid
 *
 * Revision 3.8  1993/07/27  05:30:29  lindner
 * Mondo Debug overhaul from Mitra
 *
 * Revision 3.7  1993/07/20  23:33:27  lindner
 * Fix for NeXTs and pid_t
 *
 * Revision 3.6  1993/07/14  20:37:40  lindner
 * Fixes for VMS
 *
 * Revision 3.5  1993/07/07  19:29:19  lindner
 * Added LINGER compatibility option
 *
 * Revision 3.4  1993/06/22  05:53:19  lindner
 * Added getdtablesize() option
 *
 * Revision 3.3  1993/06/15  06:12:14  lindner
 * Updates for Solaris
 *
 * Revision 3.2  1993/04/15  21:36:33  lindner
 * Emulation of geteuid calls for HPs
 *
 * Revision 3.1.1.1  1993/02/11  18:03:05  lindner
 * Gopher+1.2beta release
 *
 * Revision 1.7  1993/01/14  21:58:02  lindner
 * Added #define that makes compatible compile fine under UCX
 *
 * Revision 1.6  1993/01/11  19:56:20  lindner
 * Fixed prototype for strcasecmp
 *
 * Revision 1.5  1993/01/09  02:25:57  lindner
 * Added definition for SIG_ERR for systems that don't have it.
 *
 * Revision 1.4  1993/01/08  23:17:20  lindner
 * Added more mods for VMS.
 *
 * Revision 1.3  1993/01/06  23:09:44  lindner
 * Added definition for AIX370 for strdup()
 *
 * Revision 1.2  1992/12/31  04:42:02  lindner
 * Changes for VMS, new switch NO_STRCASECMP
 *
 * Revision 1.1  1992/12/10  23:27:52  lindner
 * gopher 1.1 release
 *
 *
 *********************************************************************/
/*
 * Protoize a little 
 */

#ifndef compatible_h_
#define compatible_h_

/*
** This construction doesn't make sense to me, and POSIX is rare
** on VMS, so forget it...  FM
*/
#ifndef VMS
/******************** get Unix standard definitions **/
#if !defined(NO_UNISTD_H) /* || !defined(YOUR_FAVORITE_SYSTEM_HERE) */
#  undef  NO_UNISTD_H
#  define NO_UNISTD_H 1
#  include <unistd.h> 
#endif
#endif

/******************** for SIG_ERR **/
#include <signal.h>

#ifndef SIG_ERR
#  ifdef __STDC__
#    define SIG_ERR         ((void (*) (int))-1)
#  else
#    define SIG_ERR         ((void (*) ())-1)
#  endif        /* __STDC__ */
#endif        /* SIG_ERR */



/******************** for strstr() **/

#if defined(NOSTRSTR) || (defined(mips) && !defined(ultrix)) || defined(sequent) || defined(n16) || defined(sony_news)
#undef  NOSTRSTR
#define NOSTRSTR
char *strstr();
#endif

/******************** for tempnam() **/

#if defined(__convex__) || defined(NeXT) || defined(sequent) || defined(VMS) || defined(NO_TEMPNAM)
#undef  NO_TEMPNAM
#define NO_TEMPNAM
char *tempnam();
#endif

/******************************** for strdup() **/

#if defined(mips) || defined(ultrix) || defined(NeXT) || defined(sony_news) || defined(sequent) || defined (VMS) || defined(_AUX_SOURCE) || defined(_AIX370) || defined(NO_STRDUP) 
#undef  NO_STRDUP
#define NO_STRDUP
char *strdup();
#endif

/********************** For bzero()/bcopy() etc */

#if defined(USG) || defined(NO_BZERO) || defined(_SEQUENT_) || defined(VMS) || defined(__svr4__) 

#define bzero(a,b)   memset(a,'\0',b)
#define bcopy(a,b,c) memcpy(b,a,c)
     
#endif

/********************* For getwd() */

#if defined(M_XENIX) || defined(hpux) || defined(USG) || defined(NO_GETWD) ||      defined(__svr4__) || defined(VMS) || defined(_POSIX_SOURCE)
# define getwd(a) getcwd(a,sizeof(a))
#endif

/********************* For tzset() */

#if defined(NO_TZSET) || defined(sequent)
#undef  NO_TZSET
#define NO_TZSET
void tzset();
#endif

/******************** For strcasecmp() */

#if defined(NO_STRCASECMP) || defined(VMS) || defined(__USLC__)
#  undef NO_STRCASECMP
#  define NO_STRCASECMP
int strcasecmp();
#endif

/******************** For geteuid() */

#if defined(NO_SETEUID) || defined (__hpux)
# undef   NO_SETEUID
# define  NO_SETEUID


#define seteuid(x) setresuid(-1,x,-1)
#define setegid(x) setresgid(-1,x,-1)  

#endif


#if defined(_SEQUENT_)
# undef   NO_SETEUID
# define  NO_SETEUID

#define seteuid(x) setreuid(-1,x)
#define setegid(x) setregid(-1,x)

#endif

/********************* For getdtablesize() **/
#if defined(NO_GETDTABLESIZE) || defined(_POSIX_SOURCE) || defined(_XOPEN_SOURCE)
#undef NO_GETDTABLESIZE

# ifdef _SC_OPEN_MAX	/* prefered POSIX method */
#  define    getdtablesize()	sysconf(_SC_OPEN_MAX) 
# else /* if !defined(systems_without_limits.h) */
#  include <limits.h>
#  ifdef _POSIX_OPEN_MAX    /* alternate POSIX method */
#   define   getdtablesize()	(_POSIX_OPEN_MAX)
#  else
#   define NO_GETDTABLESIZE 1
int getdtablesize();
#  endif
# endif
#endif

/********************* For VMS */

#if defined(VMS)
#  include <stdio.h>

#  define popen(a,b) fopen(a,b)
#  define pclose(a)  fclose(a)
#  define unlink delete

#  define fopen fopen_VMSopt
   FILE *fopen_VMSopt();
#ifndef __ALPHA
#  define open open_VMSopt
   int open_VMSopt();
#endif

#  if defined(UCX)
#    define closenet close
#  endif

   FILE *fopen_VAR();
   FILE *fopen_FIX();

#else
   /* non-VMS systems don't need a special netclose either */
#  define closenet close

#endif  /** VMS **/


/**********  For linger setsockopt() ***/

#if defined(linux) || defined(_SEQUENT_)
#  define NO_LINGER
#endif

/********** For NeXTs **************/
#ifdef NeXT
#  define ANSI_LIKE  /** freeWAIS bastards! **/
#endif

/********* Systems that can't set proctitles *******/
#if defined(_AUX_SOURCE) || defined(__svr4__) 
#   undef SETPROCTITLE
#endif

/********* Fix for old stat.h stuff on bsd (I think) ******/
#if defined(NeXT) || defined(sequent) || defined(NO_NEWSTATDOTH)
#define S_IXUSR S_IEXEC
#endif

#if defined(NO_VFORK) || defined(_AUX_SOURCE) || defined(sgi) || defined(_AIX) || defined(M_XENIX)
#   define vfork fork
#endif

#if defined(VMS) || defined(NeXT) || defined(sequent) || defined(NO_PID_T)
  typedef int pid_t;
#endif

#ifndef __DECC
#if defined(sequent) || defined(NO_STRFTIME) || defined(VMS)
#  undef NO_STRFTIME
#  define NO_STRFTIME
   int strftime();
#endif
#endif

#ifndef VMS
/********* strcoll - should be present in ANSI C libraries ******/
#if defined(NO_STRCOLL) || defined(sequent)
#  define strcoll strcmp
#endif
#endif

#endif  /* compatible_h_ */
