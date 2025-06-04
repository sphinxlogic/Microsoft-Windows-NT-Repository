$! FREEWARE_DEMO.COM	Demonstrate PQM
$!
$! Created 28-Dec-2000 by J.Begg, VSM Software Services Pty Ltd.
$!
$	old_default = f$environment("DEFAULT")
$	proc = f$environment("PROCEDURE")
$	procdir = f$parse(proc,,,"DEVICE") + f$parse(proc,,,"DIRECTORY")
$	on control_y then $ goto EXIT
$
$	set default 'procdir'
$	pqm_exe = f$search("PQM.''f$getsyi("ARCH_NAME")'_EXE")
$	if pqm_exe .eqs. ""
$	then
$	    write sys$output "%PQM-F-NOEXE, unable to locate PQM image for ", f$getsyi("ARCH_NAME")
$	    goto EXIT
$	endif
$	
$	pqm := $'pqm_exe'/browser="EDITX/TPU/NOINIT/NOCOMMAND/READ/NOCREATE/NOJOURNAL/NOOUTPUT"
$	define sys$input sys$command:
$	pqm
$
$EXIT:
$	set noon
$	set default 'old_default'
$	exit
