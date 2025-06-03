/*************************************************************************\
*   				Xpeg 1.0				  *
*									  *
* Copyright 1990, Kenneth C. Nelson			 	          *
*									  *
* Rights       : I, Ken Nelson own Xpeg.  I am donating the source	  *
*		 to the public domain with the restriction that nobody	  *
*		 sells it, or anything derived from it, for anything more *
*		 than media costs.  Xpeg uses a dynamic object library	  *
*		 that I don't own.  See the subdirectory dynobj for  	  *
*		 restrictions on its use.				  *
*								          *
*                Please feel free to modify Xpeg. See Todo for details.   *
*									  *
\*************************************************************************/

/*
 *  NAME
 *       strtrim - trim specified characters from a string.
 *
 *  SYNOPSIS
 *       char *strtrim(s1,s2)
 *       char *s1, *s2;
 *
 *  DESCRIPTION
 *       strtrim returns string s1 after deleting the leading and trailing
 *       occurrances of any characters in string s2.
 *
 *  AUTHOR
 *       Bruce Crabtree
 *       Software Systems Design, Inc.
 */

#include <stdio.h>
#include <ctype.h>
#include <string.h>

char *
strtrim(s1,s2)    /* strtrim: removes leading and trailing occurrances of */
   char *s1;      /*          any character in string s2 from s1.         */
   char *s2;
{
   char *r, *tmp, *p, *calloc();

   if (s1 == NULL)
      return(NULL);
   
   p = calloc(strlen(s1)+1,sizeof(char));
   (void) strcpy(p,s1);

   if (s2 != NULL) {
      for ( ; (*s1 != '\0') && (strchr(s2,*s1) != NULL); s1++)
                 ;                        /* remove leading characters */

                                          /* copy remaining portion of string */
      for (tmp=p ; *s1 != '\0'; s1++, tmp++)
          *tmp = *s1;
      *tmp = '\0';

      r = p + strlen(p) - 1;
      for (; strchr(s2,*r) != NULL; r--)  /* remove trailing characters */
             ;
      *(++r) = '\0';
   }
   return(p);
}


#ifdef TEST

char *text1 = { " *  This is a sample." };
char *text2 = { "This is a sample also.   " };
char *text3 = { "   This is another sample.  11111" };
char *text4 = { "This is the last sample." };

main()
{
        char *strtrim();

	printf("[%s] = [%s]\n",text1,strtrim(text1,"* \t\n"));
	printf("[%s] = [%s]\n",text2,strtrim(text2," \t\n"));
	printf("[%s] = [%s]\n",text3,strtrim(text3," \t\n1"));
	printf("[%s] = [%s]\n",text4,strtrim(text4," \t\n"));
        printf("[%s] = [%s]\n",text1,strtrim(text1,NULL));
        printf("[%s] = [%s]\n",NULL,strtrim(NULL," \t\n"));
}
#endif


