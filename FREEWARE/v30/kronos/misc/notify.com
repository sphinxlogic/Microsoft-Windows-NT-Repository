$ !
$ ! General-purpose notify users job
$ !   P1 = message (or @file) to send
$ !   P2 = primary list of users to notify
$ !   P3 = secondary list of users to notify
$ !
$ on severe_error then goto trouble
$ on error then goto trouble
$ !
$ notify := $kronos_root:[misc]notify
$ notify 'p1' 'p2' 'p3'
$ exit
$ !
$trouble:
$ istat = $status
$ mess = f$fao("NOTIFY failed : !AS","''f$message(istat)'")
$ request "''mess'"
$ exit
