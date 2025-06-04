$ ! Procedure:	SYS_DELETE_LAT_SERVICE.COM
$ __vfy = "VFY_''f$parse(f$environment("procedure"),,,"name")'"
$ if (f$type('__vfy') .eqs. "") then __vfy_saved = f$verify(0)
$ if (f$type('__vfy') .nes. "") then __vfy_saved = f$verify(&__vfy)
$	set noon
$	say = "write sys$output"
$	latcp = "$sys$system:latcp"
$	P1 = f$edit(P1, "upcase, collapse")
$	if (P1 .eqs. "")
$		then
$		say "%DELETELAT-E-NONAME, service name is missing"
$	else
$	service_symbol = "LAT_SERVICE_''P1'"
$	if (f$trnlnm(service_symbol, "LNM$SYSTEM_TABLE") .nes. "")
$		then
$		service_name   = "''P1'"
$		deassign/system 'service_symbol'
$		latcp delete service 'service_name'/nolog
$	endif !(f$trnlnm(service_symbol) .eqs. "")
$	endif !(P1 .eqs. "")
$ !'f$verify(__vfy_saved)'
$ exit
$ !+==========================================================================
$ !
$ ! Procedure:	SYS_DELETE_LAT_SERVICE.COM
$ !
$ ! Purpose:	To delete a lat service created by the sys_create_lat_service
$ !		procedure.
$ !
$ ! Parameters:
$ !	 P1	The name of the service to delete.
$ !
$ ! History:
$ !		12-Sep-1989, DBS; Version V1-001
$ !  	001 -	Original version.
$ !-==========================================================================
