/* ed style regular expressions */
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
#include <string.h>

#include "slang.h"

#define SET_BIT(b, n) b[(n) >> 3] |= 1 << ((n) % 8)
#define TEST_BIT(b, n) (b[(n) >> 3] & (1 << ((n) % 8)))
#define LITERAL 1
#define RANGE 2
#define ANY 3
#define BOL 4
#define EOL 5
#define NTH_MATCH 6
#define OPAREN 7
#define CPAREN 8

#define STAR 0x80
#define LEAST_ONCE 0x40
#define MAYBE_ONCE 0x20
#define MANY 0x10
/* The rest are additions */
#define YES_CASE (STAR | BOL)
#define NO_CASE  (STAR | EOL)

#define UPPERCASE(x)  (cs ? (x) : UPPER_CASE(x))
#define LOWERCASE(x)  (cs ? (x) : LOWER_CASE(x))

static int next_pos;
static SLRegexp_Type *this_reg;
static unsigned char *this_str;

static unsigned char *do_nth_match(int n, unsigned char *str, unsigned char *estr)
{
   unsigned char *bpos;
   if (n > next_pos) return (NULL);
   
   bpos = this_reg->beg_matches[n] + this_str;
   n = this_reg->end_matches[n];
   if (n == 0) return(str);
   if (n > (int) (estr - str)) return (NULL);
   
   /* This needs fixed for case sensitive match */
   if (0 != strncmp((char *) str, (char *) bpos, n)) return (NULL);
   str += n;
   return (str);
}


/* returns pointer to the end of regexp or NULL */
static unsigned char *regexp_looking_at(register unsigned char *str, unsigned char *estr, unsigned char *buf, register int cs)
{
   register unsigned char p, p1;
   unsigned char *save_str, *tmpstr;
   int n, n0, n1;
   int save_next = 0;
   p = *buf++;
   
   
   while (p != 0)
     {
	/* p1 = UPPERCASE(*buf); */
	/* if (str < estr) c = UPPERCASE(*str); */

	switch((unsigned char) p)
	  {
	   case YES_CASE: cs = 1; break;
	   case NO_CASE: cs = 0; break;

	   case OPAREN:
	     this_reg->beg_matches[next_pos] = (int) (str - this_str);
	     this_reg->beg_matches[next_pos + 1] = -1;
	     break;
	   case CPAREN: 
	     this_reg->end_matches[next_pos] = (int) (str - this_str) - this_reg->beg_matches[next_pos];
	     next_pos++;
	     break;

	   case LITERAL: 
	     if ((str >= estr) || (*buf != UPPERCASE(*str))) return (NULL);
	     str++; buf++;
	     break;
	     
	   case MAYBE_ONCE | LITERAL:
	     save_str = str;
	     if ((str < estr) && (*buf == UPPERCASE(*str))) str++;
	     buf++;
	     goto match_rest;

	   case LITERAL | LEAST_ONCE:   /* match at least once */
	     if ((str >= estr) || (UPPERCASE(*str) != UPPERCASE(*buf))) return (NULL);
	     str++;
	     /* drop */
	   case STAR | LITERAL:
	     save_str = str;  p1 = *buf;
	     while ((str < estr) && (UPPERCASE(*str) == p1)) str++;
	     buf++;
	     goto match_rest;
	     
	     /* this type consists of the expression + two bytes that
	        determine number of matches to perform */
	   case MANY | LITERAL:
	     p1 = *buf; buf++;
	     n = n0 = (int) (unsigned char) *buf++;
	     /* minimum number to match--- could be 0 */
	     n1 = (int) (unsigned char) *buf++;
	     /* maximum number to match */

	     while (n && (str < estr) && (p1 == *str)) 
	       {
		  n--;
		  str++;
	       }
	     if (n) return (NULL);
	     n = n1 - n0;
	     while (n && (str < estr) && (p1 == *str)) 
	       {
		  n--;
		  str++;
	       }
	     break;
	     
	   case NTH_MATCH: 
	     if ((str = do_nth_match((int) (unsigned char) *buf, str, estr)) == NULL) return(NULL);
	     buf++;
	     break;
	     
	   case MAYBE_ONCE | NTH_MATCH:
	     save_str = str;
	     tmpstr = do_nth_match((int) (unsigned char) *buf, str, estr);
	     buf++;
	     if (tmpstr != NULL)
	       {
		  str = tmpstr;
		  goto match_rest;
	       }
	     continue;

	   case LEAST_ONCE | NTH_MATCH:
	     if ((str = do_nth_match((int) (unsigned char) *buf, str, estr)) == NULL) return(NULL);
	     /* drop */
	   case STAR | NTH_MATCH: 
	     save_str = str;
	     while (NULL != (tmpstr = do_nth_match((int) (unsigned char) *buf, str, estr)))
	       {
		  str = tmpstr;
	       }
	     buf++;
	     goto match_rest;
	     
	   case MANY | NTH_MATCH: return(NULL);
	     /* needs done */
	     
	   case RANGE: 
	     if (str >= estr) return (NULL);
	     if (TEST_BIT(buf, UPPERCASE(*str)) == 0) return (NULL);
	     buf += 32; str++;
	     break;
	     
	   case MAYBE_ONCE | RANGE:
	     save_str = str;
	     if ((str < estr) && TEST_BIT(buf, UPPERCASE(*str))) str++;
	     buf += 32;
	     goto match_rest;

	   case LEAST_ONCE | RANGE: 
	     if ((str >= estr) || (0 == TEST_BIT(buf, UPPERCASE(*str)))) return NULL;
	     str++;
	     /* drop */
	   case STAR | RANGE:
	     save_str = str;
	     while ((str < estr) && TEST_BIT(buf, UPPERCASE(*str))) str++;
	     buf += 32;
	     goto match_rest;
	     
	   case MANY | RANGE:  
	     return (NULL);
	     /* needs finished */

	   case MAYBE_ONCE | ANY:
	     save_str = str;
	     if ((str < estr) && (*str != '\n')) str++;
	     goto match_rest;

	   case LEAST_ONCE | ANY:
	     if ((str >= estr) || (*str == '\n')) return (NULL);
	     str++;
	     /* drop */
	   case STAR | ANY:
	     save_str = str;
	     while ((str < estr) && (*str != '\n')) str++;
	     goto match_rest;
	     
	   case ANY:
	     if ((str >= estr) || (*str == '\n')) return (NULL);
	     str++;
	     break;
	     
	   case MANY | ANY: 
	     return (NULL);
	     /* needs finished */

	   case EOL: 
	     if ((str >= estr) || (*str == '\n')) return (str);
	     return(NULL);

	   default: return (NULL);
	  }
	p = *buf++;
	continue;
	
	match_rest:
	if (save_str == str)
	  {
	     p = *buf++;
	     continue;
	  }
	
	/* if (p == EOL)
	  {
	     if (str < estr) return (NULL); else return (str);
	  } */
	
	save_next = next_pos;
	while (str >= save_str)
	  {
	     tmpstr = regexp_looking_at(str, estr, buf, cs);
	     if (tmpstr != NULL) return(tmpstr);
	     next_pos = save_next;
	     str--;
	  }
	return (NULL);
     }
   if ((p != 0) && (p != EOL)) return (NULL); else return (str);
   
}


unsigned char *SLang_regexp_match(unsigned char *str, int len, SLRegexp_Type *reg)
{
   register unsigned char c = 0, *estr = str + len;
   int cs = reg->case_sensitive, lit = 0;
   unsigned char *buf = reg->buf, *epos;

   next_pos = 1;
   this_reg = reg;
   this_str = str;

   if (*buf == BOL) 
     {
	if (NULL == (epos = regexp_looking_at(str, estr, buf + 1, cs))) str = NULL;
	reg->beg_matches[0] = (int) (str - this_str);
	reg->end_matches[0] = (int) (epos - str);
	return(str);
     }

   if ((unsigned char) *buf == NO_CASE)
     {
	buf++;  cs = 0;
     }
   if ((unsigned char) *buf == YES_CASE)
     {
	buf++;  cs = 1;
     }
   if (*buf == LITERAL) 
     {
	lit = 1;
	c = *(buf + 1);
     }
   else if ((*buf == OPAREN) && (*(buf + 1) == LITERAL))
     {
	lit = 1;
	c = *(buf + 2);
     }
   
   while (str < estr)
     {
	/* take care of leading chars */
	if (lit)
	  {
	     while ((str < estr) && (c != UPPERCASE(*str))) str++;
	     if (str >= estr) return(NULL);
	  }
	next_pos = 1;
	if (NULL != (epos = regexp_looking_at(str, estr, buf, cs)))
	  {
	     reg->beg_matches[0] = (int) (str - this_str);
	     reg->end_matches[0] = (int) (epos - str);
	     return (str);
	  }
	str++;
     }
   return (NULL);
}

static unsigned char *convert_digit(unsigned char *pat, int *nn)
{
   int n = 0, m = 0;
   unsigned char c;
   while (c = (unsigned char) *pat, (c <= '9') && (c >= '0'))
     {
	pat++;
	n = 10 * n + (c - '0');
	m++;
     }
   if (m == 0) 
     {
	return (NULL);
     }
   *nn = n;
   return pat;
}

#define ERROR  return (int) (pat - reg->pat)

/* Returns 0 if successful or offset in pattern of error */
int SLang_regexp_compile (SLRegexp_Type *reg)
{
   register unsigned char *buf, *ebuf, *pat;
   unsigned char *last = NULL, *tmppat;
   register unsigned char c;
   int i, reverse = 0, n, cs;
   int oparen = 0, dparen = 0, nparen = 0;
   /* substring stuff */
   int count, last_count, this_max_mm = 0, max_mm = 0, ordinary_search, 
     no_osearch = 0;
   unsigned char *mm_p = NULL, *this_mm_p = NULL;
   
   reg->beg_matches[0] = reg->end_matches[0] = 0;
   buf = reg->buf;
   ebuf = reg->buf + reg->buf_len - 2; /* make some room */
   pat = reg->pat;
   cs = reg->case_sensitive;

   i = 1; while (i < 10) 
     {
	reg->beg_matches[i] = reg->end_matches[i] = -1;
	i++;
     }

   if (*pat == '^')
     {
	pat++;
	*buf++ = BOL;
	reg->must_match_bol = 1;
     }
   else reg->must_match_bol = 0;

   *buf = 0;
   
   last_count = count = 0;
   while ((c = *pat++) != 0)
     {
	count++;
	switch (c)
	  {
	   case '$': 
	     if (*pat != 0) goto literal_char;
	     *buf++ = EOL;
	     break;
	     
	   case '\\': 
	     c = *pat++;
	     no_osearch = 1;
	     switch(c)
	       {
		case 'n': c = '\n'; goto literal_char;
		case 't': c = '\t'; goto literal_char;
		case 'C': cs = 0; *buf++ = NO_CASE; break;
		case 'c': cs = 1; *buf++ = YES_CASE; break;
		case '1': case '2': case '3':  case '4':  case '5':  
		case '6': case '7': case '8':  case '9': 
		  c = c - '0';
		  if (c > nparen / 2) ERROR;
		  last = buf;
		  *buf++ = NTH_MATCH; *buf++ = c;
		  break;

		case '{':
		  if (last == NULL) goto literal_char;
		  *last |= MANY;
		  tmppat = convert_digit(pat, &n);
		  if (tmppat == NULL) ERROR;
		  pat = tmppat;
		  *buf++ = n;
		  if (*pat == '\\') *buf++ = n;
		  else if (*pat == ',')
		    {
		       pat++;
		       if (*pat == '\\')
			 {
			    n = 255;
			 }
		       else 
			 {
			    tmppat = convert_digit(pat, &n);
			    if (tmppat == NULL) ERROR;
			    pat = tmppat;
			    if (*pat != '\\') ERROR;
			 }
		       *buf++ = n;
		    }
		  else ERROR;
		  last = NULL;
		  if (*++pat != '}') ERROR;
		  break;   /* case '{' */
		  
		case '(': 
		  oparen++;
		  if ((oparen > '9') || (dparen == 1)) ERROR;
		  dparen = 1;
		  nparen++;
		  *buf++ = OPAREN;
		  break;
		case ')':
		  if (dparen != 1) ERROR;
		  dparen = 0;
		  nparen++;
		  *buf++ = CPAREN;
		  break;

		case 0: ERROR;
		default:
		  goto literal_char;
	       }
	     break;

	   case '[':

	     *buf = RANGE;
	     last = buf++;
	     
	     if (buf + 32 >= ebuf) ERROR;

	     for (i = 0; i < 32; i++) buf[i] = 0;
	     c = *pat++;
	     if (c == '^') 
	       {
		  reverse = 1;
		  SET_BIT(buf, '\n');
		  c = *pat++;
	       }
	     
	     if (c == ']')
	       {
		  SET_BIT(buf, c);
		  c = *pat++;
	       }
	     while (c && (c != ']'))
	       {
		  if (c == '\\')
		    {
		       c = *pat++;
		       switch(c)
			 {
			    case 'n': c = '\n'; break;
			    case 't': c = '\t'; break;
			    case 0: ERROR;
			 }
		    }

		  if (*pat == '-')
		    {
		       pat++;
		       while (c < *pat)
			 {
			    if (cs == 0)
			      {
				 SET_BIT(buf, UPPERCASE(c));
				 SET_BIT(buf, LOWERCASE(c));
			      }
			    else SET_BIT(buf, c);
			    c++;
			 }
		    }
		  if (cs == 0)
		    {
		       SET_BIT(buf, UPPERCASE(c));
		       SET_BIT(buf, LOWERCASE(c));
		    }
		  else SET_BIT(buf, c);
		  c = *pat++;
	       }
	     if (c != ']') ERROR;
	     if (reverse) for (i = 0; i <= 32; i++) buf[i] = buf[i] ^ 0xFF;
	     reverse = 0;
	     buf += 32;
	     break;
	     
	   case '.':
	     last = buf;
	     *buf++ = ANY;
	     break;
	     
	   case '*': 
	     if (last == NULL) goto literal_char;
	     *last |= STAR;
	     last = NULL;
	     break;
	
	   case '+': 
	     if (last == NULL) goto literal_char;
	     *last |= LEAST_ONCE;
	     last = NULL;
	     break;

	   case '?':
	     if (last == NULL) goto literal_char;
	     *last |= MAYBE_ONCE;
	     last = NULL;
	     break;

	   literal_char:
	   default:
	     /* This is to keep track of longest substring */
	     this_max_mm++;
	     if (last_count + 1 == count)
	       {
		  if (this_max_mm == 1)
		    {
		       this_mm_p = buf;
		    }
		  else if (max_mm < this_max_mm)
		    {
		       mm_p = this_mm_p;
		       max_mm = this_max_mm;
		    }
	       }
	     else 
	       {
		  this_mm_p = buf;
		  this_max_mm = 1;
	       }
	     
	     last_count = count;
		       
	     last = buf;
	     *buf++ = LITERAL;
	     *buf++ = UPPERCASE(c);
	  }
     }
   *buf = 0;
   /* Check for ordinary search */
   ebuf = buf;
   buf = reg->buf;
   
   if (no_osearch) ordinary_search = 0;
   else
     {
	ordinary_search = 1;
	while (buf < ebuf)
	  {
	     if (*buf != LITERAL) 
	       {
		  ordinary_search = 0;
		  break;
	       }
	     buf += 2;
	  }
     }
   
   reg->osearch = ordinary_search;
   reg->must_match_str[15] = 0;
   if (ordinary_search)
     {
	strncpy((char *) reg->must_match_str, (char *) reg->pat, 15);
	reg->must_match = 1;
	return(0);
     }
   /* check for longest substring of pattern */
   reg->must_match = 0;
   if ((mm_p == NULL) && (this_mm_p != NULL)) mm_p = this_mm_p;
   if (mm_p == NULL)
     {
	return (0);
     }
   n = 15;
   pat = reg->must_match_str;
   buf = mm_p;
   while (n--)
     {
	if (*buf++ != LITERAL) break;
	*pat++ = *buf++;
     }
   *pat = 0;
   if (pat != reg->must_match_str) reg->must_match = 1;
   return(0);
}

/*
#define MAX_EXP 4096
int main(int argc, char **argv)
{
   FILE *fp;
   char *regexp, *file;
   char expbuf[MAX_EXP], buf[512];
   SLRegexp_Type reg;
   
   file = argv[2];
   regexp = argv[1];
   
   if (NULL == (fp = fopen(file, "r")))
     {
	fprintf(stderr, "File not open\n");
	return(1);
     }
   
   reg.buf = expbuf;
   reg.buf_len = MAX_EXP;
   reg.pat = regexp;
   reg.case_sensitive = 1;

   if (!regexp_compile(&reg)) while (NULL != fgets(buf, 511, fp))
     {
	if (reg.osearch)
	  {
	     if (NULL == strstr(buf, reg.pat)) continue;
	  }
	else 
	  {
	     if (reg.must_match && (NULL == strstr(buf, reg.must_match_str))) continue;
	     if (0 == regexp_match(buf, buf + strlen(buf), &reg)) continue;
	  }
	
	fputs(buf, stdout);
     }
   return (0);
}
*/
