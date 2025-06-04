$! This example shows how to shadow a DECram disk with a logical disk
$ sysgen:=$sysgen
$ alloclass = f$getsyi("alloclass")
$ if .not. f$getdvi("lda0:","exists") then -
	sysgen connect lda0/noadapter
$ device = "$''alloclass'$MDA10:"
$ if .not. f$getdvi(device,"exists") then -
	sysgen connect mda10/noadapter
$ ld_alloclass = f$getdvi("lda0:","alloclass")
$ init/size=1024/nohigh/system 'device' test
$ tracks = f$getdvi(device,"tracks")
$ sectors = f$getdvi(device,"sectors")
$ cylinders = f$getdvi(device,"cylinders")
$ maxblock = f$getdvi(device,"maxblock")
$ ld create dpa1:[000000]ramdisk/size='maxblock'
$ ld connect/symbol dpa1:[000000]ramdisk/sectors='sectors'/tracks='tracks'/cylinders='cylinders'
$ init lda'ld_unit' dummy
$ mount dsa10:/shadow=($'alloclass'$mda10:,$'ld_alloclass'$lda'ld_unit':) test
$ show device dsa10
$ ld show lda'ld_unit'
