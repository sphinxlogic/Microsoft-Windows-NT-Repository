$! HUNG.COM
$! 	Force gopherd server to exit, logging exit data.
$!	Assumes P1 is process ID of gopherd server
$!
$ if P1 .nes. "" then goto force_it
$ type sys$input
Usage: @hung <procid>

where <procid> is the process id of the hung Gopherd process.

$ exit
$ FORCE_IT:
$  forcex := $gopher_exe:forcex.exe
$  forcex 'P1'
$  exit
