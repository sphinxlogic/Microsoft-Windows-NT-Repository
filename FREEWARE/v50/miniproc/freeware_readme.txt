MINIPROC, UTILITIES, General-Purpose Text Preprocessor

Miniproc is a small general purpose text preprocessor.   Miniproc 
is intended to be a portable, and much simpler, alternative to perl 
and M4. Miniproc is written in ANSI C. Some possible applications are: 

  1. Preprocessor for languages which do not have their own
     standard preprocessor (ie, Fortran.)  For use in preparing a
     single standard presource which is processed through
     miniproc to result in platform specific final source
     file(s). 

  2. Make-like utility for batches of HTML files. Example,
     you have many documents which all include a standard
     disclaimer on the bottom.  Change the disclaimer, and run a
     miniproc script to update the final HTML files. Or it could
     be run periodically in a batch queue to update dates within
     documents. 

  3. Mailing lists.  That is, whenever you need to generate
     many similar files which are all variants on a single
     template. 


  Copyright 1997,1998, 1999 by David Mathog and California Instititute of
  Technology.

  This software may be used freely, but may not be redistributed.
  Distributions may be obtained from:

     http://seqaxp.bio.caltech.edu/www/miniproc.html

  You may modify this sofware for your own use, but you may not incorporate
  any part of the original code into any other piece of software which will
  then be distributed (whether free or commercial) unless prior written
  consent is obtained.  For more information, or to report bugs, contact:

     mathog@seqaxp.bio.caltech.edu

  The current version is 3.04.

