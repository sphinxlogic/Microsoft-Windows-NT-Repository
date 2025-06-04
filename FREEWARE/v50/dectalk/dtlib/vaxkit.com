$	was_verify = 'f$verify(0)'
$! VAXKIT.COM
$!
$! Make a vax kit (vax backup format)
$!
$	device := "''p1'"
$	density := "''p2'"
$	if f$mode() .nes. "INTERACTIVE" then goto gotparameters
$ 	if device .eqs. "" then inquire device "Magtape device <mfa0:>"
$ 	if device .eqs. "" then device := "mfa0:"
$ again:
$	if density .eqs. "" then inquire density "Density (1600/6250) <1600>"
$	if density .eqs. "" then density := "1600"
$	if density .eqs. "6250" .or. density .eqs. "1600" -
	    then goto gotparameters
$	write sys$error "Illegal density ''density', try again"
$	density := ""
$	goto again
$!
$ gotparameters:
$	if device .eqs. "" then device := "mfa0:"
$	if density .eqs. "" then density := "1600"
$!
$	set verify
$!
$! You have placed a tape on drive 'device' (The MOUNT command has not
$! been issued). It will be zero'd. and written at 'density' bits per inch.
$!
$! Your current default directory is [.dtlib]
$!
$	allocate 'device'
$	init/density='density' 'device' dtlib
$	mount/foreign 'device'
$	backup [...]*.* 'device'dtlib.bck
$	if .not. 'was_verify' then set verify
