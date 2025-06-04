                  Read Before Installing MadGoat FTP V2.6-5

          Please read the following important information before
          installing or using MadGoat FTP V2.6-5.

          Installation Information

          MadGoat FTP V2.6-5 requires the following:

          o  For VAX systems:   OpenVMS VAX V5.2 or later
          o  For Alpha systems: OpenVMS Alpha V1.5 or later

          o  For Internet mail/SMTP support: Any currently
             supported TCP/IP package for VMS (see the NETLIB
             release notes for more details)

          Contents of this Kit

          This kit contains all of the materials necessary to
          install MadGoat FTP V2.6-5. You will need approximately
          15,000 blocks of disk space to hold the installation
          kit.

          The MGFTP026.% files make up the installation kit,
          which you should install with VMSINSTAL:

              $ @SYS$UPDATE:VMSINSTAL MGFTP026 disk:[dir]

          Manuals are included in save set F (file MGFTP026.F)
          in PostScript, plain ASCII, HTML, and Bookreader versions.
          The manuals are placed in the MADGOAT_ROOT:[DOC]
          directory during the installation. You can use BACKUP
          to retrieve them from the installation kit prior to
          installation:

              $ BACKUP MGFTP026.F/SAVE *.*

          It is strongly recommended that you read all of the
          manuals carefully before attempting to install MadGoat
          FTP.

          Release Notes Information

          The release notes for MadGoat FTP V2.6-5 contain
          important installation-related information as well
          as a summary of changes since the last release.
          Also included is information on known problems,
          documentation errors, and restrictions. The release
          notes can be retrieved by using VMSINSTAL OPTIONS N.

          Source Code

          The BLISS source code for MadGoat FTP is included as
          saveset MGFTP026.G. The installation gives you the
          opportunity to install the sources, if desired.
