 






                 Read Before Installing MadGoat FTP V2.1-2

          Please read the following important information before
          installing or using MadGoat FTP V2.1-2.

          Installation Information

          MadGoat FTP V2.1-2 requires the following:

          o  VMS V5.0 or later or OpenVMS AXP V1.0 or later

          o  For OpenVMS VAX TCP/IP support: CMU-Tek TCP/IP
             (aka CMU-OpenVMS/IP) V6.4 or later, VMS/ULTRIX
             Connection V1.2 or later (aka DEC TCP/IP Services
             for OpenVMS), TGV MultiNet V2.2 or later, Process
             Software Corporation TCPware, or WIN/TCP or PathWay
             from Wollongong.

             For OpenVMS AXP TCP/IP support: MultiNet V3.2 or
             later, DEC TCP/IP Services for OpenVMS v2.0 or
             later, Process Software Corporation TCPware, or
             WIN/TCP or PathWay from Wollongong.

          Contents of this Kit

          This kit contains all of the materials necessary
          to install MadGoat FTP V2.1-2. You will need
          approximately 15,000 blocks of disk space to hold the
          installation kit.

          The MGFTP021.% files make up the installation kit,
          which you should install with VMSINSTAL:

               $ @SYS$UPDATE:VMSINSTAL MGFTP021 disk:[dir]

          Manuals are included in save set E (file MGFTP021.E)
          in PostScript, plain ASCII, and Bookreader versions.
          The manuals are placed in the MADGOAT_ROOT:[DOC]
          directory during the installation. You can use BACKUP
          to retrieve them from the installation kit prior to
          installation:

               $ BACKUP MGFTP021.E/SAVE *.*

                                                                1

 






          It is strongly recommended that you read all of the
          manuals carefully before attempting to install MadGoat
          FTP.

          Release Notes Information

          The release notes for MadGoat FTP V2.1-2 contain
          important installation-related information as well
          as a summary of changes since the last release.
          Also included is information on known problems,
          documentation errors, and restrictions. The release
          notes can be retrieved by using VMSINSTAL OPTIONS N.

          Source Code

          The BLISS source code for MadGoat FTP is included as
          saveset MGFTP021.F. The installation gives you the
          opportunity to install the sources, if desired.























          2
