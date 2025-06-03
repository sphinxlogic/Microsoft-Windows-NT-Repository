
			Welcome to xephem v2.7.1

Xephem is an interactive astronomical ephemeris program for X Windows
systems. It ...

    computes heliocentric, geocentric and topocentric information for fixed
    celestial objects and objects in heliocentric and geocentric orbits;

    has built-in support for all planet positions; the moons of Jupiter, Saturn
    and Earth; Mars' and Jupiter's central meridian longitude; Saturn's rings;
    and Jupiter's Great Red Spot

    supports user-defined objects;

    comes with sample databases of over 20,000 objects in the release kit;

    can access the Hubble Guide Star Catalog from CD-ROM using gscdbd;

    can quickly locate all pairs of objects which are closer than a specified
    maximum;

    displays data in configurable tabular forms and in several detailed
    graphical formats; 

    can plot and list all data fields to disk files;

    can be programmed to search for arbitrary circumstances;

    can serve as the control point for robot telescopes in real-time via
    named streams pipes (UNIX fifos).

The complete xephem version 2.7.1 release kit is a compressed tar file at:
    ftp://iraf.noao.edu/contrib/xephem/xephem_2.7/xephem_2.7.1.tar.gz.

Xephem requires at least X11R4 and OSF/Motif 1.1. Xephem has been built on
many UNIX systems as well as VMS. A list of known ported systems and building
hints may be found in the file INSTALL. A summary of the changes since v2.6
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
changes. I maintain the master copy of the source on iraf.noao.edu in
contrib/xephem. Check there occasionally for updates and contributed prebuilt
executables. If you would like to contribute an executable, please let me know.
Release notes are maintained on http://iraf.noao.edu/~ecdowney/xephem.html.


Thank you for your interest in xephem.


Elwood Downey
ecdowney@noao.edu
http://iraf.noao.edu/~ecdowney
August 1995

===============================================================================

A complete revision history is at the top of versionmenu.c. Follows are the
changes from Version 2.7 to 2.7.1:

    Solar System Stereo view: improved expose handling and Help description.
    Solar System: Extra lines in ecliptic plane removed.
    Help: fixed several typos and improved description of Triad formats.
    Skyview: fix overlapping RA/Az grid lines near poles.
    Earth: improve mechanism to switch between Cylindrical and Spherical views.
    Database: slight change in creation sequencing for Ultrix. 
    Trails: improved appearance of stroke character set.
    Times near 0:00 no longer display as 24:00.
    Gscdbd upgraded to version 1.3.
