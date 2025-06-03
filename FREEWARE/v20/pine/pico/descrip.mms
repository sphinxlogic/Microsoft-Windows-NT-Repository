#
#  MMS (MMK) build procedure for Pico (Pine V3.89)
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

.IFDEF HEBREW
CDEFS	= /DEFINE=(HEBREW=1,ANSI_DRIVER=1)
.ELSE
CDEFS	= /DEFINE=(ANSI_DRIVER=1)
.ENDIF

.IFDEF __ALPHA__
CC	= $(CC)/STANDARD=VAXC$(CDEFS)
OPTFILE =
OPTIONS =
.ELSE
.FIRST
	define sys sys$library:

CC	= $(CC)$(CDEFS)/NOWARNING
OPTFILE = ,[-]VAX_VMS_LINK.OPT
OPTIONS = $(OPTFILE)/OPTIONS
.ENDIF

OBJS = ATTACH,ANSI,basic,bind,browse,buffer,composer ,display,file,fileio,-
	line,pico,random ,region ,search,spell,window,word,main,osdep_VMS

PICO$(EXE) :	PICO$(OLB)($(OBJS))$(OPTFILE)
	- $(LINK)$(LINKFLAGS) PICO$(OLB)/INCLUDE=MAIN/LIBRARY$(OPTIONS)
