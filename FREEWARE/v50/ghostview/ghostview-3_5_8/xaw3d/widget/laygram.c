
# line 2 "laygram.y"
#include    <X11/Xlib.h>
#include    <X11/Xresource.h>
#include    <stdio.h>
#include    <X11/IntrinsicP.h>
#include    <X11/cursorfont.h>
#include    <X11/StringDefs.h>

#include    <X11/Xmu/Misc.h>
#include    <X11/Xmu/Converters.h>
#include    "LayoutP.h"

static LayoutPtr    *dest;


# line 17 "laygram.y"
typedef union  {
    int		    ival;
    XrmQuark	    qval;
    BoxPtr	    bval;
    BoxParamsPtr    pval;
    GlueRec	    gval;
    LayoutDirection lval;
    ExprPtr	    eval;
    Operator	    oval;
} YYSTYPE;
# define OC 257
# define CC 258
# define OA 259
# define CA 260
# define OP 261
# define CP 262
# define NAME 263
# define NUMBER 264
# define INFINITY 265
# define VERTICAL 266
# define HORIZONTAL 267
# define EQUAL 268
# define DOLLAR 269
# define PLUS 270
# define MINUS 271
# define TIMES 272
# define DIVIDE 273
# define PERCENTOF 274
# define PERCENT 275
# define WIDTH 276
# define HEIGHT 277
# define UMINUS 278
# define UPLUS 279
#define LayYYclearin LayYYchar = -1
#define LayYYerrok LayYYerrflag = 0
extern int LayYYchar;
extern int LayYYerrflag;
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 150
#endif
YYSTYPE LayYYlval, LayYYval;
typedef int LayYYtabelem;
# define YYERRCODE 256

# line 252 "laygram.y"


LayYYwrap ()
{
    return 1;
}

LayYYsetdest (c)
    LayoutPtr	*c;
{
    dest = c;
}
static LayYYtabelem LayYYexca[] ={
-1, 1,
	0, -1,
	-2, 0,
	};
# define YYNPROD 39
# define YYLAST 128
static LayYYtabelem LayYYact[]={

    17,    62,     9,    18,    17,     4,     5,    18,    19,    13,
    12,    29,    19,    34,    33,    15,    16,    29,    17,    15,
    16,    18,    17,    60,    50,    18,    19,    13,    12,    39,
    19,    36,    17,    15,    16,    18,    53,    15,    16,    31,
    19,    41,    44,    45,    46,    24,    30,    15,    16,    42,
    43,    44,    45,    46,    23,     4,     5,    66,    63,    26,
    20,     6,    49,    38,    35,    32,    51,    10,    11,     2,
     7,    14,     1,    14,     3,    25,    22,    27,    28,    21,
     8,     0,     0,     0,     0,     0,     0,     0,    14,     0,
    40,    37,     0,     0,     0,     0,     0,     0,     0,    47,
    48,     0,     0,    54,    52,     0,     0,     0,    55,    56,
    57,    58,    59,     0,     0,    52,     0,    61,     0,     0,
     0,     0,     0,     0,    64,     0,     0,    65 };
static LayYYtabelem LayYYpact[]={

  -211, -1000, -1000,  -196, -1000, -1000,  -261,  -198,  -261,  -214,
  -200, -1000,  -239,  -239,  -258,  -217,  -224,  -257, -1000,  -232,
 -1000, -1000, -1000,  -243,  -241, -1000,  -241,  -258,  -258, -1000,
 -1000, -1000,  -221,  -257,  -257,  -258, -1000, -1000,  -247,  -229,
  -247, -1000,  -257,  -257,  -257,  -257,  -257, -1000, -1000,  -249,
  -229, -1000,  -264, -1000,  -202,  -230,  -230, -1000, -1000, -1000,
  -241, -1000, -1000, -1000,  -247,  -203, -1000 };
static LayYYtabelem LayYYpgo[]={

     0,    80,    70,    68,    76,    75,    66,    63,    62,    74,
    67,    64,    65,    72 };
static LayYYtabelem LayYYr1[]={

     0,    13,     1,     1,     1,     1,     3,     2,     2,     4,
     4,     5,     5,     7,     7,     8,     8,     6,     6,     6,
    10,    10,    10,    11,    11,    11,    11,    11,    11,    12,
    12,    12,    12,    12,    12,    12,    12,     9,     9 };
static LayYYtabelem LayYYr2[]={

     0,     3,     5,     5,     7,     3,     9,     5,     3,    15,
     1,     9,     1,     5,     1,     5,     1,     5,     3,     3,
     5,     5,     2,     5,     5,     7,     5,     3,     5,     7,
     7,     7,     7,     7,     5,     5,     2,     3,     3 };
static LayYYtabelem LayYYchk[]={

 -1000,   -13,    -3,    -9,   266,   267,   257,    -2,    -1,   263,
   -10,    -3,   271,   270,   -11,   276,   277,   261,   264,   269,
   258,    -2,    -4,   268,   259,    -5,   259,   -11,   -11,   275,
   263,   263,   -12,   271,   270,   -11,   263,   -10,    -7,   270,
    -7,   262,   270,   271,   272,   273,   274,   -12,   -12,    -8,
   271,    -6,   -11,   265,    -8,   -12,   -12,   -12,   -12,   -12,
   272,    -6,   265,   260,    -7,    -8,   260 };
static LayYYtabelem LayYYdef[]={

     0,    -2,     1,     0,    37,    38,     0,     0,     8,    10,
    12,     5,     0,     0,    22,     0,     0,     0,    27,     0,
     6,     7,     2,     0,    14,     3,    14,    20,    21,    26,
    23,    24,     0,     0,     0,    36,    28,     4,    16,     0,
    16,    25,     0,     0,     0,     0,     0,    34,    35,     0,
     0,    13,    18,    19,     0,    29,    30,    31,    32,    33,
    14,    15,    17,    11,    16,     0,     9 };
typedef struct { char *t_name; int t_val; } LayYYtoktype;
#ifndef YYDEBUG
#	define YYDEBUG	0	/* don't allow debugging */
#endif

#if YYDEBUG

LayYYtoktype LayYYtoks[] =
{
	"OC",	257,
	"CC",	258,
	"OA",	259,
	"CA",	260,
	"OP",	261,
	"CP",	262,
	"NAME",	263,
	"NUMBER",	264,
	"INFINITY",	265,
	"VERTICAL",	266,
	"HORIZONTAL",	267,
	"EQUAL",	268,
	"DOLLAR",	269,
	"PLUS",	270,
	"MINUS",	271,
	"TIMES",	272,
	"DIVIDE",	273,
	"PERCENTOF",	274,
	"PERCENT",	275,
	"WIDTH",	276,
	"HEIGHT",	277,
	"UMINUS",	278,
	"UPLUS",	279,
	"-unknown-",	-1	/* ends search */
};

char * LayYYreds[] =
{
	"-no such reduction-",
      "layout : compositebox",
      "box : NAME bothparams",
      "box : signedExpr oneparams",
      "box : NAME EQUAL signedExpr",
      "box : compositebox",
      "compositebox : orientation OC boxes CC",
      "boxes : box boxes",
      "boxes : box",
      "bothparams : OA opStretch opShrink TIMES opStretch opShrink CA",
      "bothparams : /* empty */",
      "oneparams : OA opStretch opShrink CA",
      "oneparams : /* empty */",
      "opStretch : PLUS glue",
      "opStretch : /* empty */",
      "opShrink : MINUS glue",
      "opShrink : /* empty */",
      "glue : simpleExpr INFINITY",
      "glue : simpleExpr",
      "glue : INFINITY",
      "signedExpr : MINUS simpleExpr",
      "signedExpr : PLUS simpleExpr",
      "signedExpr : simpleExpr",
      "simpleExpr : WIDTH NAME",
      "simpleExpr : HEIGHT NAME",
      "simpleExpr : OP expr CP",
      "simpleExpr : simpleExpr PERCENT",
      "simpleExpr : NUMBER",
      "simpleExpr : DOLLAR NAME",
      "expr : expr PLUS expr",
      "expr : expr MINUS expr",
      "expr : expr TIMES expr",
      "expr : expr DIVIDE expr",
      "expr : expr PERCENTOF expr",
      "expr : MINUS expr",
      "expr : PLUS expr",
      "expr : simpleExpr",
      "orientation : VERTICAL",
      "orientation : HORIZONTAL",
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
#   define YYERROR	goto LayYYerrlab
#endif

#define YYACCEPT	return(0)
#define YYABORT		return(1)
#define YYBACKUP( newtoken, newvalue )\
{\
	if ( LayYYchar >= 0 || ( LayYYr2[ LayYYtmp ] >> 1 ) != 1 )\
	{\
		LayYYerror( "syntax error - cannot backup" );\
		goto LayYYerrlab;\
	}\
	LayYYchar = newtoken;\
	LayYYstate = *LayYYps;\
	LayYYlval = newvalue;\
	goto LayYYnewstate;\
}
#define YYRECOVERING()	(!!LayYYerrflag)
#ifndef YYDEBUG
#	define YYDEBUG	1	/* make debugging available */
#endif

/*
** user known globals
*/
int LayYYdebug;			/* set to 1 to get debugging */

/*
** driver internal defines
*/
#define YYFLAG		(-1000)

#ifdef YYSPLIT
#   define YYSCODE { \
			extern int (*LayYYf[])(); \
			register int LayYYret; \
			if (LayYYf[LayYYtmp]) \
			    if ((LayYYret=(*LayYYf[LayYYtmp])()) == -2) \
				    goto LayYYerrlab; \
				else if (LayYYret>=0) return(LayYYret); \
		   }
#endif

/*
** global variables used by the parser
*/
YYSTYPE LayYYv[ YYMAXDEPTH ];	/* value stack */
int LayYYs[ YYMAXDEPTH ];		/* state stack */

YYSTYPE *LayYYpv;			/* top of value stack */
YYSTYPE *LayYYpvt;			/* top of value stack for $vars */
int *LayYYps;			/* top of state stack */

int LayYYstate;			/* current state */
int LayYYtmp;			/* extra var (lasts between blocks) */

int LayYYnerrs;			/* number of errors */
int LayYYerrflag;			/* error recovery flag */
int LayYYchar;			/* current input token number */



/*
** LayYYparse - return 0 if worked, 1 if syntax error not recovered from
*/
int
LayYYparse()
{
	/*
	** Initialize externals - LayYYparse may be called more than once
	*/
	LayYYpv = &LayYYv[-1];
	LayYYps = &LayYYs[-1];
	LayYYstate = 0;
	LayYYtmp = 0;
	LayYYnerrs = 0;
	LayYYerrflag = 0;
	LayYYchar = -1;

	goto LayYYstack;
	{
		register YYSTYPE *LayYY_pv;	/* top of value stack */
		register int *LayYY_ps;		/* top of state stack */
		register int LayYY_state;		/* current state */
		register int  LayYY_n;		/* internal state number info */

		/*
		** get globals into registers.
		** branch to here only if YYBACKUP was called.
		*/
	LayYYnewstate:
		LayYY_pv = LayYYpv;
		LayYY_ps = LayYYps;
		LayYY_state = LayYYstate;
		goto LayYY_newstate;

		/*
		** get globals into registers.
		** either we just started, or we just finished a reduction
		*/
	LayYYstack:
		LayYY_pv = LayYYpv;
		LayYY_ps = LayYYps;
		LayYY_state = LayYYstate;

		/*
		** top of for (;;) loop while no reductions done
		*/
	LayYY_stack:
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
		if ( LayYYdebug )
		{
			register int LayYY_i;

			printf( "State %d, token ", LayYY_state );
			if ( LayYYchar == 0 )
				printf( "end-of-file\n" );
			else if ( LayYYchar < 0 )
				printf( "-none-\n" );
			else
			{
				for ( LayYY_i = 0; LayYYtoks[LayYY_i].t_val >= 0;
					LayYY_i++ )
				{
					if ( LayYYtoks[LayYY_i].t_val == LayYYchar )
						break;
				}
				printf( "%s\n", LayYYtoks[LayYY_i].t_name );
			}
		}
#endif /* YYDEBUG */
		if ( ++LayYY_ps >= &LayYYs[ YYMAXDEPTH ] )	/* room on stack? */
		{
			LayYYerror( "yacc stack overflow" );
			YYABORT;
		}
		*LayYY_ps = LayYY_state;
		*++LayYY_pv = LayYYval;

		/*
		** we have a new state - find out what to do
		*/
	LayYY_newstate:
		if ( ( LayYY_n = LayYYpact[ LayYY_state ] ) <= YYFLAG )
			goto LayYYdefault;		/* simple state */
#if YYDEBUG
		/*
		** if debugging, need to mark whether new token grabbed
		*/
		LayYYtmp = LayYYchar < 0;
#endif
		if ( ( LayYYchar < 0 ) && ( ( LayYYchar = LayYYlex() ) < 0 ) )
			LayYYchar = 0;		/* reached EOF */
#if YYDEBUG
		if ( LayYYdebug && LayYYtmp )
		{
			register int LayYY_i;

			printf( "Received token " );
			if ( LayYYchar == 0 )
				printf( "end-of-file\n" );
			else if ( LayYYchar < 0 )
				printf( "-none-\n" );
			else
			{
				for ( LayYY_i = 0; LayYYtoks[LayYY_i].t_val >= 0;
					LayYY_i++ )
				{
					if ( LayYYtoks[LayYY_i].t_val == LayYYchar )
						break;
				}
				printf( "%s\n", LayYYtoks[LayYY_i].t_name );
			}
		}
#endif /* YYDEBUG */
		if ( ( ( LayYY_n += LayYYchar ) < 0 ) || ( LayYY_n >= YYLAST ) )
			goto LayYYdefault;
		if ( LayYYchk[ LayYY_n = LayYYact[ LayYY_n ] ] == LayYYchar )	/*valid shift*/
		{
			LayYYchar = -1;
			LayYYval = LayYYlval;
			LayYY_state = LayYY_n;
			if ( LayYYerrflag > 0 )
				LayYYerrflag--;
			goto LayYY_stack;
		}

	LayYYdefault:
		if ( ( LayYY_n = LayYYdef[ LayYY_state ] ) == -2 )
		{
#if YYDEBUG
			LayYYtmp = LayYYchar < 0;
#endif
			if ( ( LayYYchar < 0 ) && ( ( LayYYchar = LayYYlex() ) < 0 ) )
				LayYYchar = 0;		/* reached EOF */
#if YYDEBUG
			if ( LayYYdebug && LayYYtmp )
			{
				register int LayYY_i;

				printf( "Received token " );
				if ( LayYYchar == 0 )
					printf( "end-of-file\n" );
				else if ( LayYYchar < 0 )
					printf( "-none-\n" );
				else
				{
					for ( LayYY_i = 0;
						LayYYtoks[LayYY_i].t_val >= 0;
						LayYY_i++ )
					{
						if ( LayYYtoks[LayYY_i].t_val
							== LayYYchar )
						{
							break;
						}
					}
					printf( "%s\n", LayYYtoks[LayYY_i].t_name );
				}
			}
#endif /* YYDEBUG */
			/*
			** look through exception table
			*/
			{
				register int *LayYYxi = LayYYexca;

				while ( ( *LayYYxi != -1 ) ||
					( LayYYxi[1] != LayYY_state ) )
				{
					LayYYxi += 2;
				}
				while ( ( *(LayYYxi += 2) >= 0 ) &&
					( *LayYYxi != LayYYchar ) )
					;
				if ( ( LayYY_n = LayYYxi[1] ) < 0 )
					YYACCEPT;
			}
		}

		/*
		** check for syntax error
		*/
		if ( LayYY_n == 0 )	/* have an error */
		{
			/* no worry about speed here! */
			switch ( LayYYerrflag )
			{
			case 0:		/* new error */
				LayYYerror( "syntax error" );
				goto skip_init;
			LayYYerrlab:
				/*
				** get globals into registers.
				** we have a user generated syntax type error
				*/
				LayYY_pv = LayYYpv;
				LayYY_ps = LayYYps;
				LayYY_state = LayYYstate;
				LayYYnerrs++;
			skip_init:
			case 1:
			case 2:		/* incompletely recovered error */
					/* try again... */
				LayYYerrflag = 3;
				/*
				** find state where "error" is a legal
				** shift action
				*/
				while ( LayYY_ps >= LayYYs )
				{
					LayYY_n = LayYYpact[ *LayYY_ps ] + YYERRCODE;
					if ( LayYY_n >= 0 && LayYY_n < YYLAST &&
						LayYYchk[LayYYact[LayYY_n]] == YYERRCODE)					{
						/*
						** simulate shift of "error"
						*/
						LayYY_state = LayYYact[ LayYY_n ];
						goto LayYY_stack;
					}
					/*
					** current state has no shift on
					** "error", pop stack
					*/
#if YYDEBUG
#	define _POP_ "Error recovery pops state %d, uncovers state %d\n"
					if ( LayYYdebug )
						printf( _POP_, *LayYY_ps,
							LayYY_ps[-1] );
#	undef _POP_
#endif
					LayYY_ps--;
					LayYY_pv--;
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
				if ( LayYYdebug )
				{
					register int LayYY_i;

					printf( "Error recovery discards " );
					if ( LayYYchar == 0 )
						printf( "token end-of-file\n" );
					else if ( LayYYchar < 0 )
						printf( "token -none-\n" );
					else
					{
						for ( LayYY_i = 0;
							LayYYtoks[LayYY_i].t_val >= 0;
							LayYY_i++ )
						{
							if ( LayYYtoks[LayYY_i].t_val
								== LayYYchar )
							{
								break;
							}
						}
						printf( "token %s\n",
							LayYYtoks[LayYY_i].t_name );
					}
				}
#endif /* YYDEBUG */
				if ( LayYYchar == 0 )	/* reached EOF. quit */
					YYABORT;
				LayYYchar = -1;
				goto LayYY_newstate;
			}
		}/* end if ( LayYY_n == 0 ) */
		/*
		** reduction by production LayYY_n
		** put stack tops, etc. so things right after switch
		*/
#if YYDEBUG
		/*
		** if debugging, print the string that is the user's
		** specification of the reduction which is just about
		** to be done.
		*/
		if ( LayYYdebug )
			printf( "Reduce by (%d) \"%s\"\n",
				LayYY_n, LayYYreds[ LayYY_n ] );
#endif
		LayYYtmp = LayYY_n;			/* value to switch over */
		LayYYpvt = LayYY_pv;			/* $vars top of value stack */
		/*
		** Look in goto table for next state
		** Sorry about using LayYY_state here as temporary
		** register variable, but why not, if it works...
		** If LayYYr2[ LayYY_n ] doesn't have the low order bit
		** set, then there is no action to be done for
		** this reduction.  So, no saving & unsaving of
		** registers done.  The only difference between the
		** code just after the if and the body of the if is
		** the goto LayYY_stack in the body.  This way the test
		** can be made before the choice of what to do is needed.
		*/
		{
			/* length of production doubled with extra bit */
			register int LayYY_len = LayYYr2[ LayYY_n ];

			if ( !( LayYY_len & 01 ) )
			{
				LayYY_len >>= 1;
				LayYYval = ( LayYY_pv -= LayYY_len )[1];	/* $$ = $1 */
				LayYY_state = LayYYpgo[ LayYY_n = LayYYr1[ LayYY_n ] ] +
					*( LayYY_ps -= LayYY_len ) + 1;
				if ( LayYY_state >= YYLAST ||
					LayYYchk[ LayYY_state =
					LayYYact[ LayYY_state ] ] != -LayYY_n )
				{
					LayYY_state = LayYYact[ LayYYpgo[ LayYY_n ] ];
				}
				goto LayYY_stack;
			}
			LayYY_len >>= 1;
			LayYYval = ( LayYY_pv -= LayYY_len )[1];	/* $$ = $1 */
			LayYY_state = LayYYpgo[ LayYY_n = LayYYr1[ LayYY_n ] ] +
				*( LayYY_ps -= LayYY_len ) + 1;
			if ( LayYY_state >= YYLAST ||
				LayYYchk[ LayYY_state = LayYYact[ LayYY_state ] ] != -LayYY_n )
			{
				LayYY_state = LayYYact[ LayYYpgo[ LayYY_n ] ];
			}
		}
					/* save until reenter driver code */
		LayYYstate = LayYY_state;
		LayYYps = LayYY_ps;
		LayYYpv = LayYY_pv;
	}
	/*
	** code supplied by user is placed in this switch
	*/

		switch(LayYYtmp){

case 1:
# line 50 "laygram.y"
{ *dest = LayYYpvt[-0].bval; } /*NOTREACHED*/ break;
case 2:
# line 53 "laygram.y"
{
			BoxPtr	box = New(LBoxRec);
			box->nextSibling = 0;
			box->type = WidgetBox;
			box->params = *LayYYpvt[-0].pval;
			Dispose (LayYYpvt[-0].pval);
			box->u.widget.quark = LayYYpvt[-1].qval;
			LayYYval.bval = box;
		    } /*NOTREACHED*/ break;
case 3:
# line 63 "laygram.y"
{
			BoxPtr	box = New(LBoxRec);
			box->nextSibling = 0;
			box->type = GlueBox;
			box->params = *LayYYpvt[-0].pval;
			Dispose (LayYYpvt[-0].pval);
			box->u.glue.expr = LayYYpvt[-1].eval;
			LayYYval.bval = box;
		    } /*NOTREACHED*/ break;
case 4:
# line 73 "laygram.y"
{
			BoxPtr	box = New(LBoxRec);
			box->nextSibling = 0;
			box->type = VariableBox;
			box->u.variable.quark = LayYYpvt[-2].qval;
			box->u.variable.expr = LayYYpvt[-0].eval;
			LayYYval.bval = box;
		    } /*NOTREACHED*/ break;
case 5:
# line 82 "laygram.y"
{
			LayYYval.bval = LayYYpvt[-0].bval;
		    } /*NOTREACHED*/ break;
case 6:
# line 87 "laygram.y"
{
			BoxPtr	box = New(LBoxRec);
			BoxPtr	child;

			box->nextSibling = 0;
			box->parent = 0;
			box->type = BoxBox;
			box->u.box.dir = LayYYpvt[-3].lval;
			box->u.box.firstChild = LayYYpvt[-1].bval;
			for (child = LayYYpvt[-1].bval; child; child = child->nextSibling) 
			{
			    if (child->type == GlueBox) 
			    {
				child->params.stretch[!LayYYpvt[-3].lval].expr = 0;
				child->params.shrink[!LayYYpvt[-3].lval].expr = 0;
				child->params.stretch[!LayYYpvt[-3].lval].order = 100000;
				child->params.shrink[!LayYYpvt[-3].lval].order = 100000;
				child->params.stretch[!LayYYpvt[-3].lval].value = 1;
				child->params.shrink[!LayYYpvt[-3].lval].value = 1;
			    }
			    child->parent = box;
			}
			LayYYval.bval = box;
		    } /*NOTREACHED*/ break;
case 7:
# line 113 "laygram.y"
{ 
			LayYYpvt[-1].bval->nextSibling = LayYYpvt[-0].bval;
			LayYYval.bval = LayYYpvt[-1].bval;
		    } /*NOTREACHED*/ break;
case 8:
# line 118 "laygram.y"
{	LayYYval.bval = LayYYpvt[-0].bval; } /*NOTREACHED*/ break;
case 9:
# line 121 "laygram.y"
{	
			BoxParamsPtr	p = New(BoxParamsRec);
			
			p->stretch[LayoutHorizontal] = LayYYpvt[-5].gval;
			p->shrink[LayoutHorizontal] = LayYYpvt[-4].gval;
			p->stretch[LayoutVertical] = LayYYpvt[-2].gval;
			p->shrink[LayoutVertical] = LayYYpvt[-1].gval;
			LayYYval.pval = p;
		    } /*NOTREACHED*/ break;
case 10:
# line 131 "laygram.y"
{	
			BoxParamsPtr	p = New(BoxParamsRec);
			
			ZeroGlue (p->stretch[LayoutHorizontal]);
			ZeroGlue (p->shrink[LayoutHorizontal]);
			ZeroGlue (p->stretch[LayoutVertical]);
			ZeroGlue (p->shrink[LayoutVertical]);
			LayYYval.pval = p;
		    } /*NOTREACHED*/ break;
case 11:
# line 142 "laygram.y"
{	
			BoxParamsPtr	p = New(BoxParamsRec);
			
			p->stretch[LayoutHorizontal] = LayYYpvt[-2].gval;
			p->shrink[LayoutHorizontal] = LayYYpvt[-1].gval;
			p->stretch[LayoutVertical] = LayYYpvt[-2].gval;
			p->shrink[LayoutVertical] = LayYYpvt[-1].gval;
			LayYYval.pval = p;
		    } /*NOTREACHED*/ break;
case 12:
# line 152 "laygram.y"
{	
			BoxParamsPtr	p = New(BoxParamsRec);
			
			ZeroGlue (p->stretch[LayoutHorizontal]);
			ZeroGlue (p->shrink[LayoutHorizontal]);
			ZeroGlue (p->stretch[LayoutVertical]);
			ZeroGlue (p->shrink[LayoutVertical]);
			LayYYval.pval = p;
		    } /*NOTREACHED*/ break;
case 13:
# line 163 "laygram.y"
{ LayYYval.gval = LayYYpvt[-0].gval; } /*NOTREACHED*/ break;
case 14:
# line 165 "laygram.y"
{ ZeroGlue (LayYYval.gval); } /*NOTREACHED*/ break;
case 15:
# line 168 "laygram.y"
{ LayYYval.gval = LayYYpvt[-0].gval; } /*NOTREACHED*/ break;
case 16:
# line 170 "laygram.y"
{ ZeroGlue (LayYYval.gval); } /*NOTREACHED*/ break;
case 17:
# line 173 "laygram.y"
{ LayYYval.gval.order = LayYYpvt[-0].ival; LayYYval.gval.expr = LayYYpvt[-1].eval; } /*NOTREACHED*/ break;
case 18:
# line 175 "laygram.y"
{ LayYYval.gval.order = 0; LayYYval.gval.expr = LayYYpvt[-0].eval; } /*NOTREACHED*/ break;
case 19:
# line 177 "laygram.y"
{ LayYYval.gval.order = LayYYpvt[-0].ival; LayYYval.gval.expr = 0; LayYYval.gval.value = 1; } /*NOTREACHED*/ break;
case 20:
# line 180 "laygram.y"
{
			LayYYval.eval = New(ExprRec);
			LayYYval.eval->type = Unary;
			LayYYval.eval->u.unary.op = LayYYpvt[-1].oval;
			LayYYval.eval->u.unary.down = LayYYpvt[-0].eval;
		    } /*NOTREACHED*/ break;
case 21:
# line 187 "laygram.y"
{ LayYYval.eval = LayYYpvt[-0].eval; } /*NOTREACHED*/ break;
case 23:
# line 191 "laygram.y"
{	LayYYval.eval = New(ExprRec);
			LayYYval.eval->type = Width;
			LayYYval.eval->u.width = LayYYpvt[-0].qval;
		    } /*NOTREACHED*/ break;
case 24:
# line 196 "laygram.y"
{	LayYYval.eval = New(ExprRec);
			LayYYval.eval->type = Height;
			LayYYval.eval->u.height = LayYYpvt[-0].qval;
		    } /*NOTREACHED*/ break;
case 25:
# line 201 "laygram.y"
{ LayYYval.eval = LayYYpvt[-1].eval; } /*NOTREACHED*/ break;
case 26:
# line 203 "laygram.y"
{
			LayYYval.eval = New(ExprRec);
			LayYYval.eval->type = Unary;
			LayYYval.eval->u.unary.op = LayYYpvt[-0].oval;
			LayYYval.eval->u.unary.down = LayYYpvt[-1].eval;
		    } /*NOTREACHED*/ break;
case 27:
# line 210 "laygram.y"
{	LayYYval.eval = New(ExprRec);
			LayYYval.eval->type = Constant;
			LayYYval.eval->u.constant = LayYYpvt[-0].ival;
		    } /*NOTREACHED*/ break;
case 28:
# line 215 "laygram.y"
{	LayYYval.eval = New(ExprRec);
			LayYYval.eval->type = Variable;
			LayYYval.eval->u.variable = LayYYpvt[-0].qval;
		    } /*NOTREACHED*/ break;
case 29:
# line 221 "laygram.y"
{ binary: ;
			LayYYval.eval = New(ExprRec);
			LayYYval.eval->type = Binary;
			LayYYval.eval->u.binary.op = LayYYpvt[-1].oval;
			LayYYval.eval->u.binary.left = LayYYpvt[-2].eval;
			LayYYval.eval->u.binary.right = LayYYpvt[-0].eval;
		    } /*NOTREACHED*/ break;
case 30:
# line 229 "laygram.y"
{ goto binary; } /*NOTREACHED*/ break;
case 31:
# line 231 "laygram.y"
{ goto binary; } /*NOTREACHED*/ break;
case 32:
# line 233 "laygram.y"
{ goto binary; } /*NOTREACHED*/ break;
case 33:
# line 235 "laygram.y"
{ goto binary; } /*NOTREACHED*/ break;
case 34:
# line 237 "laygram.y"
{ unary: ;
			LayYYval.eval = New(ExprRec);
			LayYYval.eval->type = Unary;
			LayYYval.eval->u.unary.op = LayYYpvt[-1].oval;
			LayYYval.eval->u.unary.down = LayYYpvt[-0].eval;
		    } /*NOTREACHED*/ break;
case 35:
# line 244 "laygram.y"
{ LayYYval.eval = LayYYpvt[-0].eval; } /*NOTREACHED*/ break;
case 37:
# line 248 "laygram.y"
{   LayYYval.lval = LayoutVertical; } /*NOTREACHED*/ break;
case 38:
# line 250 "laygram.y"
{   LayYYval.lval = LayoutHorizontal; } /*NOTREACHED*/ break;
}


	goto LayYYstack;		/* reset registers in driver code */
}
