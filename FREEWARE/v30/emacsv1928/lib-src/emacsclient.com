$!'f$verify(0)
$!
$! EMACSCLIENT.COM
$!
$! Usage:
$!   emacsclient [ +LINE ] FILE [ +LINE FILE ... ]
$!
$! Description:
$!   Allows use of Emacs as an edit server for other processes.
$!   All IPC is through the file SYS$LOGIN:.EMACS_SERVER.
$!   Requests are written in the form
$!     Client: ID [+LINE] FILE [+LINE] FILE ...
$!   When Emacs finishes editting the file, a message is written as
$!     Close: ID Done
$!   Since multiple clients may connect to Emacs, emacsclient checks
$!   the ID to verify that it is really finished.
$!
$!   NOTE: Emacs seems to handle incoming requests in a stack-like 
$!   fashion, last-in first-out.
$!
$! See also:
$!   EMACSSERVER.COM
$!
$! Author:
$!   Roland B Roberts, 27 August 1994
$!
$ on control_y then goto abort
$ set symbol/verb/scope=(noglobal,nolocal)
$ ipcfile := sys$login:.emacs_server
$ if f$search(ipcfile) .eqs. ""
$ then
$   write sys$error "Emacs server does not appear to be started"
$   exit
$ endif
$ open/append/share ipc 'ipcfile'
$!
$! Build a list of LINENUMBER FILENAME pairs
$!
$ arglist = "''p1' ''p2' ''p3' ''p4' ''p5' ''p6' ''p7' ''p8'"
$ arglist = f$edit(arglist,"TRIM,COMPRESS")
$ i = 0
$ line := 
$ argloop:
$ arg = f$ele(i," ",arglist)
$ if arg .nes. " "
$ then
$   if f$ext(0,1,arg) .eqs. "+" 
$   then line = line + " " + arg
$   else
$     file = f$search(arg)
$     if file .nes. "" 
$     then line = line + " " + f$ext(0,f$loc(";",file),file)
$     else line = line + " " + f$env("DEFAULT") + arg
$     endif
$   endif
$   i = i + 1
$   goto argloop
$ endif
$ line = f$edit(line,"TRIM,COMPRESS")
$!
$! Format a unique request id
$!
$ now = f$edit(f$time(),"TRIM")
$ loop1:
$ x = f$loc(" ",now)
$ if x .lt. f$len(now)
$ then
$   now[x,1] := "+"
$   goto loop1
$ endif
$ reqid = f$fao("!AS<!AS>", f$getjpi(0,"PID"), now)
$!
$! Write out the request
$!
$ write ipc f$fao("Client: !AS !AS ", reqid, line)
$ line := 
$ done  = "Close: " + reqid + " Done"
$!
$! Wait for our completion message
$!
$ line :=
$ loop2: 
$ wait 0:00:00.33
$ read/end=loop2/nolock ipc line
$ if f$edit(line,"TRIM") .nes. "" 
$ then
$   if line .eqs. done
$   then
$     close ipc
$     exit
$   endif
$   if line .eqs. "Abort"
$   then
$     close ipc
$     write sys$error "Emacs server aborted!"
$     exit
$   endif
$ endif
$ goto loop2
$ abort:
$ close ipc
$ exit
