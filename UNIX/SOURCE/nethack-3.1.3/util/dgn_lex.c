# include "stdio.h"
# define U(x) ((unsigned char)(x))
# define NLSTATE yyprevious=YYNEWLINE
# define BEGIN yybgin = yysvec + 1 +
# define INITIAL 0
# define YYLERR yysvec
# define YYSTATE (yyestate-yysvec-1)
# define YYOPTIM 1
# ifndef YYLMAX
# define YYLMAX 200
# endif
# define output(c) (void)putc(c,yyout)
#if defined(__cplusplus) || defined(__STDC__)
	int yyback(int *, int);
	int yyinput(void);
	int yylook(void);
	void yyoutput(int);
	int yyracc(int);
	int yyreject(void);
	void yyunput(int);

#ifndef __STDC__
#ifndef yyless
	void yyless(int);
#endif
#ifndef yywrap
	int yywrap(void);
#endif
#endif

#endif
# define input() (((yytchar=yysptr>yysbuf?U(*--yysptr):getc(yyin))==10?(yylineno++,yytchar):yytchar)==EOF?0:yytchar)
# define unput(c) {yytchar= (c);if(yytchar=='\n')yylineno--;*yysptr++=yytchar;}
# define yymore() (yymorfg=1)
# define ECHO (void)fprintf(yyout, "%s",yytext)
# define REJECT { nstr = yyreject(); goto yyfussy;}
int yyleng; extern char yytext[];
int yymorfg;
extern char *yysptr, yysbuf[];
int yytchar;
FILE *yyin = {stdin}, *yyout = {stdout};
extern int yylineno;
struct yysvf { 
	struct yywork *yystoff;
	struct yysvf *yyother;
	int *yystops;};
struct yysvf *yyestate;
extern struct yysvf yysvec[], *yybgin;

# line 3 "dgn_comp.l"
/*	SCCS Id: @(#)dgn_lex.c	3.1	93/06/17	*/

# line 4 "dgn_comp.l"
/*	Copyright (c) 1989 by Jean-Christophe Collet */

# line 5 "dgn_comp.l"
/*	Copyright (c) 1990 by M. Stephenson	     */

# line 6 "dgn_comp.l"
/* NetHack may be freely redistributed.  See license for details. */

#define DGN_COMP

#include "config.h"
#include "dgn_comp.h"
#include "dgn_file.h"

long *FDECL(alloc, (unsigned int));

# line 15 "dgn_comp.l"
/*
 * Most of these don't exist in flex, yywrap is macro and
 * yyunput is properly declared in flex.skel.
 */
#if !defined(FLEX_SCANNER) && !defined(FLEXHACK_SCANNER)
int FDECL(yyback, (int *, int));
int NDECL(yylook);
int NDECL(yyinput);
int NDECL(yywrap);
int NDECL(yylex);
	/* Traditional lexes let yyunput() and yyoutput() default to int;
	 * newer ones may declare them as void since they don't return
	 * values.  For even more fun, the lex supplied as part of the
	 * newer unbundled compiler for SunOS 4.x adds the void declarations
	 * (under __STDC__ or _cplusplus ifdefs -- otherwise they remain
	 * int) while the bundled lex and the one with the older unbundled
	 * compiler do not.  To detect this, we need help from outside --
	 * sys/unix/Makefile.utl.
	 */
# if defined(NeXT) || defined(SVR4)
#  define VOIDYYPUT
# endif
# if !defined(VOIDYYPUT)
#  if defined(POSIX_TYPES) && !defined(BOS) && !defined(HISX) && !defined(_M_UNIX)
#   define VOIDYYPUT
#  endif
# endif
# if !defined(VOIDYYPUT) && defined(WEIRD_LEX)
#  if defined(SUNOS4) && defined(__STDC__) && (WEIRD_LEX > 1) 
#   define VOIDYYPUT
#  endif
# endif
# ifdef VOIDYYPUT
void FDECL(yyunput, (int));
void FDECL(yyoutput, (int));
# else
int FDECL(yyunput, (int));
int FDECL(yyoutput, (int));
# endif
#endif	/* !FLEX_SCANNER && !FLEXHACK_SCANNER */

void FDECL(init_yyin, (FILE *));
void FDECL(init_yyout, (FILE *));

#ifdef MICRO
#undef exit
extern void FDECL(exit, (int));
#endif


# line 64 "dgn_comp.l"
/* this doesn't always get put in dgn_comp.h
 * (esp. when using older versions of bison)
 */

extern YYSTYPE yylval;

int line_number = 1;

# line 71 "dgn_comp.l"
/*
 *	This is a hack required by Michael Hamel to get things
 *	working on the Mac.
 */
#if defined(applec) && !defined(FLEX_SCANNER) && !defined(FLEXHACK_SCANNER)
#undef input
#undef unput
#define unput(c) { yytchar = (c); if (yytchar == 10) yylineno--; *yysptr++ = yytchar; }				  
# ifndef YYNEWLINE
# define YYNEWLINE 10
# endif

char
input()		/* Under MPW \n is chr(13)! Compensate for this. */
{
	if (yysptr > yysbuf) return(*--yysptr);
	else {
		yytchar = getc(yyin);
	 	if (yytchar == '\n') {
		    yylineno++;
		    return(YYNEWLINE);
		}
		if (yytchar == EOF) return(0);
		else		    return(yytchar);
	}
}
#endif	/* applec && !FLEX_SCANNER && !FLEXHACK_SCANNER */

# define YYNEWLINE 10
yylex(){
int nstr; extern int yyprevious;
while((nstr = yylook()) >= 0)
yyfussy: switch(nstr){
case 0:
if(yywrap()) return(0); break;
case 1:

# line 100 "dgn_comp.l"
	return(A_DUNGEON);
break;
case 2:

# line 101 "dgn_comp.l"
	{ yylval.i=1; return(UP_OR_DOWN); }
break;
case 3:

# line 102 "dgn_comp.l"
	{ yylval.i=0; return(UP_OR_DOWN); }
break;
case 4:

# line 103 "dgn_comp.l"
	return(ENTRY);
break;
case 5:

# line 104 "dgn_comp.l"
	return(STAIR);
break;
case 6:

# line 105 "dgn_comp.l"
	return(NO_UP);
break;
case 7:

# line 106 "dgn_comp.l"
	return(NO_DOWN);
break;
case 8:

# line 107 "dgn_comp.l"
	return(PORTAL);
break;
case 9:

# line 108 "dgn_comp.l"
return(PROTOFILE);
break;
case 10:

# line 109 "dgn_comp.l"
return(DESCRIPTION);
break;
case 11:

# line 110 "dgn_comp.l"
return(LEVELDESC);
break;
case 12:

# line 111 "dgn_comp.l"
      return(ALIGNMENT);
break;
case 13:

# line 112 "dgn_comp.l"
       return(LEVALIGN);
break;
case 14:

# line 113 "dgn_comp.l"
	{ yylval.i=TOWN ; return(DESCRIPTOR); }
break;
case 15:

# line 114 "dgn_comp.l"
	{ yylval.i=HELLISH ; return(DESCRIPTOR); }
break;
case 16:

# line 115 "dgn_comp.l"
{ yylval.i=MAZELIKE ; return(DESCRIPTOR); }
break;
case 17:

# line 116 "dgn_comp.l"
{ yylval.i=ROGUELIKE ; return(DESCRIPTOR); }
break;
case 18:

# line 117 "dgn_comp.l"
      { yylval.i=D_ALIGN_NONE ; return(DESCRIPTOR); }
break;
case 19:

# line 118 "dgn_comp.l"
        { yylval.i=D_ALIGN_NONE ; return(DESCRIPTOR); }
break;
case 20:

# line 119 "dgn_comp.l"
         { yylval.i=D_ALIGN_LAWFUL ; return(DESCRIPTOR); }
break;
case 21:

# line 120 "dgn_comp.l"
        { yylval.i=D_ALIGN_NEUTRAL ; return(DESCRIPTOR); }
break;
case 22:

# line 121 "dgn_comp.l"
        { yylval.i=D_ALIGN_CHAOTIC ; return(DESCRIPTOR); }
break;
case 23:

# line 122 "dgn_comp.l"
	return(BRANCH);
break;
case 24:

# line 123 "dgn_comp.l"
return(CHBRANCH);
break;
case 25:

# line 124 "dgn_comp.l"
	return(LEVEL);
break;
case 26:

# line 125 "dgn_comp.l"
return(RNDLEVEL);
break;
case 27:

# line 126 "dgn_comp.l"
return(CHLEVEL);
break;
case 28:

# line 127 "dgn_comp.l"
return(RNDCHLEVEL);
break;
case 29:

# line 128 "dgn_comp.l"
	{ yylval.i=atoi(yytext); return(INTEGER); }
break;
case 30:

# line 129 "dgn_comp.l"
{ yytext[yyleng-1] = 0; /* Discard the trailing \" */
		  yylval.str = (char *) alloc(strlen(yytext+1)+1);
		  strcpy(yylval.str, yytext+1); /* Discard the first \" */
		  return(STRING); }
break;
case 31:

# line 133 "dgn_comp.l"
	{ line_number++; }
break;
case 32:

# line 134 "dgn_comp.l"
	{ line_number++; }
break;
case 33:

# line 135 "dgn_comp.l"
	;
break;
case 34:

# line 136 "dgn_comp.l"
	{ return yytext[0]; }
break;
case -1:
break;
default:
(void)fprintf(yyout,"bad switch yylook %d",nstr);
} return(0); }
/* end of yylex */

/* routine to switch to another input file; needed for flex */
void init_yyin( input_f )
FILE *input_f;
{
#if defined(FLEX_SCANNER) || defined(FLEXHACK_SCANNER)
	if (yyin)
	    yyrestart(input_f);
	else
#endif
	    yyin = input_f;
}
/* analogous routine (for completeness) */
void init_yyout( output_f )
FILE *output_f;
{
	yyout = output_f;
}

int yyvstop[] = {
0,

34,
0,

33,
34,
0,

32,
0,

34,
0,

29,
34,
0,

34,
0,

34,
0,

34,
0,

34,
0,

34,
0,

34,
0,

34,
0,

34,
0,

34,
0,

34,
0,

34,
0,

34,
0,

34,
0,

34,
0,

34,
0,

34,
0,

34,
0,

34,
0,

34,
0,

34,
0,

33,
0,

30,
0,

29,
0,

2,
0,

31,
0,

3,
0,

14,
0,

4,
0,

25,
0,

6,
0,

5,
0,

23,
0,

20,
0,

8,
0,

1,
0,

22,
0,

15,
0,

21,
0,

7,
0,

19,
0,

13,
0,

26,
0,

16,
0,

12,
0,

11,
0,

9,
0,

17,
0,

18,
0,

27,
0,

28,
0,

24,
0,

10,
0,
0};
# define YYTYPE unsigned char
struct yywork { YYTYPE verify, advance; } yycrank[] = {
0,0,	0,0,	1,3,	0,0,	
0,0,	0,0,	6,29,	0,0,	
27,54,	0,0,	1,4,	1,5,	
0,0,	4,28,	6,29,	6,29,	
27,54,	27,55,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	1,6,	
4,28,	0,0,	0,0,	6,30,	
0,0,	27,54,	0,0,	0,0,	
0,0,	0,0,	1,7,	0,0,	
0,0,	0,0,	6,29,	0,0,	
27,54,	0,0,	0,0,	0,0,	
0,0,	2,27,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	1,8,	1,9,	
1,10,	1,11,	1,12,	13,38,	
33,57,	10,34,	32,56,	34,58,	
11,35,	1,13,	8,32,	12,37,	
15,40,	1,14,	36,60,	1,15,	
9,33,	14,39,	35,59,	2,8,	
2,9,	2,10,	2,11,	2,12,	
11,36,	37,61,	38,62,	39,63,	
40,64,	56,78,	2,13,	19,44,	
1,16,	1,17,	2,14,	18,43,	
2,15,	1,18,	16,41,	20,45,	
41,65,	1,19,	1,20,	1,21,	
17,42,	1,22,	21,46,	1,23,	
1,24,	1,25,	1,26,	22,48,	
23,49,	2,16,	2,17,	24,50,	
21,47,	25,51,	2,18,	26,52,	
42,66,	26,53,	2,19,	2,20,	
2,21,	43,67,	2,22,	44,68,	
2,23,	2,24,	2,25,	2,26,	
7,31,	45,69,	46,70,	7,31,	
7,31,	7,31,	7,31,	7,31,	
7,31,	7,31,	7,31,	7,31,	
7,31,	47,71,	48,73,	47,72,	
49,74,	50,75,	51,76,	52,77,	
57,79,	58,80,	59,81,	60,82,	
61,83,	62,84,	63,86,	64,87,	
65,89,	62,85,	66,90,	67,91,	
68,92,	69,93,	70,94,	71,95,	
64,88,	72,97,	73,98,	74,99,	
75,100,	76,101,	77,102,	78,103,	
79,104,	80,105,	81,106,	82,107,	
83,108,	84,109,	85,110,	86,111,	
71,96,	87,112,	88,113,	89,114,	
91,115,	92,116,	93,117,	94,118,	
95,119,	96,120,	97,121,	98,122,	
99,123,	100,124,	102,125,	103,126,	
104,127,	105,128,	106,130,	107,131,	
109,132,	110,133,	111,134,	112,135,	
113,136,	114,137,	115,138,	105,129,	
116,139,	117,140,	118,141,	119,142,	
121,143,	122,144,	123,145,	125,146,	
126,147,	128,148,	129,149,	130,150,	
131,151,	132,152,	133,153,	134,154,	
135,155,	136,156,	137,157,	138,158,	
140,159,	141,160,	142,161,	143,162,	
145,163,	146,164,	147,165,	148,166,	
149,167,	150,168,	152,169,	153,170,	
154,171,	155,172,	156,173,	159,174,	
163,175,	164,176,	165,177,	166,178,	
167,179,	168,180,	170,181,	171,182,	
172,183,	175,184,	176,185,	178,186,	
179,187,	180,188,	183,189,	186,190,	
188,191,	0,0,	0,0,	0,0,	
0,0};
struct yysvf yysvec[] = {
0,	0,	0,
yycrank+-1,	0,		0,	
yycrank+-22,	yysvec+1,	0,	
yycrank+0,	0,		yyvstop+1,
yycrank+4,	0,		yyvstop+3,
yycrank+0,	0,		yyvstop+6,
yycrank+-5,	0,		yyvstop+8,
yycrank+95,	0,		yyvstop+10,
yycrank+2,	0,		yyvstop+13,
yycrank+2,	0,		yyvstop+15,
yycrank+1,	0,		yyvstop+17,
yycrank+7,	0,		yyvstop+19,
yycrank+1,	0,		yyvstop+21,
yycrank+2,	0,		yyvstop+23,
yycrank+3,	0,		yyvstop+25,
yycrank+2,	0,		yyvstop+27,
yycrank+2,	0,		yyvstop+29,
yycrank+1,	0,		yyvstop+31,
yycrank+2,	0,		yyvstop+33,
yycrank+2,	0,		yyvstop+35,
yycrank+10,	0,		yyvstop+37,
yycrank+13,	0,		yyvstop+39,
yycrank+8,	0,		yyvstop+41,
yycrank+9,	0,		yyvstop+43,
yycrank+7,	0,		yyvstop+45,
yycrank+14,	0,		yyvstop+47,
yycrank+17,	0,		yyvstop+49,
yycrank+-7,	0,		yyvstop+51,
yycrank+0,	yysvec+4,	yyvstop+53,
yycrank+0,	yysvec+6,	0,	
yycrank+0,	0,		yyvstop+55,
yycrank+0,	yysvec+7,	yyvstop+57,
yycrank+1,	0,		0,	
yycrank+7,	0,		0,	
yycrank+10,	0,		0,	
yycrank+3,	0,		0,	
yycrank+4,	0,		0,	
yycrank+9,	0,		0,	
yycrank+8,	0,		0,	
yycrank+16,	0,		0,	
yycrank+28,	0,		0,	
yycrank+11,	0,		0,	
yycrank+9,	0,		0,	
yycrank+25,	0,		0,	
yycrank+16,	0,		0,	
yycrank+19,	0,		0,	
yycrank+25,	0,		0,	
yycrank+58,	0,		0,	
yycrank+40,	0,		0,	
yycrank+53,	0,		0,	
yycrank+60,	0,		0,	
yycrank+39,	0,		0,	
yycrank+62,	0,		0,	
yycrank+0,	0,		yyvstop+59,
yycrank+0,	yysvec+27,	0,	
yycrank+0,	0,		yyvstop+61,
yycrank+26,	0,		0,	
yycrank+82,	0,		0,	
yycrank+88,	0,		0,	
yycrank+95,	0,		0,	
yycrank+92,	0,		0,	
yycrank+82,	0,		0,	
yycrank+100,	0,		0,	
yycrank+82,	0,		0,	
yycrank+100,	0,		0,	
yycrank+57,	0,		0,	
yycrank+60,	0,		0,	
yycrank+63,	0,		0,	
yycrank+70,	0,		0,	
yycrank+72,	0,		0,	
yycrank+58,	0,		0,	
yycrank+75,	0,		0,	
yycrank+69,	0,		0,	
yycrank+62,	0,		0,	
yycrank+62,	0,		0,	
yycrank+75,	0,		0,	
yycrank+71,	0,		0,	
yycrank+74,	0,		0,	
yycrank+105,	0,		0,	
yycrank+117,	0,		0,	
yycrank+107,	0,		0,	
yycrank+104,	0,		0,	
yycrank+118,	0,		0,	
yycrank+99,	0,		0,	
yycrank+113,	0,		0,	
yycrank+114,	0,		0,	
yycrank+112,	0,		0,	
yycrank+121,	0,		0,	
yycrank+125,	0,		0,	
yycrank+79,	0,		0,	
yycrank+0,	0,		yyvstop+63,
yycrank+91,	0,		0,	
yycrank+80,	0,		0,	
yycrank+90,	0,		0,	
yycrank+85,	0,		0,	
yycrank+89,	0,		0,	
yycrank+89,	0,		0,	
yycrank+97,	0,		0,	
yycrank+106,	0,		0,	
yycrank+103,	0,		0,	
yycrank+91,	0,		0,	
yycrank+0,	0,		yyvstop+65,
yycrank+101,	0,		0,	
yycrank+130,	0,		0,	
yycrank+136,	0,		0,	
yycrank+143,	0,		0,	
yycrank+137,	0,		0,	
yycrank+132,	0,		0,	
yycrank+0,	0,		yyvstop+67,
yycrank+139,	0,		0,	
yycrank+145,	0,		yyvstop+69,
yycrank+144,	0,		0,	
yycrank+139,	0,		0,	
yycrank+130,	0,		0,	
yycrank+112,	0,		0,	
yycrank+103,	0,		0,	
yycrank+112,	0,		0,	
yycrank+116,	0,		0,	
yycrank+125,	0,		0,	
yycrank+104,	0,		0,	
yycrank+0,	0,		yyvstop+71,
yycrank+121,	0,		0,	
yycrank+117,	0,		0,	
yycrank+118,	0,		0,	
yycrank+0,	0,		yyvstop+73,
yycrank+124,	0,		0,	
yycrank+159,	0,		0,	
yycrank+0,	0,		yyvstop+75,
yycrank+147,	0,		0,	
yycrank+161,	0,		0,	
yycrank+151,	0,		0,	
yycrank+154,	0,		0,	
yycrank+162,	0,		0,	
yycrank+165,	0,		0,	
yycrank+162,	0,		0,	
yycrank+167,	0,		0,	
yycrank+168,	0,		0,	
yycrank+139,	0,		0,	
yycrank+135,	0,		0,	
yycrank+0,	0,		yyvstop+77,
yycrank+133,	0,		0,	
yycrank+133,	0,		0,	
yycrank+132,	0,		0,	
yycrank+133,	0,		0,	
yycrank+0,	0,		yyvstop+79,
yycrank+139,	0,		0,	
yycrank+135,	0,		0,	
yycrank+168,	0,		0,	
yycrank+182,	0,		0,	
yycrank+162,	0,		0,	
yycrank+165,	0,		0,	
yycrank+0,	0,		yyvstop+81,
yycrank+172,	0,		0,	
yycrank+168,	0,		0,	
yycrank+176,	0,		0,	
yycrank+167,	0,		0,	
yycrank+178,	0,		0,	
yycrank+0,	0,		yyvstop+83,
yycrank+0,	0,		yyvstop+85,
yycrank+154,	0,		0,	
yycrank+0,	0,		yyvstop+87,
yycrank+0,	0,		yyvstop+89,
yycrank+0,	0,		yyvstop+91,
yycrank+149,	0,		0,	
yycrank+156,	0,		0,	
yycrank+174,	0,		0,	
yycrank+181,	0,		0,	
yycrank+191,	0,		0,	
yycrank+188,	0,		0,	
yycrank+0,	0,		yyvstop+93,
yycrank+195,	0,		0,	
yycrank+194,	0,		0,	
yycrank+195,	0,		0,	
yycrank+0,	0,		yyvstop+95,
yycrank+0,	0,		yyvstop+97,
yycrank+164,	0,		0,	
yycrank+166,	0,		0,	
yycrank+0,	0,		yyvstop+99,
yycrank+200,	0,		0,	
yycrank+192,	0,		0,	
yycrank+190,	0,		0,	
yycrank+0,	0,		yyvstop+101,
yycrank+0,	0,		yyvstop+103,
yycrank+194,	0,		0,	
yycrank+0,	0,		yyvstop+105,
yycrank+0,	0,		yyvstop+107,
yycrank+199,	0,		0,	
yycrank+0,	0,		yyvstop+109,
yycrank+194,	0,		0,	
yycrank+0,	0,		yyvstop+111,
yycrank+0,	0,		yyvstop+113,
yycrank+0,	0,		yyvstop+115,
0,	0,	0};
struct yywork *yytop = yycrank+272;
struct yysvf *yybgin = yysvec+1;
unsigned char yymatch[] = {
00  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,011 ,012 ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
011 ,01  ,'"' ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,'-' ,01  ,01  ,
'-' ,'-' ,'-' ,'-' ,'-' ,'-' ,'-' ,'-' ,
'-' ,'-' ,01  ,01  ,01  ,01  ,01  ,01  ,
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
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
0};
char yyextra[] = {
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0};
#ident	"@(#)/usr/ccs/lib/lex/ncform.sl 1.1 1.0 10/31/92 45302 Univel"
int yylineno =1;
# define YYU(x) x
# define NLSTATE yyprevious=YYNEWLINE
char yytext[YYLMAX];
struct yysvf *yylstate [YYLMAX], **yylsp, **yyolsp;
char yysbuf[YYLMAX];
char *yysptr = yysbuf;
int *yyfnd;
extern struct yysvf *yyestate;
int yyprevious = YYNEWLINE;
#if defined(__cplusplus) || defined(__STDC__)
int yylook(void)
#else
yylook()
#endif
{
	register struct yysvf *yystate, **lsp;
	register struct yywork *yyt;
	struct yysvf *yyz;
	int yych, yyfirst;
	struct yywork *yyr;
# ifdef LEXDEBUG
	int debug;
# endif
	char *yylastch;
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
			if(yylastch > &yytext[YYLMAX]) {
				fprintf(yyout,"Input string too long, limit %d\n",YYLMAX);
				exit(1);
			}
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
			if ( (int)yyt > (int)yycrank){
				yyt = yyr + yych;
				if (yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					if(lsp > &yylstate[YYLMAX]) {
						fprintf(yyout,"Input string too long, limit %d\n",YYLMAX);
						exit(1);
					}
					goto contin;
					}
				}
# ifdef YYOPTIM
			else if((int)yyt < (int)yycrank) {		/* r < yycrank */
				yyt = yyr = yycrank+(yycrank-yyt);
# ifdef LEXDEBUG
				if(debug)fprintf(yyout,"compressed state\n");
# endif
				yyt = yyt + yych;
				if(yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					if(lsp > &yylstate[YYLMAX]) {
						fprintf(yyout,"Input string too long, limit %d\n",YYLMAX);
						exit(1);
					}
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
					if(lsp > &yylstate[YYLMAX]) {
						fprintf(yyout,"Input string too long, limit %d\n",YYLMAX);
						exit(1);
					}
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
#if defined(__cplusplus) || defined(__STDC__)
int yyback(int *p, int m)
#else
yyback(p, m)
	int *p;
#endif
{
	if (p==0) return(0);
	while (*p) {
		if (*p++ == m)
			return(1);
	}
	return(0);
}
	/* the following are only used in the lex library */
#if defined(__cplusplus) || defined(__STDC__)
int yyinput(void)
#else
yyinput()
#endif
{
	return(input());
	}
#if defined(__cplusplus) || defined(__STDC__)
void yyoutput(int c)
#else
yyoutput(c)
  int c; 
#endif
{
	output(c);
	}
#if defined(__cplusplus) || defined(__STDC__)
void yyunput(int c)
#else
yyunput(c)
   int c; 
#endif
{
	unput(c);
	}
