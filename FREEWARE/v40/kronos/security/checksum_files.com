$ set noon
$ set default kronos_root:[security]
$ !
$ ! Check important executable/shareable/etc. files for tampering
$ !
$ define/usermode sys$output checksum.tmp     ! redirect output to a file
$ run checksum
kronos_dir:*.exe
kronos_dir:kronos.dat
kronos_root:[resources]*.exe
kronos_root:[security]*.exe
sys$library:*.exe
sys$library:*.mlb
sys$library:*.olb
sys$system:*.com
sys$system:*.exe
sys$system:*.stb
sys$system:rightslist.dat
$ rename checksum.tmp checkfiles.sum
$ run compare_sums
$ !
$ ! Notify system users if any changes were discovered
$ !
$  if $status .eq. 3 then goto out
$  mail/subject="Weekly checksum of system" CHECKSUM.DAT system,operator
$  delete checksum.dat;*
$out:
$  purge/keep=2 checkfiles.sum
$  exit
