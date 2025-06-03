CVTLIS V3.0-1, BUILD TOOLS, Convert .LIS files back to source files
                                       
   [10-DEC-1994]
   
   CVTLIS converts .LIS files from the VMS Source Listings on CD back to
   the appropriate source files. The following file types are recognized:

    Language        Type            Output file type
    ---------       ----            ----------------
    MACRO-32        (VAX)           .MAR
    AMACRO-32       (AXP)           .MAR
    BLISS-32        (VAX)           .B32
    BLISS-32E       (AXP)           .B32E
    BLISS-64        (AXP)           .B64
    MACRO-64        (AXP)           .M64
    VAX C           (VAX)           .C
    DEC C           (AXP)           .C
    CDU             (VAX/AXP)       .CLD
    MESSAGE         (VAX/AXP)       .MSG
    PL/I            (VAX)           .PLI
    VAX Pascal      (VAX)           .PAS
    DEC Pascal      (VAX/AXP)       .PAS
    SDL             (VAX/AXP)       .SDL

   
   
   CVTLIS is written in BLISS and will compile and run on both OpenVMS
   VAX and OpenVMS AXP.
   
   CVTLIS is highly dependent on the .LIS format produced for the source
   listings CDs. Fortunately, most of the VMS programmers follow rigorous
   source code formats, so this program will successfully convert most
   listing files.
   
   The BLISS sources are distributed with the software.
   
  CONTACTING THE AUTHOR
  
   MadGoat CVTLIS was written by Hunter Goatley. To report bugs, please
   send mail to MadGoat-Bugs@WKUVX1.WKU.EDU. Additional MadGoat mailing
   lists are available as well.
