31-Oct-85 07:53:26-MST,4174;000000000001
From: "Carl S. Gutekunst" <csg@pyramid.uucp>
Newsgroups: net.sources
Subject: bncvt -- filter bnproc batches to unbatch
Date: 31 Oct 85 03:12:27 GMT
Followup-To: net.sources.bugs
Keywords: news bnproc unbatch bncvt
To:       unix-sources@BRL-TGR.ARPA

/*===========================================================================**
**              BBBBBBB   NN    NN    CCCCC   VV    VV  TTTTTTTT             **
**              BB    BB  NNN   NN   CC   CC  VV    VV     TT                **
**              BB    BB  NNNN  NN  CC        VV    VV     TT                **
**              BBBBBBB   NN NN NN  CC        VV    VV     TT                **
**              BB    BB  NN  NNNN  CC         VV  VV      TT                **
**              BB    BB  NN   NNN   CC   CC    VVVV       TT                **
**              BBBBBBB   NN    NN    CCCCC      VV        TT                **
**===========================================================================**
**  Copyright (C) 1985 by PYRAMID TECHNOLOGY CORPORATION, Mountain View, CA  **
**===========================================================================**
** Permission is granted to freely use and distribute this software, as long **
** as no attempt is made to profit from it, and this notice is included.     **
**===========================================================================**
**
** ** bncvt.c -- utility to filter bnproc news batches to unbatch.
**
**    Written in a fit of desperation by Carl S. Gutekunst
**
** ** Decsription:
**
**	This filter accepts uncompressed news batches in "bnproc" format and
**	writes them out in "unbatch" format. Using 2.10.3 news, its output can
**	be piped directly into rnews.
**
**	The filter also adjusts for the bnproc "article eater" bug, which threw
**	off the article byte count and caused rnews to discard entire articles.
**
** ** Execution (for 2.10.3 netnews):
**
**	uncompress | bncvt | rnews
**
** ** Generation:
**
**	cc bncvt.c -o bncvt -s -O
**
** ** $Log:	bncvt.c,v $
**	Revision 1.1  85/10/30  19:07:13  csg
**	Initial version, written in a fit of desperation by Carl S. Gutekunst.
**	
**===========================================================================*/

#include <stdio.h>

#define LINESIZE 128			/* Size of the input line buffer     */

static char RCSid[] = "$Header: bncvt.c,v 1.1 85/10/30 19:07:13 csg Rel $";

main ()
{
   char linebuf[LINESIZE], *lp;		/* Single line buffer, and pointer   */
   int expected, nbytes;		/* Bytes expected and read so far    */

   nbytes = expected = 0;
   while (fgets (linebuf, LINESIZE, stdin) != NULL)
   {
      /*
       * Check for an article eater. This is a DEL character, either 0x7F or
       * 0xFF, in the first column preceeding a new article byte count. It
       * usually throws off the byte count, so we have to add some padding
       * to keep rnews from losing sync (and discarding the next article).
       */

      if ((linebuf[0] & 0x7F) == 0x7F)
      {
	 if (expected > 0)
	    while (nbytes++ < expected)
	       putc ('\0', stdout);
      }

      /*
       * If we aren't expecting text, then we're expecting an article byte
       * count. This is a left-justified integer, immediately followed by a
       * newline. We ignore leading article-eater DEL characters.
       */

      if (nbytes >= expected)
      {
	 nbytes = expected = 0;
	 lp = linebuf;
	 while ((*lp & 0x7F) == 0x7F)
	    ++lp;
	 while (*lp >= '0' && *lp <= '9')
	    expected = expected * 10 + (*lp++ - '0');

	 if (*lp == '\n' && expected > 0)
	    printf ("#! rnews %d\n", expected);
	 else
	 {  fprintf (stderr, "Sync->%s", linebuf);
	    expected = 0;
	 }
      }

      /*
       * Another normal line of text: write it out.
       */

      else
      {
	 fputs (linebuf, stdout);
	 nbytes += strlen (linebuf);
      }
   }
}
