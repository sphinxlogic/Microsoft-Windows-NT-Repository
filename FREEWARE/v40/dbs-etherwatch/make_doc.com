$ ! Procedure:	MAKE_DOC.COM
$ __vfy = "VFY_''f$parse(f$environment("procedure"),,,"name")'"
$ if (f$type('__vfy') .eqs. "") then __vfy = 0
$ __vfy_saved = f$verify(&__vfy)
$ set noon
$ say = "write sys$output"
$ filespec = f$edit(P1,"collapse,upcase")
$ if (filespec .nes. "")
$   then
$   call do_doc 'filespec'
$ else
$ wildspec = "*.title_page"
$loop:
$   filespec = f$search(wildspec,77263)
$   if (filespec .eqs. "") then goto end_loop
$   filespec = f$parse(filespec,,,"name")
$   call do_doc 'filespec'
$ goto loop
$end_loop:
$ endif !(filespec .nes. "")
$bail_out:
$ !'f$verify(__vfy_saved)'
$ exitt 1
$
$do_doc: subroutine
$ set noon
$ on control_y then goto exit_do_doc
$ filename = f$edit(P1,"collapse,upcase")
$ if (filename .eqs. "") then goto no_filename
$ say "%Processing ''filename'..."
$ copy nl: 'filename'.RNT
$!$ copy nl: 'filename'.RNX
$ runoff/intermediate/nooutput 'filename'
$ runoff/contents 'filename'
$!$ runoff/index 'filename'
$ runoff/form_size=66/right=5/down=2 'filename'
$ delete 'filename'.BRN;*
$ delete 'filename'.RNT;*
$!$ delete 'filename'.RNX;*
$ purgee/nolog 'filename'.*
$ goto exit_do_doc
$no_filename:
$ say "%MAKE_DOC-I-NOFILE, no filename supplied"
$exit_do_doc:
$ exitt 1
$ endsubroutine
$ !+==========================================================================
$ !
$ ! Procedure:	MAKE_DOC.COM
$ !
$ ! Purpose:	
$ !
$ ! Parameters:
$ !
$ ! History:
$ !		28-Sep-1994, DBS; Version V1-001
$ !	001 -	Original version.
$ !-==========================================================================
