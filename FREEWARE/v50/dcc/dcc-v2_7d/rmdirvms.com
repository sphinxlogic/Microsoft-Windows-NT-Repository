$ ! RMDIRVMS.COM
$ if F$SEARCH(" ''p1.dir") .eqs ""
$ then write sys$error "repertoire inexistant"
$ else
$    set def [.'p1]
$    delete *.*;*
$    set def [-]
$    set file/prot=o:rwed 'p1.dir
$    delete 'p1.dir;1
$ endif
$ ! End RMDIRVMS.COM
