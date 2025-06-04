$!
$ dt:= 1::	! 1 login hour => accounting reset
$!
$ savdef= f$environment ("default")
$ on warning then goto fini
$ on control_y then goto fini
$ proc= f$environment ("procedure")
$ set default 'f$parse (proc,,,"device")''f$parse(proc,,,"directory")'
$!
$ resetacc:= $SYS$DISK:[.'f$getsyi("arch_name")']acc_ckpt$main
$next_pass:
$ctx=""
$npid:
$pid= f$pid(ctx)
$	if pid.eqs."" then goto wait
$	lt= f$getjpi (pid, "logintim")
$	if f$cvtime ("''lt'+''dt'").les.f$cvtime() 
$	then	show sym pid
$		if f$cvtime(lt).ges.f$cvtime(f$getsyi ("boottime")) then -
			resetacc 'pid'	! avoid SWAPPER
$	endif
$	goto npid
$wait:
$	wait 'dt'
$	goto next_pass
$fini:	set noon
$	set default 'savdef'
