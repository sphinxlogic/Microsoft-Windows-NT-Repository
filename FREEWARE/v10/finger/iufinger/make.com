$ on control_y then goto final_exit
$!
$!***************************
$! DEFINE SYMBOLS
$!***************************
$ say := write SYS$OUTPUT
$!
$ if (f$trnlnm("TWG$TCP")    .nes. "") then tcpip = "WINS"
$ if (f$trnlnm("UCX$DEVICE") .nes. "") then tcpip = "UCX"
$ if (f$trnlnm("MULTINET")   .nes. "") then tcpip = "MULTINET"
$ if (f$trnlnm("TCPIP_ROOT") .nes. "") then tcpip = "TCPWARE"
$!
$ if (p1 .nes. "") then tcpip = p1
$!
$!***************************
$! COMPILE AND LINK
$!***************************
$ gosub prepare_'tcpip'
$ say " Compiling for ''tcpip'..."
$ cc/list/opt/define=('tcpip') iufinger.c
$ say " Linking..."
$ link iufinger, 'tcpip'.opt/opt
$ exit
$!
$!***************************
$! Prepare UCX
$!***************************
$ prepare_ucx:
$ define/nolog vaxc$include sys$common:[syslib]
$ define/nolog sys          sys$common:[syslib]
$ define/nolog netinet      sys$common:[syslib]
$ define/nolog arpa         sys$common:[syslib]
$ define/nolog vms          sys$common:[syslib]
$ return
$!
$!***************************
$! Prepare WINS
$!***************************
$ prepare_wins:
$ define/nolog vaxc$include sys$common:[syslib], -
                            twg$tcp:[netdist.include]
$ define/nolog sys          twg$tcp:[netdist.include.sys] 
$ define/nolog netinet      twg$tcp:[netdist.include.netinet] 
$ define/nolog arpa         twg$tcp:[netdist.include.arpa] 
$ define/nolog vms          twg$tcp:[netdist.include.vms]
$ define/nolog lnk$library  sys$library:vaxcrtl.olb
$ return
$!
$!***************************
$! Prepare MULTINET
$!***************************
$ prepare_multinet:
$ define/nolog vaxc$include multinet_root:[multinet.include], - 
                            sys$common:[syslib]
$ define/nolog sys     multinet_root:[multinet.include.sys], -
                       sys$common:[syslib]
$ define/nolog arpa    multinet_root:[multinet.include.arpa]
$ define/nolog netinet multinet_root:[multinet.include.netinet]
$ define/nolog vms     multinet_root:[multinet.include.vms]
$ return
$!
$!***************************
$! Prepare TCPWARE
$!***************************
$ prepare_tcpware:
$ define/nolog vaxc$include tcpip_include, sys$common:[syslib]
$ define/nolog sys     tcpip_include, sys$common:[syslib]
$ define/nolog arpa    tcpip_include
$ define/nolog netinet tcpip_include
$ define/nolog vms     tcpip_include
$ return
$!
$!***************************
$! FINAL EXIT
$!***************************
$ final_exit:
$ exit
