ZLIB, Utilities, Compression and Decompression Library

© Tous droits réservés 2003 Jean-François PIÉRONNE

Zlib V1.1.4

Zlib is a compression/decompression library used by several software packages,
including MySQL and Python.

By default, the installation procedure installs this library package in the
SYS$COMMON:[LIBZ] directory. To install it in another directory, dev:[dir], use
the /DESTINATION parameter of the PRODUCT command. In this case, the library
package will be installed in the dev:[dir.LIBZ] directory. Installation

   1. Make the directory which holds the ZIP file your default directory
   2. Extract the PCSI kit from the ZIP archive.

$ UNZIP "-V" ZLIB-V0101-4-1

   3. Install the Zlib package to your chosen destination.

$ PRODUCT INSTALL zlib  (default)

  or ...

$ PRODUCT INSTALL zlib /DESTINATION=dev:[dir]

   4. Finally, run the DCL procedure STARTUP.COM to define the necessary logicals.

(You might want to add the relevant line to your SYSTARTUP_VMS.COM file.)

$ @SYS$COMMON:[LIBZ]STARTUP  (default)

  or ...

$ @dev:[dir.LIBZ]:STARTUP

