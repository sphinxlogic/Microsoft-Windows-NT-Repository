/* Basic string functions for S-Lang */
/* 
 * Copyright (c) 1992, 1994 John E. Davis 
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

#ifdef FLOAT_TYPE
#include <math.h>
#endif

#include "config.h"
#include "slang.h"
#include "_slang.h"
#include "slarray.h"


/* Standard intrinsic functions for S-Lang.  Included here are string
   and array operations */
      
/* builtin stack manipulation functions */

void SLdo_pop(void)
{
   SLang_Object_Type x;
   if (SLang_pop(&x)) return;

   if (x.main_type == SLANG_DATA)
     {
	if (x.sub_type == STRING_TYPE) SLFREE(x.v.s_val);
	if (x.sub_type >= ARRAY_TYPE) 
	  {
	     SLang_free_user_object (x.v.uobj);
	  }
     }
}

int SLdo_dup(void)
{
   SLang_Object_Type x;
   if (SLang_pop(&x)) return(0);
   SLang_push(&x);
   if (x.sub_type == STRING_TYPE) SLang_push_string(x.v.s_val);
   else
     {
	if (x.sub_type >= ARRAY_TYPE) x.v.uobj->count += 1;
	SLang_push (&x);
     }
   return(1);
}
   
static int generic_equals (void)
{
   SLang_Object_Type obj1, obj2;
   int ret = 0;

   /* SLang_pop guarantees that if there is a stack underflow, the type will
    * come back 0. */
   SLang_pop_non_object (&obj1);
   SLang_pop_non_object (&obj2);
   if (obj1.sub_type == obj2.sub_type)
     switch (obj1.sub_type)
     {
      case 0:			       /* stack underflow */
	break;
      case INT_TYPE: ret = obj1.v.i_val == obj2.v.i_val; break;
#ifdef FLOAT_TYPE
      case FLOAT_TYPE: ret = obj1.v.f_val == obj2.v.f_val; break;
#endif
      case STRING_TYPE:
	ret = !strcmp (obj1.v.s_val, obj2.v.s_val);
	break;
      default:
	ret = obj1.v.l_val == obj2.v.l_val;
     }
   if ((obj1.sub_type == STRING_TYPE) && (obj1.main_type == SLANG_DATA))
     SLFREE (obj1.v.s_val);
   if ((obj2.sub_type == STRING_TYPE) && (obj2.main_type == SLANG_DATA))
     SLFREE (obj2.v.s_val);
   return ret;
}

   
void SLdo_strcat(void)
{
   char *a, *b, *c;
   int len, lena;
   int adata, bdata;

   if (SLang_pop_string(&b, &bdata) || SLang_pop_string(&a, &adata)) return;

   lena = strlen(a);
   len = lena + strlen(b) + 1;
   if (adata == 1)
     {
	if ((NULL != (c = (char *) SLREALLOC(a, len))))
	  {
	     strcpy (c + lena, b);
	     adata = 0;
	  }
	else
	  {
	     SLang_Error = SL_MALLOC_ERROR;
	     return;
	  }
     }
   else if (NULL != (c = (char *) SLMALLOC(len)))
     {
	strcpy(c, a);
	strcpy(c + lena, b);
     }
   else 
     {
	SLang_Error = SL_MALLOC_ERROR; /* SLang_doerror("Lang Malloc error."); */
	return;
     }
   
   /* instead of going throug push string, push it directly */
   SLang_push_malloced_string(c);
   if (adata == 1) SLFREE(a);
   if (bdata == 1) SLFREE(b);
}

void SLdo_strtrim(void)
{
   char *a, *beg, *end, *c, *neew, ch;
   int len;
   int adata;

   if (SLang_pop_string(&a, &adata)) return;

   len = strlen(a);
   beg = a;
   end = a + (len - 1);
   while (ch = *beg, (ch == ' ') || (ch == '\t') || (ch == '\n')) beg++;
   while (end >= beg)
     {
	ch = *end;
	if ((ch == ' ') || (ch == '\t') || (ch == '\n')) end--;
	else break;
     }
   end++;
   len = (int) (end - beg);
   /* instead of going throug push string, push it directly.  This is because
    * the string that is being copied might be a constant. */
   if (NULL != (c = (char *) SLMALLOC(len + 1)))
     {
	neew = c;
	while (beg < end) *c++ = *beg++;
	*c = 0;

	SLang_push_malloced_string(neew);
     }
   else SLang_Error = SL_MALLOC_ERROR; /* SLang_doerror("Lang Malloc error."); */
   
   if (adata == 1) SLFREE(a);
}

static char Utility_Char_Table [256];

static void set_utility_char_table (char *pos)
{
   register char *t = Utility_Char_Table, *tmax;
   register unsigned char ch;

   tmax = t + 256;
   while (t < tmax) *t++ = 0;
   
   t = Utility_Char_Table;
   while ((ch = (unsigned char) *pos++) != 0) t[ch] = 1;
}

/* This routine returns the string with text removed between single character
   comment delimeters from the set b and e. */

static void uncomment_string (char *str, char *b, char *e)
{
   unsigned char chb, che;
   unsigned char *s, *cbeg, *mark;
   
   if (strlen(b) != strlen(e))
     {
	SLang_doerror ("Comment delimeter length mismatch.");
	return;
     }
   
   set_utility_char_table (b);
   
   if (NULL == (str = (char *) SLmake_string(str))) return;
   
   s = (unsigned char *) str;
   
   while ((chb = *s++) != 0)
     {
	if (Utility_Char_Table [chb] == 0) continue;
	
	mark = s - 1;

	cbeg = (unsigned char *) b;
	while (*cbeg != chb) cbeg++;
	
	che = (unsigned char) *(e + (int) (cbeg - (unsigned char *) b));
	
	while (((chb = *s++) != 0) && (chb != che));
	  
	if (chb == 0)
	  {
	     /* end of string and end not found.  Just truncate it a return; */
	     *mark = 0;
	     break;
	  }
	
	strcpy ((char *) mark, (char *)s);
	s = mark;
     }
   SLang_push_malloced_string (str);
}

   
   
void SLquote_string (void)
{
   char *str, *quotes, *q;
   int sdata, qdata;
   int slash, n;
   register char *t, *s, *q1;
   register unsigned char ch;
   
   if ((SLang_pop_integer (&slash))    /* quote char */
       || (SLang_pop_string (&quotes, &qdata))) return;   /* chars to quote */
   if (SLang_pop_string (&str, &sdata))  /* string to quote */
     {
	if (qdata == 1) SLFREE (quotes);
	return;
     }
   
   /* setup the utility table to have 1s at quote char postitions. */
   set_utility_char_table (quotes);
   
   t = Utility_Char_Table;
   t[(unsigned int) slash] = 1;
   
   /* calculate length */
   s = str;
   n = 0;
   while ((ch = (unsigned char) *s++) != 0) if (t[ch]) n++;
   n += (int) (s - str);
   
   if (NULL != (q = (char *) SLMALLOC(n)))
     {
	s = str; q1 = q;
	while ((ch = (unsigned char) *s++) != 0)
	  {
	     if (t[ch]) *q1++ = slash;
	     *q1++ = (char) ch;
	  }
	*q1 = 0;
	SLang_push_malloced_string(q);
     }
   else SLang_Error = SL_MALLOC_ERROR;
   
   if (qdata == 1) SLFREE (quotes);
   if (sdata == 1) SLFREE (str);
}


/* returns the position of substrin in a string or null */
void SLdo_issubstr(void)
{
   char *a, *b, *c;
   int adata, bdata, n;

   if (SLang_pop_string(&b, &bdata) || SLang_pop_string(&a, &adata)) return;

   if (NULL == (c = (char *) strstr(a, b))) n = 0; else n = 1 + (int) (c - a);

   if (adata == 1) SLFREE(a);
   if (bdata == 1) SLFREE(b);
   SLang_push_integer (n);
}

/* returns to stack string at pos n to n + m of a */
void SLdo_substr(void)
{
   char *a;
   int adata, n, m;
   char b[256];

   if (SLang_pop_integer(&m) || SLang_pop_integer(&n) || (SLang_pop_string(&a, &adata))) return;

   *b = 0;
   if (m > 0)
     {
	strncpy(b, a + (n - 1), 254);
	if (m > 255) m = 255;
	b[m] = 0;
     }
   if (adata == 1) SLFREE(a);
   SLang_push_string(b);
}
/* substitute char m at positin string n in string*/
void SLdo_strsub(void)
{
   char *a;
   int adata, n, m;
   char b[256];

   if (SLang_pop_integer(&m) || SLang_pop_integer(&n) || (SLang_pop_string(&a, &adata))) return;

   strncpy(b, a, 254);
   b[254] = 0;
   if (adata == 1) SLFREE(a);
   if ((n < 1) || (n > 254)) n = 254;
   b[n-1] = (char) m;
   SLang_push_string(b);
}

void SLdo_strup(void)
{
   unsigned char c, *a;
   int adata;

   if (SLang_pop_string((char **) &a, &adata)) return;
   SLang_push_string((char *) a);
   if (adata == 1) SLFREE(a);
   
   a = (unsigned char *) (SLStack_Pointer - 1)->v.s_val;
   while ((c = *a) != 0)
     {
	/* if ((*a >= 'a') && (*a <= 'z')) *a -= 32; */
	*a = UPPER_CASE(c);
	a++;
     }
}

void SLdo_strlow(void)
{
   unsigned char c, *a;
   int adata;

   if (SLang_pop_string((char **) &a, &adata)) return;
   SLang_push_string((char *) a);
   if (adata == 1) SLFREE(a);
   a = (unsigned char *) (SLStack_Pointer - 1)->v.s_val;
   
   while ((c = *a) != 0)
     {
	/* if ((*a >= 'a') && (*a <= 'z')) *a -= 32; */
	*a = LOWER_CASE(c);
	a++;
     }
}

void SLdo_strcmp(void)
{
   char *a, *b;
   int adata, bdata, i;

   if (SLang_pop_string(&b, &bdata) || SLang_pop_string(&a, &adata)) return;

   i = strcmp(a, b);

   if (adata == 1) SLFREE(a);
   if (bdata == 1) SLFREE(b);
   SLang_push_integer (i);
}

void SLdo_strncmp(void)
{
   char *a, *b;
   int adata, bdata, i, n;
   
   if (SLang_pop_integer(&n) || SLang_pop_string(&b, &bdata) || SLang_pop_string(&a, &adata)) return;

   i = strncmp(a, b, n);

   if (adata == 1) SLFREE(a);
   if (bdata == 1) SLFREE(b);
   SLang_push_integer (i);
}

void SLdo_strlen(void)
{
   char *a;
   int adata, i;

   if (SLang_pop_string(&a, &adata)) return;

   i = strlen(a);

   if (adata == 1) SLFREE(a);
   SLang_push_integer (i);
}

int SLdo_isdigit(char *what)
{
   if ((*what >= '0') && (*what <= '9')) return(1); else return(0);
}

/* convert object to integer form */
void SLdo_int(void)
{
   SLang_Object_Type x;
   int i;
   unsigned char stype;

   if (SLang_pop_non_object (&x)) return;
   stype = x.sub_type;

   if (stype == INT_TYPE)
     {
	SLang_push(&x);
	return;
     }

   else if (stype == STRING_TYPE)
     {
	stype = x.main_type;
	i = (int) *(unsigned char *) x.v.s_val;
	
	if (stype == SLANG_DATA) SLFREE(x.v.s_val);
     }
#ifdef FLOAT_TYPE
   else if (stype == FLOAT_TYPE)
     {
	i = (int) x.v.f_val;
     }
#endif
   else
     {
	SLang_Error = TYPE_MISMATCH;
	return;
     }
   SLang_push_integer(i);
}

static char Float_Format[16] = "%f";

static void set_float_format (char *s)
{
   strncpy (Float_Format, s, 15);
   Float_Format[15] = 0;
}


/* Conver string to integer */
void SLdo_integer(void)
{
   char *a;
   int adata, i;

   if (SLang_pop_string(&a, &adata)) return;

   /* Should check for parse error here but later */
   i = SLatoi((unsigned char *) a);

   if (adata == 1) SLFREE(a);
   SLang_push_integer (i);
}

/* convert integer to a sring of length 1 */
void SLdo_char(void)
{
   char ch, buf[2];
   int x;

   if (SLang_pop_integer(&x)) return;

   ch = (char) x;
   buf[0] = ch;
   buf[1] = 0;
   SLang_push_string((char *) buf);
}

/* format object into a string */
char *SLstringize_object (SLang_Object_Type *obj)
{
   unsigned char stype;
   char *s;
   char buf [256];
   SLang_Class_Type *cl;
   
   s = buf;
   stype = obj->sub_type;
   switch (stype)
     {
      case STRING_TYPE:
	s = obj->v.s_val;
	break;
	
      case INT_TYPE:
	sprintf(buf, "%d", obj->v.i_val);
	break;

#ifdef FLOAT_TYPE
      case FLOAT_TYPE:
	sprintf(buf, Float_Format,  obj->v.f_val);
	break;
#endif
      default:
	cl = SLang_Registered_Types[stype];
	if ((cl != NULL) && (cl->string != NULL)
	    && (NULL != (s = cl->string ((VOID *) obj->v.uobj))))
	  {
	     return s;
	  }
	buf[0] = '?'; buf[1] = 0;
     }
   return SLmake_string (s);
}

void SLdo_string(void)
{
   SLang_Object_Type x;
   
   if (SLang_pop (&x)) return;
   SLang_push_malloced_string (SLstringize_object (&x));
   
   if (x.main_type == SLANG_DATA)
     {
	if (x.sub_type == STRING_TYPE) SLFREE(x.v.s_val);
	else if (x.sub_type >= ARRAY_TYPE)
	  SLang_free_user_object (x.v.uobj);
     }
}

/* probably more useful to have a argc, argv thing */
int SLang_run_hooks(char *hook, char *opt1, char *opt2)
{
   int ret = 0;

   if (SLang_Error || !SLang_is_defined(hook)) return(0);
   if (opt1 != NULL) SLang_push_string(opt1);
   if (opt2 != NULL) SLang_push_string(opt2);
   if (!SLang_Error) ret = SLang_execute_function(hook);
   return (ret && !SLang_Error);
}

static void lang_getenv_cmd(char *s)
{
   char *t;
   if (NULL == (t = getenv(s))) t = "";
   SLang_push_string(t);
}

#ifndef VMS
static void lang_putenv_cmd(void)
{
   char *s;
   int d;
   if (SLang_pop_string(&s, &d)) return;

   if (putenv (s))
     {
	SLang_Error = INTRINSIC_ERROR;
	if (d == 1) SLFREE (s);
     }
   
   /* Note that s is NOT freed */
}
#endif
int lang_apropos1(char *s, SLang_Name_Type *table, int max)
{
   int all = 0, n = 0;
   char *nm;
   SLang_Object_Type obj;

   if (*s == 0) all = 1;

   while(max && (nm = table->name, *nm != 0))
     {
	nm++;  /* lose hash */
	if ((*nm != 1) && (all || (NULL != strstr(nm, s))))
	  {
	     n++;
	     /* since string is static, make it literal */
	     obj.main_type = SLANG_LITERAL; obj.sub_type = STRING_TYPE;
	     obj.v.s_val = nm;
	     SLang_push(&obj);
	     if (SLang_Error) return(1);
	  }
	table++;
	max--;
     }
   return n;
}

void lang_apropos(char *s)
{
   int n;
   SLName_Table *nt;
   
   n = lang_apropos1(s, SLang_Name_Table, SLANG_MAX_SYMBOLS);
   nt = SLName_Table_Root;
   while (nt != NULL)
     {
	n += lang_apropos1(s, nt->table, nt->n);
	nt = nt->next;
     }
   SLang_push_integer(n);
}

static void lang_print_stack (void)
{
   SLang_Object_Type *x = SLStack_Pointer;
   int n;
   char *b, *t;
   char buf[132];
   char buf2[40];
   
   while (--x >= SLRun_Stack)
     {
	b = buf;
	n = (int) (x - SLRun_Stack);
	switch (x->sub_type)
	  {
	   case STRING_TYPE: 
	     b = x->v.s_val;
	     t = "(Str)"; break;
	   case INT_TYPE: sprintf(buf, "%d", x->v.i_val); t = "(Int)"; break;
#ifdef FLOAT_TYPE
	   case FLOAT_TYPE: 
	     sprintf(buf, Float_Format, x->v.f_val); t = "(float)"; break;
#endif
	   case SLANG_OBJ_TYPE: 
	     b = (char *) (x->v.n_val)->name + 1;
	     t = "(Ptr)";
	     break;
	   case ARRAY_TYPE:
	     *buf = 0;		       /* I could give some info here */
	     t = "(Array)";
	     break;
	   default: t = "(Unknown)"; *buf = 0;
	  }
	sprintf(buf2, "(%d) %s:", n, t);
	
	(*SLang_Dump_Routine)(buf2);
	(*SLang_Dump_Routine)(b);
	*buf = '\n'; *(buf + 1) = 0;
	(*SLang_Dump_Routine)(buf);
     }
}


/* sprintf functionality for S-Lang */

static char *SLdo_sprintf(char *fmt)
{
   register char *p = fmt, ch;
   char *out = NULL, *outp = NULL;
   char dfmt[80];		       /* used to hold part of format */
   char *f;
   unsigned char stmp, ttmp;
   long *varp;
   int var, want_width, width, precis, use_varp;
   int len = 0, malloc_len = 0, dlen, do_free, guess_size;
#ifdef FLOAT_TYPE
   int tmp1, tmp2, use_float;
   FLOAT x;
#endif
   
   
   while (1)
     {
	while (ch = *p, ch && (ch != '%')) p++;
		  
	/* p points at '%' */
	
	dlen = (int) (p - fmt);
	
	if (len + dlen >= malloc_len)
	  {
	     malloc_len = len + dlen;
	     if (out == NULL) outp = (char *) SLMALLOC(malloc_len + 1);
	     else outp = (char *) SLREALLOC(out, malloc_len + 1);
	     if (NULL == outp)
	       {
		  SLang_Error = SL_MALLOC_ERROR;
		  return out;
	       }
	     out = outp;
	     outp = out + len;
	  }
	
	strncpy(outp, fmt, dlen);
	len += dlen;
	outp = out + len;
	*outp = 0;
	if (ch == 0) break;

	/* bump it beyond '%' */
	++p;
	fmt = p;

	f = dfmt;
	*f++ = ch;
	/* handle flag char */
	ch = *p++;
	if ((ch == '-') || (ch == '+') || (ch == ' ') || (ch == '#'))
	  {
	     *f++ = ch;
	     ch = *p++;
	  }
	
	/* width */
	/* I have got to parse it myself so that I can see how big it needs
	   to be. */
	want_width = width = 0;
	if (ch == '*')
	  {
	     if (SLang_pop_integer(&width)) return (out);
	     want_width = 1;
	     ch = *p++;
	  }
	else 
	  {
	     if (ch == '0') 
	       {
		  *f++ = '0';
		  ch = *p++;
	       }
	     
	     while ((ch <= '9') && (ch >= '0'))
	       {
		  width = width * 10 + (ch - '0');
		  ch = *p++;
		  want_width = 1;
	       }
	  }
	
	if (want_width)
	  {
	     sprintf(f, "%d", width);
	     while (*f) f++;
	  }
	precis = 0;
	/* precision -- also indicates max number of chars from string */
	if (ch == '.')
	  {
	     *f++ = ch;
	     ch = *p++;
	     want_width = 0;
	     if (ch == '*')
	       {
		  if (SLang_pop_integer(&precis)) return (out);
		  ch = *p++;
		  want_width = 1;
	       }
	     else while ((ch <= '9') && (ch >= '0'))
	       {
		  precis = precis * 10 + (ch - '0');
		  ch = *p++;
		  want_width = 1;
	       }
	     if (want_width)
	       {
		  sprintf(f, "%d", precis);
		  while (*f) f++;
	       }
	     else precis = 0;
	  }
	
	/* not supported */
	if ((ch == 'l') || (ch == 'h')) ch = *p++;
	
	var = 0;
	varp = 0;
	guess_size = 32;
#ifdef FLOAT_TYPE
	use_float = 0;
#endif
	use_varp = 0;
	do_free = 0;
	
	/* Now the actual format specifier */
	switch(ch)
	  {
	     case 's': 
	     if (SLang_pop_string((char **) &varp, &do_free)) return (out);
	     guess_size = strlen((char *) varp);
	     use_varp = 1;
	     break;
	     
	   case '%': 
	     guess_size = 1;
	     do_free = 0;
	     use_varp = 1;
	     varp = (long *) "%";
	     break;
	     
	     case 'c': guess_size = 1;
	     /* drop */
	     case 'd':
	     case 'i': 
	     case 'o': 
	     case 'u': 
	     case 'X': 
	     case 'x':
	     if (SLang_pop_integer(&var)) return(out);
	     break;
	     
	     case 'f': 
	     case 'e': 
	     case 'g': 
	     case 'E': 
	     case 'G': 
#ifdef FLOAT_TYPE
	     if (SLang_pop_float(&x, &tmp1, &tmp2)) return (out);
	     use_float = 1;
	     guess_size = 64;
	     (void) tmp1; (void) tmp2;
	     break;
#endif
	     case 'p': 
	     guess_size = 32;
	     if (NULL == (varp = SLang_pop_pointer(&stmp, &ttmp, &do_free)))
	       {
		  return (out);
	       }
	     (void) stmp; (void) ttmp;
	     use_varp = 1;
	     break;
	     
	   default: 
	     SLang_doerror("Invalid Format.");
	     return(out);
	  }
	*f++ = ch; *f = 0;
	
	width = width + precis;
	if (width > guess_size) guess_size = width;
	
	if (len + guess_size > malloc_len)
	  {
	     outp = (char *) SLREALLOC(out, len + guess_size + 1);
	     if (outp == NULL) 
	       {
		  SLang_Error = SL_MALLOC_ERROR;
		  return (out);
	       }
	     out = outp;
	     outp = out + len;
	     malloc_len = len + guess_size;
	  }
	
	if (use_varp)
	  {
	     sprintf(outp, dfmt, varp);
	     if (do_free == 1) SLFREE(varp);
	  }
#ifdef FLOAT_TYPE
	else if (use_float) sprintf(outp, dfmt, x);
#endif
	else sprintf(outp, dfmt, var);
	
	len += strlen(outp);
	outp = out + len;
	fmt = p;
     }

   if (out != NULL)
     {
	outp = (char *) SLREALLOC(out, (int) (outp - out) + 1);
	if (outp != NULL) out = outp;
     }
   
   return (out);
}
   


static void SLsprintf(void)
{
   register char *p, ch, *b;
   char buf[256], ch1, *fmt;
   int n = 1, do_free;
   SLang_Object_Type *ptr;

   if (NULL == (ptr = SLreverse_stack(&n))) return;
   if (SLang_pop_string(&fmt, &do_free)) return;
   strncpy(buf, fmt, 255);
   if (do_free == 1) SLFREE(fmt);

   buf[255] = 0;
   p = b = buf;
   
   while ((ch = *p++) != 0)
     {
	if (ch == '\\')
	  {
	     p = SLexpand_escaped_char(p, &ch1);
	     if (SLang_Error) return;
	     ch = ch1;
	  }
	/* else if ((ch == '%') && (*p == '%')) p++; */
	*b++ = ch;
     }
   *b = 0;

   p = SLdo_sprintf(buf);
   
   while (SLStack_Pointer > ptr) SLdo_pop();
   
   if (SLang_Error)
     {
	if (p != NULL) SLFREE(p);
     }
   
   if (p != NULL) SLang_push_malloced_string (p);
}


/* converts string s to a form that can be used in an eval */

static void make_printable_string(char *s)
{
   int len;
   register char *s1 = s, ch, *ss1;
   char *ss;
   
   /* compute length */
   len = 3;
   while ((ch = *s1++) != 0)
     {
	if ((ch == '\n') || (ch == '\\') || (ch == '"')) len++;
	len++;
     }
   if (NULL == (ss = (char *) SLMALLOC(len))) 
     {
	SLang_Error = SL_MALLOC_ERROR;
	return;
     }
   s1 = s;
   ss1 = ss;
   *ss1++ = '"';
   while ((ch = *s1++) != 0)
     {
	if (ch == '\n')
	  {
	     ch = 'n';
	     *ss1++ = '\\';
	  }
	else if ((ch == '\\') || (ch == '"'))
	  {
	     *ss1++ = '\\';
	  }
	*ss1++ = ch;
     }
   *ss1++ = '"';
   *ss1 = 0;
   SLang_push_string(ss);
}

   
char *SLang_extract_list_element(char *list, int *nth, int *delim)
{
   int d = *delim, n = *nth;
   static char elem[256];
   char *el = elem;
   
   while (n-- > 0)
     {
	while (*list && (*list != (char) d)) list++;
	if (*list == 0) break;
	list++;
     }
   n = 255;
   while (n-- && *list && (*list != (char) d)) *el++ = *list++;
   *el = 0;
   return (elem);
}

int SLang_is_list_element(char *list, char *elem, int *delim)
{
   int d = *delim, n, len;
   char *l = list;
   
   len = strlen (elem);
   n = 1;
   while (1)
     {
	while (*l && (*l != (char) d)) l++;
	if ((list + len == l) && (!strncmp (elem, list, len))) return n;
	if (*l == 0) break;
	list = l = l + 1;
	n++;
     }
   return (0);
}

/* Regular expression routines for strings */
static SLRegexp_Type regexp_reg;

int string_match (char *str, char *pat, int *np)
{
   int n = *np, len;
   unsigned char rbuf[512], *match;

   regexp_reg.case_sensitive = 1;
   regexp_reg.buf = rbuf;
   regexp_reg.pat = (unsigned char *) pat;
   regexp_reg.buf_len = sizeof (rbuf);

   if (SLang_regexp_compile (&regexp_reg))
     {
	SLang_doerror ("Unable to compile pattern.");
	return 0;
     }
   
   n--;
   len = strlen(str);
   if ((n < 0) || (n >= len))
     {
	SLang_Error = SL_INVALID_PARM;
	return 0;
     }
   str += n;
   len -= n;
   
   if (NULL == (match = SLang_regexp_match((unsigned char *) str, len, &regexp_reg))) return 0;
   
   /* adjust offsets */
   regexp_reg.offset = n;
   
   return (1 + (int) ((char *) match - str));
}

int string_match_nth(int *np)
{
   int n = *np, beg;
   
   if ((n < 0) || (n > 9) || (regexp_reg.pat == NULL)
       || ((beg = regexp_reg.beg_matches[n]) == -1))
     {
	SLang_Error = SL_INVALID_PARM;
	return 0;
     }
   SLang_push_integer(beg + regexp_reg.offset);
   return regexp_reg.end_matches[n];
}

static int my_system (char *s)
{
   return system (s);
}


SLang_Name_Type SLang_Basic_Table[] =
{
   MAKE_INTRINSIC(".autoload",  SLang_autoload, VOID_TYPE, 2),
   /*Prototype: Void autoload(String fun, String file);
    * This function simply declares function 'fun' to the interpreter.  When
    * 'fun' is actually called, its actual function definition will be loaded
    * from 'file'.
    * Example:  autoload ("bessel_j0", "/usr/lib/slang/bessel.sl");
    * See Also: evalfile */
   MAKE_INTRINSIC(".pop",  SLdo_pop, VOID_TYPE, 0),
   /* Prototype: Void pop ();
    * 'pop' is used to remove the top object from the S-Lang stack.  It is 
    * typically used to ignore values from function that return a value. */
   
   MAKE_INTRINSIC(".strcmp",  SLdo_strcmp, VOID_TYPE, 0),
   /* Prototype: Integer strcmp (String a, String b);
    * 'strcmp' performs a case sensitive comparison between two strings.  It
    * returns 0 if the strings are identical, a negative number if 'a' is less 
    * than 'b' and a positive result if 'a' is greater than 'b' (in a
    * lexicographic sense).
    * See also: strup, strlow */
   MAKE_INTRINSIC(".strcat",  SLdo_strcat, VOID_TYPE, 0),
   /* Prototype: String strcat(String a, String b);
    * Conconcatenates 'a' and 'b' and returns the result.
    * See also: Sprintf */
   MAKE_INTRINSIC(".strlen",  SLdo_strlen, VOID_TYPE, 0),
   /* Prototype: Integer strlen (String a);
    * Returns the length of 'a'.
    */
   MAKE_INTRINSIC(".is_defined",  SLang_is_defined, INT_TYPE, 1),
   /* Prototype: Integer is_defined (String obj);
    * This function is used to determine whether or not 'obj' has been defined.
    * If 'obj' is not defined, it returns 0.  Otherwise, it returns a non-zero
    * value that defpends on the type of object 'obj' represents.  Specifically:
    * 
    *          +1 if arg is an intrinsic function 
    *          +2 if user defined function
    *          -1 if intrinsic variable
    *          -2 if user defined variable */
   
   MAKE_INTRINSIC(".string",  SLdo_string, VOID_TYPE, 0),
   /* Prototype: String string (obj);
    * Here 'obj' can be of any type.  The function 'string' will return a string
    * representation of 'obj'.
    * Example: string (12.34) returns "12.34"
    * See also: Sprintf
    */
   MAKE_INTRINSIC(".getenv",  lang_getenv_cmd, VOID_TYPE, 1),
   /* Prototype: String getenv(String var);
    * Returns value of an environment variable 'var' as a string.  The empty
    * "" is returned if the 'var' is not defined. 
    * See also: putenv */
#ifndef VMS
   MAKE_INTRINSIC(".putenv",  lang_putenv_cmd, VOID_TYPE, 0),
   /*Prototype: Void putenv(String s);
     This functions adds string 's' to the environment.  Typically, 's' should
     be a String of the form "name=value".  It signals an error upon failure.
     */
#endif
   MAKE_INTRINSIC(".evalfile",  SLang_load_file, INT_TYPE, 1),
   /* Prototype: Integer evalfile (String file);
    * Load 'file' as S-Lang code.  If loading is successful, a non-zero result 
    * will be returned.  If the file is not found, zero will be returned.
    * See also: eval, autoload */
   
   MAKE_INTRINSIC(".char",  SLdo_char, VOID_TYPE, 0),
   /* Prototype: String char (Integer c);
    * This function takes and integer and returns a string of length 1 whose 
    * first character has ascii value 'c'.
    */
   MAKE_INTRINSIC(".eval",  SLang_load_string, VOID_TYPE, 1),
   /* evaluate STRING as an S-Lang expression. */
   MAKE_INTRINSIC(".dup",  SLdo_dup, VOID_TYPE, 0),
   /* duplicate top object on the stack. */
   MAKE_INTRINSIC(".substr",  SLdo_substr, VOID_TYPE, 0),
   /* Syntax: "string" n len substr
       returns a substring with length len of string beginning at position n.
     */
   MAKE_INTRINSIC(".integer",  SLdo_integer, VOID_TYPE, 0),
   /* Convert from a string representation to integer.  For example,
      "1234" integer returns 1234 to stack. */
   MAKE_INTRINSIC(".is_substr",  SLdo_issubstr, VOID_TYPE, 0),
   /* Syntax: "a" "b" is_substr
      returns the position of "b" in "a".  If "b" does not occur in "a"
      it returns 0--- the first position is 1 */
   MAKE_INTRINSIC(".strsub",  SLdo_strsub, VOID_TYPE, 0),
   /* Syntax:  "string"  n ascii_value strsub
     This forces string to have a char who asciii value is ascii_val at
     the nth position.  The first character in the string is at position
     1. */

   MAKE_INTRINSIC(".extract_element", SLang_extract_list_element, STRING_TYPE, 3),
   /* Prototype: String extract_element (String list, Integer nth, Integer delim);
    * Returns 'nth' element in 'list' where 'delim' separates elements of the 
    * list. 'delim' is an Ascii value.  Elements are numbered from 0.
    *
    * For example:
    *   extract_element ("element 0, element 1, element 2", 1, ',');
    * returns the string " element 1", whereas 
    *   extract_element ("element 0, element 1, element 2", 2, ' ');
    * returns "0,".
    * See also: is_list_element.
    */
   MAKE_INTRINSIC(".is_list_element", SLang_is_list_element, INT_TYPE, 3),
   /* Prototype: Integer is_list_element (String list, String elem, Integer delim);
    * If 'elem' is an element of 'list' where 'list' is a 'delim' seperated 
    * list of strings, this function returns 1 plus the matching element 
    * number.  If 'elem' is not a member of the list, zero is returned.
    * Example:
    *   is_list_element ("element 0, element 1, element 2", "0,", ' ');
    * returns 2 since "0," is element number one of the list (numbered from
    * zero).
    * See also: extract_element.
    */
   MAKE_INTRINSIC(".case", generic_equals, INT_TYPE, 0),
   /*Prototype: Integer case(a, b);
    * This function is designed to make the switch statement look more like
    * the C one.  Basically, it does a generic compare operation.  
    * Both parameters 'a' and 'b' must be of the same type.  It returns zero
    * if their types differ or have different values.
    * In a switch statment, it may be used as:
    *   switch (token)
    *   { case "return": return_function ();}
    *   { case "break": break_function ();}
    * 
    * Unlike the C version, it one cannot have:
    * 
    *   switch (i)
    *   {case 10: case 20: do_ten_or_twenty (i);}
    * 
    * One must do:
    * 
    *   switch (i)
    *   {case 10 or case (i, 20) : do_ten_or_twenty (i);}
    */
    
   MAKE_INTRINSIC(".string_match", string_match, INT_TYPE, 3),
   /*Prototype Integer string_match(String str, String pat, Integer pos);
    * Returns 0 if 'str' does not match regular expression specified by
    * 'pat'. This function performs the match starting at position 'pos' in
    * 'str'.  The first character of 'str' corresponds to 'pos' equal to one.
    * This function returns the position of the start of the match.  To find
    * the exact substring actually matched, use 'string_match_nth'. 
    * See also: string_match_nth, strcmp, strncmp
    */
   MAKE_INTRINSIC(".string_match_nth", string_match_nth, INT_TYPE, 1),
   /*Prototype: Integer Integer string_match_nth(Integer nth);
    * This function returns 2 integers describing the result of the last
    * call to 'string_match'.  It returns both the offset into the string 
    * and the length of characters matches by the 'nth' submatch.  
    * By convention, 'nth' equal to zero means the entire match.  Otherwise,
    * 'nth' must be an integer, 1 to 9, and refers to the set of characters
    * matched by the 'nth' regular expression given by \(...\).
    * For example, consider:
    * 
    *    variable matched, pos, len;
    *    matched = string_match("hello world", "\\([a-z]+\\) \\([a-z]+\\)", 1);
    *    if (matched) {
    *        (pos, len) = string_match_nth(2);
    *    }
    *
    * This will set 'matched' to 1 since a match will be found at the first
    * position, 'pos' to 7 since 'w' is the 7th character of the string, and
    * len to 5 since "world" is 5 characters long. */
   MAKE_VARIABLE("._traceback", &SLang_Traceback, INT_TYPE, 0),
   /* If non-zero, dump S-Lang tracback on error. */
   
   MAKE_VARIABLE("._slangtrace", &SLang_Trace, INT_TYPE, 0),
   /*Prototype: Integer _slangtrace;
    * If non-zero, begin tracing when function declared by 
    * lang_trace_function is entered.  This does not trace intrinsic functions.
    */

   /* these are rarely ever referred to so make them last. */
   /* If non-zero, dump S-Lang tracback on error. */
   MAKE_INTRINSIC(".system",  my_system, INT_TYPE, 1),
   MAKE_INTRINSIC(".slapropos",  lang_apropos, VOID_TYPE, 1),
   MAKE_INTRINSIC(".slang_trace_function",  SLang_trace_fun, VOID_TYPE, 1),
   /* only argument is a string that specifies a function name that is 
      to be traced. See also the variable _slangtrace. */
   
   /* array ops: */
   MAKE_INTRINSIC(".create_array",  SLang_create_array, VOID_TYPE, 0),
   /* Prototype: create_array (Integer type, Integer i_1, i_2 ... i_dim, dim);
    * Creates an array of type 'type' with dimension 'dim'.
    * i_n is an integer which specifies the maximum size of array in 
    * direction n.   'type' is a control integer which specifies the type 
    * of the array.
    *  Types are:  's' : array of strings
    *              'f' : array of floats
    *              'i' : array of integers
    *              'c' : array of characters
    * At this point, dim cannot be larger than 3.
    * Also note that space is dynamically allocated for the array and that
    * copies of the array are NEVER put on the stack.  Rather, references to
    * the array are put on the stack.
    * Example:
    *    variable a = create_array ('f', 10, 1);
    * This creates a 1 dimensional array of 10 floats and assigns it to 'a'
    */
   MAKE_INTRINSIC(".aget",  SLarray_getelem, VOID_TYPE, 0),
   /* Syntax: i j ... k  ARRAY aget
      returns ARRAY[i][j]...[k] */
   MAKE_INTRINSIC(".aput",  SLarray_putelem, VOID_TYPE, 0),
   /* Syntax: x i j ... k  ARRAY put
      sets ARRAY[i][j]...[k] = x */

   MAKE_INTRINSIC(".strncmp",  SLdo_strncmp, VOID_TYPE, 0),
   /* like strcmp but takes an extra argument--- number of characters to
    compare.  Example, "apple"  "appliance"  3 strcmp --> 0 */
   MAKE_INTRINSIC(".strlow", SLdo_strlow, VOID_TYPE, 0),
   /* Takes a string off the stack a replaces it with all characters
      in lowercase. */
   MAKE_INTRINSIC(".strup", SLdo_strup, VOID_TYPE, 0),
   /* Takes a string off the stack a replaces it with all characters
      in uppercase. */
   MAKE_INTRINSIC(".isdigit",  SLdo_isdigit, INT_TYPE, 1),
   /* returns TRUE if CHAR (string of length 1) is a digit. */
   MAKE_INTRINSIC(".strtrim", SLdo_strtrim, VOID_TYPE, 0),
   /* Trims leading and trailing whitespace from a string.  WHitespace
      is defined to be spaces, tabs, and newline chars. */

   MAKE_INTRINSIC(".int",  SLdo_int, VOID_TYPE, 0),
   /* returns ascii value of the first character of a string. */
   MAKE_INTRINSIC(".array_sort", SLarray_sort, VOID_TYPE, 1),
   /* Requires an array on the stack as well as a function name to call 
    for the comparison.  The array to be placed on the stack is the
    array to be sorted.  The routine returns an integer index array which 
    indicates the result of the sort.  The first array is unchanged. */
   
   /* misc stuff */
   MAKE_INTRINSIC("._stkdepth", SLstack_depth, INT_TYPE, 0),
   /* returns number of items on stack */
   MAKE_INTRINSIC(".print_stack", lang_print_stack, VOID_TYPE, 0),
   /* dumps tha S-Lang stack */
   
   MAKE_INTRINSIC(".Sprintf", SLsprintf, VOID_TYPE, 0),
   /*Prototype:  String Sprintf(String format, ..., Integer n);
    *  Sprintf formats a string from 'n' objects according to 'format'.  
    *  Unlike its C counterpart, Sprintf requires the number of items to
    *  format.  For example.
    *  
    *	   Sprintf("%f is greater than %f but %s is better than %s\n",
    *		    PI, E, "Cake" "Pie", 4);
    *		    
    *  The final argument to Sprintf is the number of items to format; in
    *  this case, there are 4 items.
    */
   MAKE_INTRINSIC(".init_char_array", SLinit_char_array, VOID_TYPE, 0),
    /*Prototype: Void init_char_array(Array_Type a, String s);
     * a is an array of type 'c' (character array) and s is a string.
     */	      

   MAKE_INTRINSIC(".byte_compile_file", SLang_byte_compile_file, VOID_TYPE, 2),
   /*Prototype Void byte_compile_file (String file, Integer method);
    * byte compiles 'file' producing a new file with the same name except 
    * a 'c' is added to the output file name.  For example, 
    *	   byte_compile_file("site.sl");
    * produces a new file named 'site.slc'.  If 'method' is non-zero, the 
    * file is preprocessed only.  Note that the resulting bytecompiled file
    * must only be used by the executable that produced it.  Set 'method' to 
    * a non-zero value to use the byte compiled file with more than one 
    * executable.
    */
   MAKE_INTRINSIC(".make_printable_string", make_printable_string, VOID_TYPE, 1),
   /*Prototype: String make_printable_string(String str);
    * Takes input string 'str' and creates a new string that may be used by the
    * interpreter as an argument to the 'eval' function.  The resulting string is
    * identical to 'str' except that it is enclosed in double quotes and the
    * backslash, newline, and double quote characters are expanded. 
    * See also: eval
    */
   MAKE_INTRINSIC(".str_quote_string", SLquote_string, VOID_TYPE, 0),
   /*Prototype: String str_quote_string(String str, String qlis, Integer quote);
    * Return a string identical to 'str' except that all characters in the 
    * string 'qlis' are escaped with the 'quote' character including the quote
    * character itself.
    */
   MAKE_INTRINSIC(".str_uncomment_string", uncomment_string, VOID_TYPE, 3),
   /*Prototype: String str_uncomment_string(String s, String beg, String end);
    * 'beg' and 'end' are strings whose characters define a set of comment 
    * delimeters.  This function removes comments defined by the delimeter set
    * from the input string 's' and returns it.  For example,
    *
    *	str_uncommen_string ("Hello (testing) 'example' World", "'(", "')");
    *
    * returns the string: "Hello  World"; 
    * 
    * This routine does not handle multicharacter comment delimeters and it
    * assumes that comments are not nested.
    */
     
   MAKE_INTRINSIC(".define_case", SLang_define_case, VOID_TYPE, 2),
   /* Two parameters are integers in the range 0 to 255.  The first
      integer is the ascii value of the upprcase character and the 2nd
      integer is the value of its lowercase counterpart.  For example, to
      define X as the uppercase of Y, do:
        X Y define_case */
   
   MAKE_INTRINSIC("._clear_error", SLang_clear_error, VOID_TYPE, 0),
   /* May be used in error blocks to clear the error that triggered the
      error block.  Execution resumes following the statement
      triggering the block. */
	
   MAKE_VARIABLE("._slang_version", SLang_Version, STRING_TYPE, 1),
   MAKE_INTRINSIC(".set_float_format", set_float_format, VOID_TYPE, 1),
   /*Prototype: Void set_float_format (String fmt);
     This function is used to set the floating point format to be used
     when floating point numbers are printed.  The routines that use this
     are the traceback routines and the 'string' function. The default
     value is "%f".
    */
   MAKE_INTRINSIC(".copy_array", SLcopy_array, VOID_TYPE, 0),
   /*Prototype: Void copy_array(Array b, Array a);
    * Copies the contents of array 'a' to array 'b'.  Both arrays must be of
    * the same type and dimension.
    */
   
   SLANG_END_TABLE
};

#ifdef FLOAT_TYPE
/* doing it this way means that I can handle things like 2.0 + 3 and
 * 3 + 2.0 at the same time.
 */
static int float_bin_op (int op, unsigned char sta, unsigned char stb, 
			 FLOAT *ap, FLOAT *bp)
{
   FLOAT c, a, b;
   int ic;
   
   if (sta == FLOAT_TYPE) a = *ap;
   else a = (FLOAT) *(int *) ap;
   
   if (stb == FLOAT_TYPE) b = *bp;
   else b = (FLOAT) *(int *) bp;

   switch (op)
     {
      case SLANG_PLUS:
	c = a + b; goto push_float;
      case SLANG_MINUS: c = a - b; goto push_float;
      case SLANG_TIMES: c = a * b; goto push_float;
      case SLANG_DIVIDE:
	if (b == 0.0) 
	  {
	     SLang_Error = DIVIDE_ERROR;
	     return 1;
	  }
	c = a / b; goto push_float;
	
      case SLANG_EQ: ic = (a == b); goto push_int;
      case SLANG_NE: ic = (a != b); goto push_int;
      case SLANG_GT: ic = (a > b); goto push_int;
      case SLANG_GE: ic = (a >= b); goto push_int;
      case SLANG_LT: ic = (a < b); goto push_int;
      case SLANG_LE: ic = (a <= b); goto push_int;
      default:
	return 1;
     }
   
   push_float:
   SLang_push_float (c);
   return 1;
   
   push_int:
   SLang_push_integer (ic);
   return 1;
}	

#endif

static int int_bin_op (int op, unsigned char sta, unsigned char stb, 
		       int *ap, int *bp)
{
   int c, a = *ap, b = *bp;
   (void) sta;
   (void) stb;
   switch (op)
     {
      case SLANG_PLUS:
	c = a + b; break;
      case SLANG_MINUS: c = a - b; break;
      case SLANG_TIMES: c = a * b; break;
      case SLANG_DIVIDE:
	if (b == 0) 
	  {
	     SLang_Error = DIVIDE_ERROR;
	     return 1;
	  }
	c = a / b; break;

      case SLANG_EQ: c = (a == b); break;
      case SLANG_NE: c = (a != b); break;
      case SLANG_GT: c = (a > b); break;
      case SLANG_GE: c = (a >= b); break;
      case SLANG_LT: c = (a < b); break;
      case SLANG_LE: c = (a <= b); 
	break;
      default:
	return 0;
     }
   SLang_push_integer (c);
   return 1;
}	

static int unary_op_function (int op, unsigned char type, VOID *xp)
{
   int x;
#ifdef FLOAT_TYPE 
   FLOAT xf;
   
   if (type == FLOAT_TYPE) 
     {
	xf = *(FLOAT *) xp;
	switch (op)
	  {
	   case SLANG_CHS: xf = -xf;
	   case SLANG_SQR: xf = xf * xf; break;
	   case SLANG_MUL2: xf = xf * 2.0; break;
	   case SLANG_ABS: xf = (FLOAT) fabs((double) xf); break;
	   case SLANG_SIGN: 
	     if (xf > 0.0) x = 1;
	     else if (xf < 0.0) x = -1;
	     else x = 0;
	     SLang_push_integer (x);
	     return 1;
	     
	   default:
	     return 0;
	  }
	SLang_push_float (xf);
     }
   else
     {
#else
	(void) type;
#endif
	x = *(int *) xp;
	switch (op)
	  {
	   case SLANG_CHS: x = -x; break;
	   case SLANG_SQR: x = x * x; break;
	   case SLANG_MUL2: x = x * 2; break;
	   case SLANG_ABS: x = abs(x); break;
	   case SLANG_SIGN: 
	     if (x > 0) x = 1; else if (x < 0) x = -1;
	   default:
	     return 0;
	  }
	SLang_push_integer (x);
#ifdef FLOAT_TYPE
     }
#endif
   return 1;
}

static int register_types (void)
{
   if ((!SLang_register_class (ARRAY_TYPE, (VOID *) SLarray_free_array, NULL))
       || !SLang_register_class (INT_TYPE, NULL, NULL)
#ifdef FLOAT_TYPE
       || !SLang_register_class (FLOAT_TYPE, NULL, NULL)
#endif
       )
     return 0;
   

   if (!SLang_add_binary_op (INT_TYPE, INT_TYPE, (VOID *) int_bin_op)
       || !SLang_add_unary_op (INT_TYPE, (VOID *) unary_op_function)
#ifdef FLOAT_TYPE
       || !SLang_add_binary_op (FLOAT_TYPE, FLOAT_TYPE, (VOID *) float_bin_op)
       || !SLang_add_binary_op (FLOAT_TYPE, INT_TYPE, (VOID *) float_bin_op)
       || !SLang_add_unary_op (FLOAT_TYPE, (VOID *) unary_op_function)
#endif
       )
     return 0;
   
   return 1;
}


int init_SLang()
{
   char name[3];
   int i;
#ifdef pc_system
# ifdef __os2__ 
   char *s = "OS2";
# else
   char *s = "MSDOS";
# endif
#else
# ifdef VMS
   char *s = "VMS";
# else
   char *s = "UNIX";
# endif
#endif

   if (0 == register_types ()) return 0;

   if (!SLang_add_table(SLang_Basic_Table, "_Basic")) return(0);
   SLadd_variable(SLANG_SYSTEM_NAME);
   
   if (!SLdefine_for_ifdef (s)) return 0;
   
   /* give temp global variables $0 --> $9 */
   name[2] = 0; name[0] = '$';
   for (i = 0; i < 10; i++)
     {
	name[1] = (char) (i + '0');
	SLadd_variable(name);
     }
   
   SLstupid_hash();
   
   SLang_init_case_tables ();
   return (SLang_Error == 0);
}
