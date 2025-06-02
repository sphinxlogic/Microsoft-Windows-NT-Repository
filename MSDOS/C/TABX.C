Date: Sat, 24 Feb 90 14:46:12 IST
From: Baruch Nissenbaum <BARUCH%TAUNIVM.BITNET@CUNYVM.CUNY.EDU>
To:   Keith Petersen <w8sdz@WSMR-SIMTEL20.ARMY.MIL>
Re:   TABX.C - Filter that expands TABS to SPACES

To Keith Petersen,  In reply to Info-IBMPC Digest  Volume 90 : Issue  23

After sending TABX.C to the info-ibm list I realized I wasn't too happy with
its limited ability to handle only fixed tab spacing, So I've added the
ability to define variable tab stops.

In this newer version, tab stops are defined by a 'fancy' tab specification
string, somewhat in a word processor style:

the string  --T---T   defines tab stops at col. 3 & 7.

This version should be more useful for general usage then the original
tabx.c.  Also the header of main() was changed so it should be compiled
with unix cc.

Thanks for keeping the name of tabx as it is. I have written filters to
expand tabs several times but each time the program names where something
like 'f3' and after a while they got lost amongst other filers of this sort.
Now that the program has its own meaningful name I hope it is the last
time I'm writing it.

Baruch Nissenbaum   -  Tel Aviv University  -  School of Engineering.
E-mail: BITNET: BARUCH@TAUMIVM, BARUCH@TAUENG or BARUCH@TAUNIVM.TAU.AC.IL

-----------THE PROGRAM FOLLOWS:----------

/* TABX.C - tab expansion utility */
/* Written by  Baruch Nissenbaum  - Tel-Aviv University, Israel 2/1990  (c) */
/* E-MAIL: BARUCH@TAUNIVM.BITNET  (BARUCH@TAUNIVM.TAU.AC.IL)                */
/* TEL: + 792 3 776892 (voice) */
/* Comments, Suggestions and Ideas are welcomed */

/* This program should work for a file of any size, and any line length  */
/* compiled and tested with TURBO-C, should work with any other compiler */
/* see usage() for details of operation*/

#include <stdio.h>
#include <ctype.h>
#include <string.h>

main(argc,argv)
int argc;
char **argv;
{
   int c,col=0,tab=8,dlen=0;
   char *d;

   if(argc>1)
      if(strchr(d=argv[1],'T') != NULL || strchr(d,'t') )
         dlen=strlen(d);
      else
         tab=atoi(argv[1]);
   if(tab<1) {
      usage();
      exit(0);
   }

   while((c=getchar()) != EOF)
      if(c=='\t') {              /* expand tabs */
         putchar(' ');  col++;   /* tab will generate at least one space */
         for(; col<dlen && d[col]!='t' && d[col]!='T'; col++)
            putchar(' ');        /* expansion of tabs (tab string) */
         if( col>=dlen )
            for(; col%tab; col++)
               putchar(' ');     /* expansion of tabs (fixed spacing) */
      }
      else if(c=='\n' || c=='\r') {
         putchar(c);
         col=0;
     }
     else {                      /* any other character */
        putchar(c);
        col++;
     }
}

usage()
{
   fprintf(stderr,"      TABX  - tab expansion utility\n");
   fprintf(stderr,"      This program expands tab characters to a list of spaces
   fprintf(stderr,"usage:   TABX  [tab_spec]  < source_file  > dest_file\n");
   fprintf(stderr,"      tab_spec can be an integer for setting fixed tab spacin
   fprintf(stderr,"      tab_spec can also be a string as in:\n");
   fprintf(stderr,"        TABX ---T---T-T---T  < in_file > out_file\n");
   fprintf(stderr,"      In this case tab stops will match the position of the '
   fprintf(stderr,"      in the above example - tabs will be set to 4 8 10 and 1
   fprintf(stderr,"      There are no limits on file size or line length\n\n");
   fprintf(stderr," Written by Baruch Nissenbaum,  Israel,  6 Feb 90 (c)\n\n");
   fprintf(stderr," Not for sale,  FREE distribution only!!\n\n");
}

