# include "stdio.h"
# define U(x) ((x)&0377)
# define NLSTATE LayYYprevious=YYNEWLINE
# define BEGIN LayYYbgin = LayYYsvec + 1 +
# define INITIAL 0
# define YYLERR LayYYsvec
# define YYSTATE (LayYYestate-LayYYsvec-1)
# define YYOPTIM 1
# define YYLMAX 200
# define output(c) putc(c,LayYYout)
# define input() (((LayYYtchar=LayYYsptr>LayYYsbuf?U(*--LayYYsptr):getc(LayYYin))==10?(LayYYlineno++,LayYYtchar):LayYYtchar)==EOF?0:LayYYtchar)
# define unput(c) {LayYYtchar= (c);if(LayYYtchar=='\n')LayYYlineno--;*LayYYsptr++=LayYYtchar;}
# define LayYYmore() (LayYYmorfg=1)
# define ECHO fprintf(LayYYout, "%s",LayYYtext)
# define REJECT { nstr = LayYYreject(); goto LayYYfussy;}
int LayYYleng; extern unsigned char LayYYtext[];
int LayYYmorfg;
extern unsigned char *LayYYsptr, LayYYsbuf[];
int LayYYtchar;
FILE *LayYYin = {stdin}, *LayYYout = {stdout};
extern int LayYYlineno;
struct LayYYsvf { 
	struct LayYYwork *LayYYstoff;
	struct LayYYsvf *LayYYother;
	int *LayYYstops;};
struct LayYYsvf *LayYYestate;
extern struct LayYYsvf LayYYsvec[], *LayYYbgin;
#undef input
#undef unput

#include    <X11/Xlib.h>
#include    <X11/Xresource.h>
#include    <X11/IntrinsicP.h>
#include    <X11/StringDefs.h>

#include    "LayoutP.h"
#include    "laygram.h"
static char *LayYYsourcebase, *LayYYsource;

#define input() (*LayYYsource++)
#define unput(c)    (--LayYYsource)

# define YYNEWLINE 10
LayYYlex(){
int nstr; extern int LayYYprevious;
while((nstr = LayYYlook()) >= 0)
LayYYfussy: switch(nstr){
case 0:
if(LayYYwrap()) return(0); break;
case 1:
	return VERTICAL;
break;
case 2:
	return HORIZONTAL;
break;
case 3:
		return OC;
break;
case 4:
		return CC;
break;
case 5:
		return OP;
break;
case 6:
		return CP;
break;
case 7:
		return OA;
break;
case 8:
		return CA;
break;
case 9:
	{ LayYYlval.ival = 1; return INFINITY; }
break;
case 10:
		{ LayYYlval.ival = count(LayYYtext, 'f'); return INFINITY; }
break;
case 11:
	{ LayYYlval.ival = atoi(LayYYtext); return NUMBER; }
break;
case 12:
		{ return EQUAL; }
break;
case 13:
		{ return DOLLAR; }
break;
case 14:
		{ LayYYlval.oval = Plus; return PLUS; }
break;
case 15:
		{ LayYYlval.oval = Minus; return MINUS; }
break;
case 16:
		{ LayYYlval.oval = Times; return TIMES; }
break;
case 17:
		{ LayYYlval.oval = Divide; return DIVIDE; }
break;
case 18:
		{ LayYYlval.oval = Percent; return PERCENT; }
break;
case 19:
	{ LayYYlval.oval = Percent; return PERCENTOF; }
break;
case 20:
		return WIDTH;
break;
case 21:
		return HEIGHT;
break;
case 22:
{ 
			    LayYYtext[LayYYleng - 1] = '\0';
			    LayYYlval.qval = XrmStringToQuark (LayYYtext+1);
 			    return NAME;
			}
break;
case 23:
{ 
			    LayYYtext[LayYYleng - 1] = '\0';
			    LayYYlval.qval = XrmStringToQuark (LayYYtext);
 			    return NAME;
			}
break;
case 24:
		;
break;
case 25:
		;
break;
case 26:
		;
break;
case 27:
		fprintf (stderr, "ignoring %c\n", *LayYYtext);
break;
case -1:
break;
default:
fprintf(LayYYout,"bad switch LayYYlook %d",nstr);
} return(0); }
/* end of LayYYlex */

static int
count (s, c)
    char    *s;
    char    c;
{
    int	i = 0;
    while (*s)
	if (*s++ == c)
	    i++;
    return i;
}

LayYYsetsource(s)
    char    *s;
{
    LayYYsourcebase = LayYYsource = s;
}

LayYYerror(s)
    char    *s;
{
    char    *t;
    
    fprintf (stderr, "%s\n", s);
    t = LayYYsource - 50;
    if (t < LayYYsourcebase)
	t = LayYYsourcebase;
    while (*t && t < LayYYsource + 50) {
	if (t == LayYYsource)
	    putc ('@', stderr);
	putc (*t++, stderr);
    }
    if (t == LayYYsource)
	putc ('@', stderr);
    if (!*t)
	fprintf (stderr, "<EOF>");
    fprintf (stderr, "\n");
}
int LayYYvstop[] = {
0,

27,
0,

25,
27,
0,

26,
0,

24,
27,
0,

13,
27,
0,

18,
27,
0,

5,
27,
0,

6,
27,
0,

16,
27,
0,

14,
27,
0,

15,
27,
0,

17,
27,
0,

11,
27,
0,

7,
27,
0,

12,
27,
0,

8,
27,
0,

23,
27,
0,

27,
0,

23,
27,
0,

23,
27,
0,

23,
27,
0,

23,
27,
0,

3,
27,
0,

4,
27,
0,

11,
0,

23,
0,

22,
0,

23,
0,

23,
0,

23,
0,

23,
0,

23,
0,

19,
0,

23,
0,

23,
0,

10,
23,
0,

23,
0,

23,
0,

23,
0,

23,
0,

10,
23,
0,

23,
0,

23,
0,

23,
0,

23,
0,

23,
0,

23,
0,

23,
0,

20,
23,
0,

21,
23,
0,

23,
0,

23,
0,

23,
0,

23,
0,

23,
0,

23,
0,

23,
0,

9,
23,
0,

1,
23,
0,

23,
0,

2,
23,
0,
0};
# define YYTYPE unsigned char
struct LayYYwork { YYTYPE verify, advance; } LayYYcrank[] = {
0,0,	0,0,	1,3,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	1,4,	1,5,	
8,27,	8,27,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	1,6,	0,0,	8,27,	
0,0,	1,7,	1,8,	0,0,	
0,0,	1,9,	1,10,	1,11,	
1,12,	0,0,	1,13,	0,0,	
1,14,	1,15,	15,29,	15,29,	
15,29,	15,29,	15,29,	15,29,	
15,29,	15,29,	15,29,	15,29,	
0,0,	1,16,	1,17,	1,18,	
0,0,	0,0,	1,19,	2,6,	
0,0,	0,0,	0,0,	2,7,	
2,8,	0,0,	0,0,	2,9,	
2,10,	2,11,	2,12,	0,0,	
2,13,	0,0,	2,14,	31,31,	
31,31,	31,31,	31,31,	31,31,	
31,31,	31,31,	31,31,	31,31,	
31,31,	1,20,	0,0,	2,16,	
2,17,	2,18,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
57,60,	1,21,	1,22,	36,42,	
21,32,	23,35,	28,37,	34,40,	
24,36,	32,38,	8,28,	38,43,	
39,44,	22,34,	21,33,	1,23,	
1,24,	33,39,	35,41,	41,47,	
1,25,	40,45,	1,26,	2,20,	
40,46,	42,48,	43,49,	44,50,	
45,45,	46,51,	47,52,	48,53,	
49,54,	50,55,	51,56,	2,21,	
2,22,	52,57,	55,58,	56,59,	
58,61,	59,62,	60,63,	61,64,	
64,65,	0,0,	0,0,	0,0,	
0,0,	2,23,	2,24,	0,0,	
0,0,	0,0,	2,25,	0,0,	
2,26,	19,30,	19,30,	19,30,	
19,30,	19,30,	19,30,	19,30,	
19,30,	19,30,	19,30,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	19,30,	19,30,	
19,30,	19,30,	19,30,	19,30,	
19,30,	19,30,	19,30,	19,30,	
19,30,	19,30,	19,30,	19,30,	
19,30,	19,30,	19,30,	19,30,	
19,30,	19,30,	19,30,	19,30,	
19,30,	19,30,	19,30,	19,30,	
0,0,	0,0,	0,0,	0,0,	
19,30,	0,0,	19,30,	19,30,	
19,30,	19,30,	19,30,	19,30,	
19,30,	19,30,	19,30,	19,30,	
19,30,	19,30,	19,30,	19,30,	
19,30,	19,30,	19,30,	19,30,	
19,30,	19,30,	19,30,	19,30,	
19,30,	19,30,	19,30,	19,30,	
20,31,	20,31,	20,31,	20,31,	
20,31,	20,31,	20,31,	20,31,	
20,31,	20,31,	20,31,	20,31,	
20,31,	20,31,	20,31,	20,31,	
20,31,	20,31,	20,31,	20,31,	
20,31,	20,31,	20,31,	20,31,	
20,31,	20,31,	0,0,	0,0,	
0,0,	0,0,	20,31,	0,0,	
20,31,	20,31,	20,31,	20,31,	
20,31,	20,31,	20,31,	20,31,	
20,31,	20,31,	20,31,	20,31,	
20,31,	20,31,	20,31,	20,31,	
20,31,	20,31,	20,31,	20,31,	
20,31,	20,31,	20,31,	20,31,	
20,31,	20,31,	0,0,	0,0,	
0,0};
struct LayYYsvf LayYYsvec[] = {
0,	0,	0,
LayYYcrank+-1,	0,		0,	
LayYYcrank+-35,	LayYYsvec+1,	0,	
LayYYcrank+0,	0,		LayYYvstop+1,
LayYYcrank+0,	0,		LayYYvstop+3,
LayYYcrank+0,	0,		LayYYvstop+6,
LayYYcrank+0,	0,		LayYYvstop+8,
LayYYcrank+0,	0,		LayYYvstop+11,
LayYYcrank+3,	0,		LayYYvstop+14,
LayYYcrank+0,	0,		LayYYvstop+17,
LayYYcrank+0,	0,		LayYYvstop+20,
LayYYcrank+0,	0,		LayYYvstop+23,
LayYYcrank+0,	0,		LayYYvstop+26,
LayYYcrank+0,	0,		LayYYvstop+29,
LayYYcrank+0,	0,		LayYYvstop+32,
LayYYcrank+2,	0,		LayYYvstop+35,
LayYYcrank+0,	0,		LayYYvstop+38,
LayYYcrank+0,	0,		LayYYvstop+41,
LayYYcrank+0,	0,		LayYYvstop+44,
LayYYcrank+113,	0,		LayYYvstop+47,
LayYYcrank+171,	0,		LayYYvstop+50,
LayYYcrank+7,	LayYYsvec+19,	LayYYvstop+52,
LayYYcrank+7,	LayYYsvec+19,	LayYYvstop+55,
LayYYcrank+8,	LayYYsvec+19,	LayYYvstop+58,
LayYYcrank+7,	LayYYsvec+19,	LayYYvstop+61,
LayYYcrank+0,	0,		LayYYvstop+64,
LayYYcrank+0,	0,		LayYYvstop+67,
LayYYcrank+0,	LayYYsvec+8,	0,	
LayYYcrank+8,	0,		0,	
LayYYcrank+0,	LayYYsvec+15,	LayYYvstop+70,
LayYYcrank+0,	LayYYsvec+19,	LayYYvstop+72,
LayYYcrank+35,	LayYYsvec+20,	LayYYvstop+74,
LayYYcrank+8,	LayYYsvec+19,	LayYYvstop+76,
LayYYcrank+7,	LayYYsvec+19,	LayYYvstop+78,
LayYYcrank+9,	LayYYsvec+19,	LayYYvstop+80,
LayYYcrank+8,	LayYYsvec+19,	LayYYvstop+82,
LayYYcrank+7,	LayYYsvec+19,	LayYYvstop+84,
LayYYcrank+0,	0,		LayYYvstop+86,
LayYYcrank+12,	LayYYsvec+19,	LayYYvstop+88,
LayYYcrank+11,	LayYYsvec+19,	LayYYvstop+90,
LayYYcrank+23,	LayYYsvec+19,	LayYYvstop+92,
LayYYcrank+7,	LayYYsvec+19,	LayYYvstop+95,
LayYYcrank+13,	LayYYsvec+19,	LayYYvstop+97,
LayYYcrank+26,	LayYYsvec+19,	LayYYvstop+99,
LayYYcrank+9,	LayYYsvec+19,	LayYYvstop+101,
LayYYcrank+30,	LayYYsvec+19,	LayYYvstop+103,
LayYYcrank+23,	LayYYsvec+19,	LayYYvstop+106,
LayYYcrank+29,	LayYYsvec+19,	LayYYvstop+108,
LayYYcrank+31,	LayYYsvec+19,	LayYYvstop+110,
LayYYcrank+20,	LayYYsvec+19,	LayYYvstop+112,
LayYYcrank+26,	LayYYsvec+19,	LayYYvstop+114,
LayYYcrank+33,	LayYYsvec+19,	LayYYvstop+116,
LayYYcrank+42,	LayYYsvec+19,	LayYYvstop+118,
LayYYcrank+0,	LayYYsvec+19,	LayYYvstop+120,
LayYYcrank+0,	LayYYsvec+19,	LayYYvstop+123,
LayYYcrank+32,	LayYYsvec+19,	LayYYvstop+126,
LayYYcrank+27,	LayYYsvec+19,	LayYYvstop+128,
LayYYcrank+7,	LayYYsvec+19,	LayYYvstop+130,
LayYYcrank+28,	LayYYsvec+19,	LayYYvstop+132,
LayYYcrank+24,	LayYYsvec+19,	LayYYvstop+134,
LayYYcrank+38,	LayYYsvec+19,	LayYYvstop+136,
LayYYcrank+50,	LayYYsvec+19,	LayYYvstop+138,
LayYYcrank+0,	LayYYsvec+19,	LayYYvstop+140,
LayYYcrank+0,	LayYYsvec+19,	LayYYvstop+143,
LayYYcrank+40,	LayYYsvec+19,	LayYYvstop+146,
LayYYcrank+0,	LayYYsvec+19,	LayYYvstop+148,
0,	0,	0};
struct LayYYwork *LayYYtop = LayYYcrank+293;
struct LayYYsvf *LayYYbgin = LayYYsvec+1;
unsigned char LayYYmatch[] = {
00  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,011 ,012 ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
011 ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,
'0' ,'0' ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,01  ,01  ,01  ,01  ,'A' ,
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
unsigned char LayYYextra[] = {
0,0,0,0,0,0,0,0,
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
int LayYYlineno =1;
# define YYU(x) x
# define NLSTATE LayYYprevious=YYNEWLINE
unsigned char LayYYtext[YYLMAX];
struct LayYYsvf *LayYYlstate [YYLMAX], **LayYYlsp, **LayYYolsp;
unsigned char LayYYsbuf[YYLMAX];
unsigned char *LayYYsptr = LayYYsbuf;
int *LayYYfnd;
extern struct LayYYsvf *LayYYestate;
int LayYYprevious = YYNEWLINE;
LayYYlook(){
	register struct LayYYsvf *LayYYstate, **lsp;
	register struct LayYYwork *LayYYt;
	struct LayYYsvf *LayYYz;
	int LayYYch, LayYYfirst;
	struct LayYYwork *LayYYr;
# ifdef LEXDEBUG
	int debug;
# endif
	unsigned char *LayYYlastch;
	/* start off machines */
# ifdef LEXDEBUG
	debug = 0;
# endif
	LayYYfirst=1;
	if (!LayYYmorfg)
		LayYYlastch = LayYYtext;
	else {
		LayYYmorfg=0;
		LayYYlastch = LayYYtext+LayYYleng;
		}
	for(;;){
		lsp = LayYYlstate;
		LayYYestate = LayYYstate = LayYYbgin;
		if (LayYYprevious==YYNEWLINE) LayYYstate++;
		for (;;){
# ifdef LEXDEBUG
			if(debug)fprintf(LayYYout,"state %d\n",LayYYstate-LayYYsvec-1);
# endif
			LayYYt = LayYYstate->LayYYstoff;
			if(LayYYt == LayYYcrank && !LayYYfirst){  /* may not be any transitions */
				LayYYz = LayYYstate->LayYYother;
				if(LayYYz == 0)break;
				if(LayYYz->LayYYstoff == LayYYcrank)break;
				}
			*LayYYlastch++ = LayYYch = input();
			LayYYfirst=0;
		tryagain:
# ifdef LEXDEBUG
			if(debug){
				fprintf(LayYYout,"char ");
				allprint(LayYYch);
				putchar('\n');
				}
# endif
			LayYYr = LayYYt;
			if ( LayYYt > LayYYcrank){
				LayYYt = LayYYr + LayYYch;
				if (LayYYt <= LayYYtop && LayYYt->verify+LayYYsvec == LayYYstate){
					if(LayYYt->advance+LayYYsvec == YYLERR)	/* error transitions */
						{unput(*--LayYYlastch);break;}
					*lsp++ = LayYYstate = LayYYt->advance+LayYYsvec;
					goto contin;
					}
				}
# ifdef YYOPTIM
			else if(LayYYt < LayYYcrank) {		/* r < LayYYcrank */
				LayYYt = LayYYr = LayYYcrank+(LayYYcrank-LayYYt);
# ifdef LEXDEBUG
				if(debug)fprintf(LayYYout,"compressed state\n");
# endif
				LayYYt = LayYYt + LayYYch;
				if(LayYYt <= LayYYtop && LayYYt->verify+LayYYsvec == LayYYstate){
					if(LayYYt->advance+LayYYsvec == YYLERR)	/* error transitions */
						{unput(*--LayYYlastch);break;}
					*lsp++ = LayYYstate = LayYYt->advance+LayYYsvec;
					goto contin;
					}
				LayYYt = LayYYr + YYU(LayYYmatch[LayYYch]);
# ifdef LEXDEBUG
				if(debug){
					fprintf(LayYYout,"try fall back character ");
					allprint(YYU(LayYYmatch[LayYYch]));
					putchar('\n');
					}
# endif
				if(LayYYt <= LayYYtop && LayYYt->verify+LayYYsvec == LayYYstate){
					if(LayYYt->advance+LayYYsvec == YYLERR)	/* error transition */
						{unput(*--LayYYlastch);break;}
					*lsp++ = LayYYstate = LayYYt->advance+LayYYsvec;
					goto contin;
					}
				}
			if ((LayYYstate = LayYYstate->LayYYother) && (LayYYt= LayYYstate->LayYYstoff) != LayYYcrank){
# ifdef LEXDEBUG
				if(debug)fprintf(LayYYout,"fall back to state %d\n",LayYYstate-LayYYsvec-1);
# endif
				goto tryagain;
				}
# endif
			else
				{unput(*--LayYYlastch);break;}
		contin:
# ifdef LEXDEBUG
			if(debug){
				fprintf(LayYYout,"state %d char ",LayYYstate-LayYYsvec-1);
				allprint(LayYYch);
				putchar('\n');
				}
# endif
			;
			}
# ifdef LEXDEBUG
		if(debug){
			fprintf(LayYYout,"stopped at %d with ",*(lsp-1)-LayYYsvec-1);
			allprint(LayYYch);
			putchar('\n');
			}
# endif
		while (lsp-- > LayYYlstate){
			*LayYYlastch-- = 0;
			if (*lsp != 0 && (LayYYfnd= (*lsp)->LayYYstops) && *LayYYfnd > 0){
				LayYYolsp = lsp;
				if(LayYYextra[*LayYYfnd]){		/* must backup */
					while(LayYYback((*lsp)->LayYYstops,-*LayYYfnd) != 1 && lsp > LayYYlstate){
						lsp--;
						unput(*LayYYlastch--);
						}
					}
				LayYYprevious = YYU(*LayYYlastch);
				LayYYlsp = lsp;
				LayYYleng = LayYYlastch-LayYYtext+1;
				LayYYtext[LayYYleng] = 0;
# ifdef LEXDEBUG
				if(debug){
					fprintf(LayYYout,"\nmatch ");
					sprint(LayYYtext);
					fprintf(LayYYout," action %d\n",*LayYYfnd);
					}
# endif
				return(*LayYYfnd++);
				}
			unput(*LayYYlastch);
			}
		if (LayYYtext[0] == 0  /* && feof(LayYYin) */)
			{
			LayYYsptr=LayYYsbuf;
			return(0);
			}
		LayYYprevious = LayYYtext[0] = input();
		if (LayYYprevious>0)
			output(LayYYprevious);
		LayYYlastch=LayYYtext;
# ifdef LEXDEBUG
		if(debug)putchar('\n');
# endif
		}
	}
LayYYback(p, m)
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
LayYYinput(){
	return(input());
	}
LayYYoutput(c)
  int c; {
	output(c);
	}
LayYYunput(c)
   int c; {
	unput(c);
	}
