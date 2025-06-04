This is version II.11 of Kronos.

This version contains bug fixes only... no new functionality.

The executable in this release were compiled and linked under VMS 6.2.  We are
about to upgrade to VMS 7.1, so this will be the last release linked on 6.xx.

Both VAX and ALPHA executables are provided in the event that someone doesn't
have a FORTRAN compiler.  The executables are, further, built with 1 hour and
15 minute wakeup intervals for the same reason.  The default format is alpha. 
VAX users should manually copy the executables from the [.VAX_EXE] directory to
the proper location.

Kronos is maintained with the help of my FPP program (full release included in
the .SOURCE directory), ZIP/UNZIP (widely available), and Todd Aven's MAKE (not
included, but the functioning of the MAKEFILE in the .SOURCE directory is
fairly self-evident - run FPP {if needed}, compile, and link {see link commands
in the makefile}).  The MERLIB library of utility routines is needed to rebuild
Kronos.  This library is included in this release, but is maintained seperately
on our Web site.


Art Ragosta
ragosta@merlin.arc.nasa.gov
(Happy to help, but please check the manual first)

HTTP://AFDD.ARC.NASA.GOV/AFDD/RSD/AUTOTEAM.HTM
