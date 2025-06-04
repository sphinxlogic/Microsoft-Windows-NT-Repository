$ if f$search("sys$system:decc$compiler.exe") .nes. ""
$   then
$     cc := cc/prefix=all
$     cc [-]client
$     link client/map/full,sys$input/opt
socketshr/share
$   else
$     cc [-]client
$     link client/map/full,sys$input/opt
socketshr/share
sys$share:vaxcrtl/share
$ endif
