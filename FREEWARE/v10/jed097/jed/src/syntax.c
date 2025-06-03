/*
 *  Copyright (c) 1994 John E. Davis  (davis@amy.tch.harvard.edu)
 *  All Rights Reserved.
 */
#include <stdio.h>
#include "buffer.h"
#include "screen.h"

static unsigned char Symbol_Colors[128] = 
{
   JNORMAL_COLOR,	/* 0 */
   JNORMAL_COLOR,	/* 1 */
   JNORMAL_COLOR,	/* 2 */
   JNORMAL_COLOR,	/* 3 */
   JNORMAL_COLOR,	/* 4 */
   JNORMAL_COLOR,	/* 5 */
   JNORMAL_COLOR,	/* 6 */
   JNORMAL_COLOR,	/* 7 */
   JNORMAL_COLOR,	/* 8 */
   JNORMAL_COLOR,	/* 9 */
   JNORMAL_COLOR,	/* 10 */
   JNORMAL_COLOR,	/* 11 */
   JNORMAL_COLOR,	/* 12 */
   JNORMAL_COLOR,	/* 13 */
   JNORMAL_COLOR,	/* 14 */
   JNORMAL_COLOR,	/* 15 */
   JNORMAL_COLOR,	/* 16 */
   JNORMAL_COLOR,	/* 17 */
   JNORMAL_COLOR,	/* 18 */
   JNORMAL_COLOR,	/* 19 */
   JNORMAL_COLOR,	/* 20 */
   JNORMAL_COLOR,	/* 21 */
   JNORMAL_COLOR,	/* 22 */
   JNORMAL_COLOR,	/* 23 */
   JNORMAL_COLOR,	/* 24 */
   JNORMAL_COLOR,	/* 25 */
   JNORMAL_COLOR,	/* 26 */
   JNORMAL_COLOR,	/* 27 */
   JNORMAL_COLOR,	/* 28 */
   JNORMAL_COLOR,	/* 29 */
   JNORMAL_COLOR,	/* 30 */
   JNORMAL_COLOR,	/* 31 */
   JNORMAL_COLOR,	/* 32 */
   JOP_COLOR,	/* ! */
   JSTR_COLOR,	/* " */
   JOP_COLOR,	/* # */
   JKEY_COLOR,	/* $ */
   JOP_COLOR,	/* % */
   JOP_COLOR,	/* & */
   JSTR_COLOR,	/* ' */
   JDELIM_COLOR,	/* ( */
   JDELIM_COLOR,	/* ) */
   JOP_COLOR,	/* * */
   JOP_COLOR,	/* + */
   JDELIM_COLOR,	/* , */
   JOP_COLOR,	/* - */
   JDELIM_COLOR,	/* . */
   JOP_COLOR,	/* / */
   JNUM_COLOR,	/* 0 */
   JNUM_COLOR,	/* 1 */
   JNUM_COLOR,	/* 2 */
   JNUM_COLOR,	/* 3 */
   JNUM_COLOR,	/* 4 */
   JNUM_COLOR,	/* 5 */
   JNUM_COLOR,	/* 6 */
   JNUM_COLOR,	/* 7 */
   JNUM_COLOR,	/* 8 */
   JNUM_COLOR,	/* 9 */
   JOP_COLOR,	/* : */
   JDELIM_COLOR,	/* ; */
   JOP_COLOR,	/* < */
   JOP_COLOR,	/* = */
   JOP_COLOR,	/* > */
   JOP_COLOR,	/* ? */
   JKEY_COLOR,	/* @ */
   JKEY_COLOR,	/* A */
   JKEY_COLOR,	/* B */
   JKEY_COLOR,	/* C */
   JKEY_COLOR,	/* D */
   JKEY_COLOR,	/* E */
   JKEY_COLOR,	/* F */
   JKEY_COLOR,	/* G */
   JKEY_COLOR,	/* H */
   JKEY_COLOR,	/* I */
   JKEY_COLOR,	/* J */
   JKEY_COLOR,	/* K */
   JKEY_COLOR,	/* L */
   JKEY_COLOR,	/* M */
   JKEY_COLOR,	/* N */
   JKEY_COLOR,	/* O */
   JKEY_COLOR,	/* P */
   JKEY_COLOR,	/* Q */
   JKEY_COLOR,	/* R */
   JKEY_COLOR,	/* S */
   JKEY_COLOR,	/* T */
   JKEY_COLOR,	/* U */
   JKEY_COLOR,	/* V */
   JKEY_COLOR,	/* W */
   JKEY_COLOR,	/* X */
   JKEY_COLOR,	/* Y */
   JKEY_COLOR,	/* Z */
   JDELIM_COLOR,	/* [ */
   JDELIM_COLOR,	/* \ */
   JDELIM_COLOR,	/* ] */
   JOP_COLOR,	/* ^ */
   JKEY_COLOR,	/* _ */
   JKEY_COLOR,	/* ` */
   JKEY_COLOR,	/* a */
   JKEY_COLOR,	/* b */
   JKEY_COLOR,	/* c */
   JKEY_COLOR,	/* d */
   JKEY_COLOR,	/* e */
   JKEY_COLOR,	/* f */
   JKEY_COLOR,	/* g */
   JKEY_COLOR,	/* h */
   JKEY_COLOR,	/* i */
   JKEY_COLOR,	/* j */
   JKEY_COLOR,	/* k */
   JKEY_COLOR,	/* l */
   JKEY_COLOR,	/* m */
   JKEY_COLOR,	/* n */
   JKEY_COLOR,	/* o */
   JKEY_COLOR,	/* p */
   JKEY_COLOR,	/* q */
   JKEY_COLOR,	/* r */
   JKEY_COLOR,	/* s */
   JKEY_COLOR,	/* t */
   JKEY_COLOR,	/* u */
   JKEY_COLOR,	/* v */
   JKEY_COLOR,	/* w */
   JKEY_COLOR,	/* x */
   JKEY_COLOR,	/* y */
   JKEY_COLOR,	/* z */
   JDELIM_COLOR,	/* { */
   JOP_COLOR,	/* | */
   JDELIM_COLOR,	/* } */
   JOP_COLOR,	/* ~ */
   JNORMAL_COLOR,
};

/* These are the C++ keywords:
 * asm auto
 * break
 * case catch char class const continue
 * default delete do double
 * else enum extern
 * float for friend
 * goto
 * if inline int
 * long
 * new
 * operator
 * private protected public
 * register return
 * short signed sizeof static struct switch
 * template this throw try typedef
 * union unsigned
 * virtual void volatile
 * while
 */

static char *C_Keywords[] = /* length 2 to 9 */
{
   "doif",			       /* di */
   "asmforintnewtry",		       /* afint */
   "autocasecharelseenumgotolongthisvoid",    /* acceegltv */
   "breakcatchclassconstfloatshortthrowunionwhile",   /* bcccfstuw */
   "deletedoubleexternfriendinlinepublicreturnsignedsizeofstaticstructswitch",
   				       /* ddefiprsssss */
   "defaultprivatetypedefvirtual",     /* dptv */
   "continueoperatorregistertemplateunsignedvolatile",   /* cortuv */
   "protected",			       /* p */
};

/* Fortran 77 keywords + include, record, structure, while:
 * backspace block
 * call character common complex continue
 * data dimension do double
 * else end enddo endfile endif entry equivalence exit external
 * format function
 * goto 
 * if implicit include inquire integer intrinsic
 * logical
 * parameter pause precision program
 * real return rewind
 * save stop subroutine
 * then
 * while
 */

static char *Fortran_Keywords[] = /* length 2 to 11 */
{
   "dogoifto",			       /* 2: di */
   "end",			       /* 3: e */
   "calldataelseexitgotoopenreadrealsavestopthen",  /* 4: cdeegorrsst */
   "blockcloseenddoendifentrypauseprintwhilewrite", /* 5: bceeeppww */
   "commondoubleformatrecordreturnrewind",          /* 6: cdfrrr */
   "complexendfileincludeinquireintegerlogicalprogram", /* 7: ceiiilp */
   "continueexternalfunctionimplicit", /* 8: cefi */
   "backspacecharacterdimensionintrinsicparameterprecisionstructure", /* 9: bcdipps */
   "subroutine",		       /* 10: s */
   "equivalence"		       /* 11: e */
};

static int Max_Keyword_Len;	       /* max length of a keyword */
static char **Keywords = NULL;	       /* array of keywords */
static int The_Highlight_Mode;
static int Keyword_Not_Case_Sensitive;

void init_syntax_highlight (void)
{
   unsigned int flags = CBuf->modes;
   
   Keyword_Not_Case_Sensitive = 0;
   Mode_Has_Syntax_Highlight = 1;
   if (flags & C_MODE)
     {
	Max_Keyword_Len = 9;
	Keywords = C_Keywords;
	The_Highlight_Mode = C_MODE;
     }
   else if (flags & TEX_MODE)
     {
	The_Highlight_Mode = TEX_MODE;
     }
   else if (flags & F_MODE)
     {
	Max_Keyword_Len = 11;
	Keywords = Fortran_Keywords;
	Keyword_Not_Case_Sensitive = 0x20;   /* This value is exploited */
	The_Highlight_Mode = F_MODE;
     } 
   else	Mode_Has_Syntax_Highlight = 0;
}


static unsigned short *highlight_token(register unsigned short *p, unsigned short *pmax)
{
   register char *t;
   register unsigned short *q;
   unsigned char ch = (unsigned char) *p;
   int color, skip = 0, n;
   
   q = p;
   while (q < pmax)
     {
	ch = (unsigned char) *q & 0x7F;
	color = Symbol_Colors[ch];
	if (color == JNUM_COLOR) skip = 1;
	else if (color != JKEY_COLOR) break;
	q++;
     }

   if (skip) return q;
   n = (int) (q - p);
   if ((n < 2) || (n > Max_Keyword_Len)) return q;
   
   t = Keywords[n - 2];
   while (*t)
     {
	p = q - n;
	if (Keyword_Not_Case_Sensitive == 0)
	  {
	     while ((p < q) && (*t == (char) *p))
	       {
		  p++; t++;
	       }
	  }
	else  while ((p < q) && (*t == ((char) *p | 0x20)))
	  {
	     p++; t++;
	  }
	
	if (p == q)
	  {
	     p = q - n;
	     while (p < q) *p++ |= JKEY_COLOR << 8;
	     return q;
	  }
	
	/* alphabetical */
	if (*t > ((char) *p | Keyword_Not_Case_Sensitive))
	  break;
	
	t += (int) (q - p);
     }
   return q;
}

	

unsigned short *highlight_string (unsigned short *p, unsigned short *pmax)
{
   unsigned char ch;
   unsigned char q = (unsigned char) *p;

   *p++ |= JSTR_COLOR << 8;
   while (p < pmax)
     {
	ch = (unsigned char) *p;
	*p++ |= JSTR_COLOR << 8;
	if (ch == q) break;
	if ((ch == '\\') && (p < pmax))
	  {
	     *p++ |= JSTR_COLOR << 8;
	  }
     }
   return p;
}

unsigned short *highlight_number (unsigned short *p, unsigned short *pmax)
{
   unsigned short *p1;
   unsigned char ch, ch1;
   unsigned char color;
   int expon = 0, dec = 0;
   
   ch = (unsigned char) *p;
   if (ch == '-')
     {
	p1 = p + 1;
	if ((p1 < p) && (JNUM_COLOR == Symbol_Colors[(unsigned char) *p1]))
	  {
	     *p |= JNUM_COLOR << 8;
	     *p1 |= JNUM_COLOR << 8;
	     p += 2;
	  }
	else 
	  {
	     *p |= JOP_COLOR;
	     return p + 1;
	  }
     }
   
   while (p < pmax)
     {
	ch = (unsigned char) *p;
	color = Symbol_Colors[ch & 0x7F];
	
	if (color != JNUM_COLOR)
	  {
	     if (expon > 0) break;
	     if (ch == '.')
	       {
		  if (dec || (expon < 0)) break;
		  dec = 1;
	       }
	     else 
	       {
		  ch1 = ch | 0x20;
		  if (The_Highlight_Mode == F_MODE)
		    {
		       expon = 1;
		       if ((ch1 != 'e') && (ch1 != 'd')) break;   /* exponent */
		       if (p + 1 < pmax)
			 {
			    ch1 = *(p + 1);
			    if ((ch1 == '-') || (JNUM_COLOR == Symbol_Colors[ch1 & 0x7F]))
				*p++ |= JNUM_COLOR << 8;
			    else break; 
			 }
		    }
		  else if (ch1 != 'x')
		    {
		       if (expon == 0)
			 {
			    if (ch1 != 'e') break;
			    if ((p + 1 < pmax) && (*(p + 1) == '-'))
			      *p++ |= JNUM_COLOR << 8;
			    expon = 1;
			 }
		       /* Note that expon == -1 here */
		       else if (color != JKEY_COLOR) break;
		    }
		  else if (expon == -1) break;
		  else expon = -1;
	       }
	  }
	
	*p++ |= JNUM_COLOR << 8;
     }
	     
   return p;
}
	  
unsigned short *C_highlight_comment (unsigned short *p, unsigned short *pmax)
{
   char ch;
   while (p < pmax)
     {
	ch = (char) *p;
	*p++ |= JCOM_COLOR << 8;
	if ((ch == '*') && (p < pmax) && (*p == '/'))
	  {
	     *p++ |= JCOM_COLOR << 8;
	     break;
	  }
     }
   return p;
}

static unsigned short *highlight_preprocess (unsigned short *p, unsigned short *pmax)
{
   while (p < pmax)
     {
	*p++ |= JPREPROC_COLOR << 8;
     }
   return p;
}

void syntax_highlight (register unsigned short *p, register unsigned short *pmax)
{
   register unsigned char ch = 0;
   unsigned short *pmin;
   register unsigned int color;

   /* If we are looking at fortran, then lets assume that the user is not
    * editing beyond 80 columns so that if something other than a digit
    * is present, we have a comment.
    */
   if (CBuf->modes & F_MODE)
     {
	ch = (unsigned char) *p;
	if (((ch > '9') || (ch < '0')) && (ch > ' '))
	  {
	     while (p < pmax) *p++ |= JCOM_COLOR << 8;
	     return;
	  }
     }

   /* Skip the whitespace and if we find a '*' with a followed by whitespace,
    * highlight it as a comment-- like these lines
    */
   
   while (p < pmax)
     {
	ch = (unsigned char) *p;
	if (ch > ' ') break;
	p++;
     }
   
   if (p == pmax) return;
   
   pmin = p;

   if (The_Highlight_Mode == TEX_MODE)
     {
	while (p < pmax)
	  {
	     unsigned char ch1;
	     unsigned short *p1;
	     
	     ch = (unsigned char) *p;
	     ch1 = ch | 0x20;
	     
	     if (((ch1 >= 'a') && (ch1 <= 'z'))
		 || (ch == ' '))
	       p++;
	     else if (ch == '\\')
	       {
		  *p++ |= JKEY_COLOR << 8;
		  p1 = p;
		  while (p < pmax)
		    {
		       ch = *p;
		       ch1 = ch | 0x20;
		       
		       if ((ch1 >= 'a') && (ch1 <= 'z'))
		       	 *p++ |= JKEY_COLOR << 8;
		       else if (p == p1)
			 {
			    if ((ch == '(') || (ch == '[') || (ch == ')') || (ch == ']'))
			      {
				 *(p - 1) = (unsigned short) '\\' | (JNUM_COLOR << 8);
				 *p++ |= JNUM_COLOR << 8;
			      }
			    else
			      *p++ |= JKEY_COLOR << 8;
			    break;
			 }
		       else break;
		    }
	       }
	     else if (ch == '%')
	       {
		  while (p < pmax) *p++ |= JCOM_COLOR << 8;
		  return;
	       }
	     else if (ch == '$')
	       {
		  *p++ = (unsigned short) '$' | (JNUM_COLOR << 8);
		  if (p == pmax) break;
		  ch1 = (unsigned char) *p;
		  if (ch1 == '$')
		    {
		       *p++ = (unsigned short) '$' | (JNUM_COLOR << 8);
		    }
		  
		  while ((p < pmax) && ((ch = (unsigned char) *p) != '$'))
		    {
		       if (ch == '%')
			 {
			    while (p < pmax) *p++ |= JCOM_COLOR << 8;
			    return;
			 }
		       
		       *p++ = (unsigned short) ch | (JNUM_COLOR << 8);
		       if ((ch == '\\') && (p < pmax))
			 {
			    *p++ |= (JNUM_COLOR << 8);
			 }
		    }
		  if (p < pmax) *p++ = (unsigned short) '$' | (JNUM_COLOR << 8);
		  if ((ch1 == '$') && (p < pmax))
		    {
		       if (*p == ch1)
		       	 *p++ = (unsigned short) '$' | (JNUM_COLOR << 8);
		       else while (p < pmax) *p++ |= (JNUM_COLOR << 8);
		    }
	       }
	     else if ((ch == '{') || (ch == '}') || (ch == '&')) *p++ |= JKEY_COLOR << 8;
	     else p++;
	  }
	return;
     }
   
   
   if (ch <= '*')
     {
	if (ch == '#') 
	  {
	     highlight_preprocess (p, pmax);
	     return;
	  }
	else if (((ch == '*') && (CBuf->modes & C_MODE))
		 || ((ch == '%') && (CBuf->modes == SL_MODE)))
	  {
	     p++;
	     while ((p < pmax) && ((unsigned char) *p == ch)) p++;
	     if ((p == pmax) || (*p <= ' ')) 
	       p = C_highlight_comment (pmin, pmax);
	     else p = pmin;
	  }
     }
	
	     
   while (p < pmax)
     {
	ch = (unsigned char) *p;
	if ((ch > 32) && (ch < 127))
	  {	     
	     color = (unsigned int) Symbol_Colors[ch];
	     if (color == JKEY_COLOR) p = highlight_token (p, pmax);
	     else if (color == JSTR_COLOR) p = highlight_string(p, pmax);
	     else if (color == JNUM_COLOR) p = highlight_number (p, pmax);
	     else if (color == JDELIM_COLOR) *p++ |= color << 8; 
	     else if ((ch == '/') && (p + 1 < pmax))
	       {
		  ch = *(p + 1);
		  if (ch == '/')
		    {
		       if (The_Highlight_Mode == C_MODE)
			 {
			    while (p < pmax)
			      {
				 *p++ |= JCOM_COLOR << 8;
			      }
			    return;
			 }
		       else *p++ |= JOP_COLOR << 8;
		    }
		  else if (ch == '*') 
		    {
		       *p++ |= JCOM_COLOR << 8; *p++ |= JCOM_COLOR << 8;
		       p = C_highlight_comment (p, pmax);
		    }
		  else *p++ |= JOP_COLOR << 8;
	       }
	     else if ((ch == '*') && (p + 1 < pmax) && (*(p + 1) == '/'))
	       {
		  /* missed the comment.  This means it was started on a 
		   * previous line and the person did not follow the 
		   * rules like this comment does.
		   */
		  
		  p += 2;
		  while (pmin < p) 
		    {
		       *pmin &= 0xFF;
		       *pmin |= JCOM_COLOR << 8;
		       pmin++;
		    }
	       }
	     else if ((ch == '%') && (CBuf->modes & SL_MODE))
	       p = C_highlight_comment (p, pmax);
	     else *p++ |= color << 8; 
	  }
	else p++;
     }
}
	       
	
