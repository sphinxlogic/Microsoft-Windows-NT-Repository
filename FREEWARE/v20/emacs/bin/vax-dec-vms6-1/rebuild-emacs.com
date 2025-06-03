$ ! VMS command file to run `temacs.exe' and dump the data file `temacs.dump'.
$ define EMACS_LIBRARY/PROCESS -
$11$DUA7:[PRAETORIUS.GNU.EMACS-19_22.]/TRANS=(CONCEAL)
$ objdir := EMACS_LIBRARY:[000000.VMS]
$ proc = f$environment("PROCEDURE")
$ proc_dir = f$parse(proc,,,"NODE")+f$parse(proc,,,"DEVICE")+f$parse(proc,,,"DIRECTORY")
$ save_default = f$environment("DEFAULT")
$ set def 'objdir'
$ temacs :== $EMACS_LIBRARY:[000000.BIN.VAX-DEC-VMS6-1]emacs -batch
$ temacs -l loadup.el dump nodoc
$ rename EMACS_LIBRARY:[000000.BIN.VAX-DEC-VMS6-1]emacs.dump emacs-19_22.dump
$ purge/keep=2 EMACS_LIBRARY:[000000.BIN.VAX-DEC-VMS6-1]emacs-19_22.dump
$ if f$search("EMACS_LIBRARY:[000000.BIN.VAX-DEC-VMS6-1]emacs.dump") .nes. "" then -
	set file/remove EMACS_LIBRARY:[000000.BIN.VAX-DEC-VMS6-1]emacs.dump;*
$ set file/enter=EMACS_LIBRARY:[000000.BIN.VAX-DEC-VMS6-1]emacs.dump EMACS_LIBRARY:[000000.BIN.VAX-DEC-VMS6-1]emacs-19_22.dump
$ set default 'save_default'
$ exit
