GD, Graphics, Library to create graphs from programs

Installation

Just execute @make to build the library. This checks for available compilers 
as well as MMS/MMK and uses them for the build if available. Upon first 
invocation make will generate a template vmslib.dat, which needs to be 
tailored to reflect your local installation.
 
Minimum requirement for using the current version of the GDlib is to have 
installed Libpng and Zlib. Additional features of the library can be used if 
the following libraries are available too: Jpeglib, Freetype 2, and XPMlib. 
All information on these libraries needed by the build procedure is collected 
in the file vmslib.dat. The format of this is documented here. If not 
available make.com will create a template vmslib.dat upon first invocation 
which needs to be tailored to match the local installation. 


Build options


Make.com currently accepts the following options in arbitrary order: 

DEBUG
Build the library with debug information and without optimization. 
lopts=<value>
Options to pass to the link command e.g. lopts=/tracback 
ccopt=<value>
Options to pass to the C compiler e.g. ccopt=/float=ieee 
LINK
Do not compile, just link GD example programs 
CC=<DECC||VAXC||GNUC>
Force usage of the given C compiler over the intrinsic selection (which is 
DEC C, VAX C, and finally GNU C). 

Package contents


The main result of the compilation ist he object library libgd.olb, which 
other applications need to link against. Additionally a number of small 
executable programs are created: 

ANNOTATE
Add text to a JPEG graphics files 
FONTSIZETEST
Creates four test images with fonts of varying sizes 
FONTWHEELTEST
Creates four test images of spoke wheels 
GD2COPYPAL
Converts the color palette of a file in GD2 format to a specified palette.
GD2TIME
Timing program for GD2 reads.
GD2TOPNG
Read file in GD2 format and create file in PNG format.
GDDEMO
GD demonstration 
GDPARTTOPNG
Convert rectangular section of gd image to PNG format
GDTEST
Test suite for GD
GDTESTFT
Create test image using true type font
GDTOPNG
Convert file from GD format to PNG format.
PNGTOGD
Convert a PNG file into a GD file
PNGTOGD2
Convert a PNG file into a GD2 file
TESTAC
Create various test images from PNG image with alpha channel
WEBPNG
Report/change characteristics of a PNG image

Programs using the GD library

Fly
Simple frontend to GD 
GDchart
Library for on-the-fly chart generation (based on GD). 
GD.pm
Perl module with bindings to the GD library Installation

The latest version of the OpenVMS port of the GD library should always be 
accessible via 

http://www.decus.de:8080/www/vms/sw/gd.htmlx
