/*
 * translate.c - main guts of Texinfo to nroff/troff translator texi2roff
 * 		 Release 1.0a	 August 1988
 *
 * Copyright 1988 Beverly A.Erlebacher
 * erlebach@csri.toronto.edu	...uunet!utai!utcsri!erlebach
 *
 */

#include <stdio.h>
#ifdef BSD
#include <strings.h>
#define strchr	index
#else
#include <string.h>
#endif
#include "texi2roff.h"

#define MAXLINELEN  256
#define ERROR	    (-1)

extern int	transparent;		/* -t flag: dont discard things	   */
int		linecount;
int		displaylevel = 0;	/* nesting level of 'display' text */
int		ilevel = 0;	 	/* nesting level of itemized lists */
int		inmacroarg = NO;	/* protect roff macro args flag */
char	        *filename;
char 		*p;			/* pointer into input buffer */

/* forward references */
extern char * gettoken();
extern char * eatwhitespace();
extern struct tablerecd * lookup();
extern char * itemize();
extern char * doitem();
extern char * interpret();

/*
 * errormsg - print error messages to stderr
 */

void
errormsg( message, other)
    char	   *message;
    char	   *other;
{
    (void) fprintf(stderr, "%s line %d : %s%s\n",
	filename, linecount, message, other);
}

/*
 * translate - translate one Texinfo file
 */

int 
translate(in, inname)
    FILE	   *in;
    char	   *inname;
{
    char	    input[MAXLINELEN];
    char	    output[MAXLINELEN];
    char	    token[MAXLINELEN];
    char	    *c,*d,lastchar;

    filename = inname;
    linecount = 0;
    lastchar = '\n';

    while (fgets(input, MAXLINELEN, in) != NULL) {
	++linecount;
	p = input;
	*output = 0;
	if (*p == '.')			/* protect leading '.' in input */
	    (void) strcpy(output, "\\&");
	else if (*p == '\n') {
	    if (displaylevel > 0)
		(void) strcat(output,"\\&\n");	  /* protect newline */
	    else if (ilevel > 0)	/* indented paragraph */
		(void) strcat(output, cmds->dfltipara);
	    else			/* default paragraph */
		(void) strcat(output,cmds->dfltpara);
	}
	while (*p != '\0') {
	    p = gettoken(p, token);
	    p = interpret(token, output);
	    if (p == NULL)
		return ERROR;
	}

	/*
	 * output, stripping surplus newlines when possible.
	 * ?roff may vanish lines w/leading '. also, empirically,
	 * often lines with leading \ . so emit zero-width char \& .
	 */
	d = &lastchar;	/* character at end of previous output buffer */
	for( c = output; *c != '\0'; d = c, ++c) {
	    if (*c != '\n' || *d != '\n') {
		if ( *d == '\n' && (*c == '\\' || *c == '\''))
		    (void) fputs("\\&", stdout);
		(void) putc(*c, stdout);
	    }
	}
	lastchar = *d;
    }
    return 0;
}

/*
 * PUSH - macro to push pointer to table entry onto command stack
 */

#define MAXDEPTH    20

#define PUSH(tptr)							\
    if (++stackptr == MAXDEPTH) {					\
	errormsg("stack overflow - commands nested too deeply", "");	\
	return NULL;							\
    }									\
    stack[stackptr] = tptr;

/*
 * interpret - interprets and concatenates interpreted token onto outstring
 */

char *
interpret(token, outstring)
char	*token;
char	*outstring;
{
    static struct tablerecd *stack[MAXDEPTH];
    static int	    stackptr = 0; /* zeroth element is not used */
    static int      discarding = NO;
    static int	    discardlevel = MAXDEPTH;
    struct tablerecd *tptr;
    char	    *s, *cp, tempstr[MAXLINELEN];

    s = p;
    if (stackptr > 0 && STREQ(token, stack[stackptr]->texend)) {
    /* have fetched closing token of current Texinfo command */
	if (STREQ(token, "@end")) {/* WARNING! only works from translate() */
	    s = gettoken(eatwhitespace(s),tempstr);
	    if	(! STREQ(&(stack[stackptr]->texstart[1]), tempstr)) {
		errormsg("probably @end for unrecognized Texinfo cmd @",
							    tempstr);
		return s;
	    }
	}
	if (!discarding)
	    (void) strcat(outstring, stack[stackptr]->trfend);

	if (stack[stackptr]->type == DISPLAY)
	    --displaylevel;
	else if (stack[stackptr]->type == ITEMIZING) {
	    --ilevel;
	    if (!discarding && ilevel > 0)
		(void) strcat(outstring, cmds->indentend);
	}

	if (--stackptr < 0) {
	    errormsg("stack underflow", "");
	    return NULL;
	}
    	if (discarding && stackptr < discardlevel) {
	    discarding = NO;
	    discardlevel = MAXDEPTH;
    	}
	if (*token == '\n' || STREQ(token, "@end")) {
	    inmacroarg = NO;
	    return "";  		/* flush rest of line if any */
	}
    } else if (*token != '@') { 	/* ordinary piece of text */
	if (!discarding)
	    (void) strcat(outstring, token);
	if (*token == '\n') {
	    inmacroarg = NO;
	    return "";
	}
    } else {				/* start of Texinfo command */
	if ((tptr = lookup(token)) == NULL) 
	    errormsg("unrecognized Texinfo command ", token);
	else {
	    switch (tptr->type) {
	    case ESCAPED:
		if (!discarding)
		    (void) strcat(outstring, tptr->trfstart);
		break;
	    case DISPLAY:
		++displaylevel;
		PUSH(tptr);
		if (!discarding)
		    (void) strcat(outstring, tptr->trfstart);
		break;
	    case HEADING:
		/*
		 * not presently supporting  appendices (lettered
		 * rather than numbered	 sections) they will come
		 * out as unnumbered for now
		 */
		inmacroarg = YES;   /* protect ' and space in hdr macro args */
		s = eatwhitespace(s);
		/* fall through */
	    case CHAR:	/* may be some need to distinguish these 3 in future */
	    case INPARA:
	    case PARAGRAPH:
		PUSH(tptr);
		if (!discarding)
		    (void) strcat(outstring, tptr->trfstart);
		break;
	    case DISCARD:
		PUSH(tptr);
		if (!discarding && !transparent) {
		    discarding = YES;
		    discardlevel = stackptr;
		}
		break;
	    case ITEMIZING:
		if (!discarding) {
		    (void) strcat(outstring, tptr->trfstart);
		    if (ilevel > 0)
			(void) strcat(outstring, cmds->indentstart);
		}
		PUSH(tptr);
		++ilevel;
		s = itemize(s, token);
		break;
	    case ITEM:
		PUSH(tptr);
		if (!discarding) {
		    (void) strcat(outstring, tptr->trfstart);
		    inmacroarg = YES;
		    s = doitem(s,tempstr);
		    inmacroarg = NO;
		    (void) strcat(outstring,tempstr);
		}
		break;
	    case END:
		s = gettoken(eatwhitespace(s),tempstr);
		errormsg("probably @end for unrecognized Texinfo cmd @",
							tempstr);
		break;
	    case FOOTNOTE:
		PUSH(tptr);
		if (!discarding) {
		    s = gettoken(eatwhitespace(s),tempstr);
		    cp = outstring + strlen(outstring);
		    (void) interpret(tempstr, outstring);
		    (void) strcpy(tempstr, cp);
		    (void) strcat(outstring, tptr->trfstart);
			/* replicate footnote mark */
		    (void) strcat(outstring, tempstr);
		}
		break;
	    default:
		/* can't happen */
		errormsg("ack ptui, what was that thing? ", token);
	    }
	}
    }
    return s;
}	

/*
 * eatwhitespace - move input pointer to first char that isnt a blank or tab
 *	(note that newlines are *not* whitespace)
 */

char	       *
eatwhitespace(s)
    register char	   *s;
{
    while(*s == ' ' || *s == '\t')
	++s ;
    return s;
}


/* 
 * strpbrk_like - returns pointer to the leftmost occurrence in str of any
 *	character in set. this function provided by rayan zachariassen.
 * 	this isnt a full strpbrk(), it's just as much of one as is necessary
 *	here.  not everyone has a strpbrk() in their C library.
*/

char *
strpbrk_like(str, set)
    register char *str;
    char *set;
{
    static int inited_set = 0;
    static char set_vec[256] = { 0 };

    if (!inited_set) {	/* we *know* it'll be the same every time... */
	while (*set)
 	    set_vec[(unsigned char)*set++] = 1;
	inited_set = 1;
	}
    while (set_vec[*str] == 0)
	if (!*str++)
	    return 0;
    return str;
}


/*
 * gettoken - fetch next token from input buffer. leave the input pointer
 *	pointing to char after token.	 may need to be modified when 
 *	new Texinfo commands are added which use different token boundaries.
 */
 
char	       *
gettoken(s, token)
    char	   *s;
    char	   *token;
{
    static char	   endchars[] = " \n\t@{}:.*";
    char	   *q, *t;

    q = s;
    s = strpbrk_like(q, endchars);
    if (s != q) {
	switch (*s) {
	case ' ':
	case '\n':
	case '\t':
	case '@':
	case '}':
	case ':':
	case '.':
	case '*':
	    --s;
	    break;
	case '{':
	    break;
	}
    } else {	/* *s == *q */
	switch (*s) {
	case ' ':
	case '\n':
	case '\t':
	case '{':
	case ':':
	case '.':
	case '*':
	    break;
	case '}':
	    if (*(s+1) == '{') /* footnotes with daggers and dbl daggers!! */
		++s;
	    break;
	case '@':
	    s = strpbrk_like(q + 1, endchars );
	    /* handles 2 char @ tokens: @{ @} @@ @: @. @* */
	    if ( strchr("{}@:.*", *s) == NULL
			|| (s > q+1 && (*s =='}' || *s == '@')))
		--s;
	    break;
	}
    }
    for (t = token; q <= s; ++q, ++t) {
	switch (*q) {
	    case '\\' :		 /* replace literal \ with \e */
		*t = *q;
	        *++t = 'e';
		break;
	    case ' '  :		/* protect spaces and ' in macro args */
	    case '\'' :	
		if (inmacroarg == YES) {
		    *t = '\\';
	            *++t = *q;
		} else
		    *t = *q;
		break;
	    case '\"' :		/* try to avoid " trouble in macro args */
		if (inmacroarg == YES) {
		    *t = 0;
		    (void) strcat(t,cmds->dblquote);
		    t += strlen(cmds->dblquote) - 1;
		} else
		    *t = *q;
		break;
	    default   :
		*t = *q;
		break;
	}
    }
    *t = 0;
    return ++s;
}

/*
 * lookup - find a Texinfo command starting token in the table
 */

struct tablerecd *
lookup(token)
    char	   *token;
{
    register struct tablerecd *tptr;
    struct tablerecd *endoftable = table + tablesize/sizeof table[0];

    /* this could be a binary search, but even in a large document,
     * commands are infrequent compared to non-command tokens.
     * the program is fast enough and this works.
     */

    for (tptr = table; tptr < endoftable; ++tptr) {
	if (STREQ(tptr->texstart, token))
	    return tptr;
    }
    return NULL;
}

/*
 * all the global data and code for handling itemized stuff is down here
 */

#define MAXILEVEL	10
int icount[MAXILEVEL];
int what[MAXILEVEL];
char item[MAXILEVEL][MAXLINELEN];

#define ITEMIZE	    0
#define ENUMERATE   1
#define TABLE	    2
#define APPLY	    3

/*
 * itemize - handle the itemizing start commands @enumerate, @itemize
 *	and @table
 */

char * itemize(s, token)
char * s;
char * token;
{
struct tablerecd *i;

    if (STREQ(token,"@itemize")) {
	what[ilevel] = ITEMIZE;
	s = gettoken(eatwhitespace(s),item[ilevel]);
	if (item[ilevel][0] == '\n') { /* this is an error in the input */
	    --s;
	    (void) strcpy(item[ilevel],"-");
	    errormsg("missing itemizing argument ","");
	} else {
	    if (item[ilevel][0] == '@') {
		if ((i = lookup(item[ilevel])) != NULL) 
		    (void) strcpy(item[ilevel], i->trfstart);
		else
		    errormsg("unrecognized itemizing argument ", item[ilevel]);
		}
	    }
	}

    if (STREQ(token,"@enumerate")) {
	what[ilevel] = ENUMERATE;
	icount[ilevel] = 1;
    } else if (STREQ(token,"@table")) {
	what[ilevel] = TABLE;
	s = gettoken(eatwhitespace(s),item[ilevel]);
	if (item[ilevel][0] == '\n') {
	    item[ilevel][0] = '\0';  /* do nothing special */
	    --s;
	} else {
	    if (item[ilevel][0] =='@'
	        && (((i = lookup(item[ilevel])) != NULL)
		    || ((i = lookup(strcat(item[ilevel],"{"))) != NULL)))
		what[ilevel] = APPLY;
	    else
		errormsg("unrecognized table itemizing argument ",item[ilevel]);
	}
    }

    while (*s != '\n' && *s != '\0') 
	++s;  /* flush rest of line */
    return s;
}

/*
 * doitem - handle @item and @itemx
 */

char *
doitem(s,tag)
char * s;
char * tag;
{
    char temp[MAXLINELEN], temp2[MAXLINELEN];
    struct tablerecd *tp;

    switch (what[ilevel]) {
    case ITEMIZE:
	(void) strcpy(tag, item[ilevel]);
	break;
    case ENUMERATE:
	(void) sprintf(tag, "%d.", icount[ilevel]++);
	break;
    case TABLE:
	*tag = '\0';
	s = gettoken(eatwhitespace(s), temp);
	if (*temp == '\n') {
	    *tag++ = '-';
	    errormsg("missing table item tag","");
	}else {
	    while (temp[0] != '\n') {
		(void) strcat(tag, temp);
	    	s = gettoken(s, temp);
	    }
	}
	--s;
	break;
    case APPLY:
	*tag = '\0';
	temp2[0] = '\0';
	s = gettoken(eatwhitespace(s), temp);
	while (temp[0] != '\n') {
	    (void) interpret(temp, temp2);
	    s = gettoken(s, temp);
	}
	--s;
	if ( (tp = lookup(item[ilevel])) != NULL ||
		(tp = lookup(strcat(item[ilevel],"{"))) != NULL) {
	    (void) strcat(tag, tp->trfstart);
	    (void) strcat(tag, temp2);
	    (void) strcat(tag, tp->trfend);
	} else { /* this is an error in the input */
	    (void) strcpy(tag,temp);
	    errormsg("error applying Texinfo command to table item tag ",temp);
	}
	break;
    }
    return s;
}
