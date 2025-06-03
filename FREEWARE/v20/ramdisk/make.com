$ macro initvd
$ macro ramdriver
$ link initvd
$ link/system=0/header ramdriver
