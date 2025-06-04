$ !
$ !  Check to see that major nodes are reachable on the network
$ !
$ set default kronos_root:[resources]
$ bad = 0
$ open/write file check_net.temp
$ write file "$ SEND SYSTEM"
$ write file " "
$ write file "Kronos net check job..."
$ !
$ node = "MICKEY"
$ gosub check_it
$ node = "GOOFY"
$ gosub check_it
$ node = "DONALD"
$ gosub check_it
$ node = "MINNIE"
$ gosub check_it
$ close file
$ if bad then @check_net.temp 
$ delete check_net.temp;*
$ exit
$ !
$check_it:
$ mc ncp sho node 'node'
$ if .not. $status 
$    then
$       bad = 1
$       write file "WARNING >>> Node ''node' is unreachable."
$    endif
$ return
