$! update the CSO server with 411 data

$ say :== write sys$output
$ proc = f$environment("procedure")
$ path1 = f$parse("[-]",proc,,"device")+f$parse("[-]",proc,,"directory")
$ path2 = f$parse(proc,,,"device")+f$parse(proc,,,"directory")
$ make :== $'path1'qi_make
$ build :== $'path1'qi_build
$ soundex :== $'path1'qi_add_soundex
$ nick :== $'path1'qi_add_nickname
$ alias :== $'path1'qi_add_alias
$ define em411 foo:electronic-mail-411.idx
$ set default 'path2'
$
$ make em411 em411.dat
$ soundex em411.dat
$ nick em411.dat
$ delete database.dat;*
$ rename em411.dat database.dat
$ alias database.dat
$ append tanner.dat,control.dat database.dat
$
$! copy cso_data database.data
$! copy cso_index database.index
$ purge database.*
$ say "$ build database.dat /update/data/config=database.cnf"
$ build database.dat /update/data/config=database.cnf -
  /output='f$trnlnm("cso_data")'/start=811000000/end=812000000
$! @optimize database.index
$! @optimize database.data
$ exit
