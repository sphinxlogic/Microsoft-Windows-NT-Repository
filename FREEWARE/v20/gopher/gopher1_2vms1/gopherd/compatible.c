/********************************************************************
 * 19931117	D.Sherman <dennis_sherman@unc.edu>
 *			modified <string.h> to force use of
 *			 sys$library:string.h, even if
 *			 /include=[-.gopherd] is included in compile
 *			 command, as it is for lookaside.
/********************************************************************
 * $Author: lindner $
 * $Revision: 1.4 $
 * $Date: 1993/01/17 03:46:12 $
 * $Source: /home/mudhoney/GopherSrc/gopher1.11b/object/RCS/compatible.c,v $
 * $State: Rel $
 *
 * Paul Lindner, University of Minnesota CIS.
 *
 * Copyright 1991, 1992 by the Regents of the University of Minnesota
 * see the file "Copyright" in the distribution for conditions of use.
 *********************************************************************
 * MODULE: compatible.c
 * Compatibility routines
 *********************************************************************
 * Revision History:
 * $Log: compatible.c,v $
 * Revision 1.5  1993/05/07  09:02:43  imhw400
 * Supplied vms_errno_string() for VMS non-Multinet sites
 *
 * Revision 1.4  1993/01/17  03:46:12  lindner
 * Fixed tempnam for VMS
 *
 * Revision 1.3  1993/01/08  23:13:55  lindner
 * Added more VMS mods from jqj
 *
 *
 *********************************************************************/


/*
 * Some functions that aren't implemented on every machine on the net
 *
 * definitions should be in the form "NO_FNNAME"
 * compatible.h looks at preprocessor symbols and automatically defines
 * many of the NO_FNNAME options
 *
 */

#include <sys$library:string.h>
#include <stdio.h>
#include <errno.h>
#include "Malloc.h"  /*** For NULL ***/
#include "compatible.h"

/*** For machines that don't have strstr ***/

#if defined(NOSTRSTR)

char *
strstr(host_name, cp)
  char *host_name;
  char *cp;
{
     int i, j;

     for (i = 0; i < strlen(host_name); i++) {
          j = strncmp(host_name+i, cp, strlen(cp));
          if (j == 0)
               return(host_name+i);
     }
     return(NULL);
}
#endif

#if defined(sequent)

#include <varargs.h>
vsprintf(va_alist)
  va_dcl
{
        ;
}

vfprintf(va_alist)
  va_dcl
{
        ;
}


#endif

#if defined(NO_TEMPNAM)
static int cnt = 0;

char *tempnam(dir, pfx)
  char *dir;
  char *pfx;
{
	char space[512];
	char *newspace;

#ifdef VMS
	if (dir == NULL) {
		dir = "sys$scratch:";
	} else if (*dir == '\0') {
		dir = "sys$scratch:";
	}
	
	if (pfx == NULL) {
		pfx = "gopher.$";
	} else if (*pfx == '\0') {
		pfx = "gopher.$";
	}

	sprintf(space, "%s%s%d%d", dir, pfx, getpid(), cnt);
#else
	if (dir == NULL) {
		dir = "/usr/tmp";
	} else if (*dir == '\0') {
		dir = "/usr/tmp";
	}
	
	if (pfx == NULL) {
		pfx = "";
	}

	sprintf(space, "%s/%s%d.%d", dir, pfx, getpid(), cnt);
#endif
	cnt++;
	
	newspace = (char *)malloc(strlen(space) + 1);
	if (newspace != NULL) {
		strcpy(newspace, space);
	}
	return newspace;
}
#endif

#if defined(NO_STRDUP)
char *strdup(str)
  char *str;
{
        int len;
        char *temp;

        if (str == NULL) return(NULL);
        len = strlen(str);

        temp = (char *) malloc(sizeof(char) * len + 1);

        strcpy(temp, str);
        return(temp);
}
#endif

#if defined(NO_TZSET)
void
tzset()
{
     ;
}
#endif

#if defined(NO_STRCASECMP)
/*
 * Copyright (c) 1987 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * Modified for use on VMS by Earl Fogel, University of Saskatchewan
 * Computing Services, January 1992
 */

typedef unsigned char u_char;

/*
 * This array is designed for mapping upper and lower case letter
 * together for a case independent comparison.  The mappings are
 * based upon ascii character sequences.
 */
static const u_char charmap[] = {
	'\000', '\001', '\002', '\003', '\004', '\005', '\006', '\007',
	'\010', '\011', '\012', '\013', '\014', '\015', '\016', '\017',
	'\020', '\021', '\022', '\023', '\024', '\025', '\026', '\027',
	'\030', '\031', '\032', '\033', '\034', '\035', '\036', '\037',
	'\040', '\041', '\042', '\043', '\044', '\045', '\046', '\047',
	'\050', '\051', '\052', '\053', '\054', '\055', '\056', '\057',
	'\060', '\061', '\062', '\063', '\064', '\065', '\066', '\067',
	'\070', '\071', '\072', '\073', '\074', '\075', '\076', '\077',
	'\100', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
	'\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157',
	'\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167',
	'\170', '\171', '\172', '\133', '\134', '\135', '\136', '\137',
	'\140', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
	'\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157',
	'\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167',
	'\170', '\171', '\172', '\173', '\174', '\175', '\176', '\177',
	'\200', '\201', '\202', '\203', '\204', '\205', '\206', '\207',
	'\210', '\211', '\212', '\213', '\214', '\215', '\216', '\217',
	'\220', '\221', '\222', '\223', '\224', '\225', '\226', '\227',
	'\230', '\231', '\232', '\233', '\234', '\235', '\236', '\237',
	'\240', '\241', '\242', '\243', '\244', '\245', '\246', '\247',
	'\250', '\251', '\252', '\253', '\254', '\255', '\256', '\257',
	'\260', '\261', '\262', '\263', '\264', '\265', '\266', '\267',
	'\270', '\271', '\272', '\273', '\274', '\275', '\276', '\277',
	'\300', '\301', '\302', '\303', '\304', '\305', '\306', '\307',
	'\310', '\311', '\312', '\313', '\314', '\315', '\316', '\317',
	'\320', '\321', '\322', '\323', '\324', '\325', '\326', '\327',
	'\330', '\331', '\332', '\333', '\334', '\335', '\336', '\337',
	'\340', '\341', '\342', '\343', '\344', '\345', '\346', '\347',
	'\350', '\351', '\352', '\353', '\354', '\355', '\356', '\357',
	'\360', '\361', '\362', '\363', '\364', '\365', '\366', '\367',
	'\370', '\371', '\372', '\373', '\374', '\375', '\376', '\377',
};

int
strcasecmp(s1, s2)
	const char *s1, *s2;
{
	register const u_char *cm = charmap,
			*us1 = (const u_char *)s1,
			*us2 = (const u_char *)s2;

	while (cm[*us1] == cm[*us2++])
		if (*us1++ == '\0')
			return (0);
	return (cm[*us1] - cm[*--us2]);
}

int
strncasecmp(s1, s2, n)
	const char *s1, *s2;
	register size_t n;
{
	if (n != 0) {
		register const u_char *cm = charmap,
				*us1 = (const u_char *)s1,
				*us2 = (const u_char *)s2;

		do {
			if (cm[*us1] != cm[*us2++])
				return (cm[*us1] - cm[*--us2]);
			if (*us1++ == '\0')
				break;
		} while (--n != 0);
	}
	return (0);
}

#endif

#if defined(VMS)
/* In all versions of VMS, fopen() and open() are needlessly inefficient.
 * Define jacket routines to do file opens with more sensible parameters
 * than the VAXCRTL default, including reasonable allocation for the "a" log
 * file opens and retries using RMS to determing an STV value for the failure.
 * [Should we really be doing this for EVERY fopen() and open()?]
 */
#ifdef fopen
#undef fopen
#endif
#ifdef open
#undef open
#endif

#ifdef SERVER
int vaxc$errno_stv;
#include <rms.h>
#endif

FILE *fopen_VMSopt ( name, mode , alq, deq)
    char *name, *mode, *alq, *deq;
{
#ifdef SERVER
    struct FAB	fab;
    int		status;
    FILE	*opened;

    errno = vaxc$errno = vaxc$errno_stv = 0;
    if (strcmp(mode,"a")==0)
	opened = fopen (name, mode, "mbc=32", alq, deq);
    else
	opened = fopen ( name, mode, "mbc=32" );
    if (opened==NULL) {
	fab = cc$rms_fab;
	fab.fab$l_fna = name;
	fab.fab$b_fns = strlen(fab.fab$l_fna);
	if (((status = SYS$OPEN(&fab, 0, 0)) &1) != 1) {
	    vaxc$errno = fab.fab$l_sts;
	    vaxc$errno_stv = fab.fab$l_stv;
	}
	else SYS$CLOSE(&fab,0,0);
    }
    return opened;
#else
    return fopen ( name, mode, "mbc=32");
#endif
}

int open_VMSopt ( name, flags, mode )
    char *name;
    int flags;
    unsigned int mode;
{
    return  open ( name, flags, mode, "mbc=32");
}
#endif

#if defined(VMS)

/* Multinet defines a handy vms_errno_string() that essentially packages
 * strerror with no parameters.  (This insulates other platforms from
 * VMS' oddball two-argument strerror.) We need to supply it here for
 * other IP packages under VMS.
 */

# if !defined(MULTINET)
char *vms_errno_string (void) { return(strerror(errno, vaxc$errno)); }
# endif /* defined(MULTINET) */

#endif /* defined(VMS) */
