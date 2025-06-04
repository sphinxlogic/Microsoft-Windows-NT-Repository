$! --- LINK.COM ---
$ sv = 'f$verify(0)'
$ say := write sys$output
$ axp = f$getsyi("HW_MODEL").ge.1024
$ options = ""
$!$ DECC =  (f$trnlnm("DECC$CC_DEFAULT") .eqs. "/DECC") 	-
$!     .or. (f$search("SYS$LIBRARY:DECC$SHR.EXE") .nes. "") -
$!     .or. (f$TrnLnm("VAXCMSG") .eqs. "DECC$MSG")
$ decc = 0	!Shipped version was compiled with VAX C
$!
$ if AXP
$ then	olb = "ALPHA_OLB"
$	sysexe = "/SYSEXE"
$ else ! VAX
$	olb = "OLB"
$	sysexe = ",SYS$SYSTEM:SYS.STB/SELECTIVE_SEARCH"
$       If (.not. DECC) Then options = ",C.OPT/OPTIONS"
$ endif
$!
$ say "Linking VERB...."
$ link/notrace verb.'olb'/include=verb/library'options''sysexe'
$!
$ exit 1.or.f$verify(sv)
