$ !
$ !	"Makefile" for VMS version of unzip
$ !
$ set verify	! like "echo on", eh?
$ define lnk$library sys$library:vaxcrtl.olb
$ If P1 .EQS. ""
$ THEN
$ cc/define=(VAX_COMPILE) unzip,file_io,VMS_attr,mapname,match,misc,unimplod,unreduce,unshrink
$ ENDIF
$ link unzip,file_io,VMS_attr,mapname,match,misc,unimplod,unreduce,unshrink,sys$input:/opt
sys$share:vaxcrtl.exe/shareable
$ set noverify
