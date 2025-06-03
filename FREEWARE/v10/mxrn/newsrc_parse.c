#if !defined(lint) && !defined(SABER)
static char XRNrcsid[] = "$Header: /net/objy27/wrld/mnt11/ricks/src/master/xrn/newsrc.y,v 1.4 1993/01/11 02:15:12 ricks Exp $";
#endif

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

/*
 * newsrc.y - yacc parser for the newsrc file
 */

#include "copyright.h"
#include <stdio.h>
#include "config.h"
#include "utils.h"
#include "avl.h"
#include "mesg.h"
#include "news.h"
#include "newsrcfile.h"

extern int yylineno;

typedef union  {
    int integer;
    char *string;
    char character;
    struct list *item;
} YYSTYPE;
#define NAME 257
#define SEPARATOR 258
#define NUMBER 259
#define EOL 260
#define DASH 261
#define COMMA 262
#define yyclearin yychar = -1
#define yyerrok yyerrflag = 0
extern int yychar;
extern short yyerrflag;
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 150
#endif
YYSTYPE yylval,yyval;
#define YYERRCODE 256

#include "newsrc_parse.h"

short yyexca[] ={-1,1,0,-1,-2,0,};
#define YYNPROD 11
#define YYLAST 27
short yyact[]={13,8,14,15,12,10,17,12,7,5,4,11,3,2,1,6,9,0,0,0,0,0,0,0,0,0,16};
short yypact[]={
-247,-1000,-247,-1000,-250,-259,-1000,-255,-1000,-260,
-1000,-1000,-258,-1000,-252,-253,-1000,-1000 };
short yypgo[]={0,16,11,14,13,12 };
short yyr1[]={0,3,4,4,5,5,5,1,1,2,2};
short yyr2[]={0,1,1,2,4,3,2,1,3,1,3};
short yychk[]={-1000,-3,-4,-5,257,256,-5,258,260,-1,
 260,-2,259,260,262,261,-2,259};
short yydef[]={0,-2,1,2,0,0,3,0,6,0,5,7,9,4,0,0,8,10};
#define YYFLAG -1000
#define YYERROR goto yyerrlab
#define YYACCEPT return(0)
#define YYABORT return(1)

/*	parser for yacc output	*/

#ifdef YYDEBUG
int yydebug = 0; /* 1 for debugging */
#endif
YYSTYPE yyv[YYMAXDEPTH]; /* where the values are stored */
int yychar = -1; /* current input token number */
int yynerrs = 0;  /* number of errors */
short yyerrflag = 0;  /* error recovery flag */

yyparse() {
	short yys[YYMAXDEPTH];
	short yyj,yym;
	register YYSTYPE *yypvt;
	register short yystate,*yyps,yyn;
	register YYSTYPE *yypv;
	register short *yyxi;

	yystate = 0;
	yychar = -1;
	yynerrs = 0;
	yyerrflag = 0;
	yyps= &yys[-1];
	yypv= &yyv[-1];

 yystack:    /* put a state and value onto the stack */

		if( ++yyps> &yys[YYMAXDEPTH] ) { yyerror( "yacc stack overflow" ); return(1); }
		*yyps = yystate;
		++yypv;
		*yypv = yyval;

 yynewstate:

	yyn = yypact[yystate];

	if( yyn<= YYFLAG ) goto yydefault; /* simple state */

	if( yychar<0 ) if( (yychar=yylex())<0 ) yychar=0;
	if( (yyn += yychar)<0 || yyn >= YYLAST ) goto yydefault;

	if( yychk[ yyn=yyact[ yyn ] ] == yychar ){ /* valid shift */
		yychar = -1;
		yyval = yylval;
		yystate = yyn;
		if( yyerrflag > 0 ) --yyerrflag;
		goto yystack;
		}

 yydefault:
	/* default state action */

	if( (yyn=yydef[yystate]) == -2 ) {
		if( yychar<0 ) if( (yychar=yylex())<0 ) yychar = 0;
		/* look through exception table */

		for( yyxi=yyexca; (*yyxi!= (-1)) || (yyxi[1]!=yystate) ; yyxi += 2 ) ; /* VOID */

		while( *(yyxi+=2) >= 0 ){
			if( *yyxi == yychar ) break;
			}
		if( (yyn = yyxi[1]) < 0 ) return(0);   /* accept */
		}

	if( yyn == 0 ){ /* error */
		/* error ... attempt to resume parsing */

		switch( yyerrflag ){

		case 0:   /* brand new error */

			yyerror( "syntax error" );
		yyerrlab:
			++yynerrs;

		case 1:
		case 2: /* incompletely recovered error ... try again */

			yyerrflag = 3;

			/* find a state where "error" is a legal shift action */

			while ( yyps >= yys ) {
			   yyn = yypact[*yyps] + YYERRCODE;
			   if( yyn>= 0 && yyn < YYLAST && yychk[yyact[yyn]] == YYERRCODE ){
			      yystate = yyact[yyn];  /* simulate a shift of "error" */
			      goto yystack;
			      }
			   yyn = yypact[*yyps];

			   /* the current yyps has no shift on "error",pop stack */

			   --yyps;
			   --yypv;
			   }

			/* there is no state on the stack with an error shift ... abort */

	yyabort:
			return(1);


		case 3:  /* no shift yet; clobber input char */

			if( yychar == 0 ) goto yyabort; /* don't discard EOF,quit */
			yychar = -1;
			goto yynewstate;   /* try again in the same state */

			}
		}

	/* reduction by production yyn */

		yyps -= yyr2[yyn];
		yypvt = yypv;
		yypv -= yyr2[yyn];
		yyval = yypv[1];
		yym=yyn;
			/* consult goto table to find next state */
		yyn = yyr1[yyn];
		yyj = yypgo[yyn] + *yyps + 1;
		if( yyj>=YYLAST || yychk[ yystate = yyact[yyj] ] != -yyn ) yystate = yyact[yypgo[yyn]];
		switch(yym){
			
case 4:
{
		    struct newsgroup *newsgroup;
		    char *dummy;

		    if (!avl_lookup(NewsGroupTable,yypvt[-3].string,&dummy)) {
			mesgPane(XRN_SERIOUS,"Bogus newsgroup `%s',removing from the newsrc file",yypvt[-3].string);
		    } else {
			newsgroup = (struct newsgroup *) dummy;
			if (IS_NOENTRY(newsgroup)) {
			    CLEAR_NOENTRY(newsgroup);
			    newsgroup->status |= (yypvt[-2].character == ':' ? NG_SUB : NG_UNSUB);
			    newsgroup->nglist = yypvt[-1].item;
			    updateArticleArray(newsgroup,False);
			    newsgroup->newsrc = MaxGroupNumber;
			    Newsrc[MaxGroupNumber++] = newsgroup;
			} else {
			    mesgPane(XRN_SERIOUS,"Duplicate entry for `%s',using first one",
					   yypvt[-3].string);
			}
		    }
		    XtFree(yypvt[-3].string);
	        } break;
case 5:
{
		    struct newsgroup *newsgroup;
		    char *dummy;

		    if (!avl_lookup(NewsGroupTable,yypvt[-2].string,&dummy)) {
			mesgPane(XRN_SERIOUS,"Bogus newsgroup `%s',removing from the newsrc file",yypvt[-2].string);
		    } else {
			newsgroup = (struct newsgroup *) dummy;
			if (IS_NOENTRY(newsgroup)) {
			    CLEAR_NOENTRY(newsgroup);
			    newsgroup->status |= (yypvt[-1].character == ':' ? NG_SUB : NG_UNSUB);
			    newsgroup->nglist = NIL(struct list);
			    updateArticleArray(newsgroup,False);
			    newsgroup->newsrc = MaxGroupNumber;
			    Newsrc[MaxGroupNumber++] = newsgroup;
			} else {
			    mesgPane(XRN_SERIOUS,"Duplicate entry for `%s',using first one",
					  yypvt[-2].string);
			}
		    }
		    XtFree(yypvt[-2].string);
	        } break;
case 6:
{
		mesgPane(XRN_SERIOUS,"Unable to parse line %d in newsrc file,ignoring",
			    yylineno - 1);	/* yylineno stepped at EOL */
		yyerrok;
		yyclearin;
	    } break;
case 7:
{
		    yyval.item = yypvt[-0].item;
		} break;
case 8:
{
		    struct list *temp;

		    yyval.item = yypvt[-2].item;
		    for (temp = yyval.item; temp != NIL(struct list); temp = temp->next) {
			if (temp->next == NIL(struct list)) {
			    temp->next = yypvt[-0].item;
			    break;
			}
		    }
		} break;
case 9:
{
		    yyval.item = ALLOC(struct list);
		    yyval.item->type = SINGLE;
		    yyval.item->contents.single = (art_num) yypvt[-0].integer;
		    yyval.item->next = NIL(struct list);
		} break;
case 10:
{
		    yyval.item = ALLOC(struct list);
		    yyval.item->type = RANGE;
		    yyval.item->contents.range.start = (art_num) yypvt[-2].integer;
		    yyval.item->contents.range.end = (art_num) yypvt[-0].integer;
		    yyval.item->next = NIL(struct list);
		} break; 
		}
		goto yystack;  /* stack new state and value */

	}
