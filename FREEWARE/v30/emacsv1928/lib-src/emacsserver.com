$!'f$verify(0)
$!
$! EMACSSERVER.COM
$!
$! Description:
$!   Program to be started by server.el
$!   Define the symbol emacsserver to point to this file 
$!   before starting emacs
$!     $ emacsserver :== @dev:[dir.sub]emacsserver
$!
$!   All IPC is through the file SYS$LOGIN:.EMACS_SERVER.
$!
$! See also:
$!   EMACSCLIENT.COM
$!
$! Author:
$!   Roland B Roberts, 27 August 1994
$!
$ set noon
$ on control_y then goto abort
$ set message/nofacility/notext/noident/noseverity
$ tty = f$getjpid(0,"TERMINAL") - ":"
$ set process/name="EMSVR ''tty'"
$ set symbol/verb/scope=(noglobal,nolocal)
$ msg = f$env("MESSAGE")
$ ipcfile := sys$login:.emacs_server
$ create 'ipcfile'
$ purge 'ipcfile'
$ open/append/read/share ipc 'ipcfile'
$ loop:
$ wait 0:00:00.33
$ line := 
$ done := 
$ read/prompt=""/time=0 sys$command done
$ if f$edit(done,"TRIM") .nes. "" then -
	write ipc done
$ read/end=loop/nolock ipc line
$ if f$edit(line,"TRIM") .nes. "" then -
	write sys$output f$fao("!AS !/", line)
$ goto loop
$!
$ abort:
$ sts = $status
$ close ipc
$ set message 'msg'
$ exit 'sts'
