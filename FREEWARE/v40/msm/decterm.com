$! Command file to start local processes in MiniSm environment
$! DO NOT use Vue$* commands in MiniSm com files !!
$
$ on error then continue
$
$ disp = f$trnlnm("decw$display")
$ create/terminal/detached/display='disp
$! create/terminal/wait/display='disp
$ exit
