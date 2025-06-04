/*
 * $XConsortium: Layout.c,v 1.1 91/09/13 18:51:44 keith Exp $
 *
 * Copyright 1991 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL M.I.T.
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Keith Packard, MIT X Consortium
 */     

/*
 * Please note that this is a modified version of
 * Keith Packard's Layout widget.
 *
 * Modified Version 1.0
 *
 * Johannes Plass, 11/94
 * plass@dipmza.physik.uni-mainz.de
 *
*/ 

#include <ctype.h>
#include <stdio.h>
#ifdef VMS
#   include    <X11_DIRECTORY/Xlib.h>
#   include    <X11_DIRECTORY/Xresource.h>
#   include    <X11_DIRECTORY/IntrinsicP.h>
#   include    <X11_DIRECTORY/cursorfont.h>
#   include    <X11_DIRECTORY/StringDefs.h>
#   include    <XMU_DIRECTORY/Converters.h>
#   include    <stdlib.h> /* ###jp### inserted 21.10.94 */
static int LayYYlook(); /* ###jp### 06/18/95 */
#else
#   include    <X11/Xlib.h>
#   include    <X11/Xresource.h>
#   include    <X11/IntrinsicP.h>
#   include    <X11/cursorfont.h>
#   include    <X11/StringDefs.h>
#   include    <X11/Xmu/Converters.h>
#   include    <X11/StringDefs.h>
#endif

#include "LayoutP.h"

/*
#define MESSAGES
*/
#include "message.h"

#define New(t) (t *) malloc(sizeof (t))
#define Dispose(p)  free((char *) p)
#define Some(t,n)   (t*) malloc(sizeof(t) * n)
#define More(p,t,n) ((p)? (t *) realloc((char *) p, sizeof(t)*n):Some(t,n)

/*######################################################################*/

static void
PrintGlue (g)
    GlueRec g;
{
    if (g.order == 0 || g.value != 1.0)
        (void) printf ("%g", g.value);
    if (g.order > 0)
    {
        (void) printf ("%s", "inf");
        if (g.order > 1);
            (void) printf ("%d", g.order); 
    }
}

static void
PrintDirection (dir)
    LayoutDirection dir;
{
    switch (dir) {
    case LayoutHorizontal:
        (void) printf ("%s", "horizontal");
        break;
    case LayoutVertical:
        (void) printf ("%s", "vertical");
        break;
    default:
        (void) printf ("Unknown layout direction %d\n", dir);
        break;  
    }
}

static void
TabTo(level)
    int level;
{
    while (level--)
        (void) printf ("%s", "   ");
}

static void
PrintBox (box, level)
    BoxPtr          box;
    int             level;
{
    BoxPtr      child;
    
    TabTo (level);
    switch (box->type) {
    case BoxBox:
        PrintDirection (box->u.box.dir);
        (void) printf ("%s\n", "BoxBox");
        for (child = box->u.box.firstChild; child; child = child->nextSibling)
            PrintBox (child, level+1);
            TabTo (level);
        break;
    case WidgetBox:
        (void) printf ("%s", XrmQuarkToString (box->u.widget.quark));
        break;
    case GlueBox:
        (void) printf ("%s", "glue");
        break;
    case VariableBox:
        (void) printf ("variable %s", XrmQuarkToString (box->u.variable.quark));
        break;
    }
    (void) printf ("%s", "<");
    (void) printf ("%s", "+"); 
    PrintGlue (box->params.stretch[LayoutHorizontal]);
    (void) printf ("%s", "-"); 
    PrintGlue (box->params.shrink[LayoutHorizontal]);
    (void) printf ("%s", "*");
    (void) printf ("%s", "+"); 
    PrintGlue (box->params.stretch[LayoutVertical]);
    (void) printf ("%s", "-"); 
    PrintGlue (box->params.shrink[LayoutVertical]);
    (void) printf ("%s", ">");
    (void) printf (" size: %dx%d", box->size[0], box->size[1]);
    (void) printf (" natural: %dx%d\n", box->natural[0], box->natural[1]);
}

/*######################################################################*/


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

static LayoutPtr    *dest;

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

#define LayYYclearin LayYYchar = -1
#define LayYYerrok LayYYerrflag = 0
#ifndef YYMAXDEPTH
#   define YYMAXDEPTH 150
#endif

# define YYERRCODE 256

# define U(x) ((x)&0377)
# define BEGIN LayYYbgin = LayYYsvec + 1 +
# define INITIAL 0
# define YYLERR LayYYsvec
# define YYSTATE (LayYYestate-LayYYsvec-1)
# define YYOPTIM 1
# define YYLMAX 200
# define YYNEWLINE 10
# define output(c) putc(c,LayYYout)
# define LayYYmore() (LayYYmorfg=1)
# define ECHO fprintf(LayYYout, "%s",LayYYtext)
# define REJECT { nstr = LayYYreject(); goto LayYYfussy;}

struct LayYYsvf { 
	struct LayYYwork *LayYYstoff;
	struct LayYYsvf *LayYYother;
	int *LayYYstops;
};

#ifdef VMS
   static char LayYYtext[YYLMAX];
   static char LayYYsbuf[YYLMAX];
   static char *LayYYsptr = LayYYsbuf;   
   static int LayYYprevious = YYNEWLINE;
   static YYSTYPE LayYYlval, LayYYval;
   static int LayYYchar = -1; /*forgot the -1 ###jp### 1/95*/
   static short LayYYerrflag = 0; 
   static int LayYYleng;
   static int LayYYmorfg;
   static int LayYYtchar;
   static struct LayYYsvf *LayYYestate;
#  define LayYYin stdin
#  define LayYYout stdout
#else
   char LayYYtext[YYLMAX];
   char LayYYsbuf[YYLMAX];
   char *LayYYsptr = LayYYsbuf;   
   int LayYYprevious = YYNEWLINE;
   YYSTYPE LayYYlval, LayYYval;
   int LayYYchar = -1;
   short LayYYerrflag = 0 ; 
   int LayYYleng;
   int LayYYmorfg;
   int LayYYtchar;
   struct LayYYsvf *LayYYestate;
   FILE *LayYYin = {stdin}, *LayYYout = {stdout};
#endif

static char *LayYYsourcebase, *LayYYsource;

#define input() (*LayYYsource++)
#define unput(c)    (--LayYYsource)

#ifdef VMS /*###jp### 06/18/95*/
   static
#endif
LayYYwrap() /*###jp### moved here 21.10.94 */
{
   return 1;
}

static int count(s,c) /*###jp### moved here 21.10.94 */
    char    *s;
    char    c;
{
    int	i = 0;
    while (*s) if (*s++ == c) i++;
    return i;
}

#ifdef VMS
static
#endif
LayYYlex()
{
   int nstr; 
   while((nstr = LayYYlook()) >= 0)
      LayYYfussy: switch(nstr) {
      case 0:	if(LayYYwrap()) return(0);					break;
      case 1:	return VERTICAL;						break;
      case 2:	return HORIZONTAL;						break;
      case 3:	return OC;							break;
      case 4:	return CC;							break;
      case 5:	return OP;							break;
      case 6:	return CP;							break;
      case 7:	return OA;							break;
      case 8:	return CA;							break;
      case 9:	{ LayYYlval.ival = 1; return INFINITY; }			break;
      case 10:	{ LayYYlval.ival = count(LayYYtext, 'f'); return INFINITY; }	break;
      case 11:	{ LayYYlval.ival = atoi(LayYYtext); return NUMBER; }		break;
      case 12:	{ return EQUAL; }						break;
      case 13:	{ return DOLLAR; }						break;
      case 14:	{ LayYYlval.oval = Plus; return PLUS; }				break;
      case 15:	{ LayYYlval.oval = Minus; return MINUS; }			break;
      case 16:	{ LayYYlval.oval = Times; return TIMES; }			break;
      case 17:	{ LayYYlval.oval = Divide; return DIVIDE; }			break;
      case 18:	{ LayYYlval.oval = Percent; return PERCENT; }			break;
      case 19:	{ LayYYlval.oval = Percent; return PERCENTOF; }			break;
      case 20:	return WIDTH;							break;
      case 21:	return HEIGHT;							break;
      case 22:	{ 
		   LayYYtext[LayYYleng - 1] = '\0'; /*the author seems to know the problem ###jp###*/
		   LayYYlval.qval = XrmStringToQuark (LayYYtext+1);
 		   return NAME;
		}								break;
      case 23:	{ 
		   LayYYtext[LayYYleng - 1] = '\0';
		   LayYYlval.qval = XrmStringToQuark (LayYYtext);
 		   return NAME;
		}								break;
      case 24:	;								break;
      case 25:	;								break;
      case 26:	;								break;
      case 27:	fprintf (stderr, "ignoring %c\n", *LayYYtext);			break;
      case -1:									break;
      default:	fprintf(LayYYout,"bad switch LayYYlook %d",nstr);
      }
   return(0);
}

#ifdef VMS /*###jp### 06/18/95*/
   static
#endif
LayYYsetsource(s)
    char    *s;
{
    LayYYsourcebase = LayYYsource = s;
}

#ifdef VMS /*###jp### 06/18/95*/
   static
#endif
LayYYerror(s)
    char    *s;
{
    char    *t;
    
    fprintf (stderr, "%s\n", s);
    t = LayYYsource - 50;
    if (t < LayYYsourcebase) t = LayYYsourcebase;
    while (*t && t < LayYYsource + 50) {
	if (t == LayYYsource) putc ('@', stderr);
	putc (*t++, stderr);
    }
    if (t == LayYYsource) putc ('@', stderr);
    if (!*t) fprintf (stderr, "<EOF>");
    fprintf (stderr, "\n");
}

#ifdef VMS
   static
#endif 
int LayYYvstop[] ={
   0,27,0,25,27,0,26,0,24,27,0,13,27,0,18,27,0,5,27,0,6,27,0,16,27,0,14,27,0,15,27,
   0,17,27,0,11,27,0,7,27,0,12,27,0,8,27,0,23,27,0,27,0,23,27,0,23,27,0,23,27,
   0,23,27,0,3,27,0,4,27,0,11,0,23,0,22,0,23,0,23,0,23,0,23,0,23,0,19,0,23,0,23,
   0,10,23,0,23,0,23,0,23,0,23,0,10,23,0,23,0,23,0,23,0,23,0,23,0,23,0,23,0,20,23,
   0,21,23,0,23,0,23,0,23,0,23,0,23,0,23,0,23,0,9,23,0,1,23,0,23,0,2,23,0,0
};

# define YYTYPE unsigned char

struct LayYYwork { YYTYPE verify, advance; } 

#ifdef VMS
   static
#endif
LayYYcrank[] ={
   0,0,	0,0,	1,3,	0,0,	
   0,0,	0,0,	0,0,	0,0,	
   0,0,	0,0,	1,4,	1,5,	
   8,27,	8,27,	0,0,	0,0,	
   0,0,	0,0,	0,0,	0,0,	
   0,0,	0,0,	0,0,	0,0,	
   0,0,	0,0,	0,0,	0,0,	
   0,0,	0,0,	0,0,	0,0,	
   0,0,	1,6,	0,0,	8,27,	
   0,0,	1,7,	1,8,	0,0,	
   0,0,	1,9,	1,10,	1,11,	
   1,12,	0,0,	1,13,	0,0,	
   1,14,	1,15,	15,29,	15,29,	
   15,29,	15,29,	15,29,	15,29,	
   15,29,	15,29,	15,29,	15,29,	
   0,0,	1,16,	1,17,	1,18,	
   0,0,	0,0,	1,19,	2,6,	
   0,0,	0,0,	0,0,	2,7,	
   2,8,	0,0,	0,0,	2,9,	
   2,10,	2,11,	2,12,	0,0,	
   2,13,	0,0,	2,14,	31,31,	
   31,31,	31,31,	31,31,	31,31,	
   31,31,	31,31,	31,31,	31,31,	
   31,31,	1,20,	0,0,	2,16,	
   2,17,	2,18,	0,0,	0,0,	
   0,0,	0,0,	0,0,	0,0,	
   57,60,	1,21,	1,22,	36,42,	
   21,32,	23,35,	28,37,	34,40,	
   24,36,	32,38,	8,28,	38,43,	
   39,44,	22,34,	21,33,	1,23,	
   1,24,	33,39,	35,41,	41,47,	
   1,25,	40,45,	1,26,	2,20,	
   40,46,	42,48,	43,49,	44,50,	
   45,45,	46,51,	47,52,	48,53,	
   49,54,	50,55,	51,56,	2,21,	
   2,22,	52,57,	55,58,	56,59,	
   58,61,	59,62,	60,63,	61,64,	
   64,65,	0,0,	0,0,	0,0,	
   0,0,	2,23,	2,24,	0,0,	
   0,0,	0,0,	2,25,	0,0,	
   2,26,	19,30,	19,30,	19,30,	
   19,30,	19,30,	19,30,	19,30,	
   19,30,	19,30,	19,30,	0,0,	
   0,0,	0,0,	0,0,	0,0,	
   0,0,	0,0,	19,30,	19,30,	
   19,30,	19,30,	19,30,	19,30,	
   19,30,	19,30,	19,30,	19,30,	
   19,30,	19,30,	19,30,	19,30,	
   19,30,	19,30,	19,30,	19,30,	
   19,30,	19,30,	19,30,	19,30,	
   19,30,	19,30,	19,30,	19,30,	
   0,0,	0,0,	0,0,	0,0,	
   19,30,	0,0,	19,30,	19,30,	
   19,30,	19,30,	19,30,	19,30,	
   19,30,	19,30,	19,30,	19,30,	
   19,30,	19,30,	19,30,	19,30,	
   19,30,	19,30,	19,30,	19,30,	
   19,30,	19,30,	19,30,	19,30,	
   19,30,	19,30,	19,30,	19,30,	
   20,31,	20,31,	20,31,	20,31,	
   20,31,	20,31,	20,31,	20,31,	
   20,31,	20,31,	20,31,	20,31,	
   20,31,	20,31,	20,31,	20,31,	
   20,31,	20,31,	20,31,	20,31,	
   20,31,	20,31,	20,31,	20,31,	
   20,31,	20,31,	0,0,	0,0,	
   0,0,	0,0,	20,31,	0,0,	
   20,31,	20,31,	20,31,	20,31,	
   20,31,	20,31,	20,31,	20,31,	
   20,31,	20,31,	20,31,	20,31,	
   20,31,	20,31,	20,31,	20,31,	
   20,31,	20,31,	20,31,	20,31,	
   20,31,	20,31,	20,31,	20,31,	
   20,31,	20,31,	0,0,	0,0,	
   0,0
};

#ifdef VMS
   static
#endif
struct LayYYsvf LayYYsvec[] ={
   0,	0,	0,
   LayYYcrank+-1,	0,		0,	
   LayYYcrank+-35,	LayYYsvec+1,	0,	
   LayYYcrank+0,	0,		LayYYvstop+1,
   LayYYcrank+0,	0,		LayYYvstop+3,
   LayYYcrank+0,	0,		LayYYvstop+6,
   LayYYcrank+0,	0,		LayYYvstop+8,
   LayYYcrank+0,	0,		LayYYvstop+11,
   LayYYcrank+3,	0,		LayYYvstop+14,
   LayYYcrank+0,	0,		LayYYvstop+17,
   LayYYcrank+0,	0,		LayYYvstop+20,
   LayYYcrank+0,	0,		LayYYvstop+23,
   LayYYcrank+0,	0,		LayYYvstop+26,
   LayYYcrank+0,	0,		LayYYvstop+29,
   LayYYcrank+0,	0,		LayYYvstop+32,
   LayYYcrank+2,	0,		LayYYvstop+35,
   LayYYcrank+0,	0,		LayYYvstop+38,
   LayYYcrank+0,	0,		LayYYvstop+41,
   LayYYcrank+0,	0,		LayYYvstop+44,
   LayYYcrank+113,	0,		LayYYvstop+47,
   LayYYcrank+171,	0,		LayYYvstop+50,
   LayYYcrank+7,	LayYYsvec+19,	LayYYvstop+52,
   LayYYcrank+7,	LayYYsvec+19,	LayYYvstop+55,
   LayYYcrank+8,	LayYYsvec+19,	LayYYvstop+58,
   LayYYcrank+7,	LayYYsvec+19,	LayYYvstop+61,
   LayYYcrank+0,	0,		LayYYvstop+64,
   LayYYcrank+0,	0,		LayYYvstop+67,
   LayYYcrank+0,	LayYYsvec+8,	0,	
   LayYYcrank+8,	0,		0,	
   LayYYcrank+0,	LayYYsvec+15,	LayYYvstop+70,
   LayYYcrank+0,	LayYYsvec+19,	LayYYvstop+72,
   LayYYcrank+35,	LayYYsvec+20,	LayYYvstop+74,
   LayYYcrank+8,	LayYYsvec+19,	LayYYvstop+76,
   LayYYcrank+7,	LayYYsvec+19,	LayYYvstop+78,
   LayYYcrank+9,	LayYYsvec+19,	LayYYvstop+80,
   LayYYcrank+8,	LayYYsvec+19,	LayYYvstop+82,
   LayYYcrank+7,	LayYYsvec+19,	LayYYvstop+84,
   LayYYcrank+0,	0,		LayYYvstop+86,
   LayYYcrank+12,	LayYYsvec+19,	LayYYvstop+88,
   LayYYcrank+11,	LayYYsvec+19,	LayYYvstop+90,
   LayYYcrank+23,	LayYYsvec+19,	LayYYvstop+92,
   LayYYcrank+7,	LayYYsvec+19,	LayYYvstop+95,
   LayYYcrank+13,	LayYYsvec+19,	LayYYvstop+97,
   LayYYcrank+26,	LayYYsvec+19,	LayYYvstop+99,
   LayYYcrank+9,	LayYYsvec+19,	LayYYvstop+101,
   LayYYcrank+30,	LayYYsvec+19,	LayYYvstop+103,
   LayYYcrank+23,	LayYYsvec+19,	LayYYvstop+106,
   LayYYcrank+29,	LayYYsvec+19,	LayYYvstop+108,
   LayYYcrank+31,	LayYYsvec+19,	LayYYvstop+110,
   LayYYcrank+20,	LayYYsvec+19,	LayYYvstop+112,
   LayYYcrank+26,	LayYYsvec+19,	LayYYvstop+114,
   LayYYcrank+33,	LayYYsvec+19,	LayYYvstop+116,
   LayYYcrank+42,	LayYYsvec+19,	LayYYvstop+118,
   LayYYcrank+0,	LayYYsvec+19,	LayYYvstop+120,
   LayYYcrank+0,	LayYYsvec+19,	LayYYvstop+123,
   LayYYcrank+32,	LayYYsvec+19,	LayYYvstop+126,
   LayYYcrank+27,	LayYYsvec+19,	LayYYvstop+128,
   LayYYcrank+7,	LayYYsvec+19,	LayYYvstop+130,
   LayYYcrank+28,	LayYYsvec+19,	LayYYvstop+132,
   LayYYcrank+24,	LayYYsvec+19,	LayYYvstop+134,
   LayYYcrank+38,	LayYYsvec+19,	LayYYvstop+136,
   LayYYcrank+50,	LayYYsvec+19,	LayYYvstop+138,
   LayYYcrank+0,	LayYYsvec+19,	LayYYvstop+140,
   LayYYcrank+0,	LayYYsvec+19,	LayYYvstop+143,
   LayYYcrank+40,	LayYYsvec+19,	LayYYvstop+146,
   LayYYcrank+0,	LayYYsvec+19,	LayYYvstop+148,
   0,	0,	0
};

#ifdef VMS
   static
#endif
struct LayYYwork *LayYYtop = LayYYcrank+293;
#ifdef VMS
   static
#endif
struct LayYYsvf *LayYYbgin = LayYYsvec+1;

#ifdef VMS
   static
#endif
char LayYYmatch[] ={
   00  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
   01  ,011 ,012 ,01  ,01  ,01  ,01  ,01  ,
   01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
   01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
   011 ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
   01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
   '0' ,'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,
   '0' ,'0' ,01  ,01  ,01  ,01  ,01  ,01  ,
   01  ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
   'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
   'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
   'A' ,'A' ,'A' ,01  ,01  ,01  ,01  ,'A' ,
   01  ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
   'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
   'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
   'A' ,'A' ,'A' ,01  ,01  ,01  ,01  ,01  ,
   01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
   01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
   01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
   01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
   01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
   01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
   01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
   01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
   01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
   01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
   01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
   01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
   01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
   01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
   01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
   01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
   0
};

#ifdef VMS
   static
#endif
char LayYYextra[] ={
   0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,
   0
};

/*	ncform	4.1	83/08/11	*/

# define YYU(x) x

#ifdef VMS
   static
#endif
struct LayYYsvf *LayYYlstate[YYLMAX], **LayYYlsp, **LayYYolsp;

#ifdef VMS
   static
#endif
int *LayYYfnd;

#ifdef VMS /*###jp### 06/18/95*/
   static
#endif
LayYYback(p, m) /*###jp### moved here 21.10.94 */
	int *p;
{
   if (p==0) return(0);
   while (*p) {
	if (*p++ == m) return(1);
   }
   return(0);
}

#ifdef VMS /*###jp### 06/18/95*/
static int
#endif
LayYYlook()
{
   register struct LayYYsvf *LayYYstate, **lsp;
   register struct LayYYwork *LayYYt;
   struct LayYYsvf *LayYYz;
   int LayYYch;
   struct LayYYwork *LayYYr;
#  ifdef LEXDEBUG
      int debug;
#  endif
   char *LayYYlastch;

   /* start off machines */
#  ifdef LEXDEBUG
      debug = 1;
#  endif
   if (!LayYYmorfg) LayYYlastch = LayYYtext;
   else { LayYYmorfg=0; LayYYlastch = LayYYtext+LayYYleng; }
   for(;;) {
      lsp = LayYYlstate;
      LayYYestate = LayYYstate = LayYYbgin;
      if (LayYYprevious==YYNEWLINE) LayYYstate++;
      for (;;){
#        ifdef LEXDEBUG
            if(debug)fprintf(LayYYout,"state %d\n",LayYYstate-LayYYsvec-1);
#        endif
         LayYYt = LayYYstate->LayYYstoff;
         if (LayYYt == LayYYcrank) {		/* may not be any transitions */
            LayYYz = LayYYstate->LayYYother;
   	    if(LayYYz == 0) break;
	    if(LayYYz->LayYYstoff == LayYYcrank)break;
         }
         *LayYYlastch++ = LayYYch = input();
         tryagain:
#        ifdef LEXDEBUG
            if (debug) { 
               fprintf(LayYYout,"unsigned char "); 
               /*allprint(LayYYch);*/
	       putchar('\n');
            }
#        endif
         LayYYr = LayYYt;
         if ( (int)LayYYt > (int)LayYYcrank){
	    LayYYt = LayYYr + LayYYch;
	    if (LayYYt <= LayYYtop && LayYYt->verify+LayYYsvec == LayYYstate){
	       if (LayYYt->advance+LayYYsvec == YYLERR)	/* error transitions */
	          {unput(*--LayYYlastch);break;}
	       *lsp++ = LayYYstate = LayYYt->advance+LayYYsvec;
	       goto contin;
	    }
         }
#        ifdef YYOPTIM
            else if((int)LayYYt < (int)LayYYcrank) {		/* r < LayYYcrank */
               LayYYt = LayYYr = LayYYcrank+(LayYYcrank-LayYYt);
#              ifdef LEXDEBUG
	          if(debug)fprintf(LayYYout,"compressed state\n");
#              endif
	       LayYYt = LayYYt + LayYYch;
	       if (LayYYt <= LayYYtop && LayYYt->verify+LayYYsvec == LayYYstate){
	          if (LayYYt->advance+LayYYsvec == YYLERR)	/* error transitions */
	             {unput(*--LayYYlastch);break;}
	          *lsp++ = LayYYstate = LayYYt->advance+LayYYsvec;
	          goto contin;
	       }
	       LayYYt = LayYYr + YYU(LayYYmatch[LayYYch]);
#              ifdef LEXDEBUG
                  if (debug){
	             fprintf(LayYYout,"try fall back character ");
	             /*allprint(YYU(LayYYmatch[LayYYch])); */
	             putchar('\n');
	          }
#              endif
	       if (LayYYt <= LayYYtop && LayYYt->verify+LayYYsvec == LayYYstate){
	          if (LayYYt->advance+LayYYsvec == YYLERR)	/* error transition */
	             {unput(*--LayYYlastch);break;}
	          *lsp++ = LayYYstate = LayYYt->advance+LayYYsvec;
	          goto contin;
	       }
            }
	    if ((LayYYstate = LayYYstate->LayYYother) && (LayYYt= LayYYstate->LayYYstoff) != LayYYcrank){
#              ifdef LEXDEBUG
	          if(debug)fprintf(LayYYout,"fall back to state %d\n",LayYYstate-LayYYsvec-1);
#              endif
	       goto tryagain;
            }
#        endif
         else {unput(*--LayYYlastch); break;}
         contin:
#        ifdef LEXDEBUG
            if (debug){
               fprintf(LayYYout,"state %d char ",LayYYstate-LayYYsvec-1);
	       /*allprint(LayYYch);*/
	       putchar('\n');
	    }
#        endif
         ;
      }
#     ifdef LEXDEBUG
         if (debug){
            fprintf(LayYYout,"stopped at %d with ",*(lsp-1)-LayYYsvec-1);
            /*allprint(LayYYch); */
            putchar('\n');
         }
#     endif
      while (lsp-- > LayYYlstate){
         *LayYYlastch-- = 0;
         if (*lsp != 0 && (LayYYfnd= (*lsp)->LayYYstops) && *LayYYfnd > 0){
            LayYYolsp = lsp;
	    if (LayYYextra[*LayYYfnd]){		/* must backup */
	       while(LayYYback((*lsp)->LayYYstops,-*LayYYfnd) != 1 && lsp > LayYYlstate){
	          lsp--;
	          unput(*LayYYlastch--);
	       }
	    }
	    LayYYprevious = YYU(*LayYYlastch);
	    LayYYlsp = lsp;
	    LayYYleng = LayYYlastch-LayYYtext+1;
	    LayYYtext[LayYYleng] = 0;
#           ifdef LEXDEBUG
	       if (debug){
	          fprintf(LayYYout,"\nmatch ");
	          ECHO;
	          fprintf(LayYYout," action %d\n",*LayYYfnd);
	       }
#           endif
	    return(*LayYYfnd++);
         }
         unput(*LayYYlastch);
      }
      if (LayYYtext[0] == 0  /* && feof(LayYYin) */) {
         LayYYsptr=LayYYsbuf;
         return(0);
      }
      LayYYprevious = LayYYtext[0] = input();
      if (LayYYprevious>0) output(LayYYprevious);
      LayYYlastch=LayYYtext;
#     ifdef LEXDEBUG
         if (debug) putchar('\n');
#     endif
   }
}

/* the following are only used in the lex library */


#ifdef VMS /*###jp### 06/18/95*/
   static
#endif
LayYYinput()
{
   return(input());
}

#ifdef VMS /*###jp### 06/18/95*/
   static
#endif
LayYYoutput(c)
   int c;
{
   output(c);
}

#ifdef VMS /*###jp### 06/18/95*/
   static
#endif
LayYYunput(c)
   int c;
{
   unput(c);
}

#ifdef VMS /*###jp### 06/18/95*/
   static
#endif
LayYYsetdest (c)
    LayoutPtr	*c;
{
    BEGINMESSAGE(LayYYsetdest)
    dest = c;
    ENDMESSAGE(LayYYsetdest)
}

#ifdef VMS
   static
#endif
short LayYYexca[] ={
   -1, 1,0, -1,-2, 0,
};

# define YYNPROD 39
# define YYLAST 128

#ifdef VMS
   static
#endif  
short LayYYact[]={
  17,  62,   9,  18,  17,   4,   5,  18,  19,  13,
  12,  29,  19,  34,  33,  15,  16,  29,  17,  15,
  16,  18,  17,  60,  50,  18,  19,  13,  12,  39,
  19,  36,  17,  15,  16,  18,  53,  15,  16,  31,
  19,  41,  44,  45,  46,  24,  30,  15,  16,  42,
  43,  44,  45,  46,  23,   4,   5,  66,  63,  26,
  20,   6,  49,  38,  35,  32,  51,  10,  11,   2,
   7,  14,   1,  14,   3,  25,  22,  27,  28,  21,
   8,   0,   0,   0,   0,   0,   0,   0,  14,   0,
  40,  37,   0,   0,   0,   0,   0,   0,   0,  47,
  48,   0,   0,  54,  52,   0,   0,   0,  55,  56,
  57,  58,  59,   0,   0,  52,   0,  61,   0,   0,
   0,   0,   0,   0,  64,   0,   0,  65
};            

#ifdef VMS
   static
#endif
short LayYYpact[]={
  -211,-1000,-1000,-196,-1000,-1000,-261,-198,-261,-214,
  -200,-1000,-239,-239,-258,-217,-224,-257,-1000,-232,
  -1000,-1000,-1000,-243,-241,-1000,-241,-258,-258,-1000,
  -1000,-1000,-221,-257,-257,-258,-1000,-1000,-247,-229,
  -247,-1000,-257,-257,-257,-257,-257,-1000,-1000,-249,
  -229,-1000,-264,-1000,-202,-230,-230,-1000,-1000,-1000,
  -241,-1000,-1000,-1000,-247,-203,-1000
};

#ifdef VMS
   static
#endif
short LayYYpgo[]={
   0,  80,  70,  68,  76,  75,  66,  63,  62,  74, 67,  64,  65,  72
};

#ifdef VMS
   static
#endif
short LayYYr1[]={
   0,  13,   1,   1,   1,   1,   3,   2,   2,   4,
   4,   5,   5,   7,   7,   8,   8,   6,   6,   6,
  10,  10,  10,  11,  11,  11,  11,  11,  11,  12,
  12,  12,  12,  12,  12,  12,  12,   9,   9
};

#ifdef VMS
   static
#endif
short LayYYr2[]={
   0,   1,   2,   2,   3,   1,   4,   2,   1,   7,
   0,   4,   0,   2,   0,   2,   0,   2,   1,   1,
   2,   2,   1,   2,   2,   3,   2,   1,   2,   3,
   3,   3,   3,   3,   2,   2,   1,   1,   1
};

#ifdef VMS
   static
#endif
short LayYYchk[]={
-1000, -13,  -3,  -9, 266, 267, 257,  -2,  -1, 263,
 -10,  -3, 271, 270, -11, 276, 277, 261, 264, 269,
 258,  -2,  -4, 268, 259,  -5, 259, -11, -11, 275,
 263, 263, -12, 271, 270, -11, 263, -10,  -7, 270,
  -7, 262, 270, 271, 272, 273, 274, -12, -12,  -8,
 271,  -6, -11, 265,  -8, -12, -12, -12, -12, -12,
 272,  -6, 265, 260,  -7,  -8, 260
};

#ifdef VMS
   static
#endif
short LayYYdef[]={
   0,  -2,   1,   0,  37,  38,   0,   0,   8,  10,
  12,   5,   0,   0,  22,   0,   0,   0,  27,   0,
   6,   7,   2,   0,  14,   3,  14,  20,  21,  26,
  23,  24,   0,   0,   0,  36,  28,   4,  16,   0,
  16,  25,   0,   0,   0,   0,   0,  34,  35,   0,
   0,  13,  18,  19,   0,  29,  30,  31,  32,  33,
  14,  15,  17,  11,  16,   0,   9
};

#ifndef lint
   static char yaccpar_sccsid[] = "@(#)yaccpar	4.1	(Berkeley)	2/11/83";
#endif

#
# define YYFLAG -1000
# define YYERROR goto LayYYerrlab
# define YYACCEPT return(0)
# define YYABORT return(1)

/*	parser for yacc output	*/

#ifdef YYDEBUG
   int LayYYdebug = 1; /* 1 for debugging */
#endif

#ifdef VMS /*###jp### 06/18/95*/
   static
#endif
YYSTYPE LayYYv[YYMAXDEPTH]; /* where the values are stored */
#ifdef VMS
   static
#endif
int LayYYnerrs = 0;  /* number of errors */

#ifdef VMS
static  /*###jp### inserted 21.10.94 */
#endif
LayYYparse() {
   short LayYYs[YYMAXDEPTH];
   short LayYYj, LayYYm;
   register YYSTYPE *LayYYpvt;
   register short LayYYstate, *LayYYps, LayYYn;
   register YYSTYPE *LayYYpv;
   register short *LayYYxi;

   BEGINMESSAGE(LayYYparse) 
   LayYYstate = 0;
   LayYYchar = -1;
   LayYYnerrs = 0;
   LayYYerrflag = 0;
   LayYYps= &LayYYs[-1];
   LayYYpv= &LayYYv[-1];

LayYYstack:    /* put a state and value onto the stack */

#  ifdef YYDEBUG
	if( LayYYdebug  ) printf( "state %d, char 0%o\n", LayYYstate, LayYYchar );
#  endif
   if( ++LayYYps> &LayYYs[YYMAXDEPTH] ) { 
      LayYYerror( "yacc stack overflow" );
      INFMESSAGE(yacc stack overflow) 
      ENDMESSAGE(LayYYparse)
      return(1); 
   }
   *LayYYps = LayYYstate;
   ++LayYYpv;
   *LayYYpv = LayYYval;

LayYYnewstate:

   LayYYn = LayYYpact[LayYYstate];

   if( LayYYn<= YYFLAG ) goto LayYYdefault; /* simple state */

   if( LayYYchar<0 ) if( (LayYYchar=LayYYlex())<0 ) LayYYchar=0;
   if( (LayYYn += LayYYchar)<0 || LayYYn >= YYLAST ) goto LayYYdefault;

   if( LayYYchk[ LayYYn=LayYYact[ LayYYn ] ] == LayYYchar ){ /* valid shift */
      LayYYchar = -1;
      LayYYval = LayYYlval;
      LayYYstate = LayYYn;
      if( LayYYerrflag > 0 ) --LayYYerrflag;
      goto LayYYstack;
   }

LayYYdefault:
	/* default state action */

   if( (LayYYn=LayYYdef[LayYYstate]) == -2 ) {
      if( LayYYchar<0 ) if( (LayYYchar=LayYYlex())<0 ) LayYYchar = 0;
		/* look through exception table */

      for( LayYYxi=LayYYexca; (*LayYYxi!= (-1)) || (LayYYxi[1]!=LayYYstate) ; LayYYxi += 2 ) ; /* VOID */

      while( *(LayYYxi+=2) >= 0 ){
         if( *LayYYxi == LayYYchar ) break;
      }

      if( (LayYYn = LayYYxi[1]) < 0 ) {
         INFMESSAGE(layout accepted)
         ENDMESSAGE(LayYYparse)              
         return(0);   /* accept */
      }
   }

   if( LayYYn == 0 ){ /* error */
      /* error ... attempt to resume parsing */

      switch( LayYYerrflag ){
         case 0:   /* brand new error */
            LayYYerror( "syntax error" );
	    LayYYerrlab:
	    ++LayYYnerrs;
	 case 1:
	 case 2: /* incompletely recovered error ... try again */
            LayYYerrflag = 3;
            /* find a state where "error" is a legal shift action */
            while ( LayYYps >= LayYYs ) {
	       LayYYn = LayYYpact[*LayYYps] + YYERRCODE;
	       if( LayYYn>= 0 && LayYYn < YYLAST && LayYYchk[LayYYact[LayYYn]] == YYERRCODE ){
	           LayYYstate = LayYYact[LayYYn];  /* simulate a shift of "error" */
		   goto LayYYstack;
               }
	       LayYYn = LayYYpact[*LayYYps];
               /* the current LayYYps has no shift onn "error", pop stack */
#              ifdef YYDEBUG
	          if( LayYYdebug ) printf( "error recovery pops state %d, uncovers %d\n", *LayYYps, LayYYps[-1] );
#              endif
	       --LayYYps;
	       --LayYYpv;
	    }
            /* there is no state on the stack with an error shift ... abort */
	    LayYYabort:
               INFMESSAGE(aborting)
               ENDMESSAGE(LayYYparse)
	       return(1);
         case 3:  /* no shift yet; clobber input char */
#           ifdef YYDEBUG
	       if( LayYYdebug ) printf( "error recovery discards char %d\n", LayYYchar );
#           endif
	    if( LayYYchar == 0 ) goto LayYYabort; /* don't discard EOF, quit */
	    LayYYchar = -1;
	    goto LayYYnewstate;   /* try again in the same state */
      }

   }

   /* reduction by production LayYYn */

#  ifdef YYDEBUG
      if( LayYYdebug ) printf("reduce %d\n",LayYYn);
#  endif
   LayYYps -= LayYYr2[LayYYn];
   LayYYpvt = LayYYpv;
   LayYYpv -= LayYYr2[LayYYn];
   LayYYval = LayYYpv[1];
   LayYYm=LayYYn;
   /* consult goto table to find next state */
   LayYYn = LayYYr1[LayYYn];
   LayYYj = LayYYpgo[LayYYn] + *LayYYps + 1;
   if( LayYYj>=YYLAST || LayYYchk[ LayYYstate = LayYYact[LayYYj] ] != -LayYYn ) LayYYstate = LayYYact[LayYYpgo[LayYYn]];

   switch(LayYYm){			
      case 1: { 
        *dest = LayYYpvt[-0].bval; 
        INFMESSAGE(changed dest)
      }  break;
      case 2: {
         BoxPtr	box = New(LBoxRec);
	 box->nextSibling = 0;
	 box->type = WidgetBox;
	 box->params = *LayYYpvt[-0].pval;
	 Dispose (LayYYpvt[-0].pval);
	 box->u.widget.quark = LayYYpvt[-1].qval;
	 LayYYval.bval = box;
      }  break;
      case 3: {
	 BoxPtr	box = New(LBoxRec);
	 box->nextSibling = 0;
	 box->type = GlueBox;
	 box->params = *LayYYpvt[-0].pval;
	 Dispose (LayYYpvt[-0].pval);
	 box->u.glue.expr = LayYYpvt[-1].eval;
	 LayYYval.bval = box;
      }  break;
      case 4: {
	 BoxPtr	box = New(LBoxRec);
	 box->nextSibling = 0;
	 box->type = VariableBox;
	 box->u.variable.quark = LayYYpvt[-2].qval;
	 box->u.variable.expr = LayYYpvt[-0].eval;
	 LayYYval.bval = box;
/* ###important### ###bug### 
         PrintBox(box,0);
         Without the following initialization some of the
         unitialized (remember, malloc is used in New() above)
         to the various 'order' and 'value' fields remain uninitialized
         for the lifetime of the widget.
         Usually this will mess up the whole appearance of the widget.
For instance, with 'debug' turned on the following happens as far as
a variable called 'vs' is concerned:
  Starting the application
  1.call of PrintBox: variable vs<+6.658073e-39inf1-0*+0inf1-0inf1111621632> size: 0x1666884 natural:1818132x1667495
  2.call of PrintBox: variable vs<+6.658073e-39inf1-0*+0inf1-0inf1111621632> size: 989x-140 natural: 0x0
  3.call of PrintBox: variable vs<+6.658073e-39inf1-0*+0inf1-0inf1111621632> size: 964x-208 natural: 0x0
  4.call of PrintBox: variable vs<+6.658073e-39inf1-0*+0inf1-0inf1111621632> size: 969x-208 natural: 0x0
  Now the application is visible and it looks like a mess, most widgets are shifted out of the window.
         I wonder why unitialized values are obviously used later.
         Is it a bug in this widget or is it due to the changes
         I made to the code (I made some variables 'static' ...) ?
         Due to the run time protocols I assume the first is the case, but
         certainly I'm not sure.
         This should be fixed properly sometimes. Currently I hope we come away
         with the following lines. ###jp###
*/
         box->params.stretch[LayoutHorizontal].order = 0;
         box->params.shrink[LayoutHorizontal].order  = 0;
         box->params.stretch[LayoutVertical].order   = 0;
         box->params.shrink[LayoutVertical].order    = 0;
         box->params.stretch[LayoutHorizontal].value = 0;
         box->params.shrink[LayoutHorizontal].value  = 0;
         box->params.stretch[LayoutVertical].value   = 0;
         box->params.shrink[LayoutVertical].value    = 0;
      }  break;
      case 5: {
	 LayYYval.bval = LayYYpvt[-0].bval;
      }  break;
      case 6: {
	 BoxPtr	box = New(LBoxRec);
	 BoxPtr	child;

         box->nextSibling = 0;
	 box->parent = 0;
	 box->type = BoxBox;
	 box->u.box.dir = LayYYpvt[-3].lval;
	 box->u.box.firstChild = LayYYpvt[-1].bval;
	 for (child = LayYYpvt[-1].bval; child; child = child->nextSibling) {
	     if (child->type == GlueBox) {
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
      }  break;
      case 7: { 
	 LayYYpvt[-1].bval->nextSibling = LayYYpvt[-0].bval;
	 LayYYval.bval = LayYYpvt[-1].bval;
      }  break;
      case 8: {	
         LayYYval.bval = LayYYpvt[-0].bval; 
      }  break;
      case 9: {	
	 BoxParamsPtr	p = New(BoxParamsRec);
			
	 p->stretch[LayoutHorizontal] = LayYYpvt[-5].gval;
	 p->shrink[LayoutHorizontal] = LayYYpvt[-4].gval;
	 p->stretch[LayoutVertical] = LayYYpvt[-2].gval;
	 p->shrink[LayoutVertical] = LayYYpvt[-1].gval;
	 LayYYval.pval = p;
      }  break;
      case 10: {	
	 BoxParamsPtr	p = New(BoxParamsRec);
			
	 ZeroGlue (p->stretch[LayoutHorizontal]);
	 ZeroGlue (p->shrink[LayoutHorizontal]);
         ZeroGlue (p->stretch[LayoutVertical]);
	 ZeroGlue (p->shrink[LayoutVertical]);
	 LayYYval.pval = p;
      }  break;
      case 11: {	
	 BoxParamsPtr	p = New(BoxParamsRec);
			
	 p->stretch[LayoutHorizontal] = LayYYpvt[-2].gval;
	 p->shrink[LayoutHorizontal] = LayYYpvt[-1].gval;
	 p->stretch[LayoutVertical] = LayYYpvt[-2].gval;
	 p->shrink[LayoutVertical] = LayYYpvt[-1].gval;
	 LayYYval.pval = p;
      }  break;
      case 12: {	
	 BoxParamsPtr	p = New(BoxParamsRec);
			
	 ZeroGlue (p->stretch[LayoutHorizontal]);
	 ZeroGlue (p->shrink[LayoutHorizontal]);
	 ZeroGlue (p->stretch[LayoutVertical]);
	 ZeroGlue (p->shrink[LayoutVertical]);
	 LayYYval.pval = p;
      }  break;
      case 13: { 
         LayYYval.gval = LayYYpvt[-0].gval; 
      }  break;
      case 14: { 
         ZeroGlue (LayYYval.gval); 
      }  break;
      case 15: { 
         LayYYval.gval = LayYYpvt[-0].gval;
      }  break;
      case 16: { 
         ZeroGlue (LayYYval.gval); 
      }  break;
      case 17: { 
         LayYYval.gval.order = LayYYpvt[-0].ival; 
         LayYYval.gval.expr = LayYYpvt[-1].eval; 
      }  break;
      case 18: { 
         LayYYval.gval.order = 0; 
         LayYYval.gval.expr = LayYYpvt[-0].eval; 
      }  break;
      case 19: { 
         LayYYval.gval.order = LayYYpvt[-0].ival; 
         LayYYval.gval.expr = 0; 
         LayYYval.gval.value = 1; 
      }  break;
      case 20: {
	 LayYYval.eval = New(ExprRec);
	 LayYYval.eval->type = Unary;
	 LayYYval.eval->u.unary.op = LayYYpvt[-1].oval;
	 LayYYval.eval->u.unary.down = LayYYpvt[-0].eval;
      }  break;
      case 21: { 
         LayYYval.eval = LayYYpvt[-0].eval; 
      }  break;
      case 23: {	
         LayYYval.eval = New(ExprRec);
	 LayYYval.eval->type = Width;
	 LayYYval.eval->u.width = LayYYpvt[-0].qval;
      }  break;
      case 24: {	
         LayYYval.eval = New(ExprRec);
	 LayYYval.eval->type = Height;
	 LayYYval.eval->u.height = LayYYpvt[-0].qval;
      }  break;
      case 25: { 
         LayYYval.eval = LayYYpvt[-1].eval; 
      }  break;
      case 26: {
	 LayYYval.eval = New(ExprRec);
	 LayYYval.eval->type = Unary;
	 LayYYval.eval->u.unary.op = LayYYpvt[-0].oval;
	 LayYYval.eval->u.unary.down = LayYYpvt[-1].eval;
      }  break;
      case 27: {	
         LayYYval.eval = New(ExprRec);
	 LayYYval.eval->type = Constant;
	 LayYYval.eval->u.constant = LayYYpvt[-0].ival;
      }  break;
      case 28: {	
         LayYYval.eval = New(ExprRec);
	 LayYYval.eval->type = Variable;
	 LayYYval.eval->u.variable = LayYYpvt[-0].qval;
      }  break;
      case 29: { 
         binary: ;
	    LayYYval.eval = New(ExprRec);
	    LayYYval.eval->type = Binary;
	    LayYYval.eval->u.binary.op = LayYYpvt[-1].oval;
	    LayYYval.eval->u.binary.left = LayYYpvt[-2].eval;
	    LayYYval.eval->u.binary.right = LayYYpvt[-0].eval;
      }  break;
      case 30: { 
         goto binary; 
      }  break;
      case 31: { 
         goto binary; 
      }  break;
      case 32: { 
         goto binary; 
      }  break;
      case 33: { 
         goto binary; 
      }  break;
      case 34: { 
         unary: ;
	    LayYYval.eval = New(ExprRec);
	    LayYYval.eval->type = Unary;
	    LayYYval.eval->u.unary.op = LayYYpvt[-1].oval;
	    LayYYval.eval->u.unary.down = LayYYpvt[-0].eval;
      }  break;
      case 35: { 
         LayYYval.eval = LayYYpvt[-0].eval; 
      }  break;
      case 37: {   
         LayYYval.lval = LayoutVertical; 
      }  break;
      case 38: {   
         LayYYval.lval = LayoutHorizontal; 
      }  break; 
   }

   goto LayYYstack;  /* stack new state and value */
}

/*############################################################################*/
/*############################################################################*/
/*    L A Y O U T . C                                                         */
/*############################################################################*/
/*############################################################################*/

#   ifdef MIN
#      undef MIN
#   endif
#   define MIN(_a_,_b_) ((_a_)<(_b_) ? (_a_):(_b_))
#   ifdef MAX
#      undef MAX
#   endif
#   define MAX(_a_,_b_) ((_a_)>(_b_) ? (_a_):(_b_))


#define offset(field) XtOffsetOf(LayoutRec, layout.field)
static XtResource resources[] = {
    {XtNlayout, XtCLayout, XtRLayout, sizeof (BoxPtr),
        offset(layout), XtRLayout, NULL },
    {XtNdebug, XtCBoolean, XtRBoolean, sizeof(Boolean),
         offset(debug), XtRImmediate, (XtPointer) FALSE},
    {XtNresizeWidth, XtCBoolean, XtRBoolean, sizeof(Boolean),
         offset(resize_width), XtRImmediate, (XtPointer) TRUE}, /* introduced in 11/94 ###jp### */
    {XtNresizeHeight, XtCBoolean, XtRBoolean, sizeof(Boolean),
         offset(resize_height), XtRImmediate, (XtPointer) TRUE}, /* introduced in 11/94 ###jp### */
    {XtNconditionedResize, XtCBoolean, XtRBoolean, sizeof(Boolean),
         offset(conditioned_resize), XtRImmediate, (XtPointer) TRUE}, /* introduced in 11/94 ###jp### */
    {XtNmaximumWidth, XtCMaximumWidth, XtRDimension, sizeof(Dimension),
         offset(maximum_width), XtRImmediate, (XtPointer)0}, /* introduced in 11/94 ###jp### */
    {XtNmaximumHeight, XtCMaximumHeight, XtRDimension, sizeof(Dimension),
         offset(maximum_height), XtRImmediate, (XtPointer)0}, /* introduced in 11/94 ###jp### */
    {XtNminimumWidth, XtCMinimumWidth, XtRDimension, sizeof(Dimension),
         offset(minimum_width), XtRImmediate, (XtPointer)0}, /* introduced in 11/94 ###jp### */
    {XtNminimumHeight, XtCMinimumHeight, XtRDimension, sizeof(Dimension),
         offset(minimum_height), XtRImmediate, (XtPointer)0}, /* introduced in 11/94 ###jp### */
};
#undef offset

#define offset(field) XtOffsetOf(LayoutConstraintsRec, layout.field)
static XtResource layoutConstraintResources[] = {
    {XtNresizable, XtCBoolean, XtRBoolean, sizeof(Boolean),
	offset(allow_resize), XtRImmediate, (XtPointer) TRUE},
};
#undef offset

static void ClassInitialize(), Initialize();
static void Resize();
static Boolean SetValues();
static XtGeometryResult GeometryManager();
static void ChangeManaged();
static void InsertChild();
static XtGeometryResult QueryGeometry ();
static void GetDesiredSize ();

static void LayoutLayout ();
static void LayoutGetNaturalSize ();
static void LayoutFreeLayout ();

#define SuperClass ((ConstraintWidgetClass)&constraintClassRec)

LayoutClassRec layoutClassRec = {
   {
/* core class fields */
    /* superclass         */   (WidgetClass) SuperClass,
    /* class name         */   "Layout",
    /* size               */   sizeof(LayoutRec),
    /* class_initialize   */   ClassInitialize,
    /* class_part init    */   NULL,
    /* class_inited       */   FALSE,
    /* initialize         */   Initialize,
    /* initialize_hook    */   NULL,
    /* realize            */   XtInheritRealize,
    /* actions            */   NULL,
    /* num_actions        */   0,
    /* resources          */   resources,
    /* resource_count     */   XtNumber(resources),
    /* xrm_class          */   NULLQUARK,
#ifdef VMS /*###jp###*/
    /* compress_motion    */   0,
    /* compress_exposure  */   0,
    /* compress_enterleave*/   0,
#else
    /* compress_motion    */   NULL,
    /* compress_exposure  */   NULL,
    /* compress_enterleave*/   NULL,
#endif
    /* visible_interest   */   FALSE,
    /* destroy            */   NULL,
    /* resize             */   Resize,
    /* expose             */   NULL,
    /* set_values         */   SetValues,
    /* set_values_hook    */   NULL,
    /* set_values_almost  */   XtInheritSetValuesAlmost,
    /* get_values_hook    */   NULL,
    /* accept_focus       */   NULL,
    /* version            */   XtVersion,
    /* callback_private   */   NULL,
    /* tm_table           */   NULL,
    /* query_geometry     */   QueryGeometry,
    /* display_accelerator*/   XtInheritDisplayAccelerator,
    /* extension          */   NULL
   }, {
/* composite class fields */
    /* geometry_manager   */   GeometryManager,
    /* change_managed     */   ChangeManaged,
    /* insert_child       */   InsertChild,
    /* delete_child       */   XtInheritDeleteChild,
    /* extension          */   NULL
   }, {
/* constraint class fields */
    /* subresources       */   layoutConstraintResources,
    /* subresource_count  */   XtNumber(layoutConstraintResources),
    /* constraint_size    */   sizeof(LayoutConstraintsRec),
    /* initialize         */   NULL,
    /* destroy            */   NULL,
    /* set_values         */   NULL,
    /* extension          */   NULL
   }
};

WidgetClass layoutWidgetClass = (WidgetClass) &layoutClassRec;

#define ForAllChildren(pw, childP) \
  for ( (childP) = (pw)->composite.children ; \
        (childP) < (pw)->composite.children + (pw)->composite.num_children ; \
        (childP)++ ) if (!XtIsManaged(*childP)) ; else

/************************************************************
 *
 * Semi-public routines. 
 *
 ************************************************************/

/*      Function Name: ClassInitialize
 *      Description: The Layout widgets class initialization proc.
 *      Arguments: none.
 *      Returns: none.
 */


/*ARGSUSED*/
static Boolean
CvtStringToLayout (dpy, args, num_args, from, to, converter_data)
    Display     *dpy;
    XrmValue    *args;
    Cardinal    *num_args;
    XrmValue    *from, *to;
    XtPointer   *converter_data;
{
    BEGINMESSAGE(CvtStringToLayout)
    LayYYsetsource ((char *) from->addr);
    LayYYsetdest ((BoxPtr *) to->addr);
    if (LayYYparse () == 0) {
       INFMESSAGE(successfull) ENDMESSAGE(CvtStringToLayout)
       return TRUE;
    }
    else {
       INFMESSAGE(failed) ENDMESSAGE(CvtStringToLayout)
       return FALSE;
    }
}

/*ARGSUSED*/
static void
DisposeLayout (app, to, data, args, num_args)
    XtAppContext    app;
    XrmValue        *to;
    XtPointer       data;
    XrmValuePtr     args;
    Cardinal        *num_args;
{
    BEGINMESSAGE(DisposeLayout)
    LayoutFreeLayout (* (LayoutPtr *) to->addr);
    ENDMESSAGE(DisposeLayout)
}     

static void 
ClassInitialize()
{
    BEGINMESSAGE(ClassInitialize)
    XtSetTypeConverter ( XtRString, XtRLayout, CvtStringToLayout,
                    (XtConvertArgList)NULL, (Cardinal)0, XtCacheNone, 
                    DisposeLayout );
    ENDMESSAGE(ClassInitialize)
}

/*ARGSUSED*/
static XtGeometryResult GeometryManager(child, request, reply)
    Widget              child;
    XtWidgetGeometry    *request, *reply;
{
    LayoutWidget    w = (LayoutWidget) XtParent(child);
    SubInfoPtr      p = SubInfo(child);
    int             bw;
    Bool            changed, bwChanged;

    BEGINMESSAGE(GeometryManager)
    INFSMESSAGE(received request from:,XtName(child))

    if (!(request->request_mode & (CWWidth | CWHeight | CWBorderWidth))) {
       INFMESSAGE(request not of interest) ENDMESSAGE(GeometryManager)
       return XtGeometryYes;
    }
    if (request->request_mode & XtCWQueryOnly) {
       /* query requests are not properly implemented ... ###jp### */
       INFMESSAGE(request is query only and will be denied) ENDMESSAGE(GeometryManager)
       return XtGeometryNo;
    }
    INFIIMESSAGE(current size of child:,child->core.width,child->core.height)
    if (!(p->allow_resize)) {
       INFMESSAGE(this child is not allowed to resize) ENDMESSAGE(GeometryManager)
       return XtGeometryNo;
    }
    INFMESSAGE(child may resize)

    bw = p->naturalBw;
    changed = FALSE;
    bwChanged = FALSE;
    if (request->request_mode & CWBorderWidth &&
        request->border_width != child->core.border_width) 
    {
        INFMESSAGE(request for new border width)
        p->naturalBw = bw;
        bw = request->border_width;
        changed = TRUE;
        bwChanged = TRUE;
    }
    if (bwChanged || request->request_mode & CWWidth &&
        request->width != child->core.width)
    {
        INFMESSAGE(request for new width)
        p->naturalSize[LayoutHorizontal] = request->width + bw * 2;
        changed = TRUE;
    }
    if (bwChanged || request->request_mode & CWHeight &&
        request->height != child->core.height)
    {
        INFMESSAGE(request for new height)
        p->naturalSize[LayoutVertical] = request->height + bw * 2;
        changed = TRUE;
    }

    if (changed) {
       LayoutLayout(w,TRUE);
       INFIIMESSAGE(new size of child:,child->core.width,child->core.height)
       ENDMESSAGE(GeometryManager)
       return XtGeometryDone;
    } else {
       ENDMESSAGE(GeometryManager)
       return XtGeometryYes;
    }
}

/* ARGSUSED */
static void Initialize(request, new, args, num_args)
Widget request, new;
ArgList args;
Cardinal *num_args;
{
   LayoutWidget l= (LayoutWidget) new;
   Dimension prefwidth,prefheight;
   BEGINMESSAGE(Initialize)
   if ( (l->core.width  <= l->layout.minimum_width)  ||
        (l->core.height <= l->layout.minimum_height) )  {
      LayoutGetNaturalSize (l, &prefwidth, &prefheight);
      prefwidth = MAX(l->layout.minimum_width,prefwidth);
      if (prefwidth<1)  prefwidth =1;
      prefheight= MAX(l->layout.minimum_height,prefheight);
      if (prefheight<1) prefheight=1;
      if (l->core.width  <= l->layout.minimum_width)  l->core.width  = prefwidth;
      if (l->core.height <= l->layout.minimum_height) l->core.height = prefheight;
  }
  INFIIMESSAGE(size:,l->core.width,l->core.height)
  ENDMESSAGE(Initialize)
}

static void ChangeManaged(gw)
   Widget gw;
{
    LayoutWidget        w = (LayoutWidget) gw;
    Widget              *children;

    BEGINMESSAGE(ChangeManaged)
    ForAllChildren (w, children) GetDesiredSize (*children);
    LayoutLayout ((LayoutWidget)w,TRUE);
    ENDMESSAGE(ChangeManaged)
}

static void
GetDesiredSize (child)
    Widget  child;
{
    XtWidgetGeometry    desired;
    SubInfoPtr          p;
    
    XtQueryGeometry (child, (XtWidgetGeometry *) NULL, &desired);
    p = SubInfo (child);
    p->naturalBw = desired.border_width;
    p->naturalSize[LayoutHorizontal] = desired.width + desired.border_width * 2;
    p->naturalSize[LayoutVertical] = desired.height + desired.border_width * 2;
    INFSMESSAGE1(GetDesiredSize for:,XtName(child))
}

static void InsertChild (child)
    Widget  child;
{
    (*SuperClass->composite_class.insert_child) (child);
    GetDesiredSize (child);
}

static void
Resize(gw)
    Widget gw;
{
    BEGINMESSAGE(Resize)
    LayoutLayout ((LayoutWidget) gw, FALSE);
    ENDMESSAGE(Resize)
}

/* ARGSUSED */
static Boolean 
SetValues(gold, greq, gnew, args, num_args)
    Widget gold, greq, gnew;
    ArgList args;
    Cardinal *num_args;
{
    LayoutWidget    old = (LayoutWidget) gold,
                    new = (LayoutWidget) gnew;

    BEGINMESSAGE(SetValues)
    if (old->layout.layout != new->layout.layout) {
       LayoutLayout(new,TRUE);
    }
    ENDMESSAGE(SetValues)
    return FALSE;

} /* SetValues */

static XtGeometryResult
QueryGeometry (gw, request, preferred_return)
    Widget              gw;
    XtWidgetGeometry    *request, *preferred_return;
{
    LayoutWidget        w = (LayoutWidget) gw;
    XtGeometryResult    result;
    XtWidgetGeometry    preferred_size;

    BEGINMESSAGE(QueryGeometry)
    if (request && !(request->request_mode & CWWidth|CWHeight)) {
       INFMESSAGE(return XtGeometryYes)
       ENDMESSAGE(QueryGeometry)
       return XtGeometryYes;
    }
    LayoutGetNaturalSize (w, &preferred_size.width, &preferred_size.height);
    preferred_return->request_mode = 0;
    result = XtGeometryYes;
    if (!request) {
        preferred_return->width = preferred_size.width;
        preferred_return->height= preferred_size.height;
        if (preferred_size.width != w->core.width) {
            preferred_return->request_mode |= CWWidth;
            result = XtGeometryAlmost;
        }
        if (preferred_size.height != w->core.height) {
            preferred_return->request_mode |= CWHeight;
            result = XtGeometryAlmost;
        }
    } else {
        if (request->request_mode & CWWidth) {
            if (preferred_size.width > request->width)
            {
                if (preferred_size.width == w->core.width)
                    result = XtGeometryNo;
                else if (result != XtGeometryNo) {
                    result = XtGeometryAlmost;
                    preferred_return->request_mode |= CWWidth;
                    preferred_return->width = preferred_size.width;
                }
            }
        }
        if (request->request_mode & CWHeight) {
            if (preferred_size.height > request->height)
            {
                if (preferred_size.height == w->core.height)
                    result = XtGeometryNo;
                else if (result != XtGeometryNo) {
                    result = XtGeometryAlmost;
                    preferred_return->request_mode |= CWHeight;
                    preferred_return->height = preferred_size.height;
                }
            }
        }
    }
    ENDMESSAGE(QueryGeometry)
    return result;
}

/*
 * Layout section.  Exports LayoutGetNaturalSize and
 * LayoutLayout to above section
 */

#ifdef VMS
static
#endif
ExprPtr
LookupVariable (child, quark)
    BoxPtr      child;
    XrmQuark    quark;
{
    BoxPtr      parent, box;

    while (parent = child->parent)
    {
        for (box = parent->u.box.firstChild; box != child; box = box->nextSibling) {
            if (box->type == VariableBox && box->u.variable.quark == quark) {
               return box->u.variable.expr;
            }
        }
        child = parent;
    }
   return 0;
}

                
static double
Evaluate (l, box, expr, natural)
    LayoutWidget    l;
    BoxPtr          box;
    ExprPtr         expr;
    double          natural;
{
    double      left, right, down;
    Widget      widget;
    SubInfoPtr  info;

    switch (expr->type) {
       case Constant:
                return expr->u.constant;
       case Binary:
                left = Evaluate (l, box, expr->u.binary.left, natural);
                right = Evaluate (l, box, expr->u.binary.right, natural);
                switch (expr->u.binary.op) {
                   case Plus:
                      return left + right;
                   case Minus:
                      return left - right;
                   case Times:
                      return left * right;
                   case Divide:
                      return left / right;
                   case Percent:
                      return right * left / 100.0;
                 }
        case Unary:
                 down = Evaluate (l, box, expr->u.unary.down, natural);
                 switch (expr->u.unary.op) {
                    case Percent:
                      return natural * down / 100.0;
                    case Minus:
                      return -down;
                 }
        case Width:
                 widget = QuarkToWidget (l, expr->u.width);
                 if (!widget) return 0;
                 info = SubInfo (widget);
                 return info->naturalSize[LayoutHorizontal];
        case Height:
                 widget = QuarkToWidget (l, expr->u.height);
                 if (!widget) return 0;
                 info = SubInfo (widget);
                 return info->naturalSize[LayoutVertical];
        case Variable:
                 expr = LookupVariable (box, expr->u.variable);
                 if (!expr) {
                    char    buf[256];
                    (void) sprintf (buf, "Layout: undefined variable %s\n",
                                    XrmQuarkToString (expr->u.variable));
                    XtError (buf);
                    return 0.0;
                 }
                 return Evaluate (l, box, expr, natural);
    }
}

static void
DisposeExpr (expr)
    ExprPtr expr;
{
    if (!expr) return;
    switch (expr->type) {
       case Constant:
               break;
       case Binary:
               DisposeExpr (expr->u.binary.left);
               DisposeExpr (expr->u.binary.right);
               break;
       case Unary:
               DisposeExpr (expr->u.unary.down);
               break;
       case Width:
               break;
       case Height:
               break;
    }
    Dispose (expr);
}

#define CheckGlue(l, box, glue, n) { \
    if (glue.expr) \
        glue.value = Evaluate (l, box, glue.expr, n); \
    if (glue.order == 0 && glue.value == 0) \
        glue.order = -1; \
    else if (glue.order == -1 && glue.value != 0) \
        glue.order = 0; \
}

#define DoStretch(l, box, dir) \
    CheckGlue (l, box, box->params.stretch[dir], (double) box->natural[dir]);
        
#define DoShrink(l, box, dir) \
    CheckGlue (l, box, box->params.shrink[dir], (double) box->natural[dir])

/* compute the natural sizes of a box */
static void
ComputeNaturalSizes (l, box, dir)
    LayoutWidget    l;
    BoxPtr          box;
    LayoutDirection dir;
{
    BoxPtr      child;
    Widget      w;
    SubInfoPtr  info;
    int         minStretchOrder, minShrinkOrder;
    LayoutDirection thisDir;
    
    switch (box->type) {
       case VariableBox:
               box->natural[LayoutHorizontal] = 0;
               box->natural[LayoutVertical] = 0;
               break;
       case WidgetBox:
               w = box->u.widget.widget = QuarkToWidget (l, box->u.widget.quark);
               if (!w) {
                  box->natural[LayoutHorizontal] = 0;
                  box->natural[LayoutVertical] = 0;
               }
               else {
                  info = SubInfo (w);
                  box->natural[LayoutHorizontal] = info->naturalSize[LayoutHorizontal];
                  box->natural[LayoutVertical] = info->naturalSize[LayoutVertical];
               }
               DoStretch (l, box, dir);
               DoShrink (l, box, dir);
               DoStretch (l, box, !dir);
               DoShrink (l, box, !dir);
               break;
       case GlueBox:
               box->natural[dir] = Evaluate (l, box, box->u.glue.expr, 0.0);
               box->natural[!dir] = 0;
               DoStretch (l, box, dir);
               DoShrink (l, box, dir);
               break;
       case BoxBox:
               thisDir = box->u.box.dir;
               box->natural[0] = 0;
               box->natural[1] = 0;
               minStretchOrder = 100000;
               minShrinkOrder = 100000;
               ZeroGlue (box->params.shrink[thisDir]);
               ZeroGlue (box->params.stretch[thisDir]);
               box->params.shrink[!thisDir].order = 100000;
               box->params.stretch[!thisDir].order = 100000;
               for (child = box->u.box.firstChild; child; child = child->nextSibling) {
                   ComputeNaturalSizes (l, child, thisDir);
                  /*
                   * along box axis:
                   *  normal size += child normal size
                   *  shrink += child shrink
                   *  stretch += child stretch
                  */

                   box->natural[thisDir] += child->natural[thisDir];
                   AddGlue (box->params.shrink[thisDir],
                        box->params.shrink[thisDir],
                        child->params.shrink[thisDir]);
                   AddGlue (box->params.stretch[thisDir],
                        box->params.stretch[thisDir],
                        child->params.stretch[thisDir]);
                  /*
                   * normal to box axis:
                   *  normal size = maximum child normal size of minimum shrink order
                   *  shrink = difference between normal size and minimum shrink
                   *  stretch = minimum child stretch
                  */
                   if (box->natural[!thisDir] >= child->natural[!thisDir]) {
                      if (child->params.stretch[!thisDir].order < minShrinkOrder) {
                         box->natural[!thisDir] = child->natural[!thisDir];
                         minStretchOrder = child->params.stretch[!thisDir].order;
                         if (child->params.shrink[!thisDir].order < minShrinkOrder)
                            minShrinkOrder = child->params.shrink[!thisDir].order;
                      }
                   }
                   else {
                      if (child->params.shrink[!thisDir].order <= minStretchOrder) {
                         box->natural[!thisDir] = child->natural[!thisDir];
                         minShrinkOrder = child->params.shrink[!thisDir].order;
                         if (child->params.stretch[!thisDir].order < minStretchOrder)
                            minStretchOrder = child->params.stretch[!thisDir].order;
                      }
                   }
                   MinGlue (box->params.stretch[!thisDir],box->params.stretch[!thisDir],
                            child->params.stretch[!thisDir]);
                   MinGlue (box->params.shrink[!thisDir],box->params.shrink[!thisDir],
                            child->params.shrink[!thisDir]);
                }
                if (box->params.shrink[!thisDir].order <= 0) {
                   int      minSize;
                   int      largestMinSize;
            
                   largestMinSize = 0;
                   for (child = box->u.box.firstChild; child; child = child->nextSibling) {
                       if (child->params.shrink[!thisDir].order <= 0) {
                          minSize = child->natural[!thisDir] -
                          child->params.shrink[!thisDir].value;
                          if (minSize > largestMinSize) largestMinSize = minSize;
                       }
                   }
                   box->params.shrink[!thisDir].value = box->natural[!thisDir] - largestMinSize;
                   if (box->params.shrink[!thisDir].value == 0) box->params.shrink[!thisDir].order = -1;
                   else box->params.shrink[!thisDir].order = 0;
                }
    }
}

/* given the boxs geometry, set the geometry of the pieces */

#define GluePart(a,b,dist)      ((a) ? ((int) (((a) * (dist)) / (b) + \
                                        ((dist >= 0) ? 0.5 : -0.5))) : 0)

static Bool
ComputeSizes (box)
    BoxPtr          box;
{
    LayoutDirection dir;
    BoxPtr          child;
    GlueRec         stretch;
    GlueRec         shrink;
    GlueRec         totalGlue[2];
    double          remainingGlue;
    GluePtr         glue;
    int             size;
    int             totalSizes;
    int             totalChange[2];
    int             change;
    int             remainingChange;
    Bool            shrinking;
    Bool            happy;
    int             i;
    int             maxGlue;

    dir = box->u.box.dir;
    size = box->size[dir];
    
    stretch = box->params.stretch[dir];
    shrink = box->params.shrink[dir];
    
    /* pick the correct adjustment parameters based on the change direction */
    
    totalChange[0] = size - box->natural[dir];

    shrinking = totalChange[0] < 0;
    
    totalChange[1] = 0;
    totalGlue[1].order = 100000;
    totalGlue[1].value = 0;
    maxGlue = 1;
    if (shrinking) 
    {
        totalGlue[0] = shrink;
        /* for first-order infinites, shrink it to zero and then
         * shrink the zero-orders
         */
        if (shrink.order == 1) {
            totalSizes = 0;
            remainingGlue = 0;
            for (child = box->u.box.firstChild; 
                 child; 
                 child = child->nextSibling) 
            {
                switch (child->params.shrink[dir].order) {
                case 0:
                    remainingGlue += child->params.shrink[dir].value;
                    break;
                case 1:
                    totalSizes += child->natural[dir];
                    break;
                }
            }
            if (totalSizes < -totalChange[0])
            {
                totalGlue[1] = shrink;
                totalGlue[0].order = 0;
                totalGlue[0].value = remainingGlue;
                totalChange[1] = -totalSizes;
                totalChange[0] = totalChange[0] - totalChange[1];
                maxGlue = 2;
            }
        }
        if (totalGlue[0].order <= 0 && 
            totalChange[0] > totalGlue[0].value)
        {
            totalChange[0] = totalGlue[0].value;
        }
    }
    else
        totalGlue[0] = stretch;
        
    /* adjust each box */
    totalSizes = 0;
    remainingGlue = totalGlue[0].value + totalGlue[1].value;
    remainingChange = totalChange[0] + totalChange[1];
    happy = True;
    for (child = box->u.box.firstChild; child; child = child->nextSibling) 
    {
        if (shrinking)
            glue = &child->params.shrink[dir];
        else
            glue = &child->params.stretch[dir];
    
        child->size[dir] = child->natural[dir];
        for (i = 0; i < maxGlue; i++)
        {
            if (glue->order == totalGlue[i].order)
            {
                remainingGlue -= glue->value;
                if (remainingGlue <= 0)
                    change = remainingChange;
                else
                    change = GluePart (glue->value, 
                                       totalGlue[i].value, 
                                       totalChange[i]);
                child->size[dir] += change;
                remainingChange -= change;
            }
        }
        child->size[!dir] = box->size[!dir];
        totalSizes += child->size[dir];
        if (child->type == BoxBox)
            if (!ComputeSizes (child))
                happy = False;
    }
    
    return totalSizes == box->size[dir] && happy;
}

static void
SetSizes (box, x, y)
    BoxPtr      box;
    Position    x, y;
{
    BoxPtr      child;
    int         width, height;
    int         bw;
    Widget      w;
    SubInfoPtr  info;

    BEGINMESSAGE1(SetSizes)
    switch (box->type) {
    case WidgetBox:
        w = box->u.widget.widget;
        if (w)
        {
            info = (SubInfoPtr) w->core.constraints;
            width = box->size[LayoutHorizontal];
            height = box->size[LayoutVertical];
            bw = info->naturalBw;
            width = width - bw*2;
            height = height - bw*2;
            /* Widgets which grow too small are placed off screen */
            if (width <= 0 || height <= 0) 
            {
                width = 1;
                height = 1;
                bw = 0;
                x = -1;
                y = -1;
            }
            INFSMESSAGE1(configuring widget:,XtName(w))
            XtConfigureWidget (w, x, y, 
                              (Dimension)width, (Dimension)height, 
                              (Dimension)bw);
        }
        break;
    case GlueBox:
        break;
    case BoxBox:
        for (child = box->u.box.firstChild; child; child = child->nextSibling) 
        {
            SetSizes (child, x, y);
            if (box->u.box.dir == LayoutHorizontal)
                x += child->size[LayoutHorizontal];
            else
                y += child->size[LayoutVertical];
        }
        break;
    }
    ENDMESSAGE1(SetSizes)
}

static void
LayoutFreeLayout (box)
    BoxPtr  box;
{
    BoxPtr  child, next;
    
    BEGINMESSAGE(LayoutFreeLayout)
    switch (box->type) {
    case BoxBox:
        for (child = box->u.box.firstChild; child; child = next)
        {
            next = child->nextSibling;
            LayoutFreeLayout (child);
        }
        break;
    case GlueBox:
        DisposeExpr (box->u.glue.expr);
        break;
    }
    DisposeExpr (box->params.stretch[LayoutHorizontal].expr);
    DisposeExpr (box->params.shrink[LayoutHorizontal].expr);
    DisposeExpr (box->params.stretch[LayoutVertical].expr);
    DisposeExpr (box->params.shrink[LayoutVertical].expr);
    Dispose (box);
   ENDMESSAGE(LayoutFreeLayout)
}


static void
LayoutGetNaturalSize (l, widthp, heightp)
    LayoutWidget    l;
    Dimension       *widthp, *heightp;   
{
    BoxPtr              box;

    BEGINMESSAGE(LayoutGetNaturalSize)
    box = l->layout.layout;
    if (box) {
#      ifdef MESSAGES
          {
             Dimension w,h;
             w=box->natural[LayoutHorizontal];
             h=box->natural[LayoutVertical];
             INFIIMESSAGE(before calculation:,w,h)
          }
#      endif
       ComputeNaturalSizes (l, box, LayoutHorizontal);
       *widthp = box->natural[LayoutHorizontal];
       *heightp = box->natural[LayoutVertical];
    } else {
       INFMESSAGE(no layout available)
       *widthp = 0;
       *heightp = 0;
    }
    INFIIMESSAGE(natural size:,*widthp,*heightp)
    ENDMESSAGE(LayoutGetNaturalSize)
}

static void
LayoutLayout (l, attemptResize)
    LayoutWidget    l;
    Bool            attemptResize;
{                                   
    BoxPtr              box = l->layout.layout;
    Dimension           width, height;
    Dimension           pwidth, pheight;
    Bool		happy=False;

    BEGINMESSAGE(LayoutLayout)

    if (!box) { INFMESSAGE(no layout specified) ENDMESSAGE(LayoutLayout) return; }

    if (l->layout.maximum_width  > 0) l->layout.maximum_width  = MAX(l->layout.maximum_width,l->core.width);
    if (l->layout.maximum_height > 0) l->layout.maximum_height = MAX(l->layout.maximum_height,l->core.height);
    if (l->layout.minimum_width  > 0) l->layout.minimum_width  = MIN(l->layout.minimum_width,l->core.width);
    if (l->layout.minimum_height > 0) l->layout.minimum_height = MIN(l->layout.minimum_height,l->core.height);

#   ifdef MESSAGES
    { char *name=XtName((Widget)l);
      Dimension w,h;
      INFSMESSAGE(layouting widget:,name)
      w=l->core.width; h=l->core.height;
      INFIIMESSAGE(current size:,w,h)
      w=l->layout.maximum_width; h=l->layout.maximum_height;
      INFIIMESSAGE(new maximum size:,w,h)
      w=l->layout.minimum_width; h=l->layout.minimum_height;
      INFIIMESSAGE(new minimum size:,w,h)
      if (l->layout.conditioned_resize) { INFMESSAGE(conditioned resize active) }
      else                              { INFMESSAGE(conditioned resize disabled) }
      if (l->layout.resize_width)       { INFMESSAGE(width change is allowed) }
      else                              { INFMESSAGE(width change is not allowed) }
      if (l->layout.resize_height)      { INFMESSAGE(height change is allowed) }
      else                              { INFMESSAGE(height change is not allowed) }
    }
#   endif

    LayoutGetNaturalSize (l, &pwidth, &pheight);

    box->size[LayoutHorizontal] = l->core.width; 
    box->size[LayoutVertical]   = l->core.height;
    if (l->layout.conditioned_resize) happy= ComputeSizes(box);
    if (happy) {
       INFMESSAGE(happy with current size)
       attemptResize=False;        
    }
    if (!l->layout.resize_width && !l->layout.resize_height) attemptResize=False;

#   ifdef MESSAGES
       if (attemptResize) { INFMESSAGE(may attempt to resize) }
       else               { INFMESSAGE(will not attempt to resize) }
#   endif

    if (attemptResize) {
       if (!(l->layout.resize_width))  {
          pwidth = l->core.width;
       } else {
          if (l->layout.maximum_width > 0)  pwidth  = MIN(pwidth,l->layout.maximum_width);
          if (l->layout.minimum_width > 0)  pwidth  = MAX(pwidth,l->layout.minimum_width);
       }
       if (!(l->layout.resize_height)) {
          pheight = l->core.height;
       } else {
          if (l->layout.maximum_height > 0) pheight = MIN(pheight,l->layout.maximum_height);
          if (l->layout.minimum_height > 0) pheight = MAX(pheight,l->layout.minimum_height);
       }
       if ((pwidth == l->core.width) && (pheight == l->core.height)) {
          INFMESSAGE(resize attempt is unnecessary)
          attemptResize=False;
       }
    }

    if (attemptResize) {
       XtGeometryResult result; 
       INFIIMESSAGE(proposing resize:,pwidth,pheight)
       result = XtMakeResizeRequest ((Widget) l,pwidth,pheight,&width, &height);
       switch (result) {
           case XtGeometryYes:
              INFMESSAGE(XtGeometryYes)
              break;
           case XtGeometryAlmost:
              INFIIMESSAGE(XtGeometryAlmost:,width,height)
              result = XtMakeResizeRequest ((Widget) l,width, height,&width, &height);
              if (result!=XtGeometryYes) 
                 fprintf(stderr,"Layout widget: Warning, parent denied to set the size he proposed.");
              break;
           case XtGeometryNo:
              INFMESSAGE(XtGeometryNo)
              break;
       }
       box->size[LayoutHorizontal] = l->core.width;
       box->size[LayoutVertical]   = l->core.height;
    }

    if (!happy) ComputeSizes(box);
    if (l->layout.debug) { PrintBox (box, 0); fflush (stdout); }
    INFMESSAGE(adjusting children)
    SetSizes (box,0,0);

#   ifdef MESSAGES
    {
      Dimension w,h;
      w=l->core.width; h=l->core.height; INFIIMESSAGE(new size:,w,h)
    }
#   endif
    ENDMESSAGE(LayoutLayout)
}

/*###################### Public Routines ################################*/
                                         /* ###jp### 1/94 */

void
#if NeedFunctionPrototypes
LayoutWidgetGetNaturalSize(
    LayoutWidget    l,
    Dimension       *wp,
    Dimension       *hp
)
#else
LayoutWidgetGetNaturalSize(l, wp, hp)
    LayoutWidget    l;
    Dimension       *wp, *hp;
#endif
{
  BEGINMESSAGE(LayoutWidgetGetNaturalSize)
/*
  *wp = (l->layout.layout)->natural[LayoutHorizontal];
  *hp = (l->layout.layout)->natural[LayoutVertical];
*/
  LayoutGetNaturalSize(l, wp, hp);
  INFIIMESSAGE(natural:,*wp,*hp)
  ENDMESSAGE(LayoutWidgetGetNaturalSize)
}

void
#if NeedFunctionPrototypes
LayoutWidgetAllowResize(
    LayoutWidget    l,
#   if NeedWidePrototypes
       int wflag,
       int hflag
#   else
       Boolean wflag,
       Boolean hflag
#   endif
)
#else
LayoutWidgetAllowResize(l,wflag,hflag)
    LayoutWidget    l;
    Boolean wflag,hflag;
#endif
{
  BEGINMESSAGE(LayoutWidgetAllowResize)
  l->layout.resize_width  = wflag;
  l->layout.resize_height = hflag;
# ifdef MESSAGES
  if (wflag) {INFMESSAGE(horizontal resizing is enabled)} 
  else       {INFMESSAGE(horizontal resizing is disabled)}
  if (hflag) {INFMESSAGE(vertical resizing is enabled)} 
  else       {INFMESSAGE(vertical resizing is disabled)}
# endif
  ENDMESSAGE(LayoutWidgetAllowResize)
}

void
#if NeedFunctionPrototypes
LayoutWidgetSetSizeBounds(
    LayoutWidget  l,
    Dimension *minw_p,
    Dimension *minh_p,
    Dimension *maxw_p,
    Dimension *maxh_p
)
#else
LayoutWidgetSetSizeBounds(l,minw_p,minh_p,maxw_p,maxh_p)
    LayoutWidget  l;
    Dimension *minw_p;
    Dimension *minh_p;
    Dimension *maxw_p;
    Dimension *maxh_p;
#endif
{
  BEGINMESSAGE(LayoutWidgetSetSizeBounds)
  if (minw_p) l->layout.minimum_width  = *minw_p;
  if (minh_p) l->layout.minimum_height = *minh_p;
  if (maxw_p) l->layout.maximum_width  = *maxw_p;
  if (maxh_p) l->layout.maximum_height = *maxh_p;
  ENDMESSAGE(LayoutWidgetSetSizeBounds)
}

void
#if NeedFunctionPrototypes
LayoutWidgetGetSizeBounds(
    LayoutWidget  l,
    Dimension *minw_p,
    Dimension *minh_p,
    Dimension *maxw_p,
    Dimension *maxh_p
)
#else
LayoutWidgetGetSizeBounds(l,minw_p,minh_p,maxw_p,maxh_p)
    LayoutWidget  l;
    Dimension *minw_p;
    Dimension *minh_p;
    Dimension *maxw_p;
    Dimension *maxh_p;
#endif
{
  BEGINMESSAGE(LayoutWidgetGetSizeBounds)
  if (minw_p) *minw_p = l->layout.minimum_width ;
  if (minh_p) *minh_p = l->layout.minimum_height;
  if (maxw_p) *maxw_p = l->layout.maximum_width ;
  if (maxh_p) *maxh_p = l->layout.maximum_height;
  ENDMESSAGE(LayoutWidgetGetSizeBounds)
}

