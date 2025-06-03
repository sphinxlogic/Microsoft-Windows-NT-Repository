$link laser/opt/nodebug/notrace/map/full
$link/deb/exe=laserref laser/opt
$ open/read opt laser.opt
$ open/write tmp laser.tmp
$ read opt line1
$ line1= f$element (0,"""", line1) + """" + f$element (0,"-",f$element(1,"""",line1)) + "-" + f$parse (f$search("laser.opt"),,,"VERSION") - ";" + """"
$ write tmp line1
$bor:	read/end=eor opt line
$	write tmp line
$	goto bor
$eor:	close opt
$	close tmp
$purge laser.tmp
$rename laser.tmp .opt
$ set noon
$stop/reset dbg$queue
$on control_y then continue
$sav_prv= f$setprv ("SYSPRV")
$copy laser.exe sys$common:[sysexe]laserdbgsmb
$purge/log sys$system:laserdbgsmb.exe
$delete sys$system:laser.dmp;*/log
$sav_prv= f$setprv (sav_prv)
$start dbg$queue
$ !
$	x = f$getqui ("CANCEL_OPERATION")
$ nq:	qname= f$getqui ("DISPLAY_QUEUE","QUEUE_NAME","*","TERMINAL")
$	if qname.eqs."" then goto exit
$	proc = f$getqui ("DISPLAY_QUEUE","PROCESSOR","*", "TERMINAL,FREEZE_CONTEXT")
$	if proc.nes."LASERDBGSMB" then goto nq
$	node = f$getqui ("DISPLAY_QUEUE","SCSNODE_NAME","*", "TERMINAL,FREEZE_CONTEXT")
$	if f$getsyi ("NODENAME").eqs."BRAHMS"
$	then	if node.nes."BRAHMS" then goto nq
$	else 	if (node.nes."LISZT").and.(node.nes."CHOPIN") then goto nq
$	endif
$	sh sym qname
$	stop/queue/next 'qname'
$	i= 10
$ wq:	stopped= f$getqui ("DISPLAY_QUEUE","QUEUE_STOPPED","*", "TERMINAL,FREEZE_CONTEXT")
$	stopping= f$getqui ("DISPLAY_QUEUE","QUEUE_STOPPING","*", "TERMINAL,FREEZE_CONTEXT")
$	stopped = stopped .and. .not. stopping
$	if stopped .or. (i.eq.0)
$	then	if i.eq.0 then wo "Queue ''qname' failed to stop, cancelling stop request"
$		start/queue 'qname'
$	else	wo "Waiting for queue ''qname' to stop"
$		if i.eq.10
$		then 	wait 0::10
$		else 	wait 0:1:0
$		endif
$		i= i-1
$		goto wq
$	endif
$	goto nq
$ exit:
$	x = f$getqui ("CANCEL_OPERATION")
$
