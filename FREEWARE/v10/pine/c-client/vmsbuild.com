$ DEFINE SYS SYS$LIBRARY:		! Normal .H location.
$ CC_DEF = "(''P1')"
$ LINK_OPT = ""
$ IF P1 .EQS. "" THEN CC_DEF=""""""
$ IF F$LOCATE("MULTINET", P1) .LT. F$LENGTH(P1)
$ THEN
$	DEFINE SYS MULTINET_ROOT:[MULTINET.INCLUDE.SYS]
$	DEFINE NETINET MULTINET_ROOT:[MULTINET.INCLUDE.NETINET]
$	LINK_OPT = ",[-]VMS_MULTINET_LINK/OPTION"
$ ENDIF
$ IF F$LOCATE("NETLIB", P1) .LT. F$LENGTH(P1) THEN LINK_OPT = ",[-]VMS_NETLIB_LINK/OPTION"
$!
$ CC_PREF = ""
$ IF F$LOCATE("VAX", F$GETSYI("HW_NAME")) .EQS. F$LENGTH(F$GETSYI("HW_NAME"))
$ THEN
$ CC_PREF = "/PREFIX=(ALL,EXCEPT=(SOCKET,CONNECT,BIND,LISTEN,SOCKET_READ,SOCKET_WRITE,SOCKET_CLOSE,SELECT,ACCEPT,BCMP,BCOPY,BZERO,GETHOSTBYNAME,"
$ CC_PREF = CC_PREF + "GETHOSTBYADDR,GETPEERNAME,GETDTABLESIZE,HTONS,HTONL,NTOHS,NTOHL,SEND,SENDTO,RECV,RECVFROM))"
$ CC_PREF = CC_PREF + "/STANDARD=VAXC"
$ ELSE
$ LINK_OPT = LINK_OPT + ",[-]VAX_VMS_LINK/OPTION"
$ ENDIF
$ SET VERIFY
$ CC'CC_PREF'/DEFINE='CC_DEF' OSDEP_VMS
$ CC'CC_PREF'/DEFINE='CC_DEF' vms_mail
$ CC'CC_PREF'/DEFINE='CC_DEF' MAIL
$ CC'CC_PREF'/DEFINE='CC_DEF' SMTP
$ CC'CC_PREF'/DEFINE='CC_DEF' RFC822
$ CC'CC_PREF'/DEFINE='CC_DEF' NNTP
$ CC'CC_PREF'/DEFINE='CC_DEF' NNTPClient
$ CC'CC_PREF'/DEFINE='CC_DEF' news_vms
$ CC'CC_PREF'/DEFINE='CC_DEF' MISC
$ CC'CC_PREF'/DEFINE='CC_DEF' IMAP2
$ CC'CC_PREF'/DEFINE='CC_DEF' SM_VMS
$!
$ LIBRARY/OBJECT/CREATE/INSERT C-CLIENT OSDEP_VMS,vms_mail,MAIL,SMTP,RFC822,-
	NNTP,nntpclient,news_vms,MISC,IMAP2,SM_VMS
$!
$! The followings should not be at the library since they contain MAIN functions
$ CC'CC_PREF'/DEFINE='CC_DEF' MTEST
$ CC'CC_PREF'/DEFINE='CC_DEF' IMAPD
$!
$ SET NOVERIFY
$ LINK MTEST,IMAP2,MAIL,MISC,NNTP,nntpclient,news_vms,OSDEP_VMS,RFC822,SMTP,-
  SM_VMS,VMS_MAIL'LINK_OPT'
$ LINK IMAPD,IMAP2,MAIL,MISC,NNTP,nntpclient,news_vms,OSDEP_VMS,RFC822,SMTP,-
   SM_VMS,VMS_MAIL'LINK_OPT'
$ EXIT
