$! File created by DCL_DIET at 19-JUL-1999 09:19:11.20 from
$! DBS0:[OLGA.COM]GENERIC_DAILY_CONTROL.COS;
$__vfy = "VFY_''f$parse(f$environment("procedure"),,,"name")'"
$if (f$type('__vfy') .eqs. "") then __vfy = 0
$__vfy_saved = f$verify(&__vfy)
$procedure = f$element(0,";",f$environment("PROCEDURE"))
$procedure_name = f$parse(procedure,,,"NAME")
$facility = procedure_name
$location = f$parse(procedure,,,"DEVICE","NO_CONCEAL") -
+ f$parse(procedure,,,"DIRECTORY","NO_CONCEAL") - "]["
$set noon
$on control_y then goto bail_out
$vax = (f$getsyi("HW_MODEL") .lt. 1024)
$axp = (f$getsyi("HW_MODEL") .ge. 1024)
$say = "write sys$output"
$generic_daily = procedure_name - "_CONTROL"
$submitt/queue=daily_batch/user=on_line/keep/noprint/after=tomorrow -
'procedure'/priority=255
$submitt/queue=daily_batch/user=on_line/keep/noprint -
'location''generic_daily'/priority=255
$bail_out:
$exitt 1+(0*'f$verify(__vfy_saved)')
