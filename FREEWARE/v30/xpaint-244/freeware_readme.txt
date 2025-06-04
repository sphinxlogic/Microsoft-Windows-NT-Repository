XPAINT-244, GRAPHICS, X Window painting program and image editor

Release notes for XPaint 2.4.4
------------------------------

NOTE: This version of XPaint is released and maintained by Torsten Martinsen
(bullestock@dk-online.dk). It is based on XPaint 2.1.1, which was written by
David Koblas. Please do NOT send bug reports etc. to David Koblas.

KNOWN BUGS

The 'Water Colour Brush' tool does not work well on colour mapped visuals with
256 colours. The reasons are:
	1) It is too slow. This could perhaps be fixed.
	2) All entries in the colour map are used up in rather quickly.
	3) The limited number of colours give rise to round-off errors
	   when computing the new pixel colours.

Undo/Redo exhibits some weirdness in certain circumstances.

Selecting a region enables the 'Last Filter' menu item.

If you are running a program which uses a lot of colourmap entries, such as
Netscape without a private colormap loaded, you may see this error when
starting XPaint:

X Error of failed request:  BadDrawable (invalid Pixmap or Window parameter)
  Major opcode of failed request:  14 (X_GetGeometry)
  Resource id in failed request:  0x0
  Serial number of failed request:  686
  Current serial number in output stream:  686

-Torsten Martinsen (bullestock@dk-online.dk), June 1996

Xpaint 2.4.4 OpenVMS port:   August 1996
=========================

This new version was ported on Alphastation 255 with VMS 6.2, DEC C 5.2 
and Motif 1.2-4 . I was able to build Xpaint on VAX with VAX C and DEC C, 
but the resulting code crash. VAX olbs are provided in the [.SRC] directory 
for those who want to debug the beast.

EXE, Objs and Olb provided (Alpha): VMS 6.1-1H2, DEC C 5.0, MOTIF 1.2-3

Known bug on Alpha:
------------------
Sometimes, a crash occurs when saving a PNG image.

Patrick Moreau

pmoreau@cena.dgac.fr
moreau_p@decus.fr
