$ set noon
$ set default kronos_root:[security]
$ ! 
$ ! Note !!!!   Open files (installed) aren't processed.
$ run checksum
kronos_dir:*.exe
kronos_dir:kronos.dat
kronos_root:[security]*.exe
kronos_root:[security]*.com
kronos_root:[resources]*.exe
kronos_root:[resources]*.com
sys$library:*.exe
sys$library:*.olb
sys$manager:*.com
sys$system:*.exe
msys:*.com
msys:*.exe
$ !
$ ! Compare with last week's
$ run compare_sums
$ !
$ ! If no differences found, exit
$ if $status .eq. 3 then goto out
$ mail/subject="Weekly checksum of system files" checksum.dat ragosta,irene
$ delete checksum.dat;
$out:
$ purge/keep=2 checksum.out
$ exit
