JPEG, Graphics, Library supporting the JPEG format

Installation

The best way to build the Jpeg library is currently to execute the
modified DCL procedure make_vms.com from this server. This procedure
will examine the system environment, create a suitable jconfig.h file
and use the available tools (MMS/MMK, C compiler) to rebuild the
sources. Make_vms.com currently accepts the following options in
arbitrary order:

DEBUG
  Build the library with debug information and without optimization.
lopts=<value>
  Options to pass to the link command e.g. lopts=/tracback
ccopt=<value>
  Options to pass to the C compiler e.g. ccopt=/float=ieee
LINK
  Do not compile, just link Jpeg example programs and shareable image
CC=<DECC||VAXC||GNUC>
  Force usage of the given C compiler over the intrinsic selection
  (which is DEC C, VAX C, and finally GNU C).

An object library of the Jpeg code will be generated in any case, on
OpenVMS Alpha additionally a shareable image will be created.
It also ensures a working copy of testimgp.jpg is available for testing.

Test problems with progressive Jpeg file

In case you do experience problems with the check for the progressive
Jpeg test image (i.e. Backup /Compare/Log testimgp.jpg. testoutp.jpg.)
you most probably packed the original file testimgp.jpg directly with
Zip. This causes an extra byte to turn up at the end of the file after
unpacking. I suspect this is a bug and have reported this to the
Zip/UnZip developers. To circumvent this the archive from this server
contains a backup saveset with this specific image.

Package contents

The main result of the compilation is the object library libjpeg.olb
(OpenVMS Alpha: jpegshr.exe), which other applications need to link
against. Additionally five small executable programs are created:

CJPEG
  Compress an image file to a JPEG file. Supported input formats: PPM (PBMPLUS
  color format), PGM (PBMPLUS gray-scale format), BMP, Targa
DJPEG
  Decompress a JPEG file to an image file. Supported output formats: PBMPLUS
  (PPM/PGM), BMP, GIF, Targa
JPEGTRAN
  Jpegtran translates JPEG files from one variant of JPEG to another,
  for example from baseline JPEG to progressive JPEG.
RDJPGCOM
  Display text comments from a JPEG file
WRJPGCOM
  Insert text comments into a JPEG file

The latest version of the OpenVMS port of the JPEG library should always be 
accessible via 

http://www.decus.de:8080/www/vms/sw/jpeg.htmlx
