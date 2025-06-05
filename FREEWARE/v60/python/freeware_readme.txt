Python, Languages, Python V2.3 Programming Language

© Tous droits réservés 2003 Jean-François PIÉRONNE

Python V2.3

Full documentation and some useful links are available from the Python 
for VMS site: http://vmspython.dyndns.org/

Software Requirements

   1. CPQ SSL V1.0-B or V1.1(available as a free download from HP)
   2. JFP ZLIB 1.1-4  
   3. JFP LibBZ2 1.0-2 

I strongly recommend you install Python on an ODS-5 volume.

By default, the installation procedure installs Python in the
SYS$COMMON:[PYTHON] directory. To install it in another directory, dev:[dir],
use the /DESTINATION parameter of the PRODUCT command. In this case, Python will
be installed in the dev:[dir.PYTHON] directory. Installation

   1. Make the directory which holds the ZIP file your default directory
   2. Extract the PCSI kit from the ZIP archive.

$ UNZIP "-V" PYTHON-V0203--1

   3. Install Python to your chosen destination.

$ PRODUCT INSTALL python  (default)

  or ... 

$ PRODUCT INSTALL python /DESTINATION=dev:[dir]

   4. Finally, run the DCL procedure STARTUP.COM.

(You might want to add this line to your SYSTARTUP_VMS.COM file.)

$ @PYTHON_ROOT:[VMS]STARTUP

   4.
      Shared Images

      The STARTUP.COM procedure runs two other DCL procedures: LOGICALS.COM, 
      to set up the correct Python logicals; and INSTALL_DYNAMICS_MODULES.COM
      which uses the VMS INTALL command to load Python modules as shared 
      images (requires CMKRNL privileges). 

      By default, no modules are INSTALL'ed as shared images.

   5. (Optional): Post-Installation Module Compilation

$ SET DEFAULT PYTHON_VMS
$ @SETUP
$ PYTHON COMPILE_PYTHON_MODULES.PY
$ PYTHON -"OO" COMPILE_PYTHON_MODULES.PY

   6. (Optional): Post-Installation Module Testing

$ SET DEFAULT PYTHON_VMS
$ @SETUP
$ @ALLTESTS

Module Testing Results

Currently, 6 tests should fail:
test_codecs, test_queue, test_re, test_socket, test_unicode, and test_zipimport


