$	was_verify = 'f$verify(0)'
$! PDPKIT.COM
$!
$! Make a kit (dos format)
$!
$	device := "''p1'"
$	density := "''p2'"
$	if f$mode() .nes. "INTERACTIVE" then goto gotparameters
$ 	if device .eqs. "" then inquire device "Magtape device <mf0:>"
$ 	if device .eqs. "" then device := "mf0:"
$ again:
$	if density .eqs. "" then inquire density "Density (800/1600) <1600>"
$	if density .eqs. "" then density := "1600"
$	if density .eqs. "800" .or. density .eqs. "1600" -
	    then goto gotparameters
$	write sys$error "Illegal density ''density', try again"
$	density := ""
$	goto again
$!
$ gotparameters:
$	if device .eqs. "" then device := "mf0:"
$	if density .eqs. "" then density := "1600"
$!
$	set verify
$! You have placed a tape on drive 'device'
$! It will be zero'd. and written at 'density' bits per inch.
$!
$! You are in [.dtlib]
$!
$	allocate 'device'
$	dos := "/do/dns:''density'"
$	mcr flx	'device''dos'/ze
$	mcr flx	'device'[4,1]'dos'=readme.1st/rs/rw
$	mcr flx 'device'[4,1]'dos'=*.doc/rs/-rw
$	mcr flx 'device'[4,1]'dos'=*.rno/rs/-rw
$	mcr flx 'device'[4,1]'dos'=*.bas/rs/-rw
$	mcr flx	'device'[4,1]'dos'=*.com/rs/-rw
$	mcr flx	'device'[4,1]'dos'=*.cmd/rs/-rw
$!
$	set default [.source]
$	mcr flx	'device'[4,2]'dos'=*.*/rs/-rw
$!
$	set default [-.cobol]
$	mcr flx	'device'[4,3]'dos'=*.*/rs/-rw
$!
$	set default [-.cookie]
$	mcr flx	'device'[4,4]'dos'=*.*/rs/-rw
$!
$	set default [-.demo]
$	mcr flx	'device'[4,5]'dos'=*.*/rs/-rw
$!
$	set default [-.rstslb]
$	mcr flx	'device'[4,6]'dos'=*.*/rs/-rw
$!
$	set default [-.rsxlib]
$	mcr flx	'device'[4,7]'dos'=*.*/rs/-rw
$!
$	set default [-]
$	if .not. 'was_verify' then set verify
