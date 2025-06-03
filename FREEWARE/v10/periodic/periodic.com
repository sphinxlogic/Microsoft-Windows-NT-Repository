$	v = 'f$verify(0)'
$!
$! PERIODIC.COM - do periodic (daily, weekly, whatever) batch tasks.
$!
$! Dave Porter  15-Apr-1985  (Smaug::Porter)
$! V2.0		28-Sep-1986
$!
$!		  To kick this off, the easiest thing to do is to
$!		  run it interactively: @PERIODIC any-old-command
$!
$!		  Change the 'schedule' assignments to change the 
$!		  frequency or times of the run; insert '1' under 
$!		  the hours at which the job is to run, spaces elsewhere.
$!		  Fairly obviously, each 'schedule' variable is named
$!		  for the day to which it applies.
$!
$!                            000000000011111111112222
$!                            012345678901234567890123
$	schedule_sunday    = "                        "
$	schedule_monday    = "        1               "
$	schedule_tuesday   = "        1               "
$	schedule_wednesday = "        1               "
$	schedule_thursday  = "        1               "
$	schedule_friday    = "        1               "
$	schedule_saturday  = "                        "
$
$	set noon
$!
$! Figure out our command string
$!
$	s    = " "
$	cmd  = p1 + s + p2 + s + p3 + s + p4 + s + p5 + s + p6 + s + p7 + s + p8
$	cmd  = f$edit(cmd,"compress,trim")
$	if cmd .eqs. "" then exit %x380b2 ! %CLI-E-NOCMD
$!
$! Housekeeping: figure out procedure name, log file name, job name
$!
$	proc = f$environment("procedure")
$	proc = f$extract(0,f$locate(";",proc),proc)
$	job  = f$element(0," ",cmd) - "@"
$	job  = f$element(0,"/",job)
$  	job  = f$parse(job,,,"name")
$	if job .eqs. "" then job = f$parse(proc,,,"name")
$ 	job  = f$extract(0,15,job)
$	log  = f$parse("sys$login:.log",,job)
$	log  = f$extract(0,f$locate(";",log),log)
$!
$! If this is not batch mode, then submit ourselves right away
$!
$  	if f$mode() .eqs. "BATCH" then goto batch
$  	submit 'proc' /name="''job'" /noprint /notify /param="''cmd'"
$  	goto end
$!
$! Batch mode processing
$!
$batch:	show time
$	set process/name="''job'"
$!
$! Build string containing schedule for coming week (today plus next 7 days)
$!
$	t  = f$cvtime("today","absolute")
$	s  = ""
$	dn = 0
$bld:	ds = f$cvtime("''t'+''dn'-00:00",,"weekday")
$	s  = s + schedule_'ds'
$	dn = dn + 1
$	if dn .le. 7 then goto bld
$	s  = s + "1"	! safety net
$!
$! Determine date and time of next run (the 10 minutes is a fudge
$! factor to prevent the next run from being too soon after this one,
$! in case we run "near the end of the hour" somehow)
$!
$	t  = f$cvtime("+0-00:10","absolute")
$	hr = f$cvtime(t,"absolute","hour")
$ffo:	hr = hr + 1
$	if .not. f$extract(hr,1,s) then goto ffo
$	next = f$cvtime(t,"absolute","date") + -
	       f$fao("+!SL-!2SL:!2SL",hr/24,hr-(hr/24)*24,0)
$	next = f$cvtime(next,"absolute")
$!
$! Submit next run, purge logs
$!
$  	submit 'proc' /name="''job'" /noprint /notify /after="''next'" /param="''cmd'"
$  	purge 'log' /keep=2
$!
$! Now do whatever it is that we're really paid to do
$!
$	vv = f$verify(1)
$ 'cmd'
$	vv = 'f$verify(0)'
$
$!
$! End of PERIODIC.COM
$!
$end:	exit $status+(0*f$verify(v))
