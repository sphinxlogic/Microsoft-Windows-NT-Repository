$ !
$ ! Check that no new images have been installed since last time.
$ ! Also check privileges.
$ !
$ set default kronos_root:[security]
$ on error then goto oops
$ assign/user check_install.temp sys$output
$ install list
$ !
$ run check_install
$ if $status .eq. 3 
$    then
$      mail/subject="Installed images." check_install.status system
$      delete check_install.status;*
$    endif
$ purge/keep=3 check_install.dat
$ del check_install.temp;*
$ exit
$ !
$oops:
$ mail 
send
system
Check_install
Problem with the CHECK_INSTALL job.
$ exit
