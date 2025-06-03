$ ! VMS command file to run `temacs.exe' and dump the data file `temacs.dump'.
$ proc = f$environment("PROCEDURE")
$ proc_dir = f$parse(proc,,,"NODE")+f$parse(proc,,,"DEVICE")+f$parse(proc,,,"DIRECTORY")
$ save_default = f$environment("DEFAULT")
$ set def 'proc_dir'
$ if f$search("temacs.dump;") .nes. "" then delete temacs.dump;*
$ temacs :== $sys$disk:[]temacs -batch
$ temacs -l inc-vers
$ temacs -l loadup.el dump
$ set default 'save_default'
$ exit
