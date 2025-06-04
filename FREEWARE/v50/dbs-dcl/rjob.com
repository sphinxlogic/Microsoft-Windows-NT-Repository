$ ! Procedure:	RJOB.COM
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
$ if (f$mode().eqs."NETWORK") then goto network
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
$
$interactive:
$ @dbslibrary:sys_find_nodes node_list "''rjob_node_list'"
$ default_transport = f$trnlnm("RSTUFF_DEFAULT_TRANSPORT")
$ default_transport = f$edit(default_transport,"COLLAPSE,UPCASE")
$ if (default_transport .eqs. "") then default_transport = "DECNET"
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
$   say ""
$   say "%''facility'-I-CONNECT, connecting to node ''rjob_node'"
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
$ say ""
$ goto bail_out
$
$do_ip:
$ rsh 'rjob_node'/user_name='rjob_username'/password='password' sysid brief
$rshloop:
$   ask rjob_command "''rjob_node'_rjob" "" SNZD
$   if (sys__control_z .or. sys__control_y) then goto end_rshloop
$   if (rjob_command .eqs. "") then goto rshloop
$   rsh 'rjob_node'/user_name='rjob_username'/password='password' 'rjob_command'
$  goto rshloop
$end_rshloop:
$ return
$
$do_decnet:
$ say ""
$ on warning then goto end_loop
$ define/nolog sys$error nla0:
$ define/nolog sys$output nla0:
$ open_error = 1
$ open/read/write/error=end_loop -
	rjob_link 'rjob_node'"''rjob_username' ''password'"::"0=RJOB"
$ open_error = 0
$ deassign sys$output
$ deassign sys$error
$ set noon
$ write rjob_link "sysid brief"
$ goto int_read
$loop:
$   ask rjob_command "''rjob_node'_rjob" "" SNZD
$   if (sys__control_z .or. sys__control_y) then goto end_loop
$   if (rjob_command .eqs. "") then goto loop
$   if (f$extract(0,f$length(rjob_command),"EXIT") .eqs. rjob_command)
$     then
$     ask confirmed "Do you really want to send ''rjob_command'" "No" B
$     if (.not. confirmed) then goto loop
$     quit = 1
$   endif
$   write rjob_link rjob_command
$   if (quit) then goto end_loop
$int_read:
$     read/end_of_file=int_done/error=loop rjob_link line
$     if (f$locate("~#",line) .ne. f$length(line)) then goto int_done
$     if (line .eqs. "") then line = " "
$     saysym line
$ goto int_read
$int_done:
$ goto loop
$end_loop:
$ if (open_error)
$   then
$   error_status = $status
$   deassign sys$output
$   deassign sys$error
$   say f$message(error_status)
$ endif
$ set noon
$ close/nolog rjob_link
$ return
$
$network:
$ set noon
$ scsnode = f$edit(f$getsyi("SCSNODE"),"COLLAPSE,UPCASE")
$ groupat = "@OLGA$COM:GROUP_AT"
$ groupdo = "@OLGA$COM:GROUP_DO"
$ groupsubmit = "@OLGA$COM:GROUP_SUBMIT"
$ open/read/write xx sys$net
$ define sys$output xx:
$net_read:
$   read/end_of_file=net_done/error=net_done xx command
$   if (command .eqs. f$extract(0,f$length(command),"EXIT"))
$     then
$     quit = 1
$     goto net_done
$   endif
$   'command'
$   say "~#"
$ goto net_read
$net_done:
$ close/nolog xx
$ deassign sys$output
$ if (quit) then stopp/id=0
$
$bail_out:
$ set noon
$ close/nolog rjob_link
$ if (f$mode() .eqs. "INTERACTIVE") then set terminal/echo
$ exitt 1.or.f$verify(__vfy_saved)
$ !+==========================================================================
$ !
$ ! Procedure:	RJOB.COM
$ !
$ ! Purpose:	Allows one-line commands (non-interactive) to be done across
$ !		the network.
$ !
$ ! Parameters:
$ !	[P1]	Node name - defaults to *.
$ !	[P2]	Username - defaults to ON_LINE.
$ !
$ ! History:
$ !		14-Dec-1993, DBS; Version V1-001
$ !	001 -	Original version.  (Not entirely bulletproof methinks.)
$ !		02-Jul-1998, DBS; Version V1-002
$ !	002 -	Added node list capability - like SEND.
$ !
$ !		16-Sep-1999, DBS; Version V2-001
$ !	001 -	Major rehash and cleanup.
$ !-==========================================================================
