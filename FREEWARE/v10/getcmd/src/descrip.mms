!
!  MMS build file for GETCMD  -- Both OpenVMS VAX and AXP
!
!  Author:	Hunter Goatley
!
!  Date:	November 4, 1992
!
!  Description:
!
!	This MMS file will compile GETCMD using either the VAX BLISS-32
!	compiler or the BLISS-32E cross compiler for Alpha.  To build
!	for Alpha, define the symbol __ALPHA__:
!
!		$ MMS/MACRO=__ALPHA__
!
!
.IFDEF __MATTS_MMS__
.ELSE
TYPE :
	@ mac = ""
	@ axp = f$getsyi("HW_MODEL").ge.1024
	@ if axp then mac = "/MACRO=(__ALPHA__=1)"
	@ if axp then write sys$output "Building GETCMD for AXP"
	@ if .not.axp then write sys$output "Building GETCMD for VAX"
	@ $(MMS)$(MMS$QUALIFIERS)'mac' GETCMD
.ENDIF

.IFDEF EXE
.ELSE
EXE = .EXE
OBJ = .OBJ
OLB = .OLB
.ENDIF

.IFDEF __ALPHA__
OPT = .ALPHA_OPT
SYSEXE = /SYSEXE
.ELSE
OPT = .OPT
SYSEXE =
.ENDIF

LINKFLAGS = $(LINKFLAGS)/NOTRACE

GETCMD :	GETCMD$(EXE), GETCMD.HLP
	!GETCMD built

OBJS	= GETCMD, GETCMD_MSG, GETCMD_CLD, MADGOAT_OUTPUT

GETCMD$(EXE) :	GETCMD$(OLB)($(OBJS)), getcmd$(OPT)
	$(LINK)$(LINKFLAGS)$(SYSEXE) getcmd$(olb)/INCLUDE=GETCMD/LIBRARY,-
		getcmd$(OPT)/OPTIONS

GETCMD.HLP :		GETCMD.RNH
	RUNOFF $(MMS$SOURCE)

GETCMD.RNH :		GETCMD.HELP, CVTHELP.TPU
	EDIT/TPU/NOSECTION/NODISPLAY/COMMAND=CVTHELP.TPU $(MMS$SOURCE)

CVTHELP.TPU :
	@!

CLEAN :
	delete/nolog *.hlp;*,*.rnh;*,*.*obj;*,*.*olb;*,*.*exe;*
