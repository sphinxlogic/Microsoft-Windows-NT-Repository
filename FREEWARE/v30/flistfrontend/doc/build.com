$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$! Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                   !
$!                                                                             !
$! You may freely copy or redistribute this software, so long as there is no   !
$! profit made from its use, sale trade or reproduction. You may not change    !
$! this copyright notice, and it must be included in any copy made.            !
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$! $Id: build.com,v 1.3 1995/10/26 22:41:25 tom Exp $
$! VAX/VMS DCL build script for FLIST and BROWSE documentation
$!
$! Tested with:
$!	VMS system version 5.4-2
$!	Dec RUNOFF (V3.2-01)
$
$	VERIFY = F$VERIFY(0)
$	set := set
$	set symbol/scope=(nolocal,noglobal)
$	mylib := [-.lib]flist.olb
$
$ if "''p1'" .eqs. "" .or. "''p1'" .eqs. "ALL"
$ then
$	call document d0 FLIST
$	call document m0 BROWSE
$	call helpfile db FLIST
$	call helpfile ma BROWSE
$ endif
$
$ if "''p1'" .eqs. "CLEAN"
$ then
$	call remove *.brn;*
$	call remove *.rnt;*
$	call remove *.rnx;*
$	write sys$output "** made CLEAN"
$ endif
$
$ if "''p1'" .eqs. "CLOBBER"
$ then
$	@ 'f$environment("procedure") CLEAN
$	call remove *.hlp;*
$	call remove *.mem;*
$	write sys$output "** made CLOBBER"
$ endif
$
$ VERIFY = F$VERIFY(VERIFY)
$ exit
$
$ document: subroutine
$	call remove *.brn;*
$	opts = "/variant=""paged""
$	part = "''p1'-parts"
$	if f$search("''p2'.mem") .eqs. ""
$	then
$		write sys$output "** compiling ''p2' document"
$		runoff'opts/nooutput/intermediate 'part
$		runoff/contents/output='p1 'part
$		runoff/index/output='p1 'part
$		runoff'opts/output='p2.mem 'p1
$		purge 'p2.mem
$		call remove *.brn;*
$		call remove 'p1.rnt;*
$		call remove 'p1.rnx;*
$	endif
$ endsubroutine
$
$ helpfile: subroutine
$	if f$search("''p2'.hlp") .eqs. ""
$	then
$		runoff/variant="lib"/output='p2.hlp 'p1
$	endif
$ endsubroutine
$
$ remove: subroutine
$	if f$search("''p1'") .nes. ""
$	then
$		set file/protection=(owner:rwed) 'p1
$		delete 'p1
$	endif
$ endsubroutine
