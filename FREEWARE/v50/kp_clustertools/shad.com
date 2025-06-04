$!
$! Display shadowset members which are in a shadow merge or shadow copy state,
$! if any.
$!
$! Author: Keith Parris  parris@encompasserve.org or keithparris@yahoo.com
$!   http://encompasserve.org/~parris/ or http://www.geocities.com/keithparris/ 
$!
$	pid = f$getjpi("","PID")
$	dev = f$device("opa0:",,,1)	!Reset search context
$	close/nolog com
$	open/write com shad_temp_'pid'.com
$ loop:
$	dev = f$device("_DSA*",,,1)
$	if dev .eqs. "" then goto eof
$!!! show symbol dev	!!!
$	write com "$ SHOW DEVICE ",dev
$	goto loop
$ eof:
$	close com
$!!! type/page shad_temp_'pid'.com	!!!
$	@shad_temp_'pid'.com/out=shad_temp_'pid'.list
$!!! type/page shad_temp_'pid'.list	!!!
$	search shad_temp_'pid'.list "merged)","copied)"
$	delete	shad_temp_'pid'.com;*,-
		shad_temp_'pid'.list;*
$	exit
