$!-------------------------------------------------------------------------
$!
$!            SCAN_PRINTERS Version 2.6
$!
$!          Check required by running SCNPRT_REQUIRED.COM
$!          Logical names definition by running SCNPRT_LOGICALS
$!          Start detached Scan Printers process by running SCNPRT_DETACHED
$!
$!-------------------------------------------------------------------------
$!
$ if f$trnlnm("scnprt_dir").eqs."" then define/nolog scnprt_dir sys$manager:
$ @scnprt_dir:scnprt_required 'p1' 'p2' 'p3' 'P4' 'p5' 'p6' 'p7' 'p8'
$ if .not. $status then exit
$ @scnprt_dir:scnprt_logicals.com 'p1' 'p2' 'p3' 'P4' 'p5' 'p6' 'p7' 'p8'
$!
$ if f$search("scnprt_dir:scan_printers.log").nes."" then -
        purge/nolog/keep=2 scnprt_dir:scan_printers.log
$ @scnprt_dir:scnprt_detached.com 'p1' 'p2' 'p3' 'P4' 'p5' 'p6' 'p7' 'p8'
$!
$ exit
