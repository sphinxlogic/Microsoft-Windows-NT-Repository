$!
$!  Command file to build RCARD
$!
$ axp = f$getsyi("HW_MODEL").ge.1024
$ cc := CC
$!$ cc := GCC		!Uncomment to use GNU C
$ if axp then cc := cc/standard=vaxc
$ write sys$output "Compiling RCARD...."
$ 'cc' rcard
$ write sys$output "Linking RCARD...."
$ if cc.eqs."CC"
$ then	link/notrace rcard,sys$input/options
	sys$share:vaxcrtl.exe/share
$ else	if .not.axp then link/notrace rcard,gnu_cc:[000000]options.opt/options
$	if axp then link/notrace rcard
$ endif
$ write sys$output "RCARD build completed"
$ exit
