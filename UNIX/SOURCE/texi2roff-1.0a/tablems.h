/*
 * tablems.h - -ms macros table and other explicit troff command strings
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
char msinit[] = ".ND\n"; /* .ND suppresses date in footing */

/*
 * miscellaneous commands, described in texi2roff.h
 */
struct misccmds mscmds = { "\n.PP\n", "\n.IP\n", "\n.RS\n", "\n.RE\n", "\""};

/*
 * patches for table entries for -I option
 */
struct tablerecd msnodeentry = {
    "@node",	"\n", "\n.LP\n\\s8Node:", "\\s0\n.br\n", PARAGRAPH};

struct tablerecd msmenuentry = {
    "@menu",	"@end", "\n.DS L\n\\s8Menu: \n", "\\s0\n.DE\n", DISPLAY}; 

struct tablerecd msifinfoentry = {
    "@ifinfo",	"@end", "\n", "\n", PARAGRAPH}; 

struct tablerecd mstable[] = {
    "@*",		"",	"\n.br\n",	"",		ESCAPED,
    "@.",		"",	"\\&.",		"",		ESCAPED,
    "@:",		"",	"",		"",		ESCAPED,
    "@@",		"",	"@",		"",		ESCAPED,
    "@TeX{",		"}",	"TeX",		"",		INPARA,
    "@appendix",	"\n",	"\n.bp\n.SH\n", "\n.br\n",	HEADING,
    "@appendixsec",	"\n",	"\n.SH\n",	"\n.br\n",	HEADING,
    "@appendixsubsec",	"\n",	"\n.SH\n",	"\n.br\n",	HEADING,
    "@appendixsubsubsec","\n",	"\n.SH\n",	"\n.br\n",	HEADING,
    "@asis",		"\n",	"",		"",		PARAGRAPH,
    "@asis{",		"}",	"",		"",		INPARA,
    "@br",		"\n",	"\n.br\n.sp\n", "",		PARAGRAPH,
    "@bullet",		"\n",	"\\(bu",	"",		CHAR,
    "@bullet{",		"}",	"\\(bu",	"",		CHAR,
    "@bye",		"\n",	"",		"",		DISCARD,
    "@b{",		"}",	"\\fB",		"\\fR",		INPARA,
    "@c",		"\n",	"",		"",		DISCARD,
    "@center",		"\n",	"\n.ce\n",	"",		HEADING,
    "@chapter",		"\n",	"\n.bp\n.NH\n", "\n.br\n",	HEADING,
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
    "@deffn",		"@end", "\n.LP\n",	"",		HEADING,
    "@deffnx",		"\n", 	"\n.br\n",	"\n.br\n",	HEADING,
    "@defspec",		"@end", "\n.LP\n",	"",		HEADING,
    "@defspecx",	"\n", 	"\n.br\n",	"\n.br\n",	HEADING,
    "@defvar",		"@end", "\n.LP\n",	"",		HEADING,
    "@defvarx",		"\n", 	"\n.br\n",	"\n.br\n",	HEADING,
    "@dfn{",		"}",	"\\fI",		"\\fR",		INPARA,
    "@display",		"@end", "\n.DS\n",	"\n.DE\n",	DISPLAY,
    "@dots{",		"}",	"\\&...",	"",		INPARA,
    "@eject",		"\n",	"\n.bp\n",	"",		PARAGRAPH,
    "@emph{",		"}",	"\\fI",		"\\fR",		INPARA,
    "@end",		"\n",	"",		"\n.sp\n.in\n", END,
    "@enumerate",	"@end", "\n",		"\n",		ITEMIZING,
    "@example",		"@end", "\n.DS\n.ft CW\n",
					"\n.DE\n.ft R\n",	DISPLAY,
    "@file{",		"}",	"\\f(CW\\`",	"\\'\\fR",	INPARA,
    "@finalout",	"\n",	"",		"",		DISCARD,
    "@findex",		"\n",	"",		"",		DISCARD,
    "@footnote{",	"}",	"\n.FS \n",	"\n.FE\n",	FOOTNOTE,
    "@group",		"@end", "\n.KS\n",	"\n.KE\n",	PARAGRAPH,
    "@heading",		"\n",	"\n.sp 2\n.SH\n","\n.br\n",	HEADING,
    "@headings",	"\n",	"",		"",		DISCARD,
    "@iappendix",	"\n",	"\n.bp\n.SH\n", "\n.br\n",	HEADING,
    "@iappendixsec",	"\n",	"\n.SH\n",	"\n.br\n",	HEADING,
    "@iappendixsubsec",	"\n",	"\n.SH\n",	"\n.br\n",	HEADING,
    "@ichapter",	"\n",	"\n.bp\n.SH\n", "\n.br\n",	HEADING,
    "@ifinfo",		"@end", "",		"",		DISCARD,
    "@iftex",		"@end", "",		"",		PARAGRAPH,
    "@ignore",		"@end", "",		"",		DISCARD,
    "@include",		"\n",	"",		"",		DISCARD,
    "@inforef{",	"}",	"See Info file ","",		PARAGRAPH,
    "@isection",	"\n",	"\n.SH\n",	"\n.br\n",	HEADING,
    "@isubsection",	"\n",	"\n.SH\n",	"\n.br\n",	HEADING,
    "@isubsubsection",	"\n",	"\n.SH\n",	"\n.br\n",	HEADING,
    "@item",		"\n",	"\n.IP ",	"\\ \n",	ITEM,
    "@itemize",		"@end", "\n",		"\n",		ITEMIZING,
    "@itemx",		"\n",	"\n.IP ",	"\\ \n",	ITEM,
    "@iunnumbered",	"\n",	"\n.bp\n.SH\n", "\n.br\n",	HEADING,
    "@iunnumberedsec",	"\n",	"\n.SH\n",	"\n.br\n",	HEADING,
    "@iunnumberedsubsec","\n",	"\n.SH\n",	"\n.br\n",	HEADING,
    "@i{",		"}",	"\\fI",		"\\fR",		INPARA,
    "@kbd{",		"}",	"\\f(CW",	"\\fR",		INPARA,
    "@key{",		"}",	"\\f(CW",	"\\fR",		INPARA,
    "@kindex",		"\n",	"",		"",		DISCARD,
    "@majorheading",	"\n",	"\n.sp 4\n.SH\n","\n.br\n",	HEADING,
    "@menu",		"@end", "",		"",		DISCARD, 
    "@minus{",		"}",	"\\-",		"",		CHAR,
    "@need",		"\n",	"",		"",		DISCARD,
    "@node",		"\n",	"",		"",		DISCARD,
    "@noindent",	"\n",	"\n.LP\n",	"",		PARAGRAPH,
    "@page",		"\n",	"\n.bp\n",	"",		PARAGRAPH,
    "@pindex",		"\n",	"",		"",		DISCARD,
    "@printindex",	"\n",	"",		"",		DISCARD,
    "@pxref{",		"}",	"see section ",	"",		PARAGRAPH,
    "@quotation",	"@end", "\n.QP\n.nh\n", "\n.PP\n",	DISPLAY,
    "@r{",		"}",	"\\fR",		"\\fR",		INPARA,
    "@refill",		"\n",	"",		"",		DISCARD,
    "@samp{",		"}",	"\\f(CW\\`",	"\\'\\fR",	INPARA,
    "@section",		"\n",	"\n.NH 2\n",	"\n.br\n",	HEADING,
    "@setchapternewpage","\n",	"",		"",		DISCARD,
    "@setfilename",	"\n",	"",		"",		DISCARD,
    "@settitle",	"\n",	"\n.ds St ",
		     "\n.OH '\\\\*(St''%'\n.EH '%''\\\\*(St'\n",HEADING,
    "@smallbook",	"\n",	"",		"",		DISCARD,
    "@smallexample",	"@end", "\n.DS\n.ft CW\n",
					"\n.DE\n.ft R\n",	DISPLAY,
    "@sp",		"\n",	"\n.sp",	"\n",		PARAGRAPH,
    "@strong{",		"}",	"\\fB",		"\\fR",		INPARA,
    "@subheading",	"\n",	"\n.SH\n",	"\n.br\n",	HEADING,
    "@subsection",	"\n",	"\n.NH 3\n",	"\n.br\n",	HEADING,
    "@subsubheading",	"\n",	"\n.SH\n",	"\n.br\n",	HEADING,
    "@subsubsection",	"\n",	"\n.NH 4\n",	"\n.br\n",	HEADING,
    "@summarycontents", "\n",	"",		"",		DISCARD,
    "@synindex",	"\n",	"",		"",		DISCARD,
    "@t",		"\n",	"\t",		"",		CHAR,
    "@table",		"@end", "\n",		"\n",		ITEMIZING,
    "@tex",		"@end", "",		"",		DISCARD, 
    "@tindex",		"\n",	"",		"",		DISCARD,
    "@titlefont{",	"}",	"\n.ps +10\n.ce\n",
						 "\n.ps -10\n",	PARAGRAPH,
    "@titlepage",	"@end", "",		"\n.bp\n",	PARAGRAPH,
    "@t{",		"}",	"\\f(CW",	"\\fR",		INPARA,
    "@unnumbered",	"\n",	"\n.bp\n.SH\n", "\n.br\n",	HEADING,
    "@unnumberedsec",	"\n",	"\n.SH\n",	"\n.br\n",	HEADING,
    "@unnumberedsubsec","\n",	"\n.SH\n",	"\n.br\n",	HEADING,
    "@unnumberedsubsubsec","\n","\n.SH\n",	"\n.br\n",	HEADING,
    "@var{",		"}",	"\\fI",		"\\fR",		INPARA,
    "@vfil",		"\n",	"",		"",		DISCARD,
    "@vindex",		"\n",	"",		"",		DISCARD,
    "@vskip",		"\n",	"",		"",		DISCARD,
    "@w{",		"}",	"\n\\! ",	"\n",		INPARA,
    "@xref{",		"}",	"See section ",	"",		PARAGRAPH,
    "@{",		"",	"{",		"",		ESCAPED,
    "@}",		"",	"}",		"",		ESCAPED
};
