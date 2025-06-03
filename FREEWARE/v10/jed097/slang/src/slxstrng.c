/*
 *  Copyright (c) 1992, 1994 John E. Davis  (davis@amy.tch.harvard.edu)
 *  All Rights Reserved.
 */

/* These routines are simple and inefficient.  They were designed to work on 
 * SunOS when using Electric Fence. 
 */

char *jed_strcpy(register char *aa, register char *b)
{
   char *a = aa;
   while ((*a++ = *b++) != 0);
   return aa;
}

int jed_strcmp(register char *a, register char *b)
{
   while (*a && (*a == *b))
     {
	a++;
	b++;
     }
   if (*a) return((unsigned char) *a - (unsigned char) *b);
   else if (*b) return ((unsigned char) *a - (unsigned char) *b);
   else return 0;
}

char *jed_strncpy(char *a, register char *b,register  int n)
{
   register char *aa = a;
   while ((n > 0) && *b) 
     {
	*aa++ = *b++;
	n--;
     }
   while (n-- > 0) *aa++ = 0;
   return (a);
}
