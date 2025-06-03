$!
$! KITINSTAL.COM
$!
$! Nmail installation procedure (called from VMSINSTAL)
$!
$	say      = "write sys$output"
$	target = "''vmi$architecture'"
$	if target .eqs. "" then target = "VAX"
$	if target .eqs. "AXP" then target = "ALPHA" ! vmsinstal stupidity
$
$	nm$ident == vmi$pretty  ! just in case
$	if f$search("vmi$kwd:ident.com") .nes. "" then @vmi$kwd:ident
$
$	builton  = "6.1"	! VAX VMS version on which I linked images
$	minsupp  = "5.4"	! Minimum supported version of VAX VMS
$	diskblox = 1000         ! Required disk space (peak)
$	if target .eqs. "ALPHA"
$	then
$	    builton  = "6.1"	! Different version numbers
$	    minsupp  = "1.5"    !  for ALPHA VMS
$ 	    diskblox = 2500	! Bigger, too
$	endif
$
$	if p1 .eqs. "VMI$_INSTALL" then goto install
$ 	if p1 .eqs. "VMI$_IVP"     then goto ivp
$	if f$extract(0,4,p1) .eqs. "HELP" then goto 'p1'
$	exit vmi$_unsupported
$
$install:
$	on control_y then vmi$callback control_y
$	on warning then goto error_exit
$	help = "@vmi$kwd:kitinstal help_"
$
$	nmwork  = f$edit(f$trnlnm("nm$work", "lnm$system"),"collapse,upcase")
$	if nmwork .eqs. "" then nmwork = "SYS$COMMON:[NMAIL]"
$	nmqueue = f$edit(f$trnlnm("nm$queue","lnm$system"),"collapse,upcase")
$	if nmqueue .eqs. "NM$QUEUE" then nmqueue = ""
$	scsnode = f$edit(f$getsyi("nodename"),"collapse,upcase")
$
$!
$! Firstly, check that this system is a more recent version of VMS
$! than the one on which I built the Nmail kit.  If it isn't, then
$! we can stil support it (with relinking) as long as it's up to
$! the minimum supported level.
$!
$	vmi$callback check_vms_version nm$ 'builton'
$	relink = f$integer(f$trnlnm("nm$relink")) ! emergency override possible
$	if .not. nm$
$	then
$	    relink = 1
$	    vmi$callback check_vms_version nm$ 'minsupp'
$	    if .not. nm$
$	    then
$	    	vmi$callback message e oldvms "this kit requires VMS V''minsupp' or later"
$	    	exit vmi$_failure
$	    endif
$	endif
$
$!
$! Check for sufficient free space on system disk.
$!
$	vmi$callback check_net_utilization nm$ 'diskblox'
$	if .not. nm$ 
$	then
$	    vmi$callback message e nospace "installation needs ''diskblox' blocks free"
$	    exit vmi$_failure
$	endif
$   	vmi$callback set safety yes 'diskblox'
$
$!
$! Determine system type as it affects queue naming conventions and
$! details of the installation.
$!
$	say ""
$	say nm$ident," for ",target," VMS ",vmi$version," systems"
$	say ""
$	say "* Choose system type from this list:"
$	say "  1. Single system"
$	say "  2. Common environment cluster"
$	say "  3. Multiple environment cluster member"
$	say "  4. Multiple environments but shared queue file"
$50:	def = "1"
$	if f$getsyi("cluster_member") then def = "2"
$	if def .eqs. "2" .and. nmqueue .nes. "" then def = "4"
$	vmi$callback ask nm$ "Enter system type" "''def'" i "''help'systyp"
$	if nm$ .eq. 1 then goto 100
$	if nm$ .eq. 2 then goto 200
$	if nm$ .eq. 3 then goto 100
$	if nm$ .eq. 4 then goto 300
$	goto 50
$
$! 
$! Single-system case.  As a hack for me, make the default 
$! number of queues be 1 on a workstation (it's 2 for other
$! systems)
$!
$100:	queue_tag     = ""
$	def = 1 + (f$getsyi("window_system") .eq. 0)
$105:	vmi$callback ask nm$ -
	 "Enter number of Nmail execution queues required" "''def'" i "''help'numque"
$	server_queues = f$integer(nm$)
$	if server_queues .le. 0 then goto 105
$	on_this_node  = 1
$	goto 500
$
$!
$! Common environment cluster case - get names of all nodes in the cluster 
$! on which queues will execute, and the number of queues for each node
$!
$200:	queue_tag     = ""
$205:	server_queues = ""
$	on_this_node  = 0
$	say "* Enter names of all nodes in the cluster on which Nmail queues are to execute."
$	say "  Type one node name per line; press CTRL/Z when all names have been entered."
$	say "  For each node, you will be asked the number of queues required on that node."
$210:	vmi$callback ask nm$ "Node name" "" z "''help'nodnam"
$	node = f$edit(nm$,"trim,upcase") - "::"
$	if node .eqs. "^Z" then goto 220
$	if node .eqs. "" then goto 210
$	if scsnode .eqs. node then on_this_node = 1
$	vmi$callback ask nm$ "Number of queues" "2" i "''help'numque"
$	server_queues = server_queues + "," + node + ":''f$integer(nm$)'"
$	goto 210
$220:	server_queues = server_queues - ","
$	goto 500
$
$!
$! Mixed environment cluster member with shared queue file
$!
$300: 	queue_tag     = ""
$	def = ""
$	if nmqueue .eqs. "" then goto 310
$	if f$extract(0,9,nmqueue) .nes. "NM$QUEUE_" then goto 310
$	def = f$extract(9,16,nmqueue)
$310:	say "* You must choose a ""tag"" which uniquely identifies this instance of Nmail in"
$	say "  your VMS cluster.  The tag is included in queue names to distinguish them from"
$	say "  other Nmail queues that may be in the same queue file.  The tag can be any"
$	say "  short alphanumeric string, or null."
$320:	vmi$callback ask nm$ "Unique tag" "''def'" n "''help'quetag"
$	queue_tag = f$edit(nm$,"trim,upcase")
$	if f$length(queue_tag) .gt. 16 then goto 320
$	goto 205
$
$!
$! If we're using a non-standard work directory, then say so.
$!
$500:   if nmwork .nes. "SYS$COMMON:[NMAIL]"
$	then
$	    say ""
$	    say "  ",nmwork," will be used as the Nmail work directory."
$	endif
$
$!
$! That's the end of the configuration-related questions.
$! Offer a little unsolicited advice, since not everyone seems to
$! realise this...
$!
$	say ""
$	say "  If you wish to change your configuration at a later time, you"
$	say "  can do this by editing the file SYS$STARTUP:NM$STARTUP.COM, the"
$	say "  Nmail startup procedure."
$	say ""
$
$!
$! Ask whether files should be purged (usually they should, but in
$! clusters this can cause problems with deleting stuff installed
$! on other nodes).  Ask about running the IVP and startup procedure.
$!
$	vmi$callback set purge ask
$	vmi$callback set ivp ask
$	xstart = vmi$ivp
$	if .not. vmi$ivp
$	then 
$	    vmi$callback ask nm$ -
	 	"Do you want to start Nmail after the installation" "YES" b "''help'start""
$	    xstart = nm$
$	endif
$
$! 
$! Give a little warning if the IVP won't complete
$! because there's no daemon running on the current node.
$!
$	if vmi$ivp .and. .not. on_this_node
$	then
$	    say ""
$	    say "  Note: you did not include an execution queue for this node (''scsnode'), so"
$	    say "  no mail will be delivered until you execute NM$STARTUP.COM on at least one"
$	    say "  of the other nodes in the cluster."
$	endif
$
$!
$! No more questions
$!
$	say ""
$	say "No more questions..."   
$	say ""
$
$!
$! Create file with all the answers in it
$!
$	create vmi$kwd:nm$startup.tmp
$	open/append nm$startup vmi$kwd:nm$startup.tmp
$	write nm$startup "$!"
$	write nm$startup "$! NM$STARTUP.COM   ",nm$ident
$	write nm$startup "$! Installed at ",f$extract(0,20,f$time())
$	write nm$startup "$!"
$	write nm$startup "$! Installation-specific parameters:"
$	write nm$startup "$!
$	write nm$startup "$! ****************************************************"
$	write nm$startup "$	work_directory = ""''nmwork'"""
$	if f$type(server_queues) .eqs. "INTEGER"
$	then
$	    write nm$startup "$	server_queues  = ''server_queues'"
$	else
$	    write nm$startup "$	server_queues  = ""''server_queues'"""
$	endif
$	write nm$startup "$	install_quals  = ""/SHARE/OPEN/HEADER"""
$	write nm$startup "$	queue_tag      = ""''queue_tag'"""
$	write nm$startup "$! ****************************************************"
$	write nm$startup "$!
$	write nm$startup "$! You may edit the above values if you wish."
$	write nm$startup "$!
$	close nm$startup
$
$!
$! Now prepend the data we just prepared to the startup command
$! procedure from the distribution kit
$!
$	append vmi$kwd:nm$startup.com vmi$kwd:nm$startup.tmp
$	rename vmi$kwd:nm$startup.tmp vmi$kwd:nm$startup.com;0
$
$!
$! Clean up by deleting previous versions of Nmail (only those files
$! which have different names or directories to this version and hence
$! wouldn't be replaced).
$!
$	@vmi$kwd:kitclean
$
$!
$! If we need to relink images, then do so.  Otherwise, rename
$! the appropriate pre-built images for this architecture.
$!
$	if relink 
$	then 
$	    vmi$callback message i relink "relinking the Nmail image files"
$	    @vmi$kwd:kitlink * 'target'
$	else
$	    rename/nolog vmi$kwd:nm$daemon_'target'.exe   nm$daemon
$	    rename/nolog vmi$kwd:nm$queman_'target'.exe   nm$queman
$	    rename/nolog vmi$kwd:nm_mailshr_'target'.exe  nm_mailshr
$	    rename/nolog vmi$kwd:nm_mailshrp_'target'.exe nm_mailshrp
$	endif
$
$!
$! Ensure existing Nmail queues are stopped (so that old symbiont
$! processes are not running)
$!
$	set command vmi$kwd:nm$dcl
$	assign/user vmi$kwd:nm$queman nm$queman
$	if queue_tag .nes. "" .and. nmqueue .eqs. "" 
$	then assign/user nm$queue_'queue_tag' nm$queue
$	endif
$	nmail stop queue /log /oknone
$
$!
$! Create Nmail's working directory (N.B., created as a 'user' directory,
$! not 'system', since in the latter case VMSINSTAL would create two
$! directories in the case of a common cluster system)
$!
$	secure = "/owner=[1,4]/prot=(sys:rwe,own:rwe,gr,wo)"
$	if f$parse(nmwork) .eqs. ""
$	then 
$	    vmi$callback create_directory user 'nmwork' "''secure'"
$	else
$	    vmi$callback message i exists "directory ''nmwork' already exists"
$	endif
$
$!
$! Copy distribution files to correct locations
$!
$	vmi$callback provide_image nm$ nm_mailshr.exe  vmi$root:[syslib]
$	vmi$callback provide_image nm$ nm_mailshrp.exe vmi$root:[syslib]
$ 	vmi$callback provide_image nm$ nm$daemon.exe   vmi$root:[sysexe]
$ 	vmi$callback provide_image nm$ nm$queman.exe   vmi$root:[sysexe]
$ 	vmi$callback provide_file  nm$ nm$error.dat    vmi$root:[sysexe]
$ 	vmi$callback provide_file  nm$ nm$startup.com  vmi$root:[sys$startup] K
$	vmi$callback provide_file  nm$ nm$uguide.doc   vmi$root:[syshlp]
$	vmi$callback provide_file  nm$ nm$sysmgr.doc   vmi$root:[syshlp]
$
$!
$! Add help to system help library, add DCL command to system tables
$!
$ 	vmi$callback provide_dcl_help     nm$help.hlp
$	vmi$callback provide_dcl_command  nm$dcl.cld
$
$!
$! Let VMSINSTAL know about the startup procedure
$!
$ 	if xstart then vmi$callback set startup nm$startup.com
$
$!
$! Now end successfully
$!
$	exit vmi$_success
$
$!
$! Installation has failed - perform any clean up required and finish
$!
$error_exit:
$	s = $status
$	set noon
$	if f$trnlnm("nm$startup") .nes. "" then close nm$startup
$	exit s
$
$!
$! Installation Verification Procedure
$!
$ ivp:
$	on control_y then exit vmi$_failure
$	on warning then exit vmi$_failure
$
$	say ""
$	say "---------------------------------------------------------------"
$	say ""
$	say "  ''nm$ident' Installation Verification Procedure."
$	say "  The IVP uses Nmail to send a short mail message to you."
$	say ""
$
$	extn = f$trnlnm("NM$EXTENSION","LNM$SYSTEM")
$	if extn .nes. "" 
$	then
$	    say "  *** NOTE ***"
$	    say ""
$	    say "  An Nmail-extension image is installed on this system.  This image"
$	    say "  is not part of the Nmail kit, and it may have some adverse effect"
$	    say "  on the expected operation of the IVP."
$	    say ""
$	    say "  NM$EXTENSION is set to ",extn
$	    say ""
$	    say ""
$	endif
$	        
$	user = f$edit(f$getjpi(0,"username"),"trim")
$	args = "/SUBJECT=""''nm$ident' installed""  IVPTEXT  NM%''user'"
$	say "  $ MAIL",args
$
$	nm$confirm  = ""
$	nm$noreturn = ""
$	nm$after    = ""
$	nm$expire   = ""
$	nm$delta    = ""
$
$	assign/user sys$input: ivptext
$	mail/noself/personal="Nmail IVP"'args'
    Receipt of this mail message demonstrates that Nmail is alive 
    and well and living in your VMS system.

    Documentation is located in SYS$HELP in the following files:

	NM$UGUIDE.DOC	user guide
	NM$SYSMGR.DOC	system manager's manual

    Don't forget to edit your system startup procedures as described
    in the system manager's manual.

    Also, if this is a cluster node, you must execute NM$STARTUP.COM
    on all nodes to make Nmail available to users.
$
$	say ""
$	say "  The message will take a minute or so to arrive, assuming no"
$	say "  other Nmail jobs are queued up.   If it doesn't arrive, see"
$	say "  the file SYS$HELP:NM$SYSMGR.DOC for help."
$	say ""
$	say "  ''nm$ident' installation successfully completed."
$	say ""
$	say "---------------------------------------------------------------"
$	exit vmi$_success
$
$!
$! Help text callbacks
$!
$help_systyp:
$	type sys$input
	The answer to this question reflects the configuration of this
	system.  Your choices are as follows:

	1.  A system which is not part of a VMS cluster; it has a private 
	    system disk.  A nice, simple setup.

	2.  A system which is part of an common environment VMS cluster.
	    Key system files, such as SYSUAF.DAT, are shared by all nodes.
	    There is a common queue file, and hence all queues are visible 
	    to all nodes.  You can install Nmail once for the whole cluster.

	3.  A system which is part of a VMS cluster, but which has private
	    instances of the key system files (in particular the queue file).
	    You need to install Nmail on each node.

	4.  The strange case.  This sort of setup has a shared queue file, 
	    but the cluster is split into more than one environment: for
	    example, some nodes have private SYSUAF files. You must install 
	    Nmail in each distinct "part" of the cluster.  Complications arise
	    from the need to have unique queue names in the shared queue file.

$	exit
$help_numque:
$	type sys$input
	The number of Nmail execution queues controls how many Nmail jobs
	can be simultaneously active.  One queue can only process one job
	at a time, so if there are more messages waiting to be sent than
	there are queues available, some messages will obviously have to
	wait.

	The number of queues has no effect on users entering messages,
	only on the average time a message has to wait before it can
	be sent.

	Each queue requires one process slot for the Nmail symbiont,
	and all the overheads associated with a VMS process.  CPU usage
	probably isn't very significant in most systems.

	If in doubt, take the default.  You can easily change it later.

$	exit
$help_nodnam:
$	type sys$input
	Enter the 1-to-6 node name for each cluster member which will
	support at least one Nmail execution queue.  

	You do not need to have an execution queue on every node which
	may wish to send mail with Nmail.  Rather, you simply need to
	have some number of queues available to the cluster as a whole.

	Choose execution nodes so as to maximize availability in case
	part of the cluster is ever down.  You may also take into
	account local configuration preferences, such as having one
	machine dedicated to providing queue services.

	The number of queues is individually specified for each node
	that you name.

$	exit
$help_quetag:
$	type sys$input
	You specified that this system was part of a cluster which has
	a shared queue file, despite the fact that there are distinct
	system disks or user accounts, for example.  If you want to 
	install Nmail in more than one "part" of the cluster, then it
	is necessary to arrange to have unique queue names for each 
	installation.

	The queue names are made unique by appending a short string
	(a "tag") to the usual queue name format.  You can choose any
	tag which is meaningful to you.

	If you consider that your cluster has a main part which most
	users ordinarily use, and one or two other nodes that are
	used for special-purpose applications, then I recommend that you
	specify a null tag for the main part, and a non-null tag (or
	tags, in the case of multiple partitions) for the special-purpose 
	nodes.

$	exit
$help_start:
$	type sys$input
	Like many VMS layered products, Nmail provides a startup procedure 
	which establishes the environment needed for operation of the product. 
	Ordinarily, the startup procedure should be invoked at the end
	of Nmail installation -- answer "yes" to this question.

	You may however wish to defer startup for some reason, in which
	case you can answer "no".   You must manually execute the
	procedure SYS$STARTUP:NM$STARTUP.COM at some later time.
	
$	exit
$!
$! End of KITINSTAL.COM
$!
