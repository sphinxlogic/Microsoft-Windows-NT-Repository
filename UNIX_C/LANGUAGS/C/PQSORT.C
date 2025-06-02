31-Aug-85 05:11:17-MDT,3829;000000000001
Return-Path: <unix-sources-request@BRL.ARPA>
Received: from BRL-TGR.ARPA by SIMTEL20.ARPA with TCP; Sat 31 Aug 85 05:11:08-MDT
Received: from usenet by TGR.BRL.ARPA id a002829; 31 Aug 85 6:42 EDT
From: Duane Morse <duane@anasazi.uucp>
Newsgroups: net.sources
Subject: qsort source
Message-ID: <276@anasazi.UUCP>
Date: 29 Aug 85 14:04:07 GMT
To:       unix-sources@BRL-TGR.ARPA

Here is my own version of qsort for those of you interested in sorting
algorithms.
--------------------SUBROUTINE FOLLOWS, THEN MY SIGNATURE -------------
/*TITLE pqsort.c - quicksort algorithm 1/10/85 10:04:32 */

static char *version = "@(#)pqsort.c	1.1 1/10/85 10:04:32";

/*
** void pqsort(vec, nel, esize, compptr)
**
**  Quick Sort routine.
**  Based on Knuth's ART OF COMPUTER PROGRAMMING, VOL III, pp 114-117.
**  For some unknown reason, this works faster than the library's qsort.
**
** Parameters:
**  vec = points to beginning of structure to sort.
**  nel = number of elements.
**  esize = size of an element.
**  compptr = points to the routine for comparing two elements.
**
** Returns:
**  Nothing.
*/

static int elsize;		/* Element size */
static int (*comp)();		/* Address of comparison routing */

static void memexch(), mysort();

void pqsort(vec, nel, esize, compptr)

unsigned char *vec;
int nel;
int esize;
int (*compptr)();

{
  /* If less than 2 items, done */
  if (nel < 2)
    return;

  elsize = esize;
  comp = compptr;

  /* Call the real worker */
  mysort(vec, nel);
}
/*PAGE*/
/*
** void mysort(vec, nel)
**
**  The real quick sort routine.
**
** Parameters:
**  vec = points to beginning of structure to sort.
**  nel = number of elements.
**
**  esize = size of an element.
**  compptr = points to the routine for comparing two elements.
**
** Returns:
**  Nothing.
*/

static void mysort(vec, nel)

unsigned char *vec;
int nel;

{
  register short i, j;
  register unsigned char *iptr, *jptr, *kptr;

  /*
  ** If 2 items, check them by hand.
  */

begin:
  if (nel == 2) {
    if ((*comp)(vec, vec + elsize) > 0)
      memexch(vec, vec + elsize, elsize);
    return;
  }

  /*
  ** Initialize for this round.
  */

  j = nel;
  i = 0;
  kptr = vec;
  iptr = vec;
  jptr = vec + elsize * nel;
/*PAGE*/
  while (--j > i) {

    /*
    ** From the righthand side, find the first value that should be
    ** to the left of k.
    */

    jptr -= elsize;
    if ((*comp)(jptr, kptr) > 0)
      continue;

    /*
    ** Now from the lefthand side, find the first value that should be
    ** to the right of k.
    */

    iptr += elsize;
    while(++i < j && (*comp)(iptr, kptr) <= 0)
      iptr += elsize;

    if (i >= j)
      break;

    /*
    ** Exchange the two items.
    ** k will eventually end up between them.
    */

    memexch(jptr, iptr, elsize);
  }

  /*
  ** Move item 0 into position.
  */

  memexch(vec, iptr, elsize);

  /*
  ** Now sort the two partitions.
  */

  if ((nel -= (i + 1)) > 1)
    mysort(iptr + elsize, nel);

  /*
  ** To save a little time, just start the routine over by hand.
  */

  if (i > 1) {
    nel = i;
    goto begin;
  }
}
/*PAGE*/
/*
** memexch(s1, s2, n)
**
**  Exchange the contents of two vectors.
**
** Parameters:
**  s1 = points to one vector.
**  s2 = points to another vector.
**  n = size of the vectors in bytes.
**
** Returns:
**  Nothing.
*/

static void memexch(s1, s2, n)

register unsigned char *s1;
register unsigned char *s2;
register int n;

{
  register unsigned char c;

  while (n--) {
    c = *s1;
    *s1++ = *s2;
    *s2++ = c;
  }
}
------------------------------------------------------
-- 

Duane Morse	...!noao!terak!anasazi!duane
(602) 275-0302
