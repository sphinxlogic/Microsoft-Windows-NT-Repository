$!
$!	3 parametres: nom du decserver, nom du port, et nom du device
$!
$ ds_node:= decserver_cal-8
$ port_name:= calsec-dbg
$! ds_node:= decserver_cal-8
$! PORT_NAME:= calrdc_printer
$!
$ device_type:= unknown
$ que_name:= dbg	! nom de la queue
$!
$!
$ sav_prv= f$setprv ("SYSNAM,SYSPRV,LOG_IO")
$!
$ if f$type(que_name).eqs."" then QUE_NAME= ds_node
$ prt_name:= 'que_name'$PRINTER
$ que_name:= 'que_name'$QUEUE
$ node = f$getsyi("nodename")
$!
$! Set up local characteristics for the applications devices.
$!
$ lcp:= $latcp
$ if f$getdvi (prt_name,"EXISTS")
$ then
$	stop/reset 'que_name'
$	if f$getdvi (prt_name,"SPL") then set device 'prt_name'/nospool
$ else
$ lcp create port /logical=(table=lnm$system,name='prt_name',mode=exec)
$ endif
$!! lcp set port 'prt_name'/node='ds_node' /port='port_name'
$ lcp set port 'prt_name' /node='ds_node' /service='port_name'
$!
$! This procedure assumes that the remote devices have been mapped to the
$! same LTAx: device on each node that accesses them.
$!
$ SET TERMINAL 'prt_name' /PERM /LOWERCASE /NOBROADCAST -
	/device_type='device_type'/noecho/hostsync/ttsync/altypeahd/nowrap
$ if .not. $status then goto camerde
$!
$! Set the protection on the devices so that only the symbiont can access
$! them.
$!
$ SET PROTECTION=(S:RWLP,O,G,W) /DEVICE 'prt_name'
$!
$! Set the devices spooled.
$!
$ SET DEVICE 'prt_name' /SPOOLED=(QUE_NAME,SYS$SYSDEVICE)
$!
$! Initialize the remote printer queues.
$! The following assumes that the queue manager has been started.
$!
$ set noon
$ INITIALIZE/QUEUE /PROCESSOR=LASERDBGSMB /RETAIN=ERROR -
  /DEFAULT=(NOBURST,FLAG,TRAILER,NOFEED,FORM=HEADERS) /RECORD_BLOCKING -
  /ON='prt_name' /START 'QUE_NAME'_'NODE' /FORM=HEADERS -
  /library=LASERDBG$DEVCTL /SEPARATE=(NOBURST,FLAG,TRAILER)
$ define/system 'que_name' 'que_name'_'node'
$!
$ sav_prv= f$setprv (sav_prv)
$ exit
$camerde:
$ set noon
$ stop/next 'que_name'
$ set device/nospooled 'prt_name'
$ lcp delete port 'prt_name'
$ exit
