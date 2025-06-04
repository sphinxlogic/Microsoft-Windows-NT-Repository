$!
$! Gather lock activity data
$!
$! Author: Keith Parris  parris@encompasserve.org or keithparris@yahoo.com
$! http://www.geocities.com/keithparris/ or http://encompasserve.org/~parris/
$	proc = f$environment("procedure")
$	proc_dev = f$getdvi(f$parse(proc,,,"DEVICE"),"ALLDEVNAM")
$	proc_dir = proc_dev + f$parse(proc,,,"DIRECTORY")
$! First, look for a VMS-version-specific image
$	version = f$getsyi("VERSION") - "."	!VMS version
$	image = f$search("''proc_dir'LCKACT.EXE_''version'")
$! Failing that, look for a generic image
$	if image .eqs. "" then image = f$search("''proc_dir'LCKACT.EXE")
$	if image .nes. ""
$	then
$! for testing...
$! if f$getsyi("nodename") .eqs. "ETGB16"
$! then	!!!
$!	write sys$output "Simulating a failure of LCKACT.EXE..."
$!	exit	!For testing...	!!!
$! endif	!!!
$		run 'image'
$	else
$		write sys$output "Image LCKACT.EXE* not found."
$	endif
$	exit
