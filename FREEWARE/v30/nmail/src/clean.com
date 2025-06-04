$	set noon
$ 	call del *.obj
$ 	call del *.lis
$	call del nmail*.olb
$ 	call del nm$library*.l32
$ 	call del nm$message.req
$ 	call del nm$message.sdl
$	call del *.doc
$	call del *.hlp
$	call del *.brn
$	call del *.rnt
$	call del ident.*
$	call del *.opt
$	call del *.exe
$	call del *.map
$	call del *.tmp
$ 	exit
$del: 	subr
$	if f$search("''p1';*") .nes. "" then -
$	delete/log 'p1';*
$	endsubr
