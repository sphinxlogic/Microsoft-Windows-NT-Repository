$ ! FREWARE_DEMO.COM
$ write sys$output ">>> Setting up dcc environment"
$ arch := 'F$GETSYI("arch_name")  ! ':=' converts to upper-case
$ if arch .nes. "VAX" .and. arch .nes. "ALPHA"
$ then
$   write sys$error "Current machine has unknown architecture: ''arch'"
$   exit
$ endif
$ define dccsysincldir sys$common:[decc$lib.reference.decc$rtldef]
$ set def [.exec]
$ define dccdir 'F$ENVIRONMENT("DEFAULT")
$ set def [-]
$ if arch .eqs. "VAX"
$ then
$   copy/replace dccdir:starter.dccvmsvax dccdir:starter.dcc;1
$   copy/replace dccvax.exe dcc.exe;1
$ endif
$ if arch .eqs. "ALPHA"
$ then
$   copy/replace dccdir:starter.dccvmsalpha dccdir:starter.dcc;1
$   copy/replace dccalpha.exe dcc.exe;1
$ endif
$ dcc =="$ ''F$ENVIRONMENT("DEFAULT")dcc"
$ write sys$output ">>> Starting dcc test (in non-interactive mode)"
$ dcc -zcc -zsam +zmsg1000 trydcc
