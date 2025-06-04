$ ! Procedure:	LOCATE_PORT.COM
$ __vfy = "VFY_''f$parse(f$environment("PROCEDURE"),,,"NAME")'"
$ if (f$type('__vfy') .eqs. "") then __vfy = 0
$ __vfy_saved = f$verify(&__vfy)
$ set noon
$ on control_y then goto bail_out
$ say = "write sys$output"
$ server_name = f$edit(P1,"COLLAPSE,UPCASE")
$ server_name_len = f$length(server_name)
$ device = "*LTA*"
$loop:
$ found = f$device(device,,,42534)
$ if (found .eqs. "") then goto end_loop
$ pid = f$getdvi(found,"PID")
$ if (pid .nes. "")
$   then
$   owner = " username is ''f$edit(f$getjpi(pid,"USERNAME"),"COLLAPSE")'"
$   prcnam = " (''f$edit(f$getjpi(pid,"PRCNAM"),"TRIM")')"
$ else
$ owner = ""
$ prcnam = ""
$ endif !(pid .nes. "")
$ tt_accpornam = f$getdvi(found,"TT_ACCPORNAM")
$ if (f$extract(0,server_name_len,tt_accpornam) .eqs. server_name)
$   then
$   if (tt_accpornam .nes. "")
$     then
$     where = " is on ''tt_accpornam'"
$   else
$   where = ""
$   endif !(tt_accpornam .eqs. "")
$   say "''found'''where'''owner'''prcnam'"
$ endif !(f$extract(0,server_name_len,tt_accpornam) .eqs. server_name)
$ goto loop
$end_loop:
$bail_out:
$ !'f$verify(__vfy_saved)'
$ exitt 1
$ !+==========================================================================
$ !
$ ! Procedure:	LOCATE_PORT.COM
$ !
$ ! Purpose:	To locate an LTA device given the server name and port number.
$ !
$ ! Parameters:
$ !
$ ! History:
$ !		29-Aug-1995, DBS; Version V1-001
$ !	001 -	Original version.
$ !-==========================================================================
