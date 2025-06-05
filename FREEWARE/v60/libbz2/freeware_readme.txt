LIBBZ2, Utilities, Data Compression Library

© Tous droits réservés 2003 Jean-François PIÉRONNE

LibBZ2 V1.0.2

LibBZ2 is a block-sorting compression/decompression library used by several
software packages, including Python.

By default, the installation procedure installs this library package in the
SYS$COMMON:[LIBBZ2] directory. To install it in another directory, dev:[dir],
use the /DESTINATION parameter of the PRODUCT command. In this case, the 
library package will be installed in the dev:[dir.LIBBZ2] directory. 
Installation

   1. Make the directory which holds the ZIP file your default directory
   2. Extract the PCSI kit from the ZIP archive.

$ UNZIP "-V" LIBBZ2-V0100-2-1

   3. Install the library package to your chosen destination.

$ PRODUCT INSTALL libbz2  (default)

  or ...

$ PRODUCT INSTALL libbz2 /DESTINATION=dev:[dir]

   4. Finally, run the DCL procedure STARTUP.COM to define the necessary 
      logicals.

(You might want to add the relevant line to your SYSTARTUP_VMS.COM file.)

$ @SYS$COMMON:[LIBBZ2]STARTUP  (default)

  or ...

$ @dev:[dir.LIBBZ2]:STARTUP


