VMS Mosaic V3.1, WEB_TOOLS, Mosaic web browser for VMS

The major changes in this release include:

   o Support for HTML V3.2 including tables and client-side image maps
   o Support for animated GIF images
   o Progressive display
   o An optional preferences file

Support for frames is similar to that provided by Lynx; a list of
frames is displayed, each of which may be viewed separately.

A C compiler (DEC C, VAX C or VAX GNU C v2.7.1) is required to build
it.  Support is included for UCX, MultiNet, TCPware, Pathway, CMU
via LIBCMUII, and SOCKETSHR/NETLIB.  Although the build supports all
versions of Motif from 1.1 forward, the recommended versions of Motif
are 1.2-3 with the latest patch kit and 1.2-4.  Any version of VMS
up thru 7.1-1H2 (or later) that supports Motif 1.1 or greater can be
used.  The file README.BUGS contains the latest information on any
bugs and bug fixes for this release; please check it before reporting
problems.

The files in this directory:

    MOSAIC3_1.ZIP - the Mosaic 3.1 source in zip file format

    README.BUGS - the known bugs in and bug fixes for 3.1

    README.VMS-3_1 - the Mosaic 3.1 documentation

    PATCH1.ZIP - bug fixes for 3.1.  Should be unzipped in
                 same directory as MOSAIC3_1.ZIP

The latest release, beta test and bug fixes can be found at:

    ftp://alpha.wvnet.edu/mosaic/

-------------------------------------------------------------------------
George Cook (cook@wvnet.edu)
WVNET
