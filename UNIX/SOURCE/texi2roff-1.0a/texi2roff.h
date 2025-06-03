/*
 * texi2roff.h - header for Texinfo to nroff/troff translator texi2roff
 *	     Release 1.0a	August 1988
 *
 * Copyright 1988  Beverly A.Erlebacher
 * erlebach@csri.toronto.edu	...uunet!utai!utcsri!erlebach
 *
 */

/* miscellaneous troff command strings in macro header files. */
struct misccmds {
    char * dfltpara;	/* emit when 2 consecutive newlines are detected */
			/* in the input and the indentation level is <= 1. */
    char * dfltipara;	/* same but for indentation level > 1.  */
    char * indentstart; /* emit to increase indent level for itemized list */
    char * indentend;	/* emit to decrease indent level for itemized list */
    char * dblquote	/* emit to get double quote in macro argument. 	
			 * although a literal " seems to work for -mm and 
			 * -ms, -me chokes on it. if you have trouble with
			 * -ms or -mm and ", use the -me string. if you
			 * have SoftQuad's nroff/troff, you can use "\(dq"
			 */
};

extern struct misccmds * cmds;

struct tablerecd {
    char *  texstart;	/* starting token for a Texinfo command */
    char *  texend;	/* ending token for a Texinfo command */
    char *  trfstart;	/* troff commands to emit when texstart is found */
    char *  trfend;	/* troff commands to emit when texend is found */
    int	    type;	/* kind of Texinfo command, as #defined below */
};

extern struct tablerecd * table;
extern int tablesize;

/* Texinfo command types */

#define ESCAPED	    0	/* special character (special to Texinfo) */
#define INPARA	    1	/* in-paragraph command */
#define HEADING	    2	/* chapter structuring command */
#define DISCARD	    3	/* not supported - discard following text */
#define PARAGRAPH   4	/* applies to following paragraph */
#define ITEMIZING   5	/* starts itemized list */
#define ITEM	    6	/* item in list */
#define END	    7	/* end construct */
#define CHAR	    8	/* really special char: dagger, bullet - scary, eh? */
#define	FOOTNOTE    9	/* footnote */
#define DISPLAY    10	/* text block of the kind called a 'display' */
 
/* nroff/troff macro packages supported */
#define NONE	0	/* dummy value for error detection */
#define MS	1
#define ME	2
#define MM	3

/* useful confusion-reducing things */
#define STREQ(s,t) (*(s)==*(t) && strcmp(s, t)==0)
#define NO	0
#define YES	1
