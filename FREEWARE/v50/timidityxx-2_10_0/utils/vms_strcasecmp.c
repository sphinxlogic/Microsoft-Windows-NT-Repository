/*----------------------------------------------------------------------------*/
/* The original source for strcasecmp comes from the create.c file in the     */
/* xpm-lib distribution, Copyright 1989-95 GROUPE BULL, and was developed by  */
/* Arnaud Le Hors (lehors@sophia.inria.fr). Please look at                    */
/* ftp://koala.inria.fr/pub/xpm for a full statement of rights.               */
/*                                                                            */
/* strncasecmp was made up by me and if you have problems with it please      */
/* don't bother Arnaud.                                                       */
/* 950718 Martin P.J. Zinser (M.Zinser@gsi.de or eurmpz@eur.sas.com           */
/*----------------------------------------------------------------------------*/

#include <ctype.h>

int strcasecmp(s1, s2)
  register char *s1, *s2;
{
  register int c1, c2;
  
  while (*s1 && *s2){
      c1 = tolower(*s1);
      c2 = tolower(*s2);
      if (c1 != c2)
          return (c1-c2);
      s1++;
      s2++;
     }
  return (int) (*s1 - *s2);
} 


int strncasecmp(s1, s2, n)
  register char *s1, *s2;
  register int n;
{
  register int c1, c2, i;
  i = 1;
 
  
  while (*s1 && *s2 && i <= n){
      c1 = tolower(*s1);
      c2 = tolower(*s2);
      if (c1 != c2)
          return (c1-c2);
      s1++;
      s2++;
      i++;
     }
  if ( i = n )
      return 0;
  else
      return (int) (*s1 - *s2);
} 
