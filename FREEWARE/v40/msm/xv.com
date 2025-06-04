$! Command file to start local processes in x11 environment
$ on error then continue
$ process = "Xv"
$
$ delete/symbol/global procsymb
$ delete/symbol/local procsymb
$ vue$suppress_output_popup
$ procsymb = "$ decw$tools:''process'.exe -mono"
$ procsymb 
$! spawn/nolog/input=nl:/nowait/process="''process'" procsymb -mono
$! vue$popdown
$ exit
