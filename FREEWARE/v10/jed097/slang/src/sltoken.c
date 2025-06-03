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
#include "slang.h"
#include "_slang.h"

/* There are 1s at positions " %\t{}[];():*,/" */
static unsigned char special_chars[256] = 
{
   0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
   0,0,0,0,1,0,0,1,1,1,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

char *SLexpand_escaped_char(char *p, char *ch)
{
   char ch1;
   int num = 0;
   int base = 16, i = -1;
   int max = '9';
   ch1 = *p++;
   switch (ch1)
     {
      case 'n': ch1 = '\n'; break;
      case 't': ch1 = '\t'; break;
      case 'v': ch1 = '\v'; break;
      case 'b': ch1 = '\b'; break;
      case 'r': ch1 = '\r'; break;
      case 'f': ch1 = '\f'; break;
      case 'e': ch1 = 27; break;
      case 'a': ch1 = 7; break;
      case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': 
	max = '7'; base = 8; i = 2; num = ch1 - '0';
	/* fall */
	
      case 'd':			       /* decimal -- S-Lang extension */
	if (ch1 == 'd')
	  {
	     base = 10; i = 3;
	  }
	
      case 'x':
	
	while(i--)
	  {
	     ch1 = *p;
	     
	     if ((ch1 <= max) && (ch1 >= '0'))
	       {
		  num = base * num + (ch1 - '0');
	       }
	     else if ((base == 16) && (ch1 >= 'A'))
	       {
		  if (ch1 >= 'a') ch1 -= 32;
		  if (ch1 <= 'F')
		    {
		       num = base * num + 10 + (ch1 - 'A');
		    }
		  else break;
	       }
	     else break;
	     p++;
	  }
	ch1 = (char) num;
	
     }
   *ch = ch1;
   return p;
}

void SLexpand_escaped_string (register char *s, register char *t, 
			      register char *tmax)
{
   char ch;
   
   while (t < tmax)
     {
	ch = *t++;
	if (ch == '\\')
	  {
	     t = SLexpand_escaped_char (t, &ch);
	  }
	*s++ = ch;
     }
   *s = 0;
}


int SLang_extract_token (char **linep, char *word_parm, int byte_comp)
{
   register char ch, *line, *word = word_parm;
   int string;
   char ch1;

    line = *linep;

    /* skip white space */
    while (ch = *line++, (ch == ' ') || (ch == '\t'));

    if ((!ch) || (ch == '\n'))
      {
	 *linep = line;
	 return(0);
      }

   *word++ = ch;
   
   /* Look for -something and rule out --something and -= something */
   if ((ch == '-') && 
       (*line != '-') && (*line != '=') && ((*line > '9') || (*line < '0')))
     {
	*word = 0;
	*linep = line;
	return 1;
     }
   
       
   if (ch == '"') string = 1; else string = 0;
   if (ch == '\'')
     {
	if ((ch = *line++) != 0)
	  {
	     if (ch == '\\') 
	       {
		  line = SLexpand_escaped_char(line, &ch1);
		  ch = ch1;
	       }
	     if (*line++ == '\'')
	       {
		  --word;
		  sprintf(word, "%d", (int) ((unsigned char) ch));
		  word += 4;  ch = '\'';
	       }
	     else SLang_Error = SYNTAX_ERROR;
	  }
	else SLang_Error = SYNTAX_ERROR;
     }
   else  if (!special_chars[(unsigned char) ch])
     {
	while(ch = *line++, (ch > '"') || ((ch != '\n') && (ch != 0) && (ch != '"')))
	  {
	     if (string)
	       {
		  if (ch == '\\')
		    {
		       ch = *line++;
		       if ((ch == 0) || (ch == '\n')) break;
		       if (byte_comp) *word++ = '\\';
		       else 
			 {
			    line = SLexpand_escaped_char(line - 1, &ch1);
			    ch = ch1;
			 }
		    }
	       }
	     else if (special_chars[(unsigned char) ch])
	       {
		  line--;
		  break;
	       }
	     
	     *word++ = ch;
	  }
     }
   
   if ((!ch) || (ch == '\n')) line--;
   if ((ch == '"') && string) *word++ = '"'; else if (string) SLang_Error = SYNTAX_ERROR;
   *word = 0;
   *linep = line;
   /* massage variable-- and ++ into --variable, etc... */
   if (((int) (word - word_parm) > 2)
       && (ch = *(word - 1), (ch == '+') || (ch == '-'))
       && (ch == *(word - 2)))
     {
	word--;
	while (word >= word_parm + 2)
	  {
	     *word = *(word - 2);
	     word--;
	  }
	*word-- = ch;
	*word-- = ch;
     }
   return(1);
}


unsigned char slang_guess_type (char *t)
{
   char *p;
   register char ch;

   if (*t == '-') t++;
   p = t;
#ifdef FLOAT_TYPE
   if (*p != '.') 
     {
#endif
	while ((*p >= '0') && (*p <= '9')) p++;
	if (t == p) return(STRING_TYPE);
	if ((*p == 'x') && (p == t + 1))   /* 0x?? */
	  {
	     p++;
	     while (ch = *p, 
		    ((ch >= '0') && (ch <= '9'))
		    || ((ch >= 'A') && (ch <= 'F'))) p++;
	  }
	if (*p == 0) return(INT_TYPE);
#ifndef FLOAT_TYPE
	return(STRING_TYPE);
#else
     }
   
   /* now down to float case */
   if (*p == '.')
     {
	p++;
	while ((*p >= '0') && (*p <= '9')) p++;
     }
   if (*p == 0) return(FLOAT_TYPE);
   if ((*p != 'e') && (*p != 'E')) return(STRING_TYPE);
   p++;
   if (*p == '-') p++;
   while ((*p >= '0') && (*p <= '9')) p++;
   if (*p != 0) return(STRING_TYPE); else return(FLOAT_TYPE);
#endif
}

