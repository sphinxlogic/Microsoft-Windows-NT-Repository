/*
 * Facility:	LIBCMU
 *
 * Abstract:	AF_INET address binary to/from ascii
 *
 * Modifications:
 *	05-Nov-2000 T. Dickey
 *		Use socketprv.h header, ANSIfy'd functions.
 *	13-Mar-1999 J. Malmberg
 *		Fixed to compile under DEC C 5.8.
 *	 1-Dec-1998 J. Malmberg	1.0.2
 *		inet_addr has unitialized *dot.  Behavior flaky.
 *		Corrected ctype.h location according to X/OPEN.
 *		Converted to ANSI C format.
 *		buf[-1] was being written to.  I wonder where that is.
 *
 *	 7-OCT-1993 mlo 1.0.1
 *		add hex recognision to inet_addr
 *	 6-OCT-1993 mlo 1.0.0
 *		original
 */
#ifdef VAXC
#module INET "v1.0.1"
#endif

 /* For the DEC C compiler, need to force the old behavior */
/*--------------------------------------------------------*/
#ifndef _VMS_V6_SOURCE
#define _VMS_V6_SOURCE 1
#endif
#ifdef __DECC_VER
#ifdef __CRTL_VER
#undef __CRTL_VER
#define __CRTL_VER 60100000
#endif
#endif

#include "[-]socketprv.h"

#include <ctype.h>

#ifdef __DECC_VER
#ifdef isdigit
#undef isdigit
#endif
#ifdef __ctype
#undef __ctype
#endif
#define __ctype (*decc$ga___ctype)
#pragma __extern_model __save
#pragma __extern_model __strict_refdef
extern const char __ctype [];
#pragma __extern_model __restore
#define _D 0x4
#define _X 0x40
#define isdigit(c)	(__ctype [(c)&0177] & _D)

#ifdef isxdigit
#undef isxdigit
#endif
#define isxdigit(c)	(__ctype [(c)&0177] & (_D|_X))

#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifndef NULL
#define NULL (char *)'\0'
#endif

unsigned long inet_addr(char *str)
{
int	i;
char buf[4];
unsigned long *addr = (unsigned long *)buf;

char	*tmp, *cp, *dot, *cx;
int len;

    len = strlen(str);
    if (len == 0)	/* return 0 if strlen is 0		*/
	return(0);

    /*
     * Make a copy of the input string
     */
    tmp = malloc(len);
    strcpy(tmp,str);

    cp  = tmp;			/* reset the pointer			*/
    *addr = 0x00000000;	/* zero the result			*/

    dot = tmp;
    for (i=0; dot != NULL; i++) {

	if (i > 3)		/* more than 4 parts is an error	*/
	    goto ERROR;

	if ((dot = strchr(cp, '.')) != NULL)
	    *dot = '\0';

	if (strlen(cp) < 1)	/* invalid part length error		*/
	    goto ERROR;

	/*
	 * Check for valid formatted number
	 */
	if ((*cp=='0') && ((cp[1] & 20) =='x')) { /* is hex?		*/
	    for (cx= (&cp[2]); *cx != '\0'; cx++)  /* yes			*/
		if (!isxdigit(*cx))
		    goto ERROR;
	}
	else {
	    for (cx=cp; *cx != '\0'; cx++)	/* not hex		*/
		if (!isdigit(*cx))
		    goto ERROR;
	}


	 /* shift the bytes and convert the next */
	/*--------------------------------------*/
	*addr = *addr >> 8;
	buf[3] = (char)strtoul(cp,0,0);
	cp = dot + 1;
    }

    free(tmp);
    return (*addr);	/* give back binary address		*/

ERROR:
    free(tmp);
    return(-1);
}

char *inet_ntoa(struct in_addr in)
{
static char str[20];
   sprintf(str,"%d.%d.%d.%d",in.S_un.S_un_b.s_b1,in.S_un.S_un_b.s_b2,
			     in.S_un.S_un_b.s_b3,in.S_un.S_un_b.s_b4);
   return(str);
}
