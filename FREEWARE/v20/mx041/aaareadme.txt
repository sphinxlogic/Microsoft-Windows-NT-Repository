 






                       Read Before Installing Message Exchange V4.1

             Please read the following important information before installing
             or using MX V4.1.


             Installation Information

             MX V4.1 requires the following:

              o VMS V5.0 or later or OpenVMS AXP V1.0 or later

              o For OpenVMS VAX SMTP support: CMU-Tek TCP/IP (aka CMU-
                OpenVMS/IP) V6.4 or later, VMS/ULTRIX Connection V1.2 or later
                (aka DEC TCP/IP Services for OpenVMS) TGV MultiNet V2.2 or
                later, Process Software Corporation TCPware, or WIN/TCP or
                PathWay from Wollongong.

                For OpenVMS AXP SMTP support: MultiNet V3.2 or later, DEC
                TCP/IP Services for OpenVMS v2.0 or later, Process Software
                Corporation TCPware, or WIN/TCP or PathWay from Wollongong.

              o For BITNET mail support: Jnet V3.5 (VAX) and Jnet V3.7 (AXP).

              o For UUCP support: DECUS UUCP V1.1 or later (not supported under
                AXP yet).

              o For LISTSERV support: L-Soft International's LISTSERV, any
                version.

             Contents of this Kit

             This kit contains all of the materials necessary to install MX
             V4.1. The source kit, MX041_SRC.BCK, is a BACKUP save set contain-
             ing the full source tree for MX V4.1.

             You will need approximately 8 megabytes of disk space to hold the
             installation kit, plus 4.5 megabytes to hold the source kit.


                                                                              1

 






             The MX041.% files make up the installation kit, which you should
             install with VMSINSTAL. Manuals are included in save set L (file
             MX041.L), PostScript, plain ASCII, and Bookreader versions, and
             are placed in the MX_ROOT:[DOC] directory during the installation.
             You can use BACKUP to retrieve them from the installation kit
             prior to installation:

               $ BACKUP MX041.L/SAVE *.*

             It is strongly recommended that you read all of the manuals care-
             fully before attempting to install MX.

             Release Notes Information

             The release notes for MX V4.1 contain important installation-
             related information as well as a summary of changes since the
             last release. Also included is information on known problems,
             documentation errors, and restrictions. The release notes can be
             retrieved by using VMSINSTAL OPTIONS N.

             Source Code


             The Zip file MX041_SRC.ZIP contains the source code. Read the file
             00README.TXT in that Zip file for information on unloading the
             source code:

               $ UNZIP MX041_SRC.ZIP 00README.TXT












          2
