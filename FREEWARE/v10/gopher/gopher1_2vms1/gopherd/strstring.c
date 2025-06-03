/********************************************************************
 * lindner
 * 3.6
 * 1993/10/27 18:53:23
 * /home/mudhoney/GopherSrc/CVS/gopher+/object/STRstring.c,v
 * $Status: $
 *
 * Paul Lindner, University of Minnesota CIS.
 *
 * Copyright 1991, 1992 by the Regents of the University of Minnesota
 * see the file "Copyright" in the distribution for conditions of use.
 *********************************************************************
 * MODULE: STRstring.c
 * Implement dynamic string library functions
 *********************************************************************
 * Revision History:
 * STRstring.c,v
 * Revision 3.6  1993/10/27  18:53:23  lindner
 * don't forget \0
 *
 * Revision 3.5  1993/10/22  20:15:52  lindner
 * Remove superfulous declaration of len (Fote)
 *
 * Revision 3.4  1993/10/19  20:46:00  lindner
 * Better, tighter STRstring stuff (Fote)
 *
 * Revision 3.3  1993/08/16  19:35:09  lindner
 * Return a correct value for STRcpy
 *
 * Revision 3.2  1993/03/24  17:07:52  lindner
 * STRset with a NULL value will STRinit() the string
 *
 * Revision 3.1.1.1  1993/02/11  18:03:03  lindner
 * Gopher+1.2beta release
 *
 * Revision 1.1  1992/12/10  23:27:52  lindner
 * gopher 1.1 release
 *
 *
 *********************************************************************/


#include "STRstring.h"
#include "String.h"
#include "Malloc.h"

/*
 * Make a new string, however use supplied parameter to set it.
 */

String *
STRnewSet(in)
  char *in;
{
     register String *temp;

     temp = (String *) malloc(sizeof(String));
     temp->data = NULL;

     if (in == NULL)
	  return(temp);

     STRset(temp, in);
     return(temp);
}
     
/*
 * Make a new string, don't set anything for default yet.
 */

String *
STRnew()
{
     String *temp;

     temp = (String *) malloc(sizeof(String));
     temp->data = NULL;
     temp->len = 0;

     return(temp);
}

/*
 * Destroy a string
 */

void
STRdestroy(st)
  String *st;
{
     if (st != NULL) {
	  if (st->data != NULL)
	       free(st->data);
	  free(st);
     } else
	  perror("STRdestroy: non existant string!\n");

}


/*
 * Clear out all the crud...
 */

void 
STRinit(st) 
  String *st;
{
     if (st != NULL) {
	  
	  st->len  = 0;
	  if (st->data != NULL) {
	       free(st->data);
	       st->data = NULL;
	  }
     } else
	  perror("STRinit, non existant string!");
}

/*
 * Set a string value
 */

void
STRset(st, str)
  String *st;
  char   *str;
{
     register int len;

     if (str == NULL) {
	  STRinit(st);
	  return;
     }

     if (*str == '\0')
	  len = 1;
     else
	  len = strlen(str)+1; /** Don't forget the '\0' **/

     /* Uninitialized data... */

     if (st->data == NULL) {
	  st->data = (char *) malloc(sizeof(char*) * len);
	  st->len = len;
     }

     /** Something's already there... **/

     else if (STRsize(st) < len) {
	  char *temp;

	  temp = (char *) realloc(st->data, len);
	  /*** Should check for NULL ... ***/
	  if (temp == NULL)
	       perror("realloc failed...");

	  st->data = temp;
	  st->len  = len;
     }
     /* space is ok and st->len is set, so copy in the new string */
     strcpy(st->data, str);
}

/*
 * Add a string to the end of the string that's there already
 */

String*
STRcat(st, cp)
  String *st;
  char *cp;
{
     int len;
     char *temp;

     if (cp == NULL)
	  return(NULL);
     
     if (STRlen(st) == 0) {
	  STRset(st, cp);
	  return(st);
     }

     len = strlen(cp) + STRlen(st) + 1;

     temp = (char *) malloc(len);
     strcpy(temp, STRget(st));
     strcat(temp, cp);

     STRset(st, temp);
     
     free(temp);

     return(st);
}


int
STRcmp(st1, st2)
  String *st1;
  String *st2;
{
     register char *cp1, *cp2;

     cp1 = STRget(st1);
     cp2 = STRget(st2);

     if (cp1 == NULL) 
	  return(- !0);
     else if (cp2 == NULL)
	  return( !0);
     else
	  return(strcmp(cp1, cp2));
}
     
String*
STRcpy(s1, s2)
  String *s1;
  String *s2;
{
     STRset(s1, STRget(s2));
     return(s1);
}
