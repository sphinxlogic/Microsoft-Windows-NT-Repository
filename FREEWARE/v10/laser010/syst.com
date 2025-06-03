$!
$!	LASER configuration and startup file for a directly connected printer.
$!	Change T??? to whatever port you connect your LaserWriter to.
$!	see startqueue$laser.com for more complete setups and LAT
$!
$	define/system/exec/trans=(concealed,terminal) alw T???: ! laser
$	set term /perm /speed=9600 /nomodem /notype /nohang alw
$	set term /perm /hostsync /pasthru /ttsync /eightbit /nobroad alw
$	set term /perm /noauto /dma alw
$	set prot=o:rwlp /dev alw
$	set device /spooled=(ALW,sys$sysdevice:) alw
$!
$	define /form POST 2 /stock=default
$	define /form ZETA 3 /stock=default /setup=zeta
$	define /form LETTER 4 /stock=default /setup=headers
$	define /form LANDSCAPE 6 /stock=default /setup=headers
$	define /form HEADERS 7 /stock=default /setup=headers
$	define /form BITIMAGE 9 /stock=default /setup=bitimage_hex
$	define /form PAINT 10 /stock=default /setup=paint_hex
$	define /form 2UP 11 /stock=default /setup=headers
$!
$	define/system LASER$LOG sys$sysdevice:[laser]
$	define/system LASER$DATABASE sys$system:laser.cfg
$	define/system LASER$ACCOUNTING sys$system:laser_counters.dat
$!
$	delete /before="today-1-00:00:00" LASER$LOG:*.*.*
$!
$!   Note that /default=form=headers is VMS v4.4 and later only.
$!
$	initialize/queue/start -
	/default=(noburst,nofeed,noflag,notrailer,form=headers) -
	/separate=(noburst,noflag,trailer) - 
	/library=laser /base_prio=8 /form=headers -
	/processor=laser /on=alw: alw
