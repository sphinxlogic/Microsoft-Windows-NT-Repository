$ set def [.lib]
$ mydisk = f$trnlmn("SYS$DISK")
$ mydir  = mydisk+f$directory()
$ define XATAXX_DIR 'mydir
$ set def [-]
$ exit
