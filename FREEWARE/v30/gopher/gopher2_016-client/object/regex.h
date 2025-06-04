/********************************************************************
 * lindner
 * 3.5
 * 1994/03/08 04:04:16
 * /home/mudhoney/GopherSrc/CVS/gopher+/object/Regex.h,v
 * Exp
 *
 * Paul Lindner, University of Minnesota CIS.
 *
 * Copyright 1991, 1992 by the Regents of the University of Minnesota
 * see the file "Copyright" in the distribution for conditions of use.
 *********************************************************************
 * MODULE: Regex.h
 * Portable method of doing regular expressions
 *********************************************************************
 * Revision History:
 * Regex.h,v
 * Revision 3.5  1994/03/08  04:04:16  lindner
 * add regex.h for NeXT systems
 *
 * Revision 3.4  1994/03/04  17:42:00  lindner
 * Fix for SCO machines
 *
 * Revision 3.3  1994/02/20  16:24:02  lindner
 * no message
 *
 * Revision 3.2  1993/10/27  18:50:38  lindner
 * Support for more sysv platforms
 *
 * Revision 3.1  1993/10/19  20:48:26  lindner
 * Portable versions of Regular expression routines for System V and BSD..
 *
 *
 *********************************************************************/


#if defined(USG) || defined(__svr4__) || defined(_AUX_SOURCE) || defined(hpux) || defined(irix) || defined(M_XENIX) || defined(SYSVREGEX)

#  include "Malloc.h"  /** For NULL **/

#  ifdef  REGEX_CODEIT
#     define INIT   register char *sp = instring;
#     define GETC() (*sp++)
#     define PEEKC()     (*sp)
#     define UNGETC(c)   (--sp)
#     define RETURN(c)   return(NULL);
#     define ERROR(c)    return("error")

#     include <regexp.h>
#     define REGEX_SYSV

#  endif
  /*** BSDize the puppy ***/
  char *re_comp();
  int  re_exe();

#else
  /** BSD regex.. **/
#  ifdef NeXT
#    include <regex.h>
#  endif

#endif
