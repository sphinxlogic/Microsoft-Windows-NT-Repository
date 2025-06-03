/* pstops.c
 * AJCD 27/1/91
 * rearrange pages in conforming PS file for printing in signatures
 *
 * Usage:
 *       pstops [-q] [-b] [-w<dim>] [-h<dim>] <pagespecs> [infile [outfile]]
 */

#include "psutil.h"

void usage()
{
   fprintf(stderr, "Usage: %s [-q] [-b] [-w<dim>] [-h<dim] <pagespecs> [infile [outfile]]\n",
	   prog);
   fflush(stderr);
   exit(1);
}

void specusage()
{
   fprintf(stderr, "%s: page specification error:\n", prog);
   fprintf(stderr, "  <pagespecs> = [modulo:]<spec>\n");
   fprintf(stderr, "  <spec>      = [-]pageno[@scale][L|R|U][(xoff,yoff)][,spec|+spec]\n");
   fprintf(stderr, "                modulo>=1, 0<=pageno<modulo\n");
   fflush(stderr);
   exit(1);
}

static int modulo = 1;
static int pagesperspec = 1;
static double width = -1.0;
static double height = -1.0;

/* pagespec flags */
#define ADD_NEXT (0x01)
#define ROTATE   (0x02)
#define SCALE    (0x04)
#define OFFSET   (0x08)
#define GSAVE    (ROTATE|SCALE|OFFSET)

struct pagespec {
   int reversed, pageno, flags, rotate;
   double xoff, yoff, scale;
   struct pagespec *next;
};

struct pagespec *newspec()
{
   struct pagespec *temp = (struct pagespec *)malloc(sizeof(struct pagespec));
   temp->reversed = temp->pageno = temp->flags = temp->rotate = 0;
   temp->scale = 1.0;
   temp->xoff = temp->yoff = 0.0;
   temp->next = NULL;
   return (temp);
}

int parseint(sp)
     char **sp;
{
   char *s;
   int n = 0;

   for (s = *sp; isdigit(*s); s++)
      n = n*10 + (*s-'0');
   if (*sp == s) specusage();
   *sp = s;
   return (n);
}

double parsedouble(sp)
     char **sp;
{
   int n = 0, neg = 1;
   char *s = *sp;
   int d = 0, frac = 1;

   if (*s == '-') {
      neg = -1;
      *sp = ++s;
   }
   for (;isdigit(*s); s++)
      n = n*10 + (*s-'0');
   if (*s == '.') {
      *sp = ++s;
      for (; isdigit(*s); s++) {
	 d = d*10 + (*s-'0');
	 frac *= 10;
      }
   }
   if (*sp == s) specusage();
   *sp = s;
   return (neg*((double)n+(double)d/frac));
}

double parsedimen(sp)
     char **sp;
{
   double num = parsedouble(sp);
   char *s = *sp;

   if (strncmp(s, "pt", 2) == 0) {
      s += 2;
   } else if (strncmp(s, "in", 2) == 0) {
      num *= 72.0;
      s += 2;
   } else if (strncmp(s, "cm", 2) == 0) {
      num *= 28.346456692913385211;
      s += 2;
   } else if (*s == 'w') {
      if (width < 0.0) {
	 fprintf(stderr, "%s: width not initialised\n", prog);
	 fflush(stderr);
	 exit(1);
      }
      num *= width;
      s++;
   } else if (*s == 'h') {
      if (height < 0.0) {
	 fprintf(stderr, "%s: height not initialised\n", prog);
	 fflush(stderr);
	 exit(1);
      }
      num *= height;
      s++;
   }
   *sp = s;
   return (num);
}

struct pagespec *parsespecs(str)
     char *str;
{
   char *t;
   struct pagespec *head, *tail;
   int other = 0;
   int num = -1;

   head = tail = newspec();
   while (*str) {
      if (isdigit(*str)) {
	 num = parseint(&str);
      } else {
	 switch (*str++) {
	 case ':':
	    if (other || head != tail || num < 1) specusage();
	    modulo = num;
	    num = -1;
	    break;
	 case '-':
	    tail->reversed = !tail->reversed;
	    break;
	 case '@':
	    if (num < 0) specusage();
	    tail->scale *= parsedouble(&str);
	    tail->flags |= SCALE;
	    break;
	 case 'l': case 'L':
	    tail->rotate += 90;
	    tail->flags |= ROTATE;
	    break;
	 case 'r': case 'R':
	    tail->rotate -= 90;
	    tail->flags |= ROTATE;
	    break;
	 case 'u': case 'U':
	    tail->rotate += 180;
	    tail->flags |= ROTATE;
	    break;
	 case '(':
	    tail->xoff += parsedimen(&str);
	    if (*str++ != ',') specusage();
	    tail->yoff += parsedimen(&str);
	    if (*str++ != ')') specusage();
	    tail->flags |= OFFSET;
	    break;
	 case '+':
	    tail->flags |= ADD_NEXT;
	 case ',':
	    if (num < 0 || num >= modulo) specusage();
	    if ((tail->flags & ADD_NEXT) == 0)
	       pagesperspec++;
	    tail->pageno = num;
	    tail->next = newspec();
	    tail = tail->next;
	    num = -1;
	    break;
	 default:
	    specusage();
	 }
	 other = 1;
      }
   }
   if (num >= modulo)
      specusage();
   else if (num >= 0)
      tail->pageno = num;
   return (head);
}

double singledimen(str)
     char *str;
{
   double num = parsedimen(&str);
   if (*str) usage();
   return (num);
}


main(argc, argv)
     int argc;
     char *argv[];
{
   int thispg, maxpage;
   int pageindex = 0;
   struct pagespec *specs = NULL;
   int nobinding = 0;

   infile = stdin;
   outfile = stdout;
   verbose = 1;
   for (prog = *argv++; --argc; argv++) {
      if (argv[0][0] == '-') {
	 switch (argv[0][1]) {
	 case 'q':
	    verbose = 0;
	    break;
	 case 'b':
	    nobinding = 1;
	    break;
	 case 'w':
	    width = singledimen(*argv+2);
	    break;
	 case 'h':
	    height = singledimen(*argv+2);
	    break;
	 default:
	    if (specs == NULL)
	       specs = parsespecs(*argv);
	    else
	       usage();
	 }
      } else if (specs == NULL)
	 specs = parsespecs(*argv);
      else if (infile == stdin) {
	 if ((infile = fopen(*argv, "r")) == NULL) {
	    fprintf(stderr, "%s: can't open input file %s\n", prog, *argv);
	    fflush(stderr);
	    exit(1);
	 }
      } else if (outfile == stdout) {
	 if ((outfile = fopen(*argv, "w")) == NULL) {
	    fprintf(stderr, "%s: can't open output file %s\n", prog, *argv);
	    fflush(stderr);
	    exit(1);
	 }
      } else usage();
   }
   if (specs == NULL)
      usage();
   if ((infile=seekable(infile))==NULL) {
      fprintf(stderr, "%s: can't seek input\n", prog);
      fflush(stderr);
      exit(1);
   }
   scanpages();

   maxpage = ((pages+modulo-1)/modulo)*modulo;

   /* rearrange pages: doesn't cope properly with:
    * initmatrix, initgraphics, defaultmatrix, grestoreall, initclip */
   writeheader((maxpage/modulo)*pagesperspec);
   writestring("%%BeginProcSet: pstops 1 0\n");
   writestring("[/showpage/erasepage/copypage]{dup where{pop dup load\n");
   writestring(" type/operatortype eq{1 array cvx dup 0 3 index cvx put\n");
   writestring(" bind def}{pop}ifelse}{pop}ifelse}forall\n");
   writestring("[/letter/legal/executivepage/a4/a4small/b5/com10envelope\n");
   writestring(" /monarchenvelope/c5envelope/dlenvelope/lettersmall/note\n");
   writestring(" /folio/quarto/a5]{dup where{dup wcheck{exch{}put}\n");
   writestring(" {pop{}def}ifelse}{pop}ifelse}forall\n");
   writestring("/lcvx{dup load dup type dup/operatortype eq{pop exch pop}\n");
   writestring(" {/arraytype eq{dup xcheck{exch pop aload pop}\n");
   writestring(" {pop cvx}ifelse}{pop cvx}ifelse}ifelse}bind def\n");
   writestring("/pstopsmatrix matrix currentmatrix def\n");
   writestring("/defaultmatrix{pstopsmatrix exch copy}bind def\n");
   writestring("/initmatrix{matrix defaultmatrix setmatrix}bind def\n");
   writestring("/pathtoproc{[{currentpoint}stopped{$error/newerror false\n");
   writestring(" put{newpath}}{/newpath cvx 3 1 roll/moveto cvx 4 array\n");
   writestring(" astore cvx}ifelse]{[/newpath cvx{/moveto cvx}{/lineto cvx}\n");
   writestring(" {/curveto cvx}{/closepath cvx}pathforall]cvx exch pop}\n");
   writestring(" stopped{$error/errorname get/invalidaccess eq{cleartomark\n");
   writestring(" $error/newerror false put cvx exec}{stop}ifelse}if}def\n");
   if (width > 0.0 && height > 0.0) {
      char buffer[BUFSIZ];
      writestring("/initclip[/pathtoproc lcvx/matrix lcvx/currentmatrix lcvx");
      writestring("/initmatrix lcvx/initclip lcvx /newpath lcvx\n");
      writestring(" 0 0 /moveto lcvx\n");
      sprintf(buffer,
	      " %lf 0/rlineto lcvx 0 %lf/rlineto lcvx -%lf 0/rlineto lcvx\n",
	      width, height, width);
      writestring(buffer);
      writestring(" /clip lcvx /newpath lcvx /setmatrix lcvx /exec lcvx]\n");
      writestring(" cvx def\n");
   }
   writestring("/initgraphics{initmatrix newpath initclip 1 setlinewidth\n");
   writestring(" 0 setlinecap 0 setlinejoin []0 setdash 0 setgray\n");
   writestring(" 10 setmiterlimit}bind def\n");
   if (nobinding) /* desperation measures */
      writestring("/bind{}def\n");
   writestring("%%EndProcSet\n");
   writeprolog();
   for (thispg = 0; thispg < maxpage; thispg += modulo) {
      int add_last = 0;
      struct pagespec *ps;
      for (ps = specs; ps != NULL; ps = ps->next) {
	 int actualpg;
	 int add_next = ((ps->flags & ADD_NEXT) != 0);
	 if (ps->reversed)
	    actualpg = maxpage-thispg-modulo+ps->pageno;
	 else
	    actualpg = thispg+ps->pageno;
	 if (actualpg < pages)
	    seekpage(actualpg);
	 if (!add_last) {
	    writepageheader("pstops", ++pageindex);
	 }
	 writestring("gsave\n");
	 if (ps->flags & GSAVE) {
	    char buffer[BUFSIZ];
	    if (ps->flags & OFFSET) {
	       sprintf(buffer, "%lf %lf translate\n", ps->xoff, ps->yoff);
	       writestring(buffer);
	    }
	    if (ps->flags & ROTATE) {
	       sprintf(buffer, "%d rotate\n", ps->rotate);
	       writestring(buffer);
	    }
	    if (ps->flags & SCALE) {
	       sprintf(buffer, "%lf dup scale\n", ps->scale);
	       writestring(buffer);
	    }
	    writestring("/pstopsmatrix matrix currentmatrix def\n");
	 }
	 if (width > 0.0 && height > 0.0) {
	    writestring("initclip\n");
	 }
	 writestring("/pstopssaved save def\n");
	 if (add_next) {
	    writestring("/showpage{}def/copypage{}def/erasepage{}def\n");
	 }
	 if (actualpg < pages)
	    writepagebody();
	 else
	    writestring("showpage\n");
	 writestring("pstopssaved restore grestore\n");
	 add_last = add_next;
      }
   }
   writetrailer();

   exit(0);
}
