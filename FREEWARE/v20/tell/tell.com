$!TELL.COM
$ vb=f$verify(f$trnlnm("tell_verify"))
$ version := V3.0
$!
$! AUTHOR: kingsmill@ripper.stl.dec.com
$! Version 1.1  Geoff Kingsmill - fixed a number of bugs and added a summary 
$!                                report.
$! Version 2.0  Geoff Kingsmill - added an interactive mode and made 
$!				  modifications to preserve quotes. 
$!				  Unfortunaly this version is not compatible 
$!				  with version 1.x. To overcome this problem in
$!				  the future a version check has been added.
$! Version 2.1  Geoff Kingsmill - modified to enable nested tell.
$! Version 3.0  Geoff Kingsmill - modified to allow remote jobs to run in
$!				  batch mode. This is done by adding (the 
$!				  unix qualifier) & to the end of a command.
$!
$!------------------------------------------------------------------------------
$!
$! TELL is supported on VMS 4.7 and greater. (yuk! no if-then-else)
$!
$! This command procedure will execute commands on a remote node.
$!
$! This file needs to exist on both the local and remote nodes and requires
$! the user to either specify node access control or for the remote account to 
$! be set up for proxy access. TELL can simply reside in sys$login on the
$! remote node, which requires each user to have their own copy -or- TELL
$! can be defined as a DECnet object, requiring each system to have only one 
$! copy.
$!
$! To install TELL as a network object the system manager should enter the
$! following commands:
$!  $ MCR NCP
$!  NCP> DEFINE OBJECT TELL NUMBER 0 FILE SYS$MANAGER:TELL.COM PROXY BOTH
$!  NCP> SET OBJECT TELL ALL
$!  NCP> EXIT
$!  $
$!
$! If TELL is not installed as a network object then by default no other setups
$! are required. However if the DECnet object task has been modified to disable
$! proxy access then tell will fail. In this case the system manager should 
$! enter the following commands:
$!  $ MCR NCP
$!  NCP> DEFINE OBJECT TASK PROXY BOTH
$!  NCP> SET OBJECT TASK ALL
$!  NCP> EXIT
$!  $
$!
$! PROXY accounts allow one user direct access to another users account
$!  without having to specify username and password. The system manager
$!  can set up proxy account access from within AUTHORIZE. 
$!
$!  Example:  $ MCR AUTHORIZE>
$!            UAF> ADD/PROXY MVAX::KINGSMILL KINGSMILL/DEFAULT
$!            UAF> ADD/PROXY MVAX::OLSENK KINGSMILL
$!            UAF> EXIT
$!            $
$!  This examples allows remote user KINGSMILL and OLSENK on node MVAX
$!  into the local KINGSMILL account. The default switch enables you to
$!  specify just the nodename when addressing this node:-
$!    eg. $ DIRECTORY TVAX::
$!  If Default access is not specified then you need to specify the 
$!  nodename and username when addressing this node:-
$!    eg. $ DIRECTORY TVAX"KINGSMILL"::
$!  If you do not have proxy access then you need to specify the
$!  nodename, username and password when addressing a remote node:-
$!    eg. $ DIRECTORY TVAX"KINGSMILL password"::
$!
$! The command syntax for tell is :-
$! @TELL[/output=filename] NODENAME[,...] DCL-COMMAND [&] [|...]
$!
$!   P1 - NODENAME:
$!    More than one nodename can be specified, each being seperated by a comma.
$!    Nodename can also be a symbol and the double colon (::) after the nodename
$!     is optional.
$!    "0" can be used to specify the local nodename.
$!    If you do not have proxy access then you can specify access control 
$!      information as part of the nodename specification. 
$!    You can force a routing path by specifiy nodename of the from
$!      nodename::nodename::
$!    If two or more nodenames are specified then on completion TELL displays a 
$!      summary listing of unreachable nodes. This feature can be disabled by
$!      defining the logical name as follows:-
$!      $ DEFINE/PROCESS/NOLOG TELL_INHIBIT_SUMMARY TRUE
$!
$!   P2 to P8 - DCL COMMAND:
$!    INTERACTIVE MODE:
$!      If no DCL command is specified then you will be in interactive mode.
$!      At that point you will have a DCL session in the context of the remote
$!        node. The standard EXIT command will return you to your local node.
$!    COMMAND MODE:
$!      Here the DCL commands are specified in P2 to P8 and are separated 
$!      by a vertical bar "|". Quotes are optional and are only required to
$!      reduce the number of P parameters.
$!    DETACHED MODE:
$!      Jobs can be run in the background as a detached process by 
$!      suffixing the & (unix background command) to any command.
$!      This can be done from either Interactive or Command Mode.
$!
$!    Note that you cannot issue commands that require terminal input such as 
$!    EDIT etc. This is a restriction of the DECnet task-task software and not
$!    of TELL itself. Some of these restrictions can be overcome by defining 
$!    the verb as a foreign command. This allows many commands to be entered on 
$!    a single line.
$!        eg. $ AUTHORIZE :== $AUTHORIZE
$!            $ AUTHORIZE SHOW KINGSMILL
$!     
$!   QUALIFIERS:
$!    /Output can be used to direct the output to a filename.
$!
$!
$!   EXAMPLES:
$!
$!   $ TELL :== @SYS$MANAGER:TELL.COM
$!        All the following examples assume that the tell symbol is defined.
$!        Place this in your login.com.
$!
$!   $ TELL MVAX SHOW ERROR
$!        This command relies on proxy access to MVAX. 
$!        It does a remote $show error on node MVAX.
$!
$!   $ TELL/OUTPUT=SYS$LOGIN:TELL.OUT MVAX SHOW ERROR
$!        This command relies on proxy access to MVAX. 
$!        It does a remote $show error on node MVAX with the output
$!           directed to the file sys$login:tell.out
$!
$!   $ TELL MVAX"KINGSMILL PASSWORD":: SHOW ERROR
$!        this command supplies access control to accounts that do not have 
$!        proxy access
$!
$!   $ TELL MVAX"KINGSMILL":: SHOW ERROR
$!        Under VMS V5.x you can have a number of different proxy users pointed
$!        to one account. This example show how to gain access to a non-default
$!        proxy account. Note that only the username needs to be specified.
$!
$!   $ TELL MVAX,TVAX,SVAX SHOW ERROR
$!        this command will do a show error on MVAX, TVAX and SVAX.
$!
$!   $ DECA_CLUSTER :== MVAX,TVAX,SVAX
$!   $ TELL 'DECA_CLUSTER' SHOW ERROR
$!        this command will do a show error on MVAX, TVAX and SVAX.
$!
$!   $ TELL MVAX,TVAX "SHOW ERROR"|"SHOW NET"
$!        this command will do both a show error and a show net on both MVAX 
$!        and TVAX.
$!
$!   $ TELL MVAX,TVAX "SET DISPLAY/CREATE/NODE=MAXPS1" | "CREATE/TERM &"
$!        this command will create a detached decterm on both MVAX and TVAX
$!
$!   $ TELL MVAX
$!   MVAX_KINGSMILL>> SHOW ERROR
$!   MVAX_KINGSMILL>> SET PROC/PRIV=OPER
$!   MVAX_KINGSMILL>> MCR NCP SHOW EXEC
$!   MVAX_KINGSMILL>> EXIT
$!   $
$!        This is an example of an interactive session. The prompt indicates
$!        the remote nodename and username, whilst the ">>" is used to
$!        distinguish between a local and remote session. 
$!    
$!   $ TELL MVAX TELL TVAX SHOW ERROR
$!   $ TELL MVAX::TVAX:: SHOW ERROR
$!        Both of these commands perform the same function. 
$!        The first performs a nested TELL, ie. TELL node MVAX to TELL node
$!          TVAX to do a $ show error. Only command mode is valid with 
$!          this syntax and you must specify the command as TELL not @tell.
$!        The second example show how to do the same thing using Poor Mans
$!          Routing (PMR). This is far more efficent than the first examples
$!          and it supports both command and interactive modes.
$!
$!   CONFIGURATION SETUPS:
$!     The following logicals can be used to alter the default TELL behaviour.
$!     TELL_INHIBIT_SUMMARY to disable the statistics printout.
$!
$!   TROUBLESHOOTING:
$!     The following logicals can be used to aid in troubleshooting.
$!     TELL_VERBOSE to display the progress of both local and remote events.
$!       This can also be enabled by setting the version prefix from "V" to "X"
$!     TELL_VERIFY to turn on verify and verbose.
$!     Netserver.log on the remote node may also aid in troubleshooting.
$!     TELL_'pid'*.COM temporary file on the remote node shows the created
$!       command file if detached qualifier was specified.
$!     TELL.LOG temporary file on on the remote node showing the output
$!       from tell_'pid'.com
$!
$!------------------------------------------------------------------------------
$!
$! TELL_VERBOSE is used to display the progress of both local and remote events
$!
$ verbose = f$trnlnm("tell_verbose")	!verbose debug mode true or false
$! enable verbose if the current version is eXperimental
$ if f$extract(0,1,version) .eqs. "X" then verbose := true	
$! enable verbose if verify has been turned on
$ if f$environment("verify_procedure") then verbose := true
$!
$! define commonly used foreign commands here
$!
$ esc[0,8] = %x1b
$ bon  = esc+"[1m"	!turn on  bold printing
$ boff = esc+"[0m"	!turn off bold printing
$ say := write sys$output
$!
$! Define some commonly used System Manager commands as foreign to enable
$! them to be used in a single command.
$!
$ install 	:= $install/command_mode
$ sysgen 	:= $sysgen
$ latcp		:= $latcp
$ ncp 		:= $ncp
$ authorize 	:= $authorize
$ tell          := @'f$environment("procedure")'
$
$ if f$trnlnm("sysuaf")     .eqs. "" then -
     define sysuaf   sys$system:sysuaf.dat
$ if f$trnlnm("netproxy")   .eqs. "" then -
     define netproxy sys$system:netproxy.dat
$ if f$trnlnm("rightslist") .eqs. "" then -
     define rightslist sys$system:rightslist.dat
$!
$! Find out who we are and who we're talking too.
$!
$ remnode = f$extrac(0,f$locate("::",f$trnlnm("sys$net"))+2,f$trnlnm("sys$net"))
$ nodename = f$trnlnm("sys$node")
$ node = nodename - "::"
$ cmdmode := true	!assume command mode
$!
$! If we are a network process (remote node) and we are not attempting to do
$! a nested tell (ie. tell_source logical not defined) then goto network.
$!
$  if f$mode() .eqs. "NETWORK" .and. -
      .not. f$trnlnm("tell_source",,,,,"terminal") then goto NETWORK
$
$INTERACTIVE:
$!
$! invoke this procedure to execute a command on a remote node
$!
$ nodes	  = p1		!nodes to execute command on
$ command = P2+" "+P3+" "+P4+" "+P5+" "+P6+" "+P7+" "+P8   !command to execute
$
$! nodename is a required parameter, request nodename if not supplied
$ if nodes .eqs. "" then -
     read/prompt="Nodes: "/end_of_file=loc_exit sys$command nodes 
$ if nodes .eqs. ""  then nodes = tell_default_nodes
$ nodes = f$edit(nodes,"upcase")
$
$! initialize statistics symbols
$ tell_node_pcnt = 0	!the count on the number of nodes that were reachable
$ tell_node_fcnt = 0	!the count on the number of nodes that were unreachable
$ tell_node_plst = ""	!the list of nodes that were reachable
$ tell_node_flst = ""	!the list of nodes that were unreachable
$
$ on control_y then goto loc_end
$!
$! for each node in the list, execute each command
$!
$ found_nested_tell := false
$ node_count = 0
$LOC_PROCESS_ALL_NODES_LOOP:
$ on warning then goto loc_end		  !error handler
$ nested_tell := false			  !zero nested tell status
$ node = f$element(node_count,",",nodes)
$ if node .eqs. "," then goto loc_end	  !last node completed
$ if f$extract(0,1,node) .eqs. "0" then - !translate node 0 to local nodename
  node = f$trnlnm("sys$node")-"::"+f$extract(1,255,node)
$! ensure nodename has the colons suffix
$ if f$extract(f$length(node)-2,2,node) .nes. "::" then node = node + "::"
$ sec_node = f$parse(node,,,"node")	  !secure password
$!
$!  start the server task on the remote node.
$!  The TASK= command establishes a logical link from this machine to the node,
$!  and defines the link name as SYS$NET for the remote task to establish a
$!  connection back to us. 
$!
$ say_rem := write tell_destination
$ on warning then goto loc_cleanup_fail
$ say ""
$ say esc,"[1;4;7mEstablishing link to remote node ",sec_node,esc,"[0m"
$ say ""
$ open/read/write tell_destination 'node'"task=tell"
$
$!------------------------------------------------------------------------------
$ cmd_count = 0
$LOC_FLUSH_OUTPUT:
$!
$! This section of code set's up a handshaking between the local and remote
$! nodes. First we establish the %VERSION to handle any compatability issues
$! between versions. The we find the remote version, nodename (to distinguish
$! cluster alias nodes) and username. Then we tell the remote node whether we
$! require verbose information (a step by step account of what's happening),
$! whether we're in command or interactive mode and finally the command to 
$! execute.
$! 
$! We we've done all commands for that node we close the network connection
$! and sequence through the remaining nodes (if any)
$!
$ read tell_destination record
$
$LOC_TRY_VERBOSE:
$ if record .nes. "%VERBOSE" then goto loc_try_version
$ ! remode node is requesting whether we require a step by step account of what
$ ! going on.
$ say_rem verbose
$ goto loc_flush_output
$
$LOC_TRY_VERSION:
$ if record .nes. "%VERSION" then goto loc_try_nodename
$ ! compare the local and remote versions of TELL. If a mistmatch is found
$ ! then give a warning message to the user. 
$ if verbose then say -
     nodename," Requesting remote tell version on node ",sec_node
$ if verbose then say nodename," Local tell version is: ",version
$ if verbose then read tell_destination rem_message
$ if verbose then say rem_message
$ read tell_destination rem_version
$ if version .nes. rem_version then Say -
     bon,"WARNING, TELL version mismatch   ",boff
$ if version .nes. rem_version then Say -
     bon,"LOCAL VERSION: ",version,"   REMOTE VERSION: ",rem_version,boff
$ if version .nes. rem_version then Say ""
$ goto loc_flush_output
$
$LOC_TRY_NODENAME:
$ if record .nes. "%NODENAME" then goto loc_try_username
$ ! find out the remote nodename. This is required to be able to decide which
$ ! node we are using when a cluster alias is specified.
$ if verbose then SAY nodename," Requesting remote nodename on node ",sec_node
$ if verbose then read tell_destination rem_message
$ if verbose then say rem_message
$ read tell_destination rem_nodename
$ rem_node = rem_nodename - "::"
$ goto loc_flush_output
$
$LOC_TRY_USERNAME:
$ if record .nes. "%USERNAME" then goto loc_try_command
$ ! find out the remote username. If we are using the DECnet account then
$ ! TELL will abort as this is a security risk.
$ if verbose then SAY nodename," Requesting remote username on node ",sec_node
$ if verbose then read tell_destination rem_message
$ if verbose then say rem_message
$ read tell_destination rem_username
$ goto loc_flush_output
$
$LOC_TRY_COMMAND:
$ if record .nes. "%COMMAND" then goto loc_catchall
$ ! Send command to the remote node.
$ goto loc_cmd_loop
$ 
$LOC_CATCHALL:
$ if record .eqs. "" then record = " "	!overcome symbol error if no output
$ ! None of the above conditions was fulfilled. Therefore return the output to
$ ! the local user. Write/symbol is used to provide us with a larger output 
$ ! buffer required for certain operations
$ write/symbol sys$output record
$ goto loc_flush_output
$ 
$!------------------------------------------------------------------------------
$! Go into a loop, extracting a command and executing it on this node.
$LOC_CMD_LOOP:
$!
$! if command supplied then send command, otherwise we're in interactive mode 
$! and we need to ask for a command,
$!
$ if f$edit(command,"collapse") .nes. "" then goto loc_cmd_loop_cmdmode
$ cmdmode := false		!interactive mode
$!
$! the DECnet task currently does not support full duplex operation. ie. you
$! cannot define both sys$input and sys$output to sys$net. Therefore prohibit 
$! interactive mode on nested TELL's.
$!
$ if f$trnlnm("tell_source",,,,,"terminal") then say -
     "ERROR: You cannot user Interactive Mode when doing a nested TELL"
$ if f$trnlnm("tell_source",,,,,"terminal") then goto loc_cleanup_continue
$!
$! the inquire command enables command recall but stuffs up quotes
$! the read command retains quotes but disables command recall
$! We are in interactive mode so get the a command to execute.
$!
$ if verbose then say ""
$ read/end=loc_end/prompt="''rem_node'_''rem_username'>> " sys$command cmd_text
$ if verbose then say ""
$ if f$edit(f$extract(0,3,cmd_text),"upcase") .eqs. "EXI" then -
     goto loc_cleanup_pass
$ goto loc_cmd_write
$
$LOC_CMD_LOOP_CMDMODE:
$!
$! We are in Command mode, so break out next command. If we are doing a
$! nested TELL then send all commands to the remote node.
$!
$ ! If this is a nested tell and we've already sent the command then go and 
$ ! the close link.
$ if nested_tell then goto loc_cleanup_pass
$ ! mark the fact that we've sent the command
$ if f$edit(f$extract(0,f$locate(" ",command),command),"trim") .eqs. "TELL" -
     then nested_tell := true
$ if nested_tell then found_nested_tell := true
$ if nested_tell then cmd_text = command
$ if nested_tell then goto loc_cmd_loop_cmdmode_1
$ ! if this is not a nested tell session then break out next command
$ cmd_text = f$element(cmd_count,"|",command)
$ if cmd_text .eqs. "|" then goto loc_cleanup_pass
$ cmd_text = f$edit(cmd_text,"trim")
$!
$! strip off leading and trailing quotes if supplied. This is used to make
$! command syntax more fault tolerant. This corrects the following syntax:-
$! @tell mvax show error|"show process"
$!                       ^            ^
$!                    removes these quotes
$!
$LOC_CMD_LOOP_CMDMODE_1:
$ if f$extract(0,1,cmd_text) .nes. """" then goto loc_cmd_write
$ cmd_text = f$extract(1,255,cmd_text)
$ cmd_text = f$extract(0,f$length(cmd_text)-1,cmd_text)
$
$LOC_CMD_WRITE:
$!
$! Send the command to be executed and the current mode (interactive or command)
$! to the remote node. The server task reads  the command and then executes it.
$!
$ if verbose then SAY nodename," Sending  ",sec_node," command - { ", -
     cmd_text," }"
$ cmd_count = cmd_count + 1		!next command in list
$ say_rem cmd_text
$ if verbose then read tell_destination rem_message
$ if verbose then say rem_message
$ say_rem cmdmode
$ if verbose then read tell_destination rem_message
$ if verbose then say rem_message
$ goto loc_flush_output
$
$LOC_CLEANUP_FAIL:
$!
$! We came here due to an error talking to the remote node.
$! Log Statistics, close link and proceed to the next node
$!
$ ! keep a count of unreachable nodes
$ tell_node_fcnt = tell_node_fcnt+1
$ if tell_node_flst .nes. "" then -
     tell_node_flst = tell_node_flst + "," + node - "::"
$ if tell_node_flst .eqs. "" then tell_node_flst = node - "::"
$ got loc_cleanup_continue
$
$LOC_CLEANUP_PASS:
$!
$! We came here as all commands were executed on the remote node.
$! Log Statistics, close link and proceed to next node
$!
$ ! keep a count of reachable nodes
$ tell_node_pcnt  = tell_node_pcnt + 1
$ if tell_node_plst .nes. "" then -
     tell_node_plst = tell_node_plst + "," + node - "::"
$ if tell_node_plst .eqs. "" then tell_node_plst == node - "::"
$
$LOC_CLEANUP_CONTINUE:
$ link_was_open = f$trnlnm("tell_destination",,,,,"terminal")
$ if f$trnlnm("tell_destination",,,,,"terminal") then -
     close/nolog tell_destination
$ if verbose .and. link_was_open then say nodename," Connection closed."
$ node_count = node_count + 1		!point to next node in list
$ goto loc_process_all_nodes_loop	!execute cmds on the next node
$!
$LOC_END:
$ if f$trnlnm("tell_destination",,,,,"terminal") then -
     close/nolog tell_destination
$ if f$mode() .nes. "NETWORK" then say ""
$ if f$trnlnm("tell_inhibit_summary") .or. -
     node_count .le. 1 .or. .not. cmdmode then goto loc_exit
$ tell_node_tcnt = tell_node_pcnt+tell_node_fcnt
$ if found_nested_tell then goto loc_exit
$ say bon,"----------------------------------------"+-
          "----------------------------------------",boff
$ say bon,"T E L L   S T A T U S   R E P O R T ",boff
$ say ""
$ say "TOTAL NODE COUNT: ''tell_node_tcnt'"
$ say "NUMBER OF REACHABLE NODES: ''tell_node_pcnt'"
$ if tell_node_fcnt .eq. 0 then -
$ say "NUMBER OF UNREACHABLE NODES: ''tell_node_fcnt'"
$ if tell_node_fcnt .ne. 0 then say bon,-
      "NUMBER OF UNREACHABLE NODES: ''tell_node_fcnt'",boff
$ if tell_node_fcnt .ne. 0 then -
     say bon,"THE FOLLOWING NODES WERE UNREACHABLE:",boff
$ if tell_node_fcnt .ne. 0 then say bon,tell_node_flst,boff
$ say ""
$
$LOC_EXIT:
$ if f$trnlnm("tell_destination",,,,,"terminal") then -
     close/nolog tell_destination
$ vb=f$verify(vb) !! put verify back the way we found it...
$ exit
$
$!------------------------------------------------------------------------------
$NETWORK:
$ set noverify	!verify must be disabled 
$!
$! This procedure executes a command sent from another HOST node.
$! If an error occurs, simply close the network connection.
$!
$! Establishing the connection with the parent node and route all output 
$! to that node.
$!
$ on warning then goto rem_cleanup
$ open/read/write tell_source sys$net
$ say_rem := write tell_source
$ define/nolog sys$output tell_source
$
$ say_rem "%VERBOSE" 
$ !find out if we are to supply verbose information.
$ read/error=rem_cleanup/end_of_file=rem_cleanup -
      /error=rem_cleanup tell_source verbose
$ if verbose then define/nolog tell_verbose true
$ if .not. verbose .and. f$trnlnm("tell_verbose") then deass tell_verbose
$ if verbose then say_rem bon,nodename," Connection Established.",boff
$
$ say_rem "%VERSION"
$ ! tell them our version of TELL.
$ if verbose then say_rem bon,nodename," Remote tell version is: ",version,boff
$ say_rem version
$
$ say_rem "%NODENAME"
$ ! tell them our nodename
$ if verbose then say_rem bon,nodename," Remote nodename is: ",nodename,boff
$ say_rem nodename
$
$ say_rem "%USERNAME"
$ ! tell them our username
$ username = f$edit(f$getjpi("","username"),"collapse,trim")
$ if verbose then say_rem bon,nodename," Remote username is: ",username,boff
$ say_rem username
$
$!
$! Abort the connection if we are running under the default DECNET account.
$! This is a security risk.
$!
$ wait 00:00:02
$ if username .eqs. "DECNET" then SAY bon,"ERROR: ",-
     "You are not authorized to use the default DECNET account...",boff
$ if username .eqs. "DECNET" then SAY bon,nodename," Closing Connection...",boff
$ if username .eqs. "DECNET" then goto rem_cleanup
$
$REM_CMD_LOOP:
$! 
$! Ask the host which command he requires us to execute.
$!
$ say_rem "%COMMAND"
$ read/error=rem_cleanup/end_of_file=rem_cleanup/error=rem_cleanup -
      tell_source command
$ if verbose then say -
      bon,nodename," Received ",remnode," command - { ",command," }",boff
$ read/error=rem_cleanup/end_of_file=rem_cleanup/error=rem_cleanup -
      tell_source cmdmode
$! does this job need to run in background (detached) mode?
$ if f$ext(f$len(f$edit(command,"trim"))-1,1,f$edit(command,"trim")) -
     .nes. "&" then goto rem_non_detached
$! execute the command in detached mode
$ if verbose then say_rem bon,nodename, -
     " Executing command in detached mode.",boff
$ if verbose then say_rem ""
$ i = 0
$REM_FILENAME_LOOP:
$ i = i+1
$ outfile := 'f$trnlnm("sys$login")'tell_'f$ext(4,4,f$getj("","pid"))'_'i'.tmp;1
$ if f$search(outfile,1) .nes. "" then goto rem_filename_loop
$! we now have a unique filename
$ open/write out 'outfile'
$ write out "$!*** this is a temporary file created by tell.com ***"
$ write out "$ set noverify"
$ write out "$ set noon
$ write out "$! set process name"
$ write out "$ i = 0
$ write out "$nameloop:
$ write out "$ i = i + 1
$ write out "$ define/user/nolog sys$error nl:
$ write out "$ define/user/nolog sys$output nl:
$ write out "$ set process/name="+-
        "''f$edit(f$extr(0,7,f$getj("","username")),"trim,lowercase")'_tell_'i'"
$ write out "$ if .not. $status then goto nameloop
$ write out "$! we are here because we have a unique process name
$ ! set display if known
$ if f$trnlnm("decw$display") .eqs. "" then goto rem_no_display
$ define/user sys$output nl:
$ show displ/sym
$ write out "$ Set Display/Create/Node=",DECW$DISPLAY_NODE, -
            "/Trans=",DECW$DISPLAY_TRANSPORT, -
            "/Screen=",DECW$DISPLAY_SCREEN, -
            "/Server=",DECW$DISPLAY_SERVER
$REM_NO_DISPLAY:
$ write out "$ write sys$output ""Starting Command"""
$! strip off backgroud command "&"
$ write out "$ command := ",f$ext(0,f$len(f$edit(command,"trim"))-1,f$edit(command,"trim"))
$ write out "$ write sys$output ""$ "",command"
$ write out "$ ",command
$ write out "$ write sys$output ""Finished Command"""
$ write out "$loop:"
$ write out "$! this loop prevents the program from exiting if a "+-
            "subprocess exists"
$ write out "$ wait 00:00:15"
$ write out "$ if f$getjpi("""",""prccnt"") .ne. 0 then goto loop"
$ write out "$ delete/nolog 'f$env(""procedure"")'"
$ write out "$ purge/keep=2/nolog sys$login:tell.log"
$ write out "$ exit"
$ close out
$!
$ Say "Starting Detached Process"
$ Run/detached/authorize sys$system:loginout.exe -
     /input='outfile'/output='f$trnlnm("sys$login")'tell.log
$ goto rem_command_done 
$!
$REM_NON_DETACHED:
$! execute command in either Interactive or Command mode
$ if verbose .and. .not. cmdmode then -
      say_rem bon,nodename," Running in Interactive Mode.",boff
$ if verbose .and. cmdmode then -
      say_rem bon,nodename," Running in Command Mode.",boff
$ if verbose then say_rem bon,nodename," Executing command.",boff
$ if verbose then say_rem ""
$ !
$ ! if command mode then return nodename, username and received command 
$ ! before executing the command.
$ ! if interactive mode then skip this as we already know the information.
$ !
$ if cmdmode then -
     say_rem f$edit(node+"_"+username,"upcase,collapse"),">> ",command
$ set noon
$ 'COMMAND'	!at last
$ if verbose then say_rem ""
$ if verbose then say_rem bon,nodename," Command executed.",boff
$REM_COMMAND_DONE:
$ on warning then goto rem_cleanup
$ goto rem_cmd_loop
$!
$REM_CLEANUP:
$ deassign sys$output
$ close/nolog tell_source
$
