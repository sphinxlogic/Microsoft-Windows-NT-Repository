
/*  A Bison parser, made from ./nlmheader.y  */

#define	CHECK	258
#define	CODESTART	259
#define	COPYRIGHT	260
#define	CUSTOM	261
#define	DATE	262
#define	DEBUG	263
#define	DESCRIPTION	264
#define	EXIT	265
#define	EXPORT	266
#define	FLAG_ON	267
#define	FLAG_OFF	268
#define	FULLMAP	269
#define	HELP	270
#define	IMPORT	271
#define	INPUT	272
#define	MAP	273
#define	MESSAGES	274
#define	MODULE	275
#define	MULTIPLE	276
#define	OS_DOMAIN	277
#define	OUTPUT	278
#define	PSEUDOPREEMPTION	279
#define	REENTRANT	280
#define	SCREENNAME	281
#define	SHARELIB	282
#define	STACK	283
#define	STACKSIZE	284
#define	START	285
#define	SYNCHRONIZE	286
#define	THREADNAME	287
#define	TYPE	288
#define	VERBOSE	289
#define	VERSION	290
#define	XDCDATA	291
#define	STRING	292
#define	QUOTED_STRING	293

#line 1 "./nlmheader.y"
/* nlmheader.y - parse NLM header specification keywords.
     Copyright (C) 1993 Free Software Foundation, Inc.

This file is part of GNU Binutils.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* Written by Ian Lance Taylor <ian@cygnus.com>.

   This bison file parses the commands recognized by the NetWare NLM
   linker, except for lists of object files.  It stores the
   information in global variables.

   This implementation is based on the description in the NetWare Tool
   Maker Specification manual, edition 1.0.  */

#include <ansidecl.h>
#include <stdio.h>
#include <ctype.h>
#include <bfd.h>
#include "sysdep.h"
#include "bucomm.h"
#include "nlm/common.h"
#include "nlm/internal.h"
#include "nlmconv.h"

/* Information is stored in the structures pointed to by these
   variables.  */

Nlm_Internal_Fixed_Header *fixed_hdr;
Nlm_Internal_Variable_Header *var_hdr;
Nlm_Internal_Version_Header *version_hdr;
Nlm_Internal_Copyright_Header *copyright_hdr;
Nlm_Internal_Extended_Header *extended_hdr;

/* Procedure named by CHECK.  */
char *check_procedure;
/* File named by CUSTOM.  */
char *custom_file;
/* Whether to generate debugging information (DEBUG).  */
boolean debug_info;
/* Procedure named by EXIT.  */
char *exit_procedure;
/* Exported symbols (EXPORT).  */
struct string_list *export_symbols;
/* Map file name (MAP, FULLMAP).  */
char *map_file;
/* Whether a full map has been requested (FULLMAP).  */
boolean full_map;
/* File named by HELP.  */
char *help_file;
/* Imported symbols (IMPORT).  */
struct string_list *import_symbols;
/* File named by MESSAGES.  */
char *message_file;
/* Autoload module list (MODULE).  */
struct string_list *modules;
/* File named by SHARELIB.  */
char *sharelib_file;
/* Start procedure name (START).  */
char *start_procedure;
/* VERBOSE.  */
boolean verbose;
/* RPC description file (XDCDATA).  */
char *rpc_file;

/* The number of serious errors that have occurred.  */
int parse_errors;

/* The current symbol prefix when reading a list of import or export
   symbols.  */
static char *symbol_prefix;

/* Parser error message handler.  */
#define yyerror(msg) nlmheader_error (msg);

/* Local functions.  */
static int yylex PARAMS ((void));
static void nlmlex_file_push PARAMS ((const char *));
static boolean nlmlex_file_open PARAMS ((const char *));
static int nlmlex_buf_init PARAMS ((void));
static char nlmlex_buf_add PARAMS ((int));
static long nlmlex_get_number PARAMS ((const char *));
static void nlmheader_warn PARAMS ((const char *, int));
static void nlmheader_error PARAMS ((const char *));
static struct string_list * string_list_cons PARAMS ((char *,
						      struct string_list *));
static struct string_list * string_list_append PARAMS ((struct string_list *,
							struct string_list *));
static struct string_list * string_list_append1 PARAMS ((struct string_list *,
							 char *));
static char *xstrdup PARAMS ((const char *));


#line 108 "./nlmheader.y"
typedef union
{
  char *string;
  struct string_list *list;
} YYSTYPE;

#ifndef YYLTYPE
typedef
  struct yyltype
    {
      int timestamp;
      int first_line;
      int first_column;
      int last_line;
      int last_column;
      char *text;
   }
  yyltype;

#define YYLTYPE yyltype
#endif

#include <stdio.h>

#ifndef __STDC__
#define const
#endif



#define	YYFINAL		84
#define	YYFLAG		-32768
#define	YYNTBASE	41

#define YYTRANSLATE(x) ((unsigned)(x) <= 293 ? yytranslate[x] : 51)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    39,
    40,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     2,     3,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
    36,    37,    38
};

static const short yyrline[] = {     0,
   139,   145,   147,   152,   157,   162,   179,   183,   195,   199,
   215,   219,   224,   227,   232,   237,   242,   246,   251,   254,
   259,   263,   267,   271,   275,   279,   284,   288,   292,   308,
   312,   317,   322,   326,   330,   346,   351,   355,   377,   393,
   401,   406,   416,   421,   425,   429,   437,   448,   464,   469
};

static const char * const yytname[] = {     0,
"error","$illegal.","CHECK","CODESTART","COPYRIGHT","CUSTOM","DATE","DEBUG","DESCRIPTION","EXIT",
"EXPORT","FLAG_ON","FLAG_OFF","FULLMAP","HELP","IMPORT","INPUT","MAP","MESSAGES","MODULE",
"MULTIPLE","OS_DOMAIN","OUTPUT","PSEUDOPREEMPTION","REENTRANT","SCREENNAME","SHARELIB","STACK","STACKSIZE","START",
"SYNCHRONIZE","THREADNAME","TYPE","VERBOSE","VERSION","XDCDATA","STRING","QUOTED_STRING","'('","')'",
"file"
};

static const short yyr1[] = {     0,
    41,    42,    42,    43,    43,    43,    43,    43,    43,    43,
    43,    44,    43,    43,    43,    43,    43,    45,    43,    43,
    43,    43,    43,    43,    43,    43,    43,    43,    43,    43,
    43,    43,    43,    43,    43,    43,    43,    43,    43,    43,
    46,    46,    47,    47,    47,    47,    48,    49,    50,    50
};

static const short yyr2[] = {     0,
     1,     0,     2,     2,     2,     2,     2,     4,     1,     2,
     2,     0,     3,     2,     2,     2,     2,     0,     3,     2,
     2,     2,     2,     1,     1,     2,     1,     1,     2,     2,
     2,     2,     2,     1,     2,     2,     1,     4,     3,     2,
     0,     1,     1,     1,     2,     2,     3,     1,     0,     2
};

static const short yydefact[] = {     2,
     0,     0,     0,     0,     0,     9,     0,     0,    12,     0,
     0,     0,     0,    18,     0,     0,     0,    49,    24,    25,
     0,    27,    28,     0,     0,     0,     0,     0,    34,     0,
     0,    37,     0,     0,     1,     2,     4,     5,     6,     7,
     0,    10,    11,    41,    14,    15,    16,    17,    41,    20,
    21,    22,    49,    23,    26,    29,    30,    31,    32,    33,
    35,    36,     0,    40,     3,     0,    48,     0,    13,    42,
    44,    43,    19,    50,    39,     8,     0,    46,    45,    38,
    47,     0,     0,     0
};

static const short yydefgoto[] = {    82,
    35,    36,    44,    49,    69,    70,    71,    72,    54
};

static const short yypact[] = {    -3,
    -1,     1,    -4,     2,     3,-32768,     4,     6,-32768,     7,
     8,     9,    10,-32768,    11,    12,    13,    14,-32768,-32768,
    15,-32768,-32768,    16,    18,    19,    20,    21,-32768,    22,
    24,-32768,    25,    26,-32768,    -3,-32768,-32768,-32768,-32768,
    27,-32768,-32768,    -2,-32768,-32768,-32768,-32768,    -2,-32768,
-32768,-32768,    14,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,    28,-32768,-32768,    29,-32768,    30,-32768,    -2,
-32768,-32768,-32768,-32768,    31,-32768,    32,-32768,-32768,-32768,
-32768,    41,    53,-32768
};

static const short yypgoto[] = {-32768,
    23,-32768,-32768,-32768,    33,-32768,     0,     5,    34
};


#define	YYLAST		87


static const short yytable[] = {     1,
     2,     3,     4,     5,     6,     7,     8,     9,    10,    11,
    12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
    22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
    32,    33,    34,    39,    67,    37,    68,    38,    40,    41,
    83,    42,    43,    45,    46,    47,    48,    50,    51,    52,
    53,    55,    84,    56,    57,    58,    59,    60,    65,    61,
    62,    63,    64,    66,    75,    76,    77,    80,     0,    78,
     0,    81,     0,     0,    79,     0,     0,     0,     0,     0,
     0,    73,     0,     0,     0,     0,    74
};

static const short yycheck[] = {     3,
     4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
    14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
    24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
    34,    35,    36,    38,    37,    37,    39,    37,    37,    37,
     0,    38,    37,    37,    37,    37,    37,    37,    37,    37,
    37,    37,     0,    38,    37,    37,    37,    37,    36,    38,
    37,    37,    37,    37,    37,    37,    37,    37,    -1,    70,
    -1,    40,    -1,    -1,    70,    -1,    -1,    -1,    -1,    -1,
    -1,    49,    -1,    -1,    -1,    -1,    53
};
#define YYPURE 1

/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "bison.simple"

/* Skeleton output parser for bison,
   Copyright (C) 1984 Bob Corbett and Richard Stallman

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 1, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */


#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__)
#include <alloca.h>
#endif

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYFAIL		goto yyerrlab;
#define YYACCEPT	return(0)
#define YYABORT 	return(1)
#define YYERROR		goto yyerrlab

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYIMPURE
#define YYLEX		yylex()
#endif

#ifndef YYPURE
#define YYLEX		yylex(&yylval, &yylloc)
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYIMPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/

int yynerrs;			/*  number of parse errors so far       */
#endif  /* YYIMPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYMAXDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYMAXDEPTH
#define YYMAXDEPTH 200
#endif

/*  YYMAXLIMIT is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#ifndef YYMAXLIMIT
#define YYMAXLIMIT 10000
#endif


#line 90 "bison.simple"
int
yyparse()
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  YYLTYPE *yylsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYMAXDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYMAXDEPTH];	/*  the semantic value stack		*/
  YYLTYPE yylsa[YYMAXDEPTH];	/*  the location stack			*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */
  YYLTYPE *yyls = yylsa;

  int yymaxdepth = YYMAXDEPTH;

#ifndef YYPURE
  int yychar;
  YYSTYPE yylval;
  YYLTYPE yylloc;
  int yynerrs;
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
  yylsp = yyls;

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yymaxdepth - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      YYLTYPE *yyls1 = yyls;
      short *yyss1 = yyss;

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yymaxdepth);

      yyss = yyss1; yyvs = yyvs1; yyls = yyls1;
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yymaxdepth >= YYMAXLIMIT)
	yyerror("parser stack overflow");
      yymaxdepth *= 2;
      if (yymaxdepth > YYMAXLIMIT)
	yymaxdepth = YYMAXLIMIT;
      yyss = (short *) alloca (yymaxdepth * sizeof (*yyssp));
      bcopy ((char *)yyss1, (char *)yyss, size * sizeof (*yyssp));
      yyvs = (YYSTYPE *) alloca (yymaxdepth * sizeof (*yyvsp));
      bcopy ((char *)yyvs1, (char *)yyvs, size * sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) alloca (yymaxdepth * sizeof (*yylsp));
      bcopy ((char *)yyls1, (char *)yyls, size * sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yymaxdepth);
#endif

      if (yyssp >= yyss + yymaxdepth - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
yyresume:

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Next token is %d (%s)\n", yychar, yytname[yychar1]);
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      if (yylen == 1)
	fprintf (stderr, "Reducing 1 value via line %d, ",
		 yyrline[yyn]);
      else
	fprintf (stderr, "Reducing %d values via line %d, ",
		 yylen, yyrline[yyn]);
    }
#endif


  switch (yyn) {

case 4:
#line 154 "./nlmheader.y"
{
	    check_procedure = yyvsp[0].string;
	  ;
    break;}
case 5:
#line 158 "./nlmheader.y"
{
	    nlmheader_warn ("CODESTART is not implemented; sorry", -1);
	    free (yyvsp[0].string);
	  ;
    break;}
case 6:
#line 163 "./nlmheader.y"
{
	    int len;

	    strncpy (copyright_hdr->stamp, "CoPyRiGhT=", 10);
	    len = strlen (yyvsp[0].string);
	    if (len >= NLM_MAX_COPYRIGHT_MESSAGE_LENGTH)
	      {
		nlmheader_warn ("copyright string is too long",
				NLM_MAX_COPYRIGHT_MESSAGE_LENGTH - 1);
		len = NLM_MAX_COPYRIGHT_MESSAGE_LENGTH - 1;
	      }
	    copyright_hdr->copyrightMessageLength = len;
	    strncpy (copyright_hdr->copyrightMessage, yyvsp[0].string, len);
	    copyright_hdr->copyrightMessage[len] = '\0';
	    free (yyvsp[0].string);
	  ;
    break;}
case 7:
#line 180 "./nlmheader.y"
{
	    custom_file = yyvsp[0].string;
	  ;
    break;}
case 8:
#line 184 "./nlmheader.y"
{
	    /* We don't set the version stamp here, because we use the
	       version stamp to detect whether the required VERSION
	       keyword was given.  */
	    version_hdr->month = nlmlex_get_number (yyvsp[-2].string);
	    version_hdr->day = nlmlex_get_number (yyvsp[-1].string);
	    version_hdr->year = nlmlex_get_number (yyvsp[0].string);
	    free (yyvsp[-2].string);
	    free (yyvsp[-1].string);
	    free (yyvsp[0].string);
	  ;
    break;}
case 9:
#line 196 "./nlmheader.y"
{
	    debug_info = true;
	  ;
    break;}
case 10:
#line 200 "./nlmheader.y"
{
	    int len;

	    len = strlen (yyvsp[0].string);
	    if (len > NLM_MAX_DESCRIPTION_LENGTH)
	      {
		nlmheader_warn ("description string is too long",
				NLM_MAX_DESCRIPTION_LENGTH);
		len = NLM_MAX_DESCRIPTION_LENGTH;
	      }
	    var_hdr->descriptionLength = len;
	    strncpy (var_hdr->descriptionText, yyvsp[0].string, len);
	    var_hdr->descriptionText[len] = '\0';
	    free (yyvsp[0].string);
	  ;
    break;}
case 11:
#line 216 "./nlmheader.y"
{
	    exit_procedure = yyvsp[0].string;
	  ;
    break;}
case 12:
#line 220 "./nlmheader.y"
{
	    symbol_prefix = NULL;
	  ;
    break;}
case 13:
#line 224 "./nlmheader.y"
{
	    export_symbols = string_list_append (export_symbols, yyvsp[0].list);
	  ;
    break;}
case 14:
#line 228 "./nlmheader.y"
{
	    fixed_hdr->flags |= nlmlex_get_number (yyvsp[0].string);
	    free (yyvsp[0].string);
	  ;
    break;}
case 15:
#line 233 "./nlmheader.y"
{
	    fixed_hdr->flags &=~ nlmlex_get_number (yyvsp[0].string);
	    free (yyvsp[0].string);
	  ;
    break;}
case 16:
#line 238 "./nlmheader.y"
{
	    map_file = yyvsp[0].string;
	    full_map = true;
	  ;
    break;}
case 17:
#line 243 "./nlmheader.y"
{
	    help_file = yyvsp[0].string;
	  ;
    break;}
case 18:
#line 247 "./nlmheader.y"
{
	    symbol_prefix = NULL;
	  ;
    break;}
case 19:
#line 251 "./nlmheader.y"
{
	    import_symbols = string_list_append (import_symbols, yyvsp[0].list);
	  ;
    break;}
case 20:
#line 255 "./nlmheader.y"
{
	    nlmheader_warn ("INPUT not supported", -1);
	    free (yyvsp[0].string);
	  ;
    break;}
case 21:
#line 260 "./nlmheader.y"
{
	    map_file = yyvsp[0].string;
	  ;
    break;}
case 22:
#line 264 "./nlmheader.y"
{
	    message_file = yyvsp[0].string;
	  ;
    break;}
case 23:
#line 268 "./nlmheader.y"
{
	    modules = string_list_append (modules, yyvsp[0].list);
	  ;
    break;}
case 24:
#line 272 "./nlmheader.y"
{
	    fixed_hdr->flags |= 0x2;
	  ;
    break;}
case 25:
#line 276 "./nlmheader.y"
{
	    fixed_hdr->flags |= 0x10;
	  ;
    break;}
case 26:
#line 280 "./nlmheader.y"
{
	    nlmheader_warn ("OUTPUT not supported", -1);
	    free (yyvsp[0].string);
	  ;
    break;}
case 27:
#line 285 "./nlmheader.y"
{
	    fixed_hdr->flags |= 0x8;
	  ;
    break;}
case 28:
#line 289 "./nlmheader.y"
{
	    fixed_hdr->flags |= 0x1;
	  ;
    break;}
case 29:
#line 293 "./nlmheader.y"
{
	    int len;

	    len = strlen (yyvsp[0].string);
	    if (len >= NLM_MAX_SCREEN_NAME_LENGTH)
	      {
		nlmheader_warn ("screen name is too long",
				NLM_MAX_SCREEN_NAME_LENGTH);
		len = NLM_MAX_SCREEN_NAME_LENGTH;
	      }
	    var_hdr->screenNameLength = len;
	    strncpy (var_hdr->screenName, yyvsp[0].string, len);
	    var_hdr->screenName[NLM_MAX_SCREEN_NAME_LENGTH] = '\0';
	    free (yyvsp[0].string);
	  ;
    break;}
case 30:
#line 309 "./nlmheader.y"
{
	    sharelib_file = yyvsp[0].string;
	  ;
    break;}
case 31:
#line 313 "./nlmheader.y"
{
	    var_hdr->stackSize = nlmlex_get_number (yyvsp[0].string);
	    free (yyvsp[0].string);
	  ;
    break;}
case 32:
#line 318 "./nlmheader.y"
{
	    var_hdr->stackSize = nlmlex_get_number (yyvsp[0].string);
	    free (yyvsp[0].string);
	  ;
    break;}
case 33:
#line 323 "./nlmheader.y"
{
	    start_procedure = yyvsp[0].string;
	  ;
    break;}
case 34:
#line 327 "./nlmheader.y"
{
	    fixed_hdr->flags |= 0x4;
	  ;
    break;}
case 35:
#line 331 "./nlmheader.y"
{
	    int len;

	    len = strlen (yyvsp[0].string);
	    if (len >= NLM_MAX_THREAD_NAME_LENGTH)
	      {
		nlmheader_warn ("thread name is too long",
				NLM_MAX_THREAD_NAME_LENGTH);
		len = NLM_MAX_THREAD_NAME_LENGTH;
	      }
	    var_hdr->threadNameLength = len;
	    strncpy (var_hdr->threadName, yyvsp[0].string, len);
	    var_hdr->screenName[NLM_MAX_THREAD_NAME_LENGTH] = '\0';
	    free (yyvsp[0].string);
	  ;
    break;}
case 36:
#line 347 "./nlmheader.y"
{
	    fixed_hdr->moduleType = nlmlex_get_number (yyvsp[0].string);
	    free (yyvsp[0].string);
	  ;
    break;}
case 37:
#line 352 "./nlmheader.y"
{
	    verbose = true;
	  ;
    break;}
case 38:
#line 356 "./nlmheader.y"
{
	    long val;

	    strncpy (version_hdr->stamp, "VeRsIoN#", 8);
	    version_hdr->majorVersion = nlmlex_get_number (yyvsp[-2].string);
	    val = nlmlex_get_number (yyvsp[-1].string);
	    if (val < 0 || val > 99)
	      nlmheader_warn ("illegal minor version number (must be between 0 and 99)",
			      -1);
	    else
	      version_hdr->minorVersion = val;
	    val = nlmlex_get_number (yyvsp[0].string);
	    if (val < 1 || val > 26)
	      nlmheader_warn ("illegal revision number (must be between 1 and 26)",
			      -1);
	    else
	      version_hdr->revision = val;
	    free (yyvsp[-2].string);
	    free (yyvsp[-1].string);
	    free (yyvsp[0].string);
	  ;
    break;}
case 39:
#line 378 "./nlmheader.y"
{
	    long val;

	    strncpy (version_hdr->stamp, "VeRsIoN#", 8);
	    version_hdr->majorVersion = nlmlex_get_number (yyvsp[-1].string);
	    val = nlmlex_get_number (yyvsp[0].string);
	    if (val < 0 || val > 99)
	      nlmheader_warn ("illegal minor version number (must be between 0 and 99)",
			      -1);
	    else
	      version_hdr->minorVersion = val;
	    version_hdr->revision = 0;
	    free (yyvsp[-1].string);
	    free (yyvsp[0].string);
	  ;
    break;}
case 40:
#line 394 "./nlmheader.y"
{
	    rpc_file = yyvsp[0].string;
	  ;
    break;}
case 41:
#line 403 "./nlmheader.y"
{
	    yyval.list = NULL;
	  ;
    break;}
case 42:
#line 407 "./nlmheader.y"
{
	    yyval.list = yyvsp[0].list;
	  ;
    break;}
case 43:
#line 418 "./nlmheader.y"
{
	    yyval.list = string_list_cons (yyvsp[0].string, NULL);
	  ;
    break;}
case 44:
#line 422 "./nlmheader.y"
{
	    yyval.list = NULL;
	  ;
    break;}
case 45:
#line 426 "./nlmheader.y"
{
	    yyval.list = string_list_append1 (yyvsp[-1].list, yyvsp[0].string);
	  ;
    break;}
case 46:
#line 430 "./nlmheader.y"
{
	    yyval.list = yyvsp[-1].list;
	  ;
    break;}
case 47:
#line 439 "./nlmheader.y"
{
	    if (symbol_prefix != NULL)
	      free (symbol_prefix);
	    symbol_prefix = yyvsp[-1].string;
	  ;
    break;}
case 48:
#line 450 "./nlmheader.y"
{
	    if (symbol_prefix == NULL)
	      yyval.string = yyvsp[0].string;
	    else
	      {
		yyval.string = xmalloc (strlen (symbol_prefix) + strlen (yyvsp[0].string) + 2);
		sprintf (yyval.string, "%s@%s", symbol_prefix, yyvsp[0].string);
		free (yyvsp[0].string);
	      }
	  ;
    break;}
case 49:
#line 466 "./nlmheader.y"
{
	    yyval.list = NULL;
	  ;
    break;}
case 50:
#line 470 "./nlmheader.y"
{
	    yyval.list = string_list_cons (yyvsp[-1].string, yyvsp[0].list);
	  ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 327 "bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;
      yyerror("parse error");
    }

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;
}
#line 475 "./nlmheader.y"


/* If strerror is just a macro, we want to use the one from libiberty
   since it will handle undefined values.  */
#undef strerror
extern char *strerror ();

/* The lexer is simple, too simple for flex.  Keywords are only
   recognized at the start of lines.  Everything else must be an
   argument.  A comma is treated as whitespace.  */

/* The states the lexer can be in.  */

enum lex_state
{
  /* At the beginning of a line.  */
  BEGINNING_OF_LINE,
  /* In the middle of a line.  */
  IN_LINE
};

/* We need to keep a stack of files to handle file inclusion.  */

struct input
{
  /* The file to read from.  */
  FILE *file;
  /* The name of the file.  */
  char *name;
  /* The current line number.  */
  int lineno;
  /* The current state.  */
  enum lex_state state;
  /* The next file on the stack.  */
  struct input *next;
};

/* The current input file.  */

static struct input current;

/* The character which introduces comments.  */
#define COMMENT_CHAR '#'

/* Start the lexer going on the main input file.  */

boolean
nlmlex_file (name)
     const char *name;
{
  current.next = NULL;
  return nlmlex_file_open (name);
}

/* Start the lexer going on a subsidiary input file.  */

static void
nlmlex_file_push (name)
     const char *name;
{
  struct input *push;

  push = (struct input *) xmalloc (sizeof (struct input));
  *push = current;
  if (nlmlex_file_open (name))
    current.next = push;
  else
    {
      current = *push;
      free (push);
    }
}

/* Start lexing from a file.  */

static boolean
nlmlex_file_open (name)
     const char *name;
{
  current.file = fopen (name, "r");
  if (current.file == NULL)
    {
      fprintf (stderr, "%s:%s: %s\n", program_name, name, strerror (errno));
      ++parse_errors;
      return false;
    }
  current.name = xstrdup (name);
  current.lineno = 1;
  current.state = BEGINNING_OF_LINE;
  return true;
}

/* Table used to turn keywords into tokens.  */

struct keyword_tokens_struct
{
  const char *keyword;
  int token;
};

struct keyword_tokens_struct keyword_tokens[] =
{
  { "CHECK", CHECK },
  { "CODESTART", CODESTART },
  { "COPYRIGHT", COPYRIGHT },
  { "CUSTOM", CUSTOM },
  { "DATE", DATE },
  { "DEBUG", DEBUG },
  { "DESCRIPTION", DESCRIPTION },
  { "EXIT", EXIT },
  { "EXPORT", EXPORT },
  { "FLAG_ON", FLAG_ON },
  { "FLAG_OFF", FLAG_OFF },
  { "FULLMAP", FULLMAP },
  { "HELP", HELP },
  { "IMPORT", IMPORT },
  { "INPUT", INPUT },
  { "MAP", MAP },
  { "MESSAGES", MESSAGES },
  { "MODULE", MODULE },
  { "MULTIPLE", MULTIPLE },
  { "OS_DOMAIN", OS_DOMAIN },
  { "OUTPUT", OUTPUT },
  { "PSEUDOPREEMPTION", PSEUDOPREEMPTION },
  { "REENTRANT", REENTRANT },
  { "SCREENNAME", SCREENNAME },
  { "SHARELIB", SHARELIB },
  { "STACK", STACK },
  { "STACKSIZE", STACKSIZE },
  { "START", START },
  { "SYNCHRONIZE", SYNCHRONIZE },
  { "THREADNAME", THREADNAME },
  { "TYPE", TYPE },
  { "VERBOSE", VERBOSE },
  { "VERSION", VERSION },
  { "XDCDATA", XDCDATA }
};

#define KEYWORD_COUNT (sizeof (keyword_tokens) / sizeof (keyword_tokens[0]))

/* The lexer accumulates strings in these variables.  */
static char *lex_buf;
static int lex_size;
static int lex_pos;

/* Start accumulating strings into the buffer.  */
#define BUF_INIT() \
  ((void) (lex_buf != NULL ? lex_pos = 0 : nlmlex_buf_init ()))

static int
nlmlex_buf_init ()
{
  lex_size = 10;
  lex_buf = xmalloc (lex_size + 1);
  lex_pos = 0;
  return 0;
}

/* Finish a string in the buffer.  */
#define BUF_FINISH() ((void) (lex_buf[lex_pos] = '\0'))

/* Accumulate a character into the buffer.  */
#define BUF_ADD(c) \
  ((void) (lex_pos < lex_size \
	   ? lex_buf[lex_pos++] = (c) \
	   : nlmlex_buf_add (c)))

static char
nlmlex_buf_add (c)
     int c;
{
  if (lex_pos >= lex_size)
    {
      lex_size *= 2;
      lex_buf = xrealloc (lex_buf, lex_size + 1);
    }

  return lex_buf[lex_pos++] = c;
}

/* The lexer proper.  This is called by the bison generated parsing
   code.  */

static int
yylex ()
{
  int c;

tail_recurse:

  c = getc (current.file);

  /* Commas are treated as whitespace characters.  */
  while (isspace ((unsigned char) c) || c == ',')
    {
      current.state = IN_LINE;
      if (c == '\n')
	{
	  ++current.lineno;
	  current.state = BEGINNING_OF_LINE;
	}
      c = getc (current.file);
    }

  /* At the end of the file we either pop to the previous file or
     finish up.  */
  if (c == EOF)
    {
      fclose (current.file);
      free (current.name);
      if (current.next == NULL)
	return 0;
      else
	{
	  struct input *next;

	  next = current.next;
	  current = *next;
	  free (next);
	  goto tail_recurse;
	}
    }

  /* A comment character always means to drop everything until the
     next newline.  */
  if (c == COMMENT_CHAR)
    {
      do
	{
	  c = getc (current.file);
	}
      while (c != '\n');
      ++current.lineno;
      current.state = BEGINNING_OF_LINE;
      goto tail_recurse;
    }

  /* An '@' introduces an include file.  */
  if (c == '@')
    {
      do
	{
	  c = getc (current.file);
	  if (c == '\n')
	    ++current.lineno;
	}
      while (isspace ((unsigned char) c));
      BUF_INIT ();
      while (! isspace ((unsigned char) c) && c != EOF)
	{
	  BUF_ADD (c);
	  c = getc (current.file);
	}
      BUF_FINISH ();

      ungetc (c, current.file);
      
      nlmlex_file_push (lex_buf);
      goto tail_recurse;
    }

  /* A non-space character at the start of a line must be the start of
     a keyword.  */
  if (current.state == BEGINNING_OF_LINE)
    {
      BUF_INIT ();
      while (isalnum ((unsigned char) c) || c == '_')
	{
	  if (islower ((unsigned char) c))
	    BUF_ADD (toupper ((unsigned char) c));
	  else
	    BUF_ADD (c);
	  c = getc (current.file);
	}
      BUF_FINISH ();

      if (c != EOF && ! isspace ((unsigned char) c) && c != ',')
	fprintf (stderr, "%s:%s:%d: illegal character in keyword: %c\n",
		 program_name, current.name, current.lineno, c);
      else
	{
	  int i;

	  for (i = 0; i < KEYWORD_COUNT; i++)
	    {
	      if (lex_buf[0] == keyword_tokens[i].keyword[0]
		  && strcmp (lex_buf, keyword_tokens[i].keyword) == 0)
		{
		  /* Pushing back the final whitespace avoids worrying
		     about \n here.  */
		  ungetc (c, current.file);
		  current.state = IN_LINE;
		  return keyword_tokens[i].token;
		}
	    }
	  
	  fprintf (stderr, "%s:%s:%d: unrecognized keyword: %s\n",
		   program_name, current.name, current.lineno, lex_buf);
	}

      ++parse_errors;
      /* Treat the rest of this line as a comment.  */
      ungetc (COMMENT_CHAR, current.file);
      goto tail_recurse;
    }

  /* Parentheses just represent themselves.  */
  if (c == '(' || c == ')')
    return c;

  /* Handle quoted strings.  */
  if (c == '"' || c == '\'')
    {
      int quote;
      int start_lineno;

      quote = c;
      start_lineno = current.lineno;

      c = getc (current.file);
      BUF_INIT ();
      while (c != quote && c != EOF)
	{
	  BUF_ADD (c);
	  if (c == '\n')
	    ++current.lineno;
	  c = getc (current.file);
	}
      BUF_FINISH ();

      if (c == EOF)
	{
	  fprintf (stderr, "%s:%s:%d: end of file in quoted string\n",
		   program_name, current.name, start_lineno);
	  ++parse_errors;
	}

      /* FIXME: Possible memory leak.  */
      yylval.string = xstrdup (lex_buf);
      return QUOTED_STRING;
    }

  /* Gather a generic argument.  */
  BUF_INIT ();
  while (! isspace (c)
	 && c != ','
	 && c != COMMENT_CHAR
	 && c != '('
	 && c != ')')
    {
      BUF_ADD (c);
      c = getc (current.file);
    }
  BUF_FINISH ();

  ungetc (c, current.file);

  /* FIXME: Possible memory leak.  */
  yylval.string = xstrdup (lex_buf);
  return STRING;
}

/* Get a number from a string.  */

static long
nlmlex_get_number (s)
     const char *s;
{
  long ret;
  char *send;

  ret = strtol (s, &send, 10);
  if (*send != '\0')
    nlmheader_warn ("bad number", -1);
  return ret;
}

/* Issue a warning.  */

static void
nlmheader_warn (s, imax)
     const char *s;
     int imax;
{
  fprintf (stderr, "%s:%s:%d: %s", program_name, current.name,
	   current.lineno, s);
  if (imax != -1)
    fprintf (stderr, " (max %d)", imax);
  fprintf (stderr, "\n");
}

/* Report an error.  */

static void
nlmheader_error (s)
     const char *s;
{
  nlmheader_warn (s, -1);
  ++parse_errors;
}

/* Add a string to a string list.  */

static struct string_list *
string_list_cons (s, l)
     char *s;
     struct string_list *l;
{
  struct string_list *ret;

  ret = (struct string_list *) xmalloc (sizeof (struct string_list));
  ret->next = l;
  ret->string = s;
  return ret;
}

/* Append a string list to another string list.  */

static struct string_list *
string_list_append (l1, l2)
     struct string_list *l1;
     struct string_list *l2;
{
  register struct string_list **pp;

  for (pp = &l1; *pp != NULL; pp = &(*pp)->next)
    ;
  *pp = l2;
  return l1;
}

/* Append a string to a string list.  */

static struct string_list *
string_list_append1 (l, s)
     struct string_list *l;
     char *s;
{
  struct string_list *n;
  register struct string_list **pp;

  n = (struct string_list *) xmalloc (sizeof (struct string_list));
  n->next = NULL;
  n->string = s;
  for (pp = &l; *pp != NULL; pp = &(*pp)->next)
    ;
  *pp = n;
  return l;
}

/* Duplicate a string in memory.  */

static char *
xstrdup (s)
     const char *s;
{
  unsigned long len;
  char *ret;

  len = strlen (s);
  ret = xmalloc (len + 1);
  strcpy (ret, s);
  return ret;
}
