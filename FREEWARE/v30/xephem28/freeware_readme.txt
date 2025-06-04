XEPHEM28, MOTIF_TOYS, Interactive Astronomical Ephemeris program for X Window

			Welcome to xephem v2.8

Xephem is an interactive astronomical ephemeris program for X Windows systems.
For a full list of features and the latest news, please see the xephem home
page at:

    http://iraf.noao.edu/~ecdowney/xephem.html.


The complete xephem version 2.8 release kit is a compressed tar file at:

    ftp://iraf.noao.edu/contrib/xephem/xephem_2.8/xephem_2.8.tar.gz.


Xephem requires at least X11R4 and OSF/Motif 1.1. Xephem has been built on
many UNIX systems as well as VMS. A list of known ported systems and building
hints may be found in the file INSTALL. A summary of the changes since v2.7.1
appears at the end of this notice.


There is only a small manual, but there is extensive on-line help, including
step-by-step examples. So, the first thing to do is to build xephem then start
selecting the Help buttons.  A good order to read the help entries is the way
they are listed in the Help pulldown off the Main menu. Instructions for
building xephem may be found in the file INSTALL.


Xephem uses several X resources to name supporting files and directories. The
sample X defaults file, XEphem.ad, and the fallback resources in xephem.c, have
these defined relative to this top level directory structure I have included in
this distribution kit. You may want to rearrange things for your installation.
See the file INSTALL for a list of all the files used by xephem at runtime. The
comments throughout XEphem.ad will also help you set up all the X resource
defaults for xephem.


I would like to hear what you think of xephem, good and bad. If you drop me a
quick note I will save your email address and directly send you announcements of
changes. Thank you for your interest in xephem.


Elwood Downey
ecdowney@noao.edu
http://iraf.noao.edu/~ecdowney
December 1995

===============================================================================

A complete revision history is at the top of versionmenu.c. Follows are the
changes from Version 2.7.1 to 2.8:

Sky view:
    Constellation boundaries are now fully supported.
    Names or magnitudes of the 10 brightest objects may be displayed.
    History list now restores Orientation and Limiting Magnitude and supports
	selective deletion.
    Greater use of open symbols to reduce obsuration in dense fields.
    Bayer designations shown in Greek character set.
    Magnitude dot sizes listed in an optional key area.
    Printing and saving to Postscript files is now supported.

Moon view:
    Now uses a higher resolution, full gray scale image.
    Image may be displayed at four different scales.
    Lunar latitude and longitude grid overlay.
    Scene may be flipped top/bottom and left/right.
    True sky background; great for watching occultations.
    User adjustable Earth shine.
    Database of over 1200 lunar features, including all Apollo landing sites. 
    Left mouse button controls a roaming magnifying glass and displays local
	solar altitude. 
    Right mouse button displays closest feature name, size, location and solar
	altitude. 

Miscellaneous:
    Printing Earth, Solar System and Sky views and saving to postscript files
	is now supported.
    Fully updated asteroids.edb database.
    Yale catalog stars now specified to nearly arc-second accuracy.
    Added columns for Hour Angle and Airmass in Data view.
    Main menu location may be set from a scrolled list of site names.
    One arc minute scale display in Jupiter and Saturn views.
    Data Base "No Dups" feature now based on 1 arc second position, not name.
    Includes gscdbd version 1.4.

Xephem 2.8 for OpenVMS                             December 1995
======================

If you want an access to the stars and objects databases, you need to define
a directory tree pointed by the rooted logical XEPHEM_DIR:

$ define/trans=conc XEPHEM_DIR disk:[directory.xephem.]

Under XEPHEM_DIR, you need at least AUXIL, EDB and WORK directories:

$ dir xephem_dir:[000000]

Directory XEPHEM_DIR:[000000]

AUXIL.DIR;1         EDB.DIR;1           WORK.DIR;1

Total of 3 files.

You need to copy the content of original AUXIL and EDB directories from the 
Xephem distribution. WORK directory may be under user's directory tree. In this
case, each user needs to update the resource file XEPHEM.DAT.

XEPHEM.DAT may be placed under SYS$COMMON:[DECW$DEFAULTS.USER] or under
SYS$LOGIN:

Compilation & link: 

Il you want to rebuild Xephem, you'll find various COMPILE*.COM and LINK*.COM
procedures to match your compiler (VAX C or DEC C on VAX, DEC C on Alpha). 
The link procedure can automagically sense the version of DECW/Motif 
installed on your system and select the right option file.

Enjoy !!

Patrick Moreau - CENA/Athis-Mons - FRANCE - 
pmoreau@cena.dgac.fr 
moreau_p@decus.decus.fr

