/*
 *  Copyright (c) 1992 John E. Davis  (davis@amy.tch.harvard.edu)
 *  All Rights Reserved.
 */
/* delete next word through whitespace.  What is a word?

   Here I consider two types of words:

      a) is a sequence of alphanumeric characters.

      b) consists of non-whitespace, non-alphanumeric charcters.

   Delete word means:

      1. Delete whitespace.
      2. Delete word (type (a) or type (b))
      3. If the word was type (a) then done.
      4. If the word was type (b) consisting of only a single character and
         the next non-deleted character is not whitespace then delete the
	 next word and exit.
*/

int delete_word()
{
   volatile unsigned char *p;
   int len = 0;

   /* delete whitespace */

   while(1)
     {
	p = CLine->data + Point;
	if (*p <= ' ') del(); else break;
     }

   if (isalpha(*p) || isdigit(*p))
     {
	do
	  {
	     del();
	     p = CLine->data + Point;
	  }
	while (isalpha(*p) || isdigit(*p));
	return(1);
     }

   /* type b */

   del();
   p = CLine->data + Point;
   if (isalpha(*p) || isdigit(*p)) return(delete_word());

   while(*p >= ' ')
     {
	if ((isalpha(*p) || isdigit(*p))) break;
	del();
	p = CLine->data + Point;
     }
   return(1);
}
