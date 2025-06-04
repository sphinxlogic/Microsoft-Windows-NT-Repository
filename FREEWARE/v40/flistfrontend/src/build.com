$! $Id: build.com,v 1.12 1995/10/27 11:42:06 tom Exp $
$! VAX/VMS DCL build script for FLIST and BROWSE
$!
$! Tested with:
$!	VMS system version 5.4-2 (VAX)
$!	VAX-C version 3.2
$! and
$!	OpenVMS version 6.1 (AXP)
$!	DEC-C 4.1
$
$	VERIFY = F$VERIFY(0)
$	set := set
$	set symbol/scope=(nolocal,noglobal)
$	mylib := [-.lib]flist.olb
$
$	axp  = f$getsyi("HW_MODEL").ge.1024
$	OPTS = ""
$
$ if axp
$ then
$! assume we have DEC C
$!	CFLAGS = "/standard=VAXC /extern_model=common_block"
$	CFLAGS = "/prefix_library_entries=all_entries"
$ else
$! we have either VAX C or DEC C
$	CFLAGS = ""
$	OPTS = ",VMSSHARE.OPT/OPTIONS"
$	if f$search("SYS$SYSTEM:VAXC.EXE").eqs.""
$	then
$		CFLAGS = "/prefix_library_entries=all_entries"
$	endif
$ endif
$
$ if "''p1'" .eqs. "" .or. "''p1'" .eqs. "ALL"
$ then
$	call	mkdir bin
$	call	mkdir lib
$
$	call	cc_lib ACPCOPY
$	call	cc_lib ACPLOOK
$	call	cc_lib ALARM
$	call	cc_lib ARGVDCL
$	call	cc_lib CANOPEN
$	call	cc_lib CHPROT
$	call	cc_lib CHRCMD
$	call	cc_lib CMDSTK
$	call	cc_lib CMPPROT
$	call	cc_lib CRT
$	call	cc_lib DCLAREA
$	call	cc_lib DCLARG
$	call	cc_lib DCLCHK
$	call	cc_lib DCLINX
$	call	cc_lib DCLOPT
$	call	cc_lib DCLWILD
$	call	cc_lib DDS
$	call	cc_lib DEBRIEF
$	call	cc_lib DIRARG
$	call	cc_lib DIRCHK
$	call	cc_lib DIRCMD
$	call	cc_lib DIRDATA
$	call	cc_lib DIRENT
$	call	cc_lib DIRFIND
$	call	cc_lib DIRHIGH
$	call	cc_lib DIROPEN
$	call	cc_lib DIRPATH
$	call	cc_lib DIRPROT
$	call	cc_lib DIRREAD
$	call	cc_lib DIRSEEK
$	call	cc_lib DOMORE
$	call	cc_lib DSPC
$	call	cc_lib EDTCMD
$	call	cc_lib FLCOLS
$	call	cc_lib FLCOPY
$	call	cc_lib FLCREA
$	call	cc_lib FLDLET
$	call	cc_lib FLDUMP
$	call	cc_lib FLEDIT
$	call	cc_lib FLESCP
$	call	cc_lib FLFIND
$	call	cc_lib FLMORE
$	call	cc_lib FLNOID
$	call	cc_lib FLPAGE
$	call	cc_lib FLPROT
$	call	cc_lib FLREAD
$	call	cc_lib FLRNAM
$	call	cc_lib FLSCAN
$	call	cc_lib FLSHOW
$	call	cc_lib FLSORT
$	call	cc_lib FREELIST
$	call	cc_lib GETPAD
$	call	cc_lib GETPROT
$	call	cc_lib GETRAW
$	call	cc_lib HELP
$	call	cc_lib HIGHVER
$	call	cc_lib INSPECT
$	call	cc_lib ISOWNER
$	call	cc_lib NAMEHEAP
$	call	cc_lib PATHDOWN
$	call	cc_lib PATHUP
$	call	cc_lib PUTRAW
$	call	cc_lib RABRFA
$	call	cc_lib RMSCC
$	call	cc_lib RMSINIT
$	call	cc_lib RMSIO
$	call	cc_lib RMSLOOK
$	call	cc_lib SCANINT
$	call	cc_lib SCANVER
$	call	cc_lib SETCTRL
$	call	cc_lib SETPROT
$	call	cc_lib SHOQUOTA
$	call	cc_lib SNAPSHOT
$	call	cc_lib STRABBR
$	call	cc_lib STRCLIP
$	call	cc_lib STRFORM2
$	call	cc_lib STRLCPY
$	call	cc_lib STRNULL
$	call	cc_lib STRSCALE
$	call	cc_lib STRSKPS
$	call	cc_lib STRTRIM
$	call	cc_lib STRUCPY
$	call	cc_lib STRVCMP
$	call	cc_lib STRVCPY
$	call	cc_lib STRWCMP
$	call	cc_lib SYASCTIM
$	call	cc_lib SYBINTIM
$	call	cc_lib SYDELETE
$	call	cc_lib SYFOM
$	call	cc_lib SYGETMSG
$	call	cc_lib SYGETSYM
$	call	cc_lib SYHOUR
$	call	cc_lib SYRENAME
$	call	cc_lib SYRIGHTS
$	call	cc_lib SYTRNLOG
$	call	cc_lib TABEXP
$	call	cc_lib TERMSIZE
$	call	cc_lib TTRACE
$	call	cc_lib UID2S
$	call	cc_lib VERSION
$	call	cc_lib WHOAMI
$	call	cc_lib XABPROUI
$
$	call	compile FL
$	call	compile BROWSE
$
$	call	build FLIST FL
$	call	build BROWSE BROWSE
$
$	if f$search("CRM.*") .nes. ""
$	then
$		call cc_lib  CRS
$		call cc_lib  PIPEFUNC
$		call compile CRM
$		call build   SORTX CRM
$	endif
$
$	if f$search("[-.bin]helplib.hlb") .eqs. ""
$	then
$		write sys$output "** making help-library"
$		library/create/help [-.bin]helplib
$		library/replac/help [-.bin]helplib [-.doc]flist,browse
$	endif
$	write sys$output "** made ALL"
$ endif
$
$ if "''p1'" .eqs. "CLEAN"
$ then
$	if f$search("*.lis") .nes. "" then delete *.lis;*
$	if f$search("*.map") .nes. "" then delete *.map;*
$	if f$search("*.obj") .nes. "" then delete *.obj;*
$	write sys$output "** made CLEAN"
$ endif
$
$ if "''p1'" .eqs. "CLOBBER"
$ then
$	@ 'f$environment("procedure") CLEAN
$	if f$search("[-.lib]*.*") .nes. "" then delete [-.lib]*.*;*
$	if f$search("[-.bin]*.*") .nes. "" then delete [-.bin]*.*;*
$	write sys$output "** made CLOBBER"
$ endif
$
$ VERIFY = F$VERIFY(VERIFY)
$ exit
$
$
$ cc_lib: subroutine
$	if f$search("''p1'.obj") .eqs. ""
$	then
$		call compile 'p1
$		call mklib 'mylib
$		library/replace 'mylib 'p1.obj
$	endif
$ endsubroutine
$
$
$ compile: subroutine
$	if f$search("''p1'.obj") .eqs. ""
$	then
$		write sys$output "** compiling ''p1'"
$		cc 'CFLAGS /Show=all /NoListing /Include=([]) 'p1
$	endif
$ endsubroutine
$
$
$ build: subroutine
$	target = "[-.bin]''p1'.exe"
$	if f$search("''target'") .eqs. ""
$	then
$		write sys$output "** linking ''p1'"
$		link/map='p1/exec='target 'p2,normal/opt,'mylib/lib,SYS$LIBRARY:VAXCRTL/Lib,SYS$LIBRARY:VAXCCURSE/Lib
$	endif
$ endsubroutine
$
$
$ mklib: subroutine
$	if f$search("''p1'") .eqs. "" then library/create'p2 'p1
$ endsubroutine
$
$
$ mkdir: subroutine
$	if f$search("[-]''p1'.dir") .eqs. "" then create/directory [-.'p1]
$ endsubroutine
