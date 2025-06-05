Xpdf202, Graphics, Display/process Adobe PDF files

Package contents

The Xpdf distribution actually contains seven programs:

Xpdf
  This is the actual Xwindows viewer program and therefore the most visible
  part of the distribution.
pdffonts
  Pdffonts lists the fonts used in a Portable Document Format (PDF) file along
  with various information for each font.
pdfimages
  Pdfimages saves images from a Portable Document Format (PDF) file as
  Portable Pixmap (PPM), Portable Bitmap (PBM), or JPEG files.
pdfinfo
  Pdfinfo prints the contents of the "Info" dictionary (plus some other useful
  information) from a Portable Document Format (PDF) file.
pdftopbm
  Pdftopbm converts Portable Document Format (PDF) files to
  black-and-white image files in Portable Bitmap (PBM) format.
pdftops
  As the name implies, this filter converts a PDF document to a PS file
  suitable for printing. Graphics elements are preserved by this filter.
pdftotext
  This program extracts the text portions of a PDF document and saves them to
  a plain ASCII file. Useful when working via slow lines.

Building

vms_make.com makes some effort to check the compiler installation. If
it complains about a missing C++ compiler chances are very good, that
actually  you don't have one installed. Note that C++ is an entirely
seperate product from C, so having a C compiler installed on your
system (which is also neccessary) does in no way imply that you also
have C++. If you're not sure contact your system manager or drop me a
note and we'll try to sort out things together.

Parameters

vms_make.com takes up to three parameters:

 1.Configuration settings

  a4
     Use european A4 as the default paper size.
  no_text_select
     Disable text selections in Xpdf
  opi_support
     Compile Xpdf with support for the Open Prepress Interface (OPI)

 2.Compiler detection
  In case you want to override the automatic compiler detection specify either
  DECC or GCC as the second parameter, e.g. @vms_make "" GCC
 3.Global configuration file
  Since version 0.93 Xpdf supports a global configuration file, which
  can be used to provide meaningfull default settings for all users.
  This is by default decw$system_defaults:xpdfrc.dat on OpenVMS, but
  may be overriden during compile time using the third parameter to the
  build procedure, e.g
  @vms_make "" "" sys$manager:xpdfrc.dat

Sample invocation of the script

  
@vms_make a4,opi_support "" 

External libraries

Xpdf uses external libraries for some of its functions. These are
supported via the driver file VMSLIB.DAT. The libraries currently used
by Xpdf are:

T1lib
  Font rasterizer library used to display rotated texts (e.g. in
  tables). The main reason why one actually needs t1lib support for
  Xpdf on OpenVMS is to correctly render rotated strings. While this is
  supported by some Xservers (e.g. XFree on Linux), it is not supported
  natively on OpenVMS (at least up to Motif 1.2.5), so one has to
  resort to using t1lib to add this capability. As a bon t1lib also
  should improve the quality of the display by using antialiasing.
xpm
  Enable Xpm support for Xpdf in icons. For this to work properly you have to
  make sure that the Xpm includes are found in your X11 path.
Freetype2
  Library to process embeded fonts in PDF documents.
Libpaper
  This library adds support for various paper formats to Xpdf.

Decryption?!

Many documents use compressed PDF, which need the decryption routines
to be rendered. Since version 0.91 these are part of the base
distribution, so no further patches are required as it was the case
with earlier versions.

National Language Support

Xpdf may be extended with files describing character mappings for
various different encodings (like e.g the ISO series, BIG-5 etc.).
These can be downloaded separatly and the location of the files needs
to be added to your .xpdfrc file. To facilitate this for OpenVMS users
I did write a short DCL procedure, which will turn the files provided
for the purpose on Unix systems into one which can be used on OpenVMS.
The procedure can be found as mk-xpdfrc.com.

Xresources

Xpdf doesn't use an own Xresource file but relies on the Xresource
information it gets from the Xserver. To set values for this process
you can change decw$xdefaults.dat in your Xresource directory. Changes
in this file only take effect after the next restart of the server. For
experimenting with different settings one is better off using Xrdb,
which allows to load new values for a running Xserver.
If you do use t1lib with Xpdf you need to set one Xresource for each of
the 14 well-known PDF fonts. An example for these settings can be found
in xpdf-font.txt. In case you don't have the needed fonts already on
your  system (they are e.g. part of  Grace), you can get a copy of my
t1lib font collection in file t1fonts.zip.

Example

Just in case you don't have a PDF document handy to check how Xpdf is working,
you can fetch a copy of the VMS book from this server, which contains a short
history of the OpenVMS operating system issued by Digital.

In case you did build a version with Freetype support you might want to
use this PDF file which does contain numerous TrueType Fonts. If it
does display correctly your copy of Xpdf is working fine.

Currently open issues

The following issues were reported by OpenVMS users of Xpdf. They are
acknowledged by the author of Xpdf and should be resolved during the
next couple of maintenance releases

During startup of Xpdf the message Warning: Cannot convert string
"-*-times-medium-r-normal--16-*-*-*-*-*-iso8859-1" to
type FontStruct is displayed
  This message can be safely ignored. This particular font is used only inside
  of the "About" window of Xpdf. It will be replaced in there by a default
  font. The display of PDF documents is not affected at all by this.
  The solution will make the font settable via an Xresource.
  The same behaviour can be observed on Linux systems.
Slow rendering of of extremely fine patterned areas
  If a document contains extremely fine patterned areas (which most
  probably will look like solid color areas in the result), Xpdf might
  appear to loop, i.e. using an inordinate amount of time to render the
  document. If you are patient enough it will be shown in the end
  though. Documents known to be affected by this are recent editons of
  Shannon knows HPC.
  Most probably the solution will involve cacheing the pattern in memory
  instead of calculating it over and over again.
  The same behaviour can be observed on Linux systems.
Only print to file supported
  Currently Xpdf only can be used to save a PDF file as postscript,
  which subsequently can be printed via DCL. There is no support for
  direct printing. Adding this is not entirely trivial, since the print
  setup under Unix relies on a pipe to whatever print filter is used,
  which does not lend itself easily to an equivalent under OpenVMS.
  Still the print system is supposed to be redesigned and there might
  be an opportunity to get better support there.


The latest version of the OpenVMS port of Xpdf  should always be 
accessible via 

http://www.decus.de:8080/www/vms/sw/xpdf.htmlx
