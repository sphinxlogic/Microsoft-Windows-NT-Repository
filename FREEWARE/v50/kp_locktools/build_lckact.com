$!
$! Compile the LCKACT.MAR program
$!
$!                    K.Parris May 2000
$! Author: Keith Parris  parris@encompasserve.org or keithparris@yahoo.com
$! http://www.geocities.com/keithparris/ or http://encompasserve.org/~parris/
$	version = f$getsyi("VERSION") - "."
$	imagename := lckact.exe_'version'
$	if f$getsyi("ARCH_NAME") .eqs. "Alpha"
$	then	!Alpha
$		compile_cmd = "macro/migrate/list/object=lckact/debug"
$		link_cmd = "LINK/SYSEXE/NODEBUG/MAP/EXE=''imagename' LCKACT"
$	else	!VAX
$		compile_cmd = "macro/list/object=lckact/debug"
$		link_cmd = "LINK/NODEBUG/MAP/EXE=''imagename' LCKACT,SYS$SYSTEM:SYS.STB"
$	endif
$!
$	if version .ges. "V72"
$	then	! V7.2 or later version
$		'compile_cmd' sys$input:+lckact
V72_OR_LATER = 1	; Lock manager data structures in S2 space
$	else	! V7.1-2 or earlier version
$		'compile_cmd' sys$input:+lckact
;;;V72_OR_LATER = 1	; Lock manager data structures in S2 space
$	endif
$	'link_cmd'
$	exit
