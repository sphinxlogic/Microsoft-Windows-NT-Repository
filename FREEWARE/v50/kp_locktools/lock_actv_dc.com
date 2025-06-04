$	old_verify = f$verify('f$trnlnm("LOCK_ACTV_RC$VERIFY")'+0)
$!
$! LOCK_ACTV_DC.COM
$!
$! Summarize lock activity in the cluster.  Just collect data; don't try to
$! make a pretty report at this point (we can defer all that work until later).
$!							K.Parris 12/99
$! Modification history:
$!
$! Author: Keith Parris  parris@encompasserve.org or keithparris@yahoo.com
$! http://www.geocities.com/keithparris/ or http://encompasserve.org/~parris/
$	if_debug = "!"	!Set to "!" to disable debugging code, "" to enable
$	proc = f$environment("procedure")
$	proc_dev = f$parse(proc,,,"DEVICE")
$	proc_dir = proc_dev + f$parse(proc,,,"DIRECTORY")
$	pid = f$getjpi("","PID")
$	temp_file := lock_actv_dc_'pid'_temp.temp_file
$! Gather lock activity counts from all nodes in the cluster
$	close/nolog com
$	open/write com 'temp_file'_com
$	write com "$ MCR SYSMAN SET ENVIRONMENT/CLUSTER"
$	write com "SET TIMEOUT 0:0:10"
$	write com "DO @",proc_dir,"LCKACT.COM"
$	write com "EXIT"
$	write com "$ EXIT"
$	close com
$ 'if_debug' type/page 'temp_file'_com
$	date_time = f$time()	!Grab a timestamp
$	date = f$cvtime(date_time,"COMPARISON","DATE") - "-" - "-"
$	time = f$cvtime(date_time,"COMPARISON","TIME")
$	hhmm = f$element(0,":",time) + f$element(1,":",time)	!HoursMinutes
$	output_file := LOCK_ACTV_'date'_'hhmm'.DAT
$!!	output_dir := DISK$APP3:[KPARRIS.LCKACT]
$	output_dir = proc_dir
$	show symbol date_time	!Time-stamp the output
$	show symbol output_file	!Show output file specification
$	write sys$output "Gathering lock rates from all cluster nodes..."
$	@'temp_file'_com/out='output_dir''output_file'
$	delete 'temp_file'_com;*
$ 'if_debug' type/page 'output_file'
$	show time	!Time-stamp the output again
$!!	lock_actv_dc_output_file :== LOCK_ACTV_'date'_'hhmm'.DAT
$	exit 1+0*f$verify(old_verify)
