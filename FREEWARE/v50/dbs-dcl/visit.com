$ ! Procedure:	VISIT.COM
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
$ saved_prcnam = f$getjpi("","PRCNAM")
$ say = "write sys$output"
$ vax = (f$getsyi("HW_MODEL") .lt. 1024)
$ axp = (f$getsyi("HW_MODEL") .ge. 1024)
$ commandfile = f$getjpi("","PID") + "-SET_HOST.COM"
$ nodelist = f$edit(P1,"COLLAPSE,UPCASE")
$ username = f$edit(P2,"COLLAPSE,UPCASE")
$ default_username = f$edit(f$getjpi("","USERNAME"),"COLLAPSE,UPCASE")
$ if (nodelist .eqs. "") then nodelist = "*"
$ if (f$mode() .nes. "INTERACTIVE")
$   then
$   say "%''facility'-E-NONINTER, this can only be done interactively..."
$ else
$ if (username .eqs. "") then username = default_username
$ as_user = " as user ''username'"
$ @dbslibrary:sys_find_nodes visit_node_list "''nodelist'"
$ default_transport = f$trnlnm("RSTUFF_DEFAULT_TRANSPORT")
$ default_transport = f$edit(default_transport,"COLLAPSE,UPCASE")
$ if (default_transport .eqs. "") then default_transport = "DECNET"
$ count = 0
$h_loop:
$   nodename = f$element(count,",",visit_node_list)
$   count = count + 1
$   if (nodename .eqs. "") then goto h_loop
$   if (nodename .eqs. ",") then goto end_h_loop
$   say ""
$   say "%''facility'-I-NEXTSTOP, heading for ''nodename'''as_user' . . ."
$   say ""
$   transport = default_transport
$   __transport = "NODE_''nodename'_TRANSPORT" - "." - "." - "."
$   if (f$type('__transport') .nes. "") then transport = &__transport
$   definee/nolog sys$output nla0:
$   definee/nolog sys$error nla0:
$   set process/name="off to ''nodename'"
$   deassign sys$error
$   deassign sys$output
$   if (transport .eqs. "IP")
$     then
$     definee/user/nolog sys$input sys$command
$     telnet 'nodename'
$   else
$   if (transport .eqs. "DECNET")
$     then
$     open/write/error=end_visit cf 'commandfile'
$     write cf "$set noon"
$     write cf "$deletee/nolog ''commandfile';*"
$     write cf "$set host ''nodename'"
$     write cf "''username'"
$     write cf "$exitt 1"
$     close/nolog cf
$     @'commandfile'
$end_visit:
$   else
$   say "%''facility'-W-HOW, visit ''nodename' via ''transport'?"
$   endif !(transport .eqs. "DECNET")
$   endif !(transport .eqs. "IP")
$   say ""
$   say "%''facility'-I-BACKHOME, returned from ''nodename' . . ."
$   say ""
$ goto h_loop
$end_h_loop:
$ endif !(f$mode() .nes. "INTERACTIVE")
$bail_out:
$ close/nolog cf
$ if (f$search(commandfile) .nes. "") then deletee/erase/nolog 'commandfile';*
$ if (saved_prcnam .nes. "") then set process/name="''saved_prcnam'"
$ exitt 1.or.f$verify(__vfy_saved)
$ !+==========================================================================
$ !
$ ! Procedure:	VISIT.COM
$ !
$ ! Purpose:	Does a SET HOST (or other appropriate command) to the nodes
$ !		listed.
$ !
$ ! Parameters:
$ !	 P1	A comma separated list of node names, defaults to *.
$ !	[P2]	The username to use - default to ON_LINE
$ !
$ ! History:
$ !		28-Mar-1996, DBS; Version V1-001
$ !	001 -	Original version.
$ !		14-Sep-1999, DBS; Version V1-002
$ !	002 -	Now check the "transport" symbols for each node.
$ !		16-Sep-1999, DBS; Version V1-003
$ !	003 -	Minor cleanup.  Added RSTUFF_DEFAULT_TRANSPORT stuff.
$ !-==========================================================================
