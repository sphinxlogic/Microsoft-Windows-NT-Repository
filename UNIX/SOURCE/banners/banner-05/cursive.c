/*		CURSIVE SIGNATURE PROGRAM		Version 0.10
 *              (c) 1985 - Jan Wolter
 *
 *  Purpose:	This program translates text into crude cursive script.
 *		It works on any terminal with a fairly normal character
 *		set (must have backslashes and underscores and suchlike).
 *		The font includes all upper and lower case letters, most
 *		punctuation, and a few symbols.  No numbers are included
 *		(It's difficult to make a nice italicized set of numbers).
 *		Cursive was originally written to generate fancy signatures
 *		for electronic mail messages, but other uses may occur to
 *		you.  The attractiveness of the font varies greatly with
 *		the display device.
 *
 *  Usage:	If no command line arguments are given, cursive reads the
 *		text to translate from standard input.  Otherwise the args
 *		are translated (e.g. "cursive Jan Wolter" prints my name).
 *		Output is always to standard output.  A couple command line
 *		arguments are recognized:
 *
 *		  -in	Sets the amount of space to insert between letters.
 *			The default is "-i1".  "-i0" gives interesting
 *			results.
 *		  -tn	Sets the length of the trailing line off the end
 *			of each word.  The default is "-t1".
 *
 *		One character in the text is treated in a special way:
 *
 *		  '_'   Can be inserted in text to slightly lengthen the
 *			the spacing between two letters, or add a longer
 *			tail to the end of a word.
 *
 *  Internals:	Unfortunately, the program is a kludge and the font is
 *		somewhat difficult to edit.  It should be easy to port
 *		though.  Systems with short integers or unsigned characters
 *		should be no problem.  You should probably run "xstr" on the
 *		font.c file, but if you haven't got "xstr", just compiling it
 *		the usual way works fine.
 *
 *  Copyright:	Both the cursive program and the font it generates are
 *		copyrighted by yours truly.  However, both may be used
 *		and distributed freely.  You even have my permission to
 *		sell it, or include in it a system that you sell.  I only
 *		ask that my name be retained on this program, or any direct
 *		decendents of this program with approximately the same
 *		visibility as in this posting.
 *
 *  Mail:	I'm interested in any comments you have on this program.
 *		I can be mailed at "janc@crim.eecs.umich.edu".  Better yet,
 *		if you are into teleconferencing, call M-Net at (313) 994-6333
 *		and type "newuser" at the "login:" prompt to give yourself an
 *		account.  Send mail to janc, or join the "ugames" conference.
 *
 *		Have fun,
 *				  ___                _     __  _
 *				 (   >              ' )   /   // _/_
 *				  __/___.  ____      / / / __|/  /  _  __
 *				 / / (_/|_/ / <__   (_(_/ (_) \_<__</_/ (__
 *				<_/
 */

#include <stdio.h>


/*
#include "cursive.h"
*/

/*****************************************************************************
 * "cursive.h"                                                               *
 *****************************************************************************/

struct letter {
	char *line[6];			/* What's on each of the six lines */
	char spcs[6];			/* Number of leading spaces for each */
	short tailin,tailout;		/* Where the lines in and out go */
	};

/* Special Codes for Character Table */

#define XX  -1		/* Ignore character */
#define SP  -2          /* Space */
#define ST  -3          /* Extend trailer */
#define SA  -4		/* Alternate font number */	/* Not used */
#define SN  -5		/* Numbers */			/* Not used */

#define CHARWIDTH 10

extern struct letter list[];
extern short code1[], code2[];

char *buffer[6];	/* memory buffers to build up line in */
int c[6];		/* current index in each of the buffer lines */
int tail[6];		/* which buffer lines have tails in them */
int lasttail;		/* which line was the last letter's tail */
int space;		/* how much white space before the next letter */
int interspace = 1;	/* how much to spread adjacent letters out */
int taillen = 1;	/* how long the tails on ends of words should be */
char firstletter;	/* is this the first letter on the line? */
char message[256] = "";	/* message to print */

/*
 * end of "cursive.h"
 */

char *malloc();

main(argc,argv)
int argc;
char **argv;
{
int i;
char *s,*m;
char ch;

	m = message;
	for (i = 1; i < argc; i++)
	{
		if (*argv[i] == '-')
			switch(ch = argv[i][1])
			{
			case 'i':
			case 't':
				s = argv[i]+2;
				if (*s == '\000')
					if (++i < argc)
						s = argv[i];
				if (*s < '0' || *s > '9')
				{
					printf("%s: Illegal value %s\n",
						argv[0],s);
					exit(1);
				}
				if (ch == 'i')
					interspace = atoi(s);
				else
					taillen = atoi(s);
				break;
			default:
				printf("usage: %s [-tn] [-in] message\n",
					argv[0]);
				exit(1);
			}
		else
		{
			if (m != message)
				*(m++) = ' ';
			for (s=argv[i]; *s != '\000'; s++)
				*(m++) = *s;
		}
	}
	/* Do the deed */
	if (m != message)
	{
		/* Message from the arg list */
		*(m++) = 0;
		prline(message);
	}
	else
	{
		/* Message from standard input */
		while (gets(message) != NULL)
			prline(message);
	}
}


/* Add the given letter to the end of the current line */

place(let)
struct letter *let;
{
int i,j,n;
int col;
int max = -10000;
char pad;
char *l;

	if (firstletter)
	{
		col = space;		/* Leading spaces */
		firstletter = 0;
	}
	else
	{
		/* Find the furthest left position we can place this letter */
		for(i=0; i<6; i++)
		{
			if (let->line[i][0] != '\000' &&
			    (col = c[i] - let->spcs[i]) > max)
				max = col;
		}

		/* Insert some spaces between letters */
		col = max + space + interspace;
	}

	for(i=0; i<6; i++)

		/* If Nothing on this Line, Skip It */
		if (let->line[i][0] != '\000')
		{
			/* Number of Spaces to Put on this Line? */
			n = col - c[i] + let->spcs[i];

			/* Flyers off Ends of Letters */
			if (tail[i])
				for (j = 0;
				     j < 5 && n > space + 2; j++,n--)
					buffer[i][c[i]++] = '_';

			/* Connecting Lines Between Letters */
			pad = (lasttail == i && let->tailin == i) ? '_' : ' ';

			/* In the middle of the intersection of South and */
			/* East University, someone is playing the piano. */
			for ( ; n > 0; n--)
				buffer[i][c[i]++] = pad;

			/* Copy in the letter text */
			for (l = let->line[i]; *l != '\000'; l++)
				buffer[i][c[i]++] = *l;

			tail[i] = (i == let->tailout);
		}

	lasttail = let->tailout;
	space = 0;
}

/* Lengthen the last trailer by n */
tailer(n)
int n;
{
int j;
	if (lasttail >= 0)
		for (j = 0; j < n; j++)
			buffer[lasttail][c[lasttail]++] = '_';
}

/* Handle a line */
prline(s)
char *s;
{
int l,i;
char *ch;
short lcode;

	lasttail = -1;
	firstletter = 1;
	space = 0;
	/* Get some memory to put the output into */
	l = strlen(s) * (CHARWIDTH + interspace);
	for (i=0;i<6;i++)
	{
		buffer[i] = malloc((unsigned)l);
		tail[i] = c[i] = 0;
	}

	/* do each letter */
	for (ch=s; *ch != '\000'; ch++)
	{

		*ch &= '\177';
		/* Find the letter */
		lcode = (lasttail != 2) ? code1[*ch] : code2[*ch];
		if (lcode >= 0)
			place(&list[lcode]);
		else
			/* Various Special characters */
			switch(lcode)
			{
			case SP:
				/* Insert white space before next letter */
				tailer(taillen);
				space += 3;
				lasttail = -1;
				break;
			case ST:
				/* Lengthen the last tail */
				if (lasttail >= 0)
					buffer[lasttail][c[lasttail]++] = '_';
				break;
			}
	}
	tailer(taillen);

	/* print the line and release the memory */
	for (i=0;i<6;i++)
	{
		buffer[i][c[i]++] = '\n';
		write(1,buffer[i],c[i]);
		free(buffer[i]);
	}
}


/*****************************************************************************
 * "font.c"                                                                  *
 *****************************************************************************/

/* CURSIVE FONT FILE
 *
 * This font is copyrighted by Jan Wolter.  See notes in "cursive.c".
 *
 * Several of the letters are rather ugly, but the wonder is not how
 * well it sings.  A few uppercase letters are a bit too tall.  The
 * font has to some extent been chosen to make my name look good to me.
 * Those of you with different names will have to take pot luck, or fix
 * it to taste.  Editing this file must be done with some care.  The
 * numbers on the defines must match the index numbers of the letter
 * in the list[] structure.  The six numbers after the character are how
 * many blank spaces must be added to the beginning of each of the six
 * scan lines to make them line up.  The last two numbers are the scan
 * line numbers in which the character is connected to the preceeding and
 * succeeding characters.  -1 means it isn't.  There are two versions of
 * each lower case letter, one with the line coming in on line 2, and one
 * with the line coming in on line 3.
 */

/*
#include "cursive.h"
*/

struct letter list[] = {

#define LA  0		/* a type 1 */ 
	{ {
	"",
	"",
	 "__.",
	"(_/|",
	"",
	""
	},
	{0,0,1,0,0,0},
	3,3
	},

#define LB  1		/* b type 1 */
	{ {
	"",
	  "/",
 	 "/_",
	"/_)",
	"",
	""
	},
	{0,2,1,0,0,0},
	3,2
	},

#define LC  2		/* c type 1 */
	{ {
	"",
	"",
	 "_.",
	"(_",
	"",
	""}
	,
	{0,0,1,0,0,0},
	3,3
	},

#define LD  3		/* d type 1 */
	{ {
	"",
	    "/",
	 "__/",
	"(_/",
	"",
	""
	},
	{0,4,1,0,0,0},
	3,3
	},

#define LE  4		/* e type 1 */
	{ {
	"",
	"",
	 "_",
	"</",
	"",
	""
	},
	{0,0,1,0,0,0},
	3,3
	},

#define LF  5		/* f type 1 */
	{ {
	"",
	    "/)",
	   "// ",
	  "//",
	 "/>",
	"</ "
	},
	{0,4,3,2,1,0},
	3,3
	},

#define LG  6		/* g type 1 */
	{ {
	"",
	"",
	 "_,",
	"(_)",
	 "/|",
	"|/"
	},
	{0,0,1,0,1,0},
	3,3
	},

#define LH  7		/* h type 1 */
	{ {
	"",
	  "/",
	 "/_",
	"/ /",
	"",
	""
	},
	{0,2,1,0,0,0},
	3,3
	},

#define LI  8		/* i type 1 */
	{ {
	"",
	"",
	 "o",
	"<",
	"",
	""
	},
	{0,0,1,0,0,0},
	3,3
	},

#define LJ  9		/* j type 1 */
	{ {
	"",
	"",
	    "o",
	   "/",
	  "/",
	"-'"
	},
	{0,0,4,3,2,0},
	3,3
	},

#define LK 10		/* k type 1 */
	{ {
	"",
	  "/",
	 "/_ ",
	"/ <",
	"",
	""
	},
	{0,2,1,0,0,0},
	3,3
	},

#define LL 11		/* l type 1 */
	{ {
	   "_",
	  "//",
	 "//",
	"</",
	"",
	""
	},
	{3,2,1,0,0,0},
	3,3
	},

#define LM 12		/* m type 1 */
	{ {
	"",
	"",
	 "______",
	"/ / / <",
	"",
	""
	},
	{0,0,1,0,0,0},
	3,3
	},

#define LN 13		/* n type 1 */
	{ {
	"",
	"",
	 "____",
	"/ / <",
	"",
	""
	},
	{0,0,1,0,0,0},
	3,3
	},

#define LO 14		/* o type 1 */
	{ {
	"",
	"",
	 "_",
	"(_)",
	"",
	""
	},
	{0,0,1,0,0,0},
	3,2
	},

#define LP 15		/* p type 1 */
	{ {
	"",
	"",
	   "_",
	  "/_)",
	 "/",
	"'"
	},
	{0,0,3,2,1,0},
	3,3
	},

#define LQ 16		/* q type 1 */
	{ {
	"",
	"",
	 "_,",
	"(_)",
	 "/>",
	"|/"
	},
	{0,0,1,0,1,0},
	3,3
	},

#define LR 17		/* r type 1 */
	{ {
	"",
	"",
	 "__",
	"/ (",
	"",
	""
	},
	{0,0,1,0,0,0},
	3,3
	},

#define LS 18		/* s type 1 */
	{ {
	"",
	"",
	 "_",
	"/_)",
	"",
	""
	},
	{0,0,1,0,0,0},
	3,3
	},

#define LT 19		/* t type 1 */
	{ {
	"",
	 "_/_",
	 "/",
	"<_",
	"",
	""
	},
	{0,1,1,0,0,0},
	3,3
	},

#define LU 20		/* u type 1 */
	{ {
	"",
	"",
	 ". .",
	"(_/",
	"",
	""
	},
	{0,0,1,0,0,0},
	3,3
	},

#define LV 21		/* v type 1 */
	{ {
	"",
	"",
	", ",
	"\\/",
	"",
	""
	},
	{0,0,0,0,0,0},
	3,2
	},

#define LW 22		/* w type 1 */
	{ {
	"",
	"",
	 ", , ,",
	"(_(_/",
	"",
	""
	},
	{0,0,1,0,0,0},
	3,3
	},

#define LX 23		/* x type 1 */
	{ {
	"",
	"",
	 "_.,",
	"/ /\\",
	"",
	""
	},
	{0,0,1,0,0,0},
	3,3
	},

#define LY 24		/* y type 1 */
	{ {
	"",
	"",
	 "__  ,",
	"/ (_/",
	   "/",
	  "'"
	 },
	{0,0,1,0,3,2},
	3,3
	},

#define LZ 25		/* z type 1 */
	{ {
	"",
	"",
	 "__.",
	"/  |",
	  "(|",
	""
	},
	{0,0,1,0,2,0},
	3,3
	},

#define NA 26		/* a type 2 */
	{ {
	"",
	"",
	 "__.",
	"(_/|",
	"",
	""
	},
	{0,0,1,0,0,0},
	2,3
	},

#define NB 27		/* b type 2 */
	{ {
	"",
	  "/",
	 "/_",
	"/_)",
	"",
	""
	},
	{0,2,1,0,0,0},
	2,2
	},

#define NC 28		/* c type 2 */
	{ {
	"",
	"",
	"__.",
	"(_",
	"",
	""
	},
	{0,0,0,0,0,0},
	2,3
	},

#define ND 29		/* d type 2 */
	{ {
	"",
	    "/",
	"___/",
	"(_/",
	"",
	""
	},
	{0,4,0,0,0,0},
	2,3
	},

#define NE 30		/* e type 2 */
	{ {
	"",
	"",
	 "/>",
	"(_",
	"",
	""
	},
	{0,0,1,0,0,0},
	2,3
	},

#define NF 31		/* f type 2 */
	{ {
	"",
	    "/)",
	   "//",
	  "/_",
	 "/>",
	"</"
	},
	{0,4,3,2,1,0},
	2,3
	},

#define NG 32		/* g type 2 */
	{ {
	"",
	"",
	"__,",
	"(_)",
	 "/|",
	"|/"
	},
	{0,0,0,0,1,0},
	2,3
	},

#define NH 33		/* h type 2 */
	{ {
	"",
	  "/",
	 "/_",
	"/ /",
	"",
	""
	},
	{0,2,1,0,0,0},
	2,3
	},

#define NI 34		/* i type 2 */
	{ {
	"",
	"",
	 "o",
	"<_",
	"",
	""
	},
	{0,0,1,0,0,0},
	2,3
	},

#define NJ 35		/* j type 2 */
	{ {
	"",
	"",
	   "_o",
	   "/_",
	  "/",
	"-'"
	},
	{0,0,3,3,2,0},
	2,3
	},

#define NK 36		/* k type 2 */
	{ {
	"",
	  "/",
	 "/_",
	"/ <",
	"",
	""
	},
	{0,2,1,0,0,0},
	2,3
	},

#define NL 37		/* l type 2 */
	{ {
	  "_",
	 "//",
	"|/",
	 "\\",
	"",
	""
	},
	{2,1,0,1,0,0},
	2,3
	},

#define NM 38		/* m type 2 */
	{ {
	"",
	"",
	"_______",
	"/ / / <",
	"",
	""
	},
	{0,0,0,0,0,0},
	2,3
	},

#define NN 39		/* n type 2 */
	{ {
	"",
	"",
	"_____",
	"/ / <",
	"",
	""
	},
	{0,0,0,0,0,0},
	2,3
	},

#define NO 40		/* o type 2 */
	{ {
	"",
	"",
	"___",
	"(_)",
	"",
	""
	},
	{0,0,0,0,0,0},
	2,2
	},

#define NP 41		/* p type 2 */
	{ {
	"",
	"",
	  "__",
	  "/_)",
	 "/",
	"'"
	},
	{0,0,2,2,1,0},
	2,3
	},

#define NQ 42		/* q type 2 */
	{ {
	"",
	"",
	"__,",
	"(_)",
	 "/>",
	"|/"
	},
	{0,0,0,0,1,0},
	2,3
	},

#define NR 43		/* r type 2 */
	{ {
	"",
	"",
	",_",
	 "(",
	"",
	""
	},
	{0,0,0,1,0,0},
	2,3
	},

#define NS 44		/* s type 2 */
	{ {
	"",
	"",
	"___ ",
	 "\\_)_",
	"",
	""
	},
	{0,0,0,1,0,0},
	2,3
	},

#define NT 45		/* t type 2 */
	{ {
	"",
	 "_/_",
	"_/",
	"(_",
	"",
	""
	},
	{0,1,0,0,0,0},
	2,3
	},

#define NU 46		/* u type 2 */
	{ {
	"",
	"",
	"_. .",
	"(_/",
	"",
	""
	},
	{0,0,0,0,0,0},
	2,3
	},

#define NV 47		/* v type 2 */
	{ {
	"",
	"",
	"  .",
	"\\/",
	"",
	""
	},
	{0,0,0,0,0,0},
	2,3
	},

#define NW 48		/* w type 2 */
	{ {
	"",
	"",
	"_, , ,",
	"(_(_/",
	"",
	""
	},
	{0,0,0,0,0,0},
	2,3
	},

#define NX 49		/* x type 2 */
	{ {
	"",
	"",
	"_.,",
	 "/\\",
	"",
	""
	},
	{0,0,0,1,0,0},
	2,3
	},

#define NY 50		/* y type 2 */
	{ {
	"",
	"",
	"_  ,",
	"(_/__",
	 "/",
	"'"
	},
	{0,0,0,0,1,0},
	2,3
	},

#define NZ 51		/* z type 2 */
	{ {
	"",
	"",
	"__.",
	  "|",
	 "(|",
	""
	},
	{0,0,0,2,1,0},
	2,3
	},

#define UA 52		/* A */
	{ {
	   "__",
	  "/  )",
	 "/--/",
	"/  (",
	"",
	""
	},
	{3,2,1,0,0,0},
	3,3
	},

#define UB 53		/* B  */
	{ {
	   "__",
	  "/  )",
	 "/--<",
	"/___/",
	"",
	""
	},
	{3,2,1,0,0,0},
	3,3
	},

#define UC 54		/* C */
	{ {
	   "__",
	  "/  )",
	 "/",
	"(__/",
	"",
	""
	},
	{3,2,1,0,0,0},
	-1,-1
	},

#define UD 55		/* D  */
	{ {
	   "__",
	  "/  )",
	 "/  /",
	"/__/",
	"",
	""
	},
	{3,2,1,0,0,0},
	3,3
	},

#define UE 56		/* E */
	{ {
	   "__",
	  "/  `",
	 "/--",
	"(___,",
	"",
	""
	},
	{3,2,1,0,0,0},
	-1,-1
	},

#define UF 57		/* F */
	{ {
	   "_____",
	   " /  '",
	 ",-/-,",
	"(_/",
	"",
	""
	},
	{3,3,1,0,0,0},
	-1,-1
	},

#define UG 58		/* G */
	{ {
	  "()  ,",
	  "/`-'|",
	 "/   /",
	"/__-<",
	"",
	""
	},
	{2,2,1,0,0,0},
	-1,3
	},

#define UH 59		/* H */
	{ {
	 "_    ,",
	"' )  /",
	 "/--/",
	"/  (",
	"",
	""
	},
	{1,0,1,0,0,0},
	-1,3
	},

#define UI 60		/* I */
	{ {
	     "_",
	    "| )",
	",---|/",
	 "\\_/ \\",
	"",
	""
	},
	{5,4,0,1,0,0},
	-1,3
	},

#define UJ 61		/* J */
	{ {
	  "___",
	 "(   >",
	  "__/",
	 "/ /",
	"<_/",
	""
	},
	{2,1,2,1,0,0},
	-1,2
	},

#define UK 62		/* K */
	{ {
	 "_   ,",
	"' ) /",
	 "/-<",
	"/   )",
	"",
	""
	},
	{1,0,1,0,0,0},
	-1,-1
	},

#define UL 63		/* L */
	{ {
	   "_",
	 "_//",
	 "/",
	"/___",
	"",
	""
	},
	{3,1,1,0,0,0},
	-1,-1
	},

#define UM 64		/* M */
	{ {
	 "_ _ _",
	"' ) ) )",
	 "/ / /",
	"/ ' (",
	"",
	""
	},
	{1,0,1,0,0,0},
	-1,3
	},

#define UN 65		/* N */
	{ {
	 "_ __",
	"' )  )",
	 "/  /",
	"/  (",
	"",
	""
	},
	{1,0,1,0,0,0},
	-1,3
	},

#define UO 66		/* O */
	{ {
	   "__",
	  "/ ')",
	 "/  /",
	"(__/",
	"",
	""
	},
	{3,2,1,0,0,0},
	-1,-1
	},

#define UP 67		/* P */
	{ {
	 "_ __",
	"' )  )",
	 "/--'",
	"/",
	"",
	""
	},
	{1,0,1,0,0,0},
	-1,-1
	},

#define UQ 68		/* Q */
	{ {
	   "__",
	  "/  )",
	 "/  /",
	"(_\\/",
	   "`",
	""
	},
	{3,2,1,0,3,0},
	-1,-1
	},

#define UR 69		/* R */
	{ {
	 "_ __",
	"' )  )",
	 "/--'",
	"/  \\",
	"",
	""
	},
	{1,0,1,0,0,0},
	-1,3
	},

#define US 70		/* S */
	{ {
	  "()",
	  "/\\",
	 "/  )",
	"/__/_",
	"",
	""
	},
	{2,2,1,0,0,0},
	-1,3
	},

#define UT 71		/* T */
	{ {
	  "______",
	    "/  ",
	 "--/",
	"(_/",
	"",
	""
	},
	{2,4,1,0,0,0},
	-1,-1
	},

#define UU 72		/* U */
	{ {
	 "_    _",
	"' )  /",
	 "/  /",
	"(__/",
	"",
	""
	},
	{1,0,1,0,0,0},
	-1,0
	},

#define UV 73		/* V */
	{ {
	 "_    ",
	"' )  /",
	 "(  /",
	  "\\/",
	"",
	""
	},
	{1,0,1,2,0,0},
	-1,0
	},

#define UW 74		/* W */
	{ {
	 "_     ",
	"' )   /",
	 "/ / /",
	"(_(_/",
	"",
	""
	},
	{1,0,1,0,0,0},
	-1,0
	},

#define UX 75		/* X */
	{ {
	 "_   ,",
	"' \\ /",
	   "X",
	  "/ \\",
	"",
	""
	},
	{1,0,3,2,0,0},
	-1,3
	},

#define UY 76		/* Y */
	{ {
	 "_    ,",
	"' )  /",
	 "/  /",
	"(__/",
	 "//",
	"(/"
	},
	{1,0,1,0,1,0},
	-1,3
	},

#define UZ 77		/* Z */
	{ {
	"___",
	  "/",
	 "/",
	"/__",
	"",
	""
	},
	{0,2,1,0,0,0},
	-1,-1
	},

#define PE 78	 	/* ! */
	{ {
	   "/",
	  "/",
	 "'",
	"o",
	"",
	""
	},
	{3,2,1,0,0,0},
	-1,-1
	},

#define PU 79		/* " */
	{ {
	"o o",
	"' '",
	"",
	"",
	"",
	""
	},
	{0,0,0,0,0,0},
	-1,-1
	},

#define PX 80		/* # */
	{ {
	   "/ /",
	 "-/-/-",
	"-/-/-",
	"/ /",
	"",
	""
	},
	{3,1,0,0,0,0},
	-1,-1
	},

#define PK 81		/* $ */
	{ {
	  ",-/-",
	 "(_/",
	  "/ )",
	"-/-'",
	"",
	""
	},
	{2,1,2,0,0,0},
	-1,-1
	},

#define PF 82		/* ' */
	{ {
	"o",
	"'",
	"",
	"",
	"",
	""
	},
	{0,0,0,0,0,0},
	-1,-1
	},

#define PC 83		/* , */
	{ {
	"",
	"",
	"",
	"o",
	"'",
	""
	},
	{0,0,0,0,0,0},
	-1,-1
	},

#define PM 84		/* - */
	{ {
	"",
	"",
	"---",
	"",
	"",
	""
	},
	{0,0,0,0,0,0},
	-1,-1
	},

#define PP 85		/* . */
	{ {
	"",
	"",
	"",
	"o",
	"",
	""
	},
	{0,0,0,0,0,0},
	-1,-1
	},

#define PD 86		/* : */
	{ {
	"",
	  "o",
	"",
	"o",
	"",
	""
	},
	{0,2,0,0,0,0},
	-1,-1
	},

#define PS 87		/* ; */
	{ {
	"",
	  "o",
	"",
	"o",
	"'",
	""
	},
	{0,2,0,0,0,0},
	-1,-1
	},

#define PQ 88		/* ? */
	{ {
	  "__",
	  "__)",
	 "/",
	"o",
	"",
	""
	},
	{2,2,1,0,0,0},
	-1,-1
	},

#define PB 89		/* ` */
	{ {
	"o",
	"`",
	"",
	"",
	"",
	""
	},
	{0,0,0,0,0,0},
	-1,-1
	}
	};

short code1[] = {
     /*  0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15 */
	XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX,
	XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX,
	SP, PE, PU, PX, PK, XX, XX, PF, XX, XX, XX, XX, PC, PM, PP, XX,
	SN, SN, SN, SN, SN, SN, SN, SN, SN, SN, PD, PS, XX, XX, XX, PQ,
	XX, UA, UB, UC, UD, UE, UF, UG, UH, UI, UJ, UK, UL, UM, UN, UO,
	UP, UQ, UR, US, UT, UU, UV, UW, UX, UY, UZ, XX, XX, XX, XX, ST,
	PB, LA, LB, LC, LD, LE, LF, LG, LH, LI, LJ, LK, LL, LM, LN, LO,
	LP, LQ, LR, LS, LT, LU, LV, LW, LX, LY, LZ, XX, XX, XX, XX, XX };

short code2[] = {
     /*  0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15 */
	XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX,
	XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX,
	SP, PE, PU, PX, PK, XX, XX, PF, XX, XX, XX, XX, PC, PM, PP, XX,
	SN, SN, SN, SN, SN, SN, SN, SN, SN, SN, PD, PS, XX, XX, XX, PQ,
	XX, UA, UB, UC, UD, UE, UF, UG, UH, UI, UJ, UK, UL, UM, UN, UO,
	UP, UQ, UR, US, UT, UU, UV, UW, UX, UY, UZ, XX, XX, XX, XX, ST,
	PB, NA, NB, NC, ND, NE, NF, NG, NH, NI, NJ, NK, NL, NM, NN, NO,
	NP, NQ, NR, NS, NT, NU, NV, NW, NX, NY, NZ, XX, XX, XX, XX, XX };
/*
 * end of "font.c"
 */
