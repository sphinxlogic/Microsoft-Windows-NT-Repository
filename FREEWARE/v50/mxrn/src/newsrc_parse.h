#include <stdio.h>
static int yylook();
static int yyback();
extern int atoi();
#define U(x) ((x)&0377)
#define NLSTATE yyprevious=YYNEWLINE
#define BEGIN yybgin = yysvec + 1 +
#define INITIAL 0
#define YYLERR yysvec
#define YYSTATE (yyestate-yysvec-1)
#define YYOPTIM 1
#define YYLMAX 1000
#define output(c) putc(c,yyout)
#define input() (((yytchar=yysptr>yysbuf?U(*--yysptr):getc(yyin))==10?(yylineno++,yytchar):yytchar)==EOF?0:yytchar)
#define unput(c) {yytchar= (c);if(yytchar=='\n')yylineno--;*yysptr++=yytchar;}
#define yymore() (yymorfg=1)
#define ECHO fprintf(yyout,"%s",yytext)
#define REJECT { nstr = yyreject(); goto yyfussy;}
int yyleng; extern char yytext[];
int yymorfg;
extern char *yysptr,yysbuf[];
int yytchar;
FILE *yyin,*yyout;
extern int yylineno;
struct yysvf { 
	struct yywork *yystoff;
	struct yysvf *yyother;
	int *yystops;};
struct yysvf *yyestate;
extern struct yysvf yysvec[],*yybgin;
#define YYNEWLINE 10
yylex(){
int nstr; extern int yyprevious;
yyin = Newsrcfp;
while((nstr = yylook()) >= 0)
yyfussy: switch(nstr){
case 0:return(0);
case 1:break;
case 2:return(EOL); break;
case 3:return(DASH);break;
case 4:return(COMMA);break;
case 5:yylval.character = yytext[0]; return(SEPARATOR);break;
case 6:yylval.integer = atoi((char *)yytext); return(NUMBER);break;
case 7:
case 8:	{
	char c = yytext[yyleng - 1];
	yytext[yyleng-1] = '\0';
	yylval.string = XtNewString((char *) yytext);
	unput(c);
	return(NAME);
	} break;
case 9:break;
case 10: {
	optionsLine = XtNewString((char *)yytext);
	optionsLine[utStrlen(optionsLine) - 1] = '\0';
	} break;
case -1: break;
default: fprintf(yyout,"bad switch yylook %d",nstr);
} return(0); }
/* end of yylex */
/*
 * $Header: /net/objy27/wrld/mnt11/ricks/src/master/xrn/newsrc.l,v 1.3 1993/01/11 02:15:09 ricks Exp $
 */

/*
 * xrn - an X-based NNTP news reader
 *
 * Copyright (c) 1988-1993,Ellen M. Sentovich and Rick L. Spickelmier.
 *
 * Permission to use,copy,modify,and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation,and that the name of the University of California not
 * be used in advertising or publicity pertaining to distribution of 
 * the software without specific,written prior permission.  The University
 * of California makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty.
 *
 * THE UNIVERSITY OF CALIFORNIA DISCLAIMS ALL WARRANTIES WITH REGARD TO 
 * THIS SOFTWARE,INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND 
 * FITNESS,IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE FOR
 * ANY SPECIAL,INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE,DATA OR PROFITS,WHETHER IN AN ACTION OF
 * CONTRACT,NEGLIGENCE OR OTHER TORTIOUS ACTION,ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*ARGSUSED*/
yyerror(s)
char *s;
{ }

int yyvstop[] ={0,9,0,2,0,5,0,4,0,3,0,6,0,9,0,1,2,0,5,0,4,0,3,0,6,0,8,
0,1,0,8,0,7,0,7,8,0,10,0,0};
#define YYTYPE unsigned char
struct yywork { YYTYPE verify,advance; } yycrank[] ={
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,3,1,4,11,21,11,22,13,0,13,0,
14,0,14,0,15,0,15,0,0,0,16,0,16,0,0,0,18,0,18,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,1,3,1,5,11,21,0,0,13,0,0,0,14,0,0,0,15,0,20,0,20,0,16,0,1,6,1,7,18,0,
0,0,1,8,1,8,1,8,1,8,1,8,1,8,1,8,1,8,1,8,1,8,1,5,16,16,0,0,0,0,0,0,
0,0,20,0,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,
1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,0,0,0,0,0,0,0,0,
0,0,0,0,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,
1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,2,10,0,0,0,0,18,25,
0,0,0,0,0,0,0,0,2,11,2,12,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
9,19,0,0,10,10,0,0,0,0,17,23,19,0,19,0,9,0,9,0,10,0,10,0,2,13,17,0,17,0,23,0,
23,0,24,0,24,0,25,0,25,0,0,0,0,0,2,14,2,15,0,0,0,0,2,16,0,0,19,0,19,26,9,0,
9,19,10,0,10,20,0,0,17,0,17,24,23,0,23,27,24,0,24,27,25,0,25,27,
2,17,0,0,0,0,9,19,0,0,10,10,26,0,26,0,17,23,27,0,27,0,28,0,
28,0,29,0,29,0,30,0,30,0,31,0,31,0,0,0,9,19,0,0,10,10,0,0,
0,0,17,23,32,0,32,0,0,0,26,0,26,26,0,0,27,0,27,27,28,0,28,27,
29,0,29,27,30,0,30,27,31,0,31,27,0,0,0,0,33,33,0,0,2,18,0,0,
0,0,32,33,32,27,0,0,33,33,33,34,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
33,33,0,0,0,0,0,0,0,0,0,0,25,28,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,33,33,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,28,29,
33,33,0,0,0,0,0,0,0,0,0,0,0,0,29,30,30,31,0,0,0,0,0,0,0,0,0,0,0,0,31,32,
0,0};
struct yysvf yysvec[] ={
0,0,0,yycrank+1,0,0,yycrank+-123,0,0,yycrank+0,0,yyvstop+1,
yycrank+0,0,yyvstop+3,yycrank+0,0,yyvstop+5,yycrank+0,0,yyvstop+7,
yycrank+0,0,yyvstop+9,yycrank+86,0,yyvstop+11,yycrank+-143,0,0,
yycrank+-145,0,0,yycrank+3,0,yyvstop+13,yycrank+0,0,yyvstop+15,
yycrank+-5,yysvec+10,yyvstop+18,yycrank+-7,yysvec+10,yyvstop+20,
yycrank+-9,yysvec+10,yyvstop+22,yycrank+-12,yysvec+10,yyvstop+24,
yycrank+-148,0,0,yycrank+-15,yysvec+17,0,yycrank+-141,yysvec+9,0,
yycrank+-33,yysvec+10,yyvstop+26,yycrank+0,yysvec+11,0,
yycrank+0,0,yyvstop+28,yycrank+-150,yysvec+17,0,
yycrank+-152,yysvec+17,yyvstop+30,yycrank+-154,yysvec+17,0,
yycrank+-185,yysvec+9,yyvstop+32,yycrank+-188,yysvec+17,yyvstop+34,
yycrank+-190,yysvec+17,0,yycrank+-192,yysvec+17,0,yycrank+-194,yysvec+17,0,
yycrank+-196,yysvec+17,0,yycrank+-205,yysvec+17,0,yycrank+-231,0,0,
yycrank+0,0,yyvstop+37,0,0,0};
struct yywork *yytop = yycrank+311;
struct yysvf *yybgin = yysvec+1;
char yymatch[] ={00,01,01,01,01,01,01,01,01,011,012,01,01,01,01,01,
01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,011,'!',01,01,01,01,01,01,
01,01,01,01,01,01,01,01,'0','0','0','0','0','0','0','0',
'0','0','!',01,01,01,01,01,01,'A','A','A','A','A','A','A',
'A','A','A','A','A','A','A','A','A','A','A','A','A','A','A','A',
'A','A','A',01,01,01,01,01,01,'A','A','A','A','A','A','A',
'A','A','A','A','A','A','A','A','A','A','A','A','A','A','A','A',
'A','A','A',01,01,01,01,01,01,01,01,01,01,01,01,01,
01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,
01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,
01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,
01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,
01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,
01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,
01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,
01,01,01,01,01,01,01,01,0};
char yyextra[] ={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

int yylineno =1;
#define YYU(x) x
#define NLSTATE yyprevious=YYNEWLINE
char yytext[YYLMAX];
struct yysvf *yylstate [YYLMAX],**yylsp,**yyolsp;
char yysbuf[YYLMAX];
char *yysptr = yysbuf;
int *yyfnd;
extern struct yysvf *yyestate;
int yyprevious = YYNEWLINE;
static int yylook(){
	register struct yysvf *yystate,**lsp;
	register struct yywork *yyt;
	struct yysvf *yyz;
	int yych;
	struct yywork *yyr;
	char *yylastch;
	/* start off machines */
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
			yyt = yystate->yystoff;
			if(yyt == yycrank){		/* may not be any transitions */
				yyz = yystate->yyother;
				if(yyz == 0)break;
				if(yyz->yystoff == yycrank)break;
				}
			*yylastch++ = yych = input();
		tryagain:
			yyr = yyt;
			if ( (int)yyt > (int)yycrank){
				yyt = yyr + yych;
				if (yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					goto contin;
					}
				}
#ifdef YYOPTIM
			else if((int)yyt < (int)yycrank) {		/* r < yycrank */
				yyt = yyr = yycrank+(yycrank-yyt);
				yyt = yyt + yych;
				if(yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					goto contin;
					}
				yyt = yyr + YYU(yymatch[yych]);
				if(yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transition */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					goto contin;
					}
				}
			if ((yystate = yystate->yyother) && (yyt= yystate->yystoff) != yycrank){
				goto tryagain;
				}
#endif
			else
				{unput(*--yylastch);break;}
		contin:
			;
			}
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
		}
	}
static int yyback(p,m)
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
yyinput(){ return(input());}
yyoutput(c)
int c; {output(c); }
yyunput(c)
int c; {unput(c);}
