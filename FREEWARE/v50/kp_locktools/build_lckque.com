$!
$! Compile the LCKQUE.MAR program
$!                    K.Parris May 2000
$! Author: Keith Parris  parris@encompasserve.org or keithparris@yahoo.com
$! http://www.geocities.com/keithparris/ or http://encompasserve.org/~parris/
$	version = f$getsyi("VERSION") - "."
$	imagename := lckque.exe_'version'
$	if f$getsyi("ARCH_NAME") .eqs. "Alpha"
$	then	!Alpha
$		compile_cmd = "macro/migrate/list=lckque.lis/object=lckque/debug"
$		link_cmd = "LINK/SYSEXE/NODEBUG/MAP/EXE=''imagename' lckque"
$	else	!VAX
$		compile_cmd = "macro/list=lckque.lis/object=lckque/debug"
$		link_cmd = "LINK/NODEBUG/MAP/EXE=''imagename' lckque,SYS$SYSTEM:SYS.STB"
$	endif
$!
$	if version .ges. "V72"
$	then	! V7.2 or later version
$		'compile_cmd' sys$input:+lckque
V72_OR_LATER = 1	; Lock manager data structures in S2 space
$	else	! V7.1-2 or earlier version
$		'compile_cmd' sys$input:+lckque
;;;V72_OR_LATER = 1	; Lock manager data structures in S2 space
$	endif
$	'link_cmd'
$	exit
