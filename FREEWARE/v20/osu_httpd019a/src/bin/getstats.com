$!  This script returns a summary getstats out.
$!
$! Construct serversite variable from info provided by server.
$ write net_link "<DNETID>"
$ read net_link id
$ host = f$element(1," ", id)
$ port = ":" + f$element(2," ",id)
$ if port .eqs. ":80" .or. port .eqs. ": " then port = ""
$ getstats_serversite = "http://" + host + port + "/"
$!
$! Invoke command on access.log file.
$ write net_link "<DNETTEXT>"
$ write net_link "200 sending document"
$ getstats = "$"+f$parse("getstats.exe;",f$environ("PROCEDURE"))
$ define/user sys$output net_link:
$ getstats "-M" -l www_root:[000000]access.log -f
$ write net_link "</DNETTEXT>"
$!
$ done:
$ exit
