extern char *malloc(), *realloc();

# line 2 "gen.y"

#include "global_parse.h"
#include "result_types.h"
#include "file_dict.h"
#include "proc_dict.h"
#include "class_dict.h"
#include "browser_pars.h"
#include "browser_util.h"
#include <stdio.h>


/*------------------------------------------------------------------------------
*/
char yy_class_name[256];


static ScopeType yy_scope;

#define S_PRECEDENCE       10
#define P_PRECEDENCE       10
#define A_PRECEDENCE       20
#define MAX_PRECEDENCE     30

#define parentheses(ref_prec, inter, result) \
          if (inter.precedence < ref_prec)   \
            strcat(result.text, "(");        \
          strcat(result.text, inter.text);   \
          if (inter.precedence < ref_prec)   \
            strcat(result.text, ")");        \
          result.precedence = ref_prec;


#define YYDEBUG 1


# define ARRAY_TOK 257
# define UNSIGNED_TOK 258
# define SIGNED_TOK 259
# define CHAR_TOK 260
# define SHORT_TOK 261
# define INT_TOK 262
# define LONG_TOK 263
# define STRUCT_TOK 264
# define UNION_TOK 265
# define CLASS_TOK 266
# define CONST_TOK 267
# define IDENT_TOK 268
# define CPLUS_TOK 269
# define C_TOK 270
# define DECL_TOK 271
# define SYNC_TOK 272
# define PUBLIC_TOK 273
# define PROTECTED_TOK 274
# define PRIVATE_TOK 275
# define LEX_ERROR_TOK 276
#define yyclearin yychar = -1
#define yyerrok yyerrflag = 0
extern int yychar;
extern int yyerrflag;
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 150
#endif
#ifndef YYSTYPE
#define YYSTYPE int
#endif
YYSTYPE yylval, yyval;
# define YYERRCODE 256

# line 315 "gen.y"


int yyerror(error_msg)
    char* error_msg;
{
	if ( OpVerbose () == True ) {
	  fprintf(stderr, "warning >>>>>>  Internal Parser Error at line %d\n", line_count);
	  fprintf(stderr, "warning    >>  %s\n", error_msg);
	}
}


int browser_yyparse(file_name)
    char* file_name;
{
  FILE* file;
  int   result;

  result = 1;
  *yy_class_name = '\0';
  file = fopen(file_name, "r");
  if (file != Null) {
    flex_init(file);
    result = yyparse();
    fclose(file);
  }
  else
    fprintf(stderr, "\n>>>>>>  Unable to open file %s\n", file_name);
  return(result);
}int yyexca[] ={
-1, 1,
	0, -1,
	-2, 0,
	};
# define YYNPROD 64
# define YYLAST 250
int yyact[]={

    76,     9,     6,     7,    73,     4,    72,    71,    53,     8,
    60,    33,    61,    60,    59,    61,    48,    59,    60,    60,
    61,    61,    59,    59,    19,    32,    31,    78,    92,    80,
    34,    29,    56,    77,    10,    39,    40,    38,    41,    42,
    43,    44,    45,    46,    37,    47,    39,    40,    38,    41,
    42,    43,    44,    45,    46,    35,    47,    67,    68,    69,
    70,    63,    64,    65,    66,    97,    20,    89,    55,    88,
    55,    54,    55,    95,    55,    51,    50,    17,    18,    36,
    11,    74,    75,     5,    12,    49,     3,     2,     1,     0,
    81,     0,    82,    84,    86,    87,    30,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,    52,     0,
     0,    79,     0,     0,     0,     0,    90,    62,    91,    49,
     0,     0,    93,    94,     0,     0,    96,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,    25,    26,    24,    27,    21,    22,
    23,    28,    25,    26,    24,    27,    21,    22,    23,    28,
    25,    26,    24,    27,    21,    22,    23,    28,    25,    26,
    24,    27,    21,    22,    23,    28,    16,     0,     0,    16,
     0,    13,    14,    15,    13,    14,    15,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,    85,
    58,     0,    83,    58,    80,     0,     0,    57,    58,    58 };
int yypact[]={

 -1000,  -267, -1000, -1000, -1000,   -89,    37, -1000, -1000, -1000,
  -101,   -92, -1000,  -242,  -243,  -257, -1000,  -223,  -109, -1000,
 -1000, -1000, -1000, -1000,    36,    35, -1000, -1000, -1000,  -117,
 -1000, -1000, -1000, -1000,    30, -1000,   -20,  -212, -1000,  -199,
  -203, -1000, -1000, -1000,  -261,  -262,  -264, -1000, -1000, -1000,
  -223,  -223,  -125, -1000,  -240,  -223,  -228,   -19, -1000,   -25,
   -28,   -19,   -19, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
 -1000, -1000, -1000, -1000,    28,    26, -1000, -1000, -1000, -1000,
 -1000,  -228,  -228,   -19,  -228,   -19,   -13,  -228,  -240,  -240,
  -228,  -228,    33, -1000, -1000,  -223,    24, -1000 };
int yypgo[]={

     0,    88,    87,    86,    83,    78,    80,    84,    66,    30,
    33,    55,    79,    32 };
int yyr1[]={

     0,     1,     1,     1,     1,     2,     2,     2,     2,     4,
     6,     6,     7,     7,     7,     7,     5,     5,     8,     8,
     8,     8,     8,     8,     8,     8,     3,     3,     3,    10,
    10,     9,     9,     9,    11,    11,    11,    12,    12,    12,
    12,    12,    12,    12,    12,    12,    12,    12,    12,    12,
    12,    12,    12,    12,    12,    13,    13,    13,    13,    13,
    13,    13,    13,    13 };
int yyr2[]={

     0,     1,     4,     4,     4,     8,     6,    10,     8,     3,
     2,     4,     5,     5,     5,     3,     2,     4,     3,     3,
     3,    11,    11,     2,     2,     3,    11,     3,     3,     1,
     3,     1,     3,     7,     5,     7,     7,     3,     5,     5,
     3,     5,     5,     3,     5,     5,     3,     3,     3,     5,
     5,     5,     5,     5,     3,     1,     3,     5,     7,     5,
     7,     5,    13,     7 };
int yychk[]={

 -1000,    -1,    -2,    -3,   272,    -4,   269,   270,   276,   268,
   123,    -6,    -7,   273,   274,   275,   268,    40,    -5,   125,
    -8,   273,   274,   275,   271,   269,   270,   272,   276,   123,
    -7,   268,   268,   268,    -9,   -11,   -12,   267,   260,   258,
   259,   261,   262,   263,   264,   265,   266,   268,   125,    -8,
    40,    40,    -5,   125,    41,    44,   -13,   267,   268,    42,
    38,    40,   -12,   260,   261,   262,   263,   260,   261,   262,
   263,   268,   268,   268,    -9,    -9,   125,   -10,   267,   -11,
   257,   -13,   -13,   267,   -13,   267,   -13,   -13,    41,    41,
   -13,   -13,    41,   -10,   -10,    40,    -9,    41 };
int yydef[]={

     1,    -2,     2,     3,     4,     0,     0,    27,    28,     9,
     0,     0,    10,     0,     0,     0,    15,    31,     0,     6,
    16,    18,    19,    20,     0,     0,    23,    24,    25,     0,
    11,    12,    13,    14,     0,    32,    55,     0,    37,    46,
    47,    40,    43,    48,     0,     0,     0,    54,     5,    17,
    31,    31,     0,     8,    29,     0,    34,    55,    56,    55,
    55,    55,    55,    38,    41,    44,    49,    39,    42,    45,
    50,    51,    52,    53,     0,     0,     7,    26,    30,    33,
    61,    36,    57,    55,    59,    55,     0,    35,    29,    29,
    58,    60,    63,    21,    22,    31,     0,    62 };
typedef struct { char *t_name; int t_val; } yytoktype;
#ifndef YYDEBUG
#	define YYDEBUG	0	/* don't allow debugging */
#endif

#if YYDEBUG

yytoktype yytoks[] =
{
	"*",	42,
	"&",	38,
	"ARRAY_TOK",	257,
	"UNSIGNED_TOK",	258,
	"SIGNED_TOK",	259,
	"CHAR_TOK",	260,
	"SHORT_TOK",	261,
	"INT_TOK",	262,
	"LONG_TOK",	263,
	"STRUCT_TOK",	264,
	"UNION_TOK",	265,
	"CLASS_TOK",	266,
	"CONST_TOK",	267,
	"IDENT_TOK",	268,
	"CPLUS_TOK",	269,
	"C_TOK",	270,
	"DECL_TOK",	271,
	"SYNC_TOK",	272,
	"PUBLIC_TOK",	273,
	"PROTECTED_TOK",	274,
	"PRIVATE_TOK",	275,
	"LEX_ERROR_TOK",	276,
	"-unknown-",	-1	/* ends search */
};

char * yyreds[] =
{
	"-no such reduction-",
	"file : /* empty */",
	"file : file class_decl",
	"file : file procedure_impl",
	"file : file SYNC_TOK",
	"class_decl : class_name '{' member_list '}'",
	"class_decl : class_name '{' '}'",
	"class_decl : class_name parent_list '{' member_list '}'",
	"class_decl : class_name parent_list '{' '}'",
	"class_name : IDENT_TOK",
	"parent_list : parent",
	"parent_list : parent_list parent",
	"parent : PUBLIC_TOK IDENT_TOK",
	"parent : PROTECTED_TOK IDENT_TOK",
	"parent : PRIVATE_TOK IDENT_TOK",
	"parent : IDENT_TOK",
	"member_list : member_decl",
	"member_list : member_list member_decl",
	"member_decl : PUBLIC_TOK",
	"member_decl : PROTECTED_TOK",
	"member_decl : PRIVATE_TOK",
	"member_decl : DECL_TOK '(' arguments_list ')' trail_decl",
	"member_decl : CPLUS_TOK '(' arguments_list ')' trail_decl",
	"member_decl : C_TOK",
	"member_decl : SYNC_TOK",
	"member_decl : LEX_ERROR_TOK",
	"procedure_impl : CPLUS_TOK '(' arguments_list ')' trail_decl",
	"procedure_impl : C_TOK",
	"procedure_impl : LEX_ERROR_TOK",
	"trail_decl : /* empty */",
	"trail_decl : CONST_TOK",
	"arguments_list : /* empty */",
	"arguments_list : argument",
	"arguments_list : arguments_list ',' argument",
	"argument : type declarator",
	"argument : CONST_TOK type declarator",
	"argument : type CONST_TOK declarator",
	"type : CHAR_TOK",
	"type : UNSIGNED_TOK CHAR_TOK",
	"type : SIGNED_TOK CHAR_TOK",
	"type : SHORT_TOK",
	"type : UNSIGNED_TOK SHORT_TOK",
	"type : SIGNED_TOK SHORT_TOK",
	"type : INT_TOK",
	"type : UNSIGNED_TOK INT_TOK",
	"type : SIGNED_TOK INT_TOK",
	"type : UNSIGNED_TOK",
	"type : SIGNED_TOK",
	"type : LONG_TOK",
	"type : UNSIGNED_TOK LONG_TOK",
	"type : SIGNED_TOK LONG_TOK",
	"type : STRUCT_TOK IDENT_TOK",
	"type : UNION_TOK IDENT_TOK",
	"type : CLASS_TOK IDENT_TOK",
	"type : IDENT_TOK",
	"declarator : /* empty */",
	"declarator : IDENT_TOK",
	"declarator : '*' declarator",
	"declarator : '*' CONST_TOK declarator",
	"declarator : '&' declarator",
	"declarator : '&' CONST_TOK declarator",
	"declarator : declarator ARRAY_TOK",
	"declarator : '(' declarator ')' '(' arguments_list ')'",
	"declarator : '(' declarator ')'",
};
#endif /* YYDEBUG */
#line 1 "/usr/lib/yaccpar"
/*	@(#)yaccpar 1.10 89/04/04 SMI; from S5R3 1.10	*/

/*
** Skeleton parser driver for yacc output
*/

/*
** yacc user known macros and defines
*/
#define YYERROR		goto yyerrlab
#define YYACCEPT	{ free(yys); free(yyv); return(0); }
#define YYABORT		{ free(yys); free(yyv); return(1); }
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

/*
** static variables used by the parser
*/
static YYSTYPE *yyv;			/* value stack */
static int *yys;			/* state stack */

static YYSTYPE *yypv;			/* top of value stack */
static int *yyps;			/* top of state stack */

static int yystate;			/* current state */
static int yytmp;			/* extra var (lasts between blocks) */

int yynerrs;			/* number of errors */

int yyerrflag;			/* error recovery flag */
int yychar;			/* current input token number */


/*
** yyparse - return 0 if worked, 1 if syntax error not recovered from
*/
int
yyparse()
{
	register YYSTYPE *yypvt;	/* top of value stack for $vars */
	unsigned yymaxdepth = YYMAXDEPTH;

	/*
	** Initialize externals - yyparse may be called more than once
	*/
	yyv = (YYSTYPE*)malloc(yymaxdepth*sizeof(YYSTYPE));
	yys = (int*)malloc(yymaxdepth*sizeof(int));
	if (!yyv || !yys)
	{
		yyerror( "out of memory" );
		return(1);
	}
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

			(void)printf( "State %d, token ", yy_state );
			if ( yychar == 0 )
				(void)printf( "end-of-file\n" );
			else if ( yychar < 0 )
				(void)printf( "-none-\n" );
			else
			{
				for ( yy_i = 0; yytoks[yy_i].t_val >= 0;
					yy_i++ )
				{
					if ( yytoks[yy_i].t_val == yychar )
						break;
				}
				(void)printf( "%s\n", yytoks[yy_i].t_name );
			}
		}
#endif /* YYDEBUG */
		if ( ++yy_ps >= &yys[ yymaxdepth ] )	/* room on stack? */
		{
			/*
			** reallocate and recover.  Note that pointers
			** have to be reset, or bad things will happen
			*/
			int yyps_index = (yy_ps - yys);
			int yypv_index = (yy_pv - yyv);
			int yypvt_index = (yypvt - yyv);
			yymaxdepth += YYMAXDEPTH;
			yyv = (YYSTYPE*)realloc((char*)yyv,
				yymaxdepth * sizeof(YYSTYPE));
			yys = (int*)realloc((char*)yys,
				yymaxdepth * sizeof(int));
			if (!yyv || !yys)
			{
				yyerror( "yacc stack overflow" );
				return(1);
			}
			yy_ps = yys + yyps_index;
			yy_pv = yyv + yypv_index;
			yypvt = yyv + yypvt_index;
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

			(void)printf( "Received token " );
			if ( yychar == 0 )
				(void)printf( "end-of-file\n" );
			else if ( yychar < 0 )
				(void)printf( "-none-\n" );
			else
			{
				for ( yy_i = 0; yytoks[yy_i].t_val >= 0;
					yy_i++ )
				{
					if ( yytoks[yy_i].t_val == yychar )
						break;
				}
				(void)printf( "%s\n", yytoks[yy_i].t_name );
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

				(void)printf( "Received token " );
				if ( yychar == 0 )
					(void)printf( "end-of-file\n" );
				else if ( yychar < 0 )
					(void)printf( "-none-\n" );
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
					(void)printf( "%s\n", yytoks[yy_i].t_name );
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
						(void)printf( _POP_, *yy_ps,
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

					(void)printf( "Error recovery discards " );
					if ( yychar == 0 )
						(void)printf( "token end-of-file\n" );
					else if ( yychar < 0 )
						(void)printf( "token -none-\n" );
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
						(void)printf( "token %s\n",
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
			(void)printf( "Reduce by (%d) \"%s\"\n",
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
	switch( yytmp )
	{
		
case 1:
# line 56 "gen.y"
{ yydebug = 0; } break;
case 9:
# line 76 "gen.y"
{
      strcpy(yy_class_name, yypvt[-0].text);
      if (add_class_decl(yypvt[-0].text, yypvt[-0].position) == BERROR)
        YYABORT;
      yy_scope = PRIVATE_TOK;
    } break;
case 12:
# line 96 "gen.y"
{
      if (add_parent(yy_class_name, yypvt[-0].text, PUBLIC_SCOPE) == BERROR)
        YYABORT;
    } break;
case 13:
# line 100 "gen.y"
{
      if (add_parent(yy_class_name, yypvt[-0].text, PROTECTED_SCOPE) == BERROR)
        YYABORT;
    } break;
case 14:
# line 104 "gen.y"
{
      if (add_parent(yy_class_name, yypvt[-0].text, PRIVATE_SCOPE) == BERROR)
        YYABORT;
    } break;
case 15:
# line 108 "gen.y"
{
      if (add_parent(yy_class_name, yypvt[-0].text, PRIVATE_SCOPE) == BERROR)
        YYABORT;
    } break;
case 18:
# line 126 "gen.y"
{
      yy_scope = PUBLIC_SCOPE;
    } break;
case 19:
# line 129 "gen.y"
{
      yy_scope = PROTECTED_SCOPE;
    } break;
case 20:
# line 132 "gen.y"
{
      yy_scope = PRIVATE_SCOPE;
    } break;
case 21:
# line 135 "gen.y"
{
      strcpy(yyval.text, yypvt[-4].text);
      strcat(yyval.text, "(");
      strcat(yyval.text, yypvt[-2].text);
      strcat(yyval.text, ")");
      strcat(yyval.text, yypvt[-0].text);
      if (add_method_decl(yy_class_name, yyval.text,
                          yy_scope, yypvt[-4].is_virtual, yypvt[-4].position) == BERROR)
        YYABORT;
    } break;
case 22:
# line 145 "gen.y"
{
      strcpy(yyval.text, yypvt[-4].text);
      strcat(yyval.text, "(");
      strcat(yyval.text, yypvt[-2].text);
      strcat(yyval.text, ")");
      strcat(yyval.text, yypvt[-0].text);
      if (add_method_decl(yy_class_name, yyval.text,
                          yy_scope, yypvt[-4].is_virtual, yypvt[-4].position) == BERROR)
        YYABORT;
      if (add_method_impl(yy_class_name, yyval.text, yypvt[-4].position) == BERROR)
        YYABORT;
    } break;
case 25:
# line 159 "gen.y"
{
      fprintf(stderr, ">>>>>>  Arguments buffer overflow at line %d\n", yypvt[-0].position);
      YYABORT;
    } break;
case 26:
# line 169 "gen.y"
{
      strcpy(yyval.text, yypvt[-4].text);
      strcat(yyval.text, "(");
      strcat(yyval.text, yypvt[-2].text);
      strcat(yyval.text, ")");
      strcat(yyval.text, yypvt[-0].text);
      if (yy_class_name[0] == '\0') {
        if (add_proc_impl(yyval.text, CPLUS_PROC, yypvt[-4].position) == BERROR)
          YYABORT;
      }
      else {
        if (add_method_impl(yy_class_name, yyval.text, yypvt[-4].position) == BERROR)
          YYABORT;
      }
      *yy_class_name = '\0';
    } break;
case 27:
# line 185 "gen.y"
{
      strcpy(yyval.text, yypvt[-0].text);
      strcat(yyval.text, "()");
      if (add_proc_impl(yyval.text, C_PROC, yypvt[-0].position) == BERROR)
         YYABORT;
      *yy_class_name = '\0';
    } break;
case 28:
# line 192 "gen.y"
{
      fprintf(stderr, ">>>>>>  Arguments buffer overflow at line %d\n", yypvt[-0].position);
      YYABORT;
    } break;
case 29:
# line 202 "gen.y"
{ yyval.text[0] = '\0';         } break;
case 30:
# line 203 "gen.y"
{ strcpy(yyval.text, " const"); } break;
case 31:
# line 210 "gen.y"
{ yyval.text[0] = '\0';        } break;
case 32:
# line 211 "gen.y"
{ strcpy(yyval.text, yypvt[-0].text); } break;
case 33:
# line 212 "gen.y"
{
                                   strcpy(yyval.text, yypvt[-2].text);
                                   strcat(yyval.text, ", ");
                                   strcat(yyval.text, yypvt[-0].text);
                                 } break;
case 34:
# line 223 "gen.y"
{ 
      strcpy(yyval.text, yypvt[-1].text);
      strcat(yyval.text, yypvt[-0].text);
    } break;
case 35:
# line 227 "gen.y"
{ 
      strcpy(yyval.text, "const ");
      strcat(yyval.text, yypvt[-1].text);
      strcat(yyval.text, yypvt[-0].text);
    } break;
case 36:
# line 232 "gen.y"
{ 
      strcpy(yyval.text, "const ");
      strcat(yyval.text, yypvt[-2].text);
      strcat(yyval.text, yypvt[-0].text);
    } break;
case 37:
# line 243 "gen.y"
{ strcpy(yyval.text, "char"); } break;
case 38:
# line 244 "gen.y"
{ strcpy(yyval.text, "unsigned char"); } break;
case 39:
# line 245 "gen.y"
{ strcpy(yyval.text, "char"); } break;
case 40:
# line 246 "gen.y"
{ strcpy(yyval.text, "short"); } break;
case 41:
# line 247 "gen.y"
{ strcpy(yyval.text, "unsigned short"); } break;
case 42:
# line 248 "gen.y"
{ strcpy(yyval.text, "short"); } break;
case 43:
# line 249 "gen.y"
{ strcpy(yyval.text, "int"); } break;
case 44:
# line 250 "gen.y"
{ strcpy(yyval.text, "unsigned int"); } break;
case 45:
# line 251 "gen.y"
{ strcpy(yyval.text, "int"); } break;
case 46:
# line 252 "gen.y"
{ strcpy(yyval.text, "unsigned int"); } break;
case 47:
# line 253 "gen.y"
{ strcpy(yyval.text, "int"); } break;
case 48:
# line 254 "gen.y"
{ strcpy(yyval.text, "long"); } break;
case 49:
# line 255 "gen.y"
{ strcpy(yyval.text, "unsigned long"); } break;
case 50:
# line 256 "gen.y"
{ strcpy(yyval.text, "long"); } break;
case 51:
# line 257 "gen.y"
{ strcpy(yyval.text, yypvt[-0].text); } break;
case 52:
# line 258 "gen.y"
{ strcpy(yyval.text, yypvt[-0].text); } break;
case 53:
# line 259 "gen.y"
{ strcpy(yyval.text, yypvt[-0].text); } break;
case 54:
# line 260 "gen.y"
{ strcpy(yyval.text, yypvt[-0].text); } break;
case 55:
# line 267 "gen.y"
{ 
      yyval.text[0]    = '\0';
      yyval.precedence = MAX_PRECEDENCE;
    } break;
case 56:
# line 271 "gen.y"
{ 
      yyval.text[0]    = '\0';
      yyval.precedence = MAX_PRECEDENCE;
    } break;
case 57:
# line 275 "gen.y"
{
      strcpy(yyval.text, "*");
      parentheses(S_PRECEDENCE, yypvt[-0], yyval)
    } break;
case 58:
# line 279 "gen.y"
{
      strcpy(yyval.text, "*const ");
      parentheses(S_PRECEDENCE, yypvt[-1], yyval)
    } break;
case 59:
# line 283 "gen.y"
{
      strcpy(yyval.text, "&");
      parentheses(P_PRECEDENCE, yypvt[-0], yyval)
    } break;
case 60:
# line 287 "gen.y"
{
      strcpy(yyval.text, "&const ");
      parentheses(P_PRECEDENCE, yypvt[-1], yyval)
    } break;
case 61:
# line 291 "gen.y"
{
      yyval.text[0] = '\0';
      parentheses(A_PRECEDENCE, yypvt[-1], yyval)
      strcat(yyval.text, "[]");
    } break;
case 62:
# line 296 "gen.y"
{
      strcpy(yyval.text, "(");
      strcat(yyval.text, yypvt[-4].text);
      strcat(yyval.text, ")");
      strcat(yyval.text, " (");
      strcat(yyval.text, yypvt[-1].text);
      strcat(yyval.text, ")");
      yyval.precedence = MAX_PRECEDENCE;
    } break;
case 63:
# line 305 "gen.y"
{
      strcpy(yyval.text, yypvt[-1].text);
      yyval.precedence = yypvt[-1].precedence;
    } break;
	}
	goto yystack;		/* reset registers in driver code */
}
