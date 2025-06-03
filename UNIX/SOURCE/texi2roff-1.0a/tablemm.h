/*
 * tablemm.h - -mm macros table and other explicit troff command strings
 *	     Release 1.0a	August 1988
 *
 * Copyright 1988  Beverly A.Erlebacher
 * erlebach@csri.toronto.edu	...uunet!utai!utcsri!erlebach
 *
 */

/* 
 * string to emit before the first input. this is the place to put troff
 * commands controlling default point size, margin size, line length, etc.
 */
char mminit[] =  ".PH '''\n.SA 1\n";

/*
 * miscellaneous commands, described in texi2roff.h
 */
struct misccmds mmcmds = { "\n.P 1\n", "\n.P 0\n", "", "", "\""};

/*
 * patches for table entries for -I option
 */
struct tablerecd mmnodeentry = {
    "@node",	"\n", "\n.P 0\n\\s8Node:", "\\s0\n.br\n", PARAGRAPH};

struct tablerecd mmmenuentry = {
    "@menu",	"@end", "\n.DS L\n\\s8Menu: \n", "\\s0\n.DE\n", DISPLAY}; 

struct tablerecd mmifinfoentry = {
    "@ifinfo",	"@end", "\n", "\n", PARAGRAPH}; 

struct tablerecd mmtable[] = {
    "@*",		"",	"\n.br\n",	"",		ESCAPED,
    "@.",		"",	"\\&.",		"",		ESCAPED,
    "@:",		"",	"",		"",		ESCAPED,
    "@@",		"",	"@",		"",		ESCAPED,
    "@TeX{",		"}",	"TeX",		"",		INPARA,
    "@appendix",	"\n",	"\n.bp\n.H ",	"\n.br\n",	HEADING,
    "@appendixsec",	"\n",	"\n.H ",	"\n.br\n",	HEADING,
    "@appendixsubsec",	"\n",	"\n.H ",	"\n.br\n",	HEADING,
    "@appendixsubsubsec","\n",	"\n.H ",	"\n.br\n",	HEADING,
    "@asis",		"\n",	"",		"",		PARAGRAPH,
    "@asis{",		"}",	"",		"",		INPARA,
    "@br",		"\n",	"\n.br\n.sp\n", "",		PARAGRAPH,
    "@bullet",		"\n",	"\\(bu",	"",		CHAR,
    "@bullet{",		"}",	"\\(bu",	"",		CHAR,
    "@bye",		"\n",	"",		"",		DISCARD,
    "@b{",		"}",	"\\fB",		"\\fR",		INPARA,
    "@c",		"\n",	"",		"",		DISCARD,
    "@center",		"\n",	"\n.ce\n",	"",		HEADING,
    "@chapter",		"\n",	"\n.bp\n.H 1 ", "\n.br\n",	HEADING,
    "@cindex",		"\n",	"",		"",		DISCARD,
    "@cite{",		"}",	"\\fI",		"\\fR",		INPARA,
    "@code{",		"}",	"\\fB",		"\\fR",		INPARA,
    "@comment",		"\n",	"",		"",		DISCARD,
    "@contents",	"\n",	"",		"",		DISCARD,
    "@copyright{",	"}",	"\\(co",	"",		CHAR,
    "@ctrl{",		"}",	"\\f(CW^",	"\\fR",		INPARA,
    "@dag",		"}{",	"\\(dg",	"",		CHAR,
    "@dag{",		"}",	"\\(dg",	"",		CHAR,
    "@ddag",		"}{",	"\\(dd",	"",		CHAR,
    "@ddag{",		"}",	"\\(dd",	"",		CHAR,
    "@deffn",		"@end", "\n\\&\n.br\n",	"",		HEADING,
    "@deffnx",		"\n",   "\n.br\n",	"\n.br\n",	HEADING,
    "@defspec",		"@end", "\n\\&\n.br\n",	"",		HEADING,
    "@defspecx",	"\n",   "\n.br\n",	"\n.br\n",	HEADING,
    "@defvar",		"@end", "\n\\&\n.br\n",	"",		HEADING,
    "@defvarx",		"\n",   "\n.br\n",	"\n.br\n",	HEADING,
    "@dfn{",		"}",	"\\fI",		"\\fR",		INPARA,
    "@display",		"@end", "\n.DS\n",	"\n.DE\n",	DISPLAY,
    "@dots{",		"}",	"\\&...",	"",		INPARA,
    "@eject",		"\n",	"\n.bp\n",	"",		PARAGRAPH,
    "@emph{",		"}",	"\\fI",		"\\fR",		INPARA,
    "@end",		"\n",	"",		"\n.sp\n.in\n", END,
    "@enumerate",	"@end", "\n.VL 5 0\n",	"\n.LE\n",	ITEMIZING,
    "@example",		"@end", "\n.DS 1\n.ft CW\n",
					"\n.DE\n.ft R\n",	DISPLAY,
    "@file{",		"}",	"\\f(CW\\`",	"\\'\\fR",	INPARA,
    "@finalout",	"\n",	"",		"",		DISCARD,
    "@findex",		"\n",	"",		"",		DISCARD,
    "@footnote{",	"}",	"\n.FS \n",	"\n.FE\n",	FOOTNOTE,
    "@group",		"@end", "\n.DS L F\n",	"\n.DE\n",	PARAGRAPH,
    "@heading",		"\n",	"\n.sp 2\n.HU ","\n.br\n",	HEADING,
    "@headings",	"\n",	"",		"",		DISCARD,
    "@iappendix",	"\n",	"\n.bp\n.HU ",	"\n.br\n",	HEADING,
    "@iappendixsec",	"\n",	"\n.HU ",	"\n.br\n",	HEADING,
    "@iappendixsubsec",	"\n",	"\n.HU ",	"\n.br\n",	HEADING,
    "@ichapter",	"\n",	"\n.bp\n.HU ",	"\n.br\n",	HEADING,
    "@ifinfo",		"@end", "",		"",		DISCARD,
    "@iftex",		"@end", "",		"",		PARAGRAPH,
    "@ignore",		"@end", "",		"",		DISCARD,
    "@include",		"\n",	"",		"",		DISCARD,
    "@inforef{",	"}",	"See Info file ","",		PARAGRAPH,
    "@isection",	"\n",	"\n.HU ",	"\n.br\n",	HEADING,
    "@isubsection",	"\n",	"\n.HU ",	"\n.br\n",	HEADING,
    "@isubsubsection",	"\n",	"\n.HU ",	"\n.br\n",	HEADING,
    "@item",		"\n",	"\n.LI ",	"\\ \n",	ITEM,
    "@itemize",		"@end", "\n.VL 5 0\n",	"\n.LE\n",	ITEMIZING,
    "@itemx",		"\n",	"\n.LI ",	"\\ \n",	ITEM,
    "@iunnumbered",	"\n",	"\n.bp\n.HU ",	"\n.br\n",	HEADING,
    "@iunnumberedsec",	"\n",	"\n.HU ",	"\n.br\n",	HEADING,
    "@iunnumberedsubsec","\n",	"\n.HU ",	"\n.br\n",	HEADING,
    "@i{",		"}",	"\\fI",		"\\fR",		INPARA,
    "@kbd{",		"}",	"\\f(CW",	"\\fR",		INPARA,
    "@key{",		"}",	"\\f(CW",	"\\fR",		INPARA,
    "@kindex",		"\n",	"",		"",		DISCARD,
    "@majorheading",	"\n",	"\n.sp 4\n.HU ","\n.br\n",	HEADING,
    "@menu",		"@end", "",		"",		DISCARD, 
    "@minus{",		"}",	"\\-",		"",		CHAR,
    "@need",		"\n",	"",		"",		DISCARD,
    "@node",		"\n",	"",		"",		DISCARD,
    "@noindent",	"\n",	"\n.P 0\n",	"",		PARAGRAPH,
    "@page",		"\n",	"\n.bp\n",	"",		PARAGRAPH,
    "@pindex",		"\n",	"",		"",		DISCARD,
    "@printindex",	"\n",	"",		"",		DISCARD,
    "@pxref{",		"}",	"see section ",	"",		PARAGRAPH,
    "@quotation",	"@end", "\n.DS I F 5\n.nh\n", "\n.DE\n",DISPLAY,
    "@r{",		"}",	"\\fR",		"\\fR",		INPARA,
    "@refill",		"\n",	"",		"",		DISCARD,
    "@samp{",		"}",	"\\f(CW\\`",	"\\'\\fR",	INPARA,
    "@section",		"\n",	"\n.H 2 ",	"\n.br\n",	HEADING,
    "@setchapternewpage","\n",	"",		"",		DISCARD,
    "@setfilename",	"\n",	"",		"",		DISCARD,
    "@settitle",	"\n",	"\n.ds St ",
		     "\n.OH '\\\\*(St''%'\n.EH '%''\\\\*(St'\n",HEADING,
    "@sp",		"\n",	"\n.sp",	"\n",		PARAGRAPH,
    "@smallbook",	"\n",	"",		"",		DISCARD,
    "@smallexample",	"@end", "\n.DS 1\n.ft CW\n",
					"\n.DE\n.ft R\n",	DISPLAY,
    "@strong{",		"}",	"\\fB",		"\\fR",		INPARA,
    "@subheading",	"\n",	"\n.HU ",	"\n.br\n",	HEADING,
    "@subsection",	"\n",	"\n.H 3 ",	"\n.br\n",	HEADING,
    "@subsubheading",	"\n",	"\n.HU ",	"\n.br\n",	HEADING,
    "@subsubsection",	"\n",	"\n.H 4 ",	"\n.br\n",	HEADING,
    "@summarycontents", "\n",	"",		"",		DISCARD,
    "@synindex",	"\n",	"",		"",		DISCARD,
    "@t",		"\n",	"\t",		"",		CHAR,
    "@table",		"@end", "\n.VL 5 0\n",	"\n.LE\n",	ITEMIZING,
    "@tex",		"@end", "",		"",		DISCARD, 
    "@tindex",		"\n",	"",		"",		DISCARD,
    "@titlefont{",	"}",	"\n.ps +10\n.ce\n",
						 "\n.ps -10\n",	PARAGRAPH,
    "@titlepage",	"@end", "",		"\n.bp\n",	PARAGRAPH,
    "@t{",		"}",	"\\f(CW",	"\\fR",		INPARA,
    "@unnumbered",	"\n",	"\n.bp\n.HU ",	"\n.br\n",	HEADING,
    "@unnumberedsec",	"\n",	"\n.HU ",	"\n.br\n",	HEADING,
    "@unnumberedsubsec","\n",	"\n.HU ",	"\n.br\n",	HEADING,
    "@unnumberedsubsubsec","\n","\n.HU ",	"\n.br\n",	HEADING,
    "@var{",		"}",	"\\fI",		"\\fR",		INPARA,
    "@vfil",		"\n",	"",		"",		DISCARD,
    "@vindex",		"\n",	"",		"",		DISCARD,
    "@vskip",		"\n",	"",		"",		DISCARD,
    "@w{",		"}",	"\n\\! ",	"\n",		INPARA,
    "@xref{",		"}",	"See section ",	"",		PARAGRAPH,
    "@{",		"",	"{",		"",		ESCAPED,
    "@}",		"",	"}",		"",		ESCAPED
};
