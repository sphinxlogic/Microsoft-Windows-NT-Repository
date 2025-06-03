$! This command procedure will start the VMS Gopher server as a detached process
$!
$! @start_gopher p1
$!
$! Parameters:
$!    p1 : configuration file
$!
$!    example:
$! @start_gopher gopher_root:[000000]_gopher70
$!
$! Another, better way to run this is to submit it, running under the
$! user=GOPHERD authorization, so that it and thus the detached daemon
$! process assume that id and all its privileges and rights identifiers.
$!
$! SUBMIT/USER=GOPHERD start_gopher
$!
$!
$ If ( p1 .nes. "" )
$  Then
$   Define/system Gopher_Config 'p1
$  Else
$   Define/system Gopher_Config Gopher_Root:[000000]_Gopher70
$ Endif
$!
$ set proc/priv=(detach, sysprv, tmpmbx, netmbx)
$ if f$trnlnm("GOPHER_RESTART_70") .nes. "" then -
	deassign/system GOPHER_RESTART_70
$ run/detach/Proc="Gopher1_Server"-
	/buffer_limit=140000-
	/working_set=500/extent=2500/maximum_working_set=1500-
	/input=  GOPHER_EXE:run_gopherd_daemon.com-
	/output= GOPHER_ROOT:[000000]_output70-
	/error=  GOPHER_ROOT:[000000]_error70 -
	/priv=   (NOSAME, SYSPRV, TMPMBX, NETMBX) -
    Sys$System:Loginout
$!
$ exit
