XPAT2, GAMES, Various Patience (Solitaire) card games for X11

Xpat 2.0 port under OpenVMS:                  December 1997
============================

Xpat2 gives you the ability to play various solitaire card games.

The version built is the Motif version with XPM rounded cards.
There is also a Xlib version (not tested on VMS).

Installation:
------------

You can play xith xpat2 on the distribution by invoking SETUP_VMS.COM
procedure (you must set the LANG logical name before or comment it if you want
English language).

For a regular installation, you need to copy the whole [.LIB...] tree on
another location and point it via the XPAT_DIR rooted logical:

$ cre/dir disk:[toto.xpat]
$ def/trans=conc XPAT_DIR disk:[toto.xpat.]
$ backup/log [.LIB...]*.* XPAT_DIR:[000000...]*
$ set prot=(w:re) xpat_dir:[000000...]*.*

You need to copy XPAT.DAT resource file either under your SYS$LOGIN directory
or under the decw$defaults location:

$ copy xpat.dat sys$common:[decw$defaults.user]* /lo/prot=(w:re)

 VMS resource file XPAT.DAT was built from the [.SRC]XPAT.AD file

Rebuild:
-------
You can either relink (with LINK_AXP.COM and LINK_VAX.COM procedure into [.SRC]
directory) or recompile before relink (with COMPILE_DECC.COM procedure). Before
relinking, please unzip the obj archives AXPOBJS.ZIP or VAXOBJS.ZIP

On VAX, with Motif 1.2, XmuFillRoundedRectangle is undefined. I have commented
the invocation of DEC XMU library in VAX opt files and provided an old DECUS
XMU library which works well.

XPM libraries are also provided for VAX and Alpha.

Enjoy !!

Patrick Moreau
pmoreau@cena.dgac.fr
moreau_p@decus.fr
http://www2.cenaath.cena.dgac.fr
