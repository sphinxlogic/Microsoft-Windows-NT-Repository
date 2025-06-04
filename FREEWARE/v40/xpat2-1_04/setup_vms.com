$ set def [.LIB]
$ mydisk = f$trnlmn("SYS$DISK")
$ mydir  = mydisk+f$directory()
$ mydir = f$extract(0,f$locate("]",mydir),mydir)
$ mydir = mydir+".]"
$ def/trans=conc XPAT_DIR 'mydir
$ def LANG FRENCH
$ set def [-]
$ exit
