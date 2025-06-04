MX V4.2, SOFTWARE, E-mail message exchanging software
   
   [20-JUN-1994]
   
   Message Exchange (MX) is electronic mail software for OpenVMS VAX
   systems running VMS V5.0 or later, including OpenVMS V6.0, and OpenVMS
   AXP systems running OpenVMS AXP V1.0 or later. It supports Internet
   mail over CMU-OpenVMS TCP/IP, VMS/ULTRIX Connection, TGV MultiNet,
   Process Software's TCPware, and Wollongong's WIN/TCP and PathWay;
   BITNET mail over Wingra's Jnet; and UUCP mail over DECUS UUCP. Also
   included is support for SMTP message transfers over DECnet and X.25
   (using VAX P.S.I) and L-Soft International's LISTSERV software. MX
   uses VMS Mail for local message entry and delivery, and includes
   support for mailing lists and mail-based file servers.
   
   Features:
   
     * Users send and receive messages using VMS MAIL. Support for
       "signature" files is included in the VMS MAIL interface. Full
       support for automatic forwarding with the VMS MAIL SET FORWARD
       command is included. User-defined alias databases for e-mail
       addresses is supported. Binary files may be sent through MX to
       other VMS sites running MX, PMDF, or MultiNet.
       
     * Provides SMTP (RFC 821) message transfers over CMU-OpenVMS TCP/IP
       (aka CMU-Tek TCP/IP), DEC TCP/IP Services for OpenVMS (aka
       VMS/ULTRIX Connection), TGV MultiNet, TCPware from Process
       Software, and PathWay and WIN/TCP from Wollongong. Includes
       support for Internet domain system MX records. Also supports SMTP
       over DECnet and X.25 (using VAX P.S.I.).
       
     * Provides BSMTP message transfers with other BITNET mailers over
       Jnet, compatible with the CU Mailer package for VM systems. Fully
       supports BITNET-Internet gateways for non-Internet-connected
       systems. When combined with the SMTP support, can also provide a
       BITNET/Internet gateway service.
       
     * Interfaces with DECUS UUCP to provide a single mail interface to
       all mail protocols. Can also gateway between UUCP and other
       networks. (Not supported under OpenVMS AXP yet because DECUS UUCP
       has not been ported to AXP yet.)
       
     * Provides a mailing list processor with automatic subscription
       requests. Mailing lists can be configured to restrict postings
       only to subscribers or list owners, and to restrict the automatic
       subscription handling. Internet mailing list conventions and a
       subset of LISTSERV commands are supported.
       
     * Supports one or more file servers that can be set up by the system
       manager to handle automatic distribution of packages of files
       using mail as the distribution medium. Large transfers can be
       deferred to off-hours, and daily per-user, per-system, and/or
       per-server limits can be placed on each server.
       
     * Provides interfaces for a site-provided custom mail transport and
       custom address processing routines.
       
     * Provides its own message queueing subsystem. RMS block I/O is used
       to provide extremely fast queue access with little overhead.
       
  CONTACTING THE AUTHORS
  
   MX was written by Matt Madison and is now maintained by Hunter
   Goatley. To report bugs, please send mail to
   MadGoat-Bugs@WKUVX1.WKU.EDU. Additional MadGoat mailing lists are
   available as well.
   
   A mailing list for additional support of MX exists. To subscribe, send
   the following command in the body of a mail message to
   MXserver@WKUVX1.WKU.EDU:

        SUBSCRIBE MX-List "Your real name here"

   The MX-List mailing list is gatewayed to the vmsnet.mail.mx newsgroup.
