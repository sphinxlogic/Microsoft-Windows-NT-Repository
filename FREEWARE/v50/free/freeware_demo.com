$ alpha = f$getsyi("HW_MODEL").ge.1024
$ if alpha
$ then free := $sys$disk:[]free.alpha_exe
$ else free := $sys$disk:[]free.exe
$ endif
$ write sys$output ""
$ free
$ write sys$output ""
$ write sys$output ""
$ read/prompt="Press RETURN to continue...." sys$command tmp/error=_Exit
$_Exit:
$ exit
