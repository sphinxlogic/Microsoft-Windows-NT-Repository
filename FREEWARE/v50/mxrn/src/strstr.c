#include "config.h"

#ifdef NEED_STRSTR
#include <stdio.h>
#ifndef __STDC__
#define CONST
#else
#define CONST const
#endif

/*
===========================================================================
Marc Evans - WB1GRH - evans@decvax.DEC.COM  | Synergytics     (603)635-8876
      Unix and X Software Contractor        | 21 Hinds Ln, Pelham, NH 03076
===========================================================================

*/

char * strstr(s1, s2)
register char CONST *s1, *s2;
{
    register int n = strlen(s2);
    char *rs = (*s1) ? NULL : s2;

    while (*s1)
    {   if (strncmp(s2,s1,n) == 0L)
	{   rs = s1;
	    break;
	}
	++s1;
    }
    return(rs);
}
#endif
