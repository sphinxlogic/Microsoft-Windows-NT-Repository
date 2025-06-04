$!	LINK.COM
$!	V1.1				25-Jan-1995	IfN/Mey
$!+
$! LINK SOCKETSHR.
$!-
$	v=f$verify(0)
$	on control_y then goto exit
$	on error then goto exit
$	set on
$	if f$type(pack) .eqs. ""
$		then pack = ""
$		proc_ver = 0
$	else
$		proc_ver = 1
$	endif
$	if P1 .nes. "" then pack = f$edit(P1,"TRIM,UPCASE")
$       arch = "VAX"
$	if f$getsyi("HW_MODEL") .ge. 1024 then $ arch = "ALPHA"
$	comdir = ""
$	if f$search("LINK.COM") .eqs. "" then comdir = "[-]"
$!
$ ask:
$	if pack .eqs. ""
$	then
$		type sys$input

	There are several implementation for SOCKETSHR:

		NETLIB1	- for NETLIB V1.7
		UCX	- for UCX

	The recommended implementation is NETLIB1.

$	read/prompt="* Which implementation should be built [NETLIB1] ? " -
		/end=exit sys$output pack
$	endif
$	pack = f$edit(pack,"TRIM,UPCASE")
$	if pack .eqs. "" then pack := NETLIB1
$	if pack .eqs. "NETLIB1" then goto link
$	if pack .eqs. "UCX" then goto link
$	write sys$output "''pack' is not a valid implementation."
$	pack = ""
$	goto ask
$ link:
$       HAVEDECC = f$search("sys$system:decc$compiler.exe") .nes. ""
$	DEFAULTC = f$trnlnm("DECC$CC_DEFAULT")
$       if (DEFAULTC .eqs. "/VAXC") .or. .not. HAVEDECC
$         then
$           comp = "VAXC"
$         else
$           comp = "DECC"
$       endif
$	write sys$output ""
$	write sys$output "Linking SOCKETSHR_''pack'.EXE ..."
$!
$	vv=f$verify(proc_ver)
$ LINK /NOTRACE/SHARE=SOCKETSHR_'pack'.EXE /MAP=SOCKETSHR_'pack'.MAP/FULL -
	'comdir'SOCKETSHR_XFR_'arch'.OPT/OPT,-
	[]SOCKETLIB_'pack'_'arch'/LIB,-
	[]SOCKETSHR_'pack'_'arch'_'comp'.OPT/OPT
$ exit:	!'f$verify(v)'
