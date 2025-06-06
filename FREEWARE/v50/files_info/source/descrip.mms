#
#  MMS (MMK) build file
#
#
BUILD_IT :
	@ decc = f$search("SYS$SYSTEM:DECC$COMPILER.EXE").nes.""
	@ axp = f$getsyi("HW_MODEL").ge.1024
        @ macros = ""
        @ if axp.or.decc then macros = "/MACRO=("
        @ if decc then macros = macros + "__DECC__=1,"
        @ if axp then macros = macros + "__ALPHA__=1,"
        @ if macros.nes."" then macros = f$extract(0,f$length(macros)-1,macros)+ ")"
	@ show symbol macros
        $(MMS)$(MMSQUALIFIERS)'macros' default

.IFDEF EXE
.ELSE
EXE = .EXE
OBJ = .OBJ
.ENDIF

.IFDEF __ALPHA__
SYSEXE	= /SYSEXE
OPT	= .ALPHA_OPT
OPTFILE =
OPTIONS =
.ELSE
.IFDEF __DECC__
OPTFILE =
OPTIONS =
.ELSE
OPTFILE = ,[]VAXCRTL.OPT
OPTIONS = $(OPTFILE)/OPTIONS
.ENDIF
SYSEXE	= ,SYS$SYSTEM:SYS.STB/SELECTIVE_SEARCH
.ENDIF

.IFDEF __DEBUG__
MFLAGS	= $(MFLAGS)/DEBUG/NOOPTIMIZE
BFLAGS	= $(BFLAGS)/DEBUG/NOOPTIMIZE
CFLAGS	= $(CFLAGS)/DEBUG/NOOPTIMIZE
LINKFLAGS = $(LINKFLAGS)/DEBUG
.ELSE
LINKFLAGS = $(LINKFLAGS)/NOTRACEBACK/MAP/CROSS/FULL
.ENDIF

[]VAXCRTL.OPT :
	@ open/write tmp $(MMS$TARGET)
	@ write tmp "SYS$SHARE:VAXCRTL.EXE/SHARE"
	@ close tmp

DEFAULT :	FILES_INFO$(EXE)
	!FILES_INFO built

FILES_INFO_OBJS = FILES_INFO,FILES_MACRO,FILES_CLD,LOCK_NONPAGED_CODE

FILES_INFO$(OBJ) :		FILES_INFO.C
FILES_MACRO$(OBJ) :		FILES_MACRO.MAR
FILES_CLD$(OBJ) :		FILES_CLD.CLD
LOCK_NONPAGED_CODE$(OBJ) :	LOCK_NONPAGED_CODE.MAR

FILES_INFO$(EXE) :	FILES_INFO$(OLB)($(FILES_INFO_OBJS)),-
			FILES_INFO$(OPT)$(OPTFILE)
	$(LINK)$(LINKFLAGS) FILES_INFO$(OLB)/INCLUDE=FILES_INFO/LIBR,-
			FILES_INFO$(OPT)/OPTIONS$(OPTIONS)$(SYSEXE)
