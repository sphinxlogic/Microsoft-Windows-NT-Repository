$!
$!  do the build..should already have been configured
$!
$!  p1 = target      ... default is to build the s/w
$!
$ on error then goto cleanup
$ on control_y then goto cleanup
$ wo := write sys$output
$!
$ wo "---------------CRINOID Build---------------"
$ wo "...reading configuration"
$ open/read/err=noconfig fd CONFIGURE.MMS
$ loop:
$   read/end=eloop fd line
$   if f$extract(0,10,line) .eqs. "MAKE_UTIL=" then make_util = f$elem(1,"=",line)
$   if f$extract(0,10,line) .eqs. "PERLSETUP=" then perlsetup = f$elem(1,"=",line)
$ goto loop
$ eloop:
$ close fd
$!
$ wo "...setting up Perl"
$ @'perlsetup'
$ name = "s/w build"
$ if p1 .nes. "" then name = p1
$ wo "...doing ''name'"
$ make_util/macro=(configure.mms) 'p1'
$ wo "..''name' complete"
$ if p1 .EQS. "" then wo ""
$ if p1 .EQS. "" then wo "The next step is to @INSTALL"
$ goto cleanup
$ noconfig:
$   write sys$output "You must @CONFIGURE before @BUILD is run"
$ cleanup:
$   if f$trnlnm("FD") .nes. "" then close fd
$ exit
