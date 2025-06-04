#
#  MMS (MMK) build procedure for Pine V3.89
#
#  Written by Hunter Goatley <goathunter@WKUVX1.WKU.EDU>,  6-APR-1994 09:11
#  Eg / 8.4.94: Mod. for better Alpha support
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
.IFDEF MULTINET
TCPIP = MULTINET=1
.ELSE
TCPIP = NETLIB=1
.ENDIF

.IFDEF __ALPHA__
MACRO = __ALPHA__=1,$(TCPIP)
.ELSE
MACRO = $(TCPIP)
.ENDIF

.IFDEF HEBREW
MMS_MACROS = /MACRO=(HEBREW=1,$(MACRO))
.ELSE
MMS_MACROS = /MACRO=($(MACRO))
.ENDIF

PINE :
	set default [.pico]
	$(MMS)$(MMS_MACROS) /IGNORE=WARNING
	set default [-.c-client]
	$(MMS)$(MMS_MACROS) /IGNORE=WARNING
	set default [-.pine]
	$(MMS)$(MMS_MACROS) /IGNORE=WARNING
	@ write sys$output "Executables can be found in:"
	@ write sys$output ""
	@ write sys$output "[.PICO]PICO.EXE - The stand-alone editor (not needed)."
	@ write sys$output "[.C-CLIENT]MTEST.EXE - Interactive testing program (not needed)."
	@ write sys$output "[.C-CLIENT]IMAPD.EXE - the IMAP daemon. Should be copied elsewhere and defined"
	@ write sys$output "                       in the INETD.CONF file or equivalent."
	@ write sys$output "[.PINE]PINE.EXE - What you waited for..."
	@ write sys$output ""
	!PINE built

.FIRST
	@ COPY/LOG [.PICO]OSDEP_VMS.H [.PICO]OSDEP.H;
	@ COPY/LOG [.C-CLIENT]OSDEP_VMS.H [.C-CLIENT]OSDEP.H;
	@ copy/log [.PINE]OS_VMS.H [.PINE]OS.H;
