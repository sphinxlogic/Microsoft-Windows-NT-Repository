$ set noon
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
$ set def 'f$element(0,"]",workdir)xneko]'
$ if f$getsyi("cpu") .eq. 128
$    then
$       cpu = "AXP"
$    else
$       cpu = "VAX"
$ endif
$ xneko :== "$demos:[xneko]xneko_''cpu'"
$ define/nolog sys$error nl:
$ define/nolog sys$output nl:
$ if cpu .eqs. "AXP" then xneko -geometry 800x800+200 -title "XNeko      Decus 93"
$ if cpu .eqs. "VAX" then xneko -geometry 800x800+200 -title "XNeko      Decus 93"
$ deassign sys$error
$ deassign sys$output
$ set def 'olddir'
$ exit
