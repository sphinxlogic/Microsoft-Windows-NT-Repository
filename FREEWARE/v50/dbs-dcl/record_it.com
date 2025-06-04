$ ! Procedure:	RECORD_IT.COM
$ __vfy = "VFY_''f$parse(f$environment("procedure"),,,"name")'"
$ if (f$type('__vfy') .eqs. "") then __vfy = 0
$ __vfy_saved = f$verify(&__vfy)
$ set noon
$ if (f$edit(P1, "collapse, upcase") .eqs. "") then P1 = "<unspecified event>"
$ param_list = "''P1' ''P2' ''P3' ''P4' ''P5' ''P6' ''P7' ''P8'"
$ param_list = f$edit(param_list, "lowercase, trim, compress")
$ id = "[''f$getjpi("", "prcnam")']"
$ say = "write sys$output"
$ recording_file = f$parse("EVENTS", "SYS$LOGIN:.RECORD")
$ if (f$search(recording_file) .eqs. "") then copy/nolog nl: 'recording_file'
$ open/append/error=bail_out rf 'recording_file'
$ now = f$time()
$ today = f$extract(0, 3, f$cvtime("today",, "weekday"))
$ text = "''now' ''today' ''id' ''param_list'"
$ write rf "''text'"
$ say "%RECORD-I-NOTED, ''text'"
$bail_out:
$ if (f$trnlnm("rf") .nes. "") then close/nolog rf
$ !'f$verify(__vfy_saved)'
$ exitt
$ !+==========================================================================
$ !
$ ! Procedure:	RECORD_IT.COM
$ !
$ ! Purpose:	To record a time stamp and optional message in a recording
$ !		file located in sys$login.
$ !
$ ! Parameters:
$ !   [P1..P8]	Optional parameters that will be a message in the file.
$ !
$ ! History:
$ !		28-Sep-1989, DBS; Version V1-001
$ !  	001 -	Original version.
$ !-==========================================================================
