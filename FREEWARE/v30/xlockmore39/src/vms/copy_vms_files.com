$!
$! Copy needed VMS files into main directory
$!
$ copy AMD.C [-] /log
$ copy AMD.H [-] /log
$ copy MAKE.COM [-] /log
$ copy PWD.H [-] /log
$ copy VMS_PLAY.C [-] /log
