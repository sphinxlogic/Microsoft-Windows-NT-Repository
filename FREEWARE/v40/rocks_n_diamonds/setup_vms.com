$ mydisk = f$trnlmn("SYS$DISK")
$ mydir  = mydisk+f$directory()
$ rocksn*diamonds :==$'mydir'ROCKSNDIAMONDS.EXE
$ mydir = f$extract(0,f$locate("]",mydir),mydir)
$ mydir = mydir+".]"
$ def/trans=conc  ROCKS_N_DIR 'mydir
$ exit
