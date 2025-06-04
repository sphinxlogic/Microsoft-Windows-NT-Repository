$save_ver = f$verify(0)
$! X_REMOTE_LOGIN.COM
$!
$! This network object command file is used to start up a DECTERM on a remote
$! node and display the results on the local host.
$!
$! When invoked as a network object it will attempt to track back to the
$! calling node.
$!
$! When invoked interactively, it will open the remote object through DECNET
$! and relay the current display information.
$!
$! Interactive operation takes the following parameter:
$!  P1 - The destination node name
$!  P2 - The source display node
$!  P3 - The source transport
$!  P4 - The server number, defaults to 0.
$!  P5 - The screen number, defaults to 0.
$!
$!  The following parameters are only used when launched from the
$!  Session Manager or File View menu.
$!
$!  P6 - The remote username, or "?" to use P7 for symbol.
$!	 if P6 is "=" then the current logged in user is used.
$!  P7 - The symbol prefix to store the remote username and password.
$!	 if p7 is "" then P1 will be used.
$!
$!  The login information is cached from P6 if the login is successful
$!  and will be used for subsequent launches for any systems that
$!  match the symbol prefix generated either from P7 or P1.
$!
$! If this object is not in your default directory, then it neededs to
$! be an installed network object.
$!
$! This object needs to be installed with the following commands for Phase IV
$!
$! mcr ncp define object x_remote_login number 0 -
$!	file DEV:[dir]X_REMOTE_LOGIN.COM -
$!	proxy both alias incom enab
$!
$! Decnet Phase V:
$!
$! MCR NCL CREATE SESSION CONTROL APPLICATION X_REMOTE_LOGIN
$! MCR NCL SET SESSION CONTROL APPLICATION X_REMOTE_LOGIN -
$!	ADDRESSES = {NAME = X_REMOTE_LOGIN} -
$!	,OUTGOING PROXY = TRUE -
$!	,INCOMING PROXY = TRUE -
$!	,INCOMING ALIAS = TRUE -
$!	,NODE SYNONYM = TRUE -
$!	,IMAGE NAME = NET_TOOLS:X_REMOTE_LOGIN.COM
$!
$! The defaults for this procedure are to use the DECNET transport for
$! communicating with the remote node.  If you use the TCPIP transport
$! to communicate with the remote node, you must define the symbol
$! x_remote_login to launch this object.
$!
$! 08-Aug-1991	J. Malmberg	Original
$!
$! 01-Sep-1994	J. Malmberg	Added PC and TCP/IP Support.
$! 19-Mar-1997	J. Malmberg	FileVUE, Better TCPIP and PC support
$! 20-Apr-1997	J. Malmberg	Fixed to work with local transport
$! 25-Jun-1999  J. Malmberg	Fixed to work with TCPIP 5.0.
$! 18-Dec-2000	J. Malmberg	Fixed minor bug for UCX introduced by above
$! 18-Dec-2000	J. Malmberg	Setup to work in non-proxied environment
$!				where it is not installed as an object.
$!=============================================================================
$!
$obj_name = "x_remote_login"
$!
$!DEFINE SYS$OUTPUT SYS$LOGIN:REMOTE_LOGIN.LOG
$!
$! Verify mode of operation
$!-------------------------
$IF F$MODE() .NES. "NETWORK"
$THEN
$!
$!
$!  Check for get_pwd_callback for getting a password from a login window
$!--------------------------------------------------------------------------
$   if f$extract(0,1,p1) .eqs. "#" then goto get_pwd_call_back
$   if save_ver .nes. 0 then set ver
$!
$!
$!  Attempt to invoke the Remote Decterm back to the calling display
$!-----------------------------------------------------------------------
$   remote_node = p1
$   remote_user = ""
$   remote_pass = ""
$   remote_acc = ""
$!
$!  Set up FileView information
$!------------------------------
$   fileview = "FALSE"
$   if f$trnlnm("VUE$INPUT") .nes. ""
$   then
$	fileview = "TRUE"
$	vue$suppress_output_popup
$	vue$set_task_label "Creating Remote Login - ''remote_node'"
$	vue$set_output_title "Creating Remote Login - ''remote_node'"
$   endif
$!
$!  Use existing display for defaults
$!------------------------------------
$   if f$getdvi("DECW$DISPLAY","EXISTS")
$   then
$	define/user sys$output nl:
$	show display/symbol
$	display_node = DECW$DISPLAY_NODE
$	display_screen = DECW$DISPLAY_SCREEN
$	display_server = DECW$DISPLAY_SERVER
$	network = DECW$DISPLAY_TRANSPORT
$	if network .eqs. "LOCAL"
$	then
$	    network = "DECNET"
$	    display_node = F$TRNLNM("SYS$NODE") - "::"
$	endif
$   else
$	network = ""
$	display_node = ""
$	display_screen = "0"
$	display_server = "0"
$   endif
$!
$!
$!  Apply any overrides
$!----------------------
$   if p2 .nes. "" then display_node = p2
$   if p3 .nes. "" then network = p3
$   if p4 .nes. "" then display_server = p4
$   if p5 .nes. "" then display_screen = p5
$!
$!  If we need more information, ask for it
$!-----------------------------------------
$   if fileview
$   then
$!
$!	File View or Session Manager invoked us
$!---------------------------------------------
$	vue$get_symbol vue$show_hidden_dialogs
$	vue$read show_hidden_dialogs
$	if remote_node .eqs. ""
$	then
$	    if show_hidden_dialogs then goto x_loop
$	    vue$get_symbol X_REMOTE_NODE
$	    goto x_read
$x_loop:
$	    vue$inquire_symbol "X_REMOTE_NODE Remote Node:"
$x_read:
$	    vue$read remote_node
$	    if remote_node .eqs. "" then goto x_loop
$	    vue$set_task_label "Creating Remote Login - ''remote_node'"
$	    vue$set_output_title "Creating Remote Login - ''remote_node'"
$	endif
$!
$!	Get the login information if we do not already have it
$!-------------------------------------------------------
$	my_user = f$edit(f$getjpi("","USERNAME"),"UPCASE,TRIM")
$	if p7 .eqs. "" then p7 = remote_node
$	rem_user_sym = "''p7'__USERNAME"
$	rem_pass_sym = "''p7'__TOKEN"
$	if p6 .eqs. "=" then remote_user = my_user
$	if remote_user .nes. ""
$	then
$	    define/job/nolog 'rem_user_sym' "''remote_user'"
$	endif
$	if p6 .nes. ""
$	then
$	    if f$trnlnm(rem_pass_sym,"LNM$JOB").eqs. "" .or. show_hidden_dialogs
$	    then
$		my_proc = f$environment("PROCEDURE")
$		wxnam = "Please enter the login information for ''remote_node'."
$		ixname = "Login ''remote_node'"
$		wname = "icon=""""''ixname'"""",title=""""''wxnam'"""""
$		wsize = "rows=15,columns=40,initial_state=window"
$		wind = "/window=(''wname',''wsize')"
$		save_ver = f$verify(0)
$		create/term/wait'wind'/big_font @'my_proc' "#" 'p7'
$		if save_ver then set verify
$	    endif
$	    remote_user = f$trnlnm(rem_user_sym,"LNM$JOB")
$	    remote_pass = f$trnlnm(rem_pass_sym,"LNM$JOB")
$	endif
$!
$   else
$!	Manually invoked
$!-----------------------
$	if remote_node .eqs. ""
$	then
$	    write sys$output "Remote node not specified."
$	    exit
$	endif
$	if display_node .eqs. ""
$	then
$	    write sys$output "Remote display not specified."
$	    exit
$	endif
$	if network .eqs. ""
$	then
$	    write sys$output "Display transport not specified."
$	    exit
$	endif
$   endif
$!
$!   Send the request to the remote node
$!-------------------------------------------
$   if fileview then vue$popup_progress_box
$   save_ver = f$verify(0)
$   if network .eqs. "TCPIP"
$   then
$	rsh_user = ""
$	if remote_user .nes. ""
$	then
$	   rsh_user= "/user_name=""''remote_user'""/password=""''remote_pass'"""
$	endif
$	rsh'rsh_user' 'remote_node' 'obj_name' 'display_node' 'network' -
		'display_server' 'display_screen'
$	rsh_stat = $status
$	if ('rsh_stat' .and. 1) .eq. 0
$	then
$	    goto remote_error
$	endif
$   else
$	if remote_user .nes. ""
$	then
$	    remote_acc = """''remote_user' ''remote_pass'"""
$	endif
$	open/write/error=remote_error net -
	    'remote_node''remote_acc'::"0=''obj_name'"
$	write/error=remote_error net display_node
$	write/error=remote_error net network
$	write/error=remote_error net display_server
$	write/error=remote_error net display_screen
$	close net
$   endif
$   if save_ver then set ver
$!  WRITE SYS$OUTPUT "Invalid invocation of network object ''obj_name'!"
$!
$!
$   EXIT
$ENDIF
$!
$! Default the screen and server
$!-------------------------------
$screen = "0"
$server = "0"
$!
$! Determine where we are and where we came from
$!-----------------------------------------------
$NODE_NAME = F$TRNLNM("SYS$NODE")
$SYS_NET = F$TRNLNM("SYS$NET")
$remote_node = f$trnlnm("SYS$REM_NODE")
$prefix_sys_net = f$element(0,":",SYS_NET)
$!
$! Check for destination override
$!-------------------------------
$if p1 .nes. ""
$then
$   remote_node = p1 - ":" - ":"
$endif
$!
$! Check for TCP/IP
$!------------------
$transport = "DECNET"
$tt_trans = f$trnlnm("tt")
$tcp_cmd = ""
$if (f$locate("TCPIP$",tt_trans) .lt. f$length(tt_trans))
$then
$   tcp_cmd = "TCPIP"
$else
$   if (f$locate("UCX$",tt_trans) .lt. f$length(tt_trans))
$   then
$	tcp_cmd = "UCX"
$   endif
$endif
$if tcp_cmd .nes. ""
$then
$   transport = "TCPIP"
$   bg_dev = f$getdvi(prefix_sys_net,"devnam") - "_"
$   if p1 .eqs. ""
$   then
$	pid = f$getjpi("","pid")
$	temp_file = "SYS$LOGIN:''obj_name'_''PID'.TEMP"
$	if (f$element(1,".",remote_node) .nes. ".") .and. (tcp_cmd .eqs. "UCX")
$	then
$!
$!	    UCX 3.x
$!------------------
$	    if f$search(temp_file) .nes. "" then delete 'temp_file';*
$	    define/user sys$output 'temp_file'
$	    'tcp_cmd' show device 'bg_dev'
$	    open/read/end=tcp_loop1_end tcp 'temp_file'
$tcp_loop1:
$		read/end=tcp_loop1_end tcp line_in
$		line_in = f$edit(line_in,"COMPRESS,UPCASE,TRIM")
$		dev = f$element(0," ",line_in) + ":"
$		if dev .nes. bg_dev then goto tcp_loop1
$		remote_type = f$element(4," ",line_in)
$		remote_node = f$element(5," ",line_in)
$tcp_loop1_end:
$	    close tcp
$	    if f$search(temp_file) .nes. "" then delete 'temp_file';*
$	else
$	    if (tcp_cmd .eqs. "TCPIP")
$	    then
$!
$!		TCPIP 5.x
$!-------------------------
$		remote_node = f$edit(remote_node,"compress") - """"
$		remote_node = f$element(0," ",remote_node)
$	    else
$!
$!		UCX 4.x
$!------------------------
$		ipaddr = remote_node - "::"
$		hexipaddr=f$fao("!8XL",'ipaddr')
$		h1 = %x'f$extract(0,2,hexipaddr)
$		h2 = %x'f$extract(2,2,hexipaddr)
$		h3 = %x'f$extract(4,2,hexipaddr)
$		h4 = %x'f$extract(6,2,hexipaddr)
$		remote_node = "''h4'.''h3'.''h2'.''h1'"
$	    endif
$	endif
$   endif
$endif
$if transport .eqs. "DECNET"
$then
$   open/read remote_link sys$net
$   read/end=read_done/err=read_done remote_link remote_node
$   read/end=read_done/err=read_done remote_link transport
$   read/end=read_done/err=read_done remote_link server
$   read/end=read_done/err=read_done remote_link screen
$read_done:
$   close remote_link
$endif
$!
$clu=""
$sys_cluster_node = f$trnlnm("SYS$CLUSTER_NODE") - ":" - ":"
$if sys_cluster_node .nes. ""
$then
$   clu="(''sys_cluster_node')"
$endif
$tcode="?"
$if transport .eqs. "DECNET" then tcode="::"
$if transport .eqs. "TCPIP" then tcode=":"
$if transport .eqs. "LAT" then tcode="/"
$if transport .eqs. "LOCAL" then tcode="-"
$!
$dnode = node_name - ":" - ":"
$arch = f$getsyi("arch_name")
$version = f$edit(f$getsyi("version"),"TRIM")
$swtype = f$edit(f$getsyi("node_swtype"),"TRIM")
$if swtype .eqs. "VMS" then swtype="OpenVMS"
$hw_name = f$getsyi("hw_name")
$!
$title= "''dnode'''clu'''tcode' * [''hw_name'] ''swtype' ''arch' ''version'"
$!
$! Create the display terminal
$!-----------------------------
$SET DISPLAY/CREATE/NODE='remote_node'/TRANSPORT='transport'-
	/SCREEN='screen'/SERVER='server'
$CREATE/TERMINAL=DECTERM/DETACH/INSERT/LINE_EDITING/NOLOGGED_IN-
   /WINDOW_ATTRIBUTES=(TITLE="''title'",ICON_NAME="''title'")
$!
$!
$! Clean Exit
$!------------
$exit
$!
$remote_error:
$    if f$type(rem_pass_sym) .eqs. "STRING"
$    then
$	if f$trnlnm(rem_pass_sym,"LNM$JOB") .nes. ""
$	then
$	    deasign/job 'rem_pass_sym'
$	endif
$   endif
$   if f$trnlnm("VUE$OUTPUT") .nes. ""
$   then
$	vue$popup_message "Unable to create remote DECterm"
$   else
$	write sys$output "Unable to create remote DECterm."
$   endif
$   if f$trnlnm("NET","LNM$PROCESS_DIRECTORY",,"SUPERVISOR") .nes. ""
$   then
$	close net
$   endif
$   exit
$!
$!
$! Get username and password callback
$!=====================================
$get_pwd_call_back:
$set noon
$user_sym = "''P2'__USERNAME"
$pass_sym = "''P2'__TOKEN"
$!
$def_prompt = ""
$old_user_sym = f$trnlnm(user_sym,"LNM$JOB")
$if old_user_sym .nes. "" then def_prompt = "[" + old_user_sym + "]"
$read/err=no_user sys$command/prompt="Username: ''def_prompt'"/time=100 line_in
$line_in = f$edit(line_in, "TRIM")
$if line_in .eqs. ""
$then
$   if old_user_sym .nes. ""
$   then
$	line_in = old_user_sym
$   else
$	goto no_user
$   endif
$endif
$!
$define/job/nolog 'user_sym' "''line_in'"
$!
$set term/noecho
$read/err=no_pass sys$command/prompt="Password: "/time=100 line_in
$line_in = f$edit(line_in, "TRIM")
$goto got_pass
$no_pass:
$line_in = ""
$got_pass:
$! Add mangling here later
$!-------------------------
$define/job/nolog 'pass_sym' "''line_in'"
$set term/echo
$!
$!
$no_user:
$exit
