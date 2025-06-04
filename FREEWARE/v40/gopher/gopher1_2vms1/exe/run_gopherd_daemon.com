$! RUN_GOPHERD_DAEMON.COM
$!
$! 19931129	D.Sherman <dennis_sherman@unc.edu>
$!		changed :== to :=
$! 19931124	D.Sherman <dennis_sherman@unc.edu>
$!		removed /exec, added a sysprv
$! 19931118	D.Sherman <dennis_sherman@unc.edu>
$!		removed sysnam
$! 19931116	D.Sherman <dennis_sherman@unc.edu>
$!		modified and cleaned up a little for 1_2vms1 release
$!
$restart_server:
$ set noon
$ set nover
$ set proc/priv=(noall, sysprv, tmpmbx, netmbx)
$ if f$search("GOPHER_ROOT:[000000]_output70.*") -
	then purge/keep=3 GOPHER_ROOT:[000000]_output70.*
$ if f$search("GOPHER_ROOT:[000000]_error70.*") -
	then purge/keep=3 GOPHER_ROOT:[000000]_error70.*
$ set ver
$ show time
$ run gopher_exe:GopherD.exe
$ sts = $status
$ show time
$ ctrstr = "Gopherd_Daemon exit /!AS/ !AS
$ line = f$fao(ctrstr,f$trnlnm("GOPHER_RESTART_70"),f$message(sts))
$ request "''line'"
$ mail NL: SYSTEM /noself /subj="''line'"
$ if .not. sts
$	then
$	request "Restarting after abort..."
$	goto restart_server
$	endif
$ restart := 'f$trnlnm("GOPHER_RESTART_70")
$ show sym restart
$ if restart .nes. ""
$	then
$	set proc/priv=(noall, sysprv)
$	deassign/system GOPHER_RESTART_70
$	if restart .eqs. "RESTART" then request "Restarting..."
$	if restart .eqs. "RESTART" then goto restart_server
$	endif
