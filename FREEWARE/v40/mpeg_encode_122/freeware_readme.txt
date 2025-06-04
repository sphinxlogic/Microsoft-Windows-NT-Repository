MPEG, GRAPHICS, MPEG video encoder

                 MPEG, CCITT H.261 (P*64), JPEG
  Image and Image sequence compression/decompression C software engines.


The Portable Video Research Group at Stanford have developed
image/image sequence compression and decompression engines (codecs)
for MPEG, CCITT H.261, and JPEG. The primary goal of these codecs is
to provide the functionality - these codecs are not optimized for
speed, rather completeness, and some of the code is kludgey.

Development of MPEG, P64, and JPEG engines has not been the primary
goal of the Portable Video Research Group.  Our research has been
focused on software and hardware for portable wireless digital video
communication.  The charter of this group ended in the summer of 1994.

OpenVMS port of MPEG Encoder/Decoder V 1.22                  December 1996
===========================================

Mpeg was ported on OpenVMS with few modifications (in MPEG.C and LEXER.C
files). It compiles on VAX with either VAX C or DEC C and on Alpha with DEC C.

Sources, object and executables are provided into this kit, with also a VMS
help file.

You can use Mpeg via display program of ImageMagick to encode animations into
mpeg format; you just need to define mpeg symbol:

$ mpeg :== $disk:[directory]mpeg.exe

Have fun !!

Patrick Moreau
pmoreau@cena.dgac.fr
moreau_p@decus.fr
