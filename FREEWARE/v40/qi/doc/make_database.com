$ here = f$environment("default")
$ path1 = f$parse("[-]",here,,"device")+f$parse("[-]",here,,"directory")
$ make :== $'path1'qi_make
$ build :== $'path1'qi_build
$ soundex :== $'path1'qi_add_soundex
$ nickname :== $'path1'qi_add_nickname
$ alias :== $'path1'qi_add_alias
$
$ make em411.txt em411.dat
$ nickname em411.dat
$ soundex em411.dat
$ alias em411.dat database.dat
$ build database.dat /create/data/config=database.cnf
$ exit
