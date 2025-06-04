XPAINT_247, GRAPHICS, X Windows Paint program

Release notes for XPaint 2.4.7
------------------------------

NOTE: This version of XPaint is released and maintained by Torsten Martinsen
(bullestock@dk-online.dk). It is based on XPaint 2.1.1, which was written by
David Koblas. Please do NOT send bug reports etc. to David Koblas.

To compile this program, you will need:

        - an ANSI C compiler (such as GCC),
        - the Xpm library, version 3.4c or later

-Torsten Martinsen (bullestock@dk-online.dk), August 1996

Xpaint 2.4.7 OpenVMS port:                   Jan 1997
=========================

This new version was ported on Alphastation with VMS 6.2, DEC C 5.2 and Motif
1.2-4 . I was also able to build Xpaint on VAXstation with VAX C and DEC C,
but only the DEC C resulting code is working. (Note that with the previous
version 2.4.4 the VAX code crashes even compiled with DEC C).

Executables provided:
--------------------
VAX-VMS 6.1 built with DEC C 5.0 & DECW/MOTIF 1.2
Alpha-VMS 6.2 built with DEC C 5.2 & DECW/Motif 1.2-4

Obj files and libraries are also provided for VAX and Alpha to help relink.

Ressource file:
---------------

You can copy XPAINT.DAT file under SYS$LOGIN (or under
SYS$COMMON:[DECW$DEFAULTS.USER] for a system-wide definition).

Porting notes:
-------------

1) The "readonly" variable was renamed into "Readonly" to avoid collision with
   reserved word into all source files.

2) Adding some M_PI definition where missing.

3) Collision at link time between graphicCreate and GraphicCreate routines.
   We rename GraphicCreate into Graphic_Create to avoid double definition
   and crash at run time ...

XAW library used : the XAW3D available with Johannes Plass's 
Ghostview-VMS 2.7.6

You can find Ghostview 2.7.6 distribution at ada.cenaath.cena.dgac.fr and
ftp2.cnam.fr (into [.DECWINDOWS] directory) or at the  original ftp site
iphthf.physik.uni-mainz.de


Known bugs:
-----------
A crash generally occurs when saving a PNG image (tried with PNG 0.88 and
PNG 0.89C).  However, you can read a PNG image without troublen, but you must
save in GIF, TIFF or JPEG.


Patrick Moreau

pmoreau@cena.dgac.fr
moreau_p@decus.fr

