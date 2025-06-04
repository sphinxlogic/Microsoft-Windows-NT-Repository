$ ! Procedure:	FIND_MESSAGE.COM
$ __vfy = "VFY_''f$parse(f$environment("procedure"),,,"name")'"
$ if (f$type('__vfy') .eqs. "") then __vfy = 0
$ __vfy_saved = f$verify(&__vfy)
$ set noon
$ on control_y then goto bail_out
$ saved_message = f$environment("MESSAGE")
$ set message/facility/severity/identification/text
$ say = "write sys$output"
$ code = f$edit(P1,"COLLAPSE,UPCASE")
$ if (code .eqs. "") then goto bail_out
$ display_format = "!/!5*- !AS"
$ say f$fao(display_format, "<System>")
$ say f$message('code')
$ wildspec = "sys$message:*.exe"
$loop:
$   filespec = f$search(wildspec,81423)
$   if (filespec .eqs. "") then goto end_loop
$   filespec = f$element(0,";",filespec)
$   filename = f$parse(filespec,,,"NAME")
$   say f$fao(display_format, filename)
$   set message 'filespec'
$   say f$message('code')
$ goto loop
$end_loop:
$bail_out:
$ set message'saved_message'
$ say ""
$ !'f$verify(__vfy_saved)'
$ exitt 1
$ !+==========================================================================
$ !
$ ! Procedure:	FIND_MESSAGE.COM
$ !
$ ! Purpose:	Scans the specified message files trying to translate a VMS
$ !		status value.
$ !
$ ! Parameters:
$ !	P1	The status value to check, either decimal or %X format.
$ !
$ ! History:
$ !		03-Nov-1993, DBS; Version V1-001
$ !	001 -	Original version.
$ !-==========================================================================
