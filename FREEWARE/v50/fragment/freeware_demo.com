$ alpha = f$getsyi("HW_MODEL").ge.1024
$ if alpha
$ then fragment := $sys$disk:[]fragment.alpha_exe
$ else fragment := $sys$disk:[]fragment.exe
$ endif
$ write sys$output ""
$ fragment sys$sysdevice:
$ write sys$output ""
$ write sys$output ""
$ read/prompt="Press RETURN to continue...." sys$command tmp/error=_Exit
$_Exit:
$ exit
