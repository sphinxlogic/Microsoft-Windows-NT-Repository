
/* Copyright (C) 1992 Free Software Foundation, Inc.
   
This file is part of GLD, the Gnu Linker.

GLD is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

GLD is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GLD; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* 
 * Contributed by Steve Chamberlain
 *                sac@cygnus.com
 */

#include "bfd.h"
#include "sysdep.h"
#include <ctype.h>
#include <stdio.h>
#include "ldlex.h"
#include "ld.h"
#include "ldexp.h"
#include "ldgram.h"
#include "ldmisc.h"

#if 0
static unsigned int have_pushback;
int thischar;
#define NPUSHBACK 10
extern char *ldfile_input_filename;
extern int donehash ;
int pushback[NPUSHBACK];
FILE *ldlex_input_stack;
unsigned int lineno;

char *buystring();

extern int ldgram_in_defsym;
extern int ldgram_had_equals;
extern int ldgram_in_script;
int debug;

extern char yytext[];
extern YYSTYPE yylval;
extern int yyleng;

typedef struct {
	char *name;	
int value;
} keyword_type;
#define RTOKEN(x)  {  yylval.token = x; return x; }
keyword_type keywords[] = 
{
"/", '/',
"MEMORY",MEMORY,
"ORIGIN",ORIGIN,
"BLOCK",BLOCK,
"LENGTH",LENGTH,
"ALIGN",ALIGN_K,
"ADDR",ADDR,
"ENTRY",ENTRY,
"NEXT",NEXT,
"sizeof_headers",SIZEOF_HEADERS,
"SIZEOF_HEADERS",SIZEOF_HEADERS,
"MAP",MAP,
"SIZEOF",SIZEOF,
"TARGET",TARGET_K,
"SEARCH_DIR",SEARCH_DIR,
"OUTPUT",OUTPUT,
"INPUT",INPUT,
"DEFINED",DEFINED,
"CREATE_OBJECT_SYMBOLS",CREATE_OBJECT_SYMBOLS,
"CONSTRUCTORS", CONSTRUCTORS,
"FORCE_COMMON_ALLOCATION",FORCE_COMMON_ALLOCATION,
"SECTIONS",SECTIONS,
"FILL",FILL,
"STARTUP",STARTUP,
"OUTPUT_FORMAT",OUTPUT_FORMAT,
"OUTPUT_ARCH", OUTPUT_ARCH,
"HLL",HLL,
"SYSLIB",SYSLIB,
"FLOAT",FLOAT,
"LONG", LONG,
"SHORT", SHORT,
"BYTE", BYTE,
"NOFLOAT",NOFLOAT,

"NOLOAD",NOLOAD,
"DSECT",DSECT,
"COPY",COPY,
"INFO",INFO,
"OVERLAY",OVERLAY,

"o",ORIGIN,
"org",ORIGIN,
"l", LENGTH,
"len", LENGTH,
0,0};

int
lex_input()
{
  if (have_pushback > 0) 
      {
	have_pushback --;
	return thischar = pushback[have_pushback];
      }
  if (ldlex_input_stack) {
    thischar = getc(ldlex_input_stack);

    if (thischar == EOF)  {
      fclose(ldlex_input_stack);
      ldlex_input_stack = (FILE *)NULL;
      ldfile_input_filename = (char *)NULL;
      /* First char after script eof is a @ so that we can tell the grammer
	 that we've left */
      thischar = '@';

    }
  }
  else if (command_line && *command_line)  {
    thischar = *(command_line++);
  }
  else {
 thischar = 0;
  }
  if(thischar == '\t') thischar = ' ';
  if (thischar == '\n') { lineno++; }
  return thischar ;
}

void
lex_unput(c)
int c;
{
  if (have_pushback > NPUSHBACK) {
    info("%F%P: too many pushbacks\n");
  }

  pushback[have_pushback] = c;
  have_pushback ++;
}


	int
yywrap()
	 { return 1; }
/*VARARGS*/

#if 0
void
allprint(x) 
int x;
{
fprintf(yyout,"%d",x);
}

void
sprint(x) 
char *x;
{
fprintf(yyout,"%s",x);
}
#endif
int  thischar;



void
parse_args(ac, av)
int ac;
char **av;
{
  char *p;
  int i;
  size_t size = 0;
  char *dst;
  debug = 1;
  for (i= 1; i < ac; i++) {
    size += strlen(av[i]) + 2; 
  }
  dst = p = (char *)ldmalloc(size + 2);
/* Put a space arount each option */


  for (i =1; i < ac; i++) {

    unsigned int s = strlen(av[i]);
  *dst++ = ' ';
    memcpy(dst, av[i], s);
    dst[s] = ' ';
    dst += s + 1;
  }
  *dst	= 0;
  parse_line(p,0);

  free(p);


}

static long 
number (default_if_zero, base)
     int default_if_zero;
     int base;
{
  unsigned  long l = 0;
  int ch = yytext[0];
  if (ch == 0) {
      base = default_if_zero;
    }
  while (1) {
      switch (ch) {
	case 'x':
	  base = 16;
	  break;
	case 'k':
	case 'K':
	  l =l * 1024;
	  break;
	case 'm':
	case 'M':
	  l =l * 1024 * 1024;
	  break;
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
	  l = l * base + ch - '0';
	  break;
	case 'a': case 'b': case 'c' : case 'd' : case 'e': case 'f':
	  l =l *base + ch - 'a' + 10;
	  break;
	case 'A': case 'B': case 'C' : case 'D' : case 'E': case 'F':
	  l =l *base + ch - 'A' + 10;
	  break;
	default:
	  lex_unput(ch);
	  yylval.integer = l;
	  return INT;
	}
      ch = lex_input();
    }
}

int inquote()
{
  

  yylval.name = buystring(yytext+1);
  yylval.name[yyleng-2] = 0; /* Fry final quote */
  return NAME;
}

void
incomment()
{
  while (1) 
  {
    int ch;
    ch = lex_input();
    while (ch != '*') 
    {
      ch = lex_input();
    }



    if (lex_input() == '/') {
	break;
      }
    lex_unput(yytext[yyleng-1]);
  }
}
#endif

void
parse_line(arg, script)
char *arg;
int script;
{
  lex_redirect(arg);
  if (script)
   ldlex_script();
  
  yyparse();
}

int debug;

void
parse_args(ac, av)
int ac;
char **av;
{
  char *p;
  int i;
  size_t size = 0;
  char *dst;
  debug = 1;
  for (i= 1; i < ac; i++) {
      size += strlen(av[i]) + 2; 
    }
  dst = p = (char *)ldmalloc(size + 2);
  /* Put a space arount each option */


  for (i =1; i < ac; i++) {

      unsigned int s = strlen(av[i]);
      *dst++ = ' ';
      memcpy(dst, av[i], s);
      dst[s] = ' ';
      dst += s + 1;
    }
  *dst	= 0;
  parse_line(p,0);

  free(p);


}


