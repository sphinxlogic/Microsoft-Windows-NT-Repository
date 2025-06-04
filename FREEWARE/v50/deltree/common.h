/* Header Name             : COMMON.H                                   */
/*   Original Author       : JLAURET                                    */
/*   Date                  :                                            */
/*   Header Description    : Common include for deltree			*/
/*                         :                                            */
/*                                                                      */
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

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <descrip.h>
#include <string.h>

#ifdef __GNUC__
#define cli$present   CLI$PRESENT
#define cli$dcl_parse CLI$DCL_PARSE
#define cli$get_value CLI$GET_VALUE
#else
#include <libclidef.h>
#include <stat.h>
#endif

#include <climsgdef.h>
#include <clidef.h>
#include <time.h>
#include <lib$routines.h>
#include <str$routines.h>
#include <lnmdef.h>
#include <starlet.h>


#if defined(__DECC)
#include <cli$routines.h>
#else
#if defined(__GNUC__) && !defined(__ALPHA)
#error "*** Sorry. No GCC support on VAX ... ***"
#endif
#endif


#if defined(__DECC) || defined(__GNUC__)
#   include <unixlib.h>
#   include <unistd.h>
#   include <unixio.h>
#endif



/* Variables, constants and all of that 				*/
#ifndef TRUE
#define TRUE (1==1)
#endif
#ifndef FALSE
#define FALSE (1==0)
#endif
#ifndef NULL_C
#define NULL_C  '\0'
#endif

#if __CRTL_VER > 70200000
#define MAXDEPTH 255
#else
#define MAXDEPTH 9
#endif


#define PRCSNAMLEN 15
#define DIM	 255
#define XDIM     2048
#define VERSION  "V01-775"
#define COPYR	 "Copyright © 1994-2001 Jérôme LAURET"
#define MAXDT    15	/* Maximum number of detached processes		*/
			/* I am giving a decent number here but in all	*/
			/* cases, you cannot exceed 100 (process name	*/
			/* will be otherwise too long ...		*/
			/* Note that DELTREE waits until a slot is free	*/

#define DEEPDR "DELTREE_DEEP"			/* VMS logical		*/
#define DEEPCD "DELTREE_DEEP:[000000]"		/* Must be VMS-like	*/
#define DEEPDF "/deltree_deep/000000/"		/* Uglix-like path	*/


/* Macro definition 							*/
#define OK(s)   	$VMS_STATUS_SUCCESS(s)

#define DDESCR(a)   struct dsc$descriptor a={0,DSC$K_DTYPE_T,DSC$K_CLASS_D,0}
#define IDESCR(a,b) a.dsc$w_length = strlen(b); \
		    a.dsc$a_pointer= (void *) b;\
		    a.dsc$b_class  = DSC$K_CLASS_S;\
		    a.dsc$b_dtype  = DSC$K_DTYPE_T


#define ALLOCA(a,b,c) a = (char *) malloc(sizeof(char *)*(strlen(b)+c));\
		     if(a == NULL) hack$exit(DELTREE__INSFMEM_msg_hack);

#define REALLOCA(a,b,c) a = (char *) realloc((void *)a,sizeof(char *)*(strlen(a)+strlen(b)+c));\
			if(a == NULL) hack$exit(DELTREE__INSFMEM_msg_hack);




/* Types and structs							*/
struct returned_info {
  char	prcsnam[PRCSNAMLEN];
  int	pid;
  int	prio;
};



