$ ! Procedure:	SYS_STOP_ROGUE_SYMBIONT.COM
$ __vfy = "VFY_''f$parse(f$environment("procedure"),,,"name")'"
$ if (f$type('__vfy') .eqs. "") then __vfy = 0
$ __vfy_saved = f$verify(&__vfy)
$ set noon
$ say = "write sys$output"
$ symbiont_number = f$edit(P1, "upcase,collapse")
$ node = f$edit(P2, "upcase,collapse")
$ display_only = (f$edit(P3, "upcase,collapse") .nes. "")
$ if ((symbiont_number .eqs. "") .or. (node .eqs. "")) then goto reqparmis
$ scsnode = f$edit(f$getsyi("nodename"), "upcase,collapse")
$ if (node .nes. scsnode) then goto wrongnode
$ name_fao = "SYMBIONT_!UL"
$ symbiont_number = f$integer(symbiont_number)
$ symbiont_name = f$fao(name_fao, symbiont_number)
$ seek_symbiont = "$dbsexe:zen seek users" -
		+ "/process=''symbiont_name'/noheader/notrailer/space"
$ say " "
$ seek_symbiont
$! reset_queue = "@dbslibrary:sys_control_queues reset symbiont"
$! restart_queue = "@dbslibrary:sys_control_queues start symbiont"
$ reset_queue = "gosub stop_symbiont_queue"
$ restart_queue = "gosub start_symbiont_queue"
$ found_symbiont = "False"
$ context = ""
$pid_loop:
$ wildpid = f$pid(context)
$ if (wildpid .eqs. "") then goto end_pid_loop
$ prcnam = f$getjpi(wildpid, "prcnam")
$ if (prcnam .nes. symbiont_name) then goto pid_loop
$ symbiont_pid = f$getjpi(wildpid, "pid")
$ found_symbiont = "True"
$end_pid_loop:
$ if (.not. found_symbiont) then goto disappeared
$ cancel_it = f$getqui("cancel_operation")
$ object = "*"
$ qui_flags = "printer,terminal,wildcard"
$ freeze = qui_flags + ",freeze_context"
$ @dbslibrary:sys_delete_globals zapped_queue_*
$ que_count = 0
$que_loop:
$ queue_name = f$getqui("display_queue","queue_name",object,qui_flags)
$ if (queue_name .eqs. "") then goto end_que_loop
$ device_name = f$getqui("display_queue","device_name",queue_name,freeze)
$ device_name = f$edit(device_name, "collapse")
$ if (device_name .eqs. "") then goto que_loop
$ if (.not. f$getdvi(device_name, "exists")) then goto que_loop
$ device_pid = f$getdvi(device_name, "pid")
$ if (device_pid .nes. symbiont_pid) then goto que_loop
$ que_count = que_count + 1
$ zapped_queue_'que_count' == queue_name
$ if (display_only)
$   then
$   say "%STOPSYMB-I-CANDIDATE, queue ''queue_name' owned by selected symbiont"
$   delete/symbol/global zapped_queue_'que_count'
$ else
$ reset_queue
$ queue_stopped = f$getqui("display_queue","queue_stopped",queue_name,freeze)
$ if (.not. queue_stopped)
$   then
$   wait 00:00:03.00
$   reset_queue
$ endif !(.not. queue_stopped)
$ endif !(display_only)
$ goto que_loop
$end_que_loop:
$ if (display_only) then goto bail_out
$ say " "
$ say "I will now see if the SYMBIONT is still there, if it is you will have"
$ say "to investigate the problem manually.  Either way I will restart the"
$ say "queues that were stopped up to this point."
$ say " "
$ seek_symbiont/trailer
$ say " "
$ que_count = 0
$start_loop:
$ que_count = que_count + 1
$ if (f$type(zapped_queue_'que_count') .eqs. "") then goto end_start_loop
$ que_to_start = zapped_queue_'que_count'
$ restart_queue
$ goto start_loop
$end_start_loop:
$ goto bail_out
$reqparmis:
$ say "%STOPSYMB-E-REQPARMIS, a required parameter is missing"
$ goto bail_out
$wrongnode:
$ say "%STOPSYMB-E-WRONGNODE, please connect to the node ''node'"
$ goto bail_out
$disappeared:
$ say "%STOPSYMB-E-DISAPPEARED, target symbiont vanished during search"
$ goto bail_out
$bail_out:
$ say " "
$ cancel_it = f$getqui("cancel_operation")
$ !'f$verify(__vfy_saved)'
$ exitt
$
$stop_symbiont_queue:
$ say "%STOPSYMB-I-STOPPING, stopping queue ''queue_name'"
$ stop/queue/reset 'queue_name'
$!$ if (f$getqui("display_queue", "queue_terminal", queue_name, freeze) -
$!	.or. f$getqui("display_queue", "queue_printer", queue_name, freeze))
$!$   then
$!$   host_node = f$getqui("display_queue", "scsnode_name", queue_name, freeze)
$!$   queue_device = f$getqui("display_queue", "device_name", queue_name, freeze)
$!$   if (host_node .eqs. scsnode) then -
$!$     if (f$getdvi(queue_device, "spl")) then -
$!$       set device 'queue_device'/nospool
$!$ endif
$ return
$
$start_symbiont_queue:
$ say "%STOPSYMB-I-STARTING, starting queue ''que_to_start'"
$!$ if (f$getqui("display_queue", "queue_terminal", que_to_start,) -
$!	.or. f$getqui("display_queue", "queue_printer", que_to_start,))
$!$   then
$!$   host_node = f$getqui("display_queue", "scsnode_name", que_to_start,)
$!$   queue_device = f$getqui("display_queue", "device_name", que_to_start,)
$!$   if (host_node .eqs. scsnode) then -
$!$     if (.not. f$getdvi(queue_device, "spl")) then -
$!$	set device 'queue_device'/spool=('que_to_start',disk$scratch1)
$!$ endif
$ start/queue 'que_to_start'
$ return
$ !+==========================================================================
$ !
$ ! Procedure:	SYS_STOP_ROGUE_SYMBIONT.COM
$ !
$ ! Purpose:	To stop a rogue symbiont (one that is in a constant compute
$ !		or current state because of some problem) in an orderly
$ !		fashion.
$ !
$ ! Parameters:
$ !	 P1	The number of the symbiont.
$ !	 P2	The node on which the symbiont resides.  This must be the
$ !		current node in order to work.
$ !	[P3]	If this parameter is not null, a list of queues will be
$ !		displayed and nothing will be stopped.
$ !
$ ! History:
$ !		02-Nov-1990, DBS; Version V1-001
$ !	001 -	Original version.
$ !		11-Mar-1991, DBS; Version V1-002
$ !	002 -	Added check for the existence of a device before trying to do
$ !		things to it.
$ !		14-Aug-1991, DBS; Version V1-003
$ !	003 -	Fixed typo in symbol of queue name when generating starting
$ !		messages.
$ !		28-Jul-1992, DBS; Version V1-004
$ !	004 -	Changed format of symbiont process name for V5.5.
$ !-==========================================================================
