$ saved_verify = 'f$verify(0)'
$!TELL.COM	 
$ version := V3.1-9
$!
$! AUTHOR: Geoff.Kingsmill@compaq.com
$!
$! Version 1.1           Geoff Kingsmill 
$!   - fixed a number of bugs and added a summary report.
$! Version 2.0           Geoff Kingsmill 
$!   - added an interactive mode and made modifications to preserve quotes. 
$!     Unfortunaly this version is not compatible with version 1.x. To 
$!     overcome this problem in the future a version check has been added.
$! Version 2.1           Geoff Kingsmill 
$!  - modified to enable nested tell.
$! Version 3.0  Oct-95 Geoff Kingsmill 
$!  - modified to allow remote jobs to run in batch mode. This is done by 
$!    adding (the unix qualifier) & to the end of a command.
$! Version 3.1  Feb-96   Geoff Kingsmill 
$!  - added tcp/ip support. rlogin used in interactive mode, rsh used in 
$!    command mode. tcpip nodenames must be suffixed with a ":"
$!  - removed VMS V4.7 support. Now requires VMS V5.0 or greater.
$! Version 3.1-1  Apr-96   Geoff Kingsmill 
$!  - DAP errors reported when doing a dir node:: and login.com contained
$!    a tell command. Tell will now exit if mode is network and object
$!    is not task (object number 0).
$!  - An error was reported when you did a tell to another node and that
$!    node users login.com contained anther tell command. This is now
$!    detected and then tell contained in login.com is not executed.
$! Version 3.1-2  May-96   Geoff Kingsmill 
$!  - Fix blank process name when creating detached process
$! Version 3.1-3  May-96   Geoff Kingsmill 
$!  - Node can now point to a file containing a list of nodes eg. @vaxnodes.dat
$! Version 3.1-4  Jun-96   Geoff Kingsmill 
$!  - prevented tell summary from failing due to symbol overflow.
$!  - added bell when node unreachable
$! Version 3.1-5  Mar-98   Geoff Kingsmill 
$!  - verify state was not being restored on exit
$! Version 3.1-6  Jul-99   Geoff Kingsmill 
$!  - rsh command failed to output reason for error
$! Version 3.1-7  Jan-00   Geoff Kingsmill 
$!  - added TELL_UNREACHABLE_NODES global symbol which provides a list of
$!    unreachable nodes on exiting
$!  - added a <CR><LF> at the end of the displayed command line when a 
$!    TCPIP rsh commands is issued to prevent the output on UNIX machines
$!    from overwriting the command line. This ensures that the output on
$!    UNIX machines is displayed on a new line.
$!  - remote connections now abort if going to the anonymous account 
$! Version 3.1-8 Jan-00   Geoff Kingsmill
$!  - Previously if the number of failed nodes exceeded the length of a
$!    DCL symbol (255 characters) futher failures were not logged. This is
$!    nolonger the case. The following global symbols are now defined when
$!    exiting tell. The symbol TELL_UNREACHABLE_NODES is nolonger defined.
$!      TELL_FNODE - the number of nodes that failed 
$!      TELL_FNODE_SYM - a count of the number of TELL_FNODE_x symbols
$!      TELL_FNODE_x - the symbol containing a list of nodenames which failed.
$!      Therefore, if TELL_FNODE_SYM == 2 then the symbol TELL_FNODE_1 and
$!        TELL_FNODE_2 will be defined.
$! Version 3.1-9 Jun-00   Geoff Kingsmill
$!  - Prevent the command exit from hanging tell
$!
$!------------------------------------------------------------------------------
$!
$! This command procedure will execute commands on a remote node.
$!
$! This file needs to exist on the local node and the remote node if a decnet
$! nodename is specified. It requires the user to either specify node access 
$! control or for the remote account to be set up for proxy access. TELL can 
$! simply reside in sys$login on the remote node, which requires each user to 
$! have their own copy -or- TELL can be defined as a DECnet object, requiring 
$! each system to have only one copy.
$!
$! To install TELL as a network object the system manager should enter the
$! following commands:
$!DECnet/IV
$!  $ MCR NCP
$!  NCP> DEFINE OBJECT TELL NUMBER 0 FILE SYS$MANAGER:TELL.COM PROXY BOTH
$!  NCP> SET OBJECT TELL ALL
$!  NCP> EXIT
$!  $
$!DECnet/OSI
$!  $@sys$startup:net$configure application_add tell "" -
$!  "{name=tell}||sys$manager:tell.com|true|true|true|true|true||||1" 
$!
$! If TELL is not installed as a network object then by default no other setups
$! are required. However if the DECnet object task has been modified to disable
$! proxy access then tell will fail. In this case the system manager should 
$! enter the following ncp command or equivalent ncl command.
$!  $ MCR NCP
$!  NCP> DEFINE OBJECT TASK PROXY BOTH
$!  NCP> SET OBJECT TASK ALL
$!  NCP> EXIT
$!  $
$!
$! PROXY accounts allow one user direct access to another users account
$!  without having to specify username and password. 
$!DECNET
$!  The system manager can set up proxy account access from within AUTHORIZE. 
$!  Example:  $ MCR AUTHORIZE>
$!            UAF> ADD/PROXY MVAX::KINGSMILL KINGSMILL/DEFAULT
$!            UAF> ADD/PROXY MVAX::OLSENK KINGSMILL
$!            UAF> EXIT
$!TCPIP:
$!  To prevent the need to use passwords on the RSH command proxies can be
$!  set up to authorize remote user access. To set up remote user access
$!  on a UNIX machine, or TCPWARE/MULTINET under VMS you specify the remote
$!  node and username in the .rhosts file which resides in the users home
$!  directory. With UCX you use the "UCX> ADD PROXY command"
$!  Example:  [TCPWARE/MULTINET]
$!              $type sys$login:.rhosts 
$!              mvax kingsmill
$!              mvax olsenk
$!              $
$!            [UNIX]
$!              #more .rhosts 
$!              mvax kingsmill
$!              mvax olsenk
$!            [UCX]
$!              UCX> ADD PROXY KINGSMILL/HOST=MVAX/REMOTE_USER=KINGSMILL
$!              UCX> ADD PROXY KINGSMILL/HOST=MVAX/REMOTE_USER=OLSENK
$!
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
$!    Nodename can be a symbol 
$!    Nodename can be a file containing a list of nodes (one node per line)
$!      specified by an @ prefix (@file). The default extension is .dat.
$!      Comments should start with a ! or #.
$!    A double colon (::) after the nodename is optional and specifies
$!      decnet transport. 
$!    A single colon (:) after the nodename specifies tcpip transport.
$!    "0" can be used to specify the local nodename.
$!    If you do not have proxy access then you can specify access control 
$!      information as part of the nodename specification (node"user password")
$!    You can force a routing path by specifiy nodename of the from
$!      nodename::nodename::
$!    If in command mode and two or more nodenames are specified then on 
$!      completion TELL displays a summary listing of unreachable nodes. This 
$!      feature can be disabled by defining the logical name as follows:-
$!      $ DEFINE/PROCESS/NOLOG TELL_INHIBIT_SUMMARY TRUE
$!    If node not specified then the symbol TELL_DEFAULT_NODES is used if
$!      defined.
$!
$!   P2 to P8 - DCL COMMAND:
$!    INTERACTIVE MODE:
$!      If no DCL command is specified then you will be in interactive mode.
$!      At that point you will have a DCL session in the context of the remote
$!        node. The standard EXIT command will return you to your local node.
$!      If a decnet nodename is specified then the tell decnet object will be 
$!        used to process the command on the remote node.
$!      If a tcpip nodename is specified then an rlogin command is used
$!        used to process the command on the remote node. A password can
$!        be specified but will not be used.
$!    COMMAND MODE:
$!      Here the DCL commands are specified in P2 to P8 and are separated 
$!        by a vertical bar "|". Quotes are optional and are only required to
$!        reduce the number of P parameters.
$!      If a decnet nodename is specified then the tell decnet object will be 
$!        used to process the command on the remote node.
$!      If a tcpip nodename is specified then an rsh command is used
$!        used to process the command on the remote node. 
$!    DETACHED MODE:
$!      Jobs can be run in the background as a detached process by 
$!      suffixing the & (unix background command) to any command.
$!      This can be done from either Interactive or Command Mode.
$!
$!    EXIT STATUS:
$!      On exiting the following global symbols will be defined.
$!      TELL_FNODE - the number of nodes that were either unreachable
$!        or had a login failure.
$!      TELL_FNODE_SYM - a count of the number of TELL_FNODE_x symbols
$!      TELL_FNODE_x - the symbol containing a list of nodenames that
$!        were either unreachable or had a login failure. Therefore, if 
$!        TELL_FNODE_SYM == 2 then the symbol TELL_FNODE_1 and
$!        TELL_FNODE_2 will be defined.
$!      The symbol TELL_UNREACHABLE_NODES is nolonger defined.
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
$!   $ TELL MVAX,UNIXA"geoffk":
$!        This command will allow you to interactively enter commands
$!        on both MVAX and UVAX. Default proxy access is used to MVAX
$!	  and the geoffk account will be used on UNIXA (unix machine).
$!
$!   $ TELL MVAX SHOW ERROR
$!        This command relies on proxy access to MVAX. 
$!        It does a remote $show error on node MVAX.
$!
$!   $ TELL UNIXA: "date"
$!        This command relies on proxy access to UNIXA. 
$!        It does an "$RSH UNIXA date" command.
$!
$!   $ TELL_DEFAULT_NODES == "MVAX"
$!   $ TELL "" SHOW ERROR
$!        This command uses the symbol tell_default_nodes to obtain the
$! 	  nodename and relies on proxy access to node MVAX. 
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
$!   $ TYPE DECA_CLUSTER.DAT
$!   MVAX
$!   TVAX
$!   SVAX
$!   $ TELL @DECA_CLUSTER SHOW ERROR
$!        this command will do a show error on MVAX, TVAX and SVAX.
$!
$!   $ TELL MVAX,UVAX: "SHOW ERROR"|"SHOW NET"
$!        this command will do both a show error and a show net on 
$!        MVAX (using DECnet) and UVAX (using TCPIP [RSH]).
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
$!     SYS$SCRATCH - temporary files are written to sys$scratch. If you don't
$!       want the temporary files written to sys$login then define sys$scratch.
$!
$!   TROUBLESHOOTING:
$!     The following logicals can be used to aid in troubleshooting.
$!     TELL_VERBOSE to display the progress of both local and remote events.
$!       This can also be enabled by setting the version prefix from "V" to "X"
$!     TELL_VERIFY to turn on verify and verbose.
$!     Netserver.log on the remote node may also aid in troubleshooting.
$!     TELL_'pid'_*.TMP temporary file on the remote node shows the created
$!       command file if detached qualifier was specified.
$!     TELL_'pid'_*.LOG temporary file on on the remote node showing the output
$!       from tell_'pid'_*.tmp
$!
$!------------------------------------------------------------------------------
$!
$! TELL_VERBOSE is used to display the progress of both local and remote events
$!
$ if f$trnlnm("tell_verify") then set verify
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
$ bell[0,8] = %x07
$ cr[0,8]   = 13
$ lf[0,8]   = 10
$ crlf      = cr+lf
$ say := write sys$output
$!
$! Define some commonly used System Manager commands as foreign to enable
$! them to be used in a single command.
$!
$ sysman	:= $sysman
$ ncl		:= $ncl
$ latcp		:= $latcp
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
$ transport = ""
$!
$! Remove any previously defined TELL Global Symbols
$!
$ if f$type(tell_fnode) .nes. "" then -
     delete/symbol/global tell_fnode
$ if f$type(tell_fnode_sym) .nes. "" then -
     delete/symbol/global tell_fnode_sym
$ i = 1
$INIT_LOOP:
$ if f$type(tell_fnode_'i') .nes. ""
$ then
$   delete/symbol/global tell_fnode_'i'
$   i = i + 1
$   goto INIT_LOOP
$ endif
$!
$! If sys$net is defined then we are a network process. If this is not an
$! object 0 (task) then exit, otherwise we may/will interfere with other
$! network jobs. eg. if the following does not exist and tell is
$! used within your login.com then a $dir 0:: will fail with a DAP error.
$!
$ if f$trnlnm("sys$net") .nes. "" 
$ then
$   sys$net = f$trnlnm("sys$net")
$   flag = f$locate("::",sys$net)
$   slash = f$locate("/",sys$net)
$   object_number = f$cvui(0,8,f$extract(slash+21,1,sys$net))
$   if object_number .nes. 0 
$   then 
$     say "TELL can only run as object task"
$     goto exit 
$   endif
$ endif
$!
$! If we are a network process (remote node) and we are not attempting to do
$! a nested tell (ie. tell_source logical not defined) then goto network.
$!
$  if f$mode() .eqs. "NETWORK" .and. p1 .nes. "" .and. -
      .not. f$trnlnm("tell_source",,,,,"terminal")
$  then
$     acommand = P2+" "+P3+" "+P4+" "+P5+" "+P6+" "+P7+" "+P8   
$     say "Another instance of TELL already active"
$     say "...Aborting TELL ",p1," ",acommand
$     goto exit
$  endif
$  if f$mode() .eqs. "NETWORK" .and. -
      .not. f$trnlnm("tell_source",,,,,"terminal") then goto NETWORK
$!
$! tell calls itself using P1 "%CREATE_MBX%" to create tell mailbox
$! which is used to pass the user's password to a detached process
$! when a tcpip node is specified.
$ if p1 .eqs. "%CREATE_MBX%" then goto create_mbx_noinit
$!
$!
$INTERACTIVE:
$!
$! invoke this procedure to execute a command on a remote node
$!
$ nodes	  = p1		!nodes to execute command on
$ command = P2+" "+P3+" "+P4+" "+P5+" "+P6+" "+P7+" "+P8   !command to execute
$
$! nodename is a required parameter, request nodename if not supplied
$ if f$type(tell_default_nodes) .eqs. "" then tell_default_nodes == ""
$ if nodes .eqs. ""  then nodes = tell_default_nodes
$ if nodes .eqs. "" then -
     read/prompt="Nodes: "/end_of_file=loc_exit sys$command nodes 
$ nodes = f$edit(nodes,"upcase")
$
$! initialize statistics symbols
$ tell_pnode     =  0  ! the count on the number of nodes that were reachable
$ tell_pnode_1   =  "" ! the list of nodes that were reachable
$ tell_fnode     == 0  ! the count on the number of nodes that were unreachable
$ tell_fnode_sym == 0  ! the number of tell_fnode_x symbols
$!tell_fnode_x   == "" ! the list of nodes that were unreachable
$ if f$trnlnm("tcpware") .nes. "" 
$ then
$   if f$type(rsh) .eqs. "" then rsh := $tcpware:rsh
$   if f$type(rlogin) .eqs. "" then rlogin := $tcpware:rlogin
$ endif
$ if f$trnlnm("multinet") .nes. "" 
$ then
$   if f$type(rsh) .eqs. "" then rsh := multinet rsh
$   if f$type(rlogin) .eqs. "" then rlogin := multinet rlogin
$ endif
$ on control then goto loc_end
$!
$! for each node in the list, execute each command
$!
$ found_nested_tell := false
$ node_count = 0      !counter for all nodes
$ node_counter = 0    !counter for p1 nodes entries comma separated
$LOC_PROCESS_ALL_NODES_LOOP:
$! close node file if its still open
$ if f$trnlnm("tellfile") .nes. "" then close tellfile
$! make note so we know to return here
$ loc_process_all_nodes_loop := loc_process_all_nodes_loop
$ on warning then goto loc_end		  !error handler
$ nested_tell := false			  !zero nested tell status
$ node = f$element(node_counter,",",nodes)
$ if node .eqs. "," then goto loc_end	  !last node completed
$ node_counter = node_counter + 1         !point to next node in list
$
$! see if a file nodelist was requested
$ if f$extract(0,1,node) .nes. "@" then goto loc_process_command
$! remove the @ sign from the file specification
$ tfile = f$extract(1,255,node)
$! if the file is specified by a logical then translate that logical
$ if f$trnlnm(tfile) .nes. "" then tfile = f$trnlnm(tfile)
$! if the file does not contain an extension (file.ext) then add the 
$! default (.dat)
$ if f$length(tfile) .eq. f$loc(".",tfile) then tfile = tfile + ".dat"
$! the node(s) are specified in a file so open the file and look for nodes
$ open/read/error=loc_process_all_nodes_err tellfile 'tfile'
$ goto loc_process_all_nodes_loop_list     !file has been opened, now process
$LOC_PROCESS_ALL_NODES_ERR:
$ say ""
$ say esc,"[1;4;7mProcessing Node List ",node,esc,"[0m"
$ say ""
$ say "ERROR: Node list ",node," file does not exist",bell,bell,bell
$ node_count = node_count + 1		!keep a node counter
$ tell_fnode == tell_fnode+1
$ if tell_fnode_sym .eq. 0 then tell_fnode_sym == 1
$ if f$type(tell_fnode_'tell_fnode_sym') .eqs. "" then -
     tell_fnode_'tell_fnode_sym' == ""
$ if tell_fnode_'tell_fnode_sym' .eqs. ""
$ then
$   tell_fnode_'tell_fnode_sym' == node - "::"
$ else
$   if (f$length(tell_fnode_'tell_fnode_sym') + f$length(node - "::")) .gt. 255
$   then
$     !create a new tell_fnode_'tell_fnode_sym' symbol if we exceed 255 chars.
$     tell_fnode_sym == tell_fnode_sym + 1
$     tell_fnode_'tell_fnode_sym' == node - "::"
$   else
$     tell_fnode_'tell_fnode_sym' == tell_fnode_'tell_fnode_sym' +","+ node-"::"
$   endif
$ endif
$ goto loc_process_all_nodes_loop
$! file has been opened without error. Now go and process commands.
$LOC_PROCESS_ALL_NODES_LOOP_LIST:
$! make note so we know to return here
$ loc_process_all_nodes_loop := loc_process_all_nodes_loop_list
$! read nodes from file. If we are at the end of the list then we'll
$! go back top loc_process_all_nodes_loop to process the p1 specified nodes
$ read/error=loc_process_all_nodes_loop tellfile node
$! ignore blank lines and comment lines (!, # or $!)
$ if node .eqs. "" .or. f$extract(0,1,node) .eqs. "!" .or. -
     f$extract(0,1,node) .eqs. "#" .or. f$extract(0,2,node) .eqs. "$!" then -
     goto loc_process_all_nodes_loop_list
$! strip off any trailing comments
$ node = f$extract(0,f$locate("#",node),node)
$ node = f$extract(0,f$locate("!",node),node)
$ node = f$edit(node,"trim")
$
$LOC_PROCESS_COMMAND:
$ if f$extract(0,1,node) .eqs. "0" then - !translate node 0 to local nodename
  node = f$trnlnm("sys$node")-"::"+f$extract(1,255,node)
$ cmd_count = 0
$!
$ bnodenm = node - "::" - ":"
$ if f$length(node) .ne. f$locate(" ",node) then -
      bnodenm = f$extract(0,f$locate(" ",node),node) + """" - "::" - ":" 
$ !determine if node decnet or tcpip
$ transport := decnet
$ if f$length(node)-1 .eq. f$length(node - "::" - ":") 
$ then 
$   transport := tcpip
$   bnodenm = bnodenm + ":"
$   goto loc_cmd_loop
$ endif
$!
$! This is for DECnet nodenames only
$! ensure nodename has the colons suffix
$ node = node - "::" + "::"
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
$ say esc,"[1;4;7mEstablishing ",transport," link to remote node ", -
          sec_node,esc,"[0m"
$ say ""
$ open/read/write tell_destination 'node'"task=tell"
$
$!------------------------------------------------------------------------------
$! cmd_count = 0
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
$! When we've done all commands for that node we close the network connection
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
$ if verbose 
$ then 
$   say nodename," Requesting remote tell version on node ",sec_node
$   say nodename," Local tell version is: ",version
$   read tell_destination rem_message
$   say rem_message
$ endif
$ read tell_destination rem_version
$ if version .nes. rem_version 
$ then 
$   Say bon,"WARNING, TELL version mismatch   ",boff
$   Say bon,"LOCAL VERSION: ",version,"   REMOTE VERSION: ",rem_version,boff
$   Say ""
$ endif
$ goto loc_flush_output
$
$LOC_TRY_NODENAME:
$ if record .nes. "%NODENAME" then goto loc_try_username
$ ! find out the remote nodename. This is required to be able to decide which
$ ! node we are using when a cluster alias is specified.
$ if verbose 
$ then 
$   say nodename," Requesting remote nodename on node ",sec_node
$   read tell_destination rem_message
$   say rem_message
$ endif
$ read tell_destination rem_nodename
$ rem_node = rem_nodename - "::"
$ goto loc_flush_output
$
$LOC_TRY_USERNAME:
$ if record .nes. "%USERNAME" then goto loc_try_command
$ ! find out the remote username. If we are using the DECnet account then
$ ! TELL will abort as this is a security risk.
$ if verbose 
$ then 
$   say nodename," Requesting remote username on node ",sec_node
$   read tell_destination rem_message
$   say rem_message
$ endif
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
$ if transport .nes. "TCPIP" then goto loc_cmd_loop_decnet
$!
$! use RLOGIN to enter interactive commands on remote tcpip node
$! here we are building the rlogin command rlogin/user=x node
$! break out node
$ node = node - ":"
$ nodenm = f$extract(0,f$locate("""",node),node) 
$ ! break out user and ignore password if specified
$ user = f$extract(1,f$locate(" ",node-nodenm)-1,node-nodenm) - """" - """" 
$ ! if user not specified then default to current username
$ if user .eqs. "" then user = f$edit(f$getj("","username"),"lowercase,trim")
$ ! rlogin
$ rlogin_command = "rlogin"
$ ! rlogin/user
$ rlogin_command = rlogin_command + "/user=" + """" + user + """"
$ if f$length(node) .eq. f$locate(" ",node)
$ then
$   ! no password was supplied. 
$   sec_node = node - "::" - ":"
$ else
$   ! remove password. Password cannot be used with rlogin command
$   sec_node = f$extra(0,f$locate(" ",node),node) + """" - "::" - ":"
$ endif
$ say ""
$ say esc,"[1;4;7mEstablishing ",transport," link to remote node ", -
          sec_node,":",esc,"[0m"
$ say ""
$ say nodenm,"_",user,">> ",rlogin_command," ",nodenm
$ say ""
$ set noon
$ 'rlogin_command' 'nodenm'
$ set on
$ goto loc_cleanup_pass
$   
$LOC_CMD_LOOP_DECNET:
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
$ if f$edit(f$extract(0,f$locate(" ",command),command),"trim") .eqs. "TELL" 
$ then
$   nested_tell := true
$   found_nested_tell := true
$   cmd_text = command
$   goto loc_cmd_loop_cmdmode_1
$ endif
$ ! this is not a nested tell session so break out next command
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
$! Process the command to be executed.
$!
$ cmd_count = cmd_count + 1		!next command in list
$!
$ if transport .nes. "TCPIP" then goto loc_cmd_write_decnet
$!
$! use RSH to pass command to remote tcpip node.
$!
$! only need to decode node name for first command
$ if cmd_count .eq. 1 
$ then 
$   ! here we are building the rsh command rsh/user=x/password=x node command
$   ! break out node
$   node = node - ":" - "::"
$   nodenm = f$extract(0,f$locate("""",node),node) 
$   ! break out user
$   user = f$extract(1,f$locate(" ",node-nodenm)-1,node-nodenm) - """" - """" 
$   if user .eqs. "" then user = f$edit(f$getj("","username"),"lowercase,trim")
$   passwd = node - nodenm - user - " "
$   ! rsh
$   rsh_cmd = "rsh"
$   ! rsh/user
$   if user .nes. "" then rsh_cmd = rsh_cmd + "/user=" + """" + user + """"
$   ! rsh/user/password
$   ! rsh_cmd - includes password, rsh_npcmd - hides the password
$   rsh_npcmd = rsh_cmd
$   if (passwd - """" - """") .nes. "" 
$   then 
$     ! command including password
$     rsh_cmd = rsh_cmd + "/password=" + passwd 
$     ! command with password concealed
$     rsh_npcmd = rsh_npcmd + "/password='hidden'"
$     ! nodename including username, with password concealed
$     sec_node = f$extra(0,f$locate(" ",node),node)+" password"+""""-"::"-":"
$     password_required = 1
$   else
$     ! nodename
$     ! nodename including username (if supplied)
$     sec_node = f$extra(0,f$locate(" ",node),node)-"::"-":"
$     password_required = 0
$   endif
$   !
$   say ""
$   say esc,"[1;4;7mEstablishing ",transport," link to remote node ", -
            sec_node,":",esc,"[0m"
$   say ""
$ endif
$ detach_cmd = 0
$ if (f$extract(f$length(cmd_text)-1,1,cmd_text) - """") .eqs. "&" then -
      detach_cmd = 1
$ ! ensure case sensitivity is retained. If command is in lowercase then
$ ! add extra quotes around command.
$ if f$edit(cmd_text,"trim") .eqs. f$edit(cmd_text,"trim,upcase")
$ then
$   !command was uppercase, so ignore quotes and hope rsh/lower works
$   !cmd_ndtext is the command text less the detach command (&)
$   cmd_ndtext = cmd_text
$   if detach_cmd then cmd_ndtext = f$extract(0,f$length(cmd_text)-1,cmd_text)
$ else
$   !command was lowercase, so preserve by adding quotes
$   !cmd_ndtext is the command text less the detach command (&)
$   cmd_ndtext = """" + cmd_text + """"
$   if detach_cmd then -
       cmd_ndtext = """" + f$extract(0,f$length(cmd_text)-1,cmd_text) + """"
$   cmd_text = """" + cmd_text + """"
$ endif
$ ! now add in the command
$ ! rsh command with password
$ rsh_command = rsh_cmd + " " + nodenm + " " + cmd_text
$ ! rsh command with no password
$ rsh_npcommand = rsh_npcmd + " " + nodenm + " " + cmd_text
$ ! rsh command with no password and no detach command (&)
$ tcommand = rsh_npcmd + " " + nodenm + " " + cmd_ndtext
$ !
$ !determine if node reachable
$ set noon
$ define/user sys$error nl:
$ define/user sys$output nl:
$ 'rsh_cmd' 'nodenm' "this_is_a_dummy_command"
$ status = $status
$ set on
$ if .not. status 
$ then 
$   if status .eqs. "%X18018042" .or. status .eqs. "%X1000028C" then -
       say "%RSH-E-IVHOST, Invalid or unknown host"
$   if status .eqs. "%X1801801A" .or. status .eqs. "%X00000294" then -
       say "%RSH-E-FAILED, Login incorrect on remote node"
$   if status .nes. "%X18018042" .and. -
       status .nes. "%X1000028C" .and. -
       status .nes. "%X1801801A" .and. -
       status .nes. "%X00000294" then  -
       say f$message(status)," (",status,")"
$   goto loc_cleanup_fail
$   say
$ endif
$ !
$ say nodenm,"_",user,">> ",rsh_npcommand,crlf
$ ! if this is a detached command then we must create a command file
$ ! to run as a detached job. If password was specified then we also
$ ! need to create a Tell Mailbox to pass the password. This prevents
$ ! the need for including the password in the command file.
$ if detach_cmd
$ then 
$   if password_required 
$   then 
$     ! create tell mailbox so we can pass the password to the detached process
$     ! make sure any previous mailbox has been closed out
$     if f$trnlnm("tell_mbx") .nes. "" then close tell_mbx
$     gosub create_mbx
$     ! create the detached command file
$     gosub create_detach
$     ! pass the password to the detached process via a tell Mailbox
$     count = 0
$read_mbx_loop:
$     wait 0:0:1
$     count = count + 1
$     ! attempt to read from the tell mailbox for 20 seconds. If no response
$     ! within that time it looks like the detached job failed to start
$     ! so abort.
$     if count .eq. 20
$     then
$       if verbose then say nodename, -
           " ERROR: Detached process did not write to ''mbxdev'
$     else
$       if verbose .and. count .ne. 1 then say nodename, -
           " ...waiting for Detached process to write to Tell Mailbox"
$       read/time=0/error=read_mbx_loop tell_mbx pwdreq
$       if verbose then say nodename," Read """,pwdreq,""" from Tell Mailbox "
$       write tell_mbx passwd 
$       if verbose then say nodename," Wrote password to Tell Mailbox "
$     endif
$     if verbose then say nodename," Closing Tell Mailbox "
$     close tell_mbx
$     wait 0:0:1
$     if verbose .and. f$getdvi(mbxdev,"exists") then say nodename, -
         " ERROR: Tell Mailbox ''mbxdev' was not deleted"
$   else
$     password_required = 0
$     gosub create_detach
$   endif
$   goto loc_cmd_loop
$ endif
$ on warning then goto loc_cleanup_fail 
$ 'rsh_command'
$ on warning then goto loc_end
$ goto loc_cmd_loop
$!
$LOC_CMD_WRITE_DECNET:
$! Send the command to be executed and the current mode (interactive or command)
$! to the remote node. The server task reads  the command and then executes it.
$ if verbose then SAY nodename," Sending  ",sec_node," command - { ", -
     cmd_text," }"
$ say_rem cmd_text
$ if verbose 
$ then 
$   read tell_destination rem_message
$   say rem_message
$ endif
$ say_rem cmdmode
$ if verbose 
$ then 
$   read tell_destination rem_message
$   say rem_message
$ endif
$ goto loc_flush_output
$
$LOC_CLEANUP_FAIL:
$!
$! We came here due to an error talking to the remote node.
$! Log Statistics, close link and proceed to the next node
$!
$ ! keep a count of unreachable nodes
$ tell_fnode == tell_fnode+1
$ say bell,bell
$ if tell_fnode_sym .eq. 0 then tell_fnode_sym == 1
$ if f$type(tell_fnode_'tell_fnode_sym') .eqs. "" then -
     tell_fnode_'tell_fnode_sym' == ""
$ if tell_fnode_'tell_fnode_sym' .eqs. "" 
$ then
$   tell_fnode_'tell_fnode_sym' == bnodenm - "::"
$ else
$   if (f$length(tell_fnode_'tell_fnode_sym') + f$length(bnodenm-"::")) .gt. 255
$   then
$     !create a new tell_fnode_'tell_fnode_sym' symbol if we exceed 255 chars.
$     tell_fnode_sym == tell_fnode_sym + 1
$     tell_fnode_'tell_fnode_sym' == bnodenm - "::"
$   else
$     tell_fnode_'tell_fnode_sym'== tell_fnode_'tell_fnode_sym'+","+bnodenm-"::"
$   endif
$ endif
$ got loc_cleanup_continue
$
$LOC_CLEANUP_PASS:
$!
$! We came here as all commands were executed on the remote node.
$! Log Statistics, close link and proceed to next node
$!
$ ! keep a count of reachable nodes
$ tell_pnode = tell_pnode + 1
$ if tell_pnode_1 .eqs. "" 
$ then 
$   tell_pnode_1 = node - "::"
$ else
$   !limit list if symbol is about to exceed 255 characters.
$   if tell_pnode_1 .eqs. tell_pnode_1 - "*OVERFLOW*"
$   then
$     if (f$length(tell_pnode_1) + f$length(node-"::")) .le. 240
$     then 
$       tell_pnode_1 = tell_pnode_1 + "," + node - "::"
$     else
$       tell_pnode_1 = tell_pnode_1 + ",*OVERFLOW*"
$     endif
$   endif
$ endif
$
$LOC_CLEANUP_CONTINUE:
$ link_was_open = f$trnlnm("tell_destination",,,,,"terminal")
$ if f$trnlnm("tell_destination",,,,,"terminal") then -
     close/nolog tell_destination
$ if verbose .and. link_was_open then say nodename," Connection closed."
$ node_count = node_count + 1		!keep a node counter
$ goto 'loc_process_all_nodes_loop'     !execute cmds on the next node
$!
$LOC_END:
$ if f$trnlnm("tellfile") .nes. "" then close tellfile
$ if f$trnlnm("tell_destination",,,,,"terminal") then close tell_destination
$ if f$trnlnm("tell_mbx") then close tell_mbx
$ if f$mode() .nes. "NETWORK" then say ""
$ if f$trnlnm("tell_inhibit_summary") .or. -
     node_count .le. 1 .or. .not. cmdmode then goto loc_exit
$ tell_node_tcnt = tell_pnode+tell_fnode
$! if found_nested_tell then goto loc_exit
$ if f$mode() .eqs. "INTERACTIVE" 
$ then 
$   say bon,"----------------------------------------"+-
            "----------------------------------------",boff
$ else
$   say ""
$ endif
$ say bon,"T E L L   S T A T U S   R E P O R T ",boff
$ say "TOTAL NODE COUNT: ''tell_node_tcnt'"
$ say "NUMBER OF REACHABLE NODES: ''tell_pnode'"
$ if tell_fnode .eq. 0 
$ then 
$   say "NUMBER OF UNREACHABLE NODES: 0"
$ else
$   say bon,"NUMBER OF UNREACHABLE NODES: ''tell_fnode'",boff
$   say bon,"THE FOLLOWING NODES WERE UNREACHABLE:",boff
$   i = 1
$LOC_DE:
$   if i .le. tell_fnode_sym
$   then
$     say bon,tell_fnode_'i',boff   
$     i = i + 1
$     goto LOC_DE
$   endif
$ endif
$ say ""
$
$LOC_EXIT:
$ if f$trnlnm("tell_destination",,,,,"terminal") then -
     close/nolog tell_destination
$ goto exit
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
$ read/error=rem_cleanup/end_of_file=rem_cleanup tell_source verbose
$ if verbose 
$ then 
$   define/nolog tell_verbose true
$ else
$   if f$trnlnm("tell_verbose") then deass tell_verbose
$ endif
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
$ if username .eqs. "DECNET"  .or. username .eqs. "FAL$SERVER" .or. -
     username .eqs. "ANONYMOUS"
$ then 
$   SAY bon,"ERROR: ",-
     "You are not authorized to use the default DECNET account...",boff
$   SAY bon,nodename," Closing Connection...",boff
$   goto rem_cleanup
$ endif
$
$REM_CMD_LOOP:
$! 
$! Ask the host which command he requires us to execute.
$!
$ say_rem "%COMMAND"
$ read/error=rem_cleanup/end_of_file=rem_cleanup tell_source command
$ if verbose then say -
      bon,nodename," Received ",remnode," command - { ",command," }",boff
$ read/error=rem_cleanup/end_of_file=rem_cleanup tell_source cmdmode
$! does this job need to run in background (detached) mode?
$ if f$ext(f$len(f$edit(command,"trim"))-1,1,f$edit(command,"trim")) -
     .nes. "&" then goto rem_non_detached
$! execute the command in detached mode
$ if verbose 
$ then 
$   say_rem bon,nodename," Executing command in detached mode.",boff
$   say_rem ""
$ endif
$ tcommand = f$ext(0,f$len(f$edit(command,"trim"))-1,f$edit(command,"trim"))
$ say node,"_",username,">> ",tcommand,"&"
$ password_required = 0
$ gosub create_detach
$ goto rem_command_done
$!
$REM_NON_DETACHED:
$! execute command in either Interactive or Command mode
$ if verbose .and. .not. cmdmode then -
     say_rem bon,nodename," Running in Interactive Mode.",boff
$ if verbose .and. cmdmode then -
      say_rem bon,nodename," Running in Command Mode.",boff
$ if verbose 
$ then 
$   say_rem bon,nodename," Executing command.",boff
$   say_rem ""
$ endif
$ !
$ ! if command mode then return nodename, username and received command 
$ ! before executing the command.
$ ! if interactive mode then skip this as we already know the information.
$ !
$ if cmdmode then -
     say_rem f$edit(node+"_"+username,"upcase,collapse"),">> ",command
$ set noon
$ rsaved_verify='f$verify(0)'
$ ! don't allow just exit as this causes tell to hang
$ if f$edit(command,"upcase,trim") .nes. "EXI" .and. -
     f$edit(command,"upcase,trim") .nes. "EXIT" then -
$ 'COMMAND'	!at last
$ if rsaved_verify then set verify !'f$verify(0)'
$ if verbose 
$ then 
$   say_rem ""
$   say_rem bon,nodename," Command executed.",boff
$ endif
$REM_COMMAND_DONE:
$ on warning then goto rem_cleanup
$ goto rem_cmd_loop
$!
$REM_CLEANUP:
$ deassign sys$output
$ close/nolog tell_source
$ goto exit 
$!
$!******************************************************************************
$! This creates a command file to be run from a detach process.
$!
$CREATE_DETACH:
$ i = 0
$DETACH_FILENAME_LOOP:
$ i = i+1
$ outfile := 'f$trn("sys$scratch")'tell_'f$ext(4,4,f$getj("","pid"))'_'i'.tmp;1
$ if f$search(outfile,1) .nes. "" then goto detach_filename_loop
$ outlog  := 'f$trn("sys$scratch")'tell_'f$ext(4,4,f$getj("","pid"))'_'i'.log;1
$ if f$search(outlog,1) .nes. "" then goto detach_filename_loop
$! we now have a unique filename
$ open/write out 'outfile'
$ write out "$!*** this is a temporary file created by tell.com ***"
$ if verbose
$ then
$   write out "$ set verify"
$ else
$   write out "$ set noverify"
$ endif
$ write out "$ set noon"
$ write out "$ if f$trnlnm(""tcpware"") .nes. """" "
$ write out "$ then"
$ if verbose then -
  write out "$   write sys$output ""This node is running TCPWARE"""
$ write out "$   if f$type(rsh) .eqs. """" then rsh := $tcpware:rsh"
$ write out "$   if f$type(rlogin) .eqs. """" then rlogin := $tcpware:rlogin"
$ write out "$ endif
$ write out "$ if f$trnlnm(""multinet"") .nes. """" "
$ write out "$ then"
$ if verbose then -
  write out "$   write sys$output ""This node is running MULTINET"""
$ write out "$   if f$type(rsh) .eqs. """" then rsh := multinet rsh"
$ write out "$   if f$type(rlogin) .eqs. """" then telnet := multinet rlogin"
$ write out "$ endif
$ if verbose .and. f$trnlnm("ucx$host") .nes. "" then -
  write out "$   write sys$output ""This node is running UCX"""
$! set process name
$ write out "$ i = 0
$ write out "$nameloop:
$ write out "$ if i .le. 99
$ write out "$ then
$ write out "$   i = i + 1
$ write out "$   define/user/nolog sys$error nl:
$ write out "$   define/user/nolog sys$output nl:
$ write out "$   set process/name="+-
        "''f$edit(f$extr(0,7,f$getj("","username")),"trim,lowercase")'_tell_'i'"
$ write out "$   if .not. $status then goto nameloop
$ write out "$ else"
$ write out "$   write sys$output ""Error: Unable to set process name"""
$ write out "$ endif
$ ! set display if known
$ if f$trnlnm("decw$display") .eqs. "" .and. transport .eqs. "DECNET" 
$ then 
$   define/user sys$output nl:
$   show displ/sym
$   write out "$ Set Display/Create/Node=",DECW$DISPLAY_NODE, -
              "/Trans=",DECW$DISPLAY_TRANSPORT, -
              "/Screen=",DECW$DISPLAY_SCREEN, -
              "/Server=",DECW$DISPLAY_SERVER
$ endif
$ if password_required
$ then
$   write out "$ write sys$output ""Getting password from " +-
              "''f$trnlnm("tell_mbx")':"""
$   write out "$ open/read/write pwdmbx ",f$trnlnm("tell_mbx"),":"
$   write out "$ write pwdmbx ""please_give_me_password"""
$   write out "$ read pwdmbx hidden"
$   write out "$ close pwdmbx"
$   write out "$ wait 0:0:1"
$   write out "$ if f$getdvi(""",f$trnlnm("tell_mbx"),""",""exists"") then" +-
              " write sys$output ""WARNING: Mailbox """, -
              f$trnlnm("tell_mbx"),""" was not deleted"""
$ endif
$ write out "$ write sys$output ""-- Starting Command --"""
$ if .not. verbose
$ then
$   write out "$ type sys$input"
$   write out "$ deck/dollars=""^z"""
$   write out "$ ",tcommand
$   write out "^z"
$ endif
$ write out "$ rsaved_verify='f$verify(0)'"
$ write out "$ ",tcommand
$ write out "$ if rsaved_verify then set verify ! 'f$verify(0)'"
$ write out "$ write sys$output ""-- Finished Command --"""
$ write out "$ i = 0"
$ write out "$loop:"
$ ! this loop prevents the program from exiting if a subprocess exists
$ write out "$ if i .ne. 0 then wait 00:00:15"
$ write out "$ i = i+1"
$ write out "$ if f$getjpi("""",""prccnt"") .ne. 0 then goto loop"
$ write out "$ define/user sys$output nl:"
$ write out "$ define/user sys$error nl:"
$ write out "$ delete/before=""-0:5:0"" ''f$trn("sys$scratch")'tell_*_%.log;*"
$ write out "$ define/user sys$output nl:"
$ write out "$ define/user sys$error nl:"
$ write out "$ delete/before=""-0:5:0"" ''f$trn("sys$scratch")'tell_*_%.tmp;*"
$ if .not. verbose then write out "$ delete 'f$env(""procedure"")'"
$ write out "$ exit"
$ close out
$!
$ Run/detached/authorize sys$system:loginout.exe -
     /input='outfile'/output='outlog'
$ return
$!
$!******************************************************************************
$! This subroutine generates a Tell Mailbox so we can pass the password without
$!  having to write it into the command file
$!
$CREATE_MBX:
$ p1 = ""
$ p2 = ""
$ p3 = ""
$CREATE_MBX_NOINIT:
$ vfy = f$verify(0)
$ my_pid = f$getjpi("","pid")
$ if f$leng(p2) .gt. 0 then goto create_mbx_subprocess
$ count = ""
$! allow for multiple tell mailboxes to be created (not to exceed 32 total)
$ sanity_count = 0
$CREATE_MBX_LOOP:
$ sanity_count = sanity_count + 1
$ if sanity_count .ge. 32 then exit 4
$ if f$trnlnm("tell_mbx''count'") .eqs. "" then goto spawn
$ count = f$stri(f$inte(count)+1)
$ goto create_mbx_loop
$spawn:
$ tell_mbx = "tell_mbx" + count				! logical name used
$ procedure = f$envi("procedure")
$! spawn a subprocess creating a tell mailbox...
$ spawn/nolog/nowait/process=crembx_'my_pid' -
  @'procedure' "%CREATE_MBX%" 'my_pid''count'
$ set process/suspend/id=0
$ mailbox = "mba" + f$trnlnm(tell_mbx)
$ open/read/write 'tell_mbx' 'mailbox':			! grab mailbox
$ deassign/job 'tell_mbx'				! remove job logical
$ stop crembx_'my_pid'					! kill subprocess
$ if verbose then say nodename," ''f$fao("%CREMBX-I-CREATED, "+-
    "Mailbox !AS !AS: created.",tell_mbx,f$trnlnm(tell_mbx))
$ mbxdev = f$trnlnm("tell_mbx")
$! pipe'count' == f$fao("spawn/nowait/nolog/output=!AS:",f$trnlnm(tell_mbx))
$! grep'count' == f$fao("search !AS:",f$trnlnm(tell_mbx))
$ RETURN 1+0*f$verify(vfy)
$!
$CREATE_MBX_SUBPROCESS:
$ mailbox = f$getjpi("","tmbu")
$ define/nolog/job tell_mbx'p3' 'mailbox'		! tell parent mbx name
$CREATE_MBX_SUBPROCESS_LOOP:
$ set process/resume/id='p2'
$ wait 00:00:01.50
$ if f$extr(0,3,f$getjpi(p2,"state")) .eqs. "SUS"
$  then
$    sanity_check = f$inte("''sanity_check'")+1
$    if sanity_check .gt. 256 then exit %x4
$    goto create_mbx_subprocess_loop
$  endif
$ set process/suspend/id=0
$ RETURN 4+0*f$verify(vfy)
$!
$!******************************************************************************
$exit:
$ if saved_verify then set verify ! 'f$verify(0)'
