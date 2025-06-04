$ if p1 .eqs. "L" then goto link
$ cc/list ld
$ message/list ldmsg
$ macro/list lddriver
$link:
$ link/notrace ld,ldmsg,sys$input/opt
sys$library:vaxcrtl.exe/share
ident="V5.1"
$ link/sym lddriver,sys$input/option
base=0
