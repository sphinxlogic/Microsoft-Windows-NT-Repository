$!  example of search list logical names.
$!
$!  defines "searchlist" to be the local root (if p1 = "LOCAL") followed
$!  by the system common root, or, to be the specified root (if p1 = digit)
$!  followed by the common root.
$!
$!
$   root_format = "SYS$SYSDEVICE:[!AS.SYSEXE]"
$   root_common = "SYS$SYSDEVICE:[VMS$COMMON.SYSEXE]"
$   If F$Locate("LOCAL",p1) .ne. F$Length(p1)
$   then
$	root_local = F$FAO(root_format,f$trnlnm("SYS$TOPSYS"))
$	define/nolog/job searchlist 'root_local','root_common'
$   else
$	root_other = F$FAO(root_format,"SYS" + p1 )
$	define/nolog/job searchlist 'root_other','root_common'
$   endif
