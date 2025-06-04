/* module to generate soundex encoding; used by qi_build and qi_query */
/* Bruce Tanner - Cerritos College */

/* return a <size> character encoding in <dest> */
/* <dest> must be <size> + 1 characters long */

/* the old version had problems with names starting with "ph" and "pf"
   since p and f have the same numeric value, a numeric-only output
   seems to take care of this.
*/

#include <string.h>

#define alnum 0  /* 0 = hash to number only */
                 /* 1 = keep the first alpha character of name */


#define VOWELS "aeiouywh"
#define VALUES "b1f1p1v1c2g2j2k2q2s2x2z2d3t3l4m5n5r6"

char *soundex(char *dest, char *src, int size)
{
    char *cp, *dp;

    dp = dest;       /* point to dest */
#if alnum
    *dp++ = *src++;  /* retain the first letter */
    size--;          /* count characters moved */
#endif
    while (*src && size) {
        if (strchr(VOWELS, *src) == (char *) 0) { /* don't do vowels */
            cp = strchr(VALUES, *src);    /* find letter */
            if (cp &&                     /* if there was a character found */
                size &&                   /* and we haven't found <size> chars */
                ((dp == dest) ||          /* first character or */
                 ((dp > dest) && (*(dp - 1) != *(cp + 1))) /* not dup value */
               )) {
                *dp++ = *(cp + 1); /* get value */
                size--;
            }
        }
        src++;
    }
    while (size--)
        *dp++ = '0';  /* pad dest with 0 */
    *dp = '\0';

    return dest;     /* return pointer to dest */
}
