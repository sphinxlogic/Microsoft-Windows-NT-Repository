$ !
$ !	"Makefile" for Alpha VMS version of unzip
$ !
$ set verify	! like "echo on", eh?
$ define/nolog decc$text_library sys$library:sys$lib_c.tlb
$ If P1 .EQS. ""
$ THEN
$ cc/stand=vaxc/opt unzip,file_io,VMS_attr,mapname,match,misc,unimplod,unreduce,unshrink
$ ENDIF
$ link unzip,file_io,VMS_attr,mapname,match,misc,unimplod,unreduce,unshrink
$ set noverify
