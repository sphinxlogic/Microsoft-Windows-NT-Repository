$ ! Procedure:	RCOMMAND.COM
$ __vfy = "VFY_''f$parse(f$environment("procedure"),,,"name")'"
$ if (f$type('__vfy') .eqs. "") then __vfy = 0
$ __vfy_saved = f$verify(&__vfy)
$ procedure = f$element(0,";",f$environment("PROCEDURE"))
$ procedure_name = f$parse(procedure,,,"NAME")
$ facility = procedure_name
$ location = f$parse(procedure,,,"DEVICE","NO_CONCEAL") -
		+ f$parse(procedure,,,"DIRECTORY","NO_CONCEAL") - "]["
$ set noon
$ on control_y then goto bail_out
$ say = "write sys$output"
$ saysym = "write/symbol sys$output"
$ ask = "@DBSLIBRARY:SYS_ASK"
$ quit = 0
$ if (f$mode() .nes. "INTERACTIVE") then goto bail_out
$ rjob_node_list = f$edit(P1,"COLLAPSE,UPCASE")
$ rjob_username = f$edit(P2,"COLLAPSE,UPCASE")
$ if (rjob_node_list .eqs. "") then rjob_node_list = "*"
$!$ default_username = f$edit(f$getjpi("","USERNAME"),"COLLAPSE,UPCASE")
$ default_username = "ON_LINE"
$ if (rjob_username .eqs. "") then rjob_username = default_username
$ ask rjob_password "''rjob_username''s password" "" ENSZ
$ password = rjob_password
$ deletee/symbol/global rjob_password
$ if (sys__control_z .or. sys__control_y) then goto bail_out
$ @dbslibrary:sys_find_nodes node_list "''rjob_node_list'"
$ say ""
$ default_transport = f$trnlnm("RSTUFF_DEFAULT_TRANSPORT")
$ default_transport = f$edit(default_transport,"COLLAPSE,UPCASE")
$ if (default_transport .eqs. "") then default_transport = "DECNET"
$ nodes_visited = 0
$ nodes_failed = 0
$ cmd_count = 0
$cmd_loop:
$   ask rjob_command'cmd_count' "Command" "" NSZ
$   if ((sys__control_z) .or. (sys__control_y)) then -
$     rjob_command'cmd_count' = ""
$   if (rjob_command'cmd_count' .eqs. "") then goto end_cmd_loop
$   cmd_count = cmd_count + 1
$  goto cmd_loop
$end_cmd_loop:
$ if (cmd_count .eq. 0) then goto end_node_loop
$ dots = f$fao("!78*.")
$ node_count = 0
$node_loop:
$   rjob_node = f$element(node_count,",",node_list)
$   node_count = node_count + 1
$   if (rjob_node .eqs. ",") then goto end_node_loop
$   if (rjob_node .eqs. "") then goto node_loop
$   transport = default_transport
$   __transport = "NODE_''rjob_node'_TRANSPORT" - "." - "." - "."
$   if (f$type('__transport') .nes. "") then transport = &__transport
$   say f$fao("!/!4*=(!6%D !8%T)!57*=!/",0,0)
$   say "%''facility'-I-CONNECT, connecting to node ''rjob_node'"
$   nodes_visited = nodes_visited + 1
$   node_lc = f$edit(rjob_node,"LOWERCASE")
$   say ""
$   if (transport .eqs. "IP")
$     then
$     gosub do_ip
$   else
$   if (transport .eqs. "DECNET")
$     then
$     gosub do_decnet
$   else
$   say "%''facility'-W-NAY, not available yet via ''transport'"
$   endif !(transport .eqs. "DECNET")
$   endif !(transport .eqs. "IP")
$  goto node_loop
$end_node_loop:
$ say f$fao("!/!4*=(!6%D !8%T)!57*=",0,0)
$ say f$fao("  Visited !UL node!%S, and failed on !UL" -
		,nodes_visited,nodes_failed)
$ goto bail_out
$
$do_ip:
$ rsh 'rjob_node'/user_name='rjob_username'/password='password' sysid brief
$ counter = 0
$rshloop:
$   say "''dots'"
$   say f$fao("!AS$ !AS",node_lc,rjob_command'counter')
$   rsh_command = rjob_command'counter'
$   rsh 'rjob_node'/user_name='rjob_username'/password='password' 'rsh_command'
$   if (.not. $status) then nodes_failed = nodes_failed + 1
$   counter = counter + 1
$  if (counter .lt. cmd_count) then goto rshloop
$end_rshloop:
$ return
$
$do_decnet:
$ on warning then goto end_loop
$ define/nolog sys$error nla0:
$ define/nolog sys$output nla0:
$ open_error = 1
$ openn/read/write/error=end_loop -
	rjob_link 'rjob_node'"''rjob_username' ''password'"::"0=RJOB"
$ open_error = 0
$ deassign sys$output
$ deassign sys$error
$ set noon
$ counter = -1
$ write rjob_link "sysid brief"
$ goto int_read
$loop:
$   say "''dots'"
$   say f$fao("!AS$ !AS",node_lc,rjob_command'counter')
$   write rjob_link rjob_command'counter'
$int_read:
$     read/end_of_file=int_done/error=loop rjob_link line
$     if (f$locate("~#",line) .ne. f$length(line)) then goto int_done
$     if (line .eqs. "") then line = " "
$     saysym line
$ goto int_read
$int_done:
$   counter = counter + 1
$  if (counter .lt. cmd_count) then goto loop
$end_loop:
$ if (open_error)
$   then
$   error_status = $status
$   deassign sys$output
$   deassign sys$error
$   say f$message(error_status)
$   nodes_failed = nodes_failed + 1
$ endif
$ set noon
$ closee/nolog rjob_link
$ return
$
$bail_out:
$ set noon
$ closee/nolog rjob_link
$ if (f$mode() .eqs. "INTERACTIVE") then set terminal/echo
$ exitt 1+(0*'f$verify(__vfy_saved)')
$ !+==========================================================================
$ !
$ ! Procedure:	RCOMMAND.COM
$ !
$ ! Purpose:	Allows one-liner commands to be done across the network.
$ !		Uses RJOB at the remote end.
$ !
$ ! Parameters:
$ !	[P1]	Node name - defaults to *.
$ !	[P2]	Username - defaults to ON_LINE.
$ !
$ ! History:
$ !		16-Jul-1998, DBS; Version V1-001
$ !	001 -	Original version.  (Taken from RJOB.COM)
$ !		12-Aug-1998, DBS; Version V1-002
$ !	002 -	Added display of number of nodes visited on the way out.
$ !		24-Feb-1999, DBS; Version V1-003
$ !	003 -	Minor cosmetic stuff.
$ !
$ !		16-Sep-1999, DBS; Version V2-001
$ !	001 -	Major rehash and cleanup.
$ !-==========================================================================
