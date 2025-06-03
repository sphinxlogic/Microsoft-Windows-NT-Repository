%{

#ifdef FLUKE
# ifndef LINT
    static char RCSid[] = "@(#)FLUKE  $Header: /home/kreskin/u0/barnett/Src/ease/src/RCS/lexan.l,v 3.2 1991/05/16 10:45:25 barnett Exp $";
# endif LINT
#endif FLUKE

/*
 *	lexan.l -- Lexical Analyzer for EASE.
 *
 *		   Contains code for lex(1) which generates a lexical
 *		   analyzer (lex.yy.c) for Ease, a high-level specification 
 *		   format for sendmail configuration files.
 *
 *	author -- James S. Schoner, Purdue University Computing Center,
 *				    West Lafayette, Indiana  47907
 *
 *	date   -- July 1, 1985
 *
 *	Copyright (c) 1985 by Purdue Research Foundation
 *
 *	All rights reserved.
 *
 * $Log: lexan.l,v $
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
 * Revision 2.3  1991/02/12  20:49:34  barnett
 * Added several new tokens.
 * Merged Jeff's changes with my own.
 *
 * Revision 2.2  1990/05/07  11:12:53  jeff
 * Add support for the "Cflag" variable which controls whether or not
 * input lines are passed through as comments in the output stream.
 *
 * Version 2.1  90/01/30  15:26:23  jeff
 * If -C flag is specified, emit input as comments in the output stream.
 * 
 * Revision 2.0  88/06/15  15:11:30  root
 * Baseline release for net posting. ADR.
 * 
 */

#include "fixstrings.h"
#include "symtab.h"
#include "lexdefs.h"

#define  LEXnewline '\n'
#define	 LEXeof	    '\0'
#define  MaxFN	    200			/* maximum file name length */

extern int	  EchoInputAsComments;
extern struct he *LookupSymbol ();
extern void	  ErrorReport ();
extern void	  yymark();
extern void	  PrintError();
#ifndef FLEX_SCANNER					    /* Flex defines this (differently) */
extern char *	  malloc ();				    /* Greg Lehey, LEMIS, 19 August 1993 */
#endif

int  Lcount;				/* line counter		    */
char FNbuf[MaxFN];			/* file name buffer	    */
short RMatch  = FALSE;			/* ruleset match flag  	    */

#ifdef YYDEBUG
extern int yychar;
#else
int   yychar;
#endif

#ifndef FLEX_SCANNER
#undef input
# define input() (((yytchar=yychar=yysptr>yysbuf?U(*--yysptr):Getc(yyin,yyout))==10?(yylineno++,yytchar):yytchar)==EOF?0:yytchar)
#endif
int
Getc (yyin, yyout)
	FILE *yyin, *yyout;
{
	static char linbuf[BUFSIZ], *pc = linbuf;
	char c;

	/* initialize buffer: first call only */
	if (*pc == '\0' && pc == linbuf) {
		if (fgets(linbuf, BUFSIZ, yyin)==NULL)
			return EOF;
		/* echo input as comment */
		if (EchoInputAsComments) {
		    fprintf(yyout, "%s%s", (linbuf[0] == '#' ? "" : "# "), linbuf);
		}
	}
	c = *pc++;
	if (c == '\n') {
		pc = linbuf;
		if (fgets(linbuf, BUFSIZ, yyin) == NULL)
			*pc = (char) EOF;
		else
			/* echo input as comment (except cpp comments) */
			if (EchoInputAsComments) {
			    fprintf(yyout, "%s%s",
				(linbuf[0] == '#' ? "" : "# "), linbuf);
			}
	}
 	return (c == (char) EOF) ? EOF : c;
}

/*
 * Table of keywords. NOTE: This is in sorted order, and
 * must stay that way if anything else is added to it.
 */
static struct resword {
	char	*r_text;
	int	r_tokval;
} reswords[] = {
	{ "Argv",		MARGV },
	{ "Eol",		MEOL },
	{ "Flags",		MFLAGS },
	{ "Maxsize",		MMAXSIZE },
	{ "Path",		MPATH },
	{ "Recipient",		MRECIPIENT },
	{ "Sender",		MSENDER },
 	{ "alias",		ALIAS },
 	{ "asm",		ASM },
	{ "bind",		BIND },
	{ "canon",		CANON },
	{ "class",		CLASS },
	{ "concat",		CONCAT },
	{ "d_background",	DOPTB },
	{ "d_interactive",	DOPTI },
	{ "d_queue",		DOPTQ },
	{ "dbm",		DBM },	   /* IDA */
	{ "default",		DEFAULT }, /* IDA */
	{ "define",		DEFINE },
 	{ "eval",		EVAL },
	{ "f_addrw",		CCFLAG },
	{ "f_arpa",		AAFLAG },
 	{ "f_bsmtp",		BBFLAG },	/* IDA */
	{ "f_date",		DDFLAG },
	{ "f_dot",		XXFLAG },
	{ "f_escape",		EEFLAG },
	{ "f_expensive",	EFLAG },
	{ "f_ffrom",		FFLAG },
	{ "f_from",		FFFLAG },
	{ "f_full",		XFLAG },
	{ "f_llimit",		LLFLAG },
	{ "f_locm",		LFLAG },
	{ "f_mail11",		HHFLAG },  /* Ultrix */
	{ "f_mesg",		MMFLAG },
	{ "f_mult",		MFLAG },
	{ "f_noreset",		SSFLAG },
	{ "f_noufrom",		NFLAG },
 	{ "f_relativize",	VVFLAG },	/* IDA */
	{ "f_retsmtp",		PFLAG },
	{ "f_return",		PPFLAG },
	{ "f_rfrom",		RFLAG },
	{ "f_rport",		RRFLAG },
	{ "f_smtp",		IIFLAG },
	{ "f_strip",		SFLAG },
	{ "f_ufrom",		UUFLAG },
	{ "f_upperh",		HFLAG },
	{ "f_upperu",		UFLAG },
	{ "field",		FIELD },
	{ "for",		FOR },
	{ "h_exit",		EOPTE },
	{ "h_mail",		EOPTM },
	{ "h_mailz",		EOPTZ },
	{ "h_print",		EOPTP },
	{ "h_write",		EOPTW },
	{ "header",		HEADER },
	{ "host",		HOST },
	{ "hostnum",		HOSTNUM },
	{ "if",			IF },
	{ "ifset",		IFSET },
	{ "in",			IN },
	{ "macro",		MACRO },
	{ "mailer",		MAILER },
	{ "map",		MAP },
	{ "match",		MATCH },
	{ "next",		NEXT },
	{ "o_alias",		AAOPT },
 	{ "o_aliasfile",	YYOPT },	/* SunOS */
	{ "o_bsub",		BBOPT },
	{ "o_checkpoint",	CCOPT },
	{ "o_delivery",		DOPT },
	{ "o_dmuid",		UOPT },
	{ "o_dnet",		NNOPT },
 	{ "o_envelope",		SLOPT },	/* IDA */
	{ "o_ewait",		AOPT },
	{ "o_flog",		SSOPT },
	{ "o_fsmtp",		HHOPT },
	{ "o_gid",		GOPT },
	{ "o_handling",		EOPT },
	{ "o_hformat",		OOPT },
	{ "o_loadnc",		XXOPT },
	{ "o_loadq",		XOPT },
	{ "o_maxempty",		BOPT },	/* SunOS */
 	{ "o_maxhops",		HOPT }, /* SunOS */
  	{ "o_nameserver",	IIOPT }, /* HP/UX */
  	{ "o_newproc",		YYOPT },
 	{ "o_nfs",		RROPT },	/* SunOS 4.0 */
	{ "o_pmaster",		PPOPT },
	{ "o_prifactor",	ZOPT },
	{ "o_qdir",		QQOPT },
	{ "o_qfactor",		QOPT },
	{ "o_qtimeout",		TTOPT },
	{ "o_qwait",		COPT },
	{ "o_rebuild",		DDOPT },
	{ "o_recipfactor",	YOPT },
	{ "o_rsend",		MOPT },
	{ "o_safe",		SOPT },
	{ "o_skipd",		IOPT },
	{ "o_slog",		LLOPT },
	{ "o_timezone",		TOPT },
	{ "o_tmode",		FFOPT },
	{ "o_tread",		ROPT },
	{ "o_usave",		FOPT },
	{ "o_validate",		NOPT },
	{ "o_verbose",		VOPT },
	{ "o_waitfactor",	ZZOPT },
	{ "o_wizpass",		WWOPT },
	{ "options",		OPTIONS },
	{ "precedence",		PRECEDENCE },
 	{ "program",		PROGRAM },	/* HP/UX */
 	{ "quote",		QUOTE },
	{ "readclass",		READCLASS },
	{ "resolve",		RESOLVE },
 	{ "resolved",		RESOLVED },
	{ "retry",		RETRY },
	{ "return",		RETURN },
	{ "ruleset",		RULESET },
	{ "trusted",		TRUSTED },
	{ "user",		USER },
	{ "while",		IF },
 	{ "ypalias",		YPALIAS },	/* Ultrix */
 	{ "ypmap",		YPMAP },	/* SunOS */
 	{ "yppasswd",		YPPASSWD },	/* Ultrix */
};
%}

%%
	int INch;			/* any input character */

[ \t\f]+			; 	/* discard whitepsace  */
[\n]				Lcount++;
^\#[ \t]*[0-9]+[ \t]*\".*\"[ \t]*.*[\n]	{
/*			        sscanf (yytext, "%*c%d%s", &Lcount, FNbuf); */
	                        yymark();
			        }
[A-Za-z_][A-Za-z0-9_-]*		{
				register int l, h, m, r, c;

				l = 0;
				h = (sizeof (reswords) / sizeof(reswords[0])) - 1;
				while (l <= h) {
					m = (h + l) / 2;
					c = yytext[0] - reswords[m].r_text[0];
					r = c ? c : strcmp (yytext, reswords[m].r_text);
					if (r < 0)
						h = m - 1;
					else if (r > 0)
						l = m + 1;
					else
						return reswords[m].r_tokval;
				}

				/* not a keyword */

				/* store identifiers in symbol table */
				yylval.phe = LookupSymbol (yytext);
				return (IDENT);
				}
["]((\\\n)|(\\\")|[^"\n])*	{
				if ((INch = input()) == LEXnewline) {
					ErrorReport ("End of line in string.\n");
					unput (INch);
				}
				fixquotes ();
				yylval.psb = (char *) malloc (strlen (yytext) + 1);
				strcpy (yylval.psb, yytext + 1);
				return (SCONST);
				}
[0][0-7]*			{
				sscanf (yytext, "%o", &yylval.ival);  /* octal constant */
				return (ICONST);
				}
[-]?[1-9][0-9]*			{
				yylval.ival = atoi (yytext);
				return (ICONST);
				}
"="				return (ASGN);
","				return (COMMA);
"{"				return (LBRACE);
"}"				return (RBRACE);
"("				return (LPAREN);
")"				return (RPAREN);
";"				return (SEMI);
"$"				return (DOLLAR);
":"				return (COLON);
"*"				return (STAR);
"/*"				{
				/* eat C comments */
				INch = input ();
				while ((INch != '*') || 
				      ((INch = input ()) != '/')) {
					if (INch == LEXnewline)
						Lcount++;
					else
						if (INch == LEXeof) {
							ErrorReport ("End of file in comment.\n");
							break;
						}
					if (INch != '*')
						INch = input ();
				}
				}
"/"				return (SLASH);
[\\]?.				{
				if (RMatch) {	/* in rulesets, return literal character */
					yylval.ival = (yytext[0] == '\\') ? yytext[1] : yytext[0];
					return (SEPCHAR);
				} else {
					PrintError ("Illegal delimiter character: (octal code) \\%03o", *yytext);
				}
				}
%%

/*
 * fixquotes --- inside a "quoted string", turn `\"' into just `"'
 *
 * this is most useful inside the Argv strings for mailers,
 * particularly when debugging.
 */

fixquotes ()
{
	register char *cp1, *cp2;

	cp1 = cp2 = yytext;
	while (*cp2) {
		/*
		 * if we really wanted to get fancy,
		 * at this point we'd handle C escapes,
		 * but I don't think it's necessary.
		 */
		if (*cp2 == '\\' && cp2[1] == '"')
			cp2++;
		*cp1++ = *cp2++;
	}
	*cp1++ = *cp2++;	/* final '\0' */
}
