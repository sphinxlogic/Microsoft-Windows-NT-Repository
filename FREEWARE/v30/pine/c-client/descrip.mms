#
#  MMS (MMK) build procedure for C-Client (Pine V3.89)
#
#  Written by Hunter Goatley <goathunter@WKUVX1.WKU.EDU>,  6-APR-1994 09:11
#
#  By default, Pine is built with NETLIB support.
#
#  To build Pine, use:
#
#	$ MMS/MACRO=(macros)
#
#  To build on AXP, include __ALPHA__ in the "macros".
#  To build with Hebrew support, include HEBREW in the "macros".
#  To build with MultiNet support, include MULTINET in the "macros".
#
.IFDEF EXE
.ELSE
EXE = .EXE
OBJ = .OBJ
OLB = .OLB
.ENDIF

.IFDEF MULTINET
TCPIP = MULTINET=1
.ELSE
TCPIP = NETLIB=1
.ENDIF

.IFDEF HEBREW
CDEFS	= /DEFINE=(HEBREW=1,$(TCPIP))
.ELSE
CDEFS	= /DEFINE=($(TCPIP))
.ENDIF

.IFDEF MULTINET
.FIRST
	DEFINE SYS MULTINET_ROOT:[MULTINET.INCLUDE.SYS]
	DEFINE NETINET MULTINET_ROOT:[MULTINET.INCLUDE.NETINET]

NETOPTS = ,[-]VMS_MULTINET_LINK/OPTIONS
.ELSE
.FIRST
	define sys sys$library:

NETOPTS = ,[-]VMS_NETLIB_LINK/OPTIONS
.ENDIF

PREFIX_ROUTINES = SOCKET,CONNECT,BIND,LISTEN,SOCKET_READ,SOCKET_WRITE,-
	SOCKET_CLOSE,SELECT,ACCEPT,BCMP,BCOPY,BZERO,GETHOSTBYNAME,-
	GETHOSTBYADDR,GETPEERNAME,GETDTABLESIZE,HTONS,HTONL,NTOHS,NTOHL,-
	SEND,SENDTO,RECV,RECVFROM


.IFDEF __ALPHA__
CPREF	= /PREFIX=(ALL,EXCEPT=($(PREFIX_ROUTINES)))
CC	= $(CC)/STANDARD=VAXC$(CDEFS)$(CPREF)/INCLUDE=([-.PINE])
OPTFILE =
OPTIONS =
.ELSE
CC	= $(CC)$(CDEFS)/NOWARNINGS
OPTFILE = ,[-]VAX_VMS_LINK.OPT
OPTIONS = $(OPTFILE)/OPTIONS
.ENDIF

OBJS =	OSDEP_VMS,vms_mail,MAIL,SMTP,RFC822,NNTP,NNTPClient,-
		news_vms,MISC,IMAP2,SM_VMS

C-CLIENT :	IMAPD$(EXE),MTEST$(EXE)
	!C-Client libraries built

IMAPD$(EXE) :	C-CLIENT$(OLB)($(OBJS)),IMAPD$(OBJ)$(OPTFILE)
	- $(LINK)$(LINKFLAGS) IMAPD$(OBJ),-
		C-CLIENT$(OLB)/LIBRARY$(OPTIONS)$(NETOPTS)

MTEST$(EXE) :	C-CLIENT$(OLB)($(OBJS)),MTEST$(OBJ)$(OPTFILE)
	- $(LINK)$(LINKFLAGS) MTEST$(OBJ),-
		C-CLIENT$(OLB)/LIBRARY$(OPTIONS)$(NETOPTS)
