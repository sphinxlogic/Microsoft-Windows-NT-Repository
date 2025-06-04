NetPBM, GRAPHICS, Suite of graphic image format manipulation & conversion pgms

          Enhanced portable bitmap  toolkit.  The PBMPLUS toolkit allows
      conversions between image files  of different format.  By means of
      using common intermediate formats, only  2  * N conversion filters
      are required to support N distinct  formats,  instead  of the N**2
      which would be required to convert directly between any one format
      and  any  other.    The  package also includes  simple  tools  for
      manipulating portable bitmaps.
 
          The package consists of four upwardly compatible sections:
   
      pbm     Supports monochrome bitmaps (1 bit per pixel).
 
      pgm     Supports grayscale  images.    Reads  either  PBM  or  PGM
              formats and writes PGM format.
 
      ppm     Supports full-color images.  Reads either PBM, PGM, or PPM
              formats, writes PPM format.
 
      pnm     Supports content-independent manipulations on any  of  the
              three formats listed above, as well  as  external  formats
              having  multiple  types.  Reads either PBM,  PGM,  or  PPM
              formats,  and  generally writes the same type as  it  read
              (whenever a PNM tool makes an exception and ``promotes'' a
              file to a higher format, it informs the user).

--------------------------------------------------------------------------------

                           N E T P B M

Netpbm is based on the widely spread Pbmplus package (release: 10 Dec 91).
On top of that, a lot of improvements and additions have been made. After
the latest release of Pbmplus, a lot of additional filters have been
circulating on the net. The aim of Netpbm was, to collect these and to turn
them into a package. This work has been performed by a group of program-
mers all over the world. If *you* have some code to add, please contact us,
and we will incorporate it. There is a mailing list for discussions about
Netpbm. You post a message to the list by writing to "netpbm@fysik4.kth.se".
If you want to be on the list, send a mail with the message "subscribe netpbm"
to "majordomo@fysik4.kth.se". If you want to report a bug, please send
your report to netpbm@fysik4.kth.se, and to the author of Pbmplus,
jef@netcom.com.


Please note, that this is not an official Pbmplus release. The code in
this release is merely a collection of code from various sources around
the world. Not all of the new code parts follow the high standard of
programming of Pbmplus. We have tried to make the code portable to as
many systems as possible, but we haven't cleaned up all routines. We hope
that this release will help the many users of Pbmplus to upgrade their
code all in one piece, instead of having to hunt down different code
fragments at different sites around the world. We also hope, that our
effort will help the author of Pbmplus, Jef Poskanzer, to make a new
official release soon.

The file Netpbm lists all new featues and functions in netpbm.
