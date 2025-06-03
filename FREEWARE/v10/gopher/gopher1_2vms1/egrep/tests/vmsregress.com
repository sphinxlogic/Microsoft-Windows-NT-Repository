$!
$!  VMS DCL command procedure to check out GNU egrep
$!
$!	Hunter Goatley, 12-SEP-1991
$!
$ if f$type(egrep) .eqs. ""
$ then	write sys$output "egrep foreign command is not defined"
$	exit
$ endif
$ failures=0
$!
$! The Khadafy test is brought to you by Scott Anderson . . .
$!
$ write sys$output "Executing Anderson tests...."
$ egrep -f khadafy.regexp khadafy.lines >khadafy.out
$!
$!  Now use DIFF to check the files
$!
$ define/user sys$output nl:
$ define/user sys$error nl:
$ diff khadafy.lines khadafy.out
$ if $status.eq.%X006C8009
$ then	write sys$output "egrep passed the khadafy test"
$	delete/nolog khadafy.out;
$ else	write sys$output "egrep failed the khadafy test"
$	write sys$output "Output left in khadafy.out"
$	failures = 1
$ endif
$!
$!# . . . and the following by Henry Spencer.
$!
$ if f$type(gawk) .eqs. ""
$ then	write sys$output "Gawk foreign command not defined"
$	write sys$output "Skipping Spencer tests"
$	exit
$ else	write sys$output "Executing Spencer tests...."
$	gawk "-F:" -f vmsscriptgen.awk spencer.tests > temp.com
$	if f$search("temp.com").nes."" then @temp.com
$	delete/nolog temp.com;
$ endif
$ exit
