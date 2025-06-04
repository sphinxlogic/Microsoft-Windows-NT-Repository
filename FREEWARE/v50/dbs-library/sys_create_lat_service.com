$ ! Procedure:	SYS_CREATE_LAT_SERVICE.COM
$ __vfy = "VFY_''f$parse(f$environment("procedure"),,,"name")'"
$ if (f$type('__vfy') .eqs. "") then __vfy_saved = f$verify(0)
$ if (f$type('__vfy') .nes. "") then __vfy_saved = f$verify(&__vfy)
$	set noon
$	say = "write sys$output"
$	latcp = "$sys$system:latcp"
$	P1 = f$edit(P1, "upcase, collapse")
$	if (P1 .eqs. "")
$		then
$		say "%CREATELAT-E-NONAME, service name is missing"
$	else
$	service_symbol = "LAT_SERVICE_''P1'"
$	if (f$trnlnm(service_symbol, "LNM$SYSTEM_TABLE") .eqs. "")
$		then
$		service_name = "''P1'"
$		if (P2 .eqs. "")
$			then
$			if (f$type(&service_symbol) .eqs. "")
$				then
$				service_desc = "''P1' LAT service"
$			else
$			service_desc = &service_symbol
$			endif !(f$type(&service_symbol) .eqs. "")
$		else
$		service_desc = "''P2'"
$		endif !(P2 .eqs. "")
$		if (f$edit(service_desc, "collapse") .eqs. "") then -
$			service_desc = "Unknown LAT service"
$		definee/nolog/system 'service_symbol' "''service_desc'"
$		latcp create service 'service_name' -
					/ident="''service_desc'"/nolog
$		latcp set service 'service_name' -
					/ident="''service_desc'"/nolog
$	endif !(f$trnlnm(service_symbol) .eqs. "")
$	endif !(P1 .eqs. "")
$ !'f$verify(__vfy_saved)'
$ exit
$ !+==========================================================================
$ !
$ ! Procedure:	SYS_CREATE_LAT_SERVICE.COM
$ !
$ ! Purpose:	To create a lat service and define a system wide logical for
$ !		use by the check_service procedure.
$ !
$ ! Parameters:
$ !	 P1	The name of the service to create.
$ !	[P2]	The identification of the service, if not defined will default
$ !		to "''P1' LAT service"
$ !
$ ! History:
$ !		12-Sep-1989, DBS; Version V1-001
$ !  	001 -	Original version.
$ !		28-Nov-1989, DBS; Version V1-002
$ !	002 -	Modified to include a set service/ident in case the service
$ !		already exists and tidied up the handling of parameters.
$ !		28-Feb-1990, DBS; Version V1-003
$ !	003 -	Now have a closer look at the description before defining the
$ !		logicals.
$ !-==========================================================================
