$ olddir = f$environment("default")
$ workdir = f$trnlnm("demos")
$ if workdir .eqs. ""
$    then
$       write sys$output ""
$       write sys$output "Logical name [1mDEMOS[0m must be defined, eg: "
$       write sys$output ""
$       write sys$output "$ DEFINE/SYSTEM/EXECUTIVE DEMOS WORK:[SMITH.]"
$       write sys$output ""
$       write sys$output "[1mNOTE:[0m Be sure to include the ""."" following the last directory name" 
$       write sys$output ""
$       exit
$ endif
$ if f$getsyi("cpu") .eq. 128 then run demos:[xroach]xroach_axp.exe
$ if f$getsyi("cpu") .ne. 128 then run demos:[xroach]xroach_vax.exe
$ set def 'olddir'
$ exit
