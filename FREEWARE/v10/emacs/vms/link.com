$!
$! VMS command file to link the .OBJ files of Emacs, producing `temacs.exe'.
$!
$ set default [-.src]
$ save_verif = f$verify(1)
$ link/exe=[-.vms]temacs.exe/map=[-.vms]temacs.map [-.vms]def_temacs.opt/opt
$! 'f$verify(save_verif)
$ set default [-.vms]
$ exit
