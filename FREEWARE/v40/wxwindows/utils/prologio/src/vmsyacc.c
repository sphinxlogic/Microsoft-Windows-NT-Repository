
# line 2 "parser.y"
#include "string.h"
#include "expr.h"

#ifndef __EXTERN_C__
#define __EXTERN_C__ 1
#endif

#if defined(__cplusplus) || defined(__STDC__)
#if defined(__cplusplus) && defined(__EXTERN_C__)
extern "C" {
#endif
#endif
int yylex(void);
int yylook(void);
int yywrap(void);
int yyback(int *, int);

/* You may need to put /DLEX_SCANNER in your makefile
 * if you're using LEX!
 */
#ifdef LEX_SCANNER
/* int yyoutput(int); */
void yyoutput(int);
#else
void yyoutput(int);
#endif

#if defined(__cplusplus) || defined(__STDC__)
#if defined(__cplusplus) && defined(__EXTERN_C__)
}
#endif
#endif

# line 36 "parser.y"
typedef union  {
    char *s;
/*    struct pexpr *expr; */
} YYSTYPE;
# define INTEGER 1
# define WORD 2
# define STRING 3
# define PERIOD 13
# define OPEN 4
# define CLOSE 5
# define COMMA 6
# define NEWLINE 7
# define ERROR 8
# define OPEN_SQUARE 9
# define CLOSE_SQUARE 10
# define EQUALS 11
# define EXP 14
#define yyclearin yychar = -1
#define yyerrok yyerrflag = 0
extern int yychar;
extern int yyerrflag;
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 150
#endif
YYSTYPE yylval, yyval;
typedef int yytabelem;
# define YYERRCODE 256

# line 117 "parser.y"


#include "lex_yy.c"

/*
void yyerror(s)
char *s;
{
  syntax_error(s);
}
*/

/* Ansi prototype. If this doesn't work for you... uncomment
   the above instead.
 */

void yyerror(char *s)
{
  syntax_error(s);
}

/*
 * Unfortunately, my DOS version of FLEX
 * requires yywrap to be #def'ed, whereas
 * the UNIX flex expects a proper function.
 */

#ifdef USE_DEFINE
#ifndef yywrap
#define yywrap() 1
#endif
#else
int yywrap() { return 1; }
#endif
static yytabelem yyexca[] ={
-1, 1,
	0, -1,
	-2, 0,
	};
# define YYNPROD 21
# define YYLAST 255
static yytabelem yyact[]={

     3,    15,    31,    12,    23,    24,     8,     6,    17,    14,
    16,    21,    19,    10,     9,     7,     6,    11,    17,    28,
    16,    17,    14,    16,    27,    26,     6,     8,    20,     6,
    25,     8,    32,    18,    22,     4,    30,    29,    13,     2,
     1,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     5 };
static yytabelem yypact[]={

 -1000,    -2, -1000,     2,     1,     0,     7, -1000,    20, -1000,
 -1000, -1000,    18,     5,    23, -1000, -1000,    -9, -1000,    25,
 -1000,    20,    17,    36,    35, -1000, -1000, -1000,    27,   -12,
 -1000,    31, -1000 };
static yytabelem yypgo[]={

     0,    40,    39,    33,     3,    38,     1 };
static yytabelem yyr1[]={

     0,     1,     1,     2,     2,     2,     3,     3,     3,     4,
     4,     4,     5,     5,     6,     6,     6,     6,     6,     6,
     6 };
static yytabelem yyr2[]={

     0,     0,     4,     5,     5,     5,     9,     5,     7,     1,
     3,     7,     7,     3,     3,     3,     3,     7,     7,    11,
     3 };
static yytabelem yychk[]={

 -1000,    -1,    -2,     2,    -3,   256,     9,    13,     4,    13,
    13,    10,    -4,    -5,     2,    -6,     3,     1,    -3,    -4,
    10,     6,    11,    13,    14,     5,    -4,    -6,     2,     1,
     1,    14,     1 };
static yytabelem yydef[]={

     1,    -2,     2,     0,     0,     0,     0,     3,     9,     4,
     5,     7,     0,    10,    14,    13,    15,    16,    20,     0,
     8,     9,     0,     0,     0,     6,    11,    12,    14,    17,
    18,     0,    19 };
typedef struct { char *t_name; int t_val; } yytoktype;
#ifndef YYDEBUG
#	define YYDEBUG	0	/* don't allow debugging */
#endif

#if YYDEBUG

yytoktype yytoks[] =
{
	"INTEGER",	1,
	"WORD",	2,
	"STRING",	3,
	"PERIOD",	13,
	"OPEN",	4,
	"CLOSE",	5,
	"COMMA",	6,
	"NEWLINE",	7,
	"ERROR",	8,
	"OPEN_SQUARE",	9,
	"CLOSE_SQUARE",	10,
	"EQUALS",	11,
	"EXP",	14,
	"-unknown-",	-1	/* ends search */
};

char * yyreds[] =
{
	"-no such reduction-",
      "commands : /* empty */",
      "commands : commands command",
      "command : WORD PERIOD",
      "command : expr PERIOD",
      "command : error PERIOD",
      "expr : WORD OPEN arglist CLOSE",
      "expr : OPEN_SQUARE CLOSE_SQUARE",
      "expr : OPEN_SQUARE arglist CLOSE_SQUARE",
      "arglist : /* empty */",
      "arglist : arg",
      "arglist : arg COMMA arglist",
      "arg : WORD EQUALS arg1",
      "arg : arg1",
      "arg1 : WORD",
      "arg1 : STRING",
      "arg1 : INTEGER",
      "arg1 : INTEGER PERIOD INTEGER",
      "arg1 : INTEGER EXP INTEGER",
      "arg1 : INTEGER PERIOD INTEGER EXP INTEGER",
      "arg1 : expr",
};
#endif /* YYDEBUG */
/*
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0
 */
/* @(#)yaccpar	1.3  com/cmd/lang/yacc,3.1, 9/7/89 18:46:37 */
/*
** Skeleton parser driver for yacc output
*/

/*
** yacc user known macros and defines
*/
#ifdef YYSPLIT
#   define YYERROR	return(-2)
#else
#   define YYERROR	goto yyerrlab
#endif

#define YYACCEPT	return(0)
#define YYABORT		return(1)
#define YYBACKUP( newtoken, newvalue )\
{\
	if ( yychar >= 0 || ( yyr2[ yytmp ] >> 1 ) != 1 )\
	{\
		yyerror( "syntax error - cannot backup" );\
		goto yyerrlab;\
	}\
	yychar = newtoken;\
	yystate = *yyps;\
	yylval = newvalue;\
	goto yynewstate;\
}
#define YYRECOVERING()	(!!yyerrflag)
#ifndef YYDEBUG
#	define YYDEBUG	1	/* make debugging available */
#endif

/*
** user known globals
*/
int yydebug;			/* set to 1 to get debugging */

/*
** driver internal defines
*/
#define YYFLAG		(-1000)

#ifdef YYSPLIT
#   define YYSCODE { \
			extern int (*yyf[])(); \
			register int yyret; \
			if (yyf[yytmp]) \
			    if ((yyret=(*yyf[yytmp])()) == -2) \
				    goto yyerrlab; \
				else if (yyret>=0) return(yyret); \
		   }
#endif

/*
** global variables used by the parser
*/
YYSTYPE yyv[ YYMAXDEPTH ];	/* value stack */
int yys[ YYMAXDEPTH ];		/* state stack */

YYSTYPE *yypv;			/* top of value stack */
YYSTYPE *yypvt;			/* top of value stack for $vars */
int *yyps;			/* top of state stack */

int yystate;			/* current state */
int yytmp;			/* extra var (lasts between blocks) */

int yynerrs;			/* number of errors */
int yyerrflag;			/* error recovery flag */
int yychar;			/* current input token number */



/*
** yyparse - return 0 if worked, 1 if syntax error not recovered from
*/
int
yyparse()
{
	/*
	** Initialize externals - yyparse may be called more than once
	*/
	yypv = &yyv[-1];
	yyps = &yys[-1];
	yystate = 0;
	yytmp = 0;
	yynerrs = 0;
	yyerrflag = 0;
	yychar = -1;

	goto yystack;
	{
		register YYSTYPE *yy_pv;	/* top of value stack */
		register int *yy_ps;		/* top of state stack */
		register int yy_state;		/* current state */
		register int  yy_n;		/* internal state number info */

		/*
		** get globals into registers.
		** branch to here only if YYBACKUP was called.
		*/
	yynewstate:
		yy_pv = yypv;
		yy_ps = yyps;
		yy_state = yystate;
		goto yy_newstate;

		/*
		** get globals into registers.
		** either we just started, or we just finished a reduction
		*/
	yystack:
		yy_pv = yypv;
		yy_ps = yyps;
		yy_state = yystate;

		/*
		** top of for (;;) loop while no reductions done
		*/
	yy_stack:
		/*
		** put a state and value onto the stacks
		*/
#if YYDEBUG
		/*
		** if debugging, look up token value in list of value vs.
		** name pairs.  0 and negative (-1) are special values.
		** Note: linear search is used since time is not a real
		** consideration while debugging.
		*/
		if ( yydebug )
		{
			register int yy_i;

			printf( "State %d, token ", yy_state );
			if ( yychar == 0 )
				printf( "end-of-file\n" );
			else if ( yychar < 0 )
				printf( "-none-\n" );
			else
			{
				for ( yy_i = 0; yytoks[yy_i].t_val >= 0;
					yy_i++ )
				{
					if ( yytoks[yy_i].t_val == yychar )
						break;
				}
				printf( "%s\n", yytoks[yy_i].t_name );
			}
		}
#endif /* YYDEBUG */
		if ( ++yy_ps >= &yys[ YYMAXDEPTH ] )	/* room on stack? */
		{
			yyerror( "yacc stack overflow" );
			YYABORT;
		}
		*yy_ps = yy_state;
		*++yy_pv = yyval;

		/*
		** we have a new state - find out what to do
		*/
	yy_newstate:
		if ( ( yy_n = yypact[ yy_state ] ) <= YYFLAG )
			goto yydefault;		/* simple state */
#if YYDEBUG
		/*
		** if debugging, need to mark whether new token grabbed
		*/
		yytmp = yychar < 0;
#endif
		if ( ( yychar < 0 ) && ( ( yychar = yylex() ) < 0 ) )
			yychar = 0;		/* reached EOF */
#if YYDEBUG
		if ( yydebug && yytmp )
		{
			register int yy_i;

			printf( "Received token " );
			if ( yychar == 0 )
				printf( "end-of-file\n" );
			else if ( yychar < 0 )
				printf( "-none-\n" );
			else
			{
				for ( yy_i = 0; yytoks[yy_i].t_val >= 0;
					yy_i++ )
				{
					if ( yytoks[yy_i].t_val == yychar )
						break;
				}
				printf( "%s\n", yytoks[yy_i].t_name );
			}
		}
#endif /* YYDEBUG */
		if ( ( ( yy_n += yychar ) < 0 ) || ( yy_n >= YYLAST ) )
			goto yydefault;
		if ( yychk[ yy_n = yyact[ yy_n ] ] == yychar )	/*valid shift*/
		{
			yychar = -1;
			yyval = yylval;
			yy_state = yy_n;
			if ( yyerrflag > 0 )
				yyerrflag--;
			goto yy_stack;
		}

	yydefault:
		if ( ( yy_n = yydef[ yy_state ] ) == -2 )
		{
#if YYDEBUG
			yytmp = yychar < 0;
#endif
			if ( ( yychar < 0 ) && ( ( yychar = yylex() ) < 0 ) )
				yychar = 0;		/* reached EOF */
#if YYDEBUG
			if ( yydebug && yytmp )
			{
				register int yy_i;

				printf( "Received token " );
				if ( yychar == 0 )
					printf( "end-of-file\n" );
				else if ( yychar < 0 )
					printf( "-none-\n" );
				else
				{
					for ( yy_i = 0;
						yytoks[yy_i].t_val >= 0;
						yy_i++ )
					{
						if ( yytoks[yy_i].t_val
							== yychar )
						{
							break;
						}
					}
					printf( "%s\n", yytoks[yy_i].t_name );
				}
			}
#endif /* YYDEBUG */
			/*
			** look through exception table
			*/
			{
				register int *yyxi = yyexca;

				while ( ( *yyxi != -1 ) ||
					( yyxi[1] != yy_state ) )
				{
					yyxi += 2;
				}
				while ( ( *(yyxi += 2) >= 0 ) &&
					( *yyxi != yychar ) )
					;
				if ( ( yy_n = yyxi[1] ) < 0 )
					YYACCEPT;
			}
		}

		/*
		** check for syntax error
		*/
		if ( yy_n == 0 )	/* have an error */
		{
			/* no worry about speed here! */
			switch ( yyerrflag )
			{
			case 0:		/* new error */
				yyerror( "syntax error" );
				goto skip_init;
			yyerrlab:
				/*
				** get globals into registers.
				** we have a user generated syntax type error
				*/
				yy_pv = yypv;
				yy_ps = yyps;
				yy_state = yystate;
				yynerrs++;
			skip_init:
			case 1:
			case 2:		/* incompletely recovered error */
					/* try again... */
				yyerrflag = 3;
				/*
				** find state where "error" is a legal
				** shift action
				*/
				while ( yy_ps >= yys )
				{
					yy_n = yypact[ *yy_ps ] + YYERRCODE;
					if ( yy_n >= 0 && yy_n < YYLAST &&
						yychk[yyact[yy_n]] == YYERRCODE)					{
						/*
						** simulate shift of "error"
						*/
						yy_state = yyact[ yy_n ];
						goto yy_stack;
					}
					/*
					** current state has no shift on
					** "error", pop stack
					*/
#if YYDEBUG
#	define _POP_ "Error recovery pops state %d, uncovers state %d\n"
					if ( yydebug )
						printf( _POP_, *yy_ps,
							yy_ps[-1] );
#	undef _POP_
#endif
					yy_ps--;
					yy_pv--;
				}
				/*
				** there is no state on stack with "error" as
				** a valid shift.  give up.
				*/
				YYABORT;
			case 3:		/* no shift yet; eat a token */
#if YYDEBUG
				/*
				** if debugging, look up token in list of
				** pairs.  0 and negative shouldn't occur,
				** but since timing doesn't matter when
				** debugging, it doesn't hurt to leave the
				** tests here.
				*/
				if ( yydebug )
				{
					register int yy_i;

					printf( "Error recovery discards " );
					if ( yychar == 0 )
						printf( "token end-of-file\n" );
					else if ( yychar < 0 )
						printf( "token -none-\n" );
					else
					{
						for ( yy_i = 0;
							yytoks[yy_i].t_val >= 0;
							yy_i++ )
						{
							if ( yytoks[yy_i].t_val
								== yychar )
							{
								break;
							}
						}
						printf( "token %s\n",
							yytoks[yy_i].t_name );
					}
				}
#endif /* YYDEBUG */
				if ( yychar == 0 )	/* reached EOF. quit */
					YYABORT;
				yychar = -1;
				goto yy_newstate;
			}
		}/* end if ( yy_n == 0 ) */
		/*
		** reduction by production yy_n
		** put stack tops, etc. so things right after switch
		*/
#if YYDEBUG
		/*
		** if debugging, print the string that is the user's
		** specification of the reduction which is just about
		** to be done.
		*/
		if ( yydebug )
			printf( "Reduce by (%d) \"%s\"\n",
				yy_n, yyreds[ yy_n ] );
#endif
		yytmp = yy_n;			/* value to switch over */
		yypvt = yy_pv;			/* $vars top of value stack */
		/*
		** Look in goto table for next state
		** Sorry about using yy_state here as temporary
		** register variable, but why not, if it works...
		** If yyr2[ yy_n ] doesn't have the low order bit
		** set, then there is no action to be done for
		** this reduction.  So, no saving & unsaving of
		** registers done.  The only difference between the
		** code just after the if and the body of the if is
		** the goto yy_stack in the body.  This way the test
		** can be made before the choice of what to do is needed.
		*/
		{
			/* length of production doubled with extra bit */
			register int yy_len = yyr2[ yy_n ];

			if ( !( yy_len & 01 ) )
			{
				yy_len >>= 1;
				yyval = ( yy_pv -= yy_len )[1];	/* $$ = $1 */
				yy_state = yypgo[ yy_n = yyr1[ yy_n ] ] +
					*( yy_ps -= yy_len ) + 1;
				if ( yy_state >= YYLAST ||
					yychk[ yy_state =
					yyact[ yy_state ] ] != -yy_n )
				{
					yy_state = yyact[ yypgo[ yy_n ] ];
				}
				goto yy_stack;
			}
			yy_len >>= 1;
			yyval = ( yy_pv -= yy_len )[1];	/* $$ = $1 */
			yy_state = yypgo[ yy_n = yyr1[ yy_n ] ] +
				*( yy_ps -= yy_len ) + 1;
			if ( yy_state >= YYLAST ||
				yychk[ yy_state = yyact[ yy_state ] ] != -yy_n )
			{
				yy_state = yyact[ yypgo[ yy_n ] ];
			}
		}
					/* save until reenter driver code */
		yystate = yy_state;
		yyps = yy_ps;
		yypv = yy_pv;
	}
	/*
	** code supplied by user is placed in this switch
	*/

		switch(yytmp){

case 3:
# line 68 "parser.y"
{process_command(proio_cons(make_word(yypvt[-1].s), NULL)); free(yypvt[-1].s);} /*NOTREACHED*/ break;
case 4:
# line 70 "parser.y"
{process_command(yypvt[-1].s);} /*NOTREACHED*/ break;
case 5:
# line 72 "parser.y"
{syntax_error("Unrecognized command.");} /*NOTREACHED*/ break;
case 6:
# line 76 "parser.y"
{yyval.s = proio_cons(make_word(yypvt[-3].s), yypvt[-1].s); free(yypvt[-3].s);} /*NOTREACHED*/ break;
case 7:
# line 78 "parser.y"
{yyval.s = proio_cons(NULL, NULL);} /*NOTREACHED*/ break;
case 8:
# line 80 "parser.y"
{yyval.s = yypvt[-1].s; } /*NOTREACHED*/ break;
case 9:
# line 84 "parser.y"
{yyval.s = NULL;} /*NOTREACHED*/ break;
case 10:
# line 86 "parser.y"
{yyval.s = proio_cons(yypvt[-0].s, NULL);} /*NOTREACHED*/ break;
case 11:
# line 89 "parser.y"
{yyval.s = proio_cons(yypvt[-2].s, yypvt[-0].s);} /*NOTREACHED*/ break;
case 12:
# line 93 "parser.y"
{yyval.s = proio_cons(make_word("="), proio_cons(make_word(yypvt[-2].s), proio_cons(yypvt[-0].s, NULL)));
                         free(yypvt[-2].s); } /*NOTREACHED*/ break;
case 13:
# line 96 "parser.y"
{yyval.s = yypvt[-0].s; } /*NOTREACHED*/ break;
case 14:
# line 99 "parser.y"
{yyval.s = make_word(yypvt[-0].s); free(yypvt[-0].s);} /*NOTREACHED*/ break;
case 15:
# line 101 "parser.y"
{yyval.s = make_string(yypvt[-0].s); free(yypvt[-0].s);} /*NOTREACHED*/ break;
case 16:
# line 103 "parser.y"
{yyval.s = make_integer(yypvt[-0].s); free(yypvt[-0].s);} /*NOTREACHED*/ break;
case 17:
# line 105 "parser.y"
{yyval.s = make_real(yypvt[-2].s, yypvt[-0].s); free(yypvt[-2].s); free(yypvt[-0].s); } /*NOTREACHED*/ break;
case 18:
# line 107 "parser.y"
{yyval.s = make_exp(yypvt[-2].s, yypvt[-0].s); free(yypvt[-2].s); free(yypvt[-0].s); } /*NOTREACHED*/ break;
case 19:
# line 110 "parser.y"
{yyval.s = make_exp2(yypvt[-4].s, yypvt[-2].s, yypvt[-0].s); free(yypvt[-4].s); free(yypvt[-2].s);
                                                                  free(yypvt[-0].s); } /*NOTREACHED*/ break;
case 20:
# line 114 "parser.y"
{yyval.s = yypvt[-0].s;} /*NOTREACHED*/ break;
}


	goto yystack;		/* reset registers in driver code */
}
