$ set noon
$ month = f$extract(3,3,f$time())
$ !
$ ! New versions of system files
$ set accounting/new_file
$ rename sys$errorlog:errlog.sys sys$errorlog:errlog.'month'
$ !
$ ! Cleanup old files
$ purge/keep=4 sys$manager:*.log
$ purge/keep=4 sys$manager:accountng.dat
$ purge/keep=2 sys$errorlog:errlog.'month'
$ DELETE/BEFORE=YESTERDAY SCRATCH:*.*;*
$ !
$ ! Pathworks
$!! PURGE/KEEP=4 MRL$DISK:[SYS0.PCSA]*.LOG
$!! SET DEFAULT NETWORK$DISK:[PCFS_SPOOL]
$!! del [.default]*.*;
$!! del [.gfax_hp]*.*;
$!! del [.gfax_ps]*.*;
$!! DEL/BEF=-35-/EXCL=(*.MSAF$CAT,MSAF$*.*) NETWORK$DISK:[PCSA.PCMAC]*.*;
