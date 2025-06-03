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


#include <malloc.h>


char *strdup(old_str)
{
   char *new_str;

         new_str = (char *) malloc(strlen(old_str)+1*sizeof(char));
	       strcpy(new_str,old_str);
	             return new_str;
}

