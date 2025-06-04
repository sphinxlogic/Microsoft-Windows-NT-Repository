# include <stdio.h>
# include <string.h>
# include "strdup.h"
# define U(x) ((x)&0377)
# define NLSTATE yyprevious=YYNEWLINE
# define BEGIN yybgin = yysvec + 1 +
# define INITIAL 0
# define YYLERR yysvec
# define YYSTATE (yyestate-yysvec-1)
# define YYOPTIM 1
# define YYLMAX 5000
# define output(c) putc(c,yyout)
# define input() (((yytchar=yysptr>yysbuf?U(*--yysptr):getc(yyin))==10?(yylineno++,yytchar):yytchar)==EOF?0:yytchar)
# define unput(c) {yytchar= (c);if(yytchar=='\n')yylineno--;*yysptr++=yytchar;}
# define yymore() (yymorfg=1)
# define ECHO fprintf(yyout, "%s",yytext)
# define REJECT { nstr = yyreject(); goto yyfussy;}
int yyleng; 
///////NL modification ///////////////////
//extern unsigned char yytext[];        //
//////////////////////////////////////////
extern char yytext[];                   //
//////////////////////////////////////////

int yymorfg;
extern unsigned char *yysptr, yysbuf[];
int yytchar;
//////////////NL modification////////////////////////////
//FILE *yyin = {stdin}, *yyout = {stdout};             //
/////////////////////////////////////////////////////////
FILE *yyin = (FILE *)0, *yyout = (FILE *)0;            //
/////////////////////////////////////////////////////////
extern int yylineno;
struct yysvf { 
	struct yywork *yystoff;
	struct yysvf *yyother;
	int *yystops;};
struct yysvf *yyestate;
extern struct yysvf yysvec[], *yybgin;
/*
 * File:         lexer.l
 * Description:  Lexical analyser for PROLOGIO; can be used with
 *               either lex and flex.
 */
#include <string.h>

/* +++steve162e: added, otherwise, PROIO_input will be undefined (at least under LINUX)
             please check, if this is also TRUE under other UNIXes.
 */

#if defined(FLEX_SCANNER) && defined(_LINUX)
#define PROIO_input my_input
#endif
/* ---steve162e */

#include "expr.h"
#ifdef wx_x
extern char *malloc();
#endif
#define Return(x) return x;

static size_t lex_buffer_length = 0;
static const char *lex_buffer = NULL;
static size_t lex_string_ptr = 0;
static int lex_read_from_string = 0;

static int my_input(void);
static int my_unput(char);

#ifdef FLEX_SCANNER
#undef YY_INPUT
# define YY_INPUT(buf,result,max_size) \
   if (lex_read_from_string) \
   {  int c = my_input(); result = (c == 0) ? YY_NULL : ((buf)[0]=(c), 1); } \
   else \
	if ( (result = read( fileno(yyin), (char *) buf, max_size )) < 0 ) \
	    YY_FATAL_ERROR( "read() in flex scanner failed" );
#else
# undef unput
# define unput(_c) my_unput(_c)
#endif

# define YYNEWLINE 10
yylex(){
int nstr; extern int yyprevious;
while((nstr = yylook()) >= 0)
yyfussy: switch(nstr){
case 0:
if(yywrap()) return(0); break;
case 1:
		  {yylval.s = strdup(yytext); Return(INTEGER);}
break;
case 2:
                              Return(EXP);
break;
case 3:
	  {yylval.s = strdup(yytext); Return(WORD);}
break;
case 4:
          {int len = strlen(yytext);
                                   yytext[len-1] = 0;
                                   yylval.s = strdup(yytext+1); 
                                   Return(WORD);}
break;
case 5:
 {yylval.s = strdup(yytext); Return(STRING);}
break;
case 6:
			  Return(OPEN);
break;
case 7:
			  Return(CLOSE);
break;
case 8:
			  Return(COMMA);
break;
case 9:
                              Return(OPEN_SQUARE);
break;
case 10:
                              Return(CLOSE_SQUARE);
break;
case 11:
                              Return(EQUALS);
break;
case 12:
			  Return(PERIOD);
break;
case 13:
			  ;
break;
case 14:
			  ;
break;
case 15:
   {       loop:
#ifdef __cplusplus
                          while (yyinput() != '*');
                          switch (yyinput())
#else
                          while (input() != '*');
                          switch (input())
#endif
                                  {
                                  case '/': break;
                                  case '*': unput('*');
                                  default: goto loop;
                                  }
                          }
break;
case 16:
			  Return(ERROR);
break;
case -1:
break;
default:
fprintf(yyout,"bad switch yylook %d",nstr);
} return(0); }
/* end of yylex */


#ifdef FLEX_SCANNER
static int lex_input() {
  return input();
}
#else	/* BSD/AT&T lex */
#ifndef input
# error "Sorry, but need either flex or AT&T lex"
#endif
static int lex_input() {
  return input();
}
/* # undef unput
# define unput(_c) my_unput(_c)
*/

# undef input
# define input() my_input()
static int my_unput(char c)
{
  if (lex_read_from_string) {
    /* Make sure we have something */
    if (lex_string_ptr) {
      if (c == '\n') yylineno--;
      lex_string_ptr--;
    }
  } else {
    yytchar= (c);if(yytchar=='\n')yylineno--;*yysptr++=yytchar;
/*    unput(c); Causes infinite recursion! */
  }
  return c;
}

#endif

/* Public */ 
void LexFromFile(FILE *fd)
{
  lex_read_from_string = 0;
  yyin = fd;
  /* Don't know why this is necessary, but otherwise
   * lex only works _once_!
   */
#ifdef FLEX_SCANNER
  yy_init = 1;
#endif
}

void LexFromString(char *buffer)
{
  lex_read_from_string = 1;
  lex_buffer = buffer;
  lex_buffer_length = strlen(buffer);
  lex_string_ptr = 0;
  /* Don't know why this is necessary, but otherwise
   * lex only works _once_!
   */
#ifdef FLEX_SCANNER
  yy_init = 1;
#endif
}

static int my_input( void )
{
  if (lex_read_from_string) {
    if (lex_string_ptr == lex_buffer_length)
      return 0;
    else {
      char c = lex_buffer[lex_string_ptr++];
#ifndef FLEX_SCANNER
      if (c == '\n') yylineno++;
#endif
      return c;
    }
  } else {
    return lex_input();
  }
}

int yyvstop[] = {
0,

16,
0,

13,
16,
0,

14,
0,

16,
0,

16,
0,

6,
16,
0,

7,
16,
0,

16,
0,

8,
16,
0,

12,
16,
0,

16,
0,

1,
16,
0,

11,
16,
0,

3,
16,
0,

9,
16,
0,

10,
16,
0,

2,
3,
16,
0,

5,
0,

4,
0,

1,
0,

15,
0,

3,
0,

5,
0,
0};
# define YYTYPE unsigned char
struct yywork { YYTYPE verify, advance; } yycrank[] = {
0,0,	0,0,	1,3,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	1,4,	1,5,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	6,20,	0,0,	
0,0,	7,24,	0,0,	0,0,	
0,0,	0,0,	6,20,	6,20,	
0,0,	7,24,	7,24,	1,6,	
22,29,	0,0,	0,0,	0,0,	
1,7,	1,8,	1,9,	13,27,	
1,10,	1,11,	0,0,	1,12,	
1,13,	1,14,	2,8,	2,9,	
30,29,	0,0,	2,11,	6,21,	
2,12,	2,13,	7,24,	0,0,	
6,20,	0,0,	1,15,	7,25,	
6,20,	0,0,	1,16,	7,24,	
0,0,	6,20,	0,0,	2,15,	
7,24,	10,26,	10,26,	10,26,	
10,26,	10,26,	10,26,	10,26,	
10,26,	10,26,	10,26,	0,0,	
0,0,	0,0,	6,20,	0,0,	
0,0,	7,24,	0,0,	0,0,	
1,17,	1,3,	1,18,	23,30,	
24,0,	0,0,	0,0,	0,0,	
0,0,	2,17,	1,19,	2,18,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	2,19,	
0,0,	6,22,	0,0,	0,0,	
7,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	20,23,	22,23,	23,23,	
29,23,	16,28,	16,28,	16,28,	
16,28,	16,28,	16,28,	16,28,	
16,28,	16,28,	16,28,	0,0,	
0,0,	0,0,	30,23,	0,0,	
0,0,	6,23,	16,28,	16,28,	
16,28,	16,28,	16,28,	16,28,	
16,28,	16,28,	16,28,	16,28,	
16,28,	16,28,	16,28,	16,28,	
16,28,	16,28,	16,28,	16,28,	
16,28,	16,28,	16,28,	16,28,	
16,28,	16,28,	16,28,	16,28,	
0,0,	0,0,	0,0,	0,0,	
16,28,	0,0,	16,28,	16,28,	
16,28,	16,28,	16,28,	16,28,	
16,28,	16,28,	16,28,	16,28,	
16,28,	16,28,	16,28,	16,28,	
16,28,	16,28,	16,28,	16,28,	
16,28,	16,28,	16,28,	16,28,	
16,28,	16,28,	16,28,	16,28,	
0,0};
struct yysvf yysvec[] = {
0,	0,	0,
yycrank+-1,	0,		0,	
yycrank+-10,	yysvec+1,	0,	
yycrank+0,	0,		yyvstop+1,
yycrank+0,	0,		yyvstop+3,
yycrank+0,	0,		yyvstop+6,
yycrank+-21,	0,		yyvstop+8,
yycrank+-24,	0,		yyvstop+10,
yycrank+0,	0,		yyvstop+12,
yycrank+0,	0,		yyvstop+15,
yycrank+25,	0,		yyvstop+18,
yycrank+0,	0,		yyvstop+20,
yycrank+0,	0,		yyvstop+23,
yycrank+1,	0,		yyvstop+26,
yycrank+0,	yysvec+10,	yyvstop+28,
yycrank+0,	0,		yyvstop+31,
yycrank+81,	0,		yyvstop+34,
yycrank+0,	0,		yyvstop+37,
yycrank+0,	0,		yyvstop+40,
yycrank+0,	yysvec+16,	yyvstop+43,
yycrank+-1,	yysvec+6,	0,	
yycrank+0,	0,		yyvstop+47,
yycrank+-2,	yysvec+6,	0,	
yycrank+-3,	yysvec+6,	0,	
yycrank+-4,	yysvec+7,	0,	
yycrank+0,	0,		yyvstop+49,
yycrank+0,	yysvec+10,	yyvstop+51,
yycrank+0,	0,		yyvstop+53,
yycrank+0,	yysvec+16,	yyvstop+55,
yycrank+-4,	yysvec+6,	yyvstop+57,
yycrank+-18,	yysvec+6,	0,	
0,	0,	0};
struct yywork *yytop = yycrank+203;
struct yysvf *yybgin = yysvec+1;
unsigned char yymatch[] = {
00  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,011 ,012 ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
011 ,01  ,'"' ,01  ,01  ,01  ,01  ,047 ,
01  ,01  ,01  ,'+' ,01  ,'+' ,01  ,01  ,
'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,
'0' ,'0' ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,01  ,0134,01  ,01  ,'A' ,
01  ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
0};
unsigned char yyextra[] = {
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0};
/*
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0
*/
/*
#
# IBM CONFIDENTIAL
# Copyright International Business Machines Corp. 1989
# Unpublished Work
# All Rights Reserved
# Licensed Material - Property of IBM
#
#
# US Government Users Restricted Rights - Use, duplication or
# disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
# 
*/
/* @(#)ncform	1.3  com/lib/l,3.1,8951 9/7/89 18:48:47 */
int yylineno =1;
# define YYU(x) x
# define NLSTATE yyprevious=YYNEWLINE
//////////NL modification ///////////////
//unsigned char yytext[YYLMAX];        //
/////////////////////////////////////////
char yytext[YYLMAX];                   //
/////////////////////////////////////////
struct yysvf *yylstate [YYLMAX], **yylsp, **yyolsp;
unsigned char yysbuf[YYLMAX];
unsigned char *yysptr = yysbuf;
int *yyfnd;
extern struct yysvf *yyestate;
int yyprevious = YYNEWLINE;
yylook(){
	register struct yysvf *yystate, **lsp;
	register struct yywork *yyt;
	struct yysvf *yyz;
	int yych, yyfirst;
	struct yywork *yyr;
# ifdef LEXDEBUG
	int debug;
# endif
///////////NL modification ////////////
//	unsigned char *yylastch;     //
///////////////////////////////////////
	char *yylastch;              //
///////////////////////////////////////
	/* start off machines */
# ifdef LEXDEBUG
	debug = 0;
# endif
	yyfirst=1;
	if (!yymorfg)
		yylastch = yytext;
	else {
		yymorfg=0;
		yylastch = yytext+yyleng;
		}
	for(;;){
		lsp = yylstate;
		yyestate = yystate = yybgin;
		if (yyprevious==YYNEWLINE) yystate++;
		for (;;){
# ifdef LEXDEBUG
			if(debug)fprintf(yyout,"state %d\n",yystate-yysvec-1);
# endif
			yyt = yystate->yystoff;
			if(yyt == yycrank && !yyfirst){  /* may not be any transitions */
				yyz = yystate->yyother;
				if(yyz == 0)break;
				if(yyz->yystoff == yycrank)break;
				}
			*yylastch++ = yych = input();
			yyfirst=0;
		tryagain:
# ifdef LEXDEBUG
			if(debug){
				fprintf(yyout,"char ");
				allprint(yych);
				putchar('\n');
				}
# endif
			yyr = yyt;
			if ( yyt > yycrank){
				yyt = yyr + yych;
				if (yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					goto contin;
					}
				}
# ifdef YYOPTIM
			else if(yyt < yycrank) {		/* r < yycrank */
				yyt = yyr = yycrank+(yycrank-yyt);
# ifdef LEXDEBUG
				if(debug)fprintf(yyout,"compressed state\n");
# endif
				yyt = yyt + yych;
				if(yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					goto contin;
					}
				yyt = yyr + YYU(yymatch[yych]);
# ifdef LEXDEBUG
				if(debug){
					fprintf(yyout,"try fall back character ");
					allprint(YYU(yymatch[yych]));
					putchar('\n');
					}
# endif
				if(yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transition */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					goto contin;
					}
				}
			if ((yystate = yystate->yyother) && (yyt= yystate->yystoff) != yycrank){
# ifdef LEXDEBUG
				if(debug)fprintf(yyout,"fall back to state %d\n",yystate-yysvec-1);
# endif
				goto tryagain;
				}
# endif
			else
				{unput(*--yylastch);break;}
		contin:
# ifdef LEXDEBUG
			if(debug){
				fprintf(yyout,"state %d char ",yystate-yysvec-1);
				allprint(yych);
				putchar('\n');
				}
# endif
			;
			}
# ifdef LEXDEBUG
		if(debug){
			fprintf(yyout,"stopped at %d with ",*(lsp-1)-yysvec-1);
			allprint(yych);
			putchar('\n');
			}
# endif
		while (lsp-- > yylstate){
			*yylastch-- = 0;
			if (*lsp != 0 && (yyfnd= (*lsp)->yystops) && *yyfnd > 0){
				yyolsp = lsp;
				if(yyextra[*yyfnd]){		/* must backup */
					while(yyback((*lsp)->yystops,-*yyfnd) != 1 && lsp > yylstate){
						lsp--;
						unput(*yylastch--);
						}
					}
				yyprevious = YYU(*yylastch);
				yylsp = lsp;
				yyleng = yylastch-yytext+1;
				yytext[yyleng] = 0;
# ifdef LEXDEBUG
				if(debug){
					fprintf(yyout,"\nmatch ");
					sprint(yytext);
					fprintf(yyout," action %d\n",*yyfnd);
					}
# endif
				return(*yyfnd++);
				}
			unput(*yylastch);
			}
		if (yytext[0] == 0  /* && feof(yyin) */)
			{
			yysptr=yysbuf;
			return(0);
			}
		yyprevious = yytext[0] = input();
		if (yyprevious>0)
			output(yyprevious);
		yylastch=yytext;
# ifdef LEXDEBUG
		if(debug)putchar('\n');
# endif
		}
	}
yyback(p, m)
	int *p;
{
if (p==0) return(0);
while (*p)
	{
	if (*p++ == m)
		return(1);
	}
return(0);
}
	/* the following are only used in the lex library */
yyinput(){
	return(input());
	}
void yyoutput(c)
  int c; {
	output(c);
	}
yyunput(c)
   int c; {
	unput(c);
	}
