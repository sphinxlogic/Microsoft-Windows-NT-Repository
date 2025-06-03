:   Print cawf'ed pages using psf filters
:
:   The "-r 3" option in psfbs below removes 3 blank lines between
:   pages (to allow correct printing in the imageable area of the
:   postscript printer).  Different printers may require different
:   number of lines to remove.  (see psfbs.1)
:
:   If double sided printing is desired, use the "-d" option
:   in psf below.
:
:   NOTE: to make cawf run correctly, you may have to remove the '\f'
:         in cawf's routine "pass3.c" (line 128).
:         I don't think it should be there under any circumstances!
:
@echo off
:
: accept only -man and -ms options
:
if "%1" == "-man" goto doit
if "%1" == "-ms" goto doit
echo Usage:  %0 [-man] [-ms] doc.nr doc.nr ...
echo ......  e.g.  %0 -man psf.1
echo:
goto fini
:doit
:
: save the -man option
:
set cawfmac=%1
:
: process each file on the command line
: quit if a file does not exit
:
:next
shift
if "%1" == "" goto fini
if not exist %1 goto missing
cawf %cawfmac% %1 | psfbs -i -r 3 | psf -2 > prn
goto :next
:missing
echo file %1 does not exist
echo:
:fini
set cawfmac=
