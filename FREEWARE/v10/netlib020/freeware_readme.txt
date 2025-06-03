NETLIB V2.0C, TCP/IP, Vendor-independent TCP/IP routines
                                       
   [28-FEB-1995]
   
   NETLIB consists of routines for interfacing with any of the following
   TCP/IP packages:

    CMU-Tek TCP/IP V6.5 or later                (OpenVMS VAX)
    DEC VMS/ULTRIX Connection V2.0 or later     (OpenVMS VAX and OpenVMS AXP)
    TGV MultiNet V2.2 or later                  (OpenVMS VAX and OpenVMS AXP)
    Process Software Corporation TCPware        (OpenVMS VAX and OpenVMS AXP)
    Wollongong Software WIN/TCP and PathWay     (OpenVMS VAX and OpenVMS AXP)

   NETLIB is implemented as a pair of shareable libraries: a
   transport-independent library and a transport-dependent library.
   Multiple transports can be installed simultaneously; a logical name
   selects the library to be used at run-time.
   
   NETLIB is included in the following packages also available here:
   
     * Message Exchange (MX)
     * MadGoat FTP
     * NEWSRDR
       
   NETLIB is used by, but is not included with, the following packages
   available here:
   
     * MadGoat FINGER
     * NSQUERY
       
   The NETLIB package included in those kits may not be the most current;
   check the release notes for the other packages to see if you could
   benefit from updating your NETLIB installation.
   
   The files NETLIBvvu.A, NETLIBvvu.B, and NETLIBvvu.C comprise the
   VMSINSTALlable installation kit for NETLIB (where "vvu" represents the
   version number). The file NETLIBvvu_SRC.BCK contains the source code
   to NETLIB.

   
  CONTACTING THE AUTHOR
  
   NETLIB was written by Matt Madison. To report bugs, please send mail
   to MadGoat-Bugs@WKUVX1.WKU.EDU. Additional MadGoat mailing lists are
   available as well.
