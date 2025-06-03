$ default = f$environment ("PROCEDURE")
$ default = f$parse (default,,, "DEVICE") + f$parse (default,,, "DIRECTORY")
$ set default 'default'
$top:
$	file = f$search ("SYS$MANAGER:BANNER*.%32")
$	if file .eqs. "" then goto try_com
$	diff /par 'file' 'default'
$	goto top
$
$try_com:
$	diff /par sys$manager:bannerbld.com 'default'
$	diff /par sys$manager:bannerbld1.com 'default'
