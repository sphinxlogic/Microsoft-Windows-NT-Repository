#
#  MMS (MMK) build procedure for Pine (Pine V3.89)
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
	DEFINE SYS SYS$LIBRARY

NETOPTS = ,[-]VMS_NETLIB_LINK/OPTIONS
.ENDIF

CINCLUDE = /INCLUDE=([],[-.PICO],[-.C-CLIENT])

.IFDEF __ALPHA__
CC	= $(CC)/STANDARD=VAXC$(CDEFS)$(CINCLUDE)
OPTFILE =
OPTIONS =
.ELSE
CC	= $(CC)$(CDEFS)$(CINCLUDE)/NOWARNINGS
OPTFILE = ,[-]VAX_VMS_LINK.OPT
OPTIONS = $(OPTFILE)/OPTIONS
.ENDIF

C_CLIENT_LIB	= [-.C-CLIENT]C-CLIENT$(OLB)
PICO_LIB	= [-.PICO]PICO$(OLB)


OBJS = OS_vms,addrbook,addrutil,adrbklib,args,context,filter,folder,-
	help,helptext,imap,init,mailcmd,mailindx,mailview,newmail,-
	other,pine,print,reply,screen,send,signals,status,strings,-
	tty_vms

PINE$(EXE) :	PINE$(OLB)($(OBJS)),$(C_CLIENT_LIB),$(PICO_LIB)$(OPTFILE)
	- $(LINK)$(LINKFLAGS) PINE$(OLB)/INCLUDE=PINE/LIBRARY$(OPTIONS)-
		$(NETOPTS),$(C_CLIENT_LIB)/LIBRARY,$(PICO_LIB)/LIBRARY
