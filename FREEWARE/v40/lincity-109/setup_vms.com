$ mydisk = f$trnlmn("SYS$DISK")
$ mydir  = mydisk+f$directory()
$ mydir = f$extract(0,f$locate("]",mydir),mydir)
$ mydir = mydir+".]"
$ def/trans=conc LINCITY_DIR 'mydir
$ mydir = f$trnlnm("SYS$LOGIN")
$ mydir = f$extract(0,f$locate("]",mydir),mydir)
$ def LINCITY_SAVE_DIR 'mydir.LINCITY]
$ def LINCITY_SAVE_OLD 'mydir.LINCITY_OLD]
$ exit
