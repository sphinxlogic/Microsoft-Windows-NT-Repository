$ define BATTALIONSCOREDIR SYS$LOGIN:
$ set def [.BATTALION_DATA]
$ mydisk = f$trnlmn("SYS$DISK")
$ mydir  = mydisk+f$directory()
$ mydir = f$extract(0,f$locate("]",mydir),mydir)
$ mydir = mydir+".]"
$ def/trans=conc BATTALION_DIR 'mydir
$ battaliondatadir :== "BATTALION_DIR:"
$ set def [-]
$ set def [.source]
$ mydisk = f$trnlmn("SYS$DISK")
$ mydir  = mydisk+f$directory()
$ battalion :== $'mydir'battalion
$ exit
