$ sav_verify = 'f$verify(0)
$ set = "set"
$ set symbol/scope=noglobal
$ delete /symbol/local set
$!
$ set noon
$!
$ if p1 .eqs. ""
$ then
$   write sys$output "%DIFSTRM-E-NOCLASS, Class in P1 not supplied"
$   GOTO ERROR_EXIT
$ else
$   base_class = P1
$ endif
$!
$ if p2 .eqs. ""
$ then
$   write sys$output "%DIFSTRM-E-NOCLASS, Class in P2 not supplied"
$   EXIT
$ else
$   target_class = P2
$ endif
$!
$ if p3 .eqs. ""
$ then
$   set default sys$login
$ else
$   set default 'p3'
$ endif
$!
$ if p4 .eqs. "" .or.  p4 .eqs. "LOG"
$ then
$   output_qual = ""
$ else
$   output_qual = "/OUTPUT=''p4'"
$ endif
$!
$ if p5 .eqs. ""
$ then
$   write sys$output - 
    "%DIFFSTRM-F-NOFACS, No facilities specified for compare"
$   GOTO Error_Exit
$ else
$   facility_spec = p5
$ endif
$!
$ set command vde$system:PLMENU$$FACDIFF
$!
$ facdiff/LOG'output_qual' 'facility_spec' 'base_class' 'target_class'
$!
$Main_Exit:
$ EXIT 1+0*'f$verify(sav_verify)
$!
$Error_Exit:
$ EXIT 0+0*'f$verify(sav_verify)
$!
$!+-----------------------------------------------------------------------------
$! DIFFSTRM.COM
$!
$!  Description:
$!
$!	Difference two streams (CMS classes) in the facilities matching the
$! specification in P5.
$!
$!+-----------------------------------------------------------------------------
$! P1 => stream to compare
$! P2 => stream to compare
$! P3 => work directory 
$! P4 => report file name
$! P5 => facility spec to process
$!
