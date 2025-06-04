
/*                                                                      */
/* This program is free software; you can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation; either version 2 of the License, or    */
/* (at your option) any later version.                                  */
/*                                                                      */
/* This program is distributed in the hope that it will be useful,      */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of       */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        */
/* GNU General Public License for more details.                         */
/*                                                                      */
/* chksum - Calculates file(s) checksum according to several standards	*/
/* Written J.Lauret, <jlauret@mail.chem.sunysb.edu>			*/
/*                                                                      */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#ifndef VAXC
#include <unistd.h>
#endif
#if defined(VMS) || defined(__GNUC__)
#include <string.h>
#endif

/* Damned GCC-VAX, works everywhere else */
#if defined(EXIT_SUCCESS)
#undef EXIT_SUCCESS
#endif

#if defined(VMS) 
#define EXIT_SUCCESS 1
#else
#define EXIT_SUCCESS 0
#endif


#define LONG 255
#define VERSION "V01-210"



#ifdef VMS
#include <lib$routines.h>
#include <descrip.h>
#include <stsdef.h>

#define DDESCR(a)   struct dsc$descriptor a={0,DSC$K_DTYPE_T,DSC$K_CLASS_D,0}
#define IDESCR(a,b) a.dsc$w_length = strlen(b); \
                    a.dsc$a_pointer= (void *) b;\
                    a.dsc$b_class  = DSC$K_CLASS_S;\
                    a.dsc$b_dtype  = DSC$K_DTYPE_T
#define IIDESCR(a,b,c) a.dsc$w_length = c; \
                       a.dsc$a_pointer= (void *) b;\
                       a.dsc$b_class  = DSC$K_CLASS_S;\
                       a.dsc$b_dtype  = DSC$K_DTYPE_T

# if !defined($VMS_STATUS_SUCCESS)
#    define $VMS_STATUS_SUCCESS(code) (((code)&STS$M_SUCCESS) >>STS$V_SUCCESS)
# endif
# define OK(s) $VMS_STATUS_SUCCESS(s)
#endif



#define MEMCHECK(a)  if (!(a)){ fprintf(stderr,"Could not allocate memory\n");}

