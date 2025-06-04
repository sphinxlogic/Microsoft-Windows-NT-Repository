$!
$! BUILD_FINDMASTER.COM
$!	Build FINDMASTER program
$!
$! Author: Keith Parris  parris@encompasserve.org or keithparris@yahoo.com
$! http://www.geocities.com/keithparris/ or http://encompasserve.org/~parris/
$	if f$getsyi("ARCH_NAME") .eqs. "Alpha"
$	then	!Alpha
$		macro/migrate/list/object/debug findmaster
$	else	!VAX
$		macro/list/object/debug findmaster
$	endif
$	link findmaster
$	exit
