$ on error then goto exit
$ on control_y then goto exit
$ set on
$ zip -u socketshr_log [.netlib]*.log [.netlib]*.lis [.netlib]*.map
$ zip -u socketshr_log [.ucx]*.log [.ucx]*.lis [.ucx]*.map
$!
$ exit:
