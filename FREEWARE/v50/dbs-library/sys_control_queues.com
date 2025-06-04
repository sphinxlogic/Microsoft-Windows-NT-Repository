$ ! Procedure:	SYS_CONTROL_QUEUES.COM
$ __vfy = "VFY_''f$parse(f$environment("procedure"),,,"name")'"
$ if (f$type('__vfy') .eqs. "") then __vfy = 0
$ __vfy_saved = f$verify(&__vfy)
$	set noon
$	on control_y then goto bail_out
$  	say = "write sys$output"
$	if (f$trnlnm("disk$scratch1") .eqs. "") then -
$		define/nolog disk$scratch1 sys$sysdevice
$	current_node	= f$getsyi("nodename")
$	QUI$M_QUEUE_IDLE	= 2
$	QUI$M_QUEUE_PAUSED	= 16
$	QUI$M_QUEUE_PAUSING	= 32
$	QUI$M_QUEUE_RESETTING	= 128
$	QUI$M_QUEUE_RESUMING	= 256
$	QUI$M_QUEUE_STALLED	= 1024
$	QUI$M_QUEUE_STARTING	= 2048
$	QUI$M_QUEUE_STOPPED	= 4096
$	QUI$M_QUEUE_STOPPING	= 8192
$	QUI$M_QUEUE_UNAVAILABLE	= 16384
$	QUI$M_QUEUE_CLOSED	= 32768
$	QUI$M_QUEUE_BUSY	= 65536
$	QUI$M_QUEUE_AVAILABLE	= 262144
$	QUI$M_QUEUE_DISABLED	= 524288
$	QUI$M_QUEUE_STOP_PENDING = 2097152
$	object		= "*"
$	flags		= "wildcard"
$	dc$_term	= 66
$	dc$_lp		= 67
$	dc$_mailbox	= 160
$	dt$_null	= 3
$	valid_options	= "\START\STOP\DELETE\SET\RESET" -
				+ "\SHOW\LIST\SETACL\REMACL\"
$	valid_options_s	= f$length(valid_options)
$	valid_que_types = "\BATCH\GENERIC\PRINTER\SERVER\SYMBIONT\TERMINAL\"
$	P1 = f$edit(P1, "collapse, upcase")
$	if ((P1 .eqs. "HELP") .or. (P1 .eqs. "?")) then goto give_help
$	if (f$locate("\''P1'\", valid_options) .eq. valid_options_s) then -
$		goto no_option
$	P2 = f$edit(P2, "collapse, upcase")
$	if (P2 .eqs. "") then goto no_que_type
$	local_only = "False"
$	if (P2 .eqs. f$extract(0, f$length(P2), "LOCAL"))
$		then
$		local_only = "True"
$		P2 = P3
$		P3 = P4
$		P4 = P5
$		P5 = P6
$		P6 = P7
$		P7 = P8
$	endif !(P2 .eqs. f$extract(0, f$length(P2), 'LOCAL'))
$	if (P3 .nes. "") then object = P3
$	if (P1 .eqs. "SET")
$		then
$		set_options = f$edit(P4, "collapse, upcase")
$		if (set_options .eqs. "") then goto no_set_option
$		if (f$extract(0, 1, set_options) .nes. "/") then -
$			set_options = "/" + set_options
$	endif !(P1 .eqs. "SET")
$	if ((P1 .eqs. "SETACL") .or. (P1 .eqs. "REMACL"))
$		then
$		acl_ident = f$edit(P4, "collapse, upcase")
$		if (acl_ident .eqs. "") then goto no_acl_given
$		acl_options = f$edit(P5, "collapse, upcase")
$	endif !((P1 .eqs. "SETACL") .or. (P1 .eqs. "REMACL"))
$	option = P1
$	count  = 0
$type_loop:
$	que_type = f$element(count, ",", P2)
$	if (que_type .eqs. ",") then goto end_type_loop
$	if (f$locate("\''que_type'\", valid_que_types) .eq. -
			f$length(valid_que_types)) then -
$		goto invalid_que_type
$	flags = flags + ",''que_type'"
$	count = count + 1
$ goto type_loop
$end_type_loop:	
$	freeze = flags + ",freeze_context"
$queue_loop:
$	que_name = f$getqui("display_queue", "queue_name", object, flags)
$	if (que_name .eqs. "") then goto end_queue_loop
$	que_stopped = f$getqui("display_queue", "queue_stopped" -
				,que_name, freeze)
$	if (local_only)
$		then
$		que_home = f$getqui("display_queue", "scsnode_name" -
					,que_name, freeze)
$		if (que_home .eqs. current_node) then gosub 'option'_que
$	else
$	gosub 'option'_que
$	endif !(local_only)
$ goto queue_loop
$end_queue_loop:
$ goto bail_out
$start_que:
$!$	gosub start_symbiont_que
$	if (f$getqui("display_queue", "queue_stopped", que_name, freeze))
$		then
$		say "%CTLQUE-I-STARTING, starting queue ''que_name'"
$		start/queue 'que_name'
$	else
$	say "%CTLQUE-I-STARTED, ''que_name' already started"
$	endif
$ return
$delete_que:
$	gosub stop_que
$	if (f$getqui("display_queue", "queue_stopped", que_name, freeze))
$		then
$		say "%CTLQUE-I-DELETING, deleting queue ''que_name'"
$		delete/queue 'que_name'
$	else
$	say "%CTLQUE-I-NOTSTOP, queue ''que_name' not stopped"
$	endif
$ return
$stop_que:
$	if (f$getqui("display_queue", "queue_stopped", que_name, freeze))
$		then
$		say "%CTLQUE-I-STOPPED, ''que_name' already stopped"
$	else
$	say "%CTLQUE-I-STOPPING, stopping queue ''que_name'"
$	stop/queue/next 'que_name'
$	endif
$!$	gosub stop_symbiont_que
$ return
$reset_que:
$	say "%CTLQUE-I-RESETTING, resetting queue ''que_name'"
$	stop/queue/reset 'que_name'
$!$	gosub stop_symbiont_que
$ return
$set_que:
$	say "%CTLQUE-I-SETTING, modifying queue ''que_name'"
$	set queue 'que_name' 'set_options'
$ return
$setacl_que:
$	say "%CTLQUE-I-SETACL, modifying queue ''que_name'"
$	set acl/object=queue 'que_name'/acl=('acl_ident') 'acl_options'
$ return
$remacl_que:
$	say "%CTLQUE-I-REMACL, modifying queue ''que_name'"
$	set acl/object=queue 'que_name'/delete/acl=('acl_ident') 'acl_options'
$ return
$show_que:
$list_que:
$	host_node = f$getqui("display_queue","scsnode_name",que_name,freeze)
$	if ((local_only .and. (host_node .eqs. current_node)) -
			.or. (.not local_only))
$		then
$		device_name = f$getqui("display_queue","device_name" -
					,que_name,freeze)
$		m_queue_status = f$getqui("display_queue", "queue_status" -
					,que_name, freeze)
$		queue_status == ""
$		call get_status available
$		call get_status busy
$		call get_status closed
$		call get_status disabled
$		call get_status idle
$		call get_status paused
$		call get_status pausing
$		call get_status resetting
$		call get_status resuming
$		call get_status stalled
$		call get_status starting
$		call get_status stopped
$		call get_status stop_pending
$		call get_status stopping
$		call get_status unavailable
$		queue_status == queue_status - ","
$		say f$fao("!32AS!10AS!AS !AS" -
			,que_name, host_node, device_name, queue_status)
$	endif !((local_only .and. (host_node .eqs. current_node)) -
$ return
$stop_symbiont_que:
$!$	if (f$getqui("display_queue", "queue_terminal", que_name, freeze) -
$!		.or. f$getqui("display_queue", "queue_printer" -
$!					,que_name, freeze))
$!$		then
$!$		host_node = f$getqui("display_queue", "scsnode_name" -
$!					,que_name, freeze)
$!$		queue_device = f$getqui("display_queue", "device_name" -
$!					,que_name, freeze)
$!$		if (queue_device .nes. "") then -
$!$		    if (host_node .eqs. current_node) then -
$!$			if (f$getdvi(queue_device, "spl")) then -
$!$				set device 'queue_device'/nospool
$!$	endif
$ return
$start_symbiont_que:
$!$	if (f$getqui("display_queue", "queue_terminal", que_name, freeze) -
$!		.or. f$getqui("display_queue", "queue_printer" -
$!					,que_name, freeze))
$!$		then
$!$		host_node = f$getqui("display_queue", "scsnode_name" -
$!					,que_name, freeze)
$!$		queue_device = f$getqui("display_queue", "device_name" -
$!					,que_name, freeze)
$!$		if (queue_device .nes. "") then -
$!$		    if (host_node .eqs. current_node) then -
$!$			if (f$getdvi(queue_device, "devclass") -
$!					.ne. dc$_mailbox) then -
$!$			    if (.not. f$getdvi(queue_device, "spl")) then -
$!$				set device 'queue_device' -
$!					/spool=('que_name',disk$scratch1)
$!$	endif
$ return
$no_option:
$ say "%CTLQUE-E-INVOPT, invalid option selected"
$ say "-CTLQUE-I-OPTIONS, valid options are ''valid_options'"
$ goto bail_out
$no_que_type:
$ say "%CTLQUE-E-NOQUETYPE, no queue type specified"
$ goto bail_out
$invalid_que_type:
$ say "%CTLQUE-E-INVQUETYPE, invalid queue type specified"
$ goto bail_out
$no_set_option:
$ say "%CTLQUE-E-NOSETOPT, no SET options were specified"
$ goto bail_out
$bail_out:
$    	cancel_it = f$getqui("cancel_operation")
$die:
$ !'f$verify(__vfy_saved)'
$ exitt 1
$
$give_help:
$ say " "
$ say "The general format of the command is:"
$ say " "
$ say "     @SYS_CONTROL_QUEUES option queue_type [queue_name]"
$ say " "
$ say "Where:"
$ say " "
$ say "   option       Can be START, STOP, SET, RESET, SHOW, LIST, SETACL,"
$ say "                REMACL or DELETE.  These cannot be abbreviated."
$ say "   queue_type   Can be BATCH, GENERIC, PRINTER, SERVER, SYMBIONT or"
$ say "                TERMINAL or * to match all queue types.
$ say "                These cannot be abbreviated."
$ say "  [queue_name]  Is an optional string to use for matching on the queue"
$ say "                name, defaults to * to match all queues."
$ say " "
$ say "Examples: (assume fq = @sys_control_queues)"
$ say " "
$ say "   fq stop printer          will stop all printer queues"
$ say "   fq stop terminal lt*     will stop terminal queues on lt devices"
$ say "   fq stop batch sys$*      will stop batch queues named sys$..."
$ say "   fq start batch           will start all batch queues"
$ say "   fq set symbiont loc* owner=bloggs_f"
$ say "                            will change ownership of all loc* queues"
$ say " "
$ !'f$verify(__vfy_saved)'
$ exitt 1
$
$get_status: subroutine
$ status = f$edit(P1,"collapse,lowercase")
$ if (status .eqs. "") then goto end_get_status
$ if ((m_queue_status .and. qui$m_queue_'status') .ne. 0) then -
$   queue_status == queue_status + ",''status'"
$end_get_status:
$ exitt 1
$ endsubroutine
$ !+==========================================================================
$ !
$ ! Procedure:	SYS_CONTROL_QUEUES.COM
$ !
$ ! Purpose:	Written primarily to be used from other procedures to allow
$ !		queue manipulation fairly easily.  In most cases, a procedure
$ !		will want to stop all batch queues, or all printer queues, or
$ !		both, before doing some task and then restart them afterwards.
$ !
$ ! Parameters:
$ !	 P1	The operation to do, start, stop, set or delete.
$ !	 P2	Which type of queues to operate on (see help above).
$ !	[P3]	An optional queue name (see help above).
$ !	[P4]	The things we want to SET if P1 is SET, i.e. a required
$ !		parameter if P1 is SET.
$ !		If P1 is SETACL then this is the identifier.
$ !
$ ! History:
$ !		29-Aug-1988, DBS; Version V1-001
$ !  	001 -	Original version.
$ !		14-Oct-1988, DBS; Version V1-002
$ !	002 -	Included a check for the vms version.
$ !		23-Jan-1989, DBS; Version V1-003
$ !	003 -	Added verify stuff.
$ !		06-Jul-1989, DBS; Version V1-004
$ !	004 -	Fixed a bug (occurring on one machine only) where the queue
$ !		context didn't seem to be updated.  Replaced references to
$ !		the que_stopped symbol with calls to f$getqui.
$ !		08-Feb-1990, DBS; Version V1-005
$ !	005 -	Tidied up a few message formats.
$ !		16-May-1990, DBS; Version V1-006
$ !	006 -	Added the SET option.
$ !		29-Jun-1990, DBS; Version V1-007
$ !	007 -	Added code to despool and spool any terminal queue devices
$ !		when they are stopped and started.
$ !		04-Jul-1990, DBS; Version V1-008
$ !	008 -	Added queue name and a disk when setting spooling on devices.
$ !		13-Jul-1990, DBS; Version V1-009
$ !	009 -	Added reset option to do a stop/reset.
$ !		16-Jul-1990, DBS; Version V1-010
$ !	010 -	Added optional LOCAL parameter to select only those queues on
$ !		the current node, useful for symbionts.
$ !		18-Jul-1990, DBS; Version V1-011
$ !	011 -	Added a check for printer queues when despooling devices.
$ !		06-Nov-1990, DBS; Version V1-012
$ !	012 -	Added SHOW and LIST options.
$ !		14-Mar-1991, DBS; Version V1-013
$ !	013 -	Added SETACL option.
$ !		26-Nov-1991, DBS; Version V1-014
$ !	014 -	Added REMACL option and included P5 to allow other acl type
$ !		options.
$ !		06-Dec-1991, DBS; Version V1-015
$ !	015 -	Added code to handle nla0: as the device.
$ !		30-Jan-1992, DBS; Version V1-016
$ !	016 -	Added check to see if queue_device exists before attempting to
$ !		do anything with it - stops invalid logical name errors.
$ !		16-Nov-1993, DBS; Version V1-017
$ !	017 -	Enhanced the output from the show.
$ !-==========================================================================
