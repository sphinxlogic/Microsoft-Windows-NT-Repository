# $Id: descrip.mms,v 1.13 1995/10/27 11:42:38 tom Exp $
#
# VAX/VMS MMS build script for FLIST and BROWSE
#
# Tested with:
#	VMS system version 5.4-2
#	MMS version 2.6
#	VAX-C version 3.2
# and
#	OpenVms 6.1
#	DEC-C 4.1

########(source-lists)#########################################################
C_SRC	= -
	ACPCOPY.C, -
	ACPLOOK.C, -
	ALARM.C, -
	ARGVDCL.C, -
	BROWSE.C, -
	CANOPEN.C, -
	CHPROT.C, -
	CHRCMD.C, -
	CMDSTK.C, -
	CMPPROT.C, -
	CRT.C, -
	DCLAREA.C, -
	DCLARG.C, -
	DCLCHK.C, -
	DCLINX.C, -
	DCLOPT.C, -
	DCLWILD.C, -
	DDS.C, -
	DEBRIEF.C, -
	DIRARG.C, -
	DIRCHK.C, -
	DIRCMD.C, -
	DIRCMD.C, -
	DIRDATA.C, -
	DIRENT.C, -
	DIRFIND.C, -
	DIRHIGH.C, -
	DIROPEN.C, -
	DIRPATH.C, -
	DIRPROT.C, -
	DIRREAD.C, -
	DIRSEEK.C, -
	DOMORE.C, -
	DSPC.C, -
	EDTCMD.C, -
	FL.C, -
	FLCOLS.C, -
	FLCOPY.C, -
	FLCREA.C, -
	FLDLET.C, -
	FLDUMP.C, -
	FLEDIT.C, -
	FLESCP.C, -
	FLFIND.C, -
	FLMORE.C, -
	FLNOID.C, -
	FLPAGE.C, -
	FLPROT.C, -
	FLREAD.C, -
	FLRNAM.C, -
	FLSCAN.C, -
	FLSHOW.C, -
	FLSORT.C, -
	FREELIST.C, -
	GETPAD.C, -
	GETPROT.C, -
	GETRAW.C, -
	HELP.C, -
	HIGHVER.C, -
	INSPECT.C, -
	ISOWNER.C, -
	NAMEHEAP.C, -
	PATHDOWN.C, -
	PATHUP.C, -
	PUTRAW.C, -
	RABRFA.C, -
	RMSCC.C, -
	RMSINIT.C, -
	RMSIO.C, -
	RMSLOOK.C, -
	SCANINT.C, -
	SCANVER.C, -
	SETCTRL.C, -
	SETPROT.C, -
	SHOQUOTA.C, -
	SNAPSHOT.C, -
	STRABBR.C, -
	STRCLIP.C, -
	STRFORM2.C, -
	STRLCPY.C, -
	STRNULL.C, -
	STRSCALE.C, -
	STRSKPS.C, -
	STRTRIM.C, -
	STRUCPY.C, -
	STRVCMP.C, -
	STRVCPY.C, -
	STRWCMP.C, -
	SYASCTIM.C, -
	SYBINTIM.C, -
	SYDELETE.C, -
	SYFOM.C, -
	SYGETMSG.C, -
	SYGETSYM.C, -
	SYHOUR.C, -
	SYRENAME.C, -
	SYRIGHTS.C, -
	SYTRNLOG.C, -
	TABEXP.C, -
	TERMSIZE.C, -
	TTRACE.C, -
	UID2S.C, -
	VERSION.C, -
	WHOAMI.C, -
	XABPROUI.C

H_SRC	= -
	ACP.H, -
	ATRDEF.H, -
	BOOL.H, -
	CREDEF.H, -
	CRT.H, -
	DCLARG.H, -
	DCLOPT.H, -
	DDS.H, -
	DIRCMD2.H, -
	DIRENT.H, -
	DQFDEF.H, -
	FATDEF.H, -
	FCHDEF.H, -
	FIBDEF.H, -
	FLIST.H, -
	GETPAD.H, -
	GETPROT.H, -
	LBRDEF.H, -
	MAIN.H, -
	NAMES.H, -
	RMSIO.H, -
	TEXTLINK.H

OPT_FILES = -
	NORMAL.OPT, -
	SHARABLE.OPT

CLD_FILES = -
	BROWSE.CLD, -
	FLIST.CLD

HLP_FILES = -
	BROWSE.HLP, -
	FLIST.HLP

SOURCES = $(C_SRC) $(H_SRC) $(OPT_FILES) $(CLD_FILES) $(HLP_FILES)

########(derived lists)########################################################
MODULES	= -
	ACPCOPY, -
	ACPLOOK, -
	ALARM, -
	ARGVDCL, -
	CANOPEN, -
	CHPROT, -
	CHRCMD, -
	CMDSTK, -
	CMPPROT, -
	CRT, -
	DCLAREA, -
	DCLARG, -
	DCLCHK, -
	DCLINX, -
	DCLOPT, -
	DCLWILD, -
	DDS, -
	DEBRIEF, -
	DIRARG, -
	DIRCHK, -
	DIRCMD, -
	DIRDATA, -
	DIRENT, -
	DIRFIND, -
	DIRHIGH, -
	DIROPEN, -
	DIRPATH, -
	DIRPROT, -
	DIRREAD, -
	DIRSEEK, -
	DOMORE, -
	DSPC, -
	EDTCMD, -
	FLCOLS, -
	FLCOPY, -
	FLCREA, -
	FLDLET, -
	FLDUMP, -
	FLEDIT, -
	FLESCP, -
	FLFIND, -
	FLMORE, -
	FLNOID, -
	FLPAGE, -
	FLPROT, -
	FLREAD, -
	FLRNAM, -
	FLSCAN, -
	FLSHOW, -
	FLSORT, -
	FREELIST, -
	GETPAD, -
	GETPROT, -
	GETRAW, -
	HELP, -
	HIGHVER, -
	INSPECT, -
	ISOWNER, -
	NAMEHEAP, -
	PATHDOWN, -
	PATHUP, -
	PUTRAW, -
	RABRFA, -
	RMSCC, -
	RMSINIT, -
	RMSIO, -
	RMSLOOK, -
	SCANINT, -
	SCANVER, -
	SETCTRL, -
	SETPROT, -
	SHOQUOTA, -
	SNAPSHOT, -
	STRABBR, -
	STRCLIP, -
	STRFORM2, -
	STRLCPY, -
	STRNULL, -
	STRSCALE, -
	STRSKPS, -
	STRTRIM, -
	STRUCPY, -
	STRVCMP, -
	STRVCPY, -
	STRWCMP, -
	SYASCTIM, -
	SYBINTIM, -
	SYDELETE, -
	SYFOM, -
	SYGETMSG, -
	SYGETSYM, -
	SYHOUR, -
	SYRENAME, -
	SYRIGHTS, -
	SYTRNLOG, -
	TABEXP, -
	TERMSIZE, -
	TTRACE, -
	UID2S, -
	VERSION, -
	WHOAMI, -
	XABPROUI

B	= [-.bin]
L	= [-.lib]
DOC	= [-.doc]

DIRS	= -
	[-]bin.dir -
	[-]lib.dir

A	= $(L)FLIST.OLB
H	= $(B)HELPLIB.HLB	# put this in bin-directory for test/install

LIBRARIES = $(H)

PROGRAMS = -
	$(B)FLIST.EXE -
	$(B)BROWSE.EXE

ALL	= $(DIRS) $(LIBRARIES) $(PROGRAMS)

###############################################################################
# Link from regular libraries rather than sharable, to allow the executable
# to be used on different VMS machines.

LIB_DEP	= $(A)($(MODULES))
LIB_ARG	= $(A)/Lib,SYS$LIBRARY:VAXCRTL/Lib,SYS$LIBRARY:VAXCCURSE/Lib

LINKFLAGS = /MAP='F$PARSE("$(MMS$TARGET_NAME)",,,"NAME")/EXEC=$@

###############################################################################

all :
        @ decc = f$search("SYS$SYSTEM:DECC$COMPILER.EXE").nes.""
        @ axp = f$getsyi("HW_MODEL").ge.1024
        @ macro = ""
        @ if axp.or.decc then macro = "/MACRO=("
        @ if decc then macro = macro + "__DECC__=1,"
        @ if axp then macro = macro + "__ALPHA__=1,"
        @ if macro.nes."" then macro = f$extract(0,f$length(macro)-1,macro)+ ")"
        $(MMS)$(MMSQUALIFIERS)'macro' $(B)BROWSE.EXE
        $(MMS)$(MMSQUALIFIERS)'macro' $(B)FLIST.EXE
        $(MMS)$(MMSQUALIFIERS)'macro' $(H)
	@ write sys$output "** made $@"
#
# I've built on an Alpha with CC_OPTIONS set to
#	CC_OPTIONS = "/standard=VAXC /extern_model=common_block" (VAX-C)
#	CC_OPTIONS = /PREFIX_LIBRARY_ENTRIES=ALL_ENTRIES	(DEC-C)
# The latter (DEC-C) gives better type-checking -- T.Dickey
#
.IFDEF __ALPHA__
CC_OPTIONS = /PREFIX_LIBRARY_ENTRIES=ALL_ENTRIES
.ELSE
.IFDEF __DECC__
CC_OPTIONS = /STANDARD=VAXC
.ELSE
CC_OPTIONS =
.ENDIF
.ENDIF

CFLAGS = /Include=([]) /NoList /Show=all /Object=$@ $(CC_OPTIONS)

clean :
	@- if f$search("*.lis") .nes. "" then delete *.lis;*
	@- if f$search("*.map") .nes. "" then delete *.map;*
	@- if f$search("*.obj") .nes. "" then delete *.obj;*
	@ write sys$output "** made $@"

clobber : clean
	@- if f$search("$(L)*.*") .nes. "" then _n $(L)*.*;*
	@- if f$search("$(B)*.*") .nes. "" then _n $(B)*.*;*
	@- if f$search("$(L)*.*") .nes. "" then delete $(L)*.*;*
	@- if f$search("$(B)*.*") .nes. "" then delete $(B)*.*;*
	@ write sys$output "** made $@"

sources : $(SOURCES)
	@ write sys$output "** made $@"

###############################################################################

.first :
	@- _n := set protection=(o:rwed)
	@- _n/default

.last :
	@- if f$search("$(B)*.*") .nes. "" then purge $(B)
	@- if f$search("$(L)*.*") .nes. "" then purge $(L)
	@- if f$search("*.dia") .nes. "" then delete *.dia;*
	@- if f$search("*.lis") .nes. "" then purge  *.lis
	@- if f$search("*.map") .nes. "" then purge  *.map
	@- if f$search("*.obj") .nes. "" then purge  *.obj

###############################################################################
# Just in case there is no top-level script, make target-directories
[-]bin.dir :
	create/directory [-.bin]

[-]lib.dir :
	create/directory [-.lib]

$(B)BROWSE.EXE : BROWSE.OBJ, $(LIB_DEP) $(DIRS)
	LINK $(LINKFLAGS) BROWSE,$(LIB_ARG)

$(B)FLIST.EXE : FL.OBJ, $(LIB_DEP) $(DIRS)
	LINK $(LINKFLAGS) FL,NORMAL/Opt,$(LIB_ARG)

$(H) : $(H)(FLIST,BROWSE)
	@ write sys$output "** made $@"

$(H)(BROWSE) : $(DOC)BROWSE.HLP
	if f$search("$(H)") .eqs. "" then library/create/help $(H)
	library/replac/help $(H) $(DOC)BROWSE

$(H)(FLIST) : $(DOC)FLIST.HLP
	if f$search("$(H)") .eqs. "" then library/create/help $(H)
	library/replac/help $(H) $(DOC)FLIST

###############################################################################
# Nested includes
DIRENT_H	= DIRENT.H NAMES.H
FLIST_H		= FLIST.H BOOL.H CRT.H

# Include-dependencies

$(A)(ACPCOPY) : -
		RMSIO.H -
		BOOL.H -
		ACP.H

$(A)(ACPLOOK) : -
		ACP.H -
		FLIST.H -
		DIRENT.H

$(A)(ARGVDCL) : -
		BOOL.H -
		DCLARG.H

$(A)(CHPROT) : -
		BOOL.H -
		ACP.H

$(A)(CHRCMD) : -
		FLIST.H -
		GETPAD.H -
		DIRCMD2.H

$(A)(CMDSTK) : -
		BOOL.H -
		CRT.H

$(A)(CMPPROT) : -
		BOOL.H -
		GETPROT.H

$(A)(CRT) : -
		BOOL.H -
		CRT.H -
		NAMES.H

$(A)(DCLAREA) : -
		DCLOPT.H

$(A)(DCLARG) : -
		BOOL.H -
		CRT.H -
		DCLARG.H

$(A)(DCLCHK) : -
		BOOL.H -
		CRT.H -
		DCLARG.H

$(A)(DCLINX) : -
		BOOL.H -
		DCLARG.H

$(A)(DCLOPT) : -
		BOOL.H -
		CRT.H -
		DCLARG.H -
		DCLOPT.H

$(A)(DCLWILD) : -
		DCLARG.H

$(A)(DDS) : -
		FLIST.H -
		DDS.H -
		DIRENT.H

$(A)(DIRARG) : -
		FLIST.H -
		DIRENT.H -
		DCLARG.H -
		DIRCMD2.H

$(A)(DIRCHK) : -
		FLIST.H -
		NAMES.H -
		DIRCMD2.H -
		DCLARG.H

$(A)(DIRCMD) : -
		FLIST.H -
		GETPAD.H -
		DCLARG.H -
		DIRCMD2.H -
		DDS.H -
		DIRENT.H

$(A)(DIRDATA) : -
		FLIST.H -
		DIRENT.H

$(A)(DIRENT) : -
		FLIST.H -
		DIRENT.H -
		DCLARG.H

$(A)(DIRFIND) : -
		FLIST.H -
		DIRENT.H -
		DCLARG.H

$(A)(DIRHIGH) : -
		FLIST.H -
		DIRENT.H

$(A)(DIROPEN) : -
		BOOL.H -
		DIRENT.H -
		GETPROT.H

$(A)(DIRPATH) : -
		FLIST.H -
		DIRENT.H

$(A)(DIRPROT) : -
		FLIST.H -
		DCLARG.H -
		GETPROT.H

$(A)(DIRREAD) : -
		FLIST.H -
		DIRENT.H

$(A)(DIRSEEK) : -
		FLIST.H -
		DCLARG.H

$(A)(DOMORE) : -
		BROWSE.C

$(A)(DSPC) : -
		CRT.H -
		GETPAD.H

$(A)(EDTCMD) : -
		BOOL.H -
		CRT.H -
		GETPAD.H

$(A)(FL) : -
		GETPAD.H -
		FLIST.H -
		DDS.H -
		DIRENT.H -
		DCLARG.H -
		DCLOPT.H

$(A)(FLCOLS) : -
		BOOL.H -
		CRT.H -
		DCLARG.H -
		DDS.H -
		DIRENT.H

$(A)(FLCOPY) : -
		FLIST.H -
		DIRENT.H -
		DCLARG.H

$(A)(FLCREA) : -
		FLIST.H -
		DIRENT.H -
		DCLARG.H

$(A)(FLDLET) : -
		FLIST.H -
		DIRENT.H -
		DCLARG.H -
		DIRCMD2.H

$(A)(FLDUMP) : -
		TEXTLINK.H -
		FLIST.H -
		DIRENT.H

$(A)(FLEDIT) : -
		BOOL.H -
		CRT.H -
		DCLARG.H -
		DDS.H -
		DIRCMD2.H -
		DIRENT.H

$(A)(FLESCP) : -
		FLIST.H -
		DCLARG.H

$(A)(FLFIND) : -
		FLIST.H -
		DIRENT.H -
		DCLARG.H

$(A)(FLMORE) : -
		FLIST.H -
		DIRENT.H -
		DCLARG.H

$(A)(FLNOID) : -
		FLIST.H -
		DIRENT.H -
		DCLARG.H

$(A)(FLPAGE) : -
		FLIST.H -
		DIRENT.H -
		DDS.H -
		DCLARG.H

$(A)(FLPROT) : -
		FLIST.H -
		DIRENT.H -
		GETPAD.H -
		DCLARG.H

$(A)(FLREAD) : -
		FLIST.H -
		DCLARG.H -
		DDS.H -
		DIRENT.H

$(A)(FLRNAM) : -
		FLIST.H -
		DIRENT.H -
		DCLARG.H -
		GETPROT.H

$(A)(FLSCAN) : -
		FLIST.H -
		DCLARG.H -
		DIRENT.H

$(A)(FLSHOW) : -
		FLIST.H -
		DIRENT.H -
		DCLARG.H

$(A)(FLSORT) : -
		FLIST.H -
		DIRENT.H -
		DCLARG.H

$(A)(GETPAD) : -
		GETPAD.H

$(A)(GETPROT) : -
		ACP.H -
		BOOL.H -
		GETPROT.H

$(A)(GETRAW) : -
		BOOL.H -
		CRT.H

$(A)(HELP) : -
		LBRDEF.H

$(A)(INSPECT) : -
		BOOL.H

$(A)(ISOWNER) : -
		FLIST.H -
		DIRENT.H

$(A)(DOMORE) -
BROWSE.OBJ : -
		RMSIO.H -
		BOOL.H -
		CRT.H -
		DCLARG.H -
		DCLOPT.H -
		GETPAD.H -
		NAMES.H

$(A)(NAMEHEAP) : -
		TEXTLINK.H

$(A)(PATHDOWN) : -
		BOOL.H

$(A)(PATHUP) : -
		BOOL.H

$(A)(RMSCC) : -
		BOOL.H

$(A)(RMSIO) : -
		BOOL.H

$(A)(RMSLOOK) : -
		FLIST.H -
		DIRENT.H

$(A)(SETPROT) : -
		BOOL.H -
		DCLARG.H

$(A)(SNAPSHOT) : -
		CRT.H

$(A)(STRABBR) : -
		STRUTILS.H

$(A)(STRCLIP) : -
		STRUTILS.H

$(A)(STRLCPY) : -
		STRUTILS.H

$(A)(SYTRNLOG) : -
		BOOL.H

$(A)(TABEXP) : -
		BOOL.H
