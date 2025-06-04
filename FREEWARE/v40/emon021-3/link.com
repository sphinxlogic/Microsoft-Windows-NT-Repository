$!
$!	this file is for use only by the BINARY kit
$!	for the full kit, use @link_no_mms
$!
$	if f$search ("SYS$SHARE:DECC$SHR.EXE").eqs.""
$	then	write sys$output "cannot link: DECC$SHR.EXE not present"
$		exit
$	endif
$!
$	arch= f$getsyi ("arch_name")
$	if arch.eqs."VAX"
$	then	sysexe:= ,sys$system:sys.stb/selective_search
$		libobj:= emon,vector
$	else	sysexe:= /SYsexe
$		libobj:= emon
$	endif
$!
$	link	/share=emon.exe emon_'arch'/library/include=('libobj'),-
		emon.opt/option,emon_'arch'.opt/option 'sysexe'
$!
