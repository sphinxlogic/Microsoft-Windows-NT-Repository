$! set noverify
$ on control_y then goto final_exit
$ on error     then continue
$!***************************
$! Initialize
$!***************************
$ tcpip      = p1
$ make_type  = p2
$!
$ options    := ""
$ say        := write SYS$OUTPUT
$ install     = ""
$ facility    = "%IUFINGERD"
$!
$ is_alpha = (f$getsyi("arch_type") .gt. 1)
$ if (is_alpha)
$ then
$   arch = "ALPHA"
$   create share.opt
!this space intentionally left blank
$   cc_options == "/STANDARD=VAXC/PREFIX=ANSI"
$ else
$   arch = "VAX"
$   define/nolog LNK$LIBRARY SYS$LIBRARY:VAXCRTL.OLB
$   create share.opt
sys$share:vaxcrtl/share
$   cc_options = ""
$ endif
$!
$!***************************
$! Main Program
$!***************************
$ gosub check_parameters
$ gosub prepare_'tcpip'
$ gosub compile_'make_type'
$!
$ say facility,"-I-SUCCESS, IUFINGERD successfully made"
$ if (tcpip .nes. "UCX")
$ then
$   say ""
$   say "IMPORTANT: You may need to remove the current FINGER server from your"
$   say "           network configuration, before you can start IUFINGERD."
$   say ""
$ endif
$ goto  final_exit
$!
$!***************************
$! Check Parameters
$!***************************
$ check_parameters:
$ if (tcpip .eqs. "")
$ then
$   if (f$trnlnm("UCX$DEVICE") .nes. "") then tcpip = "UCX"
$   if (f$trnlnm("MULTINET")   .nes. "") then tcpip = "MULTINET"
$   if (f$trnlnm("TCPIP_ROOT") .nes. "") then tcpip = "TCPWARE"
$   if (f$trnlnm("TWG$TCP")    .nes. "") then tcpip = "WINS"
$ endif
$!
$ if ((tcpip .eqs. "WINS")     .or. -
      (tcpip .eqs. "UCX")      .or. -
      (tcpip .eqs. "MULTINET") .or. -
      (tcpip .eqs. "TCPWARE"))
$   then
$     say facility,"-I-TCPTYPE, making IUFINGERD for ",tcpip, " TCP/IP"
$   else
$     say facility,"-F-INVTCP, '",tcpip,"' TCP/IP is not supported"
$     exit
$ endif
$!
$ if (make_type .eqs. "")
$ then
$   on error then continue
$   define sys$output nl:
$   define sys$error nl:
$   mms
$   ss_status = $status
$   deassign sys$output 
$   deassign sys$error
$   on error then continue
$   if (ss_status .eq. %X00038090)
$   then
$     make_type = "NOMMS"
$   else
$     make_type = "MMS"
$   endif
$ endif
$!
$ if ((make_type .eqs. "MMS") .or. (make_type .eqs. "NOMMS"))
$ then
$   say facility,"-I-MAKETYPE, making IUFINGERD with ",make_type
$ else
$   say facility,"-F-INVMAKE, make type must be MMS or NOMMS"
$   exit
$ endif
$!
$ return
$!
$!***************************
$! Compile MMS
$!***************************
$ compile_mms:
$ mms/description=iufingerd.mms/macro="''tcpip'=TRUE"
$ return
$!
$!***************************
$! Compile NOMMS
$!***************************
$ compile_nomms:
$ compile := cc/list/opt/define=('tcpip','arch')'cc_options'
$ set verify
$ 'compile' args.c
$ 'compile' cache.c
$ 'compile' fuip.c
$ 'compile' hash.c
$ 'compile' jpi.c
$ 'compile' mail.c
$ 'compile' main.c
$ 'compile' net.c
$ 'compile' plan.c
$ 'compile' qio.c
$ 'compile' rfc931.c
$ 'compile' uai.c
$ 'compile' util.c
$ if (tcpip .eqs. "UCX")
$ then
$   'compile' ucx_ioctl.c
$   link/executable=iufingerd_'arch'_'tcpip'.exe  -
        args.obj, -
        cache.obj, -
        fuip.obj, -
        hash.obj, -
        jpi.obj, -
        mail.obj, -
        main.obj, -
        net.obj, -
        plan.obj, -
        qio.obj, -
        rfc931.obj, -
        uai.obj, -
        util.obj, -
        ucx_ioctl.obj, -
        'tcpip'.opt/opt, share.opt/opt
$ else
$   link/executable=iufingerd_'arch'_'tcpip'.exe  -
        args.obj, -
        cache.obj, -
        fuip.obj, -
        hash.obj, -
        jpi.obj, -
        mail.obj, -
        main.obj, -
        net.obj, -
        plan.obj, -
        qio.obj, -
        rfc931.obj, -
        uai.obj, -
        util.obj, -
        'tcpip'.opt/opt, share.opt/opt
$ endif
$ set noverify
$ return
$!
$!***************************
$! Prepare UCX
$!***************************
$ prepare_ucx:
$ define/nolog vaxc$include sys$common:[syslib]
$ define/nolog sys          sys$common:[syslib]
$ define/nolog netinet      sys$common:[syslib]
$ define/nolog arpa         sys$common:[syslib]
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
$! Final Exit
$!***************************
$ final_exit:
$ if (f$search("share.opt") .nes. "") then delete share.opt;*
$ exit
