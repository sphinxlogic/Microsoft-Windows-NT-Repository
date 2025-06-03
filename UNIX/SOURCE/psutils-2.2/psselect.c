/* psselect.c
 * AJCD 27/1/91
 * rearrange pages in conforming PS file for printing in signatures
 *
 * Usage:
 *       psselect [-q] [-e] [-o] [-r] [-p<pages>] [infile [outfile]]
 */

#include "psutil.h"

void usage()
{
   fprintf(stderr,
	   "Usage: %s [-q] [-e] [-o] [-r] [-p<pages>] [infile [outfile]]\n",
	   prog);
   fflush(stderr);
   exit(1);
}

struct pgrange {
   int first, last;
   struct pgrange *next;
};

typedef struct pgrange range;

range * makerange(beg, end, next)
     int beg, end;
     range *next;
{
   range *new;
   if ((new = (range *)malloc(sizeof(range))) == NULL) {
      fprintf(stderr, "%s: out of memory\n", prog);
      fflush(stderr);
      exit(1);
   }
   new->first = beg;
   new->last = end;
   new->next = next;
   return (new);
}

range * mergerange(beg, end, curr)
     int beg, end;
     range *curr;
{
   if (curr) {
      range *this, *prev;
      for (this=prev=curr; this; prev=this, this=this->next) {
	 int lo = (beg < curr->first) ? -1 : (beg > curr->last) ? 1 : 0;
	 int hi = (end < curr->first) ? -1 : (end > curr->last) ? 1 : 0;
	 if (hi < 0)
	    return (makerange(beg, end, curr));
	 else if (lo <= 0) { /* beginning of range */
	    if (lo < 0)
	       this->first = beg;
	    if (hi > 0) {
	       while (this->next && this->next->first <= end) {
		  range *eaten = this->next;
		  this->next = eaten->next;
		  if (eaten->last > end)
		     end = eaten->last;
		  free(eaten);
	       }
	       this->last = end;
	    }  /* else range included; no change */
	    return (curr);
	 }
      }
      prev->next = makerange(beg, end, NULL);
      return (curr);
   } else return (makerange(beg, end, NULL));
}


range * addrange(str, rp)
     char *str;
     range *rp;
{
   int first=0;
   if (isdigit(*str)) {
      first = atoi(str);
      while (isdigit(*str)) str++;
   }
   switch (*str) {
   case '\0':
      if (first)
	 return (mergerange(first, first, rp));
      break;
   case ',':
      if (first)
	 return (addrange(str+1, mergerange(first, first, rp)));
      break;
   case '-':
   case ':':
      str++;
      if (isdigit(*str)) {
	 int last = atoi(str);
	 while (isdigit(*str)) str++;
	 if (!first)
	    first = 1;
	 if (last >= first) 
	    switch (*str) {
	    case '\0':
	       return (mergerange(first, last, rp));
	    case ',':
	       return (addrange(str+1, mergerange(first, last, rp)));
	    }
      } else if (*str == '\0')
	 return (mergerange(first, MAXPAGES, rp));
   }
   fprintf(stderr, "%s: invalid page range\n", prog);
   fflush(stderr);
   exit(1);
}


int selectpage(page, even, odd, rp)
     int page, even, odd;
     range *rp;
{
   if (page&1) {
      if (odd) return (1);
   } else {
      if (even) return (1);
   }
   while (rp) {
      if (page >= rp->first) {
	 if (page <= rp->last)
	    return (1);
      } else return (0);
      rp = rp->next;
   }
   return (0);
}


main(argc, argv)
     int argc;
     char *argv[];
{
   int currentpg, maxpage = 0;
   int even = 0, odd = 0, all = 1, reverse = 0;
   range *pagerange = NULL;

   infile = stdin;
   outfile = stdout;
   verbose = 1;
   for (prog = *argv++; --argc; argv++) {
      if (argv[0][0] == '-') {
	 switch (argv[0][1]) {
	 case 'e':
	    even = 1; all = 0;
	    break;
	 case 'o':
	    odd = 1; all = 0; 
	    break;
	 case 'r':
	    reverse = 1;
	    break;
	 case 'p':
	    pagerange = addrange(*argv+2, pagerange); all = 0;
	    break;
	 case 'q':
	    verbose = 0;
	    break;
	 default:
	    usage();
	 }
      } else if (infile == stdin) {
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
   if ((infile=seekable(infile))==NULL) {
      fprintf(stderr, "%s: can't seek input\n", prog);
      fflush(stderr);
      exit(1);
   }
   scanpages();

   for (currentpg = 1; currentpg <= pages; currentpg++)
      if (selectpage(currentpg, even || all, odd || all, pagerange))
	 maxpage++;

   /* select pages */
   writeheader(maxpage);
   writeprolog();
   if (reverse) {
      for (currentpg = pages; currentpg > 0; currentpg--)
	 if (selectpage(currentpg, even || all, odd || all, pagerange))
	    writepage(currentpg-1);
   } else {
      for (currentpg = 1; currentpg <= pages; currentpg++)
	 if (selectpage(currentpg, even || all, odd || all, pagerange))
	    writepage(currentpg-1);
   }
   writetrailer();

   exit(0);
}
