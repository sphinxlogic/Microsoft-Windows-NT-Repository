This directory contains executables for VMS/OpenVMS (both VAX and Alpha AXP).

   2105 Jul 15 2000  README              what you're reading right now
   2971 Jun 22 1994  bilf100-src.zip     convert between binary and stream-LF
 126976 Sep  6 1994  gzip124x-axp.exe    gzip 1.2.4, VMS/AXP bare executable
  54784 Sep  6 1994  gzip124x-vax.exe    gzip 1.2.4, VMS/VAX bare executable
 209616 Sep  6 1994  gzip124x.zip        gzip 1.2.4, VMS executables, docs, etc.
  10282 Feb 25 1993  touchx.zip          BSD/SysV "touch" for VMS; VAX exe/docs
 165888 Jul 15 2000  unz541xV-axp.exe    UnZip 5.41, VMS/AXP bare executable
  99840 Jul 15 2000  unz541xV-vax.exe    UnZip 5.41, VMS/VAX bare executable
 808552 Apr 18 2000  unz541xV.zip        UnZip 5.41, VMS executables, docs, etc.
[ 65193 Feb  2 1993  zip11xV.zip         Zip 1.1, VMS/VAX exes/docs, no crypto ]
 439202 Apr 18 2000  zip23xV.zip         Zip 2.3, VMS/VAX exes/docs, no crypto ]

Zip 2.2 encryption executables and object files (docs in all archives) are
available only from our European site, ftp://ftp.icce.rug.nl/infozip/VMS/ :

   4155 Oct 24 1997  00binary-zip.README
 229030 Nov 23 1997  zcr22-vms-axp-exe.zip
 274574 Dec  6 1997  zcr22-vms-axp-obj.zip
 157446 Nov 23 1997  zcr22-vms-vax-decc-exe.zip
 158849 Dec  6 1997  zcr22-vms-vax-decc-obj.zip
 156625 Dec  6 1997  zcr22-vms-vax-vaxc-obj.zip

The executables are not self-extracting; they are plain executables.  Set
them up as foreign commands by defining symbols for them, as in:

  unzip == $disk:[dir]unz541xV-axp.exe

and then use normal UnZip options (e.g., "unzip -t zip23xV.zip" to test).
You will have to do something similar to install the enclosed executables;
be sure to read the README files and other documentation in the zipfiles.

Object files are included in the zipfiles so that users of older systems
can link their own executables.

All Info-ZIP packages contain documentation.  The sources are in ../src .

Send problem reports on Zip and UnZip to:  Zip-Bugs@lists.wku.edu
Send problem reports on gzip to:           support@gzip.org

Last updated:  15 July 2000
