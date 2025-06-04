$ if f$search("sys$system:decc$compiler.exe") .nes. ""
$   then
$     cc := cc/prefix=all
$     cc [-]server
$     link server,sys$input/opt
socketshr/share
$   else
$     cc [-]server
$     link server,sys$input/opt
socketshr/share
sys$share:vaxcrtl/share
$ endif
