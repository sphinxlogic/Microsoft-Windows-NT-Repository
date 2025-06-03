$   if f$getsyi("hw_model") .ge. 1024
$   then	    ! ALPHA
$	define/user sys$input sys$command
$	run msweeper.exe_alpha
$   else	    ! VAX
$	define/user sys$input sys$command
$	run msweeper.exe_vax
$   endif
