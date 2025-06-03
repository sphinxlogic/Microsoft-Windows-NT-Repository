/********************************************************************
 * $Author: lindner $
 * $Revision: 1.7 $
 * $Date: 1993/01/14 21:58:02 $
 * $Source: /home/mudhoney/GopherSrc/gopher1.11b/object/RCS/compatible.h,v $
 * $State: Rel $
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
 * $Log: compatible.h,v $
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

/******************** for SIG_ERR **/
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

#if defined(USG) || defined(NO_BZERO) || defined(_SEQUENT_) || defined(VMS)

#define bzero(a,b)   memset(a,'\0',b)
#define bcopy(a,b,c) memcpy(b,a,c)
     
#endif

/********************* For getwd() */

#if defined(M_XENIX) || defined(hpux) || defined(USG) || defined(NO_GETWD)
# define getwd(a) getcwd(a,sizeof(a))
#endif

/********************* For tzset() */

#if defined(NO_TZSET) || defined(sequent)
#undef  NO_TZSET
#define NO_TZSET
void tzset();
#endif

/******************** For strcasecmp() */

#if defined(NO_STRCASECMP) || (defined(VMS) && !defined(WOLLONGONG))
#  undef NO_STRCASECMP
#  define NO_STRCASECMP
int strcasecmp();
#endif

/********************* For VMS */

#if defined(VMS)
#  define popen(a,b) fopen(a,b)
#  define pclose(a)  fclose(a)
#  define unlink delete
#  include <stdio.h>

#  define fopen fopen_VMSopt
   FILE *fopen_VMSopt();
#  define open open_VMSopt
   int open_VMSopt();

#  if defined(UCX)
#    define closenet close
#  endif

#else
   /* non-VMS systems don't need a special netclose either */
#  define closenet close

#endif  /** VMS **/

#endif  /* compatible_h_ */





