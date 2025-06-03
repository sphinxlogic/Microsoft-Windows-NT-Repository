/* Keymap routines for SLang.  The role of these keymap routines is simple:
 * Just read keys from the tty and return a pointer to a keymap structure.  
 * That is, a keymap is simple a mapping of strings (keys from tty) to 
 * structures.  Also included are routines for managing the keymaps. 
 */
/* 
 * Copyright (c) 1994 John E. Davis
 * All rights reserved.
 *
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, and distribute this
 * software and its documentation for any purpose, provided that the
 * above copyright notice and the following two paragraphs appear in
 * all copies of this software.   Permission is not granted to modify this
 * software for any purpose without written agreement from John E. Davis.
 *
 * IN NO EVENT SHALL JOHN E. DAVIS BE LIABLE TO ANY PARTY FOR DIRECT,
 * INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF
 * THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF JOHN E. DAVIS
 * HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * JOHN E. DAVIS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS"
 * BASIS, AND JOHN E. DAVIS HAS NO OBLIGATION TO PROVIDE MAINTENANCE,
 * SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */

#include <stdio.h>
#include <string.h>

#ifndef sequent
# include <stdlib.h>
#endif

#include "slang.h"
#include "_slang.h"

/* We need a define a rule for upperand lower case chars that user cannot
   change!  This could be a problem for international chars! */

#define UPPER_CASE_KEY(x) (((x) >= 'a') && ((x) <= 'z') ? (x) - 32 : (x))
#define LOWER_CASE_KEY(x) (((x) >= 'A') && ((x) <= 'Z') ? (x) + 32 : (x))

int SLang_Key_TimeOut_Flag = 0;	       /* true if more than 1 sec has elapsed
                                          without key in multikey sequence */

int SLang_Last_Key_Char;




SLKeyMap_List_Type SLKeyMap_List[SLANG_MAX_KEYMAPS];   /* these better be inited to 0! */

SLang_Key_Type *malloc_key(unsigned char *str)
{
   SLang_Key_Type *neew;

   if (NULL == (neew = (SLang_Key_Type *) SLMALLOC(sizeof(SLang_Key_Type))))
     {
	SLang_Error = SL_MALLOC_ERROR;
	return NULL;
     }
   MEMCPY((char *) neew->str, (char *) str, (int) *str);
   return(neew);
}


static SLKeyMap_List_Type *add_keymap(char *name, SLang_Key_Type *map)
{
   int i;

   for (i = 0; i < SLANG_MAX_KEYMAPS; i++)
     {
	if (SLKeyMap_List[i].keymap == NULL)
	  {
	     SLKeyMap_List[i].keymap = map;
	     MEMCPY((char *) SLKeyMap_List[i].name, (char *) name, MAX_KEYMAP_NAME_LEN);
	     SLKeyMap_List[i].name[MAX_KEYMAP_NAME_LEN] = 0; 
	     return &SLKeyMap_List[i];
	  }
     }
   SLang_Error = UNKNOWN_ERROR;
   /* SLang_doerror ("Keymap quota exceeded."); */
   return NULL;
}

VOID *SLang_find_key_function(char *name, SLKeyMap_List_Type *keymap)
{
   SLKeymap_Function_Type *fp = keymap -> functions;
   
   while((fp != NULL) && (fp->name != NULL))
     {
	if ((*fp->name == *name) && !strcmp(fp->name, name)) return((VOID *) fp->f);
	fp++;
     }
   return(NULL);
}

/* convert things like "^A" to 1 etc... The 0th char is the strlen */
char *SLang_process_keystring(char *s)
{
   static char str[30];
   unsigned char ch;
   int i;

    i = 1;
    while (*s != 0)
      {
          ch = (unsigned char) *s++;
          if (*s) ch = UPPER_CASE_KEY(ch);
          if (ch == '^')
            {
                ch = *s++;
                if (ch == 0)
                  {
                      str[i++] = '^';
                      break;
                  }
	       if (ch == '?') ch = 127; else ch = ch - 'A' + 1;
            }

          str[i++] = ch;
      }
    str[0] = i;
    return(str);
}


static char *Define_Key_Error = "Inconsistency in define key.";

/* returns -2 if inconsistent, -1 if malloc error, 0 upon success */
int SLang_define_key1(char *s, VOID *f, unsigned char type, SLKeyMap_List_Type *kml)
{
   int cmp, i, m, n, len;
   SLang_Key_Type *key, *last, *neew;
   unsigned char *str;

   str = (unsigned char *) SLang_process_keystring(s);
   if (1 == (n = str[0])) return 0;

   i = *(str + 1);
   key = &((kml->keymap)[i]);

   if (n == 2)
     {
	if (key->next != NULL)
	  {
	     SLang_doerror (Define_Key_Error);
	     return -2;
	  }
	
	/* copy keymap uses the pointers so do not free these if copied */
	if ((SLKeyMap_List[1].keymap == NULL)
	    && (key->type == SLKEY_F_INTERPRET)) SLFREE(key->f);
	
	key->f = f;
	key->type = type;
	MEMCPY((char *) key->str, (char *) str, 2);
	return 0;
     }

   /* insert the key definition */
   while(1)
     {
	last = key;
	key = key->next;
	if ((key == NULL) || (key->str == NULL)) cmp = -1;
	else
	  {
	     m = key->str[0];
	     len = m;
	     if (m > n) len = n;
	     cmp = MEMCMP((char *) (str + 1), (char *)(key->str + 1), len - 1);
	     if ((m != n) && !cmp)
	       {
		  SLang_doerror (Define_Key_Error); 
		  return -2;
	       }
	     if ((m == n) && (!cmp)) cmp = MEMCMP((char *) str, (char *) key->str, n);
	  }

	if (cmp == 0)
	  {
	       /* copy keymap uses the pointers so do not free these if copied */
	       if ((SLKeyMap_List[1].keymap == NULL)
		   && (key->type == SLKEY_F_INTERPRET)) SLFREE(key->f);
	       key->f = f;
	       key->type = type;
	       return 0;
            }
          else if (cmp < 0)
            {
	       if (NULL == (neew = malloc_key(str))) return -1;

	       neew -> next = key;
	       last -> next = neew;

	       neew->f = f;
	       neew->type = type;
	       return 0;
            } /* else cmp > 0 */
      }
}

int SLang_define_key(char *s, char *funct, SLKeyMap_List_Type *kml)
{
   VOID *f;
   unsigned char type;

   f = SLang_find_key_function(funct, kml);
   if (f == NULL)                      /* assume interpreted */
     {
	if (NULL == (f = (VOID *) SLMALLOC(strlen(funct) + 1)))
	  {
	     SLang_Error = SL_MALLOC_ERROR;
	     SLang_doerror ("Malloc error in define key.");
	     return -1;
	  }

	strcpy((char *) f, funct);
	type = SLKEY_F_INTERPRET;
     }
   else type = SLKEY_F_INTRINSIC;

   return SLang_define_key1(s, f, type, kml);
}


SLang_Key_Type *SLang_do_key(SLKeyMap_List_Type *kml, int (*getkey)(void))
{
   register SLang_Key_Type *key, *next;
   int i;
   unsigned char ch1, ch;
   register unsigned char chi, chup, chlow;
   
   ch = (unsigned char) (*getkey)();
   
   SLang_Last_Key_Char = (int) ch;

   i = ch1 = (unsigned char) ch;

   key = (SLang_Key_Type *) &((kml->keymap)[i]);

   /* if the next one is null, then we know this is it. */
   if (key->next == NULL) 
     {
	/* check its uppercase counterpart if null and return it */
	if ((key->f == NULL) && (i >= 'a') && (i <= 'z')) key = (SLang_Key_Type *) &((kml->keymap)[i - 32]);
	SLang_Key_TimeOut_Flag = 0;
	return(key);
     }
   
   key = key->next;
   SLang_Key_TimeOut_Flag = 1;
   SLang_Last_Key_Char = (*getkey)();
   
   i = 2;			       /* 0 is keylen, 1 matches, so 2 */
   ch1 = (unsigned char) SLang_Last_Key_Char;
   chup = UPPER_CASE_KEY(ch1); chlow = LOWER_CASE_KEY(ch1);

   while(key != NULL)
     {
	if (SLKeyBoard_Quit) break;
	chi = key->str[i];
	if ((chup == chi) || (chlow == chi))
	  {
	     if (*key->str == i + 1)
	       {
		  /* look for exact match if possible */
		  if (ch1 != chi)
		    {
		       next = key->next;
		       while (next != NULL)
			 {
			    if (next->str[i] == ch1)
			      {
				 if (next->str[0] == i + 1) 
				   {
				      SLang_Key_TimeOut_Flag = 0;
				      return(next);
				   }
				 break;
			      }
			    next = next->next;
			 }
		    }
		  SLang_Key_TimeOut_Flag = 0;
		  return(key);
	       }

	     /* before reading a new key, check to see if it is lowercase
	      * and try to find a match for it.  Note that this works because
	      * only the LAST character in a key sequence can be lowercase */

	     else if ((ch1 == chlow) && (ch1 != chup))
	       {
		  next = key->next;
		  /* look for the lowercase one */
		  while (next != NULL)
		    {
                        if ((next->str[i] == chlow) &&
                            (next->str[0] == i + 1)) 
		       	 {
			    SLang_Key_TimeOut_Flag = 0;
			    return(next);
			 }
		       next = next->next;
		    }
	       }

	     SLang_Last_Key_Char = ch1 = (*getkey)();
	     i++;
	     chup = UPPER_CASE_KEY(ch1);
	     chlow = LOWER_CASE_KEY(ch1);
	     continue;
	  }
	else if (chlow < chi) break;  /* not found */

	/* else */
	key = key->next;
     }
   /* not found */
   SLang_Key_TimeOut_Flag = 0;
   return(NULL);
}



void SLang_undefine_key(char *s, SLKeyMap_List_Type *kml)
{
   int n, i;
   SLang_Key_Type *key, *next, *last, *key_root, *keymap;
   unsigned char *str;

   keymap = kml -> keymap;
   str = (unsigned char *) SLang_process_keystring(s);

   if (0 == (n = *str++ - 1)) return;
   i = *str;
   key = key_root = (SLang_Key_Type *) &(keymap[i]);

   last = key_root;
   key = key_root->next;
   while (key != NULL)
     {
	next = key->next;
	if (!MEMCMP((char *)(key->str + 1), (char *) str, n))
	  {
	     if ((SLKeyMap_List[1].keymap == NULL)
		 && (key->type == SLKEY_F_INTERPRET)) SLFREE(key->f);
	     SLFREE(key);
	     last->next = next;
	  }
	else last = key;
	key = next;
     }

   if (n == 1)
     {
	*key_root->str = 0;
	key_root->f = NULL;
	key_root->type = 0;
     }
}

char *SLang_make_keystring(unsigned char *s)
{
   static char buf[40];
   char *b;
   int n;

   b = buf;
   n = *s++ - 1;
   while (n--)
     {
	if (*s < 32)
	  {
	     *b++ = '^';
	     *b++ = *s + 'A' - 1;
	  }
	else *b++ = *s;
	s++;
     }
   *b = 0;
   return(buf);
}




static SLang_Key_Type *copy_keymap(SLKeyMap_List_Type *kml)
{
   int i;
   SLang_Key_Type *neew, *old, *new_root, *km;

   if (NULL == (new_root = (SLang_Key_Type *) SLCALLOC(256, sizeof(SLang_Key_Type))))
     {
	SLang_Error = SL_MALLOC_ERROR;
	return NULL;
     }

   if (kml == NULL) return new_root;
   km = kml->keymap;

   
   for (i = 0; i < 256; i++)
     {
	old = &(km[i]);
	neew = &(new_root[i]);
	neew->f = old->f;
	neew->type = old->type;
	MEMCPY((char *) neew->str, (char *) old->str, (int) *old->str);

	old = old->next;
	while (old != NULL)
	  {
	     neew->next = malloc_key((unsigned char *) old->str);
	     neew = neew->next;
	     neew->f = old->f;
	     neew->type = old->type;
	     old = old->next;
	  }
	neew->next = NULL;
     }
   return(new_root);
}


SLKeyMap_List_Type *SLang_create_keymap(char *name, SLKeyMap_List_Type *map)
{
   SLang_Key_Type *neew;
   SLKeyMap_List_Type *new_map;
   
   if ((NULL == (neew = copy_keymap(map)))
       || (NULL == (new_map = add_keymap(name, neew)))) return NULL;
   
   if (map != NULL) new_map -> functions = map -> functions;
   
   return new_map;
}

SLKeyMap_List_Type *SLang_find_keymap(char *name)
{
   int i;

   for (i = 0; i < SLANG_MAX_KEYMAPS; i++)
     {
	if (!strncmp(SLKeyMap_List[i].name, name, 8)) return &SLKeyMap_List[i];
     }
   return(NULL);
}
