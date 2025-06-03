$!++
$!
$! NM$STARTUP.COM - Nmail startup command file
$!
$! Inputs: P1 = "init"		initial setup (don't start queues)
$!	      = "start"	 	just start the queues
$!	      = ""		do both init and start
$!	   P2 = work directory	
$!	   P3 = server queues
$!	   P4 = install qualifiers
$!	   P5 = queue tag
$!
$! Note that P2-P5 are overridden by the prefix file that a
$! normal Nmail installation puts on the front of this one.
$!
$! Dave Porter   13-May-1985
$!
$! Copyright (C) 1985, 1986, 1987, 1988, 1989, 1993
$! by Digital Equipment Corporation, Maynard, Mass.
$!
$! FOR DEC INTERNAL USE ONLY
$!
$! Nmail V9.4 and later; 15-Nov-1993
$!
$!--
$	goto nmstart
$
$!
$! Description of user-settable parameters, included at the  head of this
$! file by the Nmail installation procedure:
$!
$! The symbol 'work_dir' defines the Nmail work directory, where  pending
$! messages are stored.  You can change this to be some other (dedicated)
$! directory if you wish.  The directory must exist  already,  and  in  a
$! cluster, must be on a common disk.
$!
$! In  the  non-cluster case, equate  'server_queues'  to  the number  of 
$! concurrently active Nmail streams that you require.  Each Nmail stream  
$! uses one server execution queue, and one server process, which will be 
$! set up by this command procedure.
$! 
$! In the cluster case, 'server_queues' has a more complicated structure.
$! It is a list  of elements separated by commas.   Each element consists
$! of a node name, a single colon,  and then the number of  queues to run 
$! on that node.  If no servers are to execute on a particular node, then
$! there is no need for that cluster node to be named in the list at all.
$!
$! Execution queues have names like NM$QUEnn (nn=01, 02...).   There will 
$! also be one  generic queue, NM$QUEUE, to which all  jobs are submitted  
$! and which feeds all the execution queues.
$! 
$! If only one execution queue is required then it will be named NM$QUEUE 
$! and no generic queue is needed.  
$! 
$! The  symbol 'install_quals' can be set to any INSTALL  qualifiers which
$! you wish to apply to all Nmail images.
$!
$! For  VAXclusters  which are  neither totally  heterogeneous nor totally
$! homogenous,  equate the symbol 'queue_tag' to some  short string  which 
$! is unique in the Nmail installations in the cluster; each different tag
$! string defines  a different Nmail operating domain.  The tag is used to
$! make queue names unique.   A null string is a legal  tag value,  but of
$! course  only one Nmail installation can use null;  others must set some 
$! non-null value.
$!
$! You should not make any other changes to this procedure.
$!
$! This command  file should be executed as part of system startup on  all
$! nodes  that  will  require access to Nmail  (not just the ones on which
$! you wish to have execution queues).   This  is  so  that  the  required
$! logical names, etc, are defined.
$!
$
$nmstart:
$	reqd_privs = "SYSNAM,SYSPRV,OPER,CMKRNL"
$	old_privs  = f$setprv(reqd_privs)
$	if .not. f$privilege(reqd_privs) then goto no_privs
$	on control_y then goto exit
$	on error then goto error
$	logpfx  = f$envir("message") .eqs. "/FACILITY/SEVERITY/IDENTIFICATION/TEXT"
$! 
$! Validate operating mode
$!
$	mode = f$extract(0,1,f$edit(p1,"lowercase,collapse"))
$	if mode .nes. "" .and. mode .nes. "i" .and. mode .nes. "s" then goto badmode
$!
$! Take parameters from command line, if they haven't been forced
$! to something by a prefix file.  Command-line parameters are not
$! ordinarily used.
$!
$	if f$type(work_directory) .eqs. "" then work_directory = p2
$	if f$type(server_queues)  .eqs. "" then server_queues  = p3
$	if f$type(install_quals)  .eqs. "" then install_quals  = p4
$	if f$type(queue_tag)	  .eqs. "" then queue_tag      = p5
$!
$! Figure out queue names
$!
$	if queue_tag .eqs. ""
$	then
$	    nmqueue = "NM$QUEUE"
$	    xqueue  = "NM$QUE!2ZL"
$	else
$	    nmqueue = "NM$QUEUE_" + queue_tag
$	    xqueue  = "NM$QUE!2ZL_" + queue_tag
$	endif
$
$!
$! Initialisation: skip this stuff if just starting the queues 
$!
$	if mode .eqs. "s" then goto part2
$part1: ! modes i and null
$!
$! Define logical name for work directory. Must be an exec-mode
$! logical and in the system table, otherwise the symbiont won't
$! use it.
$!
$	if logpfx
$	then tmp = "%NMAIL-I-WRKDIR, work directory is "
$	else tmp = "Work directory is "
$	endif
$	if work_directory .eqs. "" then work_directory = "SYS$COMMON:[NMAIL]"
$	write sys$output tmp,work_directory
$	define/system/exec/nolog  nm$work  'work_directory'
$!
$! Define logical name for queues if this is the case of a mixed
$! cluster.
$!
$	if nmqueue .nes. "NM$QUEUE" 
$	then define/system/exec/nolog  nm$queue  'nmqueue'
$	endif
$!
$! Install Nmail user interface images. (If they're currently 
$! installed, then remove them first)
$!
$	ins = 1
$	ins = f$file_attrib("sys$share:nm_mailshr.exe","known") 
$	if ins then install remove sys$share:nm_mailshr
$	install  add  sys$share:nm_mailshr 'install_quals'
$	ins = 1
$	ins = f$file_attrib("sys$share:nm_mailshrp.exe","known") 
$	if ins then install remove sys$share:nm_mailshrp
$	install  add  sys$share:nm_mailshrp /protected /shared /open /header 'install_quals'
$	ins = 1
$	ins = f$file_attrib("sys$system:nm$queman.exe","known") 
$	if ins then install remove nm$queman
$	install  add  nm$queman /priv=sysprv 'install_quals'
$
$	if mode .eqs. "i" then goto part3
$part2:	! modes s and null
$!
$! Install Nmail daemon (symbiont)
$!
$	ins = 1
$	ins = f$file_attrib("sys$system:nm$daemon.exe","known")
$	if ins then install remove nm$daemon
$	install add  nm$daemon 'install_quals'
$
$part3:	! all modes
$!
$! Queue setup commands. Note that queues are protected against insertion
$! by non-system users; this is to help prevent random files getting put
$! in the queue (the proper MAIL interface is suitably privileged).
$!
$	initque = "initialize/queue" +-
			"/retain=error" +-
			"/owner=[1,4]" +-
			"/protection=(system:e,owner:d,group:r,world:r)" +-
			"/device=server" +-
			"/processor=nm$daemon" +-
			"/schedule=nosize" +-
			"/nogeneric" +-
			"/noenable_generic"
$	initgen = "initialize/queue" +-
			"/retain=error" +-
			"/owner=[1,4]" +-
			"/protection=(system:e,owner:d,group:r,world:r)" +-
			"/device=server" +-
			"/schedule=nosize" +-
			"/generic="
$! 
$! Other queue-related setup
$!
$	scsnode = f$edit(f$getsyi("nodename"),"collapse,upcase")
$	qstart  = "/start"
$	if mode .eqs. "i" then qstart = ""
$	if logpfx
$	then qreport = "write sys$output ""%NMAIL-I-QSTART, starting queue "","
$	else qreport = "write sys$output ""Starting queue "","
$	endif
$!
$! Set up execution queues for all nodes in the cluster
$!
$	queue_list = ""				! complete queue list
$	queue_num  = 0				! number of next queue
$	nque       = f$integer(server_queues)	! number of queues for this node
$	node       = ""				! this node name
$	onqual     = ""				! /on qualifier
$	stqual	   = qstart			! /start qualifier
$	n          = 0
$	if f$type(server_queues) .eqs. "INTEGER" then goto nchk
$nloop:	node       = f$element(n,",",server_queues)
$	if node .eqs. "," .or. node .eqs. "" then goto endn
$	nque       = f$element(1,":",node)
$	node       = f$element(0,":",node)
$	if nque .eqs. ":" then nque = "1"
$	nque       = f$integer(nque)
$	onqual 	   = "/on=''node'::"
$	stqual	   = qstart
$	if node .nes. scsnode then stqual = ""
$nchk:	if queue_num .eq. 0 .and. nque .eq. 1 .and. -
	   f$element(1,",",server_queues) .eqs. "," then -
	goto singleq
$!
$! Set up the appropriate number of execution queues on one node;
$! if this is a vaxcluster, then we only start the queue if we're
$! actually executing on the correct node
$!
$	q = 0
$qloop:	if q .ge. nque then goto endq
$	queue_num = queue_num + 1
$	queue = f$fao(xqueue,queue_num)
$	queue_list = queue_list + "," + queue
$	if stqual .nes. "" then qreport queue
$	initque 'queue' 'onqual' 'stqual'
$	q = q + 1
$	goto qloop
$!
$! Loop for next node
$!
$endq:	n = n + 1
$	goto nloop
$!
$! Here to set up the generic queue, after initialising all execution queues
$!
$endn:	queue_list = queue_list - ","
$	if queue_list .nes. "" 
$	then 
$	    if qstart .nes. "" then qreport nmqueue
$	    initgen('queue_list') 'nmqueue' 'qstart'
$	endif
$	goto exit
$!
$! Here when we only want a single queue. No generic queue
$! is needed.
$!
$singleq:
$	if stqual .nes. "" then qreport nmqueue
$	initque 'nmqueue' 'onqual' 'stqual' 
$
$!
$! All done, restore privileges and exit
$!
$exit:
$	foo = f$setprv(old_privs)
$	exit
$
$!
$! Errors
$!
$no_privs:
$	write sys$output "%NMAIL-F-NOPRIV, need ''reqd_privs' privileges"
$	goto exit
$badmode:
$	write sys$output "%NMAIL-F-MODE, unknown start mode P1=""",p1,""""
$	goto exit
$error:
$	mess = "-" + f$message($status) - "%" 
$	write sys$output "%NMAIL-F-FATAL, error in startup procedure"
$	write sys$output mess
$	goto exit
$
$!
$! End of NM$STARTUP.COM
$!
