%{
#ifdef FLUKE
# ifndef LINT
    static char RCSid[] = "@(#)FLUKE  $Header: /home/kreskin/u0/barnett/Src/Ease/ease/src/RCS/parser.y,v 3.4 1991/10/15 17:02:04 barnett Exp $";
# endif LINT
#endif FLUKE

/*
 *	parser.y -- EASE parser.
 *
 *		    Contains code for yacc(1) which produces a parser (y.tab.c)
 *		    for Ease, a specification format for sendmail configuration
 *		    files.
 *
 *	author   -- James S. Schoner, Purdue University Computing Center,
 *		    		      West Lafayette, Indiana  47907
 *
 *	date     -- July 2, 1985
 *
 *	Copyright (c) 1985 by Purdue Research Foundation
 *
 *	All rights reserved.
 *
 * $Log: parser.y,v $
 * Revision 3.4  1991/10/15  17:02:04  barnett
 * Detect if (one_or_more) next ($2) error
 *
 * Revision 3.3  1991/09/09  16:33:23  barnett
 * Minor bug fix release
 *
 * Revision 3.2  1991/05/16  10:45:25  barnett
 * Better support for System V machines
 * Support for machines with read only text segments
 *
 * Revision 3.1  1991/02/25  22:09:52  barnett
 * Fixed some portability problems
 *
 * Revision 3.0  1991/02/22  18:50:27  barnett
 * Added support for HP/UX and IDA sendmail.
 *
 * Revision 2.1  1990/01/30  15:48:35  jeff
 * Added SunOS/Ultrix/IDA extensions  Jan 24, 1989 Bruce Barnett
 *
 * Version 2.0  90/01/30  15:44:34  jeff
 * Baseline release for netwide posting.
 * 
 */

#include "fixstrings.h"
#include <stdio.h>
#include "symtab.h"
#include <ctype.h>
extern void	   BindID ();
extern void	   EmitDef ();
extern char	  *ListAppend ();
extern char 	  *MakeCond ();
extern char	  *MakeRStr ();
extern char       *ConvOpt ();
extern char	  *ConvFlg ();
extern char	  *MacScan ();
extern char	  *ConvMat ();
extern void	   StartRuleset ();
extern char	  *MakePosTok ();
extern char	  *GetField ();
extern char	  *Bracket ();
extern char	  *DbmParen ();
extern char	  *MakeRSCall ();
extern char	  *CheckMailer ();
extern char	  *CheckRS ();
extern char	  *MakeField ();
extern char	   MakeMac ();
extern void	   AssignType ();
extern void	   RemoveSymbol ();
extern void	   yyerror ();
extern void	   FatalError ();
extern short RMatch;		/* ruleset match flag 		      */
extern short number_of_fields;	/* number of fields on the line	      */
#ifdef DATA_RW
char *Cbuf = " ";		/* character buffer   		      */
char *Mbuf = "$ ";		/* macro buffer	      		      */
#else
char *Cbuf = NULL;
char *Mbuf = NULL;
extern char *Strdup(); 
#endif
char *Tsb;			/* pointer to temporary string buffer */
char *Tsb1;			/* pointer to another temporary string buffer */
char *Flaglist;			/* pointer to header flag list	      */

#define DIM(x)	(sizeof x/sizeof x[0])
extern int yychar;
extern int	yydebug;

#ifndef	DATA_RW
void InitParser()
{
	if(!(Cbuf = Strdup(" ")) || !(Mbuf = Strdup("$ ")))
		FatalError("Out of memory in InitParser()", (char *)NULL);
}
#endif

static void	
Free( ptr )
char *ptr;
  {
      if (ptr == (char *)0) {
/*	  fprintf(stderr,"Freeing a null pointer\n"); */
      } else {
	  if (free(ptr) < 0 ) {
	      fprintf(stderr,"Free() returned an error\n");
	  }
      }
      fflush(stderr);
  };
static char *
yydisplay(ch)
     register int ch;
{
    static char buf[15];
    static char * token[] = {
#include "y.tok.h"
	0 };

    switch (ch) {
      case 0:
	return "[end of file]";
/* YYERRCODE is 256. See below */
/*      case YYERRCODE:
	return "[error]"; */
      case '\b':
	return "'\\b'";
      case '\f':
	return "'\\f'";
      case '\n':
	return "'\\n'";
      case '\r':
	return "'\\r'";
      case '\t':
	return "'\\t'";
    }
    /* I should use YYERRCODE - but it hasn't been defined yet */
    /* when /usr/lib/yaccpar is added to this file, it will be defined */
    if (ch == 256 ) return ("[error]"); 
    if (ch > 256 && ch < 256 + DIM(token))
      return token[ch - 257];
    if (isascii(ch) && isprint(ch))
      sprintf(buf, "'%c'",ch);
    else if (ch < 256)
      sprintf(buf, "Char %o4.3o", ch);
    else 
      sprintf(buf, "token %d", ch);
    return buf;
}
static yyyylex() 
{
	if (yychar < 0) {
	    /* don't make this match =yylex - because sed changes
	       =yylex to =yyyylex in the Makefile. 
	       the pieces it changes is in /usr/lib/yaccparr and I don't
	       want to modify THAT!  - bgb */

		if ((yychar = yylex ()) < 0)	/* call yylex, not yyyylex */
			yychar = 0;
#ifdef YYDEBUG
	if (yydebug)
		printf("[yydebug] reading %s\n",
			yydisplay(yychar));
		fflush(stdout);
#endif
		return yychar;
	}
}
 	

%}

%union {			/* value stack element type    */
	int	  ival;		/* integer token 	       */
	char	  *psb;		/* string token		       */
	struct he *phe;		/* pointer to hash entry       */
	enum opts optval;	/* sendmail options	       */
	enum flgs flgval;	/* mailer flags		       */
	enum mats mpval;	/* mailer attribute parameters */
}

%start config

%token 	<phe>	IDENT
%token  <psb>	SCONST
%token  <ival>	ICONST SEPCHAR
%token BIND CANON CLASS CONCAT FOR HEADER HOST HOSTNUM IF IFSET IN
%token MACRO MAILER MAP MARGV MATCH MEOL MFLAGS MMAXSIZE MPATH
%token MRECIPIENT MSENDER NEXT OPTIONS PRECEDENCE READCLASS RESOLVE
%token RETRY RETURN RULESET TRUSTED USER
%token YPALIAS YPMAP YPPASSWD EVAL RESOLVED QUOTE ASM PROGRAM DEFAULT ALIAS
%token DBM

%token ASGN COLON COMMA DEFINE DOLLAR FIELD LBRACE LPAREN RBRACE
%token RPAREN SEMI STAR SLASH

%token AAOPT AOPT BBOPT CCOPT COPT DDOPT DOPT DOPTB DOPTI DOPTQ EOPT
%token EOPTE EOPTM EOPTP EOPTW EOPTZ FFOPT FOPT GOPT HHOPT IOPT LLOPT
%token MOPT NNOPT NOPT OOPT PPOPT QOPT QQOPT ROPT SOPT SSOPT TOPT TTOPT
%token UOPT VOPT WWOPT XOPT XXOPT YOPT YYOPT ZOPT ZZOPT
%token RROPT BOPT SLOPT HOPT IIOPT

%token AAFLAG CCFLAG DDFLAG EEFLAG EFLAG FFFLAG FFLAG HFLAG IIFLAG LFLAG
%token LLFLAG MFLAG MMFLAG NFLAG PFLAG PPFLAG RFLAG RRFLAG SFLAG SSFLAG
%token UFLAG UUFLAG XFLAG XXFLAG
%token HHFLAG VVFLAG BBFLAG

%type	<psb>		mval strval ifcon conval ifres elseres nameset namelist
%type	<psb>		doptid eoptid idlist fcond dlist mflags route mdefs
%type	<psb>		matchaddr matchtok action actionstmt mailerspec mtdef
%type	<psb>		rwaddr rwtok ftype reftok rword cantok resolution
%type	<psb>		dbmtok dbmval dbmvaltok dbmstuff
%type	<psb>		userspec hword hostid dheader mdefine
%type	<psb>		catstring catstringlist canval canvaltok
%type	<ival>		anychar
%type	<phe>		cdef
%type	<optval>	optid
%type	<flgval>	flagid
%type	<mpval>		mvar
%type	<psb>		ifresmatch elseresmatch ifresroute elseresroute 
/* needed special class with no comma's allowed */
%type	<ival>		anycharbutcomma
%type	<psb>		matchaddrnocomma matchtoknocomma

%left COMMA
%left LPAREN RPAREN
%nonassoc SCONST

%%
config		:	/* empty */
		|	config blockdef
		|	error blockdef
		;

blockdef	:	BIND bindings
		|	MACRO macdefs
		|	CLASS classdefs
		|	OPTIONS optdefs
		|	PRECEDENCE precdefs
		|	TRUSTED tlist
		|	HEADER hdefs
		|	MAILER mlist
		|	RULESET rdef
		|	FIELD fdefs
		;

bindings	:	/* empty */
		|	bindings IDENT ASGN RULESET ICONST SEMI {
				BindID ($2, $5, ID_RULESET);
			}
		|	error SEMI {
				yyerrok;
			}
		;

macdefs		:	/* empty */
		|	macdefs IDENT ASGN mdefine SEMI {
				EmitDef (def_macro, $2, $4, (char *) NULL);
			}
		|	error SEMI {
				yyerrok;
			}
		;

/* macro value
 * can be string
 * or ifset()
 * or concat
 */
mdefine	        :       mval {
                        $$ = $1;
			}
		|	IFSET LPAREN IDENT COMMA ifres RPAREN {
				$$ = MakeCond ($3, MacScan($5));
			}
		;
mval		:	strval				%prec COMMA {
				$$ = $1;
			}
		|	CONCAT LPAREN conval RPAREN {
				$$ = $3;
			} 
		;

strval		:	SCONST {
				$$ = $1;
			}
		|	strval SCONST {
				$$ = ListAppend ($1, $2, (char *) NULL);
				Free ($1);
			}
		;

/* conval specifies what can be in a concat() function */
conval		:	strval COMMA ifcon {
				$$ = ListAppend ($1, $3, (char *) NULL);
				Free ($1);
				Free ($3);
			}
		|	ifcon COMMA strval {
				$$ = ListAppend ($1, $3, (char *) NULL);
				Free ($1);
				Free ($3);
			}
		|	ifcon {
				$$ = $1;
			}
		|	error {
				$$ = NULL;
			}
		;

ifcon		:	IFSET LPAREN IDENT COMMA ifres RPAREN {
				$$ = MakeCond ($3, MacScan($5));
			}
		;

ifres		:	mval elseres {
				if ($2 != NULL) {
					$$ = ListAppend ($1, $2, "$|");
					Free ($1);
					Free ($2);
				} else
					$$ = $1;
			}
		|	error {
				$$ = NULL;
			}
		;

elseres		:	/* empty */ {
				$$ = NULL;
			}
		|	COMMA mval {
				$$ = $2;
			}
		;

classdefs	:	/* empty */ 
		|	classdefs IDENT ASGN nameset {
				EmitDef (def_class, $2, $4, (char *) NULL);
			}
                | 	classdefs ASM LPAREN SCONST RPAREN SEMI {
				printf("%s\n",$4);
                        }
		|	error
		;

nameset		:	LBRACE namelist RBRACE SEMI {
				$$ = $2;
			}
		|	LBRACE RBRACE SEMI {
				$$ = NULL;
			}
		|	LBRACE error RBRACE SEMI {
				$$ = NULL;
			}
		|	READCLASS LPAREN strval RPAREN SEMI {
				$$ = MakeRStr ($3, (char *) NULL);
			}
		|	READCLASS LPAREN strval COMMA strval RPAREN SEMI {
				$$ = MakeRStr ($3, $5);
			}
		|	READCLASS LPAREN error RPAREN SEMI {
				$$ = NULL;
			}
		|	error SEMI {
				$$ = NULL;
				yyerrok;
			}
		;

namelist	:	IDENT {
				$$ = ListAppend ($1->psb, (char *) NULL, (char *) NULL);
				RemoveSymbol ($1);
			}
		|	strval {
				$$ = $1;
			}
		|	namelist COMMA IDENT {
				$$ = ListAppend ($1, $3->psb, " ");
				Free ($1);
				RemoveSymbol ($3);
			}
		|	namelist COMMA strval {
				$$ = ListAppend ($1, $3, " ");
				Free ($1);
				Free ($3);
			}
		;

optdefs		:	/* empty */
		|	optdefs optid ASGN strval SEMI {
				EmitDef (def_option, (struct he *) NULL, ConvOpt ($2), $4);
			}
		|	optdefs DOPT ASGN doptid SEMI {
				EmitDef (def_option, (struct he *) NULL, ConvOpt (opt_d), $4);
			}
		|	optdefs EOPT ASGN eoptid SEMI {
				EmitDef (def_option, (struct he *) NULL, ConvOpt (opt_e), $4);
			}
                | 	optdefs ASM LPAREN SCONST RPAREN SEMI {
				printf("%s\n",$4);
                        }
		|	error SEMI {
				yyerrok;
			}
		;

optid		:	AAOPT {
				$$ = opt_A;
			}
		|	AOPT {
				$$ = opt_a;
			}
		|	BBOPT {
				$$ = opt_B;
			}
		|	BOPT {
				$$ = opt_b;
			}
		|	CCOPT {
				$$ = opt_C;
			}
		|	COPT {
				$$ = opt_c;
			}
		|	DDOPT {
				$$ = opt_D;
			}
		|	FFOPT {
				$$ = opt_F;
			}
		|	FOPT {
				$$ = opt_f;
			}
		|	GOPT {
				$$ = opt_g;
			}
		|	HOPT {
				$$ = opt_h;
			}
		|	HHOPT {
				$$ = opt_H;
			}
		|	IOPT {
				$$ = opt_i;
			}
		|	IIOPT {
				$$ = opt_I;
			}
		|	LLOPT {
				$$ = opt_L;
			}
		|	MOPT {
				$$ = opt_m;
			}
		|	NNOPT {
				$$ = opt_N;
			}
		|	NOPT {
				$$ = opt_n;
			}
		|	PPOPT {
				$$ = opt_P;
			}
		|	OOPT {
				$$ = opt_o;
			}
		|	QQOPT {
				$$ = opt_Q;
			}
		|	QOPT {
				$$ = opt_q;
			}
		|	ROPT {
				$$ = opt_r;
			}
		|	RROPT {
				$$ = opt_R;
			}
		|	SSOPT {
				$$ = opt_S;
			}
		|	SOPT {
				$$ = opt_s;
			}
		|	TTOPT {
				$$ = opt_T;
			}
		|	TOPT {
				$$ = opt_t;
			}
		|	UOPT {
				$$ = opt_u;
			}
		|	VOPT {
				$$ = opt_v;
			}
		|	WWOPT {
				$$ = opt_W;
			}
		|	XOPT {
				$$ = opt_x;
			}
		|	XXOPT {
				$$ = opt_X;
			}
		|	YOPT {
				$$ = opt_y;
			}
		|	YYOPT {
				$$ = opt_Y;
			}
		|	ZOPT {
				$$ = opt_z;
			}
		|	ZZOPT {
				$$ = opt_Z;
			}
		|	SLOPT {
				$$ = opt_SL;	/* SLASH .e.g. O/ in IDA */
			}
		;

doptid		:	DOPTI {
				$$ = ConvOpt (d_opt_i);
			}
		|	DOPTB {
				$$ = ConvOpt (d_opt_b);
			}
		|	DOPTQ {
				$$ = ConvOpt (d_opt_q);
			}
		;

eoptid		:	EOPTP {
				$$ = ConvOpt (e_opt_p);
			}
		|	EOPTE {
				$$ = ConvOpt (e_opt_e);
			}
		|	EOPTM {
				$$ = ConvOpt (e_opt_m);
			}
		|	EOPTW {
				$$ = ConvOpt (e_opt_w);
			}
		|	EOPTZ {
				$$ = ConvOpt (e_opt_z);
			}
		;

precdefs	:	/* empty */
		|	precdefs IDENT ASGN ICONST SEMI {
				BindID ($2, $4, ID_PREC);
				EmitDef (def_prec, $2, (char *) NULL, (char *) NULL);
			}
		;

tlist		:	/* empty */
		|	tlist LBRACE IDENT idlist RBRACE SEMI {
				EmitDef (def_trusted, (struct he *) NULL, 
					 ListAppend ($3->psb, $4, " "), (char *) NULL);
				Free ($4); /* Gets a Null pointer */
				RemoveSymbol ($3);
			}
		|	tlist LBRACE RBRACE SEMI
		|	error SEMI {
				yyerrok;
			}
		;

hdefs		:	/* empty */
		|	hdefs FOR fcond dheader SEMI {
				EmitDef (def_header, (struct he *) NULL, $3, $4);
			}
		|	hdefs FOR fcond LBRACE { Flaglist = $3; } 
				dheaders RBRACE SEMI
		|	hdefs DEFINE dlist SEMI {
				EmitDef (def_header, (struct he *) NULL, (char *) NULL, $3);
			}
		|	error SEMI {
				yyerrok;
			}
		;

fcond		:	LPAREN RPAREN {
				$$ = NULL;
			}
		|	LPAREN mflags RPAREN {
				$$ = $2;
			}
		|	LPAREN error RPAREN {
				$$ = NULL;
			}
		;

mflags		:	flagid {
				$$ = ListAppend (ConvFlg ($1), (char *) NULL, (char *) NULL);
			}
		|	mflags COMMA flagid {
				$$ = ListAppend ($1, ConvFlg($3), (char *) NULL);
				Free ($1);
			}
		;

flagid		:	FFLAG {
				$$ = flg_f;
			}
		|	RFLAG {
				$$ = flg_r;
			}
		|	SSFLAG {
				$$ = flg_S;
			}
		|	NFLAG {
				$$ = flg_n;
			}
		|	LFLAG {
				$$ = flg_l;
			}
		|	SFLAG {
				$$ = flg_s;
			}
		|	MFLAG {
				$$ = flg_m;
			}
		|	FFFLAG {
				$$ = flg_F;
			}
		|	DDFLAG {
				$$ = flg_D;
			}
		|	MMFLAG {
				$$ = flg_M;
			}
		|	XFLAG {
				$$ = flg_x;
			}
		|	PPFLAG {
				$$ = flg_P;
			}
		|	UFLAG {
				$$ = flg_u;
			}
		|	HFLAG {
				$$ = flg_h;
			}
		|	AAFLAG {
				$$ = flg_A;
			}
		|	BBFLAG {
				$$ = flg_B;
			}
		|	UUFLAG {
				$$ = flg_U;
			}
		|	EFLAG {
				$$ = flg_e;
			}
		|	XXFLAG {
				$$ = flg_X;
			}
		|	LLFLAG {
				$$ = flg_L;
			}
		|	PFLAG {
				$$ = flg_p;
			}
		|	IIFLAG {
				$$ = flg_I;
			}
		|	CCFLAG {
				$$ = flg_C;
			}
		|	EEFLAG {
				$$ = flg_E;
			}
		|	RRFLAG {
				$$ = flg_R;
			}
		|	HHFLAG {
				$$ = flg_H;
			}
		|	VVFLAG {
				$$ = flg_V;
			}
		;

dheader		:	/* empty */ {
				$$ = NULL;
			}
		|	DEFINE dlist {
				$$ = $2;
			}
		|	error {
				$$ = NULL;
			}
		;

dheaders	:	/* empty */
		|	dheaders DEFINE dlist SEMI {
				EmitDef (def_header, (struct he *) NULL, Flaglist, $3);
			}
                | 	dheaders ASM LPAREN SCONST RPAREN SEMI {
				printf("%s\n",$4);
                        }
		|	error
		;

dlist		:	LPAREN strval COMMA catstringlist RPAREN {
				$$ = ListAppend ($2, MacScan ($4), " ");
				Free ($2);
				Free ($4);
			}
		|	LPAREN error RPAREN {
				$$ = NULL;
			}
		;

catstringlist	:	catstring {
  				$$ = $1;
			}
		|	catstring COMMA catstringlist {
  				$$ = ListAppend( $1, $3, (char *) NULL);
  				Free($1);
			}
catstring	:	SCONST {
				$$ = $1;
			}
		|	CONCAT LPAREN conval RPAREN {
				$$ = $3;
			}
		|	ifcon {
				$$ = $1;
			}
		;

mlist		:	/* empty */
		|	mlist IDENT LBRACE mdefs RBRACE SEMI {
				EmitDef (def_mailer, $2, $4, (char *) NULL);
			}
		|	mlist IDENT LBRACE RBRACE SEMI {
				EmitDef (def_mailer, $2, (char *) NULL, (char *) NULL);
			}
		|	error SEMI {
				yyerrok;
			}
		;

mdefs		:	mtdef {
				$$ = $1;
			}
		|	mdefs COMMA mtdef {
				$$ = ListAppend ($1, $3, ", ");
				Free ($1);
				Free ($3);
			}
		;	

mtdef		:	mvar ASGN mval {
				$$ = ListAppend (ConvMat ($1), MacScan ($3), "=");
				Free ($3);
			}
		|	MFLAGS ASGN LBRACE mflags RBRACE {
				$$ = ListAppend (ConvMat (mat_flags), $4, "=");
			}
		|	MSENDER ASGN IDENT {
				$$ = ListAppend (ConvMat (mat_sender), CheckRS ($3), "=");
			}
		|	MSENDER ASGN IDENT SLASH IDENT {
				$$ = ListAppend(
				 Tsb = ListAppend (ConvMat(mat_sender), CheckRS ($3), "="),
				 Tsb1 = ListAppend ("/", CheckRS ($5), (char *) NULL),
						 (char *) NULL);
				Free (Tsb);
				Free (Tsb1);
			}
		|	MRECIPIENT ASGN IDENT {
				$$ = ListAppend (ConvMat (mat_recipient), CheckRS ($3), "=");
			}
		|	MRECIPIENT ASGN IDENT SLASH IDENT {
				$$ = ListAppend(
				 Tsb = ListAppend (ConvMat(mat_recipient), CheckRS ($3), "="),
				 Tsb1 = ListAppend ("/", CheckRS ($5), (char *) NULL),
						 (char *) NULL);
				Free (Tsb);
				Free (Tsb1);
			}
		|	error {
				$$ = NULL;
			}
		;

mvar		:	MPATH {
				$$ = mat_path;
			}
		|	MARGV {
				$$ = mat_argv;
			}
		|	MEOL {
				$$ = mat_eol;
			}
		|	MMAXSIZE {
				$$ = mat_maxsize;
			}
		;

rdef		:	/* empty */
		|	rdef IDENT { StartRuleset ($2); } rulelist
		;

rulelist	:	LBRACE ruledefs RBRACE {
				RMatch = FALSE;
			}
		|	error {
				RMatch = FALSE;
			}
		;

ruledefs	:	/* empty */ {
				RMatch = TRUE;
			}
		|	ruledefs IF LPAREN matchaddr RPAREN actionstmt {
  				number_of_fields	=0;
				EmitDef (def_ruleset, (struct he *) NULL, 
					 ListAppend ($4, $6, "\t"), (char *) NULL);
			Free ($4);
			Free ($6);
			}
                | 	ruledefs ASM LPAREN SCONST RPAREN SEMI {
				  printf("%s\n",$4);
                        }
		|	error SEMI {
				yyerrok;
			}
		;

matchaddr	:	/* empty */ {
				$$ = NULL;
			}
		|	matchaddr matchtok {
				$$ = ListAppend ($1, $2, (char *) NULL);
				Free ($1); /* NULL */
			}
		|	error {
				$$ = NULL;
			}
		;

/* just like matchaddr - but comma's aren't allowed */
matchaddrnocomma	:	/* empty */ {
				$$ = NULL;
			}
		|	matchaddrnocomma matchtoknocomma {
				$$ = ListAppend ($1, $2, (char *) NULL);
				Free ($1); /* NULL */
			}
		|	error {
				$$ = NULL;
			}
		;

matchtok	:	IDENT {
				$$ = GetField ($1);
			}
		|	anychar {
				*Cbuf = $1;
				$$ = ListAppend (Cbuf, (char *) NULL, (char *) NULL);
			}
		|	mval {
				$$ = MacScan ($1);
			}
		|	DOLLAR IDENT {
				Mbuf[1] = MakeMac ($2, ID_MACRO);
				$$ = ListAppend (Mbuf, (char *) NULL, (char *) NULL);
			}
                |       YPALIAS LPAREN matchtok RPAREN {
		         $$ = ListAppend("${",$3,(char *) NULL);
/*			 Free ($3); */
		}
                |       YPPASSWD LPAREN matchtok RPAREN {
		         $$ = ListAppend("$\"",$3,(char *) NULL);
		}
                |       RESOLVED LPAREN matchtok RPAREN {
		         $$ = ListAppend("$#",$3,(char *) NULL);
		}
		|	IFSET LPAREN IDENT COMMA ifresmatch RPAREN {
				$$ = MakeCond ($3, MacScan($5));
			}
	;

/* the next one is just like matchtok - but you can't have a comma in it */
matchtoknocomma	:	IDENT {
				$$ = GetField ($1);
			}
		|	anycharbutcomma {
				*Cbuf = $1;
				$$ = ListAppend (Cbuf, (char *) NULL, (char *) NULL);
			}
		|	mval {
				$$ = MacScan ($1);
			}
		|	DOLLAR IDENT {
				Mbuf[1] = MakeMac ($2, ID_MACRO);
				$$ = ListAppend (Mbuf, (char *) NULL, (char *) NULL);
			}
                |       YPALIAS LPAREN matchtok RPAREN {
		         $$ = ListAppend("${",$3,(char *) NULL);
/*			 Free ($3); */
		}
                |       YPPASSWD LPAREN matchtok RPAREN {
		         $$ = ListAppend("$\"",$3,(char *) NULL);
		}
                |       RESOLVED LPAREN matchtok RPAREN {
		         $$ = ListAppend("$#",$3,(char *) NULL);
		}
		|	IFSET LPAREN IDENT COMMA ifresmatch RPAREN {
				$$ = MakeCond ($3, MacScan($5));
			}
	;

ifresmatch	:	matchaddrnocomma elseresmatch {
				if ($2 != NULL) {
					$$ = ListAppend ($1, $2, "$|");
					Free ($1);
					Free ($2);
				} else
					$$ = $1;
			}
		|	error {
				$$ = NULL;
			}
		;

elseresmatch	:
			COMMA matchaddrnocomma {
				$$ = $2;
			}
		;

actionstmt	:	action LPAREN rwaddr RPAREN SEMI {
				$$ = ListAppend ($1, $3, (char *) NULL);
				Free ($3);
			}
		|	RESOLVE LPAREN resolution RPAREN SEMI {
				$$ = $3;
			}
		|	error SEMI {
				$$ = NULL;
				yyerrok;
			}
		;

action		:	RETRY {
				$$ = NULL;
			}
		|	NEXT {
				$$ = "$:";
			}
		|	RETURN {
				$$ = "$@";
			}
		;

rwaddr		:	/* empty */ {
				$$ = NULL;
			}
		|	rwaddr rwtok {
				$$ = ListAppend ($1, $2, (char *) NULL);
				Free ($1);	/* NULL */
			}
		|	rwaddr IDENT LPAREN rwaddr RPAREN {
				$$ = ListAppend ($1, (Tsb = MakeRSCall ($2, $4)), (char *) NULL);
				Free ($1);	/* NULL */
				Free ($4);
				Free (Tsb);
			}
		|	error {
				$$ = NULL;
			}
		;

rwtok		:	anychar {
				*Cbuf = $1;
				$$ = ListAppend (Cbuf, (char *) NULL, (char *) NULL);
			}
		|	mval {
				$$ = MacScan ($1);
			}
		|	cantok {
				$$ = $1;
			}
		|	dbmtok {
				$$ = $1;
			}
		|	ALIAS LPAREN reftok  RPAREN {
				$$ = ListAppend("$(@", $3, "$:$)");
			}
		|	ALIAS LPAREN reftok DEFAULT LPAREN rwaddr RPAREN RPAREN {
				$$ = ListAppend(Tsb = 
						ListAppend ( "$(@", 
							    $3, 
							    (char *)NULL),
						Tsb1 = ListAppend("$:", $6, "$)" ),
						(char *) NULL);
				Free (Tsb);
				Free (Tsb1);
				Free ($3);
				Free ($6);
			}
		|	reftok {
				$$ = $1;
			}
		|	ifcon {
				$$ = $1;
			}
                |	YPMAP LPAREN IDENT COMMA rwaddr RPAREN {
				*Cbuf = MakeMac ($3, ID_MACRO);
				$$ = ListAppend(Tsb = ListAppend ("${", (char *)Cbuf, (char *)NULL),
						Tsb1 = ListAppend ($5, "$}", (char *) NULL),
						 (char *) NULL);
				Free (Tsb);
				Free (Tsb1);
				Free ($5);
			}

                |	PROGRAM LPAREN IDENT COMMA rwaddr RPAREN {
				*Cbuf = MakeMac ($3, ID_MACRO);
				$$ = ListAppend(Tsb = ListAppend ("$<", (char *)Cbuf, (char *)NULL),
						Tsb1 = ListAppend ($5, "", (char *) NULL),
						 (char *) NULL);
				Free (Tsb);
				Free (Tsb1);
				Free ($5);
			}

                |	action LPAREN rwaddr RPAREN {
				$$ = ListAppend ($1, $3, (char *) NULL);
				Free ($3);
			}
		;


cantok		:	CANON LPAREN canval RPAREN {
				$$ = Bracket ($3, TRUE);
				Free ($3);
			}

                ;
canval		:	canvaltok {
				$$ = $1;
			}
		|	canval canvaltok {
				$$ = ListAppend ($1, $2, (char *) NULL);
				Free ($1);
/*				Free ($2); */
			}
		;

canvaltok	:	IDENT {
				$$ = ListAppend ($1->psb, (char *) NULL, (char *) NULL);
				RemoveSymbol ($1);
			}
		|	SCONST {
				$$ = ListAppend (MacScan ($1), (char *) NULL, (char *) NULL);
				Free ($1);
			}
                |	NEXT LPAREN RPAREN {	/* I Used next earlier, but now use default - because it is clearer syntax */
				$$ = "$:";
			}
                |	NEXT LPAREN canval RPAREN {
				$$ = ListAppend("$:", $3, (char *)NULL);
			}
                |	DEFAULT LPAREN RPAREN {
				$$ = "$:";
			}
                |	DEFAULT LPAREN canval RPAREN {
				$$ = ListAppend("$:", $3, (char *)NULL);
			}
		|	reftok {
				$$ = $1;
			}
		|	SEPCHAR {
				*Cbuf = $1;
				$$ = ListAppend (Cbuf, (char *) NULL, (char *) NULL);
			}
		|	HOSTNUM LPAREN reftok RPAREN {
				$$ = Bracket ($3, FALSE);
				Free ($3);
			}
		;

dbmtok		:	DBM LPAREN DOLLAR IDENT COMMA dbmstuff RPAREN {
				$$ = DbmParen ($4->psb, $6);
				RemoveSymbol($4);
				Free ($6);
			}
                ;
dbmstuff	:	dbmval COMMA dbmval {
				$$ = ListAppend ($1, Tsb = ListAppend ("$@", $3, (char *) NULL),
						 (char *) NULL);
				Free (Tsb);
			}
		|		dbmval {
				$$ = $1;
			}
		;

dbmval		:	dbmvaltok {
				$$ = $1;
			}
		|	dbmval dbmvaltok {
				$$ = ListAppend ($1, $2, (char *) NULL);
				Free ($1);
/*				Free ($2); */
			}
		;

dbmvaltok	:	IDENT {
				$$ = ListAppend ($1->psb, (char *) NULL, (char *) NULL);
				RemoveSymbol ($1);
			}
		|	SCONST {
				$$ = ListAppend (MacScan ($1), (char *) NULL, (char *) NULL);
				Free ($1);
			}
                |	NEXT LPAREN RPAREN {	/* I Used next earlier, but now use default - because it is clearer syntax */
				$$ = "$:";
			}
                |	NEXT LPAREN dbmval RPAREN {
				$$ = ListAppend("$:", $3, (char *)NULL);
			}
                |	DEFAULT LPAREN RPAREN {
				$$ = "$:";
			}
                |	DEFAULT LPAREN dbmval RPAREN {
				$$ = ListAppend("$:", $3, (char *)NULL);
			}
		|	reftok {
				$$ = $1;
			}
		|	SEPCHAR {
				*Cbuf = $1;
				$$ = ListAppend (Cbuf, (char *) NULL, (char *) NULL);
			}
		;

reftok		:	DOLLAR IDENT {
				Mbuf[1] = MakeMac ($2, ID_MACRO);
				$$ = ListAppend (Mbuf, (char *) NULL, (char *) NULL);
			}
		|	DOLLAR ICONST {
				$$ = ListAppend (MakePosTok ($2), (char *) NULL, (char *) NULL);
			}
                |	EVAL LPAREN IDENT RPAREN {
				*Cbuf = MakeMac ($3, ID_MACRO);
				$$ = ListAppend("$&", (char *)Cbuf, (char *)NULL);
			}
		;


/* because of ifset, we have to treat comma's special
 otherwise ifset(a,b,c) might be ambiguous if
 c is an address with a comma in it.
*/
anychar		:	anycharbutcomma {
  				$$ = $1;
		}
  		|	COMMA {
				$$ = ',';
			}

		;

anycharbutcomma		:	SEPCHAR {
				$$ = $1;
			}
		|	COLON {
				$$ = ':';
			}
		|	STAR {
				$$ = '*';
			}
		|	SEMI {
				$$ = ';';
			}
		|	LBRACE {
				$$ = '{';
			}
		|	RBRACE {
				$$ = '}';
			}
		|	SLASH {
				$$ = '/';
			}
		|	ASGN {
				$$ = '=';
			}
		;

resolution	:	mailerspec COMMA route {
				$$ = ListAppend ($1, $3, (char *) NULL);
				Free ($1);
				Free ($3);
			}
		|	mailerspec {
			$$ = $1;
			}
		|	error {
				$$ = NULL;
			}
		;

mailerspec	:	MAILER LPAREN rword RPAREN {
				$$ = ListAppend ("$#", $3, (char *) NULL);
			}
		;

route		:	HOST LPAREN hword RPAREN COMMA userspec {
				$$ = ListAppend (Tsb = ListAppend ("$@", $3, (char *) NULL),
						 $6, (char *) NULL);
				Free (Tsb);


				Free ($6);
			}
		|	userspec {
				$$ = $1;
			}
		|	IFSET LPAREN IDENT COMMA ifresroute RPAREN {
				$$ = MakeCond ($3, MacScan($5));
			}
		;

ifresroute		:	LPAREN route RPAREN  elseresroute {
				if ($4 != NULL) {
					$$ = ListAppend ($2, $4, "$|");
					Free ($2);
					Free ($4);
				} else
					$$ = $2;
			}
		|	error {
				$$ = NULL;
			}
		;

elseresroute	:
			COMMA LPAREN route RPAREN {
				$$ = $3;
			}
		;

hword		:	hostid {
				$$ = $1;
			}
		|	HOSTNUM LPAREN reftok RPAREN {
				$$ = Bracket ($3, FALSE);
				Free ($3);
			}
		;

hostid		:	/* empty */ {
				$$ = NULL;
			}
		|	hostid IDENT {
				$$ = ListAppend ($1, $2->psb, (char *) NULL);
				RemoveSymbol ($2);
				Free ($1);
			}
		|	hostid rwtok {
				$$ = ListAppend ($1, $2, (char *) NULL);
				Free ($1);	/* NULL */
			}
		;

userspec	:	USER LPAREN rwaddr RPAREN {
				$$ = ListAppend ("$:", $3, (char *) NULL);
				Free ($3);
			}
		;

rword		:	IDENT {
				$$ = CheckMailer ($1);
			}
		|	reftok {
				$$ = $1;
			}
		;

fdefs		:	/* empty */
		|	fdefs IDENT idlist COLON ftype SEMI {
				AssignType (ListAppend ($2->psb, $3, " "), $5);
				Free ($3);	/* NULL */
			}
		|	error SEMI {
				yyerrok;
			}
		;

idlist		:	/* empty */ {
				$$ = NULL;
			}
		|	idlist COMMA IDENT {
				$$ = ListAppend ($1, $3->psb, " ");
				Free ($1);
			}
		;

ftype		:	MATCH LPAREN ICONST RPAREN cdef {
				$$ = ListAppend (MakeField ($3, $5, FALSE, FALSE), 
				    		 (char *) NULL, (char *) NULL);
			}
		|	MATCH LPAREN ICONST RPAREN MAP IDENT {
				$$ = ListAppend (MakeField ($3, $6, FALSE, TRUE), 
				    		 (char *) NULL, (char *) NULL);
			}
		|	MATCH HOST {
				$$ = ListAppend ("$%y", 
				    		 (char *) NULL, (char *) NULL);
			}
		|	MATCH LPAREN ICONST STAR RPAREN {
				$$ = ListAppend (MakeField ($3, (struct he *) NULL, TRUE, FALSE), 
						 (char *) NULL, (char *) NULL);
			}
		|	error {
				$$ = NULL;
			}
		;

cdef		:	/* empty */ {
				$$ = NULL;
			}
		|	IN IDENT {
				$$ = $2;
			}
		;
