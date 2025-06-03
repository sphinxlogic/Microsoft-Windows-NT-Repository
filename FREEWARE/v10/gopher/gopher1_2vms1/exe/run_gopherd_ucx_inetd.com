$! v = 'f$verify(0)
$! RUN_GOPHERD_UCX_INETD.COM
$!
$!  1994-07-27  D.Sherman <dennis_sherman@unc.edu>
$!      modified f$search to use sys$login instead of gopher_root,
$!       based on error report from Rich Helmke.
$!  1994-02-25	D.Sherman <dennis_sherman@unc.edu>
$!		original version added to VMSGopher distribution,
$!		based on code from Richard A. Helmke <helmke@cuis.edu>
$ set noon
$ set proc/priv=(noall, tmpmbx, netmbx)
$ if f$search("sys$login:_run_gopherd_ucx_inetd.log") -
	then purge/keep=3 sys$login:run_gopherd_ucx_inetd.log
$ run gopher_exe:GopherD.exe
$ sts = $status
$ ctrstr = "Gopherd_Daemon exit /!AS/ !AS
$ line = f$fao(ctrstr,f$trnlnm("GOPHER_RESTART_70"),f$message(sts))
$ if .not. sts
$   then
$	request "''line'"
$	mail NL: SYSTEM /noself /subj="''line'"
$   endif
