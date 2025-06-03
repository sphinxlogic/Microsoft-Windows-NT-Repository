/*
 * tableme.h - -me macros table and other explicit troff command strings
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
char meinit[] = "";

/*
 * miscellaneous commands, described in texi2roff.h 
 */
struct misccmds mecmds = { "\n.pp\n", "\n.ip\n", "", "", "\\(fm\\(fm"};

/*
 * patches for table entries for -I option 
 */
struct tablerecd menodeentry = {
    "@node",	"\n", "\n.lp\n\\s8Node:", "\\s0\n.br\n", PARAGRAPH};

struct tablerecd memenuentry = {
    "@menu",	"@end", "\n.(l I\n\\s8Menu: \n", "\\s0\n.)l\n", DISPLAY}; 

struct tablerecd meifinfoentry = {
    "@ifinfo",	"@end", "\n", "\n", PARAGRAPH}; 

struct tablerecd metable[] = {
    "@*",		"",	"\n.br\n",	"",		ESCAPED,
    "@.",		"",	"\\&.",		"",		ESCAPED,
    "@:",		"",	"",		"",		ESCAPED,
    "@@",		"",	"@",		"",		ESCAPED,
    "@TeX{",		"}",	"TeX",		"",		INPARA,
    "@appendix",	"\n",	"\n.bp\n.uh ",	"\n.br\n",	HEADING,
    "@appendixsec",	"\n",	"\n.uh ", 	"\n.br\n",	HEADING,
    "@appendixsubsec",	"\n",	"\n.uh ", 	"\n.br\n",	HEADING,
    "@appendixsubsubsec","\n",	"\n.uh ", 	"\n.br\n",	HEADING,
    "@asis",		"\n",	"",		"",		PARAGRAPH,
    "@asis{",		"}",	"",		"",		INPARA,
    "@br",		"\n",	"\n.br\n.sp\n", "",		PARAGRAPH,
    "@bullet",		"\n",	"\\(bu",	"",		CHAR,
    "@bullet{",		"}",	"\\(bu",	"",		CHAR,
    "@bye",		"\n",	"",		"",		DISCARD,
    "@b{",		"}",	"\\fB",		"\\fR",		INPARA,
    "@c",		"\n",	"",		"",		DISCARD,
    "@center",		"\n",	"\n.ce\n",	"",		HEADING,
    "@chapter",		"\n",	"\n.bp\n.sh 1 ","\n.br\n",	HEADING,
    "@cindex", 		"\n",	"",		"",		DISCARD,
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
    "@deffn",		"@end", "\n.lp\n",	"",		HEADING,
    "@deffnx",          "\n",   "\n.br\n",	"\n.br\n",	HEADING,
    "@defspec",		"@end", "\n.lp\n",	"",		HEADING,
    "@defspecx",        "\n",   "\n.br\n",	"\n.br\n",	HEADING,
    "@defvar",		"@end", "\n.lp\n",	"",		HEADING,
    "@defvarx",         "\n",   "\n.br\n",	"\n.br\n",	HEADING,
    "@dfn{",		"}",	"\\fI",		"\\fR",		INPARA,
    "@display",		"@end", "\n.(l I\n",	"\n.)l\n",	DISPLAY,
    "@dots{",		"}",	"\\&...",	"",		INPARA,
    "@eject",		"\n",	"\n.bp\n",	"",		PARAGRAPH,
    "@emph{",		"}",	"\\fI",		"\\fR",		INPARA,
    "@end",		"\n",	"",		"\n.sp\n.in\n", END,
    "@enumerate",	"@end", "\n.(l L F\n.ba +5\n",
					"\n.ba -5\n.)l\n",	ITEMIZING,
    "@example",		"@end", "\n.(l I\n.ft CW\n",
					"\n.)l\n.ft R\n",	DISPLAY,
    "@file{",		"}",	"\\f(CW\\`",	"\\'\\fR",	INPARA,
    "@finalout",	"\n",	"",		"",		DISCARD,
    "@findex", 		"\n",	"",		"",		DISCARD,
    "@footnote{",	"}",	"\n.(f \n",	"\n.)f\n",	FOOTNOTE,
    "@group",		"@end", "\n.(b\n",	"\n.)b\n",	PARAGRAPH,
    "@heading",		"\n",	"\n.uh ",	"\n.br\n",	HEADING,
    "@headings",	"\n",	"",		"",		DISCARD,
    "@iappendix",	"\n",	"\n.bp\n.uh ",	"\n.br\n",	HEADING,
    "@iappendixsec",	"\n",	"\n.uh ",	"\n.br\n",	HEADING,
    "@iappendixsubsec",	"\n",	"\n.uh ",	"\n.br\n",	HEADING,
    "@ichapter",	"\n",	"\n.bp\n.uh ",	"\n.br\n",	HEADING,
    "@ifinfo",		"@end", "",		"",		DISCARD,
    "@iftex",		"@end", "",		"",		PARAGRAPH,
    "@ignore",		"@end", "",		"",		DISCARD,
    "@include",		"\n",	"",		"",		DISCARD,
    "@inforef{",	"}",	"See Info file ","",		PARAGRAPH,
    "@isection",	"\n",	"\n.uh ",	"\n.br\n",	HEADING,
    "@isubsection",	"\n",	"\n.uh ",	"\n.br\n",	HEADING,
    "@isubsubsection",	"\n",	"\n.uh ",	"\n.br\n",	HEADING,
    "@item",		"\n",	"\n.ip ",	"\\ \n",	ITEM,
    "@itemize",		"@end", "\n.(l L F\n.ba +5\n",
					"\n.ba -5\n.)l\n",	ITEMIZING,
    "@itemx",		"\n",	"\n.ip ",	"\\ \n",	ITEM,
    "@iunnumbered",	"\n",	"\n.bp\n.uh ",	"\n.br\n",	HEADING,
    "@iunnumberedsec",	"\n",	"\n.uh ",	"\n.br\n",	HEADING,
    "@iunnumberedsubsec","\n",	"\n.uh ",	"\n.br\n",	HEADING,
    "@i{",		"}",	"\\fI",		"\\fR",		INPARA,
    "@kbd{",		"}",	"\\f(CW",	"\\fR",		INPARA,
    "@key{",		"}",	"\\f(CW",	"\\fR",		INPARA,
    "@kindex", 		"\n",	"",		"",		DISCARD,
    "@majorheading",	"\n",	"\n.sp 4\n.uh ","\n.br\n",	HEADING,
    "@menu",		"@end", "",		"",		DISCARD, 
    "@minus{",		"}",	"\\-",		"",		CHAR,
    "@need",		"\n",	"",		"",		DISCARD,
    "@node",		"\n",	"",		"",		DISCARD,
    "@noindent",	"\n",	"\n.lp\n",	"",		PARAGRAPH,
    "@page",		"\n",	"\n.bp\n",	"",		PARAGRAPH,
    "@pindex", 		"\n",	"",		"",		DISCARD,
    "@printindex",	"\n",	"",		"",		DISCARD,
    "@pxref{",		"}",	"see section  ","",		PARAGRAPH,
    "@quotation",	"@end", "\n.(q\n.nh\n", "\n.)q\n",	DISPLAY,
    "@r{",		"}",	"\\fR",		"\\fR",		INPARA,
    "@refill",		"\n",	"",		"",		DISCARD,
    "@samp{",		"}",	"\\f(CW\\`",	"\\'\\fR",	INPARA,
    "@section",		"\n",	"\n.sh 2 ",	"\n.br\n",	HEADING,
    "@setchapternewpage","\n",	"",		"",		DISCARD,
    "@setfilename",	"\n",	"",		"",		DISCARD,
    "@settitle",	"\n",	"\n.ds St ",
		"\n.oh '\\\\*(St''%'\n.eh '%''\\\\*(St'\n",	HEADING,
    "@smallbook",	"\n",	"",		"",		DISCARD,
    "@smallexample",	"@end", "\n.(l I\n.ft CW\n",
					"\n.)l\n.ft R\n",	DISPLAY,
    "@sp",		"\n",	"\n.sp",	"\n",		PARAGRAPH,
    "@strong{",		"}",	"\\fB",		"\\fR",		INPARA,
    "@subheading",	"\n",	"\n.uh ",	"\n.br\n",	HEADING,
    "@subsection",	"\n",	"\n.sh 3 ",	"\n.br\n",	HEADING,
    "@subsubheading",	"\n",	"\n.uh ",	"\n.br\n",	HEADING,
    "@subsubsection",	"\n",	"\n.sh 4 ",	"\n.br\n",	HEADING,
    "@summarycontents", "\n",	"",		"",		DISCARD,
    "@synindex",	"\n",	"",		"",		DISCARD,
    "@t",		"\n",	"\t",		"",		CHAR,
    "@table",		"@end", "\n.(l L F\n.ba +5\n",
					"\n.ba -5\n.)l\n",	ITEMIZING,
    "@tex",		"@end", "",		"",		DISCARD, 
    "@tindex", 		"\n",	"",		"",		DISCARD,
    "@titlefont{",	"}",	"\n.sz +10\n.ce\n",
						"\n.sz -10\n",	PARAGRAPH,
    "@titlepage",	"@end", "\n.tp\n",	"\n.bp\n",	PARAGRAPH,
    "@t{",		"}",	"\\f(CW",	"\\fR",		INPARA,
    "@unnumbered",	"\n",	"\n.bp\n.uh ",	"\n.br\n",	HEADING,
    "@unnumberedsec",	"\n",	"\n.uh ",	"\n.br\n",	HEADING,
    "@unnumberedsubsec","\n",	"\n.uh ",	"\n.br\n",	HEADING,
    "@unnumberedsubsubsec","\n","\n.uh ",	"\n.br\n",	HEADING,
    "@var{",		"}",	"\\fI",		"\\fR",		INPARA,
    "@vfil", 		"\n",	"",		"",		DISCARD,
    "@vindex", 		"\n",	"",		"",		DISCARD,
    "@vskip",		"\n",	"",		"",		DISCARD,
    "@w{",		"}",	"\n\\! ",	"\n",		INPARA,
    "@xref{",		"}",	"See section ",	"",		PARAGRAPH,
    "@{",		"",	"{",		"",		ESCAPED,
    "@}",		"",	"}",		"",		ESCAPED
};

