$ sd kronos_root:[misc]
$ gen_cal = "$ kronos_root:[misc]gen_calendar"
$ gen_cal 96
$ if $status .eq. 3
$    then
$    mail sys$input system
 
 
The GEN_CALENDAR job bombed.... please check it out. 

$ else
$    mail sys$input system


A new calendar file was created via the GEN_CALENDAR program... don't forget
to check it out for accuracy.

$endif
